/*
  Mechaduino 0.1 Firmware  --multi file branch

  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.



  Controlled via a SerialUSB terminal at 250000 baud.

  Implemented serial commands are:

  c  -  encoder cal routine

  s  -  enter new setpoint

  p  - parameter query (prints current parameters)

  e  - parameter edit (edit PID values)

  a  - anticogging calibration

  j  - generate step response for PID tuning

  m  - print main menu

  ...see serialCheck() in Utils for more details
*/

#include "Utils.h"
#include "Parameters.h"
#include "state.h"
#include "analogFastWrite.h"
#include "macros.h"

//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {

  SerialUSB.begin(250000);
  REG_PORT_DIRSET0 = PORT_PA17;
  SerialUSB.println("booting");
  delay(3000);

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

  #if PIN_EXISTS(ena_pin)
    if (digitalRead(ena_pin) == 1) { //read current enable setting
      enabled = false;
    }
    else {
      enabled = true;
    }
  #endif

  if (digitalRead(dir_pin)) { //read current direction setting
    dir = false;
  }
  else {
    dir = true;
  }

  enableTCInterrupts();     //start in closed loop mode

  digitalWrite(ledPin, HIGH);

  Serial_menu();
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();

  /*

    if (millis() > next_millis) {
    Serial_menu();
    next_millis = millis() + 5000;
    }

    int i = 1;
    int max_counter = 10000;
    unsigned long starting = micros();

    while (i <= max_counter) {
    TC5_Handler();
    serialCheck();
    i++;
    }

    int frequenz = 1000000 / ((micros() - starting) / max_counter);

    SerialUSB.println(frequenz);
    SerialUSB.println(0.95*frequenz);
  */
}
