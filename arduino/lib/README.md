# Arduino libraries that are required for the derotator

This directory contains all the libraries that are necessary for
building *derot.ino*. All these libraries have to be installed in

*~/Documents/Arduino/libraries*

on a Mac. For Linux or a PC, you will have to install it in the
correct place that the Arduino compiler requires.

## Libraries

* **BaseServer** the base class for SerialServer and TCPServer.
* **DeRotator** the class  calculates the amount of derotation
given the initial alt-az position of the star
* **SerialServer** the class that sets up serial port 0 to listen to
the user commands.
* **TCPServer** the class that sets up WIFI to listen to user
commands.
* **Telescopes** the class that allows the user to communicate with
the LX200.
* **UserIO** the class that prints out messages to the LCD and serial
port and reads the push buttons.

### Libraries that are zip'ed

These libraries come from Arduino or from Adafruit that I have
modified to support *derot.ino*

* **AccelStepper.zip** added *resetTime()*
* **Adafruit_CC3000_Server.zip** changed unsigned long
<section>aucKeepalive  = MIN_TIMER_VAL_SECONDS;</section>
* **ArduinoMenu.zip** Added *keyADAFRUITStream.h* and
*menuADAFRUITLCD.h* to support menuing on the ADAFruit LCD screen.
* **Adafruit-RGB-LCD-Shield.zip** No changes.

These files have to be unzipped and installed in

*~/Documents/Arduino/libraries*

on a Mac. For Linux or a PC, you will have to install it in the
correct place that the Arduino compiler requires.




