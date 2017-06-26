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



//////////////////////////////////////
/////////////////SETUP////////////////
//////////////////////////////////////

void setup() {
  boot();
  /*
  SerialUSB.println(int_Kp);
  SerialUSB.println(int_Ki);
  SerialUSB.println(int_Kd);
  SerialUSB.println('-');
  SerialUSB.println(int_pessen_Kp);
  SerialUSB.println(int_pessen_Ki);
  SerialUSB.println(int_pessen_Kd);

  
 int_Kp=int_pessen_Kp;
 int_Ki=int_pessen_Ki;
 int_Kd=int_pessen_Kd;
  SerialUSB.println(int_Kp);
  SerialUSB.println(int_Ki);
  SerialUSB.println(int_Kd);
  */
}



//////////////////////////////////////
/////////////////LOOP/////////////////
//////////////////////////////////////

void loop()
{
  serialCheck();
  error_led();
  Streaming();
}
