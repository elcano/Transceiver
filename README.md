# Transceiver
Hardware and software to support Transmit/Receive boards

The Elcano Project is an autonomous vehicle control system that runs on a stack of microprocessors, with no operating system and 
no machine learning.
Files that span more than one processor are kept in Elcano/General. The other repositories are:
Elcano/LowLevel – Drive by wire
Elcano/HighLevel – Localization, Route Finding and Pilot
Elcano/Sonar – Detects obstacles from ultrasonic sensors
Elcano/Sweep – Scanse Sweep Lidar for Obstacle Detection
Elcano/QDED – Quadrature Edge Detectors for machine vision
Elcano/Transceiver – Transmit and receive boards for remote control option

The primary autonomous vehicle control comes from HighLevel. Alternatively, the vehicle can be controlled manually, which is the function 
of the Tranceiver board. Depending on what is soldered into the board and which software is used, there are several configurations of the 
transceiver:
 
Transmitter board with joysticks and switches sending via radio.
Receiver board receiving from radio and putting information on CAN bus.
Transmitter via Bluetooth.
Receiver via Bluetooth.
Receiver from a hobbyist RC controller
User interface with joysticks and switches, putting information on CAN bus.

