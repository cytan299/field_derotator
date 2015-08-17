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

#ifndef TCPClient_HPP
#define TCPClient_HPP

/**********************************************************************
NAME
	TCPClient - wrapper class for socket calls to a tcp server.


SYNOPSIS
	TCPClient is a wrapper class for socket calls to a tcp server.


CONSTRUCTOR
   	TCPClient(
		 ipAddress   - the ipAddress of the server.
		)

INTERFACE
	Send(			- send the request to the server
		requestPacket
	)

	Receive(
	   replyPacket		- receive the reply packet from the server
	)

AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

#include "RequestPacket.hpp"
#include "ReplyPacket.hpp"
#include "StatusPacket.hpp"

class TCPClient {
public:   
  TCPClient(const char* ipAddress, const int portNumber);
  ~TCPClient();

  int Send(RequestPacket* request);

  int Receive(ReplyPacket* replyPacket);
  int Receive(StatusPacket* statusPacket);  

  void Close();

private:
   char* _serverIP;
   int _portNumber;
   int _sFd;		//socket file descriptor 
};

#endif
