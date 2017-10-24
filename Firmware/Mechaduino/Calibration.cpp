#include "Calibration.h"
#include "A4954.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "analogFastWrite.h"
#include "State.h"
#include "Utils.h"
#include "Cmd.h"
#include "Encoder.h"
#include "board.h"
#include "Serial.h"
#include "Language.h"



void calibration(int arg_cnt, char **args) {

  disableTC5Interrupts();
  disableTC4Interrupts();

  int avg = 50;
  bool smooth = check_argument(args, arg_cnt, "-smooth");
  bool debug = check_argument(args, arg_cnt, "-debug");
  bool check_readings = check_argument(args, arg_cnt, "-check");

  SerialUSB.println(calibrate_header);
  SerialUSB.println();

  enabled = true;
  dir = true;
  step_target = 0;
  output_calibration(0, uMAX / 3);
  delay(100);


  float encoderReading = 0;

  for (int i = 0; i < 50; i++) {
    encoderReading = encoderReading + readEncoder();
  }
  encoderReading = encoderReading / 50.0;




  oneStep();
  delay(10);
  oneStep();
  delay(10);
  oneStep();
  delay(10);
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
  int readings_reverse[steps_per_revolution];
  int mean_readings[steps_per_revolution];

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

    delay(10);

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
    if (i < (steps_per_revolution - 1)) {
      oneStep();
    }

  }
  SerialUSB.println(" -> done! Reverse!");



  //backwarts
  delay(10);
  oneStep();
  delay(250);
  dir = false;
  delay(250);
  oneStep();
  delay(10);

  temp_reading = 4 * readEncoder();
  last_reading = temp_reading;

  for (int i = (steps_per_revolution - 1); i >= 0; i--) {

    if (canceled()) {
      enabled = false;
      return;
    }
    delay(10);

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

    readings_reverse[i] =  (encoderReading / avg) + 0.5;
    if (readings_reverse[i] > 65536) {
      readings_reverse[i] = readings_reverse[i] - 65536;
    }

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


  // calculate the mean reading between forward and reverse
  for (int i = 0; i < steps_per_revolution; i++) {
    mean_readings[i] = (readings[i] + readings_reverse[i] ) / 2;
  }

  if (debug) {
    SerialUSB.println("");
    SerialUSB.print("forward=[");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(readings[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("];");

    SerialUSB.print("reverse=[");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(readings_reverse[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("];");

    SerialUSB.print("mean=[");
    for (int i = 0; i < steps_per_revolution; i++) {
      SerialUSB.print(mean_readings[i]);
      SerialUSB.print(",");
    }
    SerialUSB.println("];");
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
    int check_readings_reverse[steps_per_revolution];
    int mean_check_readings[steps_per_revolution];

    for (int i = 0; i < steps_per_revolution; i++) {

      if (canceled()) {
        enabled = false;
        return;
      }

      counter += 1;

      delay(10);

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
      if (i < (steps_per_revolution - 1)) {
        oneStep();
      }

    }

    SerialUSB.println(" -> done! Reverse!");


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

      delay(10);

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

      check_readings_reverse[i] = (encoderReading / avg) + 0.5;
      if (check_readings_reverse[i] > 65536) {
        check_readings_reverse[i] = check_readings_reverse[i] - 65536;
      }

      if (counter == count) {
        counter = 0;
        SerialUSB.print(".");
      }

      //increase step_target with one fullstep
      oneStep();

    }
    SerialUSB.println(" -> done!");


    // calculate the mean reading between forward and reverse
    for (int i = 0; i < steps_per_revolution; i++) {
      mean_check_readings[i] = (check_readings[i] + check_readings_reverse[i] ) / 2;
    }


    if (debug) {
      SerialUSB.println("");
      SerialUSB.print("check_forward=[");
      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print(check_readings[i]);
        SerialUSB.print(",");
      }
      SerialUSB.println("];");

      SerialUSB.print("check_reverse=[");
      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print(check_readings_reverse[i]);
        SerialUSB.print(",");
      }
      SerialUSB.println("];");

      SerialUSB.print("check_mean=[");
      for (int i = 0; i < steps_per_revolution; i++) {
        SerialUSB.print(mean_check_readings[i]);
        SerialUSB.print(",");
      }
      SerialUSB.println("];");

    }


    step_target = 0;
    enabled = false;
    output(0, 0);




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
      enableTC5Interrupts();
      return;
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

      if (debug) {
        SerialUSB.println("");
        SerialUSB.print("smoothed=[");
        for (int i = 0; i < steps_per_revolution; i++) {
          SerialUSB.print(smoothed_readings[i]);
          SerialUSB.print(",");
        }
        SerialUSB.println("];");
      }
      
      bool qst = userqst(15000, "Continue?");

      if (qst == false) {
        step_target = 0;
        enabled = false;
        output(0, 0);
        enableTC5Interrupts();
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




void output_calibration(int electric_angle, int effort) {
  int32_t v_ref_coil_A;
  int32_t v_ref_coil_B;

  int32_t pole_angle;

  int16_t sine;
  int16_t cosine;

  if (effort != 0) {
    pole_angle = mod(-(phase_multiplier * electric_angle) , 3600);


    sine = sin_lookup[pole_angle];
    cosine = cos_lookup[pole_angle];

    v_ref_coil_A = (abs(effort) * sine) / 4096;
    v_ref_coil_B = (abs(effort) * cosine) / 4096;


    if (v_ref_coil_A > 0)  {
      digitalWriteDirect(IN_1, LOW);
      digitalWriteDirect(IN_2, HIGH);
    }
    else  {
      digitalWriteDirect(IN_1, HIGH);
      digitalWriteDirect(IN_2, LOW);
    }

    if (v_ref_coil_B > 0)  {
      digitalWriteDirect(IN_3, LOW);
      digitalWriteDirect(IN_4, HIGH);
    }
    else  {
      digitalWriteDirect(IN_3, HIGH);
      digitalWriteDirect(IN_4, LOW);
    }

    analogFastWrite(VREF_1, abs(v_ref_coil_A));
    analogFastWrite(VREF_2, abs(v_ref_coil_B));
  }
  else {
    analogFastWrite(VREF_1, 0);
    analogFastWrite(VREF_2, 0);

    digitalWriteDirect(IN_1, LOW);
    digitalWriteDirect(IN_2, LOW);
    digitalWriteDirect(IN_3, LOW);
    digitalWriteDirect(IN_4, LOW);
  }

}





