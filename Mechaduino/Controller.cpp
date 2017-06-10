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



// ----- gets called with FPID -----
// ----- calculates the target velocity and PID settings -----
void TC5_Handler() {

  //----- Variables -----
  static int ITerm;
  static int DTerm;

  //int e_0;               // current error term
  int u_raw;
  static int e_1;        // last error term
  static int r_1;        // target 1 loop ago
  static int u_1;


  int omega_target;    // target angle velocity
  static int omega_target_1;

  int phase_advanced;

  //----- Calculations -----

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    // calculate the current target from the received steps and the angle per step
    r = (step_target * stepangle);

    // target angular velocity buffered over two sample times
    omega_target = (r - r_1);

    // start the calculations only if the motor is enabled
    if (enabled) {

      // ----------- PID loop ----------
      // -------------------------------
      // calculate the error
      error = (r_1 - y);

      // calculate the I- and DTerm
      ITerm = ITerm + (int_Ki * error);
      DTerm = ((D_Term_LPFa * DTerm) + (D_Term_LPFb * int_Kd * (error - e_1))) / 128;
      //DTerm = int_Kd * (error - e_1);

      // constrain the ITerm
      if (ITerm > ITerm_max) {
        ITerm = ITerm_max;
      }
      else if (ITerm < -ITerm_max) {
        ITerm = -ITerm_max;
      }



      // ------ Add up the Effort ------
      // -------------------------------
      //         u-pid                         +         feedforward       +                 acceleration              +        friction
      /*
            if (omega_target != 0) {
              u_raw = ( ((int_Kp * error) + ITerm + DTerm) + (omega_target * int_Kvff) + ((omega_target - omega_target_1) * int_J) + (sign(omega_target) * int_Kff)  ) / 1024;
            }
            else {
              u_raw = ( ((int_Kp * error) + ITerm + DTerm) + (omega_target * int_Kvff) + ((omega_target - omega_target_1) * int_J) +  0                              ) / 1024;

            }*/

      u_raw = ((int_Kp * error) + ITerm + DTerm)  / 1024;

      u = ((u_LPFa * u_1) + (u_LPFb * u_raw)) / 128;


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
    }
    else if (u < -uMAX) {
      u = -uMAX;
    }


    // calculate the phase advance term
    // when the motor is commanded to hold its position the term will be calculate from the posiition error and a small amount of the PA Term
    // when the motor is commanded to move the term will be set to it's maximum -> PA
    if (abs(omega_target) != 0) {
      phase_advanced = sign(u) * PA;
    }
    else {
      phase_advanced = ((sign(u) * PA) / 4 ) + error;

      if (phase_advanced >= PA) {
        phase_advanced = PA;
      }
      else if (phase_advanced <= -PA) {
        phase_advanced = -PA;
      }

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

    //wrtie the max error led high or low
    if (abs(error) < max_e) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
    }


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}









// ----- reads the current shaft angle with 2*FPID -----
// ----- Oversamples the shaft angle to reduce noise -----
void TC4_Handler() {
  static byte counter;
  static int y_temp;

  if (TC4->COUNT16.INTFLAG.bit.OVF == 1) {

    counter++;

    switch (counter) {
      case 1:
        y_temp = readAngle(y, raw_0);
        break;
      case 2:
        y = (y_temp + readAngle(y, raw_0)) / 2;
        raw_0 = mod(y, 36000);
        counter = 0;
        break;
    }

    TC4->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

