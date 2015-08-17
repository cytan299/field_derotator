/*$Id$*/
/*
    derot is the GUI frontend that controls the field derotator
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

#ifndef SERIALCLIENT_HPP
#define SERIALCLIENT_HPP

/**********************************************************************
NAME
	SerialClient - wrapper class for stream calls to a serial port
		       server.


SYNOPSIS

	SerialClient is a wrapper class for stream calls to a serial
	prot server.


CONSTRUCTOR
   	SerialClient(
		 devname   - name of the device name of the serial port.
			     Default: /dev/cu.usbmodem1a1231
		)

INTERFACE
	Connect(
		 devname   - name of the device name of the serial port.
			     Default: /dev/cu.usbmodem1a1231
	)		   - returns 0 on success	

	Send(			- send the request to the server
		requestPacket
	)

	Receive(
	   replyPacket		- receive the reply packet from the server
	)

	Receive(
	   statusPacket		- receive the status packet from the server
	)


	Flush(			- flush the serial port
	   ec			- reference to user defined error code variable
	)			- returns 0 if successful. -1 otherwise and there is an errorcode in ec

AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

#include "RequestPacket.hpp"
#include "ReplyPacket.hpp"
#include "StatusPacket.hpp"

#include "boost/asio.hpp"
#include "TimeoutSerial.h"

class SerialClient {
public:   
  SerialClient(const char* devname = NULL);
  ~SerialClient();

  int Connect(const char* devname = "/dev/cu.usbmodem1a1231");

  int Send(RequestPacket* request);
  
  int Receive(ReplyPacket* replyPacket);
  int Receive(StatusPacket* statusPacket);  

  int ReadString();

  int IsGotData();

private:
  TimeoutSerial* _serial;

};

#endif
