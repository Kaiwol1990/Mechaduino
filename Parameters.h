//Contains the Mechaduino parameter declarations

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__
//


//----Filter Coeffizienten-----
extern const float coeff_b0;
extern const float coeff_b1;
extern const float coeff_b2;
extern const float coeff_b3;

extern const float coeff_a1;
extern const float coeff_a2;
extern const float coeff_a3;

extern volatile float e_0;
extern volatile float e_1;
extern volatile float e_2;
extern volatile float e_3;

extern volatile float z_0;
extern volatile float z_1;
extern volatile float z_2;
extern volatile float z_3;

//----Current Parameters-----

extern volatile float Ts;
extern volatile float Fs;

extern volatile float pKp;
extern volatile float pKi;
extern volatile float pKd;

extern volatile float vKp;
extern volatile float vKi;
extern volatile float vKd;


//////////////////////////////////////
//////////////////PINS////////////////
//////////////////////////////////////
extern int ena_pin;
extern int IN_4;
extern int IN_3;
extern int VREF_2;
extern int VREF_1;
extern int IN_2;
extern int IN_1;
extern int LED;
extern const int ledPin;
extern const int chipSelectPin;

extern int step_pin;
extern int dir_pin;


//pins for debugging waveforms:
//extern int sine_out;
//extern int encoder_out;

extern const int microstepping;
extern const int spr; //  200 steps per revolution
extern const float aps; // angle per step
extern int cpr; //counts per rev

extern const float stepangle; //angle Increment per step


extern int dir;		//initialize stepping mode variables
extern int step_state;


extern long angle; //holds processed angle value

extern float anglefloat;

extern int a;  // raw encoder value in closed loop and print_angle routine (should fix the latter to include LUT)

extern int b;

//extern volatile long step_count;  //For step/dir interrupt

extern volatile int interrupted;

extern int stepNumber; // step index for cal routine


extern const PROGMEM float sine_lookup[];
extern const PROGMEM float lookup[];
extern const PROGMEM float force_lookup[];
#endif
