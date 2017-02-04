#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__


//-------------------------------------------------- Identifier -------------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// string to identify the mechaduino with the Serial monitor
#define identifier "Y"




//---------------------------------------------- Hardware Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)
#define  iMAX 1000


//uncomment this if you want to use a enable pin
#define USE_ENABLE_PIN


// microstepping setting for step input
#define microstepping 16


// fullsteps for 360 degrees
#define steps_per_revolution 200


// mm per revolution
#define mm_rev 32


// max error in mm, if the error gets bigger the led turns off
#define error_led_value 0.05


// mass of the load in g, can be set to 0 if not known
#define m_load 300


//uncomment this to invert your motor direction
//#define INVERT




//------------------------------------------------ Motor Section ------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// max moment in Nm
#define M_max 0.59

// rated current for max moment in mA
#define I_rated 2000

// rotor inertia
// rotor inertia in gcm^2
#define J_rotor 82




//---------------------------------------------- Controller Section ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---- PID Values -----
#define Kp 1.22900
#define Ki 0.03300
#define Kd 11.30900


//---- friction compensation ----
#define Kfr 55.72400


//---- velocity feedforward----
#define Kvff 15.00000




//----------------------------------------------- Signal Section -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

// baudrate speed bit/s
#define baudrate 250000


// frequency of the PID loop change only if you know what you're doing
#define FPID 10000


// break frequency in hertz for DTerm
#define pLPF 250


// break frequency in hertz for the effort
#define uLPF 600


// break frequency in hertz for position
#define positionLPF 800


// RAS constant in ms
#define RAS 1




//--------------------------------------------------- Language --------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// 1 = english
// 2 = german
#define language 1


#endif
