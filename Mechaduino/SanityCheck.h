
/**
   SanityCheck.h

   Test configuration values for errors at compile-time.
*/
#ifndef SANITYCHECK_H
#define SANITYCHECK_H

#include "board.h"
#include "Configuration.h"


#if !defined(ARDUINO) || ARDUINO < 10600
#error "Versions of Arduino IDE prior to 1.6.0 are no longer supported, please update your toolkit."
#endif

#ifndef _VARIANT_ARDUINO_ZERO_
#error "Oops!  Make sure you have 'Arduino Zero (Native USB Port)' selected from the 'Tools -> Boards' menu."
#endif


#if (language!=1) && (language!=2)
#line 87 "Configuration.h"
#error "Oops!  No valid language selected"
#endif

#if defined(use_PID) && defined(use_PIV)
#line 92 "Configuration.h"
#error "please use only one controll type"
#endif

#if !defined(use_PID) && !defined(use_PIV)
#line 92 "Configuration.h"
#error "please define one controll type"
#endif



#endif //SANITYCHECK_H
