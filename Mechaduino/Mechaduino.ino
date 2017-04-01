 /*
  Mechaduino Firmware  --multi file branch

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
  SerialUSB.println(":>");

}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();
  Streaming();

}
