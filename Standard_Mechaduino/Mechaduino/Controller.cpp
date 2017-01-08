//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

//#include <SPI.h>
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
  static int ITerm;
  static int DTerm;
  static int u_1;

  int raw_0;            // current measured angle
  static int raw_1;     // last measured angle
  int raw_diff;         // diff of both


  int e_0;               // current error term
  static int e_1;               // last error term
  static int y_1;
  static int r_1;

  int target_raw;
  static int target_raw_1;
  int omega;
  int omega_target;
  static int omega_target_1;
  int omega_dot_target;

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    target_raw = (step_target * stepangle) / 100;

    if (!anticogging) {
      r = (RASa * r_1 + RASb * target_raw) / 1000;
    }

    //omega_target =  (target_raw - target_raw_1); //target angular velocity
    omega_target =  (r - r_1); //target angular velocity

    omega_dot_target =  (omega_target - omega_target_1); //target angular acceleration

    y = readAngle(y_1, raw_1);

    raw_0 = mod(y, 36000);

    if (enabled) {

      e_0 = r - y;


      ITerm = ITerm + e_0;

      if (ITerm > ITerm_max) {
        ITerm = ITerm_max;
      }
      else if (ITerm < -ITerm_max) {
        ITerm = -ITerm_max;
      }

      omega = (y - y_1);

      DTerm = (pLPFa * DTerm -  (pLPFb * int_Kd * omega)) / 1000;


      // PID loop                                     +    feedforward term
      u = ( (int_Kp * e_0) + (int_Ki * ITerm) + DTerm + (int_Kvff * (omega_target - omega))) / 1000;

      // moment of inertia
      u = u + (J * omega_dot_target ^ 2);

      u = (uLPFa * u_1 + uLPFb * u) / 1000;


      // friction compensation
      if (abs(omega_target) > 0) {
        u = u + (omega_target / abs(omega_target)) * Kfr;
        //u = u + (e_0 / abs(e_0)) * Kfr;
      }


    }
    else {
      step_target = ( (100 * y) / stepangle);
      e_0 = 0;
      u = 0;
      ITerm = 0;
    }



    if (u > 0) {

      if (u > uMAX) {
        u = uMAX;
      }

      output(-(raw_0 + PA), abs(u));
    }
    else {

      if (u < -uMAX) {
        u = -uMAX;
      }

      output(-(raw_0 - PA), abs(u));
    }

    raw_1 = raw_0;
    e_1 = e_0;
    y_1 = y;
    u_1 = u;
    r_1 = r;
    target_raw_1 = target_raw; //letztes target
    omega_target_1 = omega_target;


    if (abs(e_0) < 10) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
    }

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

