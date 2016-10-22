//Contains the declaration of the state variables for the control loop

#ifndef __STATE_H__
#define __STATE_H__


//interrupt vars
extern volatile float r;  //setpoint
extern volatile float y;  // measured angle
extern volatile float y_1;
extern volatile float yw;

extern volatile float PA;  //

extern volatile float u;  //real control effort (not abs)

extern volatile float ITerm; //Integral term


extern volatile long wrap_count;


extern volatile float e;// e = r-y (error)
extern volatile float e_1;
extern volatile float e_2;

extern volatile float raw_0;
extern volatile float raw_1;
extern volatile float raw_2;


extern volatile float ITerm;

extern volatile char mode;


extern int dir;
extern int stepNumber;
extern void output(float, int);

extern volatile int enabled;

extern volatile int uMAX;
extern volatile int uPEAK;
extern volatile int uSTEP;

extern volatile int PEAKCounter;
extern int maxPEAKCounter;
extern int PEAKSPERSECOND;


//___________________________________
extern int val1;
extern int val2;


#endif
