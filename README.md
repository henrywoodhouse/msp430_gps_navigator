# GPS Navigation Project
This was my attempt at creating a GPS navigation system inspired by BeeLine, which uses an arrow to direct you towards your destination.
The VK16E GPS module was a poor component choice as it has an accuracy within 30 meters which makes the navigation system unusable whilst walking or cycling.

The project made use of the following components:

* MSP430G2 LaunchPad
* VK16E GPS Module
* 8x8 Dot Matrix Screen
* 9V Battery
* 2 x 10UF Capacitors
* LM1086 3.3V Regulator

The electronic schematic is shown below:

![electronic schematic](https://github.com/henrywoodhouse/msp430_gps_navigator/blob/master/electronics_schematic.png "Electronic Schematic")

Once the circuit has been built as shown the code can be loaded onto the MSP430 using the [Energia IDE](http://energia.nu/download/)