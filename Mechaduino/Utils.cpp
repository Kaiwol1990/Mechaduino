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
#if !defined(INVERT)
  if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
    dir = false;
  }
  else {
    dir = true;
  }
#else
  if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
    dir = true;
  }
  else {
    dir = false;
  }
#endif
}


void enaInterrupt() {
  if (REG_PORT_IN0 & PORT_PA14) { // check if ena_pin is HIGH
    enabled = false;
  }
  else {
    enabled = true;
  }
}


void calibration() {

  disableTC5Interrupts();

  int avg = 100;


  SerialUSB.println(calibrate_header);
  enabled = false;


  float encoderReading = 0;
  int lastencoderReading = 0;

  encoderReading = readEncoder();

  dir = true;

  oneStep();
  delay(100);
  oneStep();

  if ((readEncoder() - lastencoderReading) < 0)
  {
    SerialUSB.println("Wired backwards");
    enableTC5Interrupts();
    return;
  }

  dir = false;

  oneStep();
  delay(100);
  oneStep();
  delay(500);


  step_target = 0;

  SerialUSB.println("Calibrating fullsteps");
  SerialUSB.println(procent_bar);
  int counter = 0;
  int count = (3 * steps_per_revolution) / 50;
  dir = true;

  int readings[3][steps_per_revolution];
  for (int k = 0; k < 3; k++) {

    // step to every single fullstep position and read the Encoder
    for (int i = 0; i < steps_per_revolution; i++) {

      if (canceled()) return;
      counter += 1;

      delay(100);

      encoderReading = 0;

      for (int reading = 0; reading < avg; reading++) {
        encoderReading += readEncoder();
        delayMicroseconds(100);
      }

      readings[k][i] =  (encoderReading / avg);

      if (counter == count) {
        counter = 0;
        SerialUSB.print(".");
      }
      oneStep();
    }
  }

  float fullStepReadings_raw[steps_per_revolution];
  for (int i = 0; i < steps_per_revolution; i++) {
    fullStepReadings_raw[i] = (( 4 * (readings[0][i] + readings[1][i] + readings[2][i]) / 3.0) + 0.5);
  }

  output(0, 0);
  step_target = 0;

  int perfect[steps_per_revolution];
  for ( int i = 0; i < steps_per_revolution; i++) {
    perfect[i] = ((i * 65536) / 200);
  }

  SerialUSB.println();
  SerialUSB.println();


  // find max value in full step readings and index
  float minimum = 65536;
  int idx = 0;
  for ( int i = 0; i < steps_per_revolution; i++) {
    if (fullStepReadings_raw[i] < minimum) {
      minimum = fullStepReadings_raw[i];
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
    fullStepReadings[i] = fullStepReadings_raw[counter];
  }



  // calculate the error between the measured and perfect lookup
  float error_raw[steps_per_revolution];
  for ( int i = 0; i < steps_per_revolution; i++) {
    error_raw[i] =  (float)(perfect[i]) - (float)(fullStepReadings[i]);
  }

  //float gausian[31] = {0.000888058511381200, 0.00158610663008152, 0.00272176986137680, 0.00448743986440246, 0.00710843674842711, 0.0108187674516528, 0.0158201169212244, 0.0222264351509086, 0.0300025492494709, 0.0389112087983103, 0.0484863518426015, 0.0580487023003957, 0.0667719013138084, 0.0737943634766382, 0.0783575520685465, 0.0799404796215474, 0.0783575520685465, 0.0737943634766382, 0.0667719013138084, 0.0580487023003957, 0.0484863518426015, 0.0389112087983103, 0.0300025492494709, 0.0222264351509086, 0.0158201169212244, 0.0108187674516528, 0.00710843674842711, 0.00448743986440246, 0.00272176986137680, 0.00158610663008152, 0.000888058511381200};


  float gausian[31];
  counter = 0;
  float m;
  for (int i = -15; i <= 15; i++) {
    gausian[counter] = exp(-0.5 * ((float)i * (float)i) / (25.0));
    //gausian[counter] = pow(-0.5 * ((float)i * (float)i) / (25.0), 2.71828182846);
    m = m + gausian[counter];
    counter = counter + 1;
  }
  for (int i = 0; i < 31; i++) {
    gausian[i] =  gausian[i] / m;
  }

  SerialUSB.println();
  for (int i = 0; i < 31; i++) {
    SerialUSB.print(gausian[i]);
    SerialUSB.print(" , ");
  }
  SerialUSB.println();
  SerialUSB.println();


  // smooth the fullstep readings with a ring buffer
  float error[steps_per_revolution];
  counter = 0;
  for ( int i = 0; i < steps_per_revolution; i++) {
    m = 0;
    counter = 0;
    int lower = i - 15;
    int higher = i + 15;

    for (int j = lower; j < higher; j++) {
      if (j >= steps_per_revolution) {
        m = m + error_raw[j - 200] * gausian[counter];
      }
      else if (j < 0) {
        m = m + error_raw[j + 200] * gausian[counter];
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
  float smoothed_error;
  for (int i = 0; i < steps_per_revolution; i++) {

    if (canceled()) return;
    counter += 1;

    delay(20);

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delayMicroseconds(100);
    }

    float temp =  abs((4 * encoderReading / avg) - x[i]);

    if (temp > smoothed_error) {
      smoothed_error = temp;
    }

    if (counter == count) {
      counter = 0;
      SerialUSB.print(".");
    }
    oneStep();
  }

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
    else if (ticks[i] > 15000) {
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

  SerialUSB.print("const PROGMEM int lookup[] = {");

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
  static int steps;

  if (dir == 0) {
    steps += 1;
    step_target += microstepping;
  }
  else {
    steps -= 1;
    step_target -= microstepping;
  }

  int target_raw = (step_target * stepangle) / 100;
  int raw_0 = mod(target_raw, 36000);
  output(raw_0 , uMAX / 2);
}

int mod(int xMod, int mMod) {
  return (xMod % mMod + mMod) % mMod;
}


void setupTCInterrupts() {
  const int overflow_TC_5 = (48000000 / FPID) - 1;

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



void antiCoggingCal() {

  bool last_enabled = enabled;

  float u_cogging = 0;

  int max_count =  (16384 / 4);

  int prozent = ((max_count / 50) + 0.5) + 1;

  SerialUSB.println("//---- Calculating friciton ----");
  SerialUSB.println(procent_bar);

  step_target = ( (100 * y) / stepangle);

  enabled = true;

  for (int i = 0; i < max_count; i++) {

    step_target = step_target + 1;

    delay(15);

    float sum = 0;
    for (int k = 0; k < 50; k++) {
      sum = sum + u;
    }

    u_cogging = u_cogging + (sum / 50.0);

    if (i % prozent == 0) {
      SerialUSB.print(".");
    }

  }
  SerialUSB.println("");

  int_Kfr = 1000 * ((u_cogging / 2) / max_count);

  enabled = last_enabled;

  SerialUSB.println();
  SerialUSB.println();

}

void PID_autotune() {

  disableTC5Interrupts();

  int loops = 0;
  int outputStep = (10 * uMAX) / 10;
  int frequency = 20000;
  int dt = (1000000 / frequency);
  int scan_dt = dt - 2;

  float temp_Kp = 0;
  float temp_Ki = 0;
  float temp_Kd = 0;

  bool received = false;
  bool debug = false;
  unsigned long now = millis();
  int k = 1;

  SerialUSB.println("//---- Autotuning the PID controller ---");
  SerialUSB.println(cancle_header);
  SerialUSB.println("Enter number of tuning cycles!");
  SerialUSB.print("Cycles = ");

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
    int noiseBand = (measure_noise(false) / 2) + 0.5;

    SerialUSB.print("|   ");
    SerialUSB.print(noiseBand);
    if (noiseBand >= 10) {
      SerialUSB.print("  ");
    }
    else {
      SerialUSB.print("   ");
    }

    int raw_0 = mod(y, 36000);
    int raw_1 = raw_0;
    int y_1 = y;

    int points[1124] = {0};
    float smoothed[1124] = {0};

#define filterSamples   5
    int sensSmoothArray1 [filterSamples];

    int peakType = 0;
    int peakCount = 0;
    bool justchanged = false;
    int absMax = setpoint;
    int absMin = setpoint;

    unsigned long last_time = 0;

    bool isMax = true;
    bool isMin = true;

    int counter = 0;
    unsigned long start_millis = millis() + 500;

    // start the oscilations and measure the behavior every 50 microsseconds
    while (counter < 1124) {

      // canceling call if something goes wrong
      if (canceled()) return;

      now = micros();
      if (now > last_time + scan_dt) {

        y = readAngle(y_1, raw_1);
        raw_0 = mod(y, 36000);

        last_time = now;
        y_1 = y;
        raw_1 = raw_0;


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
          output(-raw_0 - PA, abs(u));
        }
        else {
          output(-raw_0 + PA, abs(u));
        }

        // wait 1 second to get stable oscilations
        if (millis() > start_millis) {
          points[counter] = y;
          counter++;
        }

      }
    }

    output(-raw_0 - PA, 0);


    //smoothing the measured data
    for (int i = 0; i <= 1124; i++) {
      smoothed[i] = digitalSmooth(points[i], sensSmoothArray1);
    }


    for (int i = 0; i < 1024; i++) {
      points[i] = points[i + 50];
      smoothed[i] = smoothed[i + 50];
    }

    // building mean of the position data
    int sum = 0;
    for (int i = 0; i < 1024; i++) {
      sum = sum + smoothed[i];
    }
    sum = sum / 1024.0;

    for (int i = 0; i < 1024; i++) {
      smoothed[i] = smoothed[i] - sum;
      points[i] = points[i] - sum;
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

    while (i < len - 1 && pd_state < 3) {
      sum_old = sum;
      sum = 0;

      for (int k = 1; k < len - i; k++) {
        sum = sum + (points[k]) * (points[k + i]);
      }

      if (pd_state == 2 && (sum - sum_old) <= 0) {
        period = i;
        pd_state = 3;
      }

      if (pd_state == 1 && (sum > thresh) && (sum - sum_old) > 0) {
        pd_state = 2;
      }

      if (!i) {
        thresh = sum * 0.5;
        pd_state = 1;
      }

      i = i + 1;
    }

    float frequency = (1000000.0 / (float)(dt)) / period;
    double Tu = 1.0 / frequency;

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

    if (frequency < 40.0 || frequency > 9950.0) {
      SerialUSB.println();
      SerialUSB.println("Autotune failed, frequency not in usable range!");
      SerialUSB.println();
      SerialUSB.print(":> ");
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
          justchanged = true;
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
          justchanged = true;
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
    float Ku = 4 * 2 * outputStep * 1000 / (PI * 2 * A);

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
    SerialUSB.print((0.6 * Ku) / 1000.0, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(((1.2 * Ku) / (Tu * FPID)) / 1000.0, 4);
    SerialUSB.print(" | ");
    SerialUSB.print(((0.6 * Ku * Tu * FPID) / 8) / 1000.0, 4);
    SerialUSB.print(" |");

    SerialUSB.println("");
    delay(500);
    k++;
  }


  // we are finished

  SerialUSB.println("|-----------------------------------------------------------------|");
  SerialUSB.println();
  SerialUSB.println();

  int_Kp = (((temp_Kp / loops)) + 0.5);
  int_Ki = (((temp_Ki / loops)) + 0.5);
  int_Kd = (((temp_Kd / loops)) + 0.5);

#if defined(use_PI)
  //---- PD Gains ----
  volatile int  int_Kp = (( int_Kp  * 0.4) / 0.6);
  volatile int  int_Ki = (( int_Ki  * 0.48 / 1.2));
  volatile int  int_Kd = ( int_Kd ) / 20;
#endif


  enableTC5Interrupts();
}



int digitalSmooth(int rawIn, int *sensSmoothArray) {    // "int *sensSmoothArray" passes an array to the function - the asterisk indicates the array name is a pointer
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

  return total / k;    // divide by number of samples
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
  if (abs(input) > input) {
    return -1;
  }
  else {
    return 1;
  }
  //return (input / abs(input));
}

void boot() {
  SerialUSB.begin(baudrate);

  delay(500);

  SerialUSB.print("setup pins:");
  setupPins();
  SerialUSB.println(" OK");

  SerialUSB.print("setup pins:");
  setupSPI();
  SerialUSB.println(" OK");

  SerialUSB.print("setup pins:");
  setupTCInterrupts();
  SerialUSB.println(" OK");


  delay(1000);
  SerialUSB.print("setup controller:");

  // get the filter going and ge samples for 1 second
  int i = 0;
  int raw_0 = (pgm_read_word_near(lookup + readEncoder()));
  int raw_1 = raw_0;
  int raw_diff;

  while (i < 200) {
    raw_0 = (pgm_read_word_near(lookup + readEncoder()));

    raw_diff = raw_0 - raw_1;

    if (raw_diff < -18000) {
      y = y + 36000 + raw_diff;
    }
    else if (raw_diff > 18000) {
      y = y - 36000 + raw_diff;
    }
    else {
      y = y  + raw_diff;
    }
    raw_1 = raw_0;
    i++;
  }

  step_target = ((y) / stepangle);
  SerialUSB.println(" OK");


  SerialUSB.print("setup enable pin:");
#ifdef USE_ENABLE_PIN
  enaInterrupt();
#else
  enabled = true;
#endif
  SerialUSB.println(" OK");


  SerialUSB.print("setup direction pin:");
  dirInterrupt();
  SerialUSB.println(" OK");


  SerialUSB.print("checking lookup table:");
  bool error = check_lookup(false);
  if (error) {
    SerialUSB.println(" ERROR! The lookup table has some failure! send \" check \" to get further informations!");
  }
  else {
    SerialUSB.println(" OK");
  }


  delay(100);

  SerialUSB.print("enable controller:");
  enableTC5Interrupts();
  SerialUSB.println(" OK");
  SerialUSB.println("");


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


  delay(500);
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

  if (minimal > 3) {
    if (output) {
      SerialUSB.println("minimal value to high");
    }
    error = true;
  }

  if (minimal < 0) {
    if (output) {
      SerialUSB.println("minimal value to low");
    }
    error = true;
  }

  if (maximal > 36000) {
    if (output) {
      SerialUSB.println("maximal value to high");
    }
    error = true;
  }

  if (maximal < 35995) {
    if (output) {
      SerialUSB.println("maximal value to low");
    }
    error = true;
  }

  if (max_dx > 5) {
    if (output) {
      SerialUSB.println("step between elements to high");
    }
    error = true;
  }

  if (max_dx < 1) {
    if (output) {
      SerialUSB.println("step between elements to low");
    }
    error = true;
  }

  return error;
}

