
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

#if (m_load == 0)
#warning "The mass of the load is set to 0 inertia compensation will not work for the load!"
#endif

#if (m_load <0)
#error "the mass of the load can't be negative"
#endif


#if (J_rotor==0)
#warning "The inertia of the rotor set to 0 inertia compensation will not work for the motor!"
#endif

#if (J_rotor <0)
#error "the intertia of the rotor can't be negative"
#endif




#endif //SANITYCHECK_H
