//Contains the declaration of the state variables for the control loop
#include <Wire.h>
#include "State.h"
#include "Configuration.h"
#include "Configurationals.h"


//---- interrupt vars ----
volatile int r = 0;                 //target angle
volatile int y = 0;                //current angle
volatile int y_1 = 0;                //last angle

volatile int raw_0 = 0;             // current measured angle
volatile int raw_1 = 0;             // last measured angle
volatile int raw_diff = 0;          // diff of both

volatile int u = 0;                 // control effort

volatile int e_0 = 0;               // current error term
volatile int e_1 = 0;               // last error term

volatile bool dir = true;           // flag for dir setting
volatile bool enabled = true;       // flag for enabled setting

bool frequency_test = false;        // flag for frequency test

//---- PID Gains ----
volatile int Kp = init_Kp;
volatile int Ki = init_Ki;
volatile int Kd = init_Kd;

int big_Kp = (Kp * 2) / 10;
int big_Ki = Ki;
int big_Kd = (8 * Kd) / 3;

int small_Kp =  Kp;
int small_Ki = Ki / 2;
int small_Kd = Kd / 4;


volatile int step_target = 0;                                         // target as step gets incremented if an step is received



const float M_Pi = 3.1415926535897932384626433832795;

