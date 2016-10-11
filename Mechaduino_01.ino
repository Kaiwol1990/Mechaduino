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


//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {


  setupPins();
  setupSPI();
  setupTCInterrupts();

  SerialUSB.begin(250000);
  //SerialUSB.begin(115200);

  // while (!SerialUSB) {};     //wait for serial

  delay(5000);  //This delay seems to make it easier to establish a conncetion when the Mechaduino is configured to start in closed loop mode.


  //  enableTCInterrupts();     //start in closed loop mode
  // mode = 'x';
  //
  //  Wire.begin(4);                // join i2c bus with address #8
  //  Wire.onReceive(receiveEvent); // register event


  SerialUSB.println("Mechaduino 0.1 begin...");

}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{



  serialCheck();           //checks the serial port for commands

  //r = stepangle * step_count; //step_count is updated by the D1 pin interrupt


}
