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



void TC5_Handler() {
  // gets called with PID frequency

  //----- Variables -----
  static int ITerm;
  static int DTerm;


  int raw_0;            // current measured angle
  static int raw_1;     // last measured angle

  int e_0;               // current error term
  static int e_1;        // last error term
  static int y_1;        // last wrapped angle
  static int r_1;        // target 1 loop ago
  static int u_1;
  //static int u_2;
  static int u_raw;
  /*static int u_raw_1;
    static int u_raw_2;*/


  int omega_target;    // target angle velocity
  static int omega_target_1;

  int phase_advanced;


  //----- Calculations -----

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    // calculate the current target from the received steps and the angle per step (+0.5 for fixed point arithmetics)
    r = (step_target * stepangle) + 0.5;

    // target angular velocity buffered over two sample times
    omega_target = (r - r_1) + 0.5;

    // read the current wrapped angle
    y = readAngle(y_1, raw_1);

    // calculate the curren shaft angle
    raw_0 = mod(y, 36000);



    // start the calculations only if the motor is enabled
    if (enabled) {

      // ----------- PID loop ----------
      // -------------------------------
      // calculate the error
      e_0 = (r_1 - y);

      // calculate the I- and DTerm
      ITerm = ITerm + (int_Ki * e_0);
      DTerm = ((D_Term_LPFa * DTerm) + (D_Term_LPFb * int_Kd * (e_0 - e_1))) / 128;

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

      if (omega_target != 0) {
        u_raw = ( ((int_Kp * e_0) + ITerm + DTerm) + (omega_target * int_Kvff) + ((omega_target - omega_target_1) * int_J) + (sign(omega_target) * int_Kff)  ) / 1024;
      }
      else {
        u_raw = ( ((int_Kp * e_0) + ITerm + DTerm) + (omega_target * int_Kvff) + ((omega_target - omega_target_1) * int_J) +  0                              ) / 1024;

      }

      u = ((u_LPFa * u_1) + (u_LPFb * u_raw)) / 128;

      //biquad filter
      //u = (b_0 * u_raw + b_1 * u_raw_1 + b_2 * u_raw_2 - a_1 * u_1 + a_2 * u_2) / 128;



    }
    else {
      step_target = y / stepangle;
      e_0 = 0;
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
      phase_advanced = ((sign(u) * PA) / 4 ) + e_0;

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
    output(electric_angle, abs(u));




    // buffer the variables for the next loop
    raw_1 = raw_0;
    e_1 = e_0;
    y_1 = y;
    r_1 = r;
    u_1 = u;
    /*u_2 = u_1;
      u_raw_1 = u_raw;
      u_raw_2 = u_raw_1;*/
    omega_target_1 = omega_target;

    //wrtie the max error led high or low
    if (abs(e_0) < max_e) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
    }


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

