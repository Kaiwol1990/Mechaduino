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
#include "lookup_table.h"
#include "board.h"
#include "Serial.h"
#include "Language.h"

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


void calibration() {

  disableTC5Interrupts();
  disableTC4Interrupts();

  int avg = 100;


  SerialUSB.println(calibrate_header);
  enabled = false;


  float encoderReading = 0;

  for (int i = 0; i < 50; i++) {
    encoderReading = encoderReading + readEncoder();
  }
  encoderReading = encoderReading / 50.0;


  SerialUSB.println("searching zeropoint");
  step_target = 0;
  if (encoderReading < (16384 / steps_per_revolution)) {
    dir = true;
    while (readEncoder() < (16384 / steps_per_revolution)) {
      oneStep();
      delay(100);
    }
  }


  dir = true;
  step_target = 0;

  oneStep();
  delay(100);
  oneStep();


  float temp_pos = 0;
  for (int i = 0; i < 50; i++) {
    temp_pos = temp_pos + readEncoder();
  }
  temp_pos = temp_pos / 50.0;


  if ((temp_pos - encoderReading) > 0)
  {
    SerialUSB.println("Wired backwards");
    enableTC5Interrupts();
    return;
  }


  dir = false;
  step_target = 0;

  oneStep();
  delay(100);
  oneStep();
  dir = true;
  step_target = 0;

  SerialUSB.println("Calibrating fullsteps");
  SerialUSB.println(procent_bar);
  int counter = 0;
  int count = steps_per_revolution / 50;
  dir = true;

  int readings[steps_per_revolution];

  int temp_reading = 4 * readEncoder();
  int last_reading = temp_reading;

  // step to every single fullstep position and read the Encoder
  for (int i = 0; i < steps_per_revolution; i++) {

    if (canceled()) return;
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


    readings[i] =  (encoderReading / avg) + 0.5;

    if (readings[i] > 65536) {
      readings[i] = readings[i] - 65536;
    }

    if (i >= 1) {
      if (abs(readings[i] - readings[i - 1]) < (65536 / (1.1 * steps_per_revolution))) {
        SerialUSB.println("Motor is not moving -> abbort");
        return;
      }
    }

    if (counter == count) {
      counter = 0;
      SerialUSB.print(".");
    }

    oneStep();
  }


  dir = false;
  oneStep();
  delay(100);
  dir = true;

  SerialUSB.println();
  SerialUSB.println();


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
  int x[steps_per_revolution];
  for ( int i = 0; i < steps_per_revolution; i++) {
    counter = i - idx;
    if (counter < 0) {
      counter = counter + steps_per_revolution;
    }
    x[i] = (angle_sorted[counter] + 0.5);
  }

  //  calibration check
  SerialUSB.println("checking fullsteps");
  SerialUSB.println(procent_bar);
  counter = 0;
  count = (steps_per_revolution) / 50;
  float smoothed_error = 0.0;


  temp_reading = 4 * readEncoder();
  last_reading = temp_reading;
  for (int i = 0; i < steps_per_revolution; i++) {

    if (canceled()) return;
    counter += 1;

    delay(100);

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {

      temp_reading = 4 * readEncoder();
      if ((temp_reading - last_reading) < -32768) {
        temp_reading += 65536;
      }
      else if ((temp_reading - last_reading) > 32768) {
        temp_reading -= 65536;
      }
      last_reading = temp_reading;

      encoderReading += temp_reading;

      delayMicroseconds(100);
    }

    float temp =  abs(x[i] - (encoderReading / avg));

 
    if (temp > smoothed_error) {
      smoothed_error = temp;
    }

    if (counter == count) {
      counter = 0;
      SerialUSB.print(".");
    }
    oneStep();
  }


  output(0, 0);
  step_target = 0;

  SerialUSB.println();
  SerialUSB.print("maximal error = ");
  SerialUSB.print((100.0 * smoothed_error) / 65536.0);
  SerialUSB.println("%");
  SerialUSB.println("should be lower than 0.5 %");
  SerialUSB.println("");




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

    ticks[i] = x[high] - x[low];

    if (ticks[i] < 0) {
      ticks[i] += 65536;
    }
    else if (ticks[i] > 32768) {
      ticks[i] -= 65536;
    }

    for (int j = 0; j < ticks[i]; j++) {
      stepNo = (mod(x[i] + j, 65536));
      if (stepNo == 0) {
        iStart = i;
        jStart = j;
      }
    }
  }


  // calculate the lookup table on the fly
  counter = 0;
  float angle_per_step = 36000.0 / steps_per_revolution;
  float lookupAngle;
  float tick;

  SerialUSB.print("const unsigned int lookup[] = {");

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
    step_target += 1024;
  }
  else {
    step_target -= 1024;
  }

  int target_raw = step_target * stepangle;
  int raw_0 = mod(target_raw, 36000);
  output(raw_0 , uMAX / 2);
}





int mod(int xMod, int mMod) {
  return (xMod % mMod + mMod) % mMod;
}





void setupTCInterrupts() {
  const int overflow_TC_5 = (48000000 / FPID) - 1;
  const int overflow_TC_4 = ((48000000 / (2 * FPID))) - 1;

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


void antiCoggingCal() {

  bool last_enabled = enabled;

  float temp_u_cogging = 0;


  SerialUSB.println("//---- Calculating friciton ----");

  step_target = 0;

  enabled = true;

  step_target = 0;

  delay(500);

  for (int i = 0; i < 3600; i++) {

    step_target += (10 / stepangle);

    delay(10);

    float sum = 0;
    for (int k = 0; k < 50; k++) {
      sum = sum + u;
      delayMicroseconds(20);
    }

    temp_u_cogging += (sum / 50.0);
  }


  temp_u_cogging = temp_u_cogging / 3600;

  enabled = last_enabled;

  SerialUSB.print("int u_cogging = ");
  SerialUSB.print((int)(temp_u_cogging + 0.5));

  SerialUSB.println(";");
  SerialUSB.println();
}






void PID_autotune() {

  disableTC5Interrupts();


  int loops = 0;
  float outputStep = uMAX;
  int frequency = 2 * FPID;
  int dt = (1000000 / frequency);
  int scan_dt = dt - 2;

  float temp_Kp = 0;
  float temp_Ki = 0;
  float temp_Kd = 0;

  bool received = false;
  bool debug = false;
  unsigned long now = millis();
  int k = 1;


  SerialUSB.println(autotune_header);
  SerialUSB.println(cancle_header);



  SerialUSB.print("The motor will vibrate violently, continue? (y/n) = ");
  while (!received) {

    if (timed_out(now, 5000)) return;

    if (SerialUSB.available()) {

      if (SerialUSB.read() == 'n') {

        SerialUSB.println('n');
        while (SerialUSB.available()) {
          SerialUSB.read();
        }
        return;
      }

      received = true;
    }
  }
  SerialUSB.println('y');


  // flsuh the serial port
  while (SerialUSB.available()) {
    SerialUSB.read();
  }


  SerialUSB.print("Cycles = ");
  received = false;
  now = millis();


  while (!received) {
    if (canceled()) return;

    if (timed_out(now, 5000)) return;

    if (SerialUSB.available()) {

      if (SerialUSB.peek() == 'd') {
        SerialUSB.println("");
        SerialUSB.println("debug mode!");
        debug = true;
      }

      loops = SerialUSB.parseInt();

      SerialUSB.println(loops);
      SerialUSB.println("");

      received = true;
    }
  }

  // flsuh the serial port
  while (SerialUSB.available()) {
    SerialUSB.read();
  }

  SerialUSB.println("| loop | Noise | Frequency | lookback | P      | I      | D       |");


  // start autotune
  while (k <= loops) {

    SerialUSB.println("|-----------------------------------------------------------------|");
    SerialUSB.print("|   ");
    SerialUSB.print(k);
    if (k >= 10) {
      SerialUSB.print(" ");
    }
    else {
      SerialUSB.print("  ");
    }

    delay(200);

    // measure current setpoint
    int setpoint = measure_setpoint() + 0.5;

    // measure the noise from the controller
    int noiseBand = measure_noise(false);
    disableTC5Interrupts();

    SerialUSB.print("|   ");
    SerialUSB.print(noiseBand);
    if (noiseBand >= 10) {
      SerialUSB.print("  ");
    }
    else {
      SerialUSB.print("   ");
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
          output(-raw_0 - PA, u);
        }
        else {
          output(-raw_0 + PA, u);
        }

        // wait half a second to get stable oscilations
        if (millis() > start_millis) {
          points[counter] = y;
          counter++;
        }

        last_time = now;

      }
    }

    output(-raw_0, 0);


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

    // calculating lookback points
    int nLookBack = ((1000000 * Tu) / dt) / 4;
    if (nLookBack >= 99) {
      nLookBack = 99;
    }
    SerialUSB.print("|   ");
    SerialUSB.print(nLookBack);
    SerialUSB.print("     ");


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

    SerialUSB.print("| ");
    SerialUSB.print((0.6 * Ku), 4);
    SerialUSB.print(" | ");
    SerialUSB.print(((1.2 * Ku) / (Tu * FPID)), 4);
    SerialUSB.print(" | ");
    SerialUSB.print(((0.6 * Ku * Tu * FPID) / 8.0) , 4);
    SerialUSB.print(" |");

    SerialUSB.println("");
    delay(500);
    k++;
  }


  // we are finished

  SerialUSB.println("|-----------------------------------------------------------------|");
  SerialUSB.println();
  SerialUSB.println();


  Kp = temp_Kp / loops;
  Ki = temp_Ki / loops;
  Kd = temp_Kd / loops;

  int_Kp = ((1024.0 * (temp_Kp / loops)) + 0.5);
  int_Ki = ((1024.0 * (temp_Ki / loops)) + 0.5);
  int_Kd = ((1024.0 * (temp_Kd / loops)) + 0.5);


  SerialUSB.print("Kp = ");
  SerialUSB.println(Kp, 5);
  SerialUSB.print("Ki = ");
  SerialUSB.println(Ki, 5);
  SerialUSB.print("Kd = ");
  SerialUSB.println(Kd, 5);

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




bool canceled() {
  if (SerialUSB.peek() == 'c') {
    SerialUSB.read();
    SerialUSB.read();
    SerialUSB.read();
    SerialUSB.println();
    SerialUSB.println("canceled!");
    enableTC5Interrupts();
    return true;
  }
  else {
    return false;
  }
}




bool timed_out(unsigned long now, int time_out) {
  if (millis() > now + time_out) {
    SerialUSB.println("");
    SerialUSB.println("timed out!");
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



void boot() {
  SerialUSB.begin(baudrate);

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

  SerialUSB.print("checking lookup table:");
  bool error = check_lookup(false);
  if (error) {
    SerialUSB.println(" ERROR! The lookup table has some failure! s} \" check \" to get further informations!");
  }
  else {
    SerialUSB.println(" OK");
  }


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

  Serial_menu();

  SerialUSB.println("");
  SerialUSB.println(":>");

}



bool check_lookup(bool output) {

  if (output) {
    SerialUSB.println("//---- checking lookup table ----");
    SerialUSB.println("");
  }

  int minimal = 36000;
  int maximal = 0;
  int max_dx = 0;
  int dx = 0;

  int last_temp = pgm_read_word_near(lookup + 16383);
  // int temp = 0;
  bool error = false;


  for (int i = 0; i < 16384; i++) {
    int temp = pgm_read_word_near(lookup + i);

    if (temp < minimal) {
      minimal = temp;
    }
    else if (temp > maximal) {
      maximal = temp;
    }

    dx = temp - last_temp;

    if (dx < -18000) {
      dx = dx + 36000;
    }

    if (dx > max_dx) {
      max_dx = dx;
    }

    last_temp = temp;
  }

  if (output) {
    SerialUSB.print("delta value: ");
    SerialUSB.println(max_dx);

    SerialUSB.print("minimal value: ");
    SerialUSB.println(minimal);

    SerialUSB.print("maximal value: ");
    SerialUSB.println(maximal);

    SerialUSB.println("");
  }

  if (minimal > 3) {
    if (output) {
      SerialUSB.println("minimal value to high");
    }
    error = true;
    error_register = error_register | 0B0000000000001000;
  }

  if (minimal < 0) {
    if (output) {
      SerialUSB.println("minimal value to low");
    }
    error = true;
    error_register = error_register | 0B0000000000001000;
  }

  if (maximal > 36000) {
    if (output) {
      SerialUSB.println("maximal value to high");
    }
    error = true;
    error_register = error_register | 0B0000000000001000;
  }

  if (maximal < 35995) {
    if (output) {
      SerialUSB.println("maximal value to low");
    }
    error = true;
    error_register = error_register | 0B0000000000001000;
  }

  if (max_dx > 5) {
    if (output) {
      SerialUSB.println("step between elements to high");
    }
    error = true;
    error_register = error_register | 0B0000000000001000;
  }

  if (max_dx < 1) {
    if (output) {
      SerialUSB.println("step between elements to low");
    }
    error = true;
    error_register = error_register | 0B0000000000001000;
  }


  if (output && !error) {

    SerialUSB.println("");
    SerialUSB.println("Looks good!");
  }

  return error;
}

//                            timing error                    maximal error                   maximal current                       lookuptable
bool LED_pattern[4][10] = { {1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 0, 1}};

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


void twiddle() {
  int velocity = 80;
  int F_Sample = 5000;

  static float best_err;

  int i = 0;

  Kff = 0.0;
  int_Kff = 0;
  Kacc = 0;
  int_Kacc = 0;


  double delta_parameter[5] = {0.40,  0.025,  0.750, 0.01, 0.01};


  float threshold  = 0.05;

  float sum_dp = delta_parameter[0] + delta_parameter[1] + delta_parameter[2] + delta_parameter[3] + delta_parameter[4];


  // calculate step increment to reach the desired speed
  int step_add_twiddle = (velocity * steps_per_revolution * microstepping * microstepping) / (mm_rev * F_Sample);


  //start twiddle algorithm
  SerialUSB.println(twiddle_header);
  SerialUSB.println(cancle_header);
  SerialUSB.println(" ");
  SerialUSB.println("| Kp     | Ki     | Kd     | Kff    | Kacc   | objective  |");
  SerialUSB.println("|---------------------------------------------------------|");


  enabled = true;

  best_err = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;
  float twiddle_error = best_err;


  SerialUSB.print("| ");
  SerialUSB.print(Kp, 4);
  SerialUSB.print(" | ");
  SerialUSB.print(Ki, 4);
  SerialUSB.print(" | ");
  SerialUSB.print(Kd, 4);
  SerialUSB.print(" | ");
  SerialUSB.print(Kff, 4);
  SerialUSB.print(" | ");
  SerialUSB.print(Kacc, 4);
  SerialUSB.print(" | ");
  SerialUSB.print(twiddle_error, 4);
  SerialUSB.println(" |");


  // starting twiddle algorithm
  while (sum_dp > threshold ) {


    // canceling call if something goes wrong
    if (canceled()) return;

    switch (i) {
      // change Kp
      case 0:
        if (delta_parameter[i] * 1024.0 >= 1.0) {
          Kp = Kp + delta_parameter[i];
          Kp = constrain(Kp, 0, Kp);
          int_Kp = (1024.0 * Kp) + 0.5;

          // make a step response
          twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

          SerialUSB.print("| ");
          SerialUSB.print(Kp, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Ki, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kd, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kff, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kacc, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(twiddle_error, 4);
          SerialUSB.println(" |");

          if (twiddle_error < best_err) {
            best_err = twiddle_error;
            delta_parameter[0] = 1.2 * delta_parameter[0];
          }
          else {
            Kp = Kp - 2 * delta_parameter[i];
            Kp = constrain(Kp, 0, Kp);
            int_Kp = (1024.0 * Kp) + 0.5;

            twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

            SerialUSB.print("| ");
            SerialUSB.print(Kp, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Ki, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kd, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kff, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kacc, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(twiddle_error, 4);
            SerialUSB.println(" |");

            if (twiddle_error < best_err) {
              best_err = twiddle_error;
              delta_parameter[0] = 1.1 * delta_parameter[0];
            }
            else {
              Kp = Kp + delta_parameter[i];
              Kp = constrain(Kp, 0, Kp);
              int_Kp = (1024.0 * Kp) + 0.5;
              delta_parameter[i] = 0.9 * delta_parameter[i];
            }
          }
        }
        else {
          delta_parameter[i] = 0.0;
        }

        break;

      // change Ki
      case 1:
        if (delta_parameter[i] * 1024.0 >= 1.0) {
          Ki = Ki + delta_parameter[i];
          Ki = constrain(Ki, 0, Ki);
          int_Ki = (1024.0 * Ki) + 0.5;

          // make a step response
          twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

          SerialUSB.print("| ");
          SerialUSB.print(Kp, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Ki, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kd, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kff, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kacc, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(twiddle_error, 4);
          SerialUSB.println(" |");

          if (twiddle_error < best_err) {
            best_err = twiddle_error;
            delta_parameter[1] = 1.2 * delta_parameter[1];
          }
          else {
            Ki = Ki - 2 * delta_parameter[i];
            Ki = constrain(Ki, 0, Ki);
            int_Ki = (1024.0 * Ki) + 0.5;

            twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

            SerialUSB.print("| ");
            SerialUSB.print(Kp, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Ki, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kd, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kff, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kacc, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(twiddle_error, 4);
            SerialUSB.println(" |");

            if (twiddle_error < best_err) {
              best_err = twiddle_error;
              delta_parameter[1] = 1.1 * delta_parameter[1];
            }
            else {
              Ki = Ki + delta_parameter[i];
              Ki = constrain(Ki, 0, Ki);
              int_Ki = (1024.0 * Ki) + 0.5;

              delta_parameter[i] = 0.90 * delta_parameter[i];
            }
          }
        }
        else {
          delta_parameter[i] = 0.0;
        }
        break;

      // change Kd
      case 2:
        if (delta_parameter[i] * 1024.0 >= 1.0) {
          Kd = Kd + delta_parameter[i];
          Kd = constrain(Kd, 0, Kd);
          int_Kd = (1024.0 * Kd) + 0.5;

          // make a step response
          twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;


          SerialUSB.print("| ");
          SerialUSB.print(Kp, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Ki, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kd, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kff, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kacc, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(twiddle_error, 4);
          SerialUSB.println(" |");

          if (twiddle_error < best_err) {
            best_err = twiddle_error;
            delta_parameter[2] = 1.2 * delta_parameter[2];
          }
          else {
            Kd = Kd - 2 * delta_parameter[i];
            Kd = constrain(Kd, 0, Kd);
            int_Kd = (1024.0 * Kd) + 0.5;

            twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;


            SerialUSB.print("| ");
            SerialUSB.print(Kp, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Ki, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kd, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kff, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kacc, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(twiddle_error, 4);
            SerialUSB.println(" |");

            if (twiddle_error < best_err) {
              best_err = twiddle_error;
              delta_parameter[2] = 1.1 * delta_parameter[2];
            }
            else {
              Kd = Kd + delta_parameter[i];
              Kd = constrain(Kd, 0, Kd);
              int_Kd = (1024.0 * Kd) + 0.5;

              delta_parameter[i] = 0.9 * delta_parameter[i];
            }
          }
        }
        else {
          delta_parameter[i] = 0.0;
        }
        break;

      // change Kff
      case 3:
        if (delta_parameter[i] * 1024.0 >= 1.0) {
          Kff = Kff + delta_parameter[i];
          Kff = constrain(Kff, 0, Kff);
          int_Kff = (1024.0 * Kff) + 0.5;

          twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

          SerialUSB.print("| ");
          SerialUSB.print(Kp, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Ki, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kd, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kff, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kacc, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(twiddle_error, 4);
          SerialUSB.println(" |");

          if (twiddle_error < best_err) {
            best_err = twiddle_error;
            delta_parameter[2] = 1.2 * delta_parameter[2];
          }
          else {
            Kff = Kff - 2 * delta_parameter[i];
            Kff = constrain(Kff, 0, Kff);
            int_Kff = (1024.0 * Kff) + 0.5;

            twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

            SerialUSB.print("| ");
            SerialUSB.print(Kp, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Ki, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kd, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kff, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kacc, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(twiddle_error, 4);
            SerialUSB.println(" |");

            if (twiddle_error < best_err) {
              best_err = twiddle_error;
              delta_parameter[2] = 1.1 * delta_parameter[2];
            }
            else {
              Kff = Kff + delta_parameter[i];
              Kff = constrain(Kff, 0, Kff);
              int_Kff = (1024.0 * Kff) + 0.5;

              delta_parameter[i] = 0.9 * delta_parameter[i];
            }
          }
        }
        else {
          delta_parameter[i] = 0.0;
        }
        break;

      case 4:
        if (delta_parameter[i] * 1024.0 >= 1.0) {
          Kacc = Kacc + delta_parameter[i];
          Kacc = constrain(Kacc, 0, Kacc);
          int_Kacc = (1024.0 * Kacc) + 0.5;

          twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

          SerialUSB.print("| ");
          SerialUSB.print(Kp, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Ki, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kd, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kff, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(Kacc, 4);
          SerialUSB.print(" | ");
          SerialUSB.print(twiddle_error, 4);
          SerialUSB.println(" |");

          if (twiddle_error < best_err) {
            best_err = twiddle_error;
            delta_parameter[2] = 1.2 * delta_parameter[2];
          }
          else {
            Kacc = Kacc - 2 * delta_parameter[i];
            Kacc = constrain(Kacc, 0, Kacc);
            int_Kacc = (1024.0 * Kacc) + 0.5;

            twiddle_error = (test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample) + test_move(step_add_twiddle, F_Sample)) / 4.0;

            SerialUSB.print("| ");
            SerialUSB.print(Kp, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Ki, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kd, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kff, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(Kacc, 4);
            SerialUSB.print(" | ");
            SerialUSB.print(twiddle_error, 4);
            SerialUSB.println(" |");

            if (twiddle_error < best_err) {
              best_err = twiddle_error;
              delta_parameter[2] = 1.1 * delta_parameter[2];
            }
            else {
              Kacc = Kacc + delta_parameter[i];
              Kacc = constrain(Kacc, 0, Kacc);
              int_Kacc = (1024.0 * Kacc) + 0.5;

              delta_parameter[i] = 0.9 * delta_parameter[i];
            }
          }
        }
        else {
          delta_parameter[i] = 0.0;
        }
        break;
    }

    sum_dp = delta_parameter[0] + delta_parameter[1] + delta_parameter[2] + delta_parameter[3] + delta_parameter[4];


    i++;
    if (i > 4) {
      i = 0;
    }


  }



  // we are finished there is no improvment possible
  int_pessen_Kp = (1024.0 * ((Kp * 0.7) / (0.6))) + 0.5;
  int_pessen_Ki = (1024.0 * ((Ki * 0.7 * 2.5) / (2 * 0.6))) + 0.5;
  int_pessen_Kd = (1024.0 * ((Kd * 8.0 * 0.7) / (20.0 * 0.6))) + 0.5;



  SerialUSB.println("|-----------------------------------------------------------|");
  SerialUSB.println("");
  SerialUSB.print("Convergence reached for v = ");
  SerialUSB.print(velocity);
  SerialUSB.println("[mm/s].");


  SerialUSB.println("");
  SerialUSB.print("float Kp = ");
  SerialUSB.print(Kp, 5);
  SerialUSB.println(";");
  SerialUSB.print("float Ki = ");
  SerialUSB.print(Ki, 5);
  SerialUSB.println(";");
  SerialUSB.print("float Kd = ");
  SerialUSB.print(Kd, 5);
  SerialUSB.println(";");
  SerialUSB.println("");
  SerialUSB.print("float Kff = ");
  SerialUSB.print(Kff, 5);
  SerialUSB.println(";");
  SerialUSB.println("");
  SerialUSB.print("float Kacc = ");
  SerialUSB.print(Kacc, 5);
  SerialUSB.println(";");



  //reset the error_register
  error_register = 0B1000000000000000;
}


float test_move(int step_add, int F_Sample) {

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  unsigned int dt = 1000000 / F_Sample;

  int counter = 0;

  int answer[1000];
  int target[1000];

  int omega_answer[1000];
  int omega_target[1000];

  int int_pessen_Kp_old = int_pessen_Kp;
  int int_pessen_Ki_old = int_pessen_Ki;
  int int_pessen_Kd_old = int_pessen_Kd;


  int_pessen_Kp = int_Kp;
  int_pessen_Ki = int_Ki;
  int_pessen_Kd = int_Kd;


  enabled = true;

  // save the first value;
  target[0] = step_target;

  // calculate the target vector
  for (int i = 1; i < 1000; i++) {

    if (i <= 100) {
      target[i] = target[i - 1] + ((i * step_add) / 100.0);
    }
    else if (100 < i && i <= 400) {
      // const velocity
      target[i] = target[i - 1] + step_add;
    }
    else if (400 < i && i <= 500) {
      //slow down
      target[i] = target[i - 1] + (((500 - i) * step_add) / 100.0);
    }
    else if (500 < i && i <= 600) {
      // accelerate
      target[i] = target[i - 1] - (((i - 500) * step_add) / 100.0);
    }
    else if (600 < i && i <= 900) {
      // const velocity
      target[i] = target[i - 1] - step_add;
    }
    else {
      //slow down
      target[i] = target[i - 1] - (((1000 - i) * step_add) / 100.0);
    }

  }

  while (counter < 1000) {
    current_time = micros();

    if (current_time >= next_time) {
      next_time = current_time + dt;

      answer[counter] = y;
      omega_answer[counter] = omega;
      step_target = target[counter];

      counter += 1;
    }
  }


  for (int i = 0; i < 1000; i++) {
    target[i] = target[i] * stepangle;
  }


  //disable motor
  enabled = false;


  // calculate the velocity profiles

  for (int i = 0; i < 999; i++) {
    //omega_answer[i] = answer[i + 1] - answer[i] ;
    omega_target[i] = target[i + 1] - target[i] ;
  }
  // omega_answer[999] = omega_answer[998];
  omega_target[999] = omega_target[998];

  /*
    for (int i = 0; i < 999; i++) {
      omega_answer[i] = (0.6 * omega_answer[i]) + (0.4 * omega_answer[i + 1]);
    }
    omega_answer[999] = omega_answer[998];*/


  float error_omega = 0.0;
  for (int i = 0; i < 1000; i++) {
    error_omega = error_omega + ((omega_target[i] - omega_answer[i]) * (omega_target[i] - omega_answer[i])) ;
  }
  error_omega = error_omega;

  float error_position = 0.0;
  for (int i = 0; i < 1000; i++) {
    error_position = error_position + ((target[i] - answer[i]) * (target[i] - answer[i])) ;
  }
  error_position = error_position / 15.0;

  float cost = error_position + error_omega;

  int_pessen_Kp = int_pessen_Kp_old;
  int_pessen_Ki = int_pessen_Ki_old;
  int_pessen_Kd = int_pessen_Kd_old;

  delay(100);

  return (cost);
}



void downhill_simplex() {

  int velocity = 40;
  int F_Sample = 5000;

  int step_add = (velocity * steps_per_revolution * microstepping * microstepping) / (mm_rev * F_Sample);

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

  float init_jump = 1.2;

  float p_max = 2.0;
  float i_max = 0.5;
  float d_max = 8.0;
  float Kff_max = 0.2;
  float Kacc_max = 0.2;

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


  SerialUSB.println(downhill_header);
  SerialUSB.println(cancle_header);
  SerialUSB.println(" ");
  SerialUSB.println("| Kp     | Ki     | Kd     | Kff    | Kacc   | objective  |");
  SerialUSB.println("|---------------------------------------------------------|");

  bool  cancle_crit = 1;


  //while (cancle_crit > 0.05) {
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

        cost[i] = (test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample)) / 4.0;

        SerialUSB.print("| ");
        SerialUSB.print(Kp, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Ki, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Kd, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Kff, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Kacc, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(cost[i], 4);
        SerialUSB.println(" |");
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

    float temp_Cost_r = (test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample)) / 4.0;

    SerialUSB.print("| ");
    SerialUSB.print(Kp, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(Ki, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(Kd, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(Kff, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(Kacc, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(temp_Cost_r, 4);
    SerialUSB.println(" |");

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

      float temp_Cost_e = (test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample)) / 4.0;

      SerialUSB.print("| ");
      SerialUSB.print(Kp, 4);
      SerialUSB.print(" | ");
      SerialUSB.print(Ki, 4);
      SerialUSB.print(" | ");
      SerialUSB.print(Kd, 4);
      SerialUSB.print(" | ");
      SerialUSB.print(Kff, 4);
      SerialUSB.print(" | ");
      SerialUSB.print(Kacc, 4);
      SerialUSB.print(" | ");
      SerialUSB.print(temp_Cost_e, 4);
      SerialUSB.println(" |");

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

        float temp_Cost_c = (test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample) + test_move(step_add, F_Sample)) / 4.0;

        SerialUSB.print("| ");
        SerialUSB.print(Kp, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Ki, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Kd, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Kff, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(Kacc, 4);
        SerialUSB.print(" | ");
        SerialUSB.print(temp_Cost_c, 4);
        SerialUSB.println(" |");


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


  // we are finished there is no improvment possible
  int_pessen_Kp = (1024.0 * ((Kp * 0.7) / (0.6))) + 0.5;
  int_pessen_Ki = (1024.0 * ((Ki * 0.7 * 2.5) / (2 * 0.6))) + 0.5;
  int_pessen_Kd = (1024.0 * ((Kd * 8.0 * 0.7) / (20.0 * 0.6))) + 0.5;

  SerialUSB.println("|-----------------------------------------------------------|");
  SerialUSB.println("");
  SerialUSB.print("Convergence reached for v = ");
  SerialUSB.print(velocity);
  SerialUSB.println("[mm/s].");


  SerialUSB.println("");
  SerialUSB.print("float Kp = ");
  SerialUSB.print(Kp, 5);
  SerialUSB.println(";");
  SerialUSB.print("float Ki = ");
  SerialUSB.print(Ki, 5);
  SerialUSB.println(";");
  SerialUSB.print("float Kd = ");
  SerialUSB.print(Kd, 5);
  SerialUSB.println(";");
  SerialUSB.println("");
  SerialUSB.print("float Kff = ");
  SerialUSB.print(Kff, 5);
  SerialUSB.println(";");
  SerialUSB.println("");
  SerialUSB.print("float Kacc = ");
  SerialUSB.print(Kacc, 5);
  SerialUSB.println(";");



}




void test() {


  //SerialUSB.print(dirac_header);

  int frequency = 0;
  // int last_step_target = step_target;


  //response_steps = 1000;

  unsigned const int time_out = 5000;
  unsigned int start_millis = millis();

  SerialUSB.print(" Sample rate = ");

  // get the frequency for data recording
  while (frequency == 0) {
    if (timed_out(start_millis, time_out)) return;
    frequency = SerialUSB.parseInt();
  }

  if (frequency < 200) {
    // limit the lower edge of the frequency to 200 Hz
    frequency = 200;
    SerialUSB.println(frequency);
    SerialUSB.println(" lower frequency limited to 200 Hz");

  }
  else if (frequency > 10000) {
    // limit the upper frequency to 10 kHz
    frequency = 10000;
    SerialUSB.println(frequency);
    SerialUSB.println(" upper frequency limited to 10 kHz");
  }
  else {
    SerialUSB.println(frequency);
  }





  int velocity = 80;
  int F_Sample = frequency;

  int step_add = (velocity * steps_per_revolution * microstepping * microstepping) / (mm_rev * F_Sample);

  unsigned int current_time = micros();
  unsigned int next_time = current_time;

  unsigned int dt = 1000000 / F_Sample;

  int counter = 0;


  int answer[1000];
  int target[1000];

  int omega_answer[1000];
  int omega_target[1000];

  int int_pessen_Kp_old = int_pessen_Kp;
  int int_pessen_Ki_old = int_pessen_Ki;
  int int_pessen_Kd_old = int_pessen_Kd;


  int_pessen_Kp = int_Kp;
  int_pessen_Ki = int_Ki;
  int_pessen_Kd = int_Kd;


  enabled = true;

  // save the first value;
  target[0] = step_target;

  // calculate the target vector
  for (int i = 1; i < 1000; i++) {

    if (i <= 100) {
      target[i] = target[i - 1] + ((i * step_add) / 100.0);
    }
    else if (100 < i && i <= 400) {
      // const velocity
      target[i] = target[i - 1] + step_add;
    }
    else if (400 < i && i <= 500) {
      //slow down
      target[i] = target[i - 1] + (((500 - i) * step_add) / 100.0);
    }
    else if (500 < i && i <= 600) {
      // accelerate
      target[i] = target[i - 1] - (((i - 500) * step_add) / 100.0);
    }
    else if (600 < i && i <= 900) {
      // const velocity
      target[i] = target[i - 1] - step_add;
    }
    else {
      //slow down
      target[i] = target[i - 1] - (((1000 - i) * step_add) / 100.0);
    }

  }

  while (counter < 1000) {
    current_time = micros();

    if (current_time >= next_time) {
      next_time = current_time + dt;

      answer[counter] = y;
      omega_answer[counter] = omega;
      step_target = target[counter];

      counter += 1;
    }
  }


  for (int i = 0; i < 1000; i++) {
    target[i] = target[i] * stepangle;
  }


  //disable motor
  enabled = false;


  // calculate the velocity profiles
  for (int i = 0; i < 999; i++) {
    omega_target[i] = target[i + 1] - target[i] ;
  }
  omega_target[999] = omega_target[998];

  for (int i = 0; i < 1000; i++) {
    omega_target[i] = (omega_target[i] * F_Sample) / FPID;
  }


  int_pessen_Kp = int_pessen_Kp_old;
  int_pessen_Ki = int_pessen_Ki_old;
  int_pessen_Kd = int_pessen_Kd_old;

  delay(100);

  for (int i = 0; i < 1000; i++) {
    SerialUSB.print(target[i]);
    SerialUSB.print(';');
    SerialUSB.print(answer[i]);
    SerialUSB.print(';');
    SerialUSB.print(omega_target[i]);
    SerialUSB.print(';');
    SerialUSB.println(omega_answer[i]);
  }

}
