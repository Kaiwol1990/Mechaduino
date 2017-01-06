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
#include "Configuration.h"
#include "Configurationals.h"
#include "State.h"
#include "Utils.h"
#include "board.h"
#include "analogFastWrite.h"
#include "lookup_table.h"
#include "Encoder.h"
#include "Serial.h"





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
  int raw_0 = (pgm_read_word_near(lookup + readEncoder()));
  int raw_1 = raw_0;
  int raw_diff;

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

#ifdef USE_ENABLE_PIN
  enaInterrupt();
#else
  enabled = true;
#endif

  dirInterrupt();

  enableTC5Interrupts(); // get the filter going and ge samples for 1 second

  SerialUSB.println("----- Mechaduino -----");
  SerialUSB.print("Firmware: ");
  SerialUSB.println(firmware_version);
  SerialUSB.print("Identifier: ");
  SerialUSB.println(identifier);
  SerialUSB.println("");

  Serial_menu();

  SerialUSB.println();
  SerialUSB.print(":>");
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();

  if (enabled) {
    REG_PORT_OUTSET0 = PORT_PA17;     //write LED HIGH
  }
  else {
    REG_PORT_OUTCLR0 = PORT_PA17;     //write LED LOW
  }

  //SerialUSB.print(micros());
  //SerialUSB.print(',');
  //SerialUSB.println(y);
  //SerialUSB.println((r-y)/10.0);
  //SerialUSB.println(readEncoder());
  //SerialUSB.println(r);
  //SerialUSB.println(step_target);
}
