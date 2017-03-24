//Contains the declaration of the state variables for the control loop
#include "State.h"
#include "Configuration.h"
#include "Configurationals.h"


//---- interrupt vars ----
volatile long r = 0;  //target angle
volatile long y = 0;  //current angle

volatile int u = 0;   // control effort

volatile int electric_angle = 0;


volatile bool dir = true;             // flag for dir setting
volatile bool enabled = true;         // flag for enabled setting
volatile bool frequency_test = false; // flag for frequency test
volatile bool streaming = false;    // flag for data streaming


volatile int int_Kp = Kp * 1000;
volatile int int_Ki = Ki * 1000;
volatile int int_Kd = Kd * 1000;




//---- Calculating the inertia of the system ----

// calculated from F = m*a
// calculating the moment from the Force with M = F * r
// r = mm_rev / (2*Pi)
// calculating omega_dot from a
// a = omega_dot * mm/rad  --> mm/rad = mm_rev /2
// or J_load = 1/4 * m_load * D_pulley^4
// D in cm
float D_pulley = (mm_rev / (10 * 3.14159283));
float J_load = ((m_load * D_pulley * D_pulley) / 4.0);



// M = J * omega_dot  with J[kg*m^2]  omega_dot [rad/s]  - needed Torque
// M = I * (M_max / I_rated)   - available Torque
// I = M * (I_rated / M_max)
// I = J * omega_dot * (I_rated / M_max)
// I = J * omega_dot * (pi/360.0) * (I_rated / M_max)   with omega_dot in [deg/s]


const float rSense = 0.150; // resistor value


// 1000 for int instead of float             from I to u                            from M to I                J from gcm^2 to kgm^2                           from deg/s to rad/s    from 100*deg/cycle to deg/s
const int int_J = (1000.0 * ( ((512.0 * 10.0 * rSense) / (1000.0 * 3.3)) * ((float)I_rated / (float)M_max) *  (((float)J_rotor + (float)J_load) / (1000.0 * 100.0 * 100.0)) * (3.14159283 / 360.0) * ((float)FPID / 100.0))) + 0.5;




// variable for the target in steps
volatile int step_target = 0;      // target as step gets incremented if an step is received




