/*$Id: BaseServer.h | Sat Aug 1 23:56:42 2015 -0500 | cytan  $*/
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

#ifndef BASESERVER_HPP
#define BASESERVER_HPP

#include "RequestPacket.hpp"
#include "ReplyPacket.hpp"
#include "StatusPacket.hpp"

#include "DeRotator.h"

/**********************************************************************
NAME

        BaseServer - Base class for both TCPServer and SerialServer so
		     that common functions used by both these classes can
		     be consolidated.

SYNOPSIS

	BaseServer serves as the base class for both TCPServer and
	SerialServer so that common functions can be consolidated
	here. The most important common function is ServiceRequests()
	which both derived classes have to execute identically.
	

CONSTRUCTOR

        BaseServer(			- constructor
	  userio		- userio for printing to LCD.
	  derotator		- the derotator
	)	

        
INTERFACE
	Connect(		- dummy connect function
	)			- returns 0 on success

	Disconnect()		- dummy disconnect function
				  Returns 0 on success

	ServiceRequests(	- service the requests given in
	  rq			- the request packet
	  rp			- return the results in either the
	  sp			  ReplyPacket rp or StatusPacket sp
	)			- returns 0 on success	


AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision: ed818538e6e43bcc3dddbdebbd202b9f59b809f5 $

**********************************************************************/

using namespace std;

class UserIO;

class BaseServer
{
public:
  BaseServer(UserIO* userio, DeRotator* derotator);
  ~BaseServer();

public:
  int Connect();
  int Disconnect();

  int ServiceRequests(RequestPacket* const rq,
		      ReplyPacket* const rp,
		      StatusPacket* const sp);
protected:
  UserIO* _userio;
  DeRotator* _derotator;
};
#endif
