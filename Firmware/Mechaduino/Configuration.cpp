#include "Configuration.h" 
 
 
//-------------------------------------------------- Identifier -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// String to identify the mechaduino with the Serial monitor
String identifier = "Y-Servo";
 
 
 
//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
int iMAX = 1500;
 
//set to 1 if you want to use a enable pin
int USE_ENABLE_PIN = 1;
 
// microstepping setting for step input
int microstepping = 32;
 
// fullsteps for 360 degrees
int steps_per_revolution = 200;
 
// mm per revolution
int mm_rev  =32;
 
// max error in mm, if the error gets bigger the led turns off
float error_led_value = 1.00;
 
//set to 1 to invert your motor direction
int INVERT = 0;
 
 
 
//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max moment in Nm
float M_max = 0.59;
 
// rated current for max moment in mA
int I_rated = 2000;
 
 
 
//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
//---- PID Values current control -----
//1500 mA coil current
float Kp = 0.86493;
float Ki = 0.02262;
float Kd = 7.89346;
 
float Kff = 0.21412;
 
float Kacc = 0.87333;
 
 
//----------------------------------------------- Filter  Section -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// break frequency in hertz for the error filter
int error_LPF = 229;
 
 
