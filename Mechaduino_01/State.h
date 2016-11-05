//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__


//interrupt vars
extern volatile int r;  //setpoint

extern volatile int raw_0;  // measured angle
extern volatile int raw_1;
extern volatile int raw_diff;

extern volatile int reading ;

extern volatile const int PA;  //

extern volatile int u;  //real control effort

extern volatile int ITerm; //integral term

extern volatile int e_0; // error term
extern volatile int e_1;

extern volatile int yw_0; //filtered wraped angle
extern volatile int y;

extern volatile bool dir;
extern volatile bool enabled;


//----current settings-----
extern const int uMAX;

//---- Step settings -----
extern const int counts_per_revolution;
extern const float angle_per_step;
extern const int stepangle;
extern const int microstepping;
extern const int steps_per_revolution;

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


//----Motordriver current values-----
extern int val1;
extern int val2;



extern int stepNumber; // step index for cal routine
extern int step_target;

extern bool frequency_test;

extern const float Pi;

extern const PROGMEM int sine_lookup[];

extern char pointer;
extern  long sum;

#endif
