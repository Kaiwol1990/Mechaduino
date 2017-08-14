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

#include "Utils.h"
#include "Serial.h"
#include "State.h"
#include "Cmd.h"


//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {
  boot();
  init_menu();

/*  
  insert_command("state");

  insert_command("diagnose");
  */

}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  streaming_handler();
  error_led();
  cmdPoll();
}
