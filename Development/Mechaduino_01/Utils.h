//Contains the declarations for the functions used by the firmware

#ifndef __UTILS_H__
#define __UTIL_H__

void setupPins();

void setupSPI();

void stepInterrupt();

void enaInterrupt();

void dirInterrupt();

void output(float theta, int effort);

void calibration();

void serialCheck();

void Serial_menu();

void setpoint();

void parameterQuery();

float lookup_angle(int n);

void jump_to_fullstepp();

void quaterStep();

void oneStep();

int readEncoder();

void receiveEvent(int howMany);

int mod(int xMod, int mMod);

float lookup_sine(int m);

void setupTCInterrupts();

void enableTC4Interrupts();

void enableTC5Interrupts();

void disableTC4Interrupts();

void disableTC5Interrupts();

void parameterEdit();

void step_response();

void get_max_frequency();

void calcBiquad(int cut, int sample);

void filterEdit();

float modulo(float dividend, float divisor);


void calcIIR (int cut, int sample);

int runningAverage(int M);
#endif
