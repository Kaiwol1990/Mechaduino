#pragma once

#include <stdint.h>

#ifndef __ANALOGFASTWRITE_H__
#define __ANALOGFASTWRITE_H__

/*
   \brief SAMD products have only one reference for ADC
*/

extern void digitalWriteDirect(int PIN, bool val);

extern void analogFastWrite( uint32_t ulPin, uint32_t ulValue ) ;

#endif
