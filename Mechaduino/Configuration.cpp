#include "Configuration.h"


//-------------------------------------------------- Identifier -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// string to identify the mechaduino with the Serial monitor
char identifier = 'Y';



//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
int iMAX = 1300;


//uncomment this if you want to use a enable pin
int USE_ENABLE_PIN = 1;

// microstepping setting for step input
int microstepping = 64;

// fullsteps for 360 degrees
int steps_per_revolution = 200;


// mm per revolution
int mm_rev  = 32;

// max error in mm, if the error gets bigger the led turns off
float error_led_value = 0.05;

// mass of the load in g, can be set to 0 if not known
int m_load  = 300;


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

float Kp = 0.95600;
float Ki = 0.06400;
float Kd = 3.59800;


float Kvff = 10.0;
float Kff = 0.250;


//----------------------------------------------- Signal Section -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------


// break frequency in hertz for DTerm
int D_Term_LPF = 800;



// break frequency in hertz for position
int Encoder_LPF = 2000;




