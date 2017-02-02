#ifndef __LANGUAGE_H__
#define __LANGUAGE_H__
#include "Configuration.h"


#if (language ==1)
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
const String PROGMEM editparam_header = "//---- Edit PID gains ----";


const String PROGMEM enable_command  = "enable";
const String PROGMEM enable_menu = "enables the motor";
const String PROGMEM enable_header = "//---- Motor enabled ----";


const String PROGMEM read_command  = "read";
const String PROGMEM read_menu = "reads the curren angle";
const String PROGMEM read_header = "//---- current Setpoint ---- \n\r current setpoint = ";


const String PROGMEM reset_command  = "reset";
const String PROGMEM reset_menu = "resets the board";
const String PROGMEM reset_header = "//---- reseting ---- \n\r Please re-open the Serialmonitor";


const String PROGMEM step_response_command  = "response";
const String PROGMEM step_response_menu = "generates a step response";
const String PROGMEM step_response_header = "//---- Step response ---- \n\r Number of steps = ";


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
const String PROGMEM autotune_header = "//---- Autotuning the PID controller --- \n\r Enter number of tuning cycles";


const String PROGMEM looptime_command  = "loop";
const String PROGMEM looptime_menu = "calculates the maximal loopfrequency";
const String PROGMEM looptime_header = "//---- Calibrating loop time ---- \n\r make sure the motor is moving!";


const String PROGMEM noise_command  = "noise";
const String PROGMEM noise_menu = "measures the noise of the encoder";
const String PROGMEM noise_header = "//---- Measuring noise ----";

const String PROGMEM anticogging_command = "cogging";


#elif (language==2)
const String PROGMEM cancle_header = "c druecken zum abbrechen";

const String PROGMEM calibrate_command = "calibrate";
const String PROGMEM calibrate_menu = "Startet die Motor Kalibrierung";
const String PROGMEM calibrate_header = "//---- Kalibriere ---- \n\r c druecken zum abbrechen";


const String PROGMEM diagnostics_command  = "diagnose";
const String PROGMEM diagnostics_menu = "Liest das Fehleregister des Encoders";
const String PROGMEM diagnostics_header = "//---- Ueberpruefe das Fehlerregister ---- \n\r AS5047 Datenblatt fuer genaue Angaben";


const String PROGMEM disable_command  = "disable";
const String PROGMEM disable_menu = "Schaltet den Motorstrom aus";
const String PROGMEM disable_header = "//---- Motor aus ----";


const String PROGMEM getstate_command  = "state";
const String PROGMEM getstate_menu = "Zeigt an ob der Motorstom eingeschaltet ist";
const String PROGMEM getstate_header = "//---- Status ----";


const String PROGMEM editparam_command  = "editparam";
const String PROGMEM editparam_menu = "Ermoeglicht das bearbeiten der PID Parameter";
const String PROGMEM editparam_header = "//---- PID Parameter bearbeiten ----";


const String PROGMEM enable_command  = "enable";
const String PROGMEM enable_menu = "Schaltet den Motorstrom ein";
const String PROGMEM enable_header = "//---- Motor an ----";


const String PROGMEM read_command  = "read";
const String PROGMEM read_menu = "Gibt den aktuellen Winkel aus";
const String PROGMEM read_header = "//---- aktueller Winkel ---- \n\r aktueller Winkel = ";


const String PROGMEM reset_command  = "reset";
const String PROGMEM reset_menu = "Setzt das Board zurueck";
const String PROGMEM reset_header = "//---- Reset ---- \n\r Der Seriallemonitor muss neu geoeffnet werden";


const String PROGMEM step_response_command  = "response";
const String PROGMEM step_response_menu = "Erzeugt eine Sprungantwort";
const String PROGMEM step_response_header = "//---- Sprungantwort ---- \n\r Anzahl der Schritte = ";


const String PROGMEM help_command  = "help";
const String PROGMEM help_menu = "Zeigt dieses Menu an";
const String PROGMEM help_header = "Hauptmenu";


const String PROGMEM parameter_command  = "param";
const String PROGMEM parameter_menu = "Gibt die aktuellen PID Parameter aus";
const String PROGMEM parameter_header = "//---- PID Parameter -----";


const String PROGMEM set_command  = "set";
const String PROGMEM set_menu = "Neuen Sollwert vorgeben";
const String PROGMEM set_header = "//---- Sollwert ---- \n\r Neuen Sollwert = ";


const String PROGMEM autotune_command  = "autotune";
const String PROGMEM autotune_menu = "PID autotune";
const String PROGMEM autotune_header = "//---- PID Autotuning --- \n\r Anzahl der Zyklen eingeben";


const String PROGMEM looptime_command  = "loop";
const String PROGMEM looptime_menu = "Berechnet die maximale Frequenz";
const String PROGMEM looptime_header = "//---- Frequenz ---- \n\r Der Motor sollte sich bewegen";


const String PROGMEM noise_command  = "noise";
const String PROGMEM noise_menu = "misst das Rauschen des Sensors";
const String PROGMEM noise_header = "//---- Rauschen ----";

const String PROGMEM anticogging_command = "cogging";


#endif

const String PROGMEM procent_bar = "|---+----+----+----+----+----+----+----+----+----|";
/*
const String PROGMEM bootscreen_1   = "###############################################################################################";
const String PROGMEM bootscreen_2   = "#   __  __   ______    _____   _    _              _____    _    _   _____   _   _    ____    #";
const String PROGMEM bootscreen_3   = "#  |  \\/  | |  ____|  / ____| | |  | |     /\\     |  __ \\  | |  | | |_   _| | \\ | |  / __ \\   #";
const String PROGMEM bootscreen_4   = "#  | \\  / | | |__    | |      | |__| |    /  \\    | |  | | | |  | |   | |   |  \\| | | |  | |  #";
const String PROGMEM bootscreen_5   = "#  | |\\/| | |  __|   | |      |  __  |   / /\\ \\   | |  | | | |  | |   | |   | . ` | | |  | |  #";
const String PROGMEM bootscreen_6   = "#  | |  | | | |____  | |____  | |  | |  / ____ \\  | |__| | | |__| |  _| |_  | |\\  | | |__| |  #";
const String PROGMEM bootscreen_7   = "#  |_|  |_| |______|  \\_____| |_|  |_| /_/    \\_\\ |_____/   \\____/  |_____| |_| \\_|  \\____/   #";
const String PROGMEM bootscreen_8   = "#                                                                                             #";
const String PROGMEM bootscreen_9   = "#  open-source industrial servo motor                                  written by Kai Wolter  #";
const String PROGMEM bootscreen_10  = "###############################################################################################";
const String PROGMEM bootscreen_11  = "   compiling Date - ";
const String PROGMEM bootscreen_12  = "                                        Firmware-Version: ";
*/

const String PROGMEM bootscreen_1   = "###################################################################################";
const String PROGMEM bootscreen_2   = "#                                                                                 #";
const String PROGMEM bootscreen_3   = "#  ##   ##  ######  ####   ##  ##   ####   #####   ##  ##  ######  ##  ##   ####  #";
const String PROGMEM bootscreen_4   = "#  ### ###  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ### ##  ##  ## #";
const String PROGMEM bootscreen_5   = "#  ## # ##  ####   ##      ######  ######  ##  ##  ##  ##    ##    ## ###  ##  ## #";
const String PROGMEM bootscreen_6   = "#  ##   ##  ##     ##  ##  ##  ##  ##  ##  ##  ##  ##  ##    ##    ##  ##  ##  ## #";
const String PROGMEM bootscreen_7   = "#  ##   ##  ######  ####   ##  ##  ##  ##  #####    ####   ######  ##  ##   ####  #";
const String PROGMEM bootscreen_8   = "#                                                                                 #";
const String PROGMEM bootscreen_9   = "###################################################################################";
const String PROGMEM bootscreen_10  = "   open-source industrial servo motor                      written by Kai Wolter   ";


#endif
