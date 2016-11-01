# Mechaduino
derivation of the original Mechaduino code from jcchurch13.

orginal code can be found under: https://github.com/jcchurch13/Mechaduino-Firmware/tree/multi-file/Mechaduino_01/Mechaduino_01

- added an enable pin (pin 2, only 3.3 V compatible)
- some other changes 
- menu entry for generating an step response
- deletet all modes other then position 
- changed all float to integer operations
- moved the encoder reading in it's own ISR running at 30 kHz
- added an rolling average to the encoder reading
- changed the frequency of the PID loop to 4.8 kHz

