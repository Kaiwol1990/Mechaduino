//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:


#include "State.h"
#include "Utils.h"

#include "A4954.h"
#include "board.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "Encoder.h"
#include "lookup_table.h"
#include "Serial.h"


// ----- gets called with FPID -----
// ----- calculates the target velocity and PID settings -----
void TC5_Handler() {

  static unsigned int last_micros = micros();
  unsigned int current_micros;

  //----- Variables -----
  static int ITerm;
  static int DTerm;

  static int e_1;        // last error term
  static int r_1;        // target 1 loop ago
  static int u_1;
  static int omega_target_1;

  static int serial_loop_counter;
  static unsigned int last_time;
  
  int phase_advanced;

  //----- Calculations -----

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    current_micros = micros();
    int dt = current_micros - last_micros;
    last_micros = current_micros;

    if (dt > target_dt) {
      error_register = error_register | 0B0000000000000001;    // log error in register
    }


    // calculate the current target from the received steps and the angle per step
    r = (step_target * stepangle);

    // target angular velocity buffered over two sample times
    int omega_target = (r - r_1);

    // start the calculations only if the motor is enabled
    if (enabled) {

      // ----------- PID loop ----------
      // -------------------------------
      // calculate the error
      error = (r_1 - y);


      // small errors and low speed ueses another pid gain set
      if (abs(omega_target) <= 0 && abs(error) < 50) {

        if (abs(u) < uMAX) {
          // calculate the I- and DTerm
          ITerm = ITerm + (int_pessen_Ki * error);
        }
        // constrain the ITerm
        if (ITerm > ITerm_max) {
          ITerm = ITerm_max;
        }
        else if (ITerm < -ITerm_max) {
          ITerm = -ITerm_max;
        }

        DTerm = ((D_Term_LPFa * DTerm) + (D_Term_LPFb * (int_pessen_Kd) * (error - e_1))) / 128;

        // ------ Add up the Effort ------
        // -------------------------------
        //         u-pid
        u = ((u_LPFa * u_1) + (u_LPFb * (((int_pessen_Kp * error) + ITerm + DTerm + (int_Kff * omega_target) + (int_Kacc * (omega_target - omega_target_1)))  / 1024))) / 128;



      }
      else {

        if (abs(u) < uMAX) {
          // calculate the I- and DTerm
          ITerm = ITerm + (int_Ki * error);
        }

        // constrain the ITerm
        if (ITerm > ITerm_max) {
          ITerm = ITerm_max;
        }
        else if (ITerm < -ITerm_max) {
          ITerm = -ITerm_max;
        }

        DTerm = ((D_Term_LPFa * DTerm) + (D_Term_LPFb * int_Kd * (error - e_1))) / 128;

        // ------ Add up the Effort ------
        // -------------------------------
        //         u-pid
        u = ((u_LPFa * u_1) + (u_LPFb * (((int_Kp * error) + ITerm + DTerm + (int_Kff * omega_target) + (int_Kacc * (omega_target - omega_target_1)))  / 1024))) / 128;

      }


    }
    else {
      step_target = y / stepangle;
      error = 0;
      u = 0;
      ITerm = 0;
    }


    // constrain the effort to the user spedified maximum
    if (u > uMAX) {
      u = uMAX;
      error_register = error_register | 0B0000000000000100;    // log error in register
    }
    else if (u < -uMAX) {
      u = -uMAX;
      error_register = error_register | 0B0000000000000100;    // log error in register
    }


    // calculate the phase advance term
    // when the motor is commanded to hold its position the term will be calculate from the posiition error and a small amount of the PA Term
    // when the motor is commanded to move the term will be set to it's maximum -> PA
    if (omega_target == 0) {
      phase_advanced = ((sign(u) * PA) / 4 ) + error;

      if (phase_advanced >= PA) {
        phase_advanced = PA;
      }
      else if (phase_advanced <= -PA) {
        phase_advanced = -PA;
      }

    }
    else {
      phase_advanced = (sign(u) * PA) + omega;
    }


    // calculate the electrical angle for the motor coils
    electric_angle = -(raw_0 + phase_advanced);


    // write the output
    output(electric_angle, u);


    // buffer the variables for the next loop
    e_1 = error;
    u_1 = u;
    r_1 = r;
    omega_target_1 = omega_target;


    if (abs(error) > max_e) {
      // set error register if the error was to high at some point
      error_register = error_register | 0B0000000000000010;
    }


    if (streaming) {
      serial_loop_counter++;

      if (serial_loop_counter >= max_serial_counter) {

        fifo_counter++;

        byte fifo_position = mod(fifo_counter, 99);

        unsigned int current_time = micros();

        fifo[0][fifo_position] = (current_time - last_time);
        fifo[1][fifo_position] = streaming;
        fifo[2][fifo_position] = r;
        fifo[3][fifo_position] = y;
        fifo[4][fifo_position] = error;
        fifo[5][fifo_position] = omega_target;
        fifo[6][fifo_position] = omega;
        fifo[7][fifo_position] = u;
        fifo[8][fifo_position] = electric_angle;

        serial_loop_counter = 0;
        last_time = current_time;
      }
    }


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}









// ----- reads the current shaft angle with 2*FPID -----
// ----- Oversamples the shaft angle to reduce noise -----
void TC4_Handler() {
  static byte counter;
  static int y_temp;
  static int sum = 0;
  static int omega_buffer[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  static byte omega_pointer = 0;


  if (TC4->COUNT16.INTFLAG.bit.OVF == 1) {

    counter++;

    switch (counter) {
      case 1:

        y_temp = readAngle(y, raw_0);

        sum = sum - omega_buffer[omega_pointer];
        omega_buffer[omega_pointer] = y_temp - y;
        sum = sum + omega_buffer[omega_pointer];

        omega_pointer++;
        if (omega_pointer > 7) {
          omega_pointer = 0;
        }

        break;
      case 2:

        int y_temp_2 = readAngle(y, raw_0);

        sum = sum - omega_buffer[omega_pointer];
        omega_buffer[omega_pointer] = y_temp_2 - y;
        sum = sum + omega_buffer[omega_pointer];
        omega_pointer++;
        if (omega_pointer > 7) {
          omega_pointer = 0;
        }



        omega = sum >> 3;


        y = (y_temp + y_temp_2) / 2;
        raw_0 = mod(y, 36000);


        counter = 0;
        break;
    }


    TC4->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

