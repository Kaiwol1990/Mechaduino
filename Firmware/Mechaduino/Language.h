#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#include "Configuration.h"


// command structure, can be modified in Serial.cpp
/*
  cmdAdd("help", Serial_menu)
  cmdAdd("calibrate", calibration)
  cmdAdd("diagnose", readEncoderDiagnostics)
  cmdAdd("state", state)
  cmdAdd("stream",  set_streaming)
  cmdAdd("parameter", parameterEdit)
  cmdAdd("setpoint", setpoint )
  cmdAdd("reset", SoftReset)
  cmdAdd("dirac", dirac)
  cmdAdd("response", step_response)
  cmdAdd("error", error_reg)
  cmdAdd("autotune", PID_autotune)
  cmdAdd("downhill", downhill_simplex)
  cmdAdd("testmove", start_testmove)
  cmdAdd("interrupt", set_TC)
*/

#define help_command "help"
#define help_menu "prints this menu"
#define help_header "//---- Mainmenu -----"


#define cancle_header "press c to cancle"


#define calibrate_command "calibrate"
#define calibrate_menu "starts the calibration routine"
#define calibrate_header "//---- Starting calibration routine ----"


#define diagnostics_command  "diagnose"
#define diagnostics_menu "read Encoder Diagnostics"
#define diagnostics_header "//---- Checking AS5047 diagnostic and error registers ---- \n\r See AS5047 datasheet for details"


#define state_command  "state"
#define state_menu "shows and sets the motor state [-on, -off]"
#define state_header "//---- State ----"


#define stream_command "stream"
#define stream_menu "streams servo parameters [-f {int}, -on, -off]"
#define stream_header "//---- Streaming Handler ----"


#define parameter_command  "parameter"
#define parameter_menu "Get or set parameter [-set {-parameter} {value}, -get]"
#define parameter_header "//---- Parameter menu ----"


#define setpoint_command "setpoint"
#define setpoint_menu "Get or set setpoint [-set {float}]"
#define setpoint_header "//----  Setpoint ----"


#define reset_command  "reset"
#define reset_menu "resets the board"
#define reset_header "//---- reseting ---- \n\r Please re-open the Serialmonitor"


#define dirac_command  "dirac"
#define dirac_menu "generates a dirac delta distrebution [-f {int}]"
#define dirac_header "//---- Dirac delta function ----"


#define step_response_command  "response"
#define step_response_menu "generates a step response [-f {int}, -s {int}]"
#define step_response_header "//---- Step response ---- \n"


#define error_command "error"
#define error_menu "get or reset the error register [-reset]"
#define error_header "//---- Checking Mechaduino diagnostic and error register ----"


#define autotune_command  "autotune"
#define autotune_menu "PID auto tune [-c {byte}, -d]"
#define autotune_header "//---- Autotuning the current PID controller ----"


#define downhill_command "downhill"
#define downhill_menu "Downhhill simplex algorithm [-f {int}, -v {int}, -o]"
#define downhill_header "//---- Autotuning the current PID controller ----"


#define test_command "testmove"
#define test_menu "Starts a testmove for the servo [-f {int}, -v {int}, -o]"
#define test_header "//---- Test move ----"


#define interrupt_command "interrupt"
#define interrupt_menu "Starts or stops the ISR [-TC5 {bool}, -TC4 {bool}]"
#define interrupt_header "//---- ISR ----"






#define procent_bar "|---+----+----+----+----+----+----+----+----+----|"


#define bootscreen_1   "###################################################################################"
#define bootscreen_2   "#                                                                                 #"
#define bootscreen_3   "#  ##   ##  ######  ####   ##  ##   ####   #####   ##  ##  ######  ##  ##   ####  #"
#define bootscreen_4   "#  ### ###  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ### ##  ##  ## #"
#define bootscreen_5   "#  ## # ##  ####   ##      ######  ######  ##  ##  ##  ##    ##    ## ###  ##  ## #"
#define bootscreen_6   "#  ##   ##  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ##  ##  ##  ## #"
#define bootscreen_7   "#  ##   ##  ######  ####   ##  ##  ##  ##  #####    ####   ######  ##  ##   ####  #"
#define bootscreen_8   "#                                                                                 #"
#define bootscreen_9   "###################################################################################"
#define bootscreen_10  "  open-source industrial servo motor by Tropical Labs       written by Kai Wolter   "


#endif
