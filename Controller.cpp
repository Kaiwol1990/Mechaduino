//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include <SPI.h>
#include "State.h"
#include "Utils.h"
#include "Parameters.h"

void TC5_Handler()
{
  if (TC5->COUNT16.INTFLAG.bit.OVF == 1) {  // A overflow caused the interrupt
    y = lookup_angle(readEncoder());


    if ((y - y_1) < -180.0) {
      wrap_count += 1;
    }
    else if ((y - y_1) > 180.0) {
      wrap_count -= 1;
    }
    yw = (y + (360.0 * wrap_count));



    if (enabled == true) {
      e = (r - yw);

      ITerm = (ITerm + e);

      u = ((pKp * e) + (pKi * ITerm) + (pKd * (e - e_1)));
    }
    else {
      r = yw;
      u = 0;
      ITerm = 0;
    }


    if (abs(u) < 1.3 * uMAX) {
      u = constrain(u, -uMAX, uMAX);
      PEAKCounter -= 1;
    }
    else {
      if ((PEAKCounter + uSTEP) <= maxPEAKCounter) {
        u = constrain(u, -uPEAK, uPEAK);
        PEAKCounter += uSTEP;
      }
      else {
        u = constrain(u, -uMAX, uMAX);
        PEAKCounter -= 1;
      }
    }

    if (u > 0) {
      output(-y - PA, abs(u));
    }
    else {
      output(-y + PA, abs(u));
    }

    if (e <= 0.1) {
      REG_PORT_DIRSET0 = PORT_PA17;
    }
    else {
      REG_PORT_OUTCLR0 = PORT_PA17;
    }

    y_1 = y;
    e_1 = e;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}
