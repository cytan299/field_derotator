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

#ifndef SERIALSERVER_HPP
#define SERIALSERVER_HPP

#include "Arduino.h"
#include "BaseServer.h"
#include "UserIO.h"
#include "DeRotator.h"

/**********************************************************************
NAME

        SerialServer - This class sets up serial port 0 to listen to
		       user commands


SYNOPSIS

	SerialServer sets up serial port 0 to user commands

CONSTRUCTOR

        SerialServer(		- constructor
	  userio		- userio for printing to LCD.
	  derotator		- the derotator
	)	

        
INTERFACE

	ServiceLoop()		- listen for client data packets

AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

using namespace std;

class SerialServer: public BaseServer
{
public:
  SerialServer(UserIO* userio, DeRotator* derotator);
  ~SerialServer();

public:
  int ServiceLoop();

};
#endif
