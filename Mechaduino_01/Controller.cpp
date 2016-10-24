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

    y = lookup_angle(readEncoder());

    if ((y - y_1) < -180.0) {
      yw = yw + 360;
    }
    else if ((y - y_1) > 180.0) {
      yw = yw - 360;
    }

    yw = yw  + (y - y_1);


    //change to if(1) if you don't want to use the enable pin
    if (enabled) {

      e = (5 * e) + (5 * (r - yw));
      e = e / 10;

      ITerm = (ITerm + e);

      if (ITerm > 150) {
        ITerm = 150;
      }
      else if (ITerm < -150) {
        ITerm = -150;
      }

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

    y_1 = y;
    e_1 = e;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}
