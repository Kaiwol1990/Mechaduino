#ifndef __ENCODER_H__
#define __ENCODER_H__
#include <stdint.h>


int readEncoder() __attribute__ ((optimize("O3")));

int readAngle(int last_angle, int last_raw) __attribute__ ((optimize("O3")));
#endif
