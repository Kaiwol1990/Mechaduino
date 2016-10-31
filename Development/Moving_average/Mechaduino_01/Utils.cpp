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
  disableTC5Interrupts();

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
    delay(3);
  }
  lastencoderReading = lastencoderReading / avg;

  dir = true;
  oneStep();
  delay(100);

  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delay(3);
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

  // fast
  while ( (encoderReading * 0.02197265625)  > 10) {
    encoderReading = 0;

    oneStep();

    delay(10); //wait for seatle

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(3);
    }
    encoderReading = encoderReading / avg;
  }

  //slow
  while ( (encoderReading * 0.02197265625)  > 2) {
    encoderReading = 0;

    quaterStep();

    delay(10); //wait for seatle

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(3);
    }
    encoderReading = encoderReading / avg;
  }

  SerialUSB.println();
  SerialUSB.println("got close to zero position");
  SerialUSB.print("position = ");
  SerialUSB.println(encoderReading * 0.02197265625);

  delay(100); //wait for seatle

  if ((encoderReading * 0.02197265625)  > 0.91) {
    dir = false;
    quaterStep();
    quaterStep();
    dir = true;
  }

  SerialUSB.print("position = ");
  SerialUSB.println(encoderReading * 0.02197265625);

  // got close to zero position

  jump_to_fullstepp(); // jump to the next fullstep

  delay(100); //wait for seatle

  encoderReading = 0;
  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delay(3);
  }
  encoderReading = encoderReading / avg;


  SerialUSB.print("position = ");
  SerialUSB.println(encoderReading * 0.02197265625);



  delay(2000);


  SerialUSB.println();
  SerialUSB.println("calibration single steps");
  // step to every single fullstep position and read the Encoder
  step_target = 0.0;
  for (int x = 0; x < steps_per_revolution; x++) {

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(3);
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
  enableTC5Interrupts();

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

      case 'j':
        step_response();
        break;

      case 'm':
        Serial_menu();
        break;

      case 'f':
        get_max_frequency();
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
  SerialUSB.println("j  -  step response");
  SerialUSB.println("m  -  print main menu");
  SerialUSB.println("f  -  get max loop frequency");
  SerialUSB.println("");
}

void setpoint() {
  unsigned long start_millis = millis();
  int time_out = 5000;
  bool received = false;

  SerialUSB.println("Enter step value in degree!");

  SerialUSB.print("current Setpoint: ");
  SerialUSB.println(y);
  SerialUSB.println("Enter setpoint:");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      r = SerialUSB.parseFloat();
      received = true;
    }
    else if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }
  }

  SerialUSB.print("new Setpoint: ");
  SerialUSB.println(r);
}

void parameterQuery() {
  SerialUSB.println(' ');
  SerialUSB.println("//---- PID Values -----");

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
    step_target += (PA / 4);
  }
  else {
    step_target -= (PA / 4);
  }

  output(step_target, 64);

}

void oneStep() {

  if (dir == 0) {
    step_target += PA;
  }
  else {
    step_target -= PA;
  }

  output(step_target, 64);

}

int readEncoder()           //////////////////////////////////////////////////////   READENCODER   ////////////////////////////
{
  long angleTemp;

  REG_PORT_OUTCLR1 = PORT_PB09;  // write chipSelectPin LOW

  //read with Measured angle with dynamic angle error compensation (DAEC)
  byte b1 = SPI.transfer(0xFF);
  byte b2 = SPI.transfer(0xFF);

  //read with Measured angle without dynamic angle error compensation (DAEC)
  //byte b1 = SPI.transfer(0x7F);
  //byte b2 = SPI.transfer(0xFE);

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
  const int overflow_TC_5 = (48000000 / FPID) - 1;
  const int overflow_TC_4 = (48000000 / FSAMPLE) - 1;

  // Enable GCLK for TC4 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (uint16_t) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY);

  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;   // Set Timer counter Mode to 16 bits
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;   // Set Timer counter Mode to 16 bits
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal Normal Frq
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal Normal Frq
  WAIT_TC16_REGS_SYNC(TC5)

  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;   // Set perscaler
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;   // Set perscaler
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.CC[0].reg = overflow_TC_5;
  TC4->COUNT16.CC[0].reg = overflow_TC_4;
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC5->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC5->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0


  TC4->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC4->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC4->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0




  NVIC_SetPriority(TC4_IRQn, 2);
  NVIC_SetPriority(TC5_IRQn, 1);


  // Enable InterruptVector
  NVIC_EnableIRQ(TC5_IRQn);
  NVIC_EnableIRQ(TC4_IRQn);
}

void enableTC4Interrupts() {
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC4
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
}


void enableTC5Interrupts() {
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
}

void disableTC5Interrupts() {
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync
}


void disableTC4Interrupts() {
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC4
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync
}



void parameterEdit() {
  bool received_1 = false;
  bool received_2 = false;
  unsigned long start_millis = millis();
  int time_out = 5000;

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


  while (!received_1)  {
    delay(100);
    char inChar2 = (char)SerialUSB.read();

    if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }

    switch (inChar2) {
      case 'p':
        {
          SerialUSB.println("enter new pKp!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              pKp = SerialUSB.parseFloat();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'i':
        {
          SerialUSB.println("enter new pKi!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              pKi = SerialUSB.parseFloat();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'd':
        {
          SerialUSB.println("enter new pKd!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              pKd = SerialUSB.parseFloat();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'q':
        {
          SerialUSB.println("quit");
          received_1 = true;
        }
        break;
    }
  }

  SerialUSB.println();
  SerialUSB.println();
  parameterQuery();
}


void step_response() {
  bool last_enabled = enabled;
  enabled = 1;

  float current_position = y;
  bool received = false;
  int response_step = 0;

  unsigned long start_millis = millis();
  int time_out = 5000;

  SerialUSB.println("Enter step value in degree!");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      response_step = SerialUSB.parseInt();
      received = true;
    }
    else if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }
  }

  r = current_position;
  SerialUSB.println(micros());

  start_millis = millis();

  while (millis() < (start_millis + 1700)) { //half a second

    SerialUSB.print(micros());
    SerialUSB.print(',');
    SerialUSB.print(r - current_position); //print target position
    SerialUSB.print(",");
    SerialUSB.println(y - current_position); // print current position

    if (millis() > start_millis + 300) {
      r = (current_position + response_step);
    }

    if (millis() > start_millis + 1000) {
      r = current_position;
    }
  }
  enabled = last_enabled;
}

void get_max_frequency() {
  disableTC5Interrupts();

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
  SerialUSB.print("volatile float FPID = ");
  SerialUSB.print(frequency);
  SerialUSB.println("; //Hz");

  enabled = last_enabled;

  enableTC5Interrupts();
  enableTC4Interrupts();
}


float modulo(float dividend, float divisor) {
  dividend = (1000 * dividend) ;
  divisor = (1000 * divisor);

  float answer = (((int)dividend % (int)divisor) / 1000);
  return answer;
}


