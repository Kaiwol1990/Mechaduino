/*
  Mechaduino 0.1 Firmware  --multi file branch

  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.



  Controlled via a SerialUSB terminal at 250000 baud.

  Implemented serial commands are:

  c  -  encoder calibration routine

  s  -  enter new setpoint

  p  -  parameter query (prints current parameters)

  e  -  parameter edit (edit PID values)

  a  -  anticogging calibration

  j  -  generate step response for PID tuning

  m  -  print main menu

  f  -  get max loop frequency

  o  -  set filter frequency

  ...see serialCheck() in Utils for more details
*/
#include "SanityCheck.h"
#include "Utils.h"
#include "Parameters.h"
#include "State.h"
#include "analogFastWrite.h"
#include "macros.h"

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {

  SerialUSB.begin(baudrate);

  digitalWrite(ledPin, HIGH);

  SerialUSB.println("booting");
  delay(3000);

  setupPins();
  setupSPI();
  setupTCInterrupts();
  calcBiquad(Fc, Fs, 0.33);

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

  Serial_menu();
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();
  //SerialUSB.println(1000 * e);
  //SerialUSB.println(yw);

}
