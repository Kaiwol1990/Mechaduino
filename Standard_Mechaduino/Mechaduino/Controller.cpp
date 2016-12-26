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

  int raw_0;            // current measured angle
  static int raw_1;     // last measured angle
  int raw_diff;         // diff of both


  int e_0;               // current error term
  static int e_1;               // last error term
  static int y_1;

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    r = (step_target * stepangle) / 100;

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

      u = ( (int_Kp * e_0) + (int_Ki * ITerm) + (int_Kd * (e_0 - e_1)) ) / 1000;


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

      //output(-(raw_0 + PA), abs(u));
      output(-(raw_0 + PA), abs(u));
    }
    else {

      if (u < -uMAX) {
        u = -uMAX;
      }

      //output(-(raw_0 - PA), abs(u));
      output(-(raw_0 - PA), abs(u));
    }

    raw_1 = raw_0;
    e_1 = e_0;
    y_1 = y;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

