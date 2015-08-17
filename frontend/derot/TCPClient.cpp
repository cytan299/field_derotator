/* $Id$*/
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
/* operating system header files (use <> for make depend) */

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <unistd.h>

#include <iostream>

using namespace std;

/* general system header files (use "" for make depend) */

/* local include files (use "") */

#include "TCPClient.hpp"
#include "logging.hpp"

/* file global variables */

/**********************************************************************
NAME
        TCPClient - wrapper class for socket calls to a tcp server.

SYNOPSIS
	See TCPServer.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/



TCPClient::TCPClient(const char* serverIP, const int portNumber)
  try
{
  struct sockaddr_in   serverAddr;	/* server's socket address */
  int		sockAddrSize;		/* size of socket address structure*/

  _serverIP = new char[strlen(serverIP)+1];
  strcpy(_serverIP, serverIP);

  _portNumber = portNumber;
  
  /* create client's socket TCP*/
  
  if((_sFd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1){
    throw string("TCPClient::TCPClient: Error opening socket");
  }


  /* build server socket address */

  sockAddrSize = sizeof(struct sockaddr_in);

  serverAddr.sin_family = AF_INET;
  serverAddr.sin_addr.s_addr = inet_addr(_serverIP);
  serverAddr.sin_port = htons(_portNumber);

  /* connect to server */

  if(connect(_sFd, (struct sockaddr *)&serverAddr, sockAddrSize) == -1){
    throw string("Cannot connect to server");
    close(_sFd);
  }
}
catch(const string& message)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  LOG_ERROR << message <<"\n";
}

TCPClient::~TCPClient()
{
  close(_sFd);
  delete [] _serverIP;
  _serverIP = NULL;
}


int TCPClient::Send(RequestPacket* request)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  if(write(_sFd, (char*)request, sizeof(RequestPacket)) == -1){
    LOG_ERROR << "TCPClient::Send(): Error sending request packet\n";
    close(_sFd);	
    return -1;	
  }

  return 0;
}

int TCPClient::Receive(ReplyPacket* replyPacket)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  if(read(_sFd, (char*)replyPacket, sizeof(ReplyPacket)) < 0){  
    LOG_ERROR << "TCPClient::Receive(replyPacket): Error reading socket\n";
    close(_sFd);
    return -1;
  }

  return 0;
}


int TCPClient::Receive(StatusPacket* statusPacket)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  if(read(_sFd, (char*)statusPacket, sizeof(StatusPacket)) < 0){  
    LOG_ERROR << "TCPClient::Receive(statusPacket): Error reading socket\n";
    close(_sFd);
    return -1;
  }

  return 0;
}


void TCPClient::Close()
{
  close(_sFd);
  delete [] _serverIP;
  _serverIP = NULL;  
}
