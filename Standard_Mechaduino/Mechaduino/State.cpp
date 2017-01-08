//Contains the declaration of the state variables for the control loop
#include "State.h"
#include "Configuration.h"
#include "Configurationals.h"
#include "math.h"


//---- interrupt vars ----
volatile int r = 0;                 //target angle
volatile int y = 0;                //current angle

volatile int u = 0;                 // control effort

volatile bool dir = true;           // flag for dir setting
volatile bool enabled = true;       // flag for enabled setting

volatile bool frequency_test = false;        // flag for frequency test

volatile bool anticogging = false; 

const int pLPFa = (1000 * exp(pLPF * -2 * 3.14159283 / FPID)); // z = e^st pole mapping
const int pLPFb = (1000 - pLPFa);

const int positionLPFa = (1000 * exp(positionLPF * -2 * 3.14159283 / FPID)); // z = e^st pole mapping
const int positionLPFb = (1000 - positionLPFa);


const int uLPFa = (1000 * exp(uLPF * -2 * 3.14159283 / FPID)); // z = e^st pole mapping
const int uLPFb = (1000 - uLPFa);

const int RASa = (1000 * exp((1000 / RAS) * -2 * 3.14159283 / FPID)); // z = e^st pole mapping
const int RASb = (1000 - RASa);

#if defined(use_PID)
//---- PID Gains ----
volatile int int_Kp = Kp * 1000;
volatile int int_Ki = Ki * 1000;
volatile int int_Kd = Kd * 1000;

volatile int int_Kvff = Kvff * 1000;


#elif defined(use_PI)
//---- PD Gains ----
volatile int  int_Kp = (( Kp * 1000 * 2) / 3);
volatile int  int_Ki = (( Ki * 1000 * 0.48 / 1.2));
volatile int  int_Kd = ( Kd * 1000) / 10;


#endif


volatile int step_target = 0;      // target as step gets incremented if an step is received
