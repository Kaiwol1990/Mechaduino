//Contains the Mechaduino parameter declarations

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

//---- Identifier -----
extern const String identifier;

//---- Sample Frequenz -----
extern volatile int FSAMPLE;
extern volatile int FPID;

//---- PID Values -----
extern volatile int Kp;
extern volatile int Ki;
extern volatile int Kd;

//----current settings-----
extern const float iMAX;

//---- Lookup -----
extern const PROGMEM int lookup[];

//---- Baudrate -----
extern const int baudrate;

//---- Enable Pins -----
extern const bool use_enable_pin;


//---- Step settings -----
extern const int microstepping;
extern const int steps_per_revolution;

//---- Filter settings -----
extern const char shifts;
extern const int LM_SIZE;
extern int LM[];
#endif
