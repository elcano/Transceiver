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

### Required libraries:

#### *Seeed-Studio/CAN-BUS-Shield*
Download as zip from https://github.com/Seeed-Studio/Seeed_Arduino_CAN. 
In Arduino IDE, go to Sketch -> Include Library -> Add .ZIP Library and select the .zip folder.

#### *elcano/Elcano_C2_LowLevel/Can_Protocol.h*
Download “Can_Protocol.h” from https://github.com/elcano/elcano/tree/master/Elcano_C2_LowLevel. 
Create a folder in the location where the Arduino IDE contains it’s libraries on your computer and add this file to the newly created folder.

#### *http://www.airspayce.com/mikem/arduino/RadioHead/classRH__RF69.html*
Download as zip from http://www.airspayce.com/mikem/arduino/RadioHead/index.html. 
In Arduino IDE, go to Sketch -> Include Library -> Add .ZIP Library and select the .zip folder.

#### *elcano/libraries/Settings/SettingsTemplate.h*
Download “SettingsTemplate.h” from https://github.com/elcano/elcano/tree/master/Elcano_C2_LowLevel. 
Create a folder in the location where the Arduino IDE contains it’s libraries on your computer and add this file to the newly created folder.

#### *scanse/sweep-arduino/tree/master/Sweep*
Download as zip from https://github.com/scanse/sweep-arduino. 
Note: need to unzip zip folder and then compress to zip only the sweep folder.
In Arduino IDE, go to Sketch -> Include Library -> Add .ZIP Library and select the .zip folder.