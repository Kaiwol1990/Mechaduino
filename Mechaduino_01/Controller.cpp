//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include <SPI.h>
#include "State.h"
#include "Utils.h"
#include "Parameters.h"
#include "macros.h"

void TC5_Handler()
{
  if (TC5->COUNT16.INTFLAG.bit.OVF == 1  || frequency_test == true) {  // A overflow caused the interrupt

    y = pgm_read_float_near(lookup + readEncoder());

    if ((y - y_1) < -180.0) {
      yw = yw + 360 + (y - y_1);
    }
    else if ((y - y_1) > 180.0) {
      yw = yw - 360 + (y - y_1);
    }
    else {
      yw = yw  + (y - y_1);
    }

    y_filtered_0 =  (coeff_b0 * yw) + (coeff_b1 * yw_1) + (coeff_b2 * yw_2) - (coeff_a1 * y_filtered_1) - (coeff_a2 * y_filtered_2);

    if (enabled || ena_pin == -1) {

      e = (r - y_filtered_0);

      ITerm = (ITerm + e);

      if (ITerm > 150) {
        ITerm = 150;
      }
      else if (ITerm < -150) {
        ITerm = -150;
      }

      //u = ((pKp * e) + (pKi * ITerm) - (pKd * (yw - yw_1))); //ARDUINO library style

      u = ((pKp * e) + (pKi * ITerm) + (pKd * (e - e_1)));

    }
    else {
      r = yw;
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
      output(-y - PA, abs(u));
    }
    else {
      output(-y + PA, abs(u));
    }

    if (abs(e) < 0.1) {
      REG_PORT_OUTSET0 = PORT_PA17;
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;
    }

    y_filtered_2 = y_filtered_1;
    y_filtered_1 = y_filtered_0;

    yw_2 = yw_1;
    yw_1 = yw;

    y_1 = y;
    
    e_1 = e;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}
