//Contains the declarations for the functions used by the firmware

#ifndef __UTILS_H__
#define __UTIL_H__
#pragma once

#include <arduino.h>

void setupPins();

void setupSPI();

void stepInterrupt();

void enaInterrupt();

void dirInterrupt();

void calibration(String arg = "");

void oneStep();

int mod(int xMod, int mMod) __attribute__ ((optimize("O3")));

void setupTCInterrupts();

void enableTC5Interrupts();

void disableTC4Interrupts();

void disableTC5Interrupts();

void antiCoggingCal();

void PID_autotune(String arg = "");

float digitalSmooth(int rawIn, int *sensSmoothArray);

bool canceled();

bool timed_out(unsigned long now, int time_out);

int sign(int input);

void boot();

bool check_lookup(bool output);

void error_led();

void twiddle();

float test_move(int step_add, int F_Sample);

void downhill_simplex();

void test();

#endif

