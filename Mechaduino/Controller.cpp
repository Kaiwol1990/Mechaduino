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



const int pLPFa = ((100 * exp(pLPF * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
const int pLPFb = ((100 - pLPFa) + 0.5);

const int ITerm_max = uMAX * 1000;



void TC5_Handler() {
  // gets called with PID frequency

  //----- Variables -----
  static int_fast32_t ITerm;
  static int_fast32_t DTerm;

  int_fast16_t raw_0;            // current measured angle
  static int_fast16_t raw_1;     // last measured angle

  int_fast32_t e_0;               // current error term
  static int_fast32_t e_1;        // last error term
  static int_fast32_t y_1;        // last wrapped angle
  static int_fast32_t r_1;        // last target

  static int_fast16_t last_phase_advanced;


  int_fast16_t omega_target;           // target angle velocity
  static int_fast16_t omega_target_1;  // last target angle velocity
  int_fast16_t omega_dot_target;       // derivation of the target angle velocity




  //----- Calculations -----

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    r = (step_target * stepangle) / 100;

    omega_target = (r - r_1); //target angular velocity

    omega_dot_target =  (omega_target - omega_target_1); //target angular acceleration

    y = readAngle(y_1, raw_1);

    raw_0 = mod(y, 36000);



    if (enabled) {

      e_0 = (r - y);

      ITerm = ITerm + (int_Ki * e_0);


      if (ITerm > ITerm_max) {
        ITerm = ITerm_max;
      }
      else if (ITerm < -ITerm_max) {
        ITerm = -ITerm_max;
      }

      DTerm = (pLPFa * DTerm + (pLPFb * int_Kd * (e_0 - e_1))) / 100;

      // PID loop                         +    moment of inertia
      u = ((int_Kp * e_0) + ITerm + DTerm + (int_J * omega_dot_target)) / 1000;

    }
    else {
      step_target = ( (100 * y) / stepangle);
      e_0 = 0;
      u = 0;
      ITerm = 0;
    }

    //int phase_advanced = e_0 + omega_target;
    //int_fast16_t phase_advanced = e_0 + omega_target;

    int_fast16_t phase_advanced = ((sign(u) * PA) / 4) + e_0 + (omega_target);

    if (phase_advanced >= PA) {
      phase_advanced = PA;
    }
    else if (phase_advanced <= -PA) {
      phase_advanced = -PA;
    }





    if (u > uMAX) {
      u = uMAX;
    }
    else if (u < -uMAX) {
      u = -uMAX;
    }


    electric_angle = -(raw_0 + phase_advanced);
    output(electric_angle, abs(u));


    raw_1 = raw_0;
    e_1 = e_0;
    y_1 = y;


    r_1 = r;
    omega_target_1 = omega_target;



    if (abs(e_0) < max_e) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
    }


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

