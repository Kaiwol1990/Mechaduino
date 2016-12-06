//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__

//---- interrupt vars ----
extern volatile int r;            //target angle
extern volatile int y;           //current angle

extern volatile int raw_0;        // current measured angle
extern volatile int raw_1;        // last measured angle
extern volatile int raw_diff;     // diff of both

extern volatile int u;            // control effort

extern volatile int e_0;          // current error term
extern volatile int e_1;          // last error term

extern volatile bool dir;         // flag for  dir setting
extern volatile bool enabled;     // flag for  enabled setting

extern bool calibration_running;  // flag for calibration
extern bool frequency_test;       // flag for frequency test

//---- PID Gains ----
extern int big_Kp;
extern int big_Ki;
extern int big_Kd;

extern int small_Kp;
extern int small_Ki;
extern int small_Kd;

//----current settings-----
extern const int uMAX;    // max voltage for the vref pins
extern int ITerm_max;
extern int phase_multiplier;


//---- Step settings -----
extern const int counts_per_revolution; // encoder counts for 360 degrees
extern const float angle_per_step;      // only needed for calibration routine by now
extern const float stepangle;             // angle of one step as int
extern const int PA;                    // angle of one fullstep
extern volatile int step_target;        // target as step gets incremented if an step is received


//--- lookup tables for the coils ---
extern const PROGMEM int_fast16_t sin_lookup[];
extern const PROGMEM int_fast16_t cos_lookup[];


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


extern const float M_Pi;

//--- PID autotuning variables ---
extern bool tune_running; //set flag for running indication

#endif
