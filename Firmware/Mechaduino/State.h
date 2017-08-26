//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__
#include <stdint.h>

//---- interrupt vars ----
extern volatile int r;            //target angle
extern volatile int y;           //current angle
extern volatile int error;      //error angle
extern volatile int omega;      //error angle
extern volatile int omega_target;  //error angle

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

extern volatile int int_Kff;
extern volatile int int_Kacc;

extern volatile int step_target;        // target as step gets incremented if an step is received


//---- filter section ----
extern int error_LPFa; // z = e^st pole mapping
extern int error_LPFb;


extern bool stream_variables[8];;

extern int max_serial_counter;


extern int fifo[8][50];
extern int fifo_counter;
extern int serial_counter;



extern const uint16_t lookup[]; // lookup table for the encoder reading



#endif
