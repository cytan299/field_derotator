/*$Id$*/

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
	$Revision$

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
