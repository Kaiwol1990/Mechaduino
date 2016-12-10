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
    REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
  }
  else {
    REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    enabled = true;
  }
}


void calibration() {
  bool last_enabled = enabled;
  disableTC5Interrupts();

  SerialUSB.println(calibrate_header);

  int encoderReading = 0;
  int lastencoderReading = 0;

  int avg = 250;         //how many readings to average

  int iStart = 0;
  int jStart = 0;
  int stepNo = 0;

  int fullStepReadings[steps_per_revolution];
  int ticks = 0;

  float lookupAngle = 0.0;
  float angle_per_step = 360.0 / steps_per_revolution;

  dir = true;
  output(0, 255);

  for (int reading = 0; reading < avg; reading++) {
    lastencoderReading += readEncoder();
    delayMicroseconds(1);
  }
  lastencoderReading = lastencoderReading / avg;

  oneStep();
  oneStep();
  oneStep();

  delay(500);

  for (int reading = 0; reading < avg; reading++) {
    encoderReading += readEncoder();
    delayMicroseconds(1);
  }
  encoderReading = encoderReading / avg;

  if ((encoderReading - lastencoderReading) < 0)
  {
    SerialUSB.println("Wired backwards");
    enabled = last_enabled;
    enableTC5Interrupts();
    return;
  }


  SerialUSB.println("Calibrating single steps");
  SerialUSB.println("0%      20%       40%       60%        80%       100%");
  SerialUSB.println("|--------------------------------------------------|");
  SerialUSB.print("|");

  // step to every single fullstep position and read the Encoder
  step_target = 0;

  for (int x = 0; x < steps_per_revolution; x++) {

    if (SerialUSB.available()) {
      if (SerialUSB.read() == 'c') {
        SerialUSB.println();
        SerialUSB.println("canceled!");
        enabled = last_enabled;
        enableTC5Interrupts();
        return;
      }
    }

    delay(100);

    encoderReading = 0;

    for (int reading = 0; reading < avg; reading++) {
      encoderReading += readEncoder();
      delayMicroseconds(1);
    }
    fullStepReadings[x] = encoderReading / avg;

    oneStep();

    if (((50 * x) % steps_per_revolution) == 0) {
      SerialUSB.print("-");
    }
  }

  SerialUSB.println("|");
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

  enableTC5Interrupts();
}


void oneStep() {
  if (dir == 0) {
    step_target += PA;
  }
  else {
    step_target -= PA;
  }

  output(step_target, 255);
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


void PID_autotune() {

  int loops = 0;
  int outputStep = uMAX;

  float temp_Kp = 0;
  float temp_Ki = 0;
  float temp_Kd = 0;

  bool received = false;
  bool debug = false;
  unsigned long now = millis();
  int k = 1;

  SerialUSB.println("//---- Autotuning the PID controller ---");
  SerialUSB.println("Press c to cancle!");
  SerialUSB.println("Enter number of tuning cycles!");
  SerialUSB.print("Cycles = ");

  while (!received) {
    if (SerialUSB.available()) {

      if (SerialUSB.peek() == 'd') {
        debug = true;
      }

      if (SerialUSB.peek() == 'c') {
        SerialUSB.read();
        SerialUSB.println("");
        SerialUSB.println("canceled!");
        SerialUSB.println();
        SerialUSB.print(":> ");
        return;
      }

      loops = SerialUSB.parseInt();

      SerialUSB.println(loops);
      SerialUSB.println("");

      received = true;
    }
    if (millis() > now + 5000) {
      SerialUSB.println("");
      SerialUSB.println("timed out!");
      SerialUSB.println();
      SerialUSB.print(":> ");
      return;
    }
  }

  SerialUSB.println("| loop | Noise | Frequency | lookback | P    | I  | D     |");

  // start autotune
  while (k <= loops) {

    SerialUSB.println("|---------------------------------------------------------|");
    SerialUSB.print("|   ");
    SerialUSB.print(k);
    if (k >= 10) {
      SerialUSB.print(" ");
    }
    else {
      SerialUSB.print("  ");
    }


    delay(200);

    int raw_0;
    int raw_1;
    int raw_diff;

    // measure current setpoint
    int setpoint = 0;
    for (int i = 0; i < 1000; i++) {
      raw_0 = pgm_read_word_near(lookup + readEncoder());

      raw_diff = raw_0 - raw_1;
      raw_1 = raw_0;
      if (raw_diff < -18000) {
        y = y + 36000 + raw_diff;
      }
      else if (raw_diff > 18000) {
        y = y - 36000 + raw_diff;
      }
      else {
        y = y  + raw_diff;
      }
      setpoint = setpoint + y;
    }
    setpoint = setpoint / 10000;



    // measuring the noise while the system is steady
    int higher_noise = 0;
    int lower_noise = 36000;

    for (int i = 0; i < 1000; i++) {

      raw_0 = pgm_read_word_near(lookup + readEncoder());

      if (raw_0 > higher_noise) higher_noise = raw_0;
      if (raw_0 < lower_noise) lower_noise = raw_0;

    }

    int noiseBand = (higher_noise - lower_noise);

    SerialUSB.print("|   ");
    SerialUSB.print(noiseBand);
    if (noiseBand >= 10) {
      SerialUSB.print("  ");
    }
    else {
      SerialUSB.print("   ");
    }

    unsigned long times[1124] = {0};
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

    raw_0 = pgm_read_word_near(lookup + readEncoder());;
    raw_1 = raw_0;
    raw_diff = 0;

    // start the oscilations and measure the behavior every 50 microsseconds
    while (counter < 1124) {

      // canceling call if something goes wrong
      if (SerialUSB.peek() == 'c') {
        SerialUSB.read();
        SerialUSB.println();
        SerialUSB.println("canceled!");
        return;
      }

      now = micros();
      if (now > last_time + 48) {

        raw_0 =  pgm_read_word_near(lookup + readEncoder());

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

        last_time = now;
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
          times[counter] = now;
          points[counter] = y;
          counter++;
          //u = 0;
        }

      }
    }

    output(-raw_0 - PA, 0);


    //smoothing the measured data
    for (int i = 0; i <= 1124; i++) {
      smoothed[i] = digitalSmooth(points[i], sensSmoothArray1);
    }


    for (int i = 0; i < 1024; i++) {
      times[i] = times[i + 50];
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
        SerialUSB.print(times[i]);
        SerialUSB.print(',');
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

    float frequency = (1000000.0 / 50.0) / period;
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
    int nLookBack = ((1000000 * Tu) / 50) / 4;
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
      now = times[i];

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
    SerialUSB.print((0.6 * Ku), 0);
    SerialUSB.print(" | ");
    SerialUSB.print(((1.2 * Ku) / (Tu * FPID)), 0);
    SerialUSB.print(" | ");
    SerialUSB.print(((0.6 * Ku * Tu * FPID) / 8), 0);
    SerialUSB.print(" |");

    SerialUSB.println("");
    delay(500);
    k++;
  }


  // we are finished

  SerialUSB.println("|---------------------------------------------------------|");
  SerialUSB.println();
  SerialUSB.println("Building Average!");
  SerialUSB.println();

  int_Kp = (((temp_Kp / loops)) + 0.5);
  int_Ki = (((temp_Ki / loops)) + 0.5);
  int_Kd = (((temp_Kd / loops)) + 0.5);

  ITerm_max = (uMAX * 1000) / (3 * int_Ki);

  big_Kp = (int_Kp * 2) / 10;
  big_Ki = int_Ki;
  big_Kd = (8 * int_Kd) / 3;

  small_Kp =  int_Kp;
  small_Ki = int_Ki / 2;
  small_Kd = int_Kd / 4;
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

