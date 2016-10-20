
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

  delay(1000);  //This delay seems to make it easier to establish a conncetion when the Mechaduino is configured to start in closed loop mode.

  SerialUSB.println("Mechaduino 0.1 begin...");
  SerialUSB.println("Postion: X...");

  enableTCInterrupts();     //start in closed loop mode
  mode = 'x';

}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{

  serialCheck();           //checks the serial port for commands
  SerialUSB.println(e);

}
