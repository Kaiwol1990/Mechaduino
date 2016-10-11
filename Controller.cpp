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
          e_0 = (r - yw);

          // IIR Filter for the error
          e = coeff_b0 * e_0 +  coeff_b1 * e_1 + coeff_b2 * e_2  - coeff_a1 * z_1 - coeff_a2 * z_2;

          z_2 = z_1;
          z_1 = e;

          e_2 = e_1;
          e_1 = e_0;
          Serial.print(e_0);
          Serial.print(" , ");
          Serial.println(e);

          ITerm += (pKi * e);
          ITerm = sign(ITerm) * constrain(abs(ITerm), 0, 150);
          //if (ITerm > 150) ITerm = 150;
          //else if (ITerm < -150) ITerm = -150;

          u =  ((pKp * e) + ITerm - (pKd * (yw - yw_1))); //ARDUINO library style
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
      u = 0; //set effort to 0 if not enabled
      r = yw; //set current position to target position
    }

    if (u < 1.3 * uMAX) {
      u = constrain(abs(u), 0, uMAX);
      PEAKCounter -= 1;
    }
    else {
      if (PEAKCounter < maxPEAKCounter) {
        u = constrain(abs(u), 0, uPEAK);
        PEAKCounter += uSTEP;
      }
      else {
        u = constrain(abs(u), 0, uMAX);
        PEAKCounter -= 1;
      }
    }

    PEAKCounter = constrain(PEAKCounter, 0, 2 * maxPEAKCounter);


    //Turn LED On if Error is below 0.1
    /*
      if (abs(e) < 0.1) {
        digitalWrite(LED, HIGH);
      }
      else  {
        digitalWrite(LED, LOW);
      }
    */

    if (u > 0) {
      //y += PA;
      output(-y + PA, u);
    }
    else {
      //y -= PA;
      output(-y - PA, u);
    }
    //output(-y, U);  //-y


    yw_1 = yw;
    y_1 = y;


    TC5->COUNT16.INTFLAG.bit.OVF = 1;    // writing a one clears the flag ovf flag
  }


}
