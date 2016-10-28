//Contains the Mechaduino parameter declarations

#ifndef __PARAMETERS_H__
#define __PARAMETERS_H__

//---- Identifier -----
extern const String identifier;

//---- Sample Frequenz -----
extern volatile int Fs;


//---- PID Values -----
extern volatile float pKp;
extern volatile float pKi;
extern volatile float pKd;

//----current settings-----
extern const int PEAKSPERSECOND;
extern const float iMAX;
extern const float iPEAK;

//---- Lookup -----
extern const PROGMEM float lookup[];

//---- Baudrate -----
extern const int baudrate;

//---- Pins -----
extern const int ena_pin;


//---- Step settings -----
extern const int microstepping;
extern const int steps_per_revolution;

//----Filter Coeffizienten-----
extern float coeff_b0;
extern float coeff_b1;
extern float coeff_b2;
extern float coeff_a1;
extern float coeff_a2;
#endif
