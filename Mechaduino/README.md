# Mechaduino
derivation of the original Mechaduino code from jcchurch13.

orginal code can be found under: https://github.com/jcchurch13/Mechaduino-Firmware/tree/multi-file/Mechaduino_01/Mechaduino_01

- added an enable pin (pin 2, only 3.3 V compatible)
- some other changes 
- menu entry for generating an step response
- deleted all modes other then position 
- changed all float to integer operations
- added some filtering to the coil current setting
- changed the frequency of the PID loop to 15-20 kHz
- changed the PWM resolution from 8 bit to 10 bit
- Processing script to display the step resonse
- PID autotuning


Changed some Parameters! 
- The lookuptable is now in its own file (lookup_table.cpp)
- the Parameters are now in Configuration.h
- Added Language support, by now only englisch and german
- added an invert direction setting
- major cleanup of the Serial interface


License

All Mechaduino related materials are released under the Creative Commons Attribution Share-Alike 4.0 License
Much of the work is based on Mechaduino project:
https://github.com/jcchurch13/Mechaduino-Firmware

If you want to support me for further improvments and things like that you can send me a small kickback with paypal


[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=64GHBDR3Z55JE)
