//Contains the declaration of the state variables for the control loop
#include "State.h"
#include "Configuration.h"
#include "Configurationals.h"


//---- interrupt vars ----
volatile int r = 0;                 //target angle
volatile int y = 0;                //current angle

volatile int u = 0;                 // control effort

volatile bool dir = true;           // flag for dir setting
volatile bool enabled = true;       // flag for enabled setting

volatile bool frequency_test = false;        // flag for frequency test

//---- PID Gains ----
volatile int int_Kp = Kp * 1000;
volatile int int_Ki = Ki * 1000;
volatile int int_Kd = Kd * 1000;

volatile int big_Kp = (int_Kp * 2) / 10;
volatile int big_Ki = int_Ki;
volatile int big_Kd = (8 * int_Kd) / 3;

volatile int small_Kp = int_Kp;
volatile int small_Ki = int_Ki / 2;
volatile int small_Kd = int_Kd / 4;


volatile int step_target = 0;      // target as step gets incremented if an step is received
