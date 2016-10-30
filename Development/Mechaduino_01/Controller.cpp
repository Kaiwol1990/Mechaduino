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

      e_0 = (r - y_filtered_0);

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
      r = yw_0;
      u = 0;
      ITerm = 0;
    }


    if (abs(u) < 1.3 * uMAX) {

      if (u > uMAX) {
        u = uMAX;
      }
      else if (u < -uMAX) {
        u = -uMAX;
      }

      PEAKCounter -= 1;
    }
    else {
      if ((PEAKCounter + uSTEP) <= maxPEAKCounter) {

        if (u > uPEAK) {
          u = uPEAK;
        }
        else if (u < -uPEAK) {
          u = -uPEAK;
        }

        PEAKCounter += uSTEP;
      }
      else {

        if (u > uMAX) {
          u = uMAX;
        }
        else if (u < -uMAX) {
          u = -uMAX;
        }

        PEAKCounter -= 1;
      }
    }

    if (u > 0) {
      output(-raw_0 - PA, abs(u));
    }
    else {
      output(-raw_0 + PA, abs(u));
    }



    e_1 = e_0;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}



void TC4_Handler() {
  // gets called with FSAMPLE frequency

  if (TC4->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    raw_0 = pgm_read_float_near(lookup + readEncoder());
    raw_diff = raw_0 - raw_1;

    if (raw_diff < -180.0) {
      yw_0 = yw_1 + 360 + raw_diff;
    }
    else if (raw_diff > 180.0) {
      yw_0 = yw_1 - 360 + raw_diff;
    }
    else {
      yw_0 = yw_1  + raw_diff;
    }

    y_filtered_0 =  (coeff_b0 * yw_0) + (coeff_b1 * yw_1) + (coeff_b2 * yw_2) - (coeff_a1 * y_filtered_1) - (coeff_a2 * y_filtered_2);

    y_filtered_2 = y_filtered_1;
    y_filtered_1 = y_filtered_0;

    yw_2 = yw_1;
    yw_1 = yw_0;

    raw_1 = raw_0;


    TC4->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}


