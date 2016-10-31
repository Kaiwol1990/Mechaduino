//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include <SPI.h>
#include "State.h"
#include "Utils.h"
#include "Parameters.h"
#include "macros.h"

void TC5_Handler() {
  // gets called with FPID frequency

  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    if (enabled || ena_pin == -1) {

      e_0 = (r - y);;

      ITerm = (ITerm + e_0);

      if (ITerm > 150) {
        ITerm = 150;
      }
      else if (ITerm < -150) {
        ITerm = -150;
      }


      u = ((pKp * e_0) + (pKi * ITerm) + (pKd * (e_0 - e_1)));
    }
    else {
      r = y;
      e_0 = 0;
      u = 0;
      ITerm = 0;
    }


    if (u > uMAX) {
      u = uMAX;
    }
    else if (u < -uMAX) {
      u = -uMAX;
    }

    if (u > 0) {
      output(-(raw_0 / 100.0) - PA, abs(u));
    }
    else {
      output(-(raw_0 / 100.0) + PA, abs(u));
    }

    e_1 = e_0;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}



void TC4_Handler() {
  // gets called with FSAMPLE frequency

  if (TC4->COUNT16.INTFLAG.bit.OVF == 1) {  // A overflow caused the interrupt

    int temp = readEncoder();
    raw_0 = (100 * pgm_read_float_near(lookup + temp));

    raw_diff = raw_0 - raw_1;

    if (raw_diff < -18000) {
      yw_0 = yw_0 + 36000 + raw_diff;
    }
    else if (raw_diff > 18000) {
      yw_0 = yw_0 - 36000 + raw_diff;
    }
    else {
      yw_0 = yw_0  + raw_diff;
    }

    sum -= LM[pointer];
    LM[pointer] = yw_0;
    sum += LM[pointer];
    pointer++;
    pointer = pointer % LM_SIZE;

    y = ((sum >> shifts) / 100.0);

    raw_1 = raw_0;

    TC4->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}


