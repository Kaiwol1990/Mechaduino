//Contains the declarations for the functions used by the firmware

#ifndef __UTILS_H__
#define __UTIL_H__

void setupSPI();

void stepInterrupt();

void enaInterrupt();

void dirInterrupt();

void calibration();

void oneStep();

int mod(int xMod, int mMod) __attribute__ ((optimize("O3")));

void setupTCInterrupts();

void enableTC5Interrupts();

void disableTC5Interrupts();

void PID_autotune();

extern int digitalSmooth(int rawIn, int *sensSmoothArray);

#endif

