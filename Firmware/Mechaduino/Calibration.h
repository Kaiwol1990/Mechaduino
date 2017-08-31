#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__



void calibration(int arg_cnt, char **args);

void oneStep();

void output_calibration(int electric_angle, int effort) __attribute__ ((optimize("O3")));


#endif




