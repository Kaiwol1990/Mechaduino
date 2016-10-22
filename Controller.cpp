//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:

#include <SPI.h>
#include "State.h"
#include "Utils.h"
#include "Parameters.h"

void TC5_Handler()
{
  if (TC5->COUNT16.INTFLAG.bit.OVF == 1) {  // A overflow caused the interrupt

    //a = readEncoder();
    //y = lookup_angle(a);
    y = lookup_angle(readEncoder());


    if ((y - y_1) < -180.0) {
      wrap_count += 1;
    }
    else if ((y - y_1) > 180.0) {
      wrap_count -= 1;
    }
    yw = (y + (360.0 * wrap_count));



    if (enabled == 1) {
      raw_0 = (r - yw);

      e = (coeff_b0 * raw_0) + (coeff_b1 * raw_1) + (coeff_b2 * raw_2)  - (coeff_a1 * e_1) - (coeff_a2 * e_2);

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


    if (abs(e) < 0.05) {
      digitalWrite(ledPin, HIGH);
    }
    else  {
      digitalWrite(ledPin, LOW);
    }

    y_1 = y;
    raw_2 = raw_1;
    raw_1 = raw_0;
    e_2 = e_1;
    e_1 = e;

    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }
}
