#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#include "Configuration.h"


const String PROGMEM cancle_header = "press c to cancle";

const String PROGMEM calibrate_command = "calibrate";
const String PROGMEM calibrate_menu = "starts the calibration routine";
const String PROGMEM calibrate_header = "//---- Starting calibration routine ----";


const String PROGMEM diagnostics_command  = "diagnose";
const String PROGMEM diagnostics_menu = "read Encoder Diagnostics";
const String PROGMEM diagnostics_header = "//---- Checking AS5047 diagnostic and error registers ---- \n\r See AS5047 datasheet for details";


const String PROGMEM disable_command  = "disable";
const String PROGMEM disable_menu = "disables the motor";
const String PROGMEM disable_header = "//---- Motor disabled ----";


const String PROGMEM getstate_command  = "state";
const String PROGMEM getstate_menu = "shows the current motor state";
const String PROGMEM getstate_header = "//---- State ----";


const String PROGMEM editparam_command  = "editparam";
const String PROGMEM editparam_menu = "edit parameter";
const String PROGMEM editparam_header = "//---- Edit parameter ----";


const String PROGMEM enable_command  = "enable";
const String PROGMEM enable_menu = "enables the motor";
const String PROGMEM enable_header = "//---- Motor enabled ----";
const String PROGMEM dir_header = "//---- Direction = ";


const String PROGMEM read_command  = "read";
const String PROGMEM read_menu = "reads the curren angle";
const String PROGMEM read_header = "//---- current Setpoint ---- \n\r current setpoint = ";


const String PROGMEM reset_command  = "reset";
const String PROGMEM reset_menu = "resets the board";
const String PROGMEM reset_header = "//---- reseting ---- \n\r Please re-open the Serialmonitor";


const String PROGMEM step_response_command  = "response";
const String PROGMEM step_response_menu = "generates a step response";
const String PROGMEM step_response_header = "//---- Step response ---- \n Number of steps = ";

const String PROGMEM dirac_command  = "dirac";
const String PROGMEM dirac_header = "//---- Dirac delta function ---- \n";


const String PROGMEM help_command  = "help";
const String PROGMEM help_menu = "prints this menu";
const String PROGMEM help_header = "Mainmenu";


const String PROGMEM parameter_command  = "param";
const String PROGMEM parameter_menu = "print the current PID parameter";
const String PROGMEM parameter_header = "//---- PID Values -----";


const String PROGMEM set_command  = "set";
const String PROGMEM set_menu = "enter new setpoint";
const String PROGMEM set_header = "//---- Setpoint ---- \n\r new setpoint = ";


const String PROGMEM autotune_command  = "autotune";
const String PROGMEM autotune_menu = "PID autotune";
const String PROGMEM autotune_header = "//---- Autotuning the current PID controller --- \n\r Enter number of tuning cycles";

const String PROGMEM looptime_command  = "loop";
const String PROGMEM looptime_menu = "calculates the maximal loopfrequency";
const String PROGMEM looptime_header = "//---- Calibrating loop time ---- \n\r make sure the motor is moving!";


const String PROGMEM noise_command  = "noise";
const String PROGMEM noise_menu = "measures the noise of the encoder";
const String PROGMEM noise_header = "//---- Measuring noise ----";

const String PROGMEM check_lookup_command = "check";
const String PROGMEM check_lookup_menu = "check the lookup table";
const String PROGMEM check_lookup_header = "//---- checking lookup table ----";

const String PROGMEM anticogging_command = "cogging";


const String PROGMEM stream_start_command = "start_stream";
const String PROGMEM stream_stop_command = "stop_stream";

const String PROGMEM load_param_command = "load_param";



const String PROGMEM procent_bar = "|---+----+----+----+----+----+----+----+----+----|";


const String PROGMEM bootscreen_1   = "###################################################################################";
const String PROGMEM bootscreen_2   = "#                                                                                 #";
const String PROGMEM bootscreen_3   = "#  ##   ##  ######  ####   ##  ##   ####   #####   ##  ##  ######  ##  ##   ####  #";
const String PROGMEM bootscreen_4   = "#  ### ###  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ### ##  ##  ## #";
const String PROGMEM bootscreen_5   = "#  ## # ##  ####   ##      ######  ######  ##  ##  ##  ##    ##    ## ###  ##  ## #";
const String PROGMEM bootscreen_6   = "#  ##   ##  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ##  ##  ##  ## #";
const String PROGMEM bootscreen_7   = "#  ##   ##  ######  ####   ##  ##  ##  ##  #####    ####   ######  ##  ##   ####  #";
const String PROGMEM bootscreen_8   = "#                                                                                 #";
const String PROGMEM bootscreen_9   = "###################################################################################";
const String PROGMEM bootscreen_10  = "  open-source industrial servo motor by Tropical Labs       written by Kai Wolter   ";


#endif
