//Contains the definitions of the functions used by the firmware.


#include <SPI.h>
#include <Wire.h>

#include <Arduino.h>
#include "Parameters.h"
#include "Controller.h"
#include "Utils.h"
#include "State.h"
#include "analogFastWrite.h"
#include "macros.h"

void setupPins() {
  pinMode(VREF_2, OUTPUT);
  pinMode(VREF_1, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(step_pin, INPUT);
  pinMode(dir_pin, INPUT);
  pinMode(chipSelectPin, OUTPUT); // CSn -- has to toggle high and low to signal chip to start data transfer


  attachInterrupt(step_pin, stepInterrupt, RISING);
  attachInterrupt(dir_pin, dirInterrupt, CHANGE);

  if (ena_pin != -1) {
    pinMode(ena_pin, INPUT_PULLUP);
    attachInterrupt(ena_pin, enaInterrupt, CHANGE);
  }


  REG_PORT_OUTSET0 = PORT_PA20;  // write IN_4 HIGH
  REG_PORT_OUTCLR0 = PORT_PA15;  // write IN_3 LOW
  REG_PORT_OUTSET0 = PORT_PA21;  // write IN_2 HIGH
  REG_PORT_OUTCLR0 = PORT_PA06;  // write IN_1 LOW

}

void setupSPI() {
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE1);             ///400000, MSBFIRST, SPI_MODE1);

  SPI.begin();    //AS5047D SPI uses mode=1 (CPOL=0, CPHA=1)
  delay(1000);
  SPI.beginTransaction(settingsA);

}


void stepInterrupt() {
  if (dir) {
    r = r + stepangle;
  }
  else {
    r = r - stepangle;
  }
}

void dirInterrupt() {
  if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
    dir = false;
  }
  else {
    dir = true;
  }
}


void enaInterrupt() {
  if (REG_PORT_IN0 & PORT_PA14) { // check if ena_pin is HIGH
    enabled = false;
  }
  else {
    enabled = true;
  }
}



void output(float theta, int effort) {
  static float angle;
  static float floatangle;

  angle = (10000 * theta * 0.87266 );

  floatangle = angle + 23562;
  //floatangle = angle + 7854;

  val1 = effort * lookup_sine((int)floatangle);

  analogFastWrite(VREF_2, abs(val1));

  if (val1 >= 0)  {
    REG_PORT_OUTSET0 = PORT_PA20;     //write IN_4 HIGH

    REG_PORT_OUTCLR0 = PORT_PA15;     //write IN_3 LOW

  }
  else  {
    REG_PORT_OUTCLR0 = PORT_PA20;     //write IN_4 LOW

    REG_PORT_OUTSET0 = PORT_PA15;     //write IN_3 HIGH

  }

  floatangle = angle + 7854;
  //floatangle = angle + 23562;

  val2 = effort * lookup_sine((int)floatangle);

  analogFastWrite(VREF_1, abs(val2));

  if (val2 >= 0)  {
    REG_PORT_OUTSET0 = PORT_PA21;     //write IN_2 HIGH

    REG_PORT_OUTCLR0 = PORT_PA06;     //write IN_1 LOW

  }
  else  {
    REG_PORT_OUTCLR0 = PORT_PA21;     //write IN_2 LOW

    REG_PORT_OUTSET0 = PORT_PA06;     //write IN_1 HIGH

  }
}

void calibration() {
  disableTCInterrupts();

  SerialUSB.println(" ");
  SerialUSB.println("---- Calibration Routine ----");
  SerialUSB.println(" ");

  int encoderReading = 0;
  int lastencoderReading = 0;

  int avg = 50;         //how many readings to average

  int iStart = 0;
  int jStart = 0;
  int stepNo = 0;

  int fullStepReadings[steps_per_revolution];
  int ticks = 0;

  float lookupAngle = 0.0;
  float percent = 0.0;


  for (int reading = 0; reading < avg; reading++) {
    lastencoderReading += readEncoder();
    delay(1);
  }
  lastencoderReading = lastencoderReading / avg;

  dir = true;
  oneStep();
  delay(100);

  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delay(1);
  }
  encoderReading = encoderReading / avg;


  if ((encoderReading - lastencoderReading) < 0)
  {
    SerialUSB.println("Wired backwards");
    return;
  }

  if ((encoderReading * 0.02197265625) < 180.0) {
    dir = false;
  }
  else {
    dir = true;
  }

  // find the zeropoint of the encoder and get close to it
  SerialUSB.println("Searching Zeropoint");
  while (abs(encoderReading * 0.02197265625)  > 3) {
    encoderReading = 0.0;

    quaterStep();

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(1);
    }
    encoderReading = encoderReading / avg;
  }


  // got close to zero position
  dir = true;

  jump_to_fullstepp(); // jump to the next fullstep

  step_target = 0.0;
  encoderReading = 0.0;

  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delay(1);
  }
  encoderReading = encoderReading / avg;

  SerialUSB.println();
  SerialUSB.println("got close to zero position");
  SerialUSB.print("position = ");
  SerialUSB.println(encoderReading * 0.02197265625);

  delay(2000);


  SerialUSB.println();
  SerialUSB.println("calibration single steps");
  // step to every single fullstep position and read the Encoder
  for (int x = 0; x < steps_per_revolution; x++) {

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(1);
    }
    encoderReading = encoderReading / avg;


    fullStepReadings[x] = encoderReading;

    oneStep();

    delay(100);

    percent = 100 * ((float)x) / (float)(steps_per_revolution);
    SerialUSB.print(percent);
    SerialUSB.println('%');
  }
  // end fullsteps


  // interpolate between the fullsteps
  for (int i = 0; i < steps_per_revolution; i++) {
    ticks = fullStepReadings[mod((i + 1), steps_per_revolution)] - fullStepReadings[mod((i), steps_per_revolution)];
    if (ticks < -15000) {
      ticks += counts_per_revolution;
    }
    else if (ticks > 15000) {
      ticks -= counts_per_revolution;
    }

    if (ticks > 1) {
      for (int j = 0; j < ticks; j++) {
        stepNo = (mod(fullStepReadings[i] + j, counts_per_revolution));
        if (stepNo == 0) {
          iStart = i;
          jStart = j;
        }
      }
    }

    if (ticks < 1) {
      for (int j = -ticks; j > 0; j--) {
        stepNo = (mod(fullStepReadings[steps_per_revolution - 1 - i] + j, counts_per_revolution));
        if (stepNo == 0) {
          iStart = i;
          jStart = j;
        }

      }
    }
  }




  SerialUSB.println(" ");
  SerialUSB.println("newLookup:");
  SerialUSB.println(" ");

  for (int i = iStart; i < (iStart + steps_per_revolution + 1); i++) {
    ticks = fullStepReadings[mod((i + 1), steps_per_revolution)] - fullStepReadings[mod((i), steps_per_revolution)];

    if (ticks < -15000) {
      ticks += counts_per_revolution;

    }
    else if (ticks > 15000) {
      ticks -= counts_per_revolution;
    }

    if (ticks > 1) {

      if (i == iStart) {
        for (int j = jStart; j < ticks; j++) {
          lookupAngle = 0.001 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle);
          SerialUSB.print(" , ");
        }
      }

      else if (i == (iStart + steps_per_revolution)) {
        for (int j = 0; j < jStart; j++) {
          lookupAngle = 0.001 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle);
          SerialUSB.print(" , ");
        }
      }
      else {
        for (int j = 0; j < ticks; j++) {
          lookupAngle = 0.001 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle);
          SerialUSB.print(" , ");
        }
      }



    }

    else if (ticks < 1) {
      if (i == iStart) {
        for (int j = - ticks; j > (jStart); j--) {
          lookupAngle = 0.001 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle);
          SerialUSB.print(" , ");
        }
      }
      else if (i == iStart + steps_per_revolution) {
        for (int j = jStart; j > 0; j--) {
          lookupAngle = 0.001 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle);
          SerialUSB.print(" , ");
        }
      }
      else {
        for (int j = - ticks; j > 0; j--) {
          lookupAngle = 0.001 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle);
          SerialUSB.print(" , ");
        }
      }

    }
  }
  SerialUSB.println();
  enableTCInterrupts();

}


void serialCheck() {
  if (SerialUSB.peek() != -1) {

    char inChar = (char)SerialUSB.read();

    switch (inChar) {
      case 'c':
        calibration();           //cal routine
        break;

      case 's':             //new setpoint
        setpoint();
        break;

      case 'p':
        parameterQuery();     // prints copy-able parameters
        break;

      case 'e':
        parameterEdit();
        break;

      case 'a':             //anticogging
        antiCoggingCal();
        break;

      case 'j':
        step_response();
        break;

      case 'm':
        Serial_menu();
        break;

      case 'f':
        get_max_frequency();
        break;

      case 'o':
        set_filter_frequency();
        break;

      default:
        break;
    }
  }

}
void Serial_menu() {
  SerialUSB.println("");
  SerialUSB.println("");
  SerialUSB.println("----- Mechaduino 0.1 -----");
  SerialUSB.print("Identifier: ");
  SerialUSB.println(identifier);
  SerialUSB.println("");
  SerialUSB.println("");
  SerialUSB.println("Main menu");
  SerialUSB.println("c  -  calibration");
  SerialUSB.println("s  -  setpoint");
  SerialUSB.println("p  -  print parameter");
  SerialUSB.println("e  -  edit parameter ");
  SerialUSB.println("a  -  anticogging");
  SerialUSB.println("j  -  step response");
  SerialUSB.println("m  -  print main menu");
  SerialUSB.println("f  -  get max loop frequency");
  SerialUSB.println("");
}

void setpoint() {
  SerialUSB.print("current Setpoint: ");
  SerialUSB.println(yw);

  SerialUSB.println("Enter setpoint:");

  while (SerialUSB.peek() == -1)  {}
  r = SerialUSB.parseFloat();

  SerialUSB.print("new Setpoint: ");
  SerialUSB.println(r);
}

void parameterQuery() {
  SerialUSB.println(' ');
  SerialUSB.println("----Current Parameters-----");
  SerialUSB.println(' ');

  SerialUSB.print("volatile float pKp = ");
  SerialUSB.print(pKp, 4);
  SerialUSB.println(';');

  SerialUSB.print("volatile float pKi = ");
  SerialUSB.print(pKi, 4);
  SerialUSB.println(';');

  SerialUSB.print("volatile float pKd = ");
  SerialUSB.print(pKd, 4);
  SerialUSB.println(';');

}


float lookup_angle(int n)
{
  float a_out;
  a_out = pgm_read_float_near(lookup + n);
  return a_out;
}

void jump_to_fullstepp() {
  // set coil 2 to zero
  analogFastWrite(VREF_2, 0);
  REG_PORT_OUTSET0 = PORT_PA20;     //write IN_4 HIGH
  REG_PORT_OUTCLR0 = PORT_PA15;     //write IN_3 LOW

  // step coil 1 to some value
  analogFastWrite(VREF_1, 64);
  REG_PORT_OUTSET0 = PORT_PA21;     //write IN_2 HIGH
  REG_PORT_OUTCLR0 = PORT_PA06;     //write IN_1 LOW
}

void quaterStep() {           /////////////////////////////////   oneStep    ///////////////////////////////

  if (dir == 0) {
    step_target += 0.9;
  }
  else {
    step_target -= 0.9;
  }

  output(step_target, 64); //1.8 = 90/50

}

void oneStep() {

  if (dir == 0) {
    step_target += 1.8;
  }
  else {
    step_target -= 1.8;
  }

  output(step_target, 64); //1.8 = 90/50

}

int readEncoder()           //////////////////////////////////////////////////////   READENCODER   ////////////////////////////
{
  long angleTemp;

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW

  byte b1 = SPI.transfer(0xFF);
  byte b2 = SPI.transfer(0xFF);

  angleTemp = (((b1 << 8) | b2) & 0B0011111111111111);

  REG_PORT_OUTSET1 = PORT_PB09;  // write chipSelectPin HIGH

  return angleTemp;
}


void receiveEvent(int howMany)
{
  while (1 < Wire.available()) // loop through all but the last
  {
    char c = Wire.read(); // receive byte as a character
    SerialUSB.print(c);         // print the character
  }
  int x = Wire.read();    // receive byte as an integer
  SerialUSB.println(x);         // print the integer
  r = 0.1 * ((float)x);
}

int mod(int xMod, int mMod) {
  return (xMod % mMod + mMod) % mMod;
}


float lookup_sine(int m)        /////////////////////////////////////////////////  LOOKUP_SINE   /////////////////////////////
{
  float b_out;

  m = (0.01 * (((m % 62832) + 62832) % 62832)) + 0.5; //+0.5 for rounding

  //SerialUSB.println(m);

  if (m > 314) {
    m = m - 314;
    b_out = -pgm_read_float_near(sine_lookup + m);

  }
  else
  {
    b_out = pgm_read_float_near(sine_lookup + m);
  }

  return b_out;
}


void setupTCInterrupts() {
  const int overflow = (48000000 / Fs) - 1;

  // Enable GCLK for TC4 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY);

  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;   // Set Timer counter Mode to 16 bits
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal Normal Frq
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;   // Set perscaler
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.CC[0].reg = overflow;
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC5->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC5->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0


  NVIC_SetPriority(TC5_IRQn, 1);


  // Enable InterruptVector
  NVIC_EnableIRQ(TC5_IRQn);
}


void enableTCInterrupts() {
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
}

void disableTCInterrupts() {
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync
}


void antiCoggingCal() {
  SerialUSB.println(" -----------------BEGIN ANTICOGGING CALIBRATION!----------------");
  r = lookup_angle(1);
  enableTCInterrupts();
  delay(1000);


  for (int i = 1; i < 657; i++) {
    r = lookup_angle(i);
    SerialUSB.print(r, 4);
    SerialUSB.print(" , ");
    delay(100);
    SerialUSB.println(u, 4);
  }
  SerialUSB.println(" -----------------REVERSE!----------------");

  for (int i = 656; i > 0; i--) {
    r = lookup_angle(i);
    SerialUSB.print(r, 4);
    SerialUSB.print(" , ");
    delay(100);
    SerialUSB.println(u, 4);
  }
  SerialUSB.println(" -----------------DONE!----------------");
  disableTCInterrupts();
}


void parameterEdit() {
  int received_1 = 0;
  int received_2 = 0;

  SerialUSB.println();
  SerialUSB.println("---- Edit position loop gains: ----");
  SerialUSB.println();
  SerialUSB.print("p ----- pKp = ");
  SerialUSB.println(pKp, 4);

  SerialUSB.print("i ----- pKi = ");
  SerialUSB.println(pKi, 4);

  SerialUSB.print("d ----- pKd = ");
  SerialUSB.println(pKd, 4);

  SerialUSB.println("q ----- quit");
  SerialUSB.println();


  while (received_1 == 0)  {
    delay(100);
    char inChar2 = (char)SerialUSB.read();

    switch (inChar2) {
      case 'p':
        {
          SerialUSB.println("enter new pKp!");
          while (received_2 == 0) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              pKp = SerialUSB.parseFloat();
              received_2 = 1;
            }
          }
          received_1 = 1;
        }
        break;
      case 'i':
        {
          SerialUSB.println("enter new pKi!");
          while (received_2 == 0) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              pKi = SerialUSB.parseFloat();
              received_2 = 1;
            }
          }
          received_1 = 1;
        }
        break;
      case 'd':
        {
          SerialUSB.println("enter new pKd!");
          while (received_2 == 0) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              pKd = SerialUSB.parseFloat();
              received_2 = 1;
            }
          }
          received_1 = 1;
        }
        break;
      case 'q':
        {
          SerialUSB.println("quit");
          received_1 = 1;
        }
        break;
    }
  }
  SerialUSB.println();
  SerialUSB.println();
  parameterQuery();
}


void step_response() {

  float current_position = yw;
  bool received = false;
  int response_step = 0;

  SerialUSB.println("Enter step value in degree!");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      response_step = SerialUSB.parseInt();
      received = true;
    }
  }

  r = current_position;
  SerialUSB.println(micros());

  unsigned long start_millis = millis();

  while (millis() < (start_millis + 1700)) { //half a second

    SerialUSB.print(micros());
    SerialUSB.print(',');
    SerialUSB.print(r - current_position); //print target position
    SerialUSB.print(",");
    SerialUSB.println(yw - current_position); // print current position

    if (millis() > start_millis + 300) {
      r = (current_position + response_step);
    }

    if (millis() > start_millis + 1000) {
      r = current_position;
    }
  }
}

void get_max_frequency() {
  disableTCInterrupts();

  SerialUSB.println("");
  SerialUSB.println("Calibrating loop time");
  SerialUSB.println("---------------------");
  SerialUSB.println("make sure you move the motor while testing");
  SerialUSB.println("");

  int k = 1;
  int max_counter = 10000;
  int frequency = 100000;
  int temp_frequency = 0;
  bool last_enabled = enabled;
  enabled = 1;
  frequency_test = true;

  while (k <= 10) {
    int i = 1;

    unsigned long starting = micros();

    while (i <= max_counter) {
      TC5_Handler();
      serialCheck();
      i++;
    }

    temp_frequency = 1000000 / ((micros() - starting) / max_counter);

    if (temp_frequency < frequency) {
      frequency = temp_frequency;
    }

    SerialUSB.print("loop : ");
    SerialUSB.print(k);
    SerialUSB.print("/10 frequency = ");
    SerialUSB.println(temp_frequency);

    k++;

  }

  frequency = 10 * (floor(( 0.99 * frequency) / 10));

  SerialUSB.println("");
  SerialUSB.println("-----------");
  SerialUSB.print("volatile float Fs = ");
  SerialUSB.println(frequency);
  SerialUSB.println("; //Hz");

  enabled = last_enabled;

  enableTCInterrupts();
}


void set_filter_frequency() {
  bool received = false;
  SerialUSB.println();
  SerialUSB.println("set new cutoff frequency");
  SerialUSB.println();
  SerialUSB.println("enter new value:");


  while (!received) {
    delay(100);

    if (SerialUSB.peek() != -1) {
      Fc = SerialUSB.parseFloat();
      received = true;
    }
  }

  set_filter_coeff(Fs , Fc);
}

void set_filter_coeff(float F_sample, float F_cut) {

  float frequency_ratio = (F_cut / F_sample);
  float ita = 1.0 / tan(Pi * frequency_ratio);
  float q = sqrt(2.0);

  coeff_b0 = 1.0 / (1.0 + (q * ita ) + (ita * ita));
  coeff_b1 = 2 * coeff_b0;
  coeff_b2 = coeff_b0;

  coeff_a1 = 2.0 * ((ita * ita) - 1.0) * coeff_b0;
  coeff_a2 = -(1.0 - (q * ita) + (ita * ita)) * coeff_b0;

}

