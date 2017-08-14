
#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__


//-------------------------------------------------- Identifier -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// string to identify the mechaduino with the Serial monitor
extern char identifier;



//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
extern int iMAX;


//uncomment this if you want to use a enable pin
extern int USE_ENABLE_PIN;

// microstepping setting for step input
extern int microstepping;

// fullsteps for 360 degrees
extern int steps_per_revolution;

// mm per revolution
extern int mm_rev;

// max error in mm, if the error gets bigger the led turns off
extern float error_led_value;

//uncomment this to invert your motor direction
extern int INVERT;




//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max moment in Nm
extern float M_max;

// rated current for max moment in mA
extern int I_rated;


//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---- PID Values current control -----

extern float Kp;
extern float Ki;
extern float Kd;

extern float Kff;
extern float Kacc;

//----------------------------------------------- Filter  Section -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// break frequency in hertz for DTerm
extern int D_Term_LPF;

// break frequency in hertz for effort filter
extern int u_LPF;






//----------------------------------------------- Internal Section ----------------------------------------------
//------------------------------------------------- don't change ------------------------------------------------
// baudrate speed bit/s
#define baudrate 1843200

// frequency of the PID loop change only if you know what you're doing
#define FPID 5000
#endif




