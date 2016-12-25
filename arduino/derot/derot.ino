/*$Id$*/
/*
    derot is the controller code for the Arduino MEGA2560
    Copyright (C) 2015  C.Y. Tan
    Contact: cytan299@yahoo.com

    This file is part of derot

    derot is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    derot is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with derot.  If not, see <http://www.gnu.org/licenses/>.

*/

/* operating system header files (use <> for make depend) */
#include <EEPROM.h>
#include <string.h>
#include <math.h>

/* general system header files (use "" for make depend) */

#include "Adafruit_CC3000.h"
#include "ccspi.h"
#include "SPI.h"
#include "Arduino.h"
#include "AccelStepper.h"

#include "Wire.h"
#include "Adafruit_MCP23017.h"
#include "Adafruit_RGBLCDShield.h"
#include "menuADAFRUITLCD.h"
#include "keyADAFRUITStream.h"
#include "chainStream.h"

/* local include files (use "") */
#include "Telescope.h"
#include "BaseServer.h"
#include "TCPServer.h"
#include "SerialServer.h" 
#include "DeRotator.h"
#include "UserIO.h"


/**********************************************************************
  Define the number of teeth on each gear in the de-rotator mechanism
 **********************************************************************/  

#define Td 		17.0   // number of teeth of gear on the stepper
#define T1C		41.0   // number of teeth connected to Td
#define T2C		16.0   // number of teeth on top T1C
#define Tf		200.0  // number of teeth connected to T1C


#define GEAR_RATIO	T1C/Td*Tf/T2C

/*
   number of stepper motor steps to turn 360 deg
*/
#define STEPPER_360	200.0

/*
   the mechanical step size in deg/step

	The default mechanical step size is calculated as follows:
		# of teeth on stepper motor = 17 = Td
		# of teeth on gear that connects to Td, T1C  = 41 
		# of teeth on gear that sits on top of T1C, T2C = 16 
		# of teeth on gear that connects to T2C, Tf  = 200

	The final gear ratio is:
		T1C/Td * Tf/T2C = (41/17)*(200/16) = 30.14705882

	i.e. 1 360 degree turn of the final gear (Tf) requires
	30.14705882 turns of the drive gear (Td).
	=> 1 turn of Td gives 360.0/30.1405882 = 11.94146341 degrees
	of Tf

	The stepper motor takes 200 steps to make 1 turn (because it
	is 1.8 deg/step and thus 360/1.8 = 200), this means 1 step of
	the stepper motor gives: 11.94146341/200.0 = 0.0597 deg/step

	For accuracy: the default mechanical stepsize is
		mechanical_stepsize = 360.0/(T1C/Td*Tf/T2C)/(200.0) =
		0.05970731707 deg/step
*/
#define MECHANICAL_STEPSIZE	360.0/(GEAR_RATIO*STEPPER_360)

UserIO userio;
Telescope telescope;
DeRotator derotator(&telescope, MECHANICAL_STEPSIZE);
TCPServer tcpServer(&userio, &derotator);
SerialServer serialServer(&userio, &derotator);


/**********************************************************************
NAME

	derot - calculates the angular speed required to correct for
		field rotation. The angular speed is applied to the
		stepper motor.
		   
SYNOPSIS
	
	derot calculates the angular speed required to correct
	for field rotation. This angular speed is applied to the
	stepper motor.
	
AUTHOR
	C.Y. Tan

REVISION
	$Revision$

SEE ALSO

**********************************************************************/

void setup()
{

  Serial.begin(115200);
  while(!Serial);
  Serial.println(F("Serial port is open"));
  
  userio.Init(&derotator, &telescope, &tcpServer);
  userio.ShowStartupMessage();
  
  if(telescope.Connect() != 0){
    userio.Print("Cannot connect ", "to telescope", 2000);
    Serial.print(F("setup(): Cannot connect to telescope\n"));
  }
  else {
    Serial.print(F("Connected to telescope\n"));
  }
}

void loop()
{
  userio.ServiceButtons();
  userio.ServiceWifi();
  userio.ServiceDeRotator();
  userio.ServiceSetup();
  userio.ServiceOtherCommands();
  
  serialServer.ServiceLoop();
}
