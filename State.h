//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__


//interrupt vars

extern volatile float ei;
extern volatile float r;  //setpoint
extern volatile float y;  // measured angle
extern volatile float yw;
extern volatile float yw_1;
extern volatile float e;  // e = r-y (error)
extern volatile float p;  // proportional effort
extern volatile float i;  // integral effort
extern volatile float PA;  //

extern volatile float u;  //real control effort
extern volatile float e_0; 
extern volatile float e_1;
extern volatile float e_2;
extern volatile float z_0; 
extern volatile float z_1;

extern volatile long wrap_count;
extern volatile float y_1;


extern volatile float ITerm;

extern volatile char mode;


extern int dir;
extern int stepNumber;
extern void output(float, int);

extern volatile int enabled;

extern int uMAX;
extern int uPEAK;
extern int uSTEP;

extern volatile int PEAKCounter;
extern int maxPEAKCounter;
extern int PEAKSPERSECOND;

extern volatile float ITerm;

extern volatile char mode;


//___________________________________

extern const float pi;
extern const int  half;//128;

extern float new_angle; //input angle
extern float current_angle; //current angle
extern float diff_angle;
extern int val1;
extern int val2;


#endif
