#ifndef __CALIBRATION_H__
#define __CALIBRATION_H__



void calibration(int arg_cnt, char **args);

void oneStep();

void coil_calibration(bool debug);

float read_encoder_calibration(int avg, bool mode);


#endif




