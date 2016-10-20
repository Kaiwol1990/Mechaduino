//Contains TC5 Controller definition
//The main control loop is executed by the TC5 timer interrupt:


#include <SPI.h>

#include "State.h"
#include "Utils.h"
#include "Parameters.h"


void TC5_Handler()
{
  if (TC5->COUNT16.INTFLAG.bit.OVF == 1) {  // A overflow caused the interrupt

    a = readEncoder();
    y = lookup_angle(a);


    if ((y - y_1) < -180.0) {
      wrap_count += 1;
    }
    else if ((y - y_1) > 180.0) {
      wrap_count -= 1;
    }

    yw = (y + (360.0 * wrap_count));



    if (enabled == 1) {
      switch (mode) {

        case 'x':
          //e = (r - yw);
          e = (0.8 * (r - yw)) + (0.2 * e);

          ITerm = (ITerm + e);

          //ITerm = constrain(ITerm, -150, 150);

          u = ((pKp * e) + (pKi * ITerm) + (pKd * (e - e_1)));


          break;

        case 'v':


          e = (r - ((yw - yw_1) * Fs * 0.16666667)); //error in degrees per rpm (sample frequency in Hz * (60 seconds/min) / (360 degrees/rev) )

          ITerm += (vKi * e);
          if (ITerm > 200) ITerm = 200;
          else if (ITerm < -200) ITerm = -200;


          u = ((vKp * e) + ITerm - (vKd * (yw - yw_1)));//+ lookup_force(a)-20; //ARDUINO library style

          break;

        case 't':
          u = 1.0 * r ;//+ 1.7*(lookup_force(a)-20);
          break;

        default:
          u = 0;
          break;
      }
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
      digitalWrite(pulse, HIGH);
    }
    else  {
      digitalWrite(pulse, LOW);
    }



    //yw_1 = yw;
    y_1 = y;
    e_1 = e;


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }


}
