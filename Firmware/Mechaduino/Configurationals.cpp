#include "Configurationals.h"
#include "Configuration.h"



//----current settings-----
const float rSense = 0.150;                           		// resistor value
//int uMAX = ((512.0 * iMAX * 10.0 * rSense) / (1000.0 * 3.3));  // max voltage for the vref pins
int uMAX = ((1024.0 * iMAX * 10.0 * rSense) / (1000.0 * 3.3));  // max voltage for the vref pins
int phase_multiplier = ((steps_per_revolution / 4) / 10);


//---- Step settings -----
//const int counts_per_revolution = 16384; 						// encoder counts for 360 degrees
float stepangle = (36000.0 / (steps_per_revolution * 1024)); 	// angle of one microstep as float
int PA =   36000 / steps_per_revolution;                      // angle of one fullstep as int

int max_e = (36000 * error_led_value) / mm_rev;             // max error to turn the led off in mm

int target_dt = 1.1 * (1000000 / FPID);

int step_add = 1024 / microstepping;

int ITerm_max = uMAX * 1024.0;


// calculate maximal peak count from maximal peak time
int countPEAKMax = (tPEAK*FPID) / 1000;
int countPEAKDEADMax = (tDEAD*FPID) / 1000;

int uPEAK = ((1024.0 * iPEAK * 10.0 * rSense) / (1000.0 * 3.3));  // max voltage for the vref pins


//                            timing error                    maximal error                   maximal current                       lookuptable
bool LED_pattern[4][10] = { {1, 0, 1, 0, 1, 0, 1, 0, 1, 0}, {1, 1, 1, 1, 1, 0, 0, 0, 0, 0}, {1, 1, 1, 1, 1, 1, 1, 1, 0, 1}, {1, 1, 1, 1, 1, 1, 0, 1, 0, 1}};





