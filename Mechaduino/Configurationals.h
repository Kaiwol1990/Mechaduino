#ifndef __CONFIGURATIONALS_H__
#define __CONFIGURATIONALS_H__


#define firmware_version "0.3"
#define counts_per_revolution 16384

//----current settings-----
extern const int uMAX;    // max voltage for the vref pins
extern const int phase_multiplier;


//---- Step settings -----
//extern const int counts_per_revolution; // encoder counts for 360 degrees
extern const int stepangle;             // angle of one step as int
extern const int PA;                    // angle of one fullstepstep


extern const int max_e;                 // max error to turn the led off in mm


#endif
