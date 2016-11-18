//Contains the declarations for the functions used by the firmware

#ifndef __UTILS_H__
#define __UTIL_H__

void setupPins();

void setupSPI();

void setupPID();

void stepInterrupt();

void enaInterrupt();

void dirInterrupt();

void output(int theta, int effort) __attribute__ ((optimize("O3")));

void calibration();

void serialCheck();

void Serial_menu();

void setpoint();

void parameterQuery();

void quaterStep();

void oneStep();

int readEncoder();

void receiveEvent(int howMany);

int mod(int xMod, int mMod);

void setupTCInterrupts();

void enableTC5Interrupts();

void disableTC5Interrupts();

void parameterEdit();

void step_response();

void get_max_frequency();

#endif

