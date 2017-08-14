//Contains the declaration of the state variables for the control loop
#include "State.h"
#include "Configuration.h"
#include "Configurationals.h"
#include <math.h>

//---- interrupt vars ----
volatile int r = 0;  //target angle
volatile int y = 0;  //current angle
volatile int raw_0 = 0;  //current shaft-angle
volatile int error = 0;  //error angle
volatile int omega = 0;  //error angle

volatile int u = 0;   // control effort

volatile int electric_angle = 0;


volatile bool dir = true;             // flag for dir setting
volatile bool enabled = true;         // flag for enabled setting
volatile bool frequency_test = false; // flag for frequency test
volatile bool streaming = false;    // flag for data streaming


// ----- array that holds the different possible errors -----
// ----- [timing error, error to big, ..., ..., ...]
volatile int error_register = 0B1000000000000000;

volatile int int_Kp = (Kp * 1024.0) + 0.5;
volatile int int_Ki = (Ki * 1024.0) + 0.5;
volatile int int_Kd = (Kd * 1024.0) + 0.5;

volatile int int_Kff = (Kff * 1024.0) + 0.5;
volatile int int_Kacc = (Kacc * 1024.0) + 0.5;


// calculate pid settings after pesson rule. is active for small errors
// https://en.wikipedia.org/wiki/Ziegler%E2%80%93Nichols_method#cite_note-microstar-2
volatile int int_pessen_Kp = (1024.0 * ((Kp * 0.7) / (0.6))) + 0.5;
volatile int int_pessen_Ki = (1024.0 * ((Ki * 0.7 * 2.5) / (2 * 0.6))) + 0.5;
volatile int int_pessen_Kd = (1024.0 * ((Kd * 8.0 * 0.7) / (15.0 * 0.6))) + 0.5;




//---- Calculating the inertia of the system ----
float D_pulley = (mm_rev / (10 * 3.14159283));



// variable for the target in steps
volatile int step_target = 0;      // target as step gets incremented if an step is received



//---- filter section ----
int D_Term_LPFa = ((128.0 * exp(D_Term_LPF * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
int D_Term_LPFb = 128 - D_Term_LPFa;


int u_LPFa = ((128.0 * exp(u_LPF * -2 * 3.14159283 / FPID)) + 0.5); // z = e^st pole mapping
int u_LPFb = 128 - u_LPFa;



int max_serial_counter = 20;


int fifo[8][100] = {0};
int fifo_counter = 0;
int serial_counter = 0;





