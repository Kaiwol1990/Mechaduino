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

  j  -  generate step response for PID tuning

  m  -  print main menu

  f  -  get max loop frequency

  ...see serialCheck() in Utils for more details
*/

#include "SanityCheck.h"
#include "Parameters.h"
#include "State.h"
#include "Utils.h"


//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {

  SerialUSB.begin(baudrate);

  setupPins();
  setupSPI();
  setupTCInterrupts();

  delay(1000);

  int i = 0;
  raw_0 = (pgm_read_word_near(lookup + readEncoder()));
  raw_1 = raw_0;

  while (i < 200) {
    raw_0 = (pgm_read_word_near(lookup + readEncoder()));

    raw_diff = raw_0 - raw_1;

    if (raw_diff < -18000) {
      y = y + 36000 + raw_diff;
    }
    else if (raw_diff > 18000) {
      y = y - 36000 + raw_diff;
    }
    else {
      y = y  + raw_diff;
    }
    raw_1 = raw_0;
    i++;
  }

  step_target = ((y + 5) / stepangle);

  delay(500);

  if (use_enable_pin == true) {
    if (REG_PORT_IN0 & PORT_PA14) { // check if ena_pin is HIGH
      enabled = false;
    }
    else {
      enabled = true;
    }
  }
  else
  {
    enabled = true;
  }

  if (REG_PORT_IN0 & PORT_PA11) { // check if dir_pin is HIGH
    dir = false;
  }
  else {
    dir = true;
  }

  enableTC5Interrupts(); // get the filter going and ge samples for 1 second

  digitalWrite(ledPin, HIGH);

  Serial_menu();
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();
  //SerialUSB.println(e_0);
  //SerialUSB.println(y);
  //SerialUSB.println(reading);
  //SerialUSB.println(raw_diff);
  //SerialUSB.println(u);
  //SerialUSB.println(readEncoder());
  //SerialUSB.println(raw_0);
  //SerialUSB.println(r);
  //SerialUSB.println(step_target);
}
