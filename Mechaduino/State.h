//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__

//---- interrupt vars ----
extern volatile long r;            //target angle
extern volatile long y;           //current angle

extern volatile int u;            // control effort

extern volatile bool dir;         // flag for  dir setting
extern volatile bool enabled;     // flag for  enabled setting
extern volatile bool response;    // flag for step response

extern volatile bool frequency_test;       // flag for frequency test

//---- PID Gains ----
extern volatile int int_Kp;
extern volatile int int_Ki;
extern volatile int int_Kd;

extern volatile int int_Kvff;

extern volatile int int_Kfr;

extern volatile int int_pa_Kp;
extern volatile int int_pa_Ki;
extern volatile int int_pa_Kd;

extern const int int_J;

extern volatile int step_target;        // target as step gets incremented if an step is received

#endif
