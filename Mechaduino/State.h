//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__

//---- interrupt vars ----
extern volatile long r;            //target angle
extern volatile long y;           //current angle

extern volatile int u;            // control effort

extern volatile int electric_angle;


extern volatile bool dir;         // flag for  dir setting
extern volatile bool enabled;     // flag for  enabled setting

extern volatile bool frequency_test;       // flag for frequency test
extern volatile bool streaming;	  		// flag for data streaming

//---- PID Gains ----
extern volatile int int_Kp;
extern volatile int int_Ki;
extern volatile int int_Kd;

extern float D_pulley;
extern float J_load;

extern int int_J;


extern volatile int step_target;        // target as step gets incremented if an step is received

#endif
