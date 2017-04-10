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



int pLPFa = ((100 * exp(pLPF * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
int pLPFb = ((100 - pLPFa) + 0.5);




float  Tmax = 0.1;
float zr = exp(-(1.0 / (FPID * Tmax)));
float J = (J_rotor + J_load) * (1 / (1000.0 * 100.0 * 100.0));

float a = -(1 / J) * (1.0 / FPID);

int theta = (10000 * exp(a)) + 0.5;
int h = (10000 * (((1000 * M_max) / I_rated) * (1.0 - exp(-(1 / theta) * (1.0 / FPID))))) + 0.5;

int r_k = (1000 * ((theta - zr) / h)) + 0.5;
int v  = (1000 * ((1.0 - zr) / h)) + 0.5;


const int_fast16_t from_i_to_u = ((100.0 * ((512.0 * 10.0 * rSense) / (1000.0 * 3.3))) + 0.5);


void TC5_Handler() {
  // gets called with PID frequency

  //----- Variables -----
  static int_fast32_t ITerm;
  static int_fast32_t DTerm;

  static int_fast32_t ITerm_omega;

  int_fast16_t raw_0;            // current measured angle
  static int_fast16_t raw_1;     // last measured angle

  int_fast32_t e_0;               // current error term
  static int_fast32_t e_1;        // last error term
  static int_fast32_t y_1;        // last wrapped angle
  static int_fast32_t r_1;        // last target
  static int_fast32_t e_omega;        // last target


  int_fast16_t omega_target;    // target angle velocity
  // int_fast16_t omega;           // angle velocity

  int_fast16_t phase_advanced;


  static int_fast32_t x_k;



  //----- Calculations -----

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    r = step_target * stepangle;

    omega_target = (r - r_1); //target angular velocity

    y = readAngle(y_1, raw_1);

    raw_0 = mod(y, 36000);

    //omega = (y - y_1);

    if (enabled) {

      e_0 = (r - y);

      //state controller for omega
      int_fast32_t u_omega = (((v * omega_target) - (r_k * x_k)) * from_i_to_u) / 100000;



      // PID loop
      ITerm = ITerm + (int_Ki * e_0);
      DTerm = ((pLPFa * DTerm) + (pLPFb * int_Kd * (e_0 - e_1))) / 100;

      if (ITerm > ITerm_max) {
        ITerm = ITerm_max;
      }
      else if (ITerm < -ITerm_max) {
        ITerm = -ITerm_max;
      }

      int_fast32_t u_pid = ((int_Kp * e_0) + ITerm + DTerm) / 1000;


      // correct the needed current
      u = (u_pid + u_omega);

      
    }
    else {
      step_target = y / stepangle;
      e_0 = 0;
      u = 0;
      ITerm = 0;
      ITerm_omega = 0;
      e_omega = 0;
    }




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


    if (u > uMAX) {
      u = uMAX;
    }
    else if (u < -uMAX) {
      u = -uMAX;
    }


    electric_angle = -(raw_0 + phase_advanced);

    output(electric_angle, abs(u));



    // calculate the future omega value for the state controller
    x_k = ((theta * x_k) + (h * u)) / 10000;

    raw_1 = raw_0;
    e_1 = e_0;
    y_1 = y;
    r_1 = r;


    if (abs(e_0) < max_e) {
      REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
    }


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

