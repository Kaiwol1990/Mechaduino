//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__

//---- interrupt vars ----
extern volatile int r;            //target angle
extern volatile int y;           //current angle
extern volatile int raw_0;      //current shaft-angle
extern volatile int error;      //error angle

extern volatile int u;            // control effort

extern volatile int electric_angle;


extern volatile bool dir;         // flag for  dir setting
extern volatile bool enabled;     // flag for  enabled setting

extern volatile bool frequency_test;       // flag for frequency test
extern volatile bool streaming;	  		// flag for data streaming


extern volatile int error_register;

//---- PID Gains ----
extern volatile int int_Kp;
extern volatile int int_Ki;
extern volatile int int_Kd;


extern volatile int step_target;        // target as step gets incremented if an step is received



//---- filter section ----
extern int D_Term_LPFa; // z = e^st pole mapping
extern int D_Term_LPFb;


extern int u_LPFa; // z = e^st pole mapping
extern int u_LPFb;





#endif
