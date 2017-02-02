//Contains the declaration of the state variables for the control loop
#include "State.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "math.h"


//---- interrupt vars ----
volatile long r = 0;                //target angle
volatile long y = 0;                //current angle

volatile int u = 0;                 // control effort

volatile bool dir = true;           // flag for dir setting
volatile bool enabled = true;       // flag for enabled setting
volatile bool response = false;   // flag for step response

volatile bool frequency_test = false;        // flag for frequency test

volatile int int_Kp = Kp * 1000;
volatile int int_Ki = Ki * 1000;
volatile int int_Kd = Kd * 1000;

volatile int int_Kvff = Kvff * 1000;

volatile int int_Kfr = Kfr * 1000;


// calculated from F = m*a
// calculating the moment from the Force with M = F * r
// r = mm_rev / (2*Pi)
// calculating omega_dot from a
// a = omega_dot * mm/rad  --> mm/rad = mm_rev /2
// or J_load = 1/4 * m_load * D_pulley^4
// D in cm
float D_pulley = (mm_rev/(10*3.14159283));
float J_load = ((m_load * D_pulley * D_pulley)/4.0);


// calculating the necessary current
// u = M / (Nm/mA)
// u = M * mA_M_max / M_max
// 10000000 to fit the units
// 1000 to get a int from the float
volatile int int_J = 1000 * (((J_rotor + J_load) * I_rated) / (10000000 * M_max));

// variable for the target in steps
volatile int step_target = 0;      // target as step gets incremented if an step is received
