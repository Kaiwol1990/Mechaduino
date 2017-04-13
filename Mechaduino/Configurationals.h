#ifndef __CONFIGURATIONALS_H__
#define __CONFIGURATIONALS_H__
#include <stdint.h>


#define firmware_version "0.3"
#define counts_per_revolution 16384

//----current settings-----
extern const float rSense;
extern int uMAX;    // max voltage for the vref pins
extern int phase_multiplier;


//---- Step settings -----
//extern const int counts_per_revolution; // encoder counts for 360 degrees
extern float stepangle;             // angle of one step as int
extern int PA;                    // angle of one fullstepstep


extern int max_e;                 // max error to turn the led off in mm

extern int step_add;

extern int pLPFa;
extern int pLPFb;

extern int encoderLPFa;
extern int encoderLPFb;

extern int32_t ITerm_max ;

#endif
