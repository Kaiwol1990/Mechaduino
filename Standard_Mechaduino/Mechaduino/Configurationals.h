#ifndef __CONFIGURATIONALS_H__
#define __CONFIGURATIONALS_H__


//----current settings-----
extern const int uMAX;    // max voltage for the vref pins
extern int ITerm_max;
extern const int phase_multiplier;


//---- Step settings -----
extern const int counts_per_revolution; // encoder counts for 360 degrees
extern const int stepangle;             // angle of one step as int
extern const int PA;                    // angle of one fullstepstep


#endif
