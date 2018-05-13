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

// ###################################################################################
// #                                                                                 #
// #  ##   ##  ######  ####   ##  ##   ####   #####   ##  ##  ######  ##  ##   ####  #
// #  ### ###  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ### ##  ##  ## #
// #  ## # ##  ####   ##      ######  ######  ##  ##  ##  ##    ##    ## ###  ##  ## #
// #  ##   ##  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ##  ##  ##  ## #
// #  ##   ##  ######  ####   ##  ##  ##  ##  #####    ####   ######  ##  ##   ####  #
// #                                                                                 #
// ###################################################################################
//   open-source industrial servo motor by Tropical Labs       written by Kai Wolter   


//#define no_millis
#include <Arduino.h>

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

uint32_t counter = 0;
uint32_t nextMilis = 0;

void loop() {
  while (1) {
    streaming_handler();
    cmdPoll();

//    counter ++;
//
//    if (millis() > nextMilis) {
//      nextMilis += 50;
//      SerialUSB.println((float)100.0 * (counter / 50116.0));
//      counter = 0;
//    }
  }
}
