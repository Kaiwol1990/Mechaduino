<<<<<<< HEAD
/*
  Mechaduino 0.1 Firmware  --multi file branch

  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.


  Controlled via a SerialUSB terminal at 250000 baud.
  ...send help to get all commands

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
#include "Language.h"





//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {
  boot();

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

  //SerialUSB.println((r-y));
  //SerialUSB.println(u_filtered);
  //SerialUSB.println(y);
  //SerialUSB.println(u);
  //SerialUSB.println(r);
  //SerialUSB.println(step_target);
=======
/*
  Mechaduino 0.1 Firmware  --multi file branch

  SAM21D18 (Arduino Zero compatible), AS5047 encoder, A4954 driver



  All Mechaduino related materials are released under the

  Creative Commons Attribution Share-Alike 4.0 License

  https://creativecommons.org/licenses/by-sa/4.0/



  Many thanks to Will Church and Marco Farrugia.


  Controlled via a SerialUSB terminal at 250000 baud.
  ...send help to get all commands

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

  delay(500);


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

  step_target = ((y) / stepangle);

#ifdef USE_ENABLE_PIN
  enaInterrupt();
#else
  enabled = true;
#endif

  dirInterrupt();

  delay(100);

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

  //SerialUSB.println((r-y));
  //SerialUSB.println(u_filtered);
  //SerialUSB.println(y);
  //SerialUSB.println(u);
  //SerialUSB.println(r);
  //SerialUSB.println(step_target);
>>>>>>> origin/dev
}
