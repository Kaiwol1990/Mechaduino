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

void step_response(String arg = "");

void get_max_frequency();

void readEncoderDiagnostics(); //check encoder diagnostics registers

void measure_noise();

String read_serialcommand(int timeout);

#endif
