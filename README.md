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

TODO
