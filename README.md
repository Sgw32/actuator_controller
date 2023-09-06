# actuator_controller
Actuator controller on STM32

Electronic control unit is based on an STM32 MCU. The ECU controls a linear actuator. The
actuator can be extended, shrinked or kept in steady state. Moving the actuator is controlled by
two digital outputs (GPIO). Setting one pin enables moving in one direction, another - in
opposite direction. Endpoints of the actuator's  working range are configured mechanically by
adjusting 2 end switches. Actuator can pass the switches. Moving velocity is constant (and probably not equal)
in both directions.

For testing 2 LEDs and 2 buttons are used. LEDs are connected to the control outputs and
indicate movement of the actuator. Buttons are used to imitate the end switches.

Here the Bluepill on STM32F103C8T6 is used for evaluation

Project files are provided for Keil and for plain Linux Makefile project (gnu-arm-none-eabi-gcc) 
Files are made by STM32CubeMX wizard. 


# Pinout:
Forward output: PB15
Backward output: PB14

Forward endpoint: PB13 (normally set to 1 by pull up)
Backward endpoint:.PB12.(normally set to 1 by pull up)

![Prototype](img/pinout.png?raw=true "Prototype")

# Prototype picture
![Prototype](img/prototype.jpg?raw=true "Prototype")

# Building

Use MDK-ARM v5 or Linux machine with Make (or eclipse with make environment).
Open project or write "make" in project folder. 

# Evaluation

Normally started, we check for endpoints status. 
- If no switched endpoints, move backwards until endpoint. Timeout is set to 10 seconds. 
After touching backward endpoint we calibrate forward velocity by measuring time, and then backward velocity again by measuring time. 
- If we are on forward endpoint, move backward is in previous case
- If we are on backward endpoint, move forward until endpoint, and then measure backward and forward velocities by measured time. 
- From the last endpoint, move in opposite direction by corresponding time / 2. Stop here and threat it as a middle point. 

To evaluate such a behaviour, press a backward endpoint button soon after start, then press forward endpoint button, and then again backward point(calibration of time). Then wait for middle point position. 
