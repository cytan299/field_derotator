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

/* operating system header files (use <> for make depend) */
#include <iostream>
#include <iomanip>
#include <stdlib.h>

/* general system header files (use "" for make depend) */

/* local include files (use "") */
#include "constants.h"
#include "DeRotatorCMD.hpp"


/**********************************************************************
	Defines for the mechanical de-rotator
 **********************************************************************/

#define TIME_STEP	0.1 // this is arbitrary. I am setting 100 ms
			    // to be the smallest time interval that
			    // the loop can be processed. So, if the
			    // next time step is less than this, we
			    // won't be in time to make the next
			    // correction.

#define DEG2RAD	M_PI/180.0
#define RAD2DEG 180.0/M_PI

// Frontend angle to hardware angle
#define FA2HA(x)	x
// Frontend angle to hardware steps
#define FA2HS(x)	x*MECHANICAL_STEPSIZE

// Hardware angle to frontend angle
#define HA2FA(x)	x
// Hardware steps to frontend angle
#define HS2FA(x)	x*MECHANICAL_STEPSIZE
// Hardware angle to frontend steps
#define HA2FS(x)	x/MECHANICAL_STEPSIZE



/**********************************************************************
NAME
        DeRotatorCMD - This class sends a user commands to the
		       derotator  


SYNOPSIS
	See DeRotatorCMD.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

#define STEPPER_SPEED  100	// 100 steps/second
				// 200 steps required for 1 turn
				// therefore 0.5 Hz

#define MIN_STEPPER_TIME_US  static_cast<long>(1000000.0/STEPPER_SPEED) 			

DeRotatorCMD::DeRotatorCMD(const double mechanical_stepsize)
:
  _MECHANICAL_STEPSIZE(mechanical_stepsize)
{
  _tcpClient = NULL;
  _serialClient = NULL;
}


DeRotatorCMD::~DeRotatorCMD()
{
  delete _tcpClient;
  delete _serialClient;
}

int DeRotatorCMD::Connect2Wifi(const char* ipAddress)
{
  try{
    _tcpClient = new TCPClient(ipAddress, 5001);
  }
  catch(const string& message)
  {
    cerr << message << "\n";
    return -1;
  }

  return 0;
}

int DeRotatorCMD::Connect2Serial(const char* devicename)
{
  try{
    _serialClient = new SerialClient(devicename);
  }
  catch(const string& message)
  {
    cerr << message << "\n";
    return -1;
  }

  return 0;
}


int DeRotatorCMD::SendCommand(RequestPacket* const rq,
			      ReplyPacket* const rp) const
{
  // send the given command and receive the reply
  // from the derotator
  using namespace std;

  try{
    if(_serialClient){
      if(_serialClient->Send(rq) != 0){
	throw string("Send request failed");
      }
    }

    if(_tcpClient){
      if(_tcpClient->Send(rq) != 0){
        throw string("Send request failed");
      }
    }

    if(_serialClient){
      if(_serialClient->Receive(rp) !=0){
	throw string("Did not receive reply packet");
      }
    }

    if(_tcpClient){
      if(_tcpClient->Receive(rp) !=0){
	throw string("Did not receive reply packet");
      }
    }


    if(rp->_reply != REPLY_OK){
      throw string("Reply is not ok");
    }

    return rp->_reply;
  }
  catch(string& message){
    cerr << "DeRotatorCMD::SendCommand(): "
	 << message
	 << "\n";
    return -1;
  }
}

int DeRotatorCMD::SendCommand(RequestPacket* const rq) const
{
  // send the given command to the derotator
  using namespace std;

  try{
    if(_serialClient){
      if(_serialClient->Send(rq) != 0){
	throw string("Send request failed");
      }
    }

    if(_tcpClient){
      if(_tcpClient->Send(rq) != 0){
	throw string("Send request failed");
      }
    }

    ReplyPacket rp;
    if(_serialClient){
      if(_serialClient->Receive(&rp) !=0){
	throw string("Did not receive reply packet");
      }
    }

    if(_tcpClient){
      if(_tcpClient->Receive(&rp) !=0){
	throw string("Did not receive reply packet");
      }
    }


    if(rp._reply != REPLY_OK){
      throw string("Reply is not ok");
    }

    return rp._reply;
  }
  catch(string& message){
    cerr << "DeRotatorCMD::SendCommand(): "
	 << message
	 << "\n";
    return -1;    
  }
}


int DeRotatorCMD::Goto(const float degrees) const
{
  RequestPacket rq;

  rq._command = CMD_GOTO_THETA;
  rq._fvalue[0] = FA2HA(degrees);

  if(SendCommand(&rq) != REPLY_OK){
    cerr << "DeRotatorCMD::Goto(): SendCommand() error\n";
    return -1;
  }

  if(WaitUntil(degrees)){  // blocking wait until the derotator has reached degrees
    return 0;
  }

  return -1;
}

int DeRotatorCMD::Goto(const float d0,
		       const float d1,
		       const float time) const
{
  RequestPacket rq;
  // first goto the first angle
  rq._command = CMD_GOTO_THETA;
  rq._fvalue[0] = FA2HA(d0);

  if(SendCommand(&rq) != REPLY_OK){
    cerr << "DeRotatorCMD::Goto(): (0) SendCommand() error\n";
    return -1;
  }

  if(WaitUntil(d0)){  // blocking wait until the derotator has reached d0

    // calculate the size of the angle to rotate
    double dd = d1-d0;

    // calculate the number of steps between d0 to d1
    int16_t dsteps = dd/_MECHANICAL_STEPSIZE; // degrees/(degrees/step)
    float dangle = dd/dsteps; // degrees per step

    // calculate the speed to move from d0 to d1 per step
    double dtdstep = time/dsteps; // seconds/step

    if(dtdstep < 0.01){
      cerr << "DeRotatorCMD::Goto(): limiting speed is "
  	   << 0.1 << " s/step. "
	   << "Got = "
	   << dtdstep << " s/step\n";
      return -1;
    }

    for(int i= 0; i<= dsteps; i++){
    
      rq._command = CMD_GOTO_THETA;
      rq._fvalue[0] = FA2HA(dangle*i);
      if(SendCommand(&rq) != REPLY_OK){
        cerr << "DeRotatorCMD::Goto(): (2) SendCommand() error\n";
        return -1;
      };
#ifdef AAAAAAA
      cerr << "a = " << setw(6) << setprecision(3)
	   << setiosflags(ios::left)
	   << dangle*i << "\r";
#endif
      usleep(static_cast<int>(dtdstep*1000000));
    }
    cerr << "\n";
  }
  else {
    return -1;
  }
  
  return 0;
}

bool DeRotatorCMD::WaitUntil(const float degrees, const float wait_time) const
{
  RequestPacket rq;
  ReplyPacket rp;

  rq._command = CMD_GET_THETA;

  while(1){
    if(SendCommand(&rq, &rp) != REPLY_OK){
      throw string("DeRotatorCMD::WaitUntil(): SendCommand() error\n");
    }

    if(fabs(HA2FA(rp._fvalue[0]) - degrees) <= _MECHANICAL_STEPSIZE){
      return true;
    }
  
    usleep(wait_time*1000000); // sleep
  }

  return true;
}

   
int DeRotatorCMD::SetOmega(const float omega) const
{
  // first check that omega is reasonable, or else return error
  if((omega > 0.9*OMEGA) && (omega < 1.1*OMEGA)){
    RequestPacket rq;

    rq._command = CMD_SET_OMEGA_VALUE;
    rq._fvalue[0] = omega;
  
    if(SendCommand(&rq) != REPLY_OK){
      cerr << "DeRotatorCMD::SetOmega(): SendCommand() error\n";
      return -1;
    }
    return 0;
  }
  else {
    cerr << "DeRotatorCMD::SetOmega(): Unreasonable omega value: "
	 << omega << " rad/s\n";
    cerr << "\t Must be within 10% of " << OMEGA << " rad/s\n";
    return -1;
  }

  return 0;
}
