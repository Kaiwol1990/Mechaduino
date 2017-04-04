#include "Configuration.h"


//-------------------------------------------------- Identifier -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// string to identify the mechaduino with the Serial monitor
char identifier = 'X';



//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
int iMAX = 1300;


//uncomment this if you want to use a enable pin
int USE_ENABLE_PIN = 1;

// microstepping setting for step input
int microstepping = 16;

// fullsteps for 360 degrees
int steps_per_revolution = 200;


// mm per revolution
int mm_rev  =32;

// max error in mm, if the error gets bigger the led turns off
float error_led_value = 0.05;

// mass of the load in g, can be set to 0 if not known
int m_load  =300;


//uncomment this to invert your motor direction
int INVERT = 0;




//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max moment in Nm
float M_max = 0.59;

// rated current for max moment in mA
int I_rated = 2000;

// rotor inertia
// rotor inertia in gcm^2
int J_rotor = 82;




//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---- PID Values current control -----

float Kp =1.14400;
float Ki =0.04200;
float Kd =7.86600;


//----------------------------------------------- Signal Section -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// baudrate speed bit/s
#define baudrate 250000


// frequency of the PID loop change only if you know what you're doing
#define FPID 10000


// break frequency in hertz for DTerm
int pLPF = 1000;



// break frequency in hertz for position
int encoderLPF = 400;




