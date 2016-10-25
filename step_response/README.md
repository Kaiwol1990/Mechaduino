# Step response
Processing script to plot the step response reportet from the machaduino



---------------------------
Download Processing from:
https://processing.org/
---------------------------

# How to use:

1. Set all your PID values to zero an upload the firmware (your motor should be installed in your printer or wher ever you want to use it)

2. wait till the Mechaduino is ready and then close every serial monitor etc.

3. in step_response.pde line 9 set int step = 200; to whatever stepsizeyou want to use

4. in step_response.pde line 34 set   String portName = Serial.list()[   x  ]; set x to whatever your port is (1,2,3... depending on how mutch serial ports are available)

5. make sure your motor can move the amount of degree you set it to in both directions

6. start the step_response.pde script and wait a few seconds, it should open a window an some time later plot 3 graphes

7. those graphes are your step and step response of the system

8. close the plot and open the serial monitor in Arduino IDE

9. send an e to get to paramter edit menu and then the new paramter you want to set (something like p10.0, to set kp to 10.0)

10. close the serial monitor an repeat with step 6.


when you are finished open the serial monitor an send an p to get the parameters, you can copy it directly into parameters.cpp.

this gif shows how you want to set the single values eatch at the time:

![image](PID_Compensation_Animated.gif)


but there are other ways to set the values just  search a little bit for pid tuning everybody use it own methods.

It's importent to copy the values from the serial monitor to your paramters.cpp since the Mechaduino dosn't use an eeprom by now.

I hope i made everything clear.
