//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include <SPI.h>
#include "Parameters.h"
#include "State.h"
#include "Utils.h"

void TC5_Handler() {
  // gets called with PID frequency
  static int ITerm;

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    r = step_target * stepangle;

    raw_0 = (pgm_read_word_near(lookup + readEncoder()));

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


    if (!tune_running) {
      if (enabled) {

        e_0 = r - y;

        ITerm = ITerm + e_0;

        if (ITerm > ITerm_max) {
          ITerm = ITerm_max;
        }
        else if (ITerm < -ITerm_max) {
          ITerm = -ITerm_max;
        }

        u = ( (Kp * e_0) + ((Ki * ITerm)) + (Kd * (e_0 - e_1)) ) / 1000;

      }
      else {
        step_target = ((9 * step_target) + ( y / stepangle)) / 10;
        e_0 = 0;
        u = 0;
        ITerm = 0;
      }
    }


    if (u > uMAX) {
      u = uMAX;
    }
    else if (u < -uMAX) {
      u = -uMAX;
    }

    if (u > 0) {
      output(-(raw_0 + PA), abs(u));
    }
    else {
      output(-(raw_0 - PA), abs(u));
    }

    e_1 = e_0;
    raw_1 = raw_0;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}

