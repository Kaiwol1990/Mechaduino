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

unsigned long starting;

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {

  SerialUSB.begin(250000);
 REG_PORT_DIRSET0 = PORT_PA17;
  delay(3000);

  SerialUSB.println("Mechaduino 0.1");
  SerialUSB.print("Identifier: ");
  SerialUSB.println(identifier);

  setupPins();
  setupSPI();
  setupTCInterrupts();

  for (int k = 1 ; k < 100; k++) {
    delay(10);
    y = y + lookup_angle(readEncoder());
  }
  y = (y / 100);
  y_1 = y;
  yw = y;
  r = y;


  if (digitalRead(ena_pin) == 1) { //read current enable setting
    enabled = false;
  }
  else {
    enabled = true;
  }

  if (digitalRead(dir_pin)) { //read current direction setting
    dir = false;
  }
  else {
    dir = true;
  }

  enableTCInterrupts();     //start in closed loop mode

  digitalWrite(ledPin, HIGH);

  SerialUSB.println("ready!");
  SerialUSB.println("");
  SerialUSB.println("");
  
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();
}
