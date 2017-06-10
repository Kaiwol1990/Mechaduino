#ifndef __LOOKUP_TABLE_H__
#define __LOOKUP_TABLE_H__

#include <avr/pgmspace.h>

//---- Lookup -----
extern const unsigned int lookup[] PROGMEM; // lookup table for the encoder reading


extern const unsigned int cogging_lookup[] PROGMEM; // lookup table for the cogging_effort can be calibratet after main tuining


#endif
