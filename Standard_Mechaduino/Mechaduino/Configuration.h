#ifndef __CONFIGURATION_H__
#define __CONFIGURATION_H__

//---- Hardware ----
#define MECHADUINO_HARDWARE
//#define NZS_HARDWARE

//---- Identifier -----
#define identifier "Y"   // string to identify the mechaduino with the Serial monitor

//---- PID Frequenz -----
#define FPID (15000)  // frequency of the PID loop change only if you know what you're doing

//---- PID Values -----
#define init_Kp (2062)
#define init_Ki (53)
#define init_Kd (19951)

//----current settings-----
#define  iMAX (1000) // max current per coil 2000 mA for A4954 driver should be lower (thermal conditions)

//---- Baudrate -----
#define baudrate (250000)    // baudrate speed bit/s

//---- Enable Pins -----
#define USE_ENABLE_PIN		//flag for enable pin

//---- Invert direction ----
//#define INVERT          //uncomment this to invert your motor direction

//---- Step settings -----
#define microstepping (16)          // microstepping setting for step input
#define steps_per_revolution (200)  // fullsteps for 360 degrees

//---- Language ----
// 1 = english
// 2 = german
#define language (1)

#endif
