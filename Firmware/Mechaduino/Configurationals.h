#ifndef __CONFIGURATIONALS_H__
#define __CONFIGURATIONALS_H__



#define firmware_version "0.4"
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

extern int target_dt;

extern int step_add;

extern int ITerm_max;

extern bool LED_pattern[4][10];


// calculate maximal peak count from maximal peak time
extern int countPEAKMax;
extern int countPEAKDEADMax;
extern int uPEAK;

#endif
