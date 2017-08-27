![image](https://github.com/Kaiwol1990/Mechaduino/blob/dev/images/arduino.png)

Derivation of the original Mechaduino code from jcchurch13.

Original code can be found under: https://github.com/jcchurch13/Mechaduino-Firmware/tree/multi-file/Mechaduino_01/Mechaduino_01

# Firmware
### Changes:

- added an enable pin (pin 2, only 3.3 V compatible)
- Menu entry for generating a step response
- Menu entry for generating a dirac delta function
- deleted all modes other than position 
- changed all float to integer operations
- added some filtering to the effort and D-Term
- Changed the frequency of the PID loop to 5 kHz
- Encoder readings are placed in its own ISR that runs with 10 kHz (oversampling)
- maximal step frequency = 40 kHz
- changed the PWM resolution from 8 bit to 9 bit
- PID auto tuning
- The lookup table is now in its own file (lookup_table.cpp)
- The Parameters are now in Configuration.cpp
- Added Language file for easy translation
- added an invert direction setting
- simple error register to save if an error occurred
- LED shows errors by different blinking patterns
- downhill simplex algorithm to fine tune the pid settings

### LED blink pattern:
If an error occures the led will switch from constant on to different patterns. These patterns are separated from each other by a longer pause. After this pause one pattern will be shown and then after the pause the next pattern will be shown.

|Pattern| Error|
|-------|------|
|Fast blinking | Timing error. This can occur if the mechaduino receives too much steps. Consider lowering your microsteps or limit the maximal speed|
|Long on | maximal error was reached|
|Long on, one flash | the maximal current was reached|
|Long on, two flashs | the lookup table shows some errors|


### List of commands
To get an list of the possible commands send help over the serial monitor

|Command|Description|Arguments|
|-------|-----------|---------|
|help | prints this menu |
|calibrate | starts the calibration routine |-check -smooth |
|diagnose | read Encoder Diagnostics | |
|state | shows and sets the motor state | -on, -off |
|stream | streams servo parameters | -f {int}, -on, -off |
|parameter | Get or set parameter | -set {-parameter} {value}, -get |
|error | get or reset the error register | -reset |
|dirac | generates a Delta-Distribution | -f {int} |
|response | generates a step response | -f {int}, -s {int} |
|error | get or reset the error register | -reset |
|autotune | PID auto tune | -c {byte}, -d |
|downhill | Downhhill simplex algorithm | -f {int}, -v {int}, -o |
|testmove | Starts a testmove for the servo | -f {int}, -v {int}, -o |
|interrupt | Starts or stops the ISR | -TC5 {bool}, -TC4 {bool} |

# License
All Mechaduino related materials are released under the Creative Commons Attribution Share-Alike 4.0 License
https://creativecommons.org/licenses/by-sa/4.0/

Much of the work is based on Mechaduino project:
https://github.com/jcchurch13/Mechaduino-Firmware


# Support
If you want to support me for further improvements and things like that, you can send me a small kickback with PayPal

[![Donate](https://img.shields.io/badge/Donate-PayPal-green.svg)](https://www.paypal.com/cgi-bin/webscr?cmd=_s-xclick&hosted_button_id=64GHBDR3Z55JE)

