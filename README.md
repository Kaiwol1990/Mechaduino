# Mechaduino
derivation of the original Mechaduino code from jcchurch13.

orginal code can be found under: https://github.com/jcchurch13/Mechaduino-Firmware/tree/multi-file/Mechaduino_01/Mechaduino_01

- added an enable pin (pin 2, only 3.3 V compatible)
- some other changes 
- menu entry for generating an step response
- deleted all modes other then position 
- changed all float to integer operations
- added some filtering to the coil current setting
- changed the frequency of the PID loop to 5 kHz
- changed the PWM resolution from 8 bit to 9 bit
- PID autotuning
- moment of inertia compensation term


License

All Mechaduino related materials are released under the Creative Commons Attribution Share-Alike 4.0 License
Much of the work is based on Mechaduino project:
https://github.com/jcchurch13/Mechaduino-Firmware

If you want to support me for further improvments and things like that you can send me a small kickback with paypal


[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=64GHBDR3Z55JE)
