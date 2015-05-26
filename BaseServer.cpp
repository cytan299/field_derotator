/*$Id$*/
/* operating system header files (use <> for make depend) */
#include <Adafruit_CC3000.h>
#include <ccspi.h>
#include <SPI.h>
#include <Arduino.h>
#include <stdlib.h>
#include <math.h>
#include "utility/debug.h"

/* general system header files (use "" for make depend) */

/* local include files (use "") */
#include "BaseServer.h"
#include "UserIO.h"

/**********************************************************************
NAME

        BaseServer - Base class for both TCPServer and SerialServer so
		 that common functions used by both these classes can
		 be consolidated.

SYNOPSIS
	See BaseServer.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS

LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/


BaseServer::BaseServer(UserIO* userio, DeRotator* derotator)
{
  _userio = userio;
  _derotator = derotator;
}

BaseServer::~BaseServer()
{
}

int BaseServer::Connect()
{
  
  return 0;
}

int BaseServer::Disconnect()
{
  return 0;
}

int BaseServer::ServiceRequests(RequestPacket* const rq,
				ReplyPacket* const rp,
				StatusPacket* const sp)
{
  extern menu main_menu;
  extern menu control_menu;
  extern menu setup_menu;
  

  // do derotator control commands
  if((rq->_command >= 10) && (rq->_command <20)){ 
    main_menu.activeNode=&control_menu;
    switch(rq->_command){
      case DEROTATOR_START:
	_userio->SetInitDeRotatorFlag();
      break;
      case DEROTATOR_STOP:
	_userio->SetStopDeRotatorFlag();
	_derotator->Stop();
      break;
      case DEROTATOR_GOTO_HALL_HOME:
	_userio->SetGotoHallHomeFlag();
      break;
      case DEROTATOR_GOTO_USER_HOME:
	_userio->SetGotoUserHomeFlag();
      break;	    
    }
  }

  // do setup  commands
  if((rq->_command >= 20) && (rq->_command <30)){ 
    main_menu.activeNode=&setup_menu;
    switch(rq->_command){
      case SETUP_SET_USER_HOME:
	_userio->_is_got_user_home = !(_userio->_is_got_user_home);
	_userio->ForceLCDPrintMenu(setup_menu, true);	    
      break;
      case SETUP_MAX_CW:
	_userio->_is_got_max_cw = !(_userio->_is_got_max_cw);
	_userio->ForceLCDPrintMenu(setup_menu, true);	    
      break;
      case SETUP_MAX_CCW:
	_userio->_is_got_max_ccw = !(_userio->_is_got_max_ccw);
	_userio->ForceLCDPrintMenu(setup_menu, true);	    
       break;
       case SETUP_IS_CLOCKWISE:
	 _userio->_is_clockwise = rq->_ivalue != 0; // is clockwise if a nonzero value is found
	 _userio->ForceLCDPrintMenu(setup_menu, true);
       break;
       case SETUP_SAVE_SETTINGS:
	 _userio->SaveSettings();	    
       break;
       case SETUP_LOAD_SETTINGS:
	 _userio->LoadSavedSettings();
       break;
       case SETUP_DEF_SETTINGS:
       _userio->LoadDefaultSettings();
       break;
    }
  }

  // do other commands
  if(rq->_command >= 100){
    switch(rq->_command){
      case CMD_GET_ALTAZ_ZETA:
	double alt, az;
	_derotator->GetAltAz(&alt, &az);
	rp->_fvalue[0] = alt;
	rp->_fvalue[1] = az;
	rp->_fvalue[2] = _derotator->GetAccumulatedAngle();
	rp->_fvalue[3] = _derotator->GetAngle();
      break;
      case CMD_GET_THETA:
	double theta;
	theta = _derotator->GetAngle();
	rp->_fvalue[0] = theta;
      break;
      case CMD_GOTO_THETA:
	theta = rq->_fvalue[0];
	_derotator->StartGoingToUserAngle(theta);
	_userio->_is_goto_user_angle = true;
      break;
      case CMD_QUERY_STATE:
	sp->_reply = REPLY_OK;
	sp->_is_clockwise_correction = _derotator->GetCorrectionDirection()? 1:0;

	sp->_home_pos = _derotator->GetUserHome();
	sp->_max_cw = _derotator->GetMaxCW();
	sp->_max_ccw = _derotator->GetMaxCCW();
	sp->_angle = _derotator->GetAngle();
	sp->_accumulated_angle = _derotator->GetAccumulatedAngle();

      break;
    }
  }
  return 0;  
}
