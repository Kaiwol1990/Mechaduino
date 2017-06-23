#ifndef __Serial_H__
#define __Serial_H__
#include <arduino.h>

void serialCheck();

void SoftReset();

void getstate();

void enable();

void disable();

void Serial_menu();

void setpoint(String arg = "");

void readangle();

void parameterQuery();

void parameterEdit(String arg = "");

void step_response();

void dirac();

void get_max_frequency();

void readEncoderDiagnostics(); //check encoder diagnostics registers

void print_error_register();

void reset_error_register();

int measure_noise(bool serialoutput = true);

int measure_setpoint();

bool read_serialcommand(int timeout, String *command, String *argument);

bool split_command(String *Input_pointer, String *first_substring, String *second_substring);

void Streaming();

void send_param();

#endif
