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


const int uLPFa = ((100 * exp(uLPF * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
const int uLPFb = ((100 - uLPFa) + 0.5);

const int RASa = ((100 * exp((1000 / RAS) * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
const int RASb = ((100 - RASa) + 0.5);

const int pLPFa = ((100 * exp(pLPF * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
const int pLPFb = ((100 - pLPFa) + 0.5);



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
  static long y_1;
  static long r_1;

  long target_raw;
  static long target_raw_1;
  int omega;
  int omega_target;
  static int omega_target_1;
  int omega_dot_target;

  static int print_counter;

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    target_raw = (step_target * stepangle) / 100;

    r = (RASa * r_1 + RASb * target_raw) / 100;

    omega_target = (r - r_1); //target angular velocity

    omega_dot_target =  (omega_target - omega_target_1); //target angular acceleration

    y = readAngle(y_1, raw_1);

    omega = (y - y_1);

    raw_0 = mod(y, 36000);

    uint8_t omega_target_abs = abs(omega_target);

    if (enabled) {

      e_0 = (r - y);

      ITerm = ITerm + (int_Ki * e_0);

      if (ITerm > 150000) {
        ITerm = 150000;
      }
      else if (ITerm < -150000) {
        ITerm = -150000;
      }


      DTerm = (pLPFa * DTerm + (pLPFb * int_Kd * (e_0 - e_1))) / 100;


      // PID loop                          +    feedforward term                 +    moment of inertia
      u = ( (int_Kp * e_0) + ITerm + DTerm + (int_Kvff * (omega_target - omega)) + (int_J * omega_dot_target) );


      // friction compensation
      if (omega_target > 4) {
        u = u + int_Kfr;
      }
      else if (omega_target < -4) {
        u = u - int_Kfr;
      }


      u = u / 1000;

      u = (uLPFa * u_1 + uLPFb * u) / 100;


    }
    else {
      step_target = ( (100 * y) / stepangle);
      e_0 = 0;
      u = 0;
      ITerm = 0;
    }


    uint8_t phase_advanced = PA;
    if (omega_target >= 30) {
      phase_advanced = PA + omega_target_abs;
    }

    if (u >= 0) {

      if (u > uMAX) {
        u = uMAX;
      }

      output(-(raw_0 + phase_advanced), u);
    }
    else {

      if (u < -uMAX) {
        u = -uMAX;
      }

      output(-(raw_0 - phase_advanced), -u);
    }


    raw_1 = raw_0;
    e_1 = e_0;
    y_1 = y;

    u_1 = u;

    r_1 = r;
    target_raw_1 = target_raw; //letztes target
    omega_target_1 = omega_target;

    /*
        print_counter += 1;

        // print target and current angle every fifth loop
        if (print_counter >= 4) {

          SerialUSB.println(omega_target);
          print_counter = 0;
        }

    */

    // step respone active
    if (response) {
      print_counter += 1;

      // print target and current angle every fifth loop
      if (print_counter >= 4) {

        SerialUSB.println((int)y); // print current position

        print_counter = 0;
      }
    }


    if (abs(e_0) < max_e) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
    }

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

