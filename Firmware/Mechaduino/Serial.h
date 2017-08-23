#ifndef __Serial_H__
#define __Serial_H__

#pragma once

#include <arduino.h>



void init_menu();

void set_TC(int arg_cnt, char **args);

void state(int arg_cnt, char **args);

void SoftReset(int arg_cnt, char **args);

void Serial_menu(int arg_cnt, char **args);

void setpoint(int arg_cnt, char **args);

void parameterEdit(int arg_cnt, char **args);

void step_response(int arg_cnt, char **args);

void dirac(int arg_cnt, char **args);

void readEncoderDiagnostics(int arg_cnt, char **args); 

void error_reg(int arg_cnt, char **args);

void set_streaming(int arg_cnt, char **args);

void streaming_handler();

void start_testmove(int arg_cnt, char **args);

void Serial_oneStep(int arg_cnt, char **args);

void get_max_frequency(int arg_cnt, char **args);

#endif
