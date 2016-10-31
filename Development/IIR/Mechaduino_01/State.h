//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__


//interrupt vars
extern volatile float r;  //setpoint

extern volatile float raw_0;  // measured angle
extern volatile float raw_1;
extern volatile float raw_diff;


extern volatile const float PA;  //

extern volatile float u;  //real control effort
extern volatile float u_1;


extern volatile float ITerm; //integral term


extern volatile float e_0; // error term
extern volatile float e_1;


extern volatile float y_filtered; // raw measured wrapped angle


extern volatile float yw; //filtered wraped angle

extern volatile bool dir;
extern volatile bool enabled;


//----current settings-----
extern const int uMAX;
extern const int uDEADBAND;

//---- Step settings -----
extern const int counts_per_revolution;
extern const float angle_per_step;
extern const float stepangle;
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



//----Motordriver current values-----
extern int val1;
extern int val2;



extern int stepNumber; // step index for cal routine
extern float step_target;

extern bool frequency_test;

extern const float Pi;

extern const PROGMEM float sine_lookup[];

//----Filter Coeffizienten-----
extern float coeff_b0;
extern float coeff_b1;
extern float coeff_b2;
extern float coeff_a1;
extern float coeff_a2;
#endif
