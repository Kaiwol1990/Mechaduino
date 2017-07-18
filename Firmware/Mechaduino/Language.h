#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#include "Configuration.h"

const String cancle_header = "press c to cancle";

const String calibrate_command = "calibrate";
const String calibrate_menu = "starts the calibration routine";
const String calibrate_header = "//---- Starting calibration routine ----";


const String diagnostics_command  = "diagnose";
const String diagnostics_menu = "read Encoder Diagnostics";
const String diagnostics_header = "//---- Checking AS5047 diagnostic and error registers ---- \n\r See AS5047 datasheet for details";


const String disable_command  = "disable";
const String disable_menu = "disables the motor";
const String disable_header = "//---- Motor disabled ----";


const String getstate_command  = "state";
const String getstate_menu = "shows the current motor state";
const String getstate_header = "//---- State ----";


const String editparam_command  = "editparam";
const String editparam_menu = "submenu to edit the parameter";
const String editparam_header = "//---- Edit parameter ----";


const String enable_command  = "enable";
const String enable_menu = "enables the motor";
const String enable_header = "//---- Motor enabled ----";
const String dir_header = "//---- Direction = ";


const String read_command  = "read";
const String read_menu = "reads the current angle";
const String read_header = "//---- current Setpoint ---- \n\r current setpoint = ";


const String reset_command  = "reset";
const String reset_menu = "resets the board";
const String reset_header = "//---- reseting ---- \n\r Please re-open the Serialmonitor";


const String step_response_command  = "response";
const String step_response_menu = "generates a step response";
const String step_response_header = "//---- Step response ---- \n Number of steps = ";

const String dirac_command  = "dirac";
const String dirac_header = "//---- Dirac delta function ---- \n";


const String help_command  = "help";
const String help_menu = "prints this menu";
const String help_header = "//---- Mainmenu -----";


const String parameter_command  = "param";
const String parameter_menu = "print the current PID parameter";
const String parameter_header = "//---- PID Values -----";


const String set_command  = "set";
const String set_menu = "enter new set point";
const String set_header = "//---- Setpoint ---- \n\r new setpoint = ";


const String autotune_command  = "autotune";
const String autotune_menu = "PID auto tune";
const String autotune_header = "//---- Autotuning the current PID controller --- \n\r Enter number of tuning cycles";



const String downhill_command = "downhill";
const String downhill_menu = "Downhhill simplex algorithm";
const String downhill_header = "//---- Autotuning the current PID controller with the downhill simplex algorithm--- ";


const String twiddle_command = "twiddle";
const String twiddle_menu = "Downhhill simplex algorithm";
const String twiddle_header = "//---- Autotuning the current PID controller with the twiddle algorithm--- ";


const String looptime_command  = "loop";
const String looptime_menu = "calculates the maximal loopfrequency";
const String looptime_header = "//---- Measuring loop time ---- \n\r make sure the motor is moving!";


const String noise_command  = "noise";
const String noise_menu = "measures the noise of the encoder";
const String noise_header = "//---- Measuring noise ----";


const String check_lookup_command = "check";
const String check_lookup_menu = "check the lookup table";
const String check_lookup_header = "//---- checking lookup table ----";


const String print_error_command = "error";
const String print_error_menu = "outputs the mechaduino error register";
const String print_error_header = "//---- Checking Mechaduino diagnostic and error register ----";

const String reset_error_command = "reset_error";
const String reset_error_menu = "resets the mechaduino error register";
const String reset_error_header = "//---- resetting error register ----";



const String stream_start_command = "start_stream";
const String stream_stop_command = "stop_stream";

const String anticogging_command = "cogging";

const String load_param_command = "load_param";

const String disableTC5 = "TC_off";
const String enableTC5 = "TC_on";

const String test_command = "test";




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
