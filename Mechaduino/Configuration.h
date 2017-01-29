#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

//---- Firmwareversion
#define firmware_version "0.2"

//---- Identifier -----
#define identifier "Y"   // string to identify the mechaduino with the Serial monitor



//---------------------------------------------- Hardware Secition ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//----current settings-----
#define  iMAX 1000 // max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)


//---- Enable Pins -----
#define USE_ENABLE_PIN    //flag for enable pin


//---- Invert direction ----
//#define INVERT          //uncomment this to invert your motor direction


//---- Step settings -----
#define microstepping 16          // microstepping setting for step input
#define steps_per_revolution 200  // fullsteps for 360 degrees
#define mm_rev 32                 // mm per revolution
#define error_led_value 0.05      // max error in mm, if the error gets bigger the led turns off


//---- motor parameters ----
// max moment in Nm
#define M_max 0.59

// current for max moment in mA
#define A_max 2000


//---- inertia ----
// rotor inertia in gcm^2
#define J 82

// mass of the load in grams
#define mass_load 300 





//---------------------------------------------- Controller Secition ----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---- PID Values -----
#define Kp 1.22900
#define Ki 0.03300
#define Kd 11.30900


//---- friction compensation ----
#define Kfr 55.72400


//---- velocity feedforward----
#define Kvff 15.00000



//----------------------------------------------- Signal Secition -----------------------------------------------
//---------------------------------------------------------------------------------------------------------------

//---- Baudrate -----
#define baudrate 250000   // baudrate speed bit/s


//---- PID Frequenz -----
#define FPID 10000  // frequency of the PID loop change only if you know what you're doing


//---- break frequency in hertz for DTerm----
#define pLPF 250


//---- break frequency in hertz for the effort----
#define uLPF 380


//---- break frequency in hertz for position----
#define positionLPF 800


//---- RAS constant in ms----
#define RAS 5




//--------------------------------------------------- Language --------------------------------------------------
//---------------------------------------------------------------------------------------------------------------
// 1 = english
// 2 = german
#define language 1

#endif
