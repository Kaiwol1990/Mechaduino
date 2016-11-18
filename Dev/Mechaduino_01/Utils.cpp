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
#include <avr/pgmspace.h>

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


  if (use_enable_pin == true) {
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
    step_target += 1;
  }
  else {
    step_target -= 1;
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

void output(int theta, int effort) {
  static int angle;
  
  static int v_coil_A;
  static int v_coil_B;
  
  static int sin_coil_A;
  static int sin_coil_B;

  angle = mod((5 * theta) , 3600);

  sin_coil_A = pgm_read_word_near(sine_lookup_val1 + angle);
  if (sin_coil_A > 1024) {
    sin_coil_A = sin_coil_A - 65536;
  }

  sin_coil_B = pgm_read_word_near(sine_lookup_val2 + angle);
  if (sin_coil_B > 1024) {
    sin_coil_B = sin_coil_B - 65536;
  }

  if (calibration_running) {
    v_coil_A = ((effort * sin_coil_A) / 1024);
    v_coil_B = ((effort * sin_coil_B) / 1024);
  }
  else {
    v_coil_A = ((7 * v_coil_A) + ((effort * sin_coil_A) / 1024)) / 8;
    v_coil_B = ((7 * v_coil_B) + ((effort * sin_coil_B) / 1024)) / 8;
  }

  analogFastWrite(VREF_2, abs(v_coil_A));
  analogFastWrite(VREF_1, abs(v_coil_B));

  if (v_coil_A >= 0)  {
    REG_PORT_OUTSET0 = PORT_PA20;     //write IN_4 HIGH
    REG_PORT_OUTCLR0 = PORT_PA15;     //write IN_3 LOW
  }
  else  {
    REG_PORT_OUTCLR0 = PORT_PA20;     //write IN_4 LOW
    REG_PORT_OUTSET0 = PORT_PA15;     //write IN_3 HIGH
  }

  if (v_coil_B >= 0)  {
    REG_PORT_OUTSET0 = PORT_PA21;     //write IN_2 HIGH
    REG_PORT_OUTCLR0 = PORT_PA06;     //write IN_1 LOW
  }
  else  {
    REG_PORT_OUTCLR0 = PORT_PA21;     //write IN_2 LOW
    REG_PORT_OUTSET0 = PORT_PA06;     //write IN_1 HIGH
  }
}

void calibration() {
  calibration_running = true;
  disableTC5Interrupts();

  SerialUSB.println(" ");
  SerialUSB.println("---- Calibration Routine ----");
  SerialUSB.println(" ");

  int encoderReading = 0;
  int lastencoderReading = 0;

  int avg = 20;         //how many readings to average

  int iStart = 0;
  int jStart = 0;
  int stepNo = 0;
  //step_target = 0;

  int fullStepReadings[steps_per_revolution];
  int ticks = 0;

  float lookupAngle = 0.0;

  dir = true;
  output(0, 64);

  for (int reading = 0; reading < avg; reading++) {
    lastencoderReading += readEncoder();
    delay(5);
  }
  lastencoderReading = lastencoderReading / avg;

  output(PA, 64);
  output(2 * PA, 64);
  output(3 * PA, 64);

  delay(500);

  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delay(5);
  }
  encoderReading = encoderReading / avg;

  if ((encoderReading - lastencoderReading) < 0)
  {
    SerialUSB.println("Wired backwards");
    return;
  }

  //jump to fullstep
  output(0, 64);

  delay(500);

  SerialUSB.println("Calibrating single steps");
  SerialUSB.println("|--------------------------------------------------|");
  SerialUSB.print("|");

  // step to every single fullstep position and read the Encoder
  step_target = 0;

  for (int x = 0; x < steps_per_revolution; x++) {

    //output((x * PA), 64);

    delay(100);

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delay(5);
    }
    encoderReading = encoderReading / avg;


    fullStepReadings[x] = encoderReading;

    oneStep();

    if (((50 * x) % steps_per_revolution) == 0) {
      SerialUSB.print("-");
    }
  }

  SerialUSB.println("|");
  SerialUSB.println();
  SerialUSB.println("done!");
  SerialUSB.println();
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


  SerialUSB.println("Generating lookuptable");
  SerialUSB.print("const PROGMEM int lookup[] = {");

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
          lookupAngle = 0.1 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }

      else if (i == (iStart + steps_per_revolution)) {
        for (int j = 0; j < jStart; j++) {
          lookupAngle = 0.1 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }
      else {
        for (int j = 0; j < ticks; j++) {
          lookupAngle = 0.1 * mod(1000 * ((angle_per_step * i) + ((angle_per_step * j ) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }



    }

    else if (ticks < 1) {
      if (i == iStart) {
        for (int j = - ticks; j > (jStart); j--) {
          lookupAngle = 0.1 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }
      else if (i == iStart + steps_per_revolution) {
        for (int j = jStart; j > 0; j--) {
          lookupAngle = 0.1 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }
      else {
        for (int j = - ticks; j > 0; j--) {
          lookupAngle = 0.1 * mod(1000 * (angle_per_step * (i) + (angle_per_step * ((ticks + j)) / float(ticks))), 360000.0);
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(" , ");
        }
      }

    }
  }
  SerialUSB.println();
  SerialUSB.println("};");
  SerialUSB.println();

  calibration_running = false;
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

  unsigned long start_millis;
  start_millis = millis();
  int time_out = 5000;
  bool received = false;

  SerialUSB.println("Enter step value in degree!");

  SerialUSB.print("current Setpoint: ");
  SerialUSB.println((y / 100.0));
  SerialUSB.println("Enter setpoint:");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      r = 100 * SerialUSB.parseFloat();
      received = true;
    }
    else if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }
  }

  SerialUSB.print("new Setpoint: ");
  SerialUSB.println((r / 100.0));
}

void parameterQuery() {
  SerialUSB.println(' ');
  SerialUSB.println("//---- PID Values -----");

  SerialUSB.print("volatile int Kp = ");
  SerialUSB.print(Kp);
  SerialUSB.println(';');

  SerialUSB.print("volatile int Ki = ");
  SerialUSB.print(Ki);
  SerialUSB.println(';');

  SerialUSB.print("volatile int Kd = ");
  SerialUSB.print(Kd);
  SerialUSB.println(';');

}

void quaterStep() {

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


void setupTCInterrupts() {
  const int overflow_TC_5 = (48000000 / FPID) - 1;
  //const int overflow_TC_4 = (48000000 / FSAMPLE) - 1;

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


  TC5->COUNT16.CC[0].reg = overflow_TC_5;
  WAIT_TC16_REGS_SYNC(TC5)


  TC5->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC5->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC5->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0


  NVIC_SetPriority(TC5_IRQn, 1);

  // Enable InterruptVector
  NVIC_EnableIRQ(TC5_IRQn);
}




void enableTC5Interrupts() {
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
}

void disableTC5Interrupts() {
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC5
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
  SerialUSB.print("p ----- Kp = ");
  SerialUSB.println(Kp);

  SerialUSB.print("i ----- Ki = ");
  SerialUSB.println(Ki);

  SerialUSB.print("d ----- Kd = ");
  SerialUSB.println(Kd);

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
          SerialUSB.println("enter new Kp!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              Kp = SerialUSB.parseInt();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'i':
        {
          SerialUSB.println("enter new Ki!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              Ki = SerialUSB.parseInt();
              received_2 = true;
            }
          }
          received_1 = true;
        }
        break;
      case 'd':
        {
          SerialUSB.println("enter new Kd!");
          while (!received_2) {
            delay(100);
            if (SerialUSB.peek() != -1) {
              Kd = SerialUSB.parseInt();
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
  dir = true;
  enabled = 1;

  int current_position = y;
  bool received = false;
  int response_steps = 0;
  int last_step_target = step_target;

  unsigned long start_millis = millis();
  int time_out = 5000;

  SerialUSB.println("Enter step value in degree!");

  while (!received) {
    delay(100);
    if (SerialUSB.peek() != -1) {
      response_steps = SerialUSB.parseInt();
      received = true;
    }
    else if (millis() > start_millis + time_out) {
      SerialUSB.println("time out");
      return;
    }
  }

  SerialUSB.print(micros());
  SerialUSB.print(',');
  SerialUSB.print("current_position");
  SerialUSB.println();

  start_millis = millis();

  while (millis() < (start_millis + 1700)) { //half a second

    SerialUSB.print(micros());
    SerialUSB.print(',');
    SerialUSB.print((r - current_position) / 100.0); //print target position
    SerialUSB.print(",");
    SerialUSB.println((y - current_position) / 100.0); // print current position

    if (millis() > start_millis + 300) {
      step_target = last_step_target + response_steps;
    }

    if (millis() > start_millis + 1000) {
      step_target = last_step_target;
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
  SerialUSB.print("volatile int FPID = ");
  SerialUSB.print(frequency);
  SerialUSB.println("; //Hz");

  enabled = last_enabled;

  enableTC5Interrupts();

}

