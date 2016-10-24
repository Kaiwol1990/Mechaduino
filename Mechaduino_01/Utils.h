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

void oneStep(void);

int readEncoder();

void receiveEvent(int howMany);

int mod(int xMod, int mMod);

float lookup_sine(int m);

void setupTCInterrupts();

void enableTCInterrupts();

void disableTCInterrupts();

void antiCoggingCal();

void parameterEdit();

void step_response();

void get_max_frequency();

#endif
