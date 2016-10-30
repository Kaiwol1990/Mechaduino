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

  setupPins();
  setupSPI();
  setupTCInterrupts();
  calcBiquad(Fc, FSAMPLE);

  // get the filter going and ge samples for 1 second
  for (int k = 0 ; k < FSAMPLE; k++) {
    raw_0 = pgm_read_float_near(lookup + readEncoder());
    raw_diff = raw_0 - raw_1;

    yw_0 = yw_1  + raw_diff;

    y_filtered_0 =  (coeff_b0 * yw_0) + (coeff_b1 * yw_1) + (coeff_b2 * yw_2) - (coeff_a1 * y_filtered_1) - (coeff_a2 * y_filtered_2);

    y_filtered_2 = y_filtered_1;
    y_filtered_1 = y_filtered_0;
    yw_2 = yw_1;
    yw_1 = yw_0;
    raw_1 = raw_0;
  }

  r = y_filtered_0;


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

  enableTC4Interrupts();     //start in closed loop mode
  enableTC5Interrupts();     //start in closed loop mode

  Serial_menu();
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();
  //SerialUSB.println(1000 * e_0);
  //SerialUSB.println(y_filtered_0);

}
