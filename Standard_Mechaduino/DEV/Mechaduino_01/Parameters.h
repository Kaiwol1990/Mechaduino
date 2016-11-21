//Contains the Mechaduino parameter declarations

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

//---- Identifier -----
extern const String identifier;   // string to identify the mechaduino with the Serial monitor

//---- Sample Frequenz -----
extern volatile int FPID;   // frequency of the PID loop

//---- PID Values -----
extern volatile int Kp;   // proportional constant
extern volatile int Ki;   // integrational constant
extern volatile int Kd;   // differential constant

//----current settings-----
extern const float iMAX; // max current per coil 2.0 A for A4954 driver should be lower (thermal conditions)

//---- Lookup -----
extern const PROGMEM int lookup[]; // lookup table for the encoder reading

//---- Baudrate -----
extern const int baudrate;     // baudrate speed bit/s

//---- Enable Pins -----
extern const bool use_enable_pin;     //flag for enable pin

//---- Step settings -----
extern const int microstepping;         // microstepping setting for step input
extern const int steps_per_revolution;  // fullsteps for 360 degrees

#endif
