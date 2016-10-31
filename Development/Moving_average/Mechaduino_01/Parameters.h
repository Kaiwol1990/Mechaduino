//Contains the Mechaduino parameter declarations

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

//---- Identifier -----
extern const String identifier;

//---- Sample Frequenz -----
extern volatile int FSAMPLE;
extern volatile int FPID;

//---- PID Values -----
extern volatile float pKp;
extern volatile float pKi;
extern volatile float pKd;

//----current settings-----
extern const float iMAX;

//---- Lookup -----
extern const PROGMEM float lookup[];

//---- Baudrate -----
extern const int baudrate;

//---- Pins -----
extern const int ena_pin;


//---- Step settings -----
extern const int microstepping;
extern const int steps_per_revolution;

//---- Filter settings -----
extern const char shifts;
extern const int LM_SIZE;
extern int LM[];
#endif
