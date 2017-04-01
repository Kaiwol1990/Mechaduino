#include "Configurationals.h"
#include "Configuration.h"



//----current settings-----
const float rSense = 0.150;                           // resistor value
int uMAX = ((512.0 * iMAX * 10 * rSense) / (1000 * 3.3)); // max voltage for the vref pins
int phase_multiplier = ((10 * steps_per_revolution / 4) / 100);


//---- Step settings -----
//const int counts_per_revolution = 16384;                              // encoder counts for 360 degrees
int stepangle = ((100 * 36000.0 / (steps_per_revolution * 512))); // angle of one step as int
int PA = ((36000 / steps_per_revolution));                        // angle of one fullstep

int max_e = (36000 * error_led_value) / mm_rev;             // max error to turn the led off in mm


int step_add = 512 / microstepping;



int ITerm_max = uMAX * 1000;

