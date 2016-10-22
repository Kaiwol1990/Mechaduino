/*
  Mechaduino 0.1 Firmware  --multi file branch

  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.



  Controlled via a SerialUSB terminal at 250000 baud.

  Implemented serial commands are:

  w  -  encoder cal routine

  r  -  enter new setpoint

  q  - parameter query (prints current parameters)

  k  - parameter edit (edit PID values)

  a  - anticogging calibration

  s  - generate step response for PID tuning

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

  SerialUSB.begin(250000);
  delay(3000);

  SerialUSB.println("Mechaduino 0.1");
  SerialUSB.print("Identifier: ");
  SerialUSB.println(identifier);

  setupPins();
  setupSPI();
  setupTCInterrupts();

  for (int k = 1 ; k < 100; k++) {
    delay(10);
    a = readEncoder();
    y = y + lookup_angle(a);
  }
  y = (y / 100);
  y_1 = y;
  yw = y;
  r = y;


  if (digitalRead(ena_pin) == 1) { //read current enable setting
    enabled = 0;
  }
  else {
    enabled = 1;
  }

  if (digitalRead(dir_pin)) { //read current direction setting
    dir = -1;
  }
  else {
    dir = 1;
  }

  enableTCInterrupts();     //start in closed loop mode

  SerialUSB.println("ready!");
  SerialUSB.println("");
  SerialUSB.println("");
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();           //checks the serial port for commands,
  
}
