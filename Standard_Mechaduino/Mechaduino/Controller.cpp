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

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    r = (step_target * stepangle) / 100;

    raw_0 = pgm_read_word_near(lookup + readEncoder());


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


    if (enabled) {

      e_0 = r - y;

      ITerm = ITerm + e_0;

      if (ITerm > ITerm_max) {
        ITerm = ITerm_max;
      }
      else if (ITerm < -ITerm_max) {
        ITerm = -ITerm_max;
      }
      if (abs(e_0) > 150) {
        u = ( (big_Kp * e_0) + (big_Ki * ITerm) + (big_Kd * (e_0 - e_1)) ) / 1000;
      }
      else {
        u = ( (small_Kp * e_0) + (small_Ki * ITerm) + (small_Kd * (e_0 - e_1)) ) / 1000;
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

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

