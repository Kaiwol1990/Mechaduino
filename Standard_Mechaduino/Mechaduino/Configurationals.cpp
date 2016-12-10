#include "Configurationals.h"
#include "Configuration.h"


//----current settings-----
const float rSense = 0.150;                           // resistor value
const int uMAX = ((1024.0 * iMAX * 10 * rSense) / (1000 * 3.3)); // max voltage for the vref pins
int ITerm_max = ((uMAX * 1000) / (3 * init_Ki));
const int phase_multiplier = ((10 * steps_per_revolution / 4) / 100);

//---- Step settings -----
const int counts_per_revolution = (16384);                              // encoder counts for 360 degrees
const int angle_per_step = (360.0 / steps_per_revolution);            // only needed for calibration routine by now
const int stepangle = ((100 * 36000.0 / (steps_per_revolution * microstepping))); // angle of one step as int
const int PA = ((36000 / steps_per_revolution));                        // angle of one fullstep



