#pragma once

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
   \brief SAMD products have only one reference for ADC
*/

extern void digitalWriteDirect(int PIN, bool val);

extern void analogFastWrite( uint32_t ulPin, uint32_t ulValue ) ;


#ifdef __cplusplus
}
#endif
