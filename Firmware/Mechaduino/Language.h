#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#include "Configuration.h"


// command structure, can be modified in Serial.cpp
/*
  cmdAdd("help", Serial_menu);
  cmdAdd("calibrate", calibration);
  cmdAdd("diagnose", readEncoderDiagnostics);
  cmdAdd("state", state);
  cmdAdd("stream",  set_streaming);
  cmdAdd("parameter", parameterEdit);
  cmdAdd("setpoint", setpoint );
  cmdAdd("reset", SoftReset);
  cmdAdd("dirac", dirac);
  cmdAdd("response", step_response);
  cmdAdd("error", error_reg);
  cmdAdd("autotune", PID_autotune);
  cmdAdd("downhill", downhill_simplex);
  cmdAdd("testmove", start_testmove);
  cmdAdd("interrupt", set_TC);
*/


const String help_command ="help";
const String help_menu ="prints this menu";
const String help_header ="//---- Mainmenu -----";


const String cancle_header ="press c to cancle";


const String calibrate_command ="calibrate";
const String calibrate_menu ="starts the calibration routine";
const String calibrate_header ="//---- Starting calibration routine ----";


const String diagnostics_command  ="diagnose";
const String diagnostics_menu ="read Encoder Diagnostics";
const String diagnostics_header ="//---- Checking AS5047 diagnostic and error registers ---- \n\r See AS5047 datasheet for details";


const String state_command  ="state";
const String state_menu ="shows and sets the motor state [-on, -off]";
const String state_header ="//---- State ----";


const String stream_command ="stream";
const String stream_menu ="streams servo parameters [-f {int}, -on, -off]";
const String stream_header ="//---- Streaming Handler ----";


const String parameter_command  ="parameter";
const String parameter_menu ="Get or set parameter [-set {-parameter} {value}, -get]";
const String parameter_header ="//---- Parameter menu ----";


const String setpoint_command ="setpoint";
const String setpoint_menu ="Get or set setpoint [-set {float}]";
const String setpoint_header ="//----  Setpoint ----";


const String reset_command  ="reset";
const String reset_menu ="resets the board";
const String reset_header ="//---- reseting ---- \n\r Please re-open the Serialmonitor";


const String dirac_command  ="dirac";
const String dirac_menu ="generates a dirac delta distrebution [-f {int}]";
const String dirac_header ="//---- Dirac delta function ----";


const String step_response_command  ="response";
const String step_response_menu ="generates a step response [-f {int}, -s {int}]";
const String step_response_header ="//---- Step response ---- \n";


const String error_command ="error";
const String error_menu ="get or reset the error register [-reset]";
const String error_header ="//---- Checking Mechaduino diagnostic and error register ----";


const String autotune_command  ="autotune";
const String autotune_menu ="PID auto tune [-c {byte}, -d]";
const String autotune_header ="//---- Autotuning the current PID controller ----";


const String downhill_command ="downhill";
const String downhill_menu ="Downhhill simplex algorithm [-f {int}, -v {int}, -o]";
const String downhill_header ="//---- Autotuning the current PID controller ----";


const String test_command ="testmove";
const String test_menu ="Starts a testmove for the servo [-f {int}, -v {int}, -o]";
const String test_header ="//---- Test move ----";


const String interrupt_command ="interrupt";
const String interrupt_menu= "Starts or stops the ISR [-TC5 {bool}, -TC4 {bool}]";
const String interrupt_header ="//---- ISR ----";






const String procent_bar = "|---+----+----+----+----+----+----+----+----+----|";


const String bootscreen_1   = "###################################################################################";
const String bootscreen_2   = "#                                                                                 #";
const String bootscreen_3   = "#  ##   ##  ######  ####   ##  ##   ####   #####   ##  ##  ######  ##  ##   ####  #";
const String bootscreen_4   = "#  ### ###  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ### ##  ##  ## #";
const String bootscreen_5   = "#  ## # ##  ####   ##      ######  ######  ##  ##  ##  ##    ##    ## ###  ##  ## #";
const String bootscreen_6   = "#  ##   ##  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ##  ##  ##  ## #";
const String bootscreen_7   = "#  ##   ##  ######  ####   ##  ##  ##  ##  #####    ####   ######  ##  ##   ####  #";
const String bootscreen_8   = "#                                                                                 #";
const String bootscreen_9   = "###################################################################################";
const String bootscreen_10  = "  open-source industrial servo motor by Tropical Labs       written by Kai Wolter   ";


#endif
