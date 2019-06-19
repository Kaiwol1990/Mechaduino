//Contains the declarations for the functions used by the firmware

#ifndef __UTILS_H__
#define __UTIL_H__
#pragma once
#include <Arduino.h>

void boot();

void setupPins();

void setupSPI();

void stepInterrupt();

void enaInterrupt();

void dirInterrupt();

int mod(int xMod, int mMod) __attribute__ ((optimize("O3")));

void setupTCInterrupts();

void enableTC5Interrupts();

void disableTC4Interrupts();

void disableTC5Interrupts();

void PID_autotune(int arg_cnt, char **args);

float digitalSmooth(int rawIn, int *sensSmoothArray);

int measure_noise();

bool canceled();

int sign(int input);

void error_led();

float test_move(int steps, int F_Sample, bool output, char identifier);

void downhill_simplex(int arg_cnt, char **args);

void paramter_streamer(bool output, bool gui);

void getStictionTable(int arg_cnt, char **args);

void getCoggingTable(int arg_cnt, char **args);

int rateLimiter(int currentValue, int lastValue, int RSR, int FSR);

#endif
