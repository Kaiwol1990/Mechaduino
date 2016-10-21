
/*

  Mechaduino 0.1 Firmware  --multi file branch

  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver




  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.






  Controlled via a SerialUSB terminal at 115200 baud.

  Implemented serial commands are:


  s  -  step

  d  -  dir toggle

  w  -  encoder cal routine

  y  -  enable controller interrupts

  n  - disable controller interrupts

  r  -  enter new setpoint

  x  - position mode

  v  -  velocity mode

  t  -  torque mode

  q  - parameter query (prints current parameters)

  e  -  reads encoder diagnostic register

  p  -  print [step count] , [assumed angle] , [encoder reading]

  ...see serialCheck() in Utils for more details

*/
float r_0 = 0;
int jumped = 0;

#include "Utils.h"
#include "Parameters.h"
#include "state.h"
#include "analogFastWrite.h"

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {
  setupPins();
  setupSPI();
  setupTCInterrupts();

  SerialUSB.begin(115200);
  for (int k = 1 ; k < 100; k++) {
    delay(10);
    a = readEncoder();
    y = y + lookup_angle(a);
  }
  r = y / 100;
  r_0 = r;
  
  if (digitalRead(ena_pin) == 1) {
    enabled = 0;
  }
  else {
    enabled = 1;
  }

  if (digitalRead(dir_pin)) {
    dir = -1;
  }
  else {
    dir = 1;
  }
  
  ITerm = 0;

  SerialUSB.println("Mechaduino 0.1 begin...");
  SerialUSB.println("Postion: Y...");

  enableTCInterrupts();     //start in closed loop mode
  mode = 'x';

}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  /*
  if (millis() > 5000 && jumped == 0) {
    r = r_0 + 30;
    jumped = 1;
  }
*/
  serialCheck();           //checks the serial port for commands,
  /*
  SerialUSB.print(yw);
  SerialUSB.print(",");
  SerialUSB.print(r);
  SerialUSB.println();
  */
}
