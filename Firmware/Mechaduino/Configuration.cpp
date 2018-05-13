#include "Configuration.h"


//-------------------------------------------------- Identifier -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// String to identify the mechaduino with the Serial monitor
String identifier = "Y1-Servo";



//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
int iMAX = 1500;

// peak current that is limited to the Peaktime. When the limit was reached for the time it can't be reached for the same time constatn again
int iPEAK = 1800;
int tPEAK = 2;
int tDEAD = 200;

//set to 1 if you want to use a enable pin
int USE_ENABLE_PIN = 1;

// microstepping setting for step input
int microstepping = 32;

// fullsteps for 360 degrees
int steps_per_revolution = 200;

// mm per revolution
int mm_rev  = 32;

// max error in mm, if the error gets bigger the led turns off
float error_led_value = 1.00;

//set to 1 to invert your motor direction
int INVERT = 0;



//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max moment in Nm
float M_max = 0.45;

// rated current for max moment in mA
int I_rated = 2000;


//----- Coil calibrtaions---------

int16_t gainCoilA = 7898;
int16_t gainCoilB = 8192;


//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---- PID Values current control -----
//1500 mA coil current
/*
  float Kp = 0.82138;
  float Ki = 0.01975;
  float Kd = 7.38035;

  float Kff = 0.17123;

  float Kacc = 0.54066;

  float Kb  = 0.05332;
*/

float Kp = 1.12138;
float Ki = 0.02324;
float Kd = 5.38682;

float Kff = 0.24123;

float Kacc = 0.54066;

float Kb  = 0.05332;


//----------------------------------------------- Filter  Section -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// break frequency in hertz for the error filter
int error_LPF = 500;


