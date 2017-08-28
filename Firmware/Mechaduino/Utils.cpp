//Contains the definitions of the functions used by the firmware.
#include <SPI.h>
#include "Utils.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "State.h"
#include "Controller.h"
#include "analogFastWrite.h"
#include "A4954.h"
#include "Encoder.h"
#include "board.h"
#include "Serial.h"
#include "Language.h"
#include "Cmd.h"


void boot() {
  SerialUSB.begin(baudrate);
  cmdInit(&SerialUSB);

  delay(1000);

  SerialUSB.print("setup pins:");
  setupPins();
  SerialUSB.println(" OK");

  SerialUSB.print("setup SPI:");
  setupSPI();
  SerialUSB.println(" OK");

  SerialUSB.print("setup Interrupts:");
  setupTCInterrupts();
  SerialUSB.println(" OK");


  delay(1000);
  SerialUSB.print("setup controller:");
  enabled = false;
  enableTC5Interrupts();
  SerialUSB.println(" OK");

  delay(200);

  if (USE_ENABLE_PIN) {
    SerialUSB.print("setup enable pin:");
    enaInterrupt();
  }
  else {
    enabled = true;
  }
  SerialUSB.println(" OK");


  SerialUSB.print("setup direction pin:");
  dirInterrupt();
  SerialUSB.println(" OK");

  // resetting the error register
  error_register = 0B1000000000000000;

  SerialUSB.println(bootscreen_1);
  SerialUSB.println(bootscreen_2);
  SerialUSB.println(bootscreen_3);
  SerialUSB.println(bootscreen_4);
  SerialUSB.println(bootscreen_5);
  SerialUSB.println(bootscreen_6);
  SerialUSB.println(bootscreen_7);
  SerialUSB.println(bootscreen_8);
  SerialUSB.println(bootscreen_9);
  SerialUSB.println(bootscreen_10);

  SerialUSB.print("   compiling date: ");
  SerialUSB.println(__DATE__);

  SerialUSB.print("   firmware-version: ");
  SerialUSB.println(firmware_version);

  SerialUSB.print("   identifier: ");
  SerialUSB.println(identifier);
  SerialUSB.println("");
  SerialUSB.print("CMD >> ");
}



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

  pinMode(ena_pin, INPUT_PULLUP);
  attachInterrupt(ena_pin, enaInterrupt, CHANGE);

  digitalWriteDirect(IN_4, LOW);
  digitalWriteDirect(IN_3, LOW);
  digitalWriteDirect(IN_2, LOW);
  digitalWriteDirect(IN_1, LOW);

  digitalWriteDirect(VREF_2, LOW);
  digitalWriteDirect(VREF_1, LOW);
}


void setupSPI() {
  SPISettings settingsA(10000000, MSBFIRST, SPI_MODE1);

  SPI.begin();    //AS5047D SPI uses mode=1 (CPOL=0, CPHA=1)
  delay(1000);
  SPI.beginTransaction(settingsA);
}


void stepInterrupt() {
  if (dir) {
    step_target += step_add;
  }
  else {
    step_target -= step_add;
  }
}


void dirInterrupt() {
  if (!INVERT) {
    if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
      dir = false;
    }
    else {
      dir = true;
    }
  }
  else {
    if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
      dir = true;
    }
    else {
      dir = false;
    }
  }
}


void enaInterrupt() {
  if (USE_ENABLE_PIN) {
    if (REG_PORT_IN0 & PORT_PA14) { // check if ena_pin is HIGH
      enabled = false;
    }
    else {
      enabled = true;
    }
  }
}


void calibration(int arg_cnt, char **args) {

  disableTC5Interrupts();
  disableTC4Interrupts();

  int avg = 100;
  bool smooth = check_argument(args, arg_cnt, "-smooth");
  bool debug = check_argument(args, arg_cnt, "-debug");
  bool check_readings = check_argument(args, arg_cnt, "-check");

  SerialUSB.println(calibrate_header);
  SerialUSB.println();

  enabled = true;
  dir = true;
  step_target = 0;
  delay(500);


  float encoderReading = 0;

  for (int i = 0; i < 50; i++) {
    encoderReading = encoderReading + readEncoder();
  }
  encoderReading = encoderReading / 50.0;




  oneStep();
  delay(100);
  oneStep();
  delay(100);
  oneStep();
  delay(100);
  oneStep();


  float temp_pos = 0;
  for (int i = 0; i < 50; i++) {
    temp_pos = temp_pos + readEncoder();
  }
  temp_pos = temp_pos / 50.0;

  if ((temp_pos - encoderReading) < 0)
  {
    SerialUSB.println("Wired backwards");
    enabled = false;
    enableTC5Interrupts();
    return;
  }


  SerialUSB.println("Calibrating fullsteps");
  SerialUSB.println(procent_bar);
  int counter = 0;
  int count = steps_per_revolution / 50;
  dir = true;

  int readings[steps_per_revolution];
  int reverse_readings[steps_per_revolution];

  int temp_reading = 4 * readEncoder();
  int last_reading = temp_reading;


  step_target = 0;
  // step to every single fullstep position and read the Encoder
  for (int i = 0; i < steps_per_revolution; i++) {

    if (canceled()) {
      enabled = false;
      return;
    }

    counter += 1;

    delay(100);

    // flush the encoder
    readEncoder();
    readEncoder();

    // start real readings
    encoderReading = 0;
    temp_reading = 4 * readEncoder();
    last_reading = temp_reading;

    for (int k = 0; k < avg; k++) {

      temp_reading = 4 * readEncoder();

      if ((temp_reading - last_reading) <= -32768) {
        temp_reading += 65536;
      }
      else if ((temp_reading - last_reading) >= 32768) {
        temp_reading -= 65536;
      }

      encoderReading += temp_reading;
      last_reading = temp_reading;

      delayMicroseconds(100);
    }


    readings[i] =  (encoderReading / avg) + 0.5;

    if (readings[i] > 65536) {
      readings[i] = readings[i] - 65536;
    }

    if (counter == count) {
      counter = 0;
      SerialUSB.print(".");
    }

    //increase step_target with one
    if (i < 199) {
      oneStep();
    }

  }
  SerialUSB.println(" -> done! Reverse!");

  if (debug) {
    SerialUSB.println("");
    SerialUSB.println("");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(readings[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("");
  }


  //backwarts
  delay(100);
  oneStep();
  delay(250);
  dir = false;
  delay(250);
  oneStep();
  delay(100);

  temp_reading = 4 * readEncoder();
  last_reading = temp_reading;

  for (int i = (steps_per_revolution - 1); i >= 0; i--) {

    if (canceled()) {
      enabled = false;
      return;
    }
    delay(100);

    counter += 1;

    // start real readings
    encoderReading = 0;
    temp_reading = 4 * readEncoder();
    last_reading = temp_reading;

    for (int k = 0; k < avg; k++) {

      temp_reading = 4 * readEncoder();

      if ((temp_reading - last_reading) <= -32768) {
        temp_reading += 65536;
      }
      else if ((temp_reading - last_reading) >= 32768) {
        temp_reading -= 65536;
      }

      encoderReading += temp_reading;
      last_reading = temp_reading;

      delayMicroseconds(100);
    }

    reverse_readings[i] =  (encoderReading / avg) + 0.5;

    if (counter == count) {
      counter = 0;
      SerialUSB.print(".");
    }

    //increase step_target with one
    if (i > 0) {
      oneStep();
    }
  }
  SerialUSB.println(" -> done!");

  if (debug) {
    SerialUSB.println("");
    SerialUSB.println("");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(reverse_readings[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("");
  }


  // calculate the mean reading between forward and reverse
  for (int i = 0; i < steps_per_revolution; i++) {
    int temp = readings[i] - reverse_readings[i];

    if (temp < 0) {
      readings[i] = readings[i] - (temp / 2);
    }
    else {

      readings[i] = reverse_readings[i] + (temp / 2);
    }
  }


  if (debug) {
    SerialUSB.println("");
    SerialUSB.println("");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(readings[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("");
  }



  if (check_readings) {

    delay(100);
    oneStep();
    delay(250);
    dir = true;
    delay(250);
    oneStep();
    delay(100);

    SerialUSB.println("");
    SerialUSB.println("");
    SerialUSB.println("Checking fullsteps");
    SerialUSB.println(procent_bar);

    int check_readings[steps_per_revolution];

    for (int i = 0; i < steps_per_revolution; i++) {

      if (canceled()) {
        enabled = false;
        return;
      }

      counter += 1;

      delay(100);

      encoderReading = 0;


      for (int k = 0; k < avg; k++) {

        temp_reading = 4 * readEncoder();

        if ((temp_reading - last_reading) <= -32768) {
          temp_reading += 65536;
        }
        else if ((temp_reading - last_reading) >= 32768) {
          temp_reading -= 65536;
        }

        encoderReading += temp_reading;

        last_reading = temp_reading;

        delayMicroseconds(100);
      }


      check_readings[i] = ((encoderReading / avg) + 0.5);

      if (check_readings[i] > 65536) {
        check_readings[i] = check_readings[i] - 65536;
      }


      if (counter == count) {
        counter = 0;
        SerialUSB.print(".");
      }

      //increase step_target with one fullstep
      if (i < 199) {
        oneStep();
      }

    }

    SerialUSB.println(" -> done! Reverse!");


    if (debug) {
      SerialUSB.println("");
      SerialUSB.println("");
      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print(check_readings[i]);
        SerialUSB.print(",");
      }
      SerialUSB.println("");
    }

    //backwarts
    delay(100);
    oneStep();
    delay(250);
    dir = false;
    delay(250);
    oneStep();
    delay(100);

    for (int i = (steps_per_revolution - 1); i >= 0; i--) {

      if (canceled()) {
        enabled = false;
        return;
      }

      counter += 1;

      delay(100);

      // flush the encoder
      readEncoder();
      readEncoder();

      // start real readings
      encoderReading = 0;

      for (int k = 0; k < avg; k++) {

        temp_reading = 4 * readEncoder();

        if ((temp_reading - last_reading) <= -32768) {
          temp_reading += 65536;
        }
        else if ((temp_reading - last_reading) >= 32768) {
          temp_reading -= 65536;
        }

        encoderReading += temp_reading;

        last_reading = temp_reading;

        delayMicroseconds(100);
      }

      int temp_reading =  (encoderReading / avg) + 0.5;


      if (temp_reading > 65536) {
        temp_reading = temp_reading - 65536;
      }

      check_readings[i] = (check_readings[i] + temp_reading) / 2;

      if (counter == count) {
        counter = 0;
        SerialUSB.print(".");
      }

      //increase step_target with one fullstep
      oneStep();

    }
    SerialUSB.println(" -> done!");

    if (debug) {
      SerialUSB.println("");
      SerialUSB.println("");
      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print(check_readings[i]);
        SerialUSB.print(",");
      }
      SerialUSB.println("");

    }





    SerialUSB.println("");

    float max_error = 0;
    float min_error = 65536;

    for (int i = 0; i < steps_per_revolution; i++) {

      if ((readings[i] - check_readings[i]) > max_error) {
        max_error = (readings[i] - check_readings[i]);
      }

      if ((readings[i] - check_readings[i])  < min_error) {
        min_error = (readings[i] - check_readings[i]);
      }

    }

    max_error = max_error / 65536;
    min_error = min_error / 65536;


    SerialUSB.println("");
    SerialUSB.print("Maximal occureing Error = ");
    SerialUSB.print(100 * max_error, 5);
    SerialUSB.println(" %");
    SerialUSB.print("Minimal occureing Error = ");
    SerialUSB.print(100 * min_error, 5);
    SerialUSB.println(" %");

    bool qst = userqst(15000, "Continue?");

    if (qst == false) {
      step_target = 0;
      enabled = false;
      output(0, 0);
      return;
    }


    if (debug) {
      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print(check_readings[i]);
        SerialUSB.print(",");
      }
      SerialUSB.println("");

      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print((readings[i] - check_readings[i]));
        SerialUSB.print(",");
      }
      SerialUSB.println("");
      SerialUSB.println("");
    }
  }





  step_target = 0;
  enabled = false;
  output(0, 0);



  if (smooth) {

    SerialUSB.println("Smoothing fullstep readings");

    int smoothed_readings[steps_per_revolution];

    // generate a perfect lookup table
    int perfect[steps_per_revolution];
    for ( int i = 0; i < steps_per_revolution; i++) {
      perfect[i] = ((i * 65536) / steps_per_revolution);
    }

    // find min value in full step readings and its index
    float minimum = 65536;
    int idx = 0;
    for ( int i = 0; i < steps_per_revolution; i++) {
      if (readings[i] < minimum) {
        minimum = readings[i];
        idx = i;
      }
    }


    // shift the raw readings to sort them
    float fullStepReadings[steps_per_revolution];
    for ( int i = 0; i < steps_per_revolution; i++) {
      counter = i + idx;
      if (counter >= steps_per_revolution) {
        counter = counter - steps_per_revolution;
      }
      fullStepReadings[i] = readings[counter];
    }


    // calculate the error between the measured and perfect lookup
    float error_raw[steps_per_revolution];
    for ( int i = 0; i < steps_per_revolution; i++) {
      error_raw[i] =  (float)(perfect[i]) - (float)(fullStepReadings[i]);
    }


    // generate a normal distribution curve
    // needed to smooth the fullstep readings
    float gausian[31];
    counter = 0;
    float m = 0.0;
    for (int i = -15; i <= 15; i++) {
      gausian[counter] = exp(-0.5 * ((float)i * (float)i) / (25.0));
      m = m + gausian[counter];
      counter = counter + 1;
    }
    for (int i = 0; i < 31; i++) {
      gausian[i] =  gausian[i] / m;
    }


    // smooth the fullstep readings with a ring buffer and the normal distribution
    float error[steps_per_revolution];
    counter = 0;
    for ( int i = 0; i < steps_per_revolution; i++) {
      m = 0;
      counter = 0;
      int lower = i - 15;
      int higher = i + 15;

      for (int j = lower; j < higher; j++) {
        if (j >= steps_per_revolution) {
          m = m + error_raw[j - steps_per_revolution] * gausian[counter];
        }
        else if (j < 0) {
          m = m + error_raw[j + steps_per_revolution] * gausian[counter];
        }
        else {
          m = m + error_raw[j] * gausian[counter];
        }
        counter += 1;
      }
      error[i] = m;
    }


    // recalculate the measured angles with the smoothed error
    float angle_sorted[steps_per_revolution];
    for ( int i = 0; i < steps_per_revolution; i++) {
      angle_sorted[i] = (float)(perfect[i]) - (float)(error[i]);
    }


    // shift the sorted angle back
    for ( int i = 0; i < steps_per_revolution; i++) {
      counter = i - idx;
      if (counter < 0) {
        counter = counter + steps_per_revolution;
      }
      smoothed_readings[i] = (angle_sorted[counter] + 0.5);
    }


    if (check_readings) {
      float max_error = 0;
      float min_error = 65536;

      for (int i = 0; i < steps_per_revolution; i++) {

        if ((readings[i] - smoothed_readings[i]) > max_error) {
          max_error = (readings[i] - smoothed_readings[i]);
        }

        if ((readings[i] - smoothed_readings[i])  < min_error) {
          min_error = (readings[i] - smoothed_readings[i]);
        }

      }

      max_error = max_error / 65536;
      min_error = min_error / 65536;


      SerialUSB.println("");
      SerialUSB.print("Maximal occureing Error = ");
      SerialUSB.print(100 * max_error, 5);
      SerialUSB.println(" %");
      SerialUSB.print("Minimal occureing Error = ");
      SerialUSB.print(100 * min_error, 5);
      SerialUSB.println(" %");

      bool qst = userqst(15000, "Continue?");

      if (qst == false) {
        step_target = 0;
        enabled = false;
        output(0, 0);
        return;
      }
    }

    for (int i = 0; i < steps_per_revolution; i++) {
      readings[i] = smoothed_readings[i];
    }

  }


  SerialUSB.println();
  SerialUSB.println();

  // calculate the ticks between the fullsteps
  float ticks[steps_per_revolution];

  int low;
  int high;
  int iStart = 0;
  int jStart = 0;
  int stepNo = 0;
  for (int i = 0; i < steps_per_revolution; i++) {
    low = i;
    high = i + 1;

    if (high >= steps_per_revolution) {
      high = high - steps_per_revolution;
    }

    ticks[i] = readings[high] - readings[low];

    if (ticks[i] < -32768) {
      ticks[i] += 65536;
    }
    else if (ticks[i] > 32768) {
      ticks[i] -= 65536;
    }

    for (int j = 0; j < ticks[i]; j++) {
      stepNo = (mod(readings[i] + j, 65536));
      if (stepNo == 0) {
        iStart = i;
        jStart = j;
      }
    }
  }

  if (debug) {
    SerialUSB.println("ticks ");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(ticks[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("");
    SerialUSB.println("ticks done");
  }



  // calculate the lookup table on the fly
  counter = 0;
  float angle_per_step = 36000.0 / steps_per_revolution;
  float lookupAngle;
  float tick;

  SerialUSB.print("const uint16_t lookup[] = {");

  for (int i = iStart; i < (iStart + steps_per_revolution + 1); i++) {

    if (i >= steps_per_revolution) {
      tick = ticks[i - steps_per_revolution];
    }
    else {
      tick = ticks[i];
    }

    if (i == iStart) {
      for (int j = jStart; j < tick; j++) {
        counter += 1;

        lookupAngle =  mod( (angle_per_step * i) + (((angle_per_step * j ) / tick) + 0.5) , 36000);

        if (mod(counter, 4) == 0) {
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(", ");
        }
      }
    }

    else if (i == (iStart + steps_per_revolution)) {
      for (int j = 0; j < jStart; j++) {
        counter += 1;

        lookupAngle =  mod( (angle_per_step * i) + (((angle_per_step * j ) / tick) + 0.5) , 36000);

        if (mod(counter, 4) == 0) {
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(", ");
        }
      }
    }
    else {
      for (int j = 0; j < tick; j++) {
        counter += 1;

        lookupAngle =  mod( (angle_per_step * i) + (((angle_per_step * j ) / tick) + 0.5) , 36000);

        if (mod(counter, 4) == 0) {
          SerialUSB.print(lookupAngle, 0);
          SerialUSB.print(", ");
        }
      }
    }


  }
  SerialUSB.println();
  SerialUSB.println("};");


  enableTC5Interrupts();
}





void oneStep() {

  if (dir == 0) {
    step_target -= 1024;
  }
  else {
    step_target += 1024;
  }

  int target_raw = step_target * stepangle;
  int raw_0 = mod(target_raw, 36000);
  output_calibration(raw_0 , uMAX / 3);
}





int mod(int xMod, int mMod) {
  return (xMod % mMod + mMod) % mMod;
}





void setupTCInterrupts() {
  const int overflow_TC_5 = (48000000 / FPID) - 1;
  const int overflow_TC_4 = (48000000 / Fs) - 1;

  // Enable GCLK for TC4 and TC5 (timer counter input clock)
  GCLK->CLKCTRL.reg = (int) (GCLK_CLKCTRL_CLKEN | GCLK_CLKCTRL_GEN_GCLK0 | GCLK_CLKCTRL_ID(GCM_TC4_TC5));
  while (GCLK->STATUS.bit.SYNCBUSY);

  // ----- TC5 with FPID-----
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

  NVIC_SetPriority(TC5_IRQn, 2);


  // ----- TC4 with FMEASURE-----
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TCx
  WAIT_TC16_REGS_SYNC(TC4)                      // wait for sync

  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_MODE_COUNT16;   // Set Timer counter Mode to 16 bits
  WAIT_TC16_REGS_SYNC(TC4)

  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_WAVEGEN_MFRQ; // Set TC as normal Normal Frq
  WAIT_TC16_REGS_SYNC(TC4)

  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_PRESCALER_DIV1;   // Set perscaler
  WAIT_TC16_REGS_SYNC(TC4)

  TC4->COUNT16.CC[0].reg = overflow_TC_4;
  WAIT_TC16_REGS_SYNC(TC4)

  TC4->COUNT16.INTENSET.reg = 0;              // disable all interrupts
  TC4->COUNT16.INTENSET.bit.OVF = 1;          // enable overfollow
  TC4->COUNT16.INTENSET.bit.MC0 = 1;         // enable compare match to CC0

  NVIC_SetPriority(TC4_IRQn, 1);


  // Enable InterruptVector
  NVIC_EnableIRQ(TC5_IRQn);
  NVIC_EnableIRQ(TC4_IRQn);
}


void enableTC5Interrupts() {
  // ----- enables the main PID and measuring loop -----
  TC5->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      //wait for sync
  TC4->COUNT16.CTRLA.reg |= TC_CTRLA_ENABLE;    //Enable TC4
  WAIT_TC16_REGS_SYNC(TC4)                      //wait for sync
}


void disableTC5Interrupts() {
  // ----- disables the main PID loop -----
  TC5->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC5
  WAIT_TC16_REGS_SYNC(TC5)                      // wait for sync
}

void disableTC4Interrupts() {
  // ----- disables the main PID loop -----
  TC4->COUNT16.CTRLA.reg &= ~TC_CTRLA_ENABLE;   // Disable TC4
  WAIT_TC16_REGS_SYNC(TC4)                      // wait for sync
}



void PID_autotune(int arg_cnt, char **args) {
  SerialUSB.println(autotune_header);

  disableTC5Interrupts();

  float outputStep = uMAX;
  int frequency = 2 * FPID;
  int dt = (1000000 / frequency);
  int scan_dt = dt - 2;

  float temp_Kp = 0;
  float temp_Ki = 0;
  float temp_Kd = 0;

  unsigned long now = millis();
  int k = 1;


  SerialUSB.println(cancle_header);

  bool debug = check_argument(args, arg_cnt, "-d");
  bool gui = check_argument(args, arg_cnt, "-gui");
  int loops = return_float_argument(args, arg_cnt, "-c", 1, 1, 10);

  if (!gui) {
    SerialUSB.println("| loop | Noise | Frequency | lookback | P      | I      | D       |");
  }

  // start autotune
  while (k <= loops) {


    if (!gui) {
      SerialUSB.println("|-----------------------------------------------------------------|");
      SerialUSB.print("|   ");
      SerialUSB.print(k);
      if (k >= 10) {
        SerialUSB.print(" ");
      }
      else {
        SerialUSB.print("  ");
      }
    }
    delay(200);

    // measure current setpoint
    int setpoint = r;

    // measure the noise from the controller
    int noiseBand = measure_noise();
    disableTC5Interrupts();

    if (!gui) {
      SerialUSB.print("|   ");
      SerialUSB.print(noiseBand);
      if (noiseBand >= 10) {
        SerialUSB.print("  ");
      }
      else {
        SerialUSB.print("   ");
      }
    }

    int points[1124] = {0};
    float smoothed[1124] = {0};

#define filterSamples   3
    int sensSmoothArray1 [filterSamples];

    int peakType = 0;
    int peakCount = 0;
    int absMax = setpoint;
    int absMin = setpoint;

    unsigned long last_time = 0;

    bool isMax = true;
    bool isMin = true;

    int counter = 0;
    unsigned long start_millis = millis() + 500;

    u = outputStep;
    // start the oscilations and measure the behavior every 50 microsseconds
    while (counter < 1124) {

      // canceling call if something goes wrong
      if (canceled()) return;

      now = micros();
      if (now > last_time + scan_dt) {


        if (y > absMax) {
          absMax = y;
        }
        if (y < absMin) {
          absMin = y;
        }

        // oscillate the output base on the input's relation to the setpoint
        if (y > setpoint + noiseBand) {
          u = -outputStep;
        }
        else if (y < setpoint - noiseBand) {
          u = outputStep;
        }



        if (u > 0) {
          output(y + PA, u);
        }
        else {
          output(y - PA, u);
        }

        // wait half a second to get stable oscilations
        if (millis() > start_millis) {
          points[counter] = y;
          counter++;
        }

        last_time = now;

      }
    }

    output(y, 0);


    //smoothing the measured data
    for (int i = 0; i <= 1124; i++) {
      smoothed[i] = digitalSmooth(points[i], sensSmoothArray1);
    }


    for (int i = 0; i < 1024; i++) {
      points[i] = points[i + 50];
      smoothed[i] = smoothed[i + 50];
    }

    // building mean of the position data
    float sum = 0;
    for (int i = 0; i < 1024; i++) {
      sum = sum + smoothed[i];
    }
    sum = sum / 1024.0;

    for (int i = 0; i < 1024; i++) {
      smoothed[i] = smoothed[i] - sum;
      points[i] = (points[i] - sum) + 0.5;
    }

    if (debug) {
      for (int i = 0; i < 1024; i++) {
        SerialUSB.print(points[i]);
        SerialUSB.print(',');
        SerialUSB.println(smoothed[i]);
      }
    }

    // searching the main frequency
    int len = 1024;
    int thresh = 0;
    int sum_old = 0;
    char pd_state = 0;
    int period = 0;
    int i = 0;

    while (i < len - 1 && pd_state != 3) {
      sum_old = sum;
      sum = 0;

      for (int k = 1; k < len - i; k++) {
        sum = sum + (smoothed[k]) * (smoothed[k + i]);
      }

      if (pd_state == 2 && (sum - sum_old) <= 0) {
        period = i;
        pd_state = 3;
      }

      if (pd_state == 1 && (sum > thresh) && (sum - sum_old) > 0) {
        pd_state = 2;
      }

      //if (!i) {
      if (i == 0) {
        thresh = sum * 0.5;
        pd_state = 1;
      }

      i = i + 1;
    }


    if (period == 0) {
      SerialUSB.println();
      SerialUSB.println("Autotune failed, no oscilation detected!");
      return;
    }

    float frequency = (1000000.0 / (float)(dt)) / period;
    float Tu = 1.0 / frequency;

    if (!gui) {
      SerialUSB.print("|   ");
      SerialUSB.print(frequency, 1);
      if (frequency >= 100.0) {
        SerialUSB.print("   ");
      }
      else if (frequency >= 1000.0) {
        SerialUSB.print("  ");
      }
      else {
        SerialUSB.print("    ");
      }

      if (frequency < 2.0 || frequency > 9950.0) {
        SerialUSB.println();
        SerialUSB.println("Autotune failed, frequency not in usable range!");
        return;
      }
    }
    // calculating lookback points
    int nLookBack = ((1000000 * Tu) / dt) / 4;
    if (nLookBack >= 99) {
      nLookBack = 99;
    }

    if (!gui) {
      SerialUSB.print("|   ");
      SerialUSB.print(nLookBack);
      SerialUSB.print("     ");
    }
    // searching the min and max in data
    int peaks[20] = {0};
    int lastInputs[101] = {0};
    int refVal = 0;

    for (int i = 0; i < 1024; i++) {
      refVal = smoothed[i];

      if (refVal > absMax) {
        absMax = refVal;
      }
      if (refVal < absMin) {
        absMin = refVal;
      }
      isMax = true;
      isMin = true;

      for (int i = nLookBack - 1; i >= 0; i--) {
        int val = lastInputs[i];

        if (isMax == 1) {
          isMax = refVal > val;
        }

        if (isMin == 1) {
          isMin = refVal < val;
        }

        lastInputs[i + 1] = lastInputs[i];
      }
      lastInputs[0] = refVal;

      if (isMax) {
        if (peakType == 0) {
          peakType = 1;
        }
        if (peakType == -1)
        {
          peakType = 1;
        }
        peaks[peakCount] = refVal;

      }
      else if (isMin) {

        if (peakType == 0) {
          peakType = -1;
        }

        if (peakType == 1) {
          peakType = -1;
          peakCount++;
        }
      }
    }



    //Amplitude of the oscilation
    float A = 0;
    for (int j = 1; j <= peakCount - 1; j++) {
      A = A + peaks[j];
    }
    A = A / (peakCount - 1);

    // calculating PID settings
    float Ku = (4.0 * (2.0 * (float)outputStep))  / (PI * 2.0 * A);

    if (k == 1) {
      temp_Kp = (0.6 * Ku);
      temp_Ki = ((1.2 * Ku) / (Tu * FPID));
      temp_Kd = ((0.6 * Ku * Tu * FPID) / 8);
    }
    else {
      temp_Kp = temp_Kp + (0.6 * Ku);
      temp_Ki = temp_Ki + ((1.2 * Ku) / (Tu * FPID));
      temp_Kd = temp_Kd + ((0.6 * Ku * Tu * FPID) / 8);
    }

    if (!gui) {
      SerialUSB.print("| ");
      SerialUSB.print((0.6 * Ku), 4);
      SerialUSB.print(" | ");
      SerialUSB.print(((1.2 * Ku) / (Tu * FPID)), 4);
      SerialUSB.print(" | ");
      SerialUSB.print(((0.6 * Ku * Tu * FPID) / 8.0) , 4);
      SerialUSB.print(" |");

      SerialUSB.println("");
    }
    delay(500);
    k++;
  }


  // we are finished

  if (!gui) {
    SerialUSB.println("|-----------------------------------------------------------------|");
    SerialUSB.println();
    SerialUSB.println();
  }

  Kp = temp_Kp / loops;
  Ki = temp_Ki / loops;
  Kd = temp_Kd / loops;

  int_Kp = ((1024.0 * (temp_Kp / loops)) + 0.5);
  int_Ki = ((1024.0 * (temp_Ki / loops)) + 0.5);
  int_Kd = ((1024.0 * (temp_Kd / loops)) + 0.5);


  if (!gui) {
    SerialUSB.print("Kp = ");
    SerialUSB.println(Kp, 5);
    SerialUSB.print("Ki = ");
    SerialUSB.println(Ki, 5);
    SerialUSB.print("Kd = ");
    SerialUSB.println(Kd, 5);
  }
  if (gui) {
    paramter_streamer(false, true);
  }

  enableTC5Interrupts();
}






float digitalSmooth(int rawIn, int *sensSmoothArray) {    // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
  int j, k, temp, top, bottom;
  long total;
  static int i;
  static int sorted[filterSamples];
  boolean done;

  i = (i + 1) % filterSamples;    // increment counter and roll over if necc. -  % (modulo operator) rolls over variable
  sensSmoothArray[i] = rawIn;                 // input new data into the oldest slot

  for (j = 0; j < filterSamples; j++) { // transfer data array into anther array for sorting and averaging
    sorted[j] = sensSmoothArray[j];
  }

  done = 0;                // flag to know when we're done sorting
  while (done != 1) {      // simple swap sort, sorts numbers from lowest to highest
    done = 1;
    for (j = 0; j < (filterSamples - 1); j++) {
      if (sorted[j] > sorted[j + 1]) {    // numbers are out of order - swap
        temp = sorted[j + 1];
        sorted [j + 1] =  sorted[j] ;
        sorted [j] = temp;
        done = 0;
      }
    }
  }

  // throw out top and bottom 15% of samples - limit to throw out at least one from top and bottom
  bottom = max(((filterSamples * 15)  / 100), 1);
  top = min((((filterSamples * 85) / 100) + 1  ), (filterSamples - 1));   // the + 1 is to make up for asymmetry caused by integer rounding
  k = 0;
  total = 0;
  for ( j = bottom; j < top; j++) {
    total += sorted[j];  // total remaining indices
    k++;
  }

  return (float)total / (float)k;    // divide by number of samples
}



int measure_noise() {

  disableTC5Interrupts();


  delay(100);
  int counter = 0;
  float points[500] = {0};

  unsigned long now = micros();
  unsigned long last_time = now;
  int dt = ((1000000.0 / FPID) - 1);


  while (counter < 500) {
    now = micros();
    if (now > last_time + dt) {
      last_time = now;

      points[counter] = y;
      counter++;
    }
  }

  float mean = 0;
  for (int i = 0; i < 500; i++) {
    mean = mean + points[i];
  }
  mean = mean / 500.0;

  float upper = 0;
  float lower = 0;
  int upcounter = 0;
  int lowcounter = 0;

  for (int i = 0; i < 500; i++) {
    if (points[i] > mean ) {
      upcounter++;
      upper = upper + points[i];
    }
    else if (points[i] < mean) {
      lowcounter++;
      lower = lower + points[i];
    }
  }
  upper = upper / upcounter;
  lower = lower / lowcounter;

  float highest = mean;
  float lowest = mean;

  for (int i = 0; i < 500; i++) {
    if (points[i] > highest) {
      highest = points[i];
    }
    else if (points[i] < lowest) {
      lowest = points[i];
    }
  }

  enableTC5Interrupts();
  return abs((abs(highest) - abs(lowest)));

}



bool canceled() {
  if (SerialUSB.peek() == 'c') {
    SerialUSB.read();
    SerialUSB.read();
    SerialUSB.read();
    SerialUSB.println();
    SerialUSB.println("canceled!");
    enabled = false;
    error_register = 0B1000000000000000;
    enableTC5Interrupts();
    return true;
  }
  else {
    return false;
  }
}



int sign(int input) {
  if (input < 0) {
    return -1;
  }
  else {
    return 1;
  }
}



void error_led() {

  static unsigned int last_time;

  unsigned int current_time = millis();

  int error_count = 0;

  static int pattern_counter;
  static int errorcase;
  static int k = 1;
  static byte error_cases[5];


  // LED is switched with 10 Hz
  if (current_time - last_time > 100) {

    if (error_register & (1 << 3))   {
      //The lookup table has some failure
      error_cases[error_count] = 3;
      error_count += 1;
    }
    if (error_register & (1 << 2))   {
      //Maximal coil current reached
      error_cases[error_count] = 2;
      error_count += 1;
    }
    if (error_register & (1 << 1))   {
      //Maximal angular error exceeded
      error_cases[error_count] = 1;
      error_count += 1;
    }
    if (error_register & (1 << 0))   {
      //Timing error
      error_cases[error_count] = 0;
      error_count += 1;
    }




    if (pattern_counter >= 30) {


      pattern_counter = 0;

      if (k > (error_count)) {
        k = 1;
      }

      errorcase = error_cases[k - 1];

      k += 1;
    }


    if (error_count == 0) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else  {
      if (pattern_counter < 10) {
        if (LED_pattern[errorcase][pattern_counter]) {
          REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
        }
        else {
          REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
        }
      }
      else {
        REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
      }
    }

    pattern_counter += 1;

    last_time = current_time;
  }
}


float test_move(int steps, int F_Sample, bool output, char identifier) {

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  unsigned int dt = 1000000 / F_Sample;

  int counter = 0;

  int answer[1000];
  int target[1000];

  int o_answer[1000];
  int o_target[1000];



  enabled = true;

  // save the first value;
  target[0] = step_target;

  // calculate the target vector
  for (int i = 1; i < 1000; i++) {

    if (i <= 100) {
      target[i] = target[i - 1] + ((i * steps) / 100.0);
    }
    else if (100 < i && i <= 400) {
      // const velocity
      target[i] = target[i - 1] + steps;
    }
    else if (400 < i && i <= 500) {
      //slow down
      target[i] = target[i - 1] + (((500 - i) * steps) / 100.0);
    }
    else if (500 < i && i <= 600) {
      // accelerate
      target[i] = target[i - 1] - (((i - 500) * steps) / 100.0);
    }
    else if (600 < i && i < 900) {
      // const velocity
      target[i] = target[i - 1] - steps;
    }
    else {
      //slow down
      target[i] = target[i - 1] - (((1000 - i) * steps) / 100.0);
    }

  }

  while (counter < 1000) {
    current_time = micros();


    if (current_time >= next_time) {
      next_time = current_time + dt;

      answer[counter] = y;
      o_answer[counter] = omega;
      o_target[counter] = omega_target;
      step_target = target[counter];

      counter += 1;
    }
  }

  // canceling call if something goes wrong
  if (canceled()) {
    return -1;
  }

  for (int i = 0; i < 1000; i++) {
    target[i] = target[i] * stepangle;
  }


  //disable motor
  enabled = false;



  float error_omega = 0.0;
  for (int i = 0; i < 1000; i++) {
    error_omega = error_omega + ((o_target[i] - o_answer[i]) * (o_target[i] - o_answer[i])) ;
  }
  error_omega = error_omega;

  float error_position = 0.0;
  for (int i = 0; i < 1000; i++) {
    error_position = error_position + ((target[i] - answer[i]) * (target[i] - answer[i])) ;
  }
  error_position = error_position / 15.0;

  float cost = error_position + error_omega;


  delay(100);
  if (output) {
    for (int i = 0; i < 1000; i++) {
      SerialUSB.print(identifier);
      SerialUSB.print(',');
      SerialUSB.print(target[i]);
      SerialUSB.print(';');
      SerialUSB.print(answer[i]);
      SerialUSB.print(';');
      SerialUSB.print(o_target[i]);
      SerialUSB.print(';');
      SerialUSB.println(o_answer[i]);
    }
  }
  return (cost);
}



void downhill_simplex(int arg_cnt, char **args) {


  int F_Sample = return_float_argument(args, arg_cnt, "-f", 5000, 200, 10000);
  int velocity = return_float_argument(args, arg_cnt, "-v", 60, 10, 500);
  bool output = check_argument(args, arg_cnt, "-o");
  bool gui = check_argument(args, arg_cnt, "-gui");

  if (gui) {
    output = true;
  }

  if (!gui) {
    SerialUSB.println(downhill_header);
    SerialUSB.println(cancle_header);
    SerialUSB.println(" ");

    SerialUSB.print("Samplerate = ");
    SerialUSB.print(F_Sample);
    SerialUSB.println(" Hz");
    SerialUSB.print("velocity = ");
    SerialUSB.print(velocity);
    SerialUSB.println(" mm/s");
  }

  int steps = (velocity * steps_per_revolution * microstepping * step_add) / (mm_rev * F_Sample);

  if (Kff == 0.0) {
    Kff = 0.01;
  }
  if (Kacc == 0.0) {
    Kacc = 0.01;
  }

  int_Kp = (1024.0 * Kp) + 0.5;
  int_Ki = (1024.0 * Ki) + 0.5;
  int_Kd = (1024.0 * Kd) + 0.5;
  int_Kff = (Kff * 1024.0) + 0.5;
  int_Kacc = (Kacc * 1024.0) + 0.5;

  paramter_streamer(false, true);

  float init_jump = 1.2;
  float p_max = return_float_argument(args, arg_cnt, "-p", 2.0);
  float i_max = return_float_argument(args, arg_cnt, "-i", 0.5);
  float d_max = return_float_argument(args, arg_cnt, "-d", 8.0);
  float Kff_max = return_float_argument(args, arg_cnt, "-kf", 0.02);
  float Kacc_max = return_float_argument(args, arg_cnt, "-ka", 0.02);

  // generate startpoints
  float p[6][5] = {{Kp, Ki, Kd, Kff, Kacc},
    {init_jump * Kp, Ki, Kd, Kff, Kacc},
    {Kp, init_jump * Ki, Kd, Kff, Kacc},
    {Kp, Ki, init_jump * Kd, Kff, Kacc},
    {Kp, Ki, Kd, init_jump * Kff, Kacc},
    {Kp, Ki, Kd, Kff, init_jump * Kacc}
  };


  float alpha = 1.0;
  float gamma = 2.0;
  float beta = 0.5;
  float sigma = 0.5;

  float last_Kp = Kp;
  float last_Ki = Ki;
  float last_Kd = Kd;
  float last_Kff = Kff;
  float last_Kacc = Kacc;

  float cost[6] = {0, 0, 0, 0, 0, 0};

  bool changed[6] = {1, 1, 1, 1, 1, 1};

  byte worst_idx = 0;
  byte mid_idx = 0;
  byte best_idx = 0;

  int counter = 0;

  float r[5];
  float e[5];
  float c[5];

  bool  cancle_crit = 1;

  while (cancle_crit) {
    counter = counter + 1;

    // canceling call if something goes wrong
    if (canceled()) return;

    for (int i = 0; i < 6; i++) {
      if (changed[i] == 1) {
        changed[i] = 0;

        Kp = p[i][0];
        Ki = p[i][1];
        Kd = p[i][2];
        Kff = p[i][3];
        Kacc = p[i][4];

        int_Kp = (1024.0 * Kp) + 0.5;
        int_Ki = (1024.0 * Ki) + 0.5;
        int_Kd = (1024.0 * Kd) + 0.5;
        int_Kff = (Kff * 1024.0) + 0.5;
        int_Kacc = (Kacc * 1024.0) + 0.5;

        cost[i] = 0;
        for (byte j = 0; j < 4; j++) {
          if (j < 3) {
            cost[i] = cost[i] + test_move(steps, F_Sample, 0, 'a');
          }
          else {
            cost[i] = cost[i] + test_move(steps, F_Sample, output, 'a');
          }

          // canceling call if something goes wrong
          if (canceled()) return;
        }
        cost[i] = cost[i] / 4.0;

        if (gui) {
          paramter_streamer(false, true);
        }

      }
    }

    float min_cost = 100000;
    float max_cost = 0;

    for (int i = 0; i < 6; i++) {

      if (cost[i] < min_cost) {
        min_cost = cost[i];
        best_idx = i;
      }

      if (cost[i] > max_cost) {
        max_cost = cost[i];
        worst_idx = i;
      }

    }


    byte mid_idxs[4] = {0, 0, 0, 0};
    byte mid_counter = 0;
    for (int i = 0; i < 6; i++) {

      if (i != best_idx && i != worst_idx) {
        mid_idxs[i] = i;

        mid_counter++;
      }
    }

    int best_mid_cost = 1000000;

    for (int i = 0; i > 4; i++) {
      if (cost[mid_idxs[i]] < best_mid_cost) {
        best_mid_cost = cost[mid_idxs[i]];
        mid_idx = mid_idxs[i];
      }

    }


    float p_sum = 0;
    float i_sum = 0;
    float d_sum = 0;
    float Kff_sum = 0;
    float Kacc_sum = 0;

    for (int i = 0; i < 6; i++) {
      if (i != worst_idx) {
        p_sum = p_sum + p[i][0];
        i_sum = i_sum + p[i][1];
        d_sum = d_sum + p[i][2];
        Kff_sum = Kff_sum + p[i][3];
        Kacc_sum = Kacc_sum + p[i][4];
      }
    }
    p_sum = p_sum / 5.0;
    i_sum = i_sum / 5.0;
    d_sum = d_sum / 5.0;
    Kff_sum = Kff_sum / 5.0;
    Kacc_sum = Kacc_sum / 5.0;


    r[0] = (1 + alpha) * p_sum - alpha * p[worst_idx][0];
    r[1] = (1 + alpha) * i_sum - alpha * p[worst_idx][1];
    r[2] = (1 + alpha) * d_sum - alpha * p[worst_idx][2];
    r[3] = (1 + alpha) * Kff_sum - alpha * p[worst_idx][3];
    r[4] = (1 + alpha) * Kacc_sum - alpha * p[worst_idx][4];

    r[0] = constrain(r[0], 0, p_max);
    r[1] = constrain(r[1], 0, i_max);
    r[2] = constrain(r[2], 0, d_max);
    r[3] = constrain(r[3], 0, Kff_max);
    r[4] = constrain(r[4], 0, Kacc_max);

    Kp = r[0];
    Ki = r[1];
    Kd = r[2];
    Kff = r[3];
    Kacc = r[4];

    int_Kp = (1024.0 * Kp) + 0.5;
    int_Ki = (1024.0 * Ki) + 0.5;
    int_Kd = (1024.0 * Kd) + 0.5;
    int_Kff = (Kff * 1024.0) + 0.5;
    int_Kacc = (Kacc * 1024.0) + 0.5;

    float temp_Cost_r = 0;

    for (byte j = 0; j < 4; j++) {

      if (j < 3) {
        temp_Cost_r = temp_Cost_r + test_move(steps, F_Sample, 0, 'a');
      }
      else {
        temp_Cost_r = temp_Cost_r +  test_move(steps, F_Sample, output, 'a');
      }

      // canceling call if something goes wrong
      if (canceled()) return;
    }
    temp_Cost_r = temp_Cost_r / 4.0;

    if (gui) {
      paramter_streamer(false, true);
    }


    if (temp_Cost_r < cost[best_idx]) {
      e[0] = (1 + gamma) * p_sum - gamma * p[worst_idx][0];
      e[1] = (1 + gamma) * i_sum - gamma * p[worst_idx][1];
      e[2] = (1 + gamma) * d_sum - gamma * p[worst_idx][2];
      e[3] = (1 + gamma) * Kff_sum - gamma * p[worst_idx][3];
      e[4] = (1 + gamma) * Kacc_sum - gamma * p[worst_idx][4];

      e[0] = constrain(e[0], 0, p_max);
      e[1] = constrain(e[1], 0, i_max);
      e[2] = constrain(e[2], 0, d_max);
      e[3] = constrain(e[3], 0, Kff_max);
      e[4] = constrain(e[4], 0, Kacc_max);

      Kp = e[0];
      Ki = e[1];
      Kd = e[2];
      Kff = e[3];
      Kacc = e[4];


      int_Kp = (1024.0 * Kp) + 0.5;
      int_Ki = (1024.0 * Ki) + 0.5;
      int_Kd = (1024.0 * Kd) + 0.5;
      int_Kff = (Kff * 1024.0) + 0.5;
      int_Kacc = (Kacc * 1024.0) + 0.5;

      float temp_Cost_e = 0;
      for (byte j = 0; j < 4; j++) {
        if (j < 3) {
          temp_Cost_e = temp_Cost_e + test_move(steps, F_Sample, 0, 'a');
        }
        else {
          temp_Cost_e = temp_Cost_e +  test_move(steps, F_Sample, output, 'a');
        }

        // canceling call if something goes wrong
        if (canceled()) return;
      }
      temp_Cost_e = temp_Cost_e / 4.0;

      if (gui) {
        paramter_streamer(false, true);
      }

      if (temp_Cost_e < temp_Cost_r) {

        p[worst_idx][0] = e[0];
        p[worst_idx][1] = e[1];
        p[worst_idx][2] = e[2];
        p[worst_idx][3] = e[3];
        p[worst_idx][4] = e[4];

        changed[worst_idx] = 1;

      }
      else {
        p[worst_idx][0] = r[0];
        p[worst_idx][1] = r[1];
        p[worst_idx][2] = r[2];
        p[worst_idx][3] = r[3];
        p[worst_idx][4] = r[4];

        changed[worst_idx] = 1;
      }

    }
    else {


      if (temp_Cost_r < cost[mid_idx]) {

        p[worst_idx][0] = r[0];
        p[worst_idx][1] = r[1];
        p[worst_idx][2] = r[2];
        p[worst_idx][3] = r[3];
        p[worst_idx][4] = r[4];

        changed[worst_idx] = 1;

      }
      else {

        if (temp_Cost_r < cost[worst_idx]) {
          c[0] = beta * p_sum + (1 - beta) * r[0];
          c[1] = beta * i_sum + (1 - beta) * r[1];
          c[2] = beta * d_sum + (1 - beta) * r[2];
          c[3] = beta * Kff_sum + (1 - beta) * r[3];
          c[4] = beta * Kacc_sum + (1 - beta) * r[4];
        }
        else {
          c[0] = beta * p_sum + (1 - beta) * p[worst_idx][0];
          c[1] = beta * i_sum + (1 - beta) * p[worst_idx][1];
          c[2] = beta * d_sum + (1 - beta) * p[worst_idx][2];
          c[3] = beta * Kff_sum + (1 - beta) * p[worst_idx][3];
          c[4] = beta * Kacc_sum + (1 - beta) * p[worst_idx][4];
        }

        c[0] = constrain(c[0], 0, p_max);
        c[1] = constrain(c[1], 0, i_max);
        c[2] = constrain(c[2], 0, d_max);
        c[3] = constrain(c[3], 0, Kff_max);
        c[4] = constrain(c[4], 0, Kacc_max);

        Kp = c[0];
        Ki = c[1];
        Kd = c[2];
        Kff = c[3];
        Kacc = c[4];


        int_Kp = (1024.0 * Kp) + 0.5;
        int_Ki = (1024.0 * Ki) + 0.5;
        int_Kd = (1024.0 * Kd) + 0.5;
        int_Kff = (Kff * 1024.0) + 0.5;
        int_Kacc = (Kacc * 1024.0) + 0.5;

        float temp_Cost_c = 0;
        for (byte j = 0; j < 4; j++) {
          if (j < 3) {
            temp_Cost_c = temp_Cost_c +  test_move(steps, F_Sample, 0, 'a');
          }
          else {
            temp_Cost_c = temp_Cost_c +  test_move(steps, F_Sample, output, 'a');
          }

          // canceling call if something goes wrong
          if (canceled()) return;
        }
        temp_Cost_c = temp_Cost_c / 4.0;

        if (gui) {
          paramter_streamer(false, true);
        }

        if (temp_Cost_c < cost[worst_idx]) {

          p[worst_idx][0] = c[0];
          p[worst_idx][1] = c[1];
          p[worst_idx][2] = c[2];
          p[worst_idx][3] = c[3];
          p[worst_idx][4] = c[4];

          changed[worst_idx] = 1;
        }
        else {

          for (int i = 0; i < 6; i++) {
            if (i != best_idx) {
              p[i][0] = sigma * p[best_idx][0] + (1 - sigma) * p[i][0];
              p[i][1] = sigma * p[best_idx][1] + (1 - sigma) * p[i][1];
              p[i][2] = sigma * p[best_idx][2] + (1 - sigma) * p[i][2];
              p[i][3] = sigma * p[best_idx][3] + (1 - sigma) * p[i][3];
              p[i][4] = sigma * p[best_idx][4] + (1 - sigma) * p[i][4];

              changed[i] = 1;
            }
          }

        }

      }

    }

    float dKp = abs(Kp - last_Kp) * 1024.0;
    float dKi = abs(Ki - last_Ki) * 1024.0;
    float dKd = abs(Kd - last_Kd) * 1024.0;
    float dKff = abs(Kff - last_Kff) * 1024.0;
    float dKacc = abs(Kacc - last_Kacc) * 1024.0;

    cancle_crit = (dKp > 1.0) || (dKi > 1.0) || (dKd > 1.0) || (dKff > 1.0) || (dKacc > 1.0);

    last_Kp = Kp;
    last_Ki = Ki;
    last_Kd = Kd;
    last_Kff = Kff;
    last_Kacc = Kacc;

  }


  // we are ready search the best settings of all and set them
  float min_cost = 1000000;
  for (int i = 0; i < 4; i++) {

    if (cost[i] < min_cost) {
      min_cost = cost[i];
      best_idx = i;
    }
  }

  Kp = p[best_idx][0];
  Ki = p[best_idx][1];
  Kd = p[best_idx][2];
  Kff = p[best_idx][3];
  Kacc = p[best_idx][4];

  int_Kp = (1024.0 * Kp) + 0.5;
  int_Ki = (1024.0 * Ki) + 0.5;
  int_Kd = (1024.0 * Kd) + 0.5;
  int_Kff = (Kff * 1024.0) + 0.5;
  int_Kacc = (Kacc * 1024.0) + 0.5;


  if (!gui) {
    paramter_streamer (true, false);
  } else {
    paramter_streamer(false, true);
  }


}


void paramter_streamer (bool output, bool gui) {

  if (output) {
    SerialUSB.print("identifier = ");
    SerialUSB.println(identifier);

    SerialUSB.print("steps_per_revolution = ");
    SerialUSB.println(steps_per_revolution);

    SerialUSB.print("microstepping = ");
    SerialUSB.println(microstepping);

    SerialUSB.print("iMAX = ");
    SerialUSB.println(iMAX);

    SerialUSB.print("error_led_value = ");
    SerialUSB.println(error_led_value);

    SerialUSB.print("M_max = ");
    SerialUSB.println(M_max);

    SerialUSB.print("I_rated = ");
    SerialUSB.println(I_rated);

    SerialUSB.print("Kp = ");
    SerialUSB.println(Kp, 5);

    SerialUSB.print("Ki = ");
    SerialUSB.println(Ki, 5);

    SerialUSB.print("Kd = ");
    SerialUSB.println(Kd, 5);

    SerialUSB.print("Kff = ");
    SerialUSB.println(Kff, 5);

    SerialUSB.print("Kacc = ");
    SerialUSB.println(Kacc, 5);

    SerialUSB.print("error_LPF = ");
    SerialUSB.println(error_LPF);

    SerialUSB.print("mm_rev = ");
    SerialUSB.println(mm_rev);

    SerialUSB.print("USE_ENABLE_PIN = ");
    SerialUSB.println(USE_ENABLE_PIN);

    SerialUSB.print("INVERT = ");
    SerialUSB.println(INVERT);
  }

  if (gui) {
    SerialUSB.print('p');
    SerialUSB.write(',');
    SerialUSB.print(identifier);
    SerialUSB.write(';');
    SerialUSB.print(steps_per_revolution);
    SerialUSB.write(';');
    SerialUSB.print(microstepping);
    SerialUSB.write(';');
    SerialUSB.print(iMAX);
    SerialUSB.write(';');
    SerialUSB.print(M_max);
    SerialUSB.write(';');
    SerialUSB.print(I_rated);
    SerialUSB.write(';');
    SerialUSB.print(Kp, 5);
    SerialUSB.write(';');
    SerialUSB.print(Ki, 5);
    SerialUSB.write(';');
    SerialUSB.print(Kd, 5);
    SerialUSB.write(';');
    SerialUSB.print(error_LPF);
    SerialUSB.write(';');
    SerialUSB.print(mm_rev);
    SerialUSB.write(';');
    SerialUSB.print(error_led_value);
    SerialUSB.write(';');
    SerialUSB.print(USE_ENABLE_PIN);
    SerialUSB.write(';');
    SerialUSB.print(INVERT);
    SerialUSB.write(';');
    SerialUSB.print(Kff, 5);
    SerialUSB.write(';');
    SerialUSB.print(Kacc, 5);
    SerialUSB.println();
  }
}


