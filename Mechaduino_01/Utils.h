//Contains the declarations for the functions used by the firmware

#ifndef __UTILS_H__
#define __UTIL_H__

void setupPins();

void setupSPI();

void setupPID();

void stepInterrupt();

void enaInterrupt();

void dirInterrupt();

void output(int theta, int effort);

void calibration();

void serialCheck();

void Serial_menu();

void setpoint();

void parameterQuery();

void jump_to_fullstepp();

void quaterStep();

void oneStep();

int readEncoder();

void receiveEvent(int howMany);

int mod(int xMod, int mMod);

int lookup_sine(int m);

void setupTCInterrupts();

void enableTC4Interrupts();

void enableTC5Interrupts();

void disableTC4Interrupts();

void disableTC5Interrupts();

void parameterEdit();

void step_response();

void get_max_frequency();

float modulo(float dividend, float divisor);

#endif
