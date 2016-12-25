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

#ifndef DEROTATORCMD_HPP
#define DEROTATORCMD_HPP

#include "TCPClient.hpp"
#include "SerialClient.hpp"

#include "RequestPacket.hpp"
#include "ReplyPacket.hpp"

/**********************************************************************
NAME

        DeRotatorCMD - This class sends a user commands to the
		       derotator  


SYNOPSIS
	DeRotatorCMD allows the user to send commands to the derotator
	either via Wifi or via the serial line. 

CONSTRUCTOR

        DeRotatorCMD(		- constructor
	  mechanical_stepsize	- the mechanical step size of the
				  system in degrees/step.
				  Default: 0.05970731707 deg/step
	)	

        
INTERFACE

	Connect2Wifi(		- connect to the derotator via WIFI
		ipAddress	- to this ip address
	)			- returns 0 on success

	Connect2Serial(		- connect to the derotator via
		   		  Serial line
		devicename	- the serial device name
	)			- returns 0 on success

	SendCommand(		- send the command to the derotator
		rq		- stored in the request packet
		rp		- the reply from the derotator
	)			- returns 0 on success

	SendCommand(		- send the command to the derotator
		rq		- stored in the request packet
	)			- returns 0 on success

	Goto(			- goto this position 
		degrees		- in degrees w.r.t. home
	)			- returns 0 on success

	Goto(			- goto from this position
		d0		- in degrees w.r.t. home
		d1		- to this position in degrees
		  		   w.r.t. home
		time		- taking this time in seconds
	)			- returns 0 on success


        WaitUntil(		- wait until the derotator
		degrees		- reaches at this angle in degrees
		wait_time	- the time to wait before each check
				   in ms. Default: 0.5 s
	)			- returns true when the derotator is
				  at this angle.
				  WARNING: This function is BLOCKING.

AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

using namespace std;

class DeRotatorCMD
{
public:
  DeRotatorCMD(const double mechanical_stepsize = 0.05970731707);
  ~DeRotatorCMD();

public:
  int Connect2Wifi(const char* ipAddress);
  int Connect2Serial(const char* devname);

  int SendCommand(RequestPacket* const rq,
		  ReplyPacket* const rp) const;

  int SendCommand(RequestPacket* const rq) const;

  int Goto(const float degrees) const;
  int Goto(const float d0,
	   const float d1,
	   const float time) const;  
  
  bool WaitUntil(const float degrees,
		 const float wait_time = 0.5) const;

  int SetOmega(const float omega) const;
  

private:
  TCPClient* _tcpClient;
  SerialClient* _serialClient;

private:
  const double _MECHANICAL_STEPSIZE;
};
#endif
