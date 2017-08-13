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

void calibration(int arg_cnt, char **args);

void oneStep();

int mod(int xMod, int mMod) __attribute__ ((optimize("O3")));

void setupTCInterrupts();

void enableTC5Interrupts();

void disableTC4Interrupts();

void disableTC5Interrupts();

void PID_autotune(int arg_cnt, char **args);

float digitalSmooth(int rawIn, int *sensSmoothArray);

int measure_noise();

bool canceled();

bool timed_out(unsigned long now, int time_out);

int sign(int input);

void boot();

void error_led();

float test_move(int steps, int F_Sample, bool output, char identifier);

void downhill_simplex(int arg_cnt, char **args);

void paramter_streamer(bool output, bool gui) ;

#endif

