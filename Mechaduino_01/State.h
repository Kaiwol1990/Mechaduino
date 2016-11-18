//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__

//---- interrupt vars ----
extern volatile int r;            //target angle
extern volatile long y;           //current angle

extern volatile int raw_0;        // current measured angle
extern volatile int raw_1;        // last measured angle
extern volatile int raw_diff;     // diff of both

extern volatile int u;            // control effort

extern volatile long ITerm;       //integral term

extern volatile int e_0;          // current error term
extern volatile int e_1;          // last error term

extern volatile bool dir;         // flag for  dir setting
extern volatile bool enabled;     // flag for  enabled setting

extern bool calibration_running;  // flag for calibration
extern bool frequency_test;       // flag for frequency test


//----current settings-----
extern const int uMAX;    // max voltage for the vref pins


//---- Step settings -----
extern const int counts_per_revolution; // encoder counts for 360 degrees
extern const float angle_per_step;      // only needed for calibration routine by now
extern const int stepangle;             // angle of one step as int
extern volatile const int PA;           // angle of one fullstep
extern volatile int step_target;        // target as step gets incremented if an step is received


//--- lookup tables for the coils ---
// take a look at the current plot for microstepping online
extern const PROGMEM int_fast16_t sine_lookup_coil_A[];   // lookup for coil A
extern const PROGMEM int_fast16_t sine_lookup_coil_B[];   // lookup for coil B


//---- Pins -----
extern const int IN_4;
extern const int IN_3;
extern const int VREF_2;
extern const int VREF_1;
extern const int IN_2;
extern const int IN_1;
extern const int ledPin;
extern const int chipSelectPin;
extern const int dir_pin;
extern const int step_pin;
extern const int ena_pin;


#endif
