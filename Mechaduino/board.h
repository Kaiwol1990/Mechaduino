#ifndef __BOARD_H__
#define __BOARD_H__

#include <Arduino.h>
#include "Configuration.h"
#include "Utils.h"
#include "analogFastWrite.h"

//---- Pins -----
#define IN_4 (6)          //PA20
#define IN_3 (5)          //PA15
#define VREF_2 (4)        //PA08
#define VREF_1 (9)        //PA07
#define IN_2 (7)          //PA21
#define IN_1 (8)          //PA06
#define ledPin (13)       //PA17
#define chipSelectPin (A2)//PB08
#define dir_pin (0)       //PORT_PA11
#define step_pin (1)      //Port_PA10
#define ena_pin (2)       //PORT_PA14




static void setupPins() {

  pinMode(VREF_2, OUTPUT);
  pinMode(VREF_1, OUTPUT);
  pinMode(IN_4, OUTPUT);
  pinMode(IN_3, OUTPUT);
  pinMode(IN_2, OUTPUT);
  pinMode(IN_1, OUTPUT);
  pinMode(ledPin, OUTPUT);
  pinMode(step_pin, INPUT);
  pinMode(dir_pin, INPUT);
  pinMode(chipSelectPin, OUTPUT); // CSn -- has to toggle high and low to signal chip to start data transfer

  attachInterrupt(step_pin, stepInterrupt, RISING);
  attachInterrupt(dir_pin, dirInterrupt, CHANGE);

#ifdef USE_ENABLE_PIN
  pinMode(ena_pin, INPUT_PULLUP);
  attachInterrupt(ena_pin, enaInterrupt, CHANGE);
#endif

#ifdef NZS_HARDWARE
  pinMode(PIN_SW1, INPUT_PULLUP);
  pinMode(PIN_SW3, INPUT_PULLUP);
  pinMode(PIN_SW4, INPUT_PULLUP);
  pinMode(PIN_AS5047D_PWR, OUTPUT);
  pinMode(PIN_YELLOW_LED, OUTPUT);

  digitalWrite(PIN_YELLOW_LED, HIGH);
  digitalWrite(PIN_AS5047D_PWR, LOW); //turn AS5047D on
#endif

  digitalWriteDirect(IN_4, LOW);
  digitalWriteDirect(IN_3, LOW);
  digitalWriteDirect(IN_2, LOW);
  digitalWriteDirect(IN_1, LOW);

  digitalWriteDirect(VREF_2, LOW);
  digitalWriteDirect(VREF_1, LOW);
}
#endif//__BOARD_H__

