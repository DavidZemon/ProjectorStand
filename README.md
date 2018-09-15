Specifications
==============

Overview
--------

Inside a house - safety is of the utmost concern

Requirements
------------

* Current sensor on motor driver to kill power in high current situation
* Power to motor is cut when travel reaches limit switch
* Optional: Redundant limit switches
* Optional: Calibration stage

* Microcontroller I/O:
  * Input
    * 2 x Digital: Momentary buttons for beginning movement
      Polarity ???
    * 2 x Digital: Limit switches at top and bottom of travel
      Immediately stop motor when switches change state
      Do not allow start of travel in limit state
    * 1 x Digital: Current sensor alert state on motor driver
      Movement should stop if current sensor throws an alert
    * 1 x Analog: Potentiometer controls speed of motor movement
  * Output
    * 1 x Digital: Motor polarity to control motor direction
    * 1 x Digital: Motor PWM to control motor speed/power
    * 1 x Digital: Status LED
    
Dream Features
--------------

* Phone control of projector and screen
* IR input/output for controlling projector power
* LCD display for potentiometer status
    
    
Architecture
------------

* Cog 1 - Main
  Wait on button push to begin movement. Upon button push:
  1. Check limit switch to ensure movement is allowed
  2. Read from potentiometer to determine speed
  3. Enable hardware PWM for motor control
  4. While limit switch not reached
     1. Pause briefly (10 ms?)
  5. Stop PWM signal 
* Cog 2 - Current sensor interrupt
  Watch digital input for high-current. If high-current detected:
  * Stop all other cogs
  * Blink error message on LED(s)
