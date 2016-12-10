#ifndef __Serial_H__
#define __Serial_H__

void serialCheck();

void SoftReset();

void enable();

void disable();

void Serial_menu();

void setpoint();

void readangle();

void parameterQuery();

void parameterEdit();

void step_response();

void get_max_frequency();

void readEncoderDiagnostics(); //check encoder diagnostics registers

void measure_noise();

#endif
