# FSM_Cortex_ARM_M4_project
This project implements the realization of an embedded system project where I had to make a finite-state-machine-based simple device based on a STM32 Nucleo-32 board with a Cortex Arm M4 microprocessor. 

The device features ON/OFF functionality, a 4 option Menu, with a Distance measuring functionality, a lightmeter functionality, and a 3 LED light play. The project also has 2 7-segment LED displays operated through multiplexation to inform about the menu item selected. The 3 LEDs also have the functionality of informing the user about the duration of the pressing of the 3 buttons, which have implications in the functionality. 

The distance measurement is done with a Ultrasonic Telemetric Sensor, while the lightmeter with a fotoresistor. Both the lightmeter and the distance measuring have the options of continuous measurement or periodic sampling. 
