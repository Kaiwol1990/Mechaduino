#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

//---- Hardware ----
#define MECHADUINO_HARDWARE
//#define NZS_HARDWARE

//---- Identifier -----
#define identifier "Y"   // string to identify the mechaduino with the Serial monitor


//---- Firmwareversion
#define firmware_version "0.2"


//---- inertia ----
#define J 100


//---- PID Frequenz -----
#define FPID 10000  // frequency of the PID loop change only if you know what you're doing


/*
//---- PID Values -----
#define Kp 0.63600
#define Ki 0.01040
#define Kd 13.4570
*/

//working classic PID
//---- PID Values -----
#define Kp 1.47900
#define Ki 0.04400
#define Kd 12.33200



//---- friction compensation ----
#define Kfr 15


//---- velocity feedforward----
#define Kvff 10

/*
  // some overshoot
  //---- PID Values -----
  #define Kp 0.6303
  #define Ki 0.01240
  #define Kd 30.552
*/


//---- break frequency in hertz for DTerm----
#define pLPF 250


//---- break frequency in hertz for the effort----
#define uLPF 500


//---- break frequency in hertz for position----
#define positionLPF 1000


//---- RAS constant in ms----
#define RAS 10


//----current settings-----
#define  iMAX 1000 // max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)


//---- Baudrate -----
#define baudrate 250000   // baudrate speed bit/s


//---- Enable Pins -----
#define USE_ENABLE_PIN		//flag for enable pin


//---- Invert direction ----
//#define INVERT          //uncomment this to invert your motor direction


//---- Step settings -----
#define microstepping 16          // microstepping setting for step input
#define steps_per_revolution 200  // fullsteps for 360 degrees


//---- Language ----
// 1 = english
// 2 = german
#define language 1


//---- PI or PID
//#define use_PI
#define use_PID

#endif
