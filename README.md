# Implementation of OpenCV with the UON EEE-Bot

Utilised a Raspberry Pi and an Arduino Nano to control the EEE-Bot using OpenCV

**How It Worked**
1. The Raspberry Pi Zero ran a basic CPP program capturing images and attempting to detect a set of known patterns.
2. Once an Image was detected, the Pi would send a manoeuvre command to the Arduino Nano via i2c (with a voltage level shifter).
3. The Arduino Nano would then perform the manoeuvre via PWM control of the two rearwards motors.
4. Upon completetion of the manoeuvre, the aruino would signal back to the Raspberry Pi and the program would continue.
