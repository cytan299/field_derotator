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
#include <string.h>

#include <Arduino.h>
#include <EEPROM.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* general system header files (use "" for make depend) */

/* local include files (use "") */

#include "UserIO.h"

#define MECHANICAL_STEPSIZE	0.05970731707 // deg/step

/**********************************************************************
	Defines for the LCD
 **********************************************************************/
// These #defines make it easy to set the backlight color
#define RED 0x1
#define YELLOW 0x3
#define GREEN 0x2
#define TEAL 0x6
#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7

// The defines for the user defined LCD characters
#define ETA	0
#define XI	1
#define ZETA	2
#define BS	3

// The maxium angle that the derotator should rotate the camera
#define MAX_ANGLE	60.0 // degrees

/**********************************************************************
NAME
        UserIO - class that prints out messages to the LCD and serial
		 port and reads push buttons

SYNOPSIS
	See UserIO.h

                                                
PRIVATE FUNCTIONS
	load_saved_settings()	- basic retrieval of EEPROM setings
				  without printing to the LCD

LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

/**********************************************************************
	The Wifi router ssid and security settings
 **********************************************************************/
// default
#define WLAN_SSID       "CYTech"        // cannot be longer than 32 characters!
#define WLAN_PASS	""
#define WLAN_SECURITY   WLAN_SEC_UNSEC


// Security can be WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2

/**********************************************************************
	User defined LCD characters
 **********************************************************************/

const uint8_t UserIO::_wheel[] = {0x2F,	// '/'
				  0x2D, // '-'
				  BS, // '\\'
				  0x7C, //'|'
				 };

const uint8_t UserIO::_eta[] = {
  B00000,
  B10110,
  B11001,
  B10001,
  B10001,
  B10001,
  B00001,
  B00001,
};

const uint8_t UserIO::_xi[] = {
  B10000,
  B01111,
  B10000,
  B01111,
  B10000,
  B01110,
  B00001,
  B00110,    
};

const uint8_t UserIO::_zeta[] = {
  B10000,
  B01111,
  B10000,
  B10000,
  B10000,
  B01110,
  B00001,
  B00110,    
};


const uint8_t UserIO::_bs[] = {
  B00000,
  B10000,
  B01000,
  B00100,
  B00010,
  B00001,
  B00000,
  B00000,    
};

/**********************************************************************
	Defines for the LCD menu and submenus
 **********************************************************************/

// LCD buttons. LEFT, RIGHT buttons are taken care of in ServiceButtons()
keyMap UserIO::_keymap[] = 
  {{BUTTON_UP, menu::downCode},  
   {BUTTON_DOWN, menu::upCode},
   {BUTTON_SELECT, menu::enterCode}
  };


bool UserIO::_is_connected_to_wifi = false;
bool UserIO::_is_wifi_selected = false;
TOGGLE(UserIO::_is_wifi_selected, wifi_menu, "Wifi? ",
       VALUE("YES", true),
       VALUE("NO", false)
     );


bool UserIO::_is_clockwise = true;
TOGGLE(UserIO::_is_clockwise, clockwise_menu, "Clockwise? ",
       VALUE("YES", true),
       VALUE("NO", false)
       );

bool UserIO::_is_goto_hall_home = false;
bool UserIO::_is_goto_user_home = false;
MENU(control_menu, "Control Menu",
     OP("START", UserIO::SetInitDeRotatorFlag),     
     OP("STOP", UserIO::SetStopDeRotatorFlag),
     OP("Goto HALL HOME", UserIO::SetGotoHallHomeFlag),
     OP("Goto USER HOME", UserIO::SetGotoUserHomeFlag)
     );

bool UserIO::_is_enable_limits = false;
TOGGLE(UserIO::_is_enable_limits, got_enable_limits, "Enable limits?",
       VALUE("Y", true),
       VALUE("N", false)
       );

bool UserIO::_is_got_user_home = false;
bool UserIO::_is_got_max_cw  = false;
bool UserIO::_is_got_max_ccw = false;

MENU(setup_menu, "Setup Menu",
     OP("Set User home", UserIO::SetUserHomeFlag),
     OP("Set Max CW", UserIO::SetMaxCWFlag),
     OP("Set Max CCW", UserIO::SetMaxCCWFlag),
     SUBMENU(got_enable_limits),
     SUBMENU(clockwise_menu),
     OP("Save settings", UserIO::SaveSettings),
     OP("Load settings", UserIO::LoadSavedSettings),
     OP("Def. settings", UserIO::LoadDefaultSettings)
     );

MENU(main_menu,"Main menu",
     SUBMENU(wifi_menu),
     SUBMENU(control_menu),
     SUBMENU(setup_menu),
     OP("Get Wifi IP ...", UserIO::PrintWifiInfo)     
     );


bool UserIO::_is_goto_user_angle = false;

/**********************************************************************
	Defines for the static variables of UserIO
 **********************************************************************/

Adafruit_RGBLCDShield UserIO::_lcd;
DeRotator* UserIO::_derotator = NULL;
Telescope* UserIO::_telescope = NULL;
TCPServer* UserIO::_tcpServer = NULL;
UserIO*    UserIO::_userio = NULL;

UserIO::UserIO()
  : _menulcd(menuLCD(_lcd, 16, 2)),
    _keybuttons(keyLook<3>(_keymap, &_lcd)), // keyLook<3>

    _is_start_derotator(false),
    _is_stop_derotator(true)
{
  _wheel_i=0;

  _userio = this;
}


UserIO::~UserIO()
{
}

void UserIO::Init(DeRotator* const derotator,
		  Telescope* const telescope,
		  TCPServer* const tcpServer)
{
  _derotator = derotator;
  _telescope = telescope;
  _tcpServer = tcpServer;
  
  // set up the LCD's number of columns and rows:   
  _lcd.begin(16, 2);

  // create the user defined characters
  _lcd.createChar(ETA, const_cast<uint8_t*>(_eta));
  _lcd.createChar(XI, const_cast<uint8_t*>(_xi));
  _lcd.createChar(ZETA, const_cast<uint8_t*>(_zeta));
  _lcd.createChar(BS, const_cast<uint8_t*>(_bs));

  // Load the initial settings from EEPROM
  if(load_saved_settings() != 0){
    _userio->LoadDefaultSettings();
  }
}

void UserIO::ShowStartupMessage()
{
  _lcd.clear();
  _lcd.print("Field De-rotator");
  _lcd.setCursor(0, 1);
  _lcd.print("by C.Y. Tan "); _lcd.print(2015);
  _lcd.setBacklight(RED);
  delay(2000);
}

void UserIO::Print(const char* message1,
		   const char* message2,
		   const int delay_ticks,
		   const bool is_clear)
{
  if(is_clear)
    _lcd.clear();

  if(message1){
    _lcd.home();
    _lcd.print(message1);
  }
  
  if(message2){
    _lcd.setCursor(0,1);
    _lcd.print(message2);
  }
  delay(delay_ticks);
}

void UserIO::PrintAltAzRot(const double alt,
			   const double az,
			   const double angle)
{
  _lcd.home();
  _lcd.write(ETA); _lcd.setCursor(1,0);
  _lcd.print(","); _lcd.setCursor(2,0);
  _lcd.write(XI); _lcd.print("=");
  _lcd.print(alt); _lcd.print(","); _lcd.print(az);      
  _lcd.setCursor(0,1);
  _lcd.print("  "); _lcd.write(ZETA); _lcd.print("=");
  _lcd.print(angle);

  _lcd.setCursor(15,1);
}

int UserIO::ServiceButtons()
{
  // check to see whether the left right buttons have been pushed
  // before polling the main menu
  uint8_t buttons = _lcd.readButtons();

  /*
    Note: left button is defined to rotate the CAMERA
    anti-clockwise. The mechanism of the derotator, unfortunately,
    rotates the camera *clockwise* in the way that I have mounted the
    camera on the de-rotator. Therefore, although I tell the DeRotator
    to rotate CW, the camera rotates counter-clockwise.

    And similarly for the other direction.

   */

  if(buttons & BUTTON_LEFT){
    if(_derotator->Turn(DeRotator::CW) < 0){

      Print("WARNING! Max CCW", "pos reached!");

      return -1;
    }
  }
  
  if(buttons & BUTTON_RIGHT){
    if(_derotator->Turn(DeRotator::CCW) < 0){

      Print("WARNING! Max CW", "pos reached!");

      return -1;
    }      
  }
  main_menu.poll(_menulcd, _keybuttons);
  return 0;
}

int UserIO::ServiceSetup()
{
  
  if(_is_goto_hall_home){
    int err;
    if((err=_derotator->ContinueFindingHallHome()) == 0){
      _is_goto_hall_home = false;

      _derotator->StopFindingHallHome();
      
      Print("Hall home", "found!", 1000);
      ForceLCDPrintMenu(setup_menu, true);       	            
    }
    
    if(err < 0){
      _is_goto_hall_home = false;
      _derotator->StopFindingHallHome();
      
      Print("!!Hall not found", "Press OK to cont", 1000);
      ForceLCDPrintMenu(setup_menu, true);       	      
    }
  }

  if(_is_goto_user_home){
    if(_derotator->ContinueFindingUserHome() == 0){
      _is_goto_user_home = false;
    }
  }

  // Only go into servicing the settings if they have changed. 
  if(_is_got_user_home){
    char buf[32];
    _derotator->SetUserHome();
    sprintf(buf, "%d", _derotator->GetUserHome()*MECHANICAL_STEPSIZE);	
    
    _userio->Print("Setting Home to",
		   buf,
		   1000);
    _userio->ForceLCDPrintMenu(setup_menu, true);
    _is_got_user_home = false;

  }

  if(_is_got_max_cw){
    char buf[32];    
    // NOTE: Derotator direction is OPPOSITE to that of USERIO
    // because the camera is mounted upside down
    // Therefore SetMaxCCW() is here.        
    _derotator->SetMaxCCW();

    sprintf(buf, "%d", _derotator->GetMaxCCW()*MECHANICAL_STEPSIZE);	
    
    _userio->Print("Setting CW to",
		   buf,
		   1000);
    _userio->ForceLCDPrintMenu(setup_menu, true);    
    _is_got_max_cw = false;
  }

  if(_is_got_max_ccw){
    char buf[32];            
    // NOTE: Derotator direction is OPPOSITE to that of USERIO
    // because the camera is mounted upside down.
    // Therefore SetMaxCW() is here.
    _derotator->SetMaxCW();

    sprintf(buf, "%d", _derotator->GetMaxCW()*MECHANICAL_STEPSIZE);	
    
    _userio->Print("Setting CCW to",
		   buf,
		   1000);
    _userio->ForceLCDPrintMenu(setup_menu, true);        
    _is_got_max_ccw = false;
  }

  if(_is_enable_limits != _derotator->IsEnableLimits()){
    _is_enable_limits? _derotator->EnableLimits():_derotator->DisableLimits();
  }

  if(_is_clockwise != _derotator->GetCorrectionDirection()){
    _derotator->SetCorrectionDirection(_is_clockwise);
  }

  return 0;
}

int UserIO::ServiceDeRotator()
{
  if(_is_start_derotator){
    _telescope->Init();

    double alt, az;
    _telescope->GetAltAz(static_cast<double>(millis())*1e-3, &alt, &az);
    PrintAltAzRot(alt, az, 0.0);
  
    if(_derotator->Start(alt, az) < 0){
      Serial.print(F("setup(): Cannot start derotator."));
    }

    _is_start_derotator = false;
  }

  if(_is_stop_derotator == false){

    int status;
    double alt, az;

    if((status=_derotator->Continue()) == 1){

      double angle = _derotator->GetAccumulatedAngle();

      _derotator->GetAltAz(&alt, &az);
      PrintAltAzRot(alt, az, angle);
    
      if(angle >= MAX_ANGLE){
#ifdef AAAAAAA
	Serial.println("*************");
	Serial.println(alt,9);
	Serial.println(az,9);
	Serial.println(angle,9);
	Serial.println("*************");    
	Serial.flush();
#endif
	_derotator->Stop();
	_is_stop_derotator = true;

	ForceLCDPrintMenu(control_menu, true);       	
      }
    }
    else {
      // time step is too small, so we have to break
      if(status == -1){
	double angle = _derotator->GetAccumulatedAngle();

	_derotator->GetAltAz(&alt, &az);
	PrintAltAzRot(alt, az, angle);

	Serial.println("+++++++++++++");
	Serial.println(alt,9);
	Serial.println(az,9);
        Serial.println(angle,9);
	Serial.println("+++++++++++++");    
	Serial.flush();

	_derotator->Stop();
	_is_stop_derotator = true;

      }
      
      if(status == -2){

	Serial.println("!!!!!!!!!!!!!");
	_derotator->GetAltAz(&alt, &az);	
	Serial.println(alt,9);
	Serial.println(az,9);
	double angle = _derotator->GetAccumulatedAngle();	
        Serial.println(angle,9);
	Serial.println("!!!!!!!!!!!!!");    
	Serial.flush();

	_derotator->Stop();
	_is_stop_derotator=true;

	
	Print("Derot. stopped",
	      "at user limits!", 1000);
	
	ForceLCDPrintMenu(control_menu, true);


      }
    }
#ifdef AAAAAAA
    if(status != -100){
      Serial.print("status = ");
      Serial.println(status, DEC);
    }
#endif      
  } // is_stop_derotator == false

  return 0;
}

int UserIO::ServiceWifi()
{
  if((_is_wifi_selected == true) && (_is_connected_to_wifi == false)){
    if(strlen(_userio_memento._WLAN_ssid) > 0){
      Print("Connecting to", _userio_memento._WLAN_ssid);
      if(_tcpServer->Connect(_userio_memento._WLAN_ssid,
			     _userio_memento._WLAN_pass,
			     _userio_memento._WLAN_security) != 0){
	Print(_userio_memento._WLAN_ssid, "Connect FAILED!");
	Serial.print(F("setup(): Cannot start TCP Server\n"));
	_is_connected_to_wifi = false;
	return -1;
      }
    }
    else {
      Print("Connecting to", WLAN_SSID);
      if(_tcpServer->Connect(WLAN_SSID, WLAN_PASS, WLAN_SECURITY) != 0){
	Print(WLAN_SSID, "Connect FAILED!");
	Serial.print(F("setup(): Cannot start TCP Server\n"));
	_is_connected_to_wifi = false;
	return -1;	
      }
    }

    Print("SUCCESS! Got ", WLAN_SSID, 1000);      
      
    _is_connected_to_wifi = true;
    ForceLCDPrintMenu(wifi_menu, true);    
  }

  // user wants to disconnect from wifi
  if((_is_wifi_selected == false) && _is_connected_to_wifi){
    if(strlen(_userio_memento._WLAN_ssid) > 0){
      Print("Disconnecting ", _userio_memento._WLAN_ssid);      
    }
    else {
      Print("Disconnecting ", WLAN_SSID);
    }
    _tcpServer->Disconnect();
    Print("Disconnected!", "", 1000);

    _is_connected_to_wifi = false;
    ForceLCDPrintMenu(wifi_menu, true);        
  }

  if(_is_connected_to_wifi){
    if(_tcpServer->ServiceLoop() > 0){
#ifdef AAAAAAA      
      Serial.println(F("Got packet"));
#endif      
    }
  }
  
  return 0;  
}

int UserIO::ServiceOtherCommands()
{
  if(_is_goto_user_angle){
    if(_derotator->ContinueFindingUserAngle() <= 0){
      _is_goto_user_angle = false;
    }
  }  
  return 0;
}

void UserIO::PrintProgressWheel()
{
  if(_wheel_i >= 4){
    _wheel_i = 0;
  }

  _lcd.setCursor(15, 0);
  _lcd.write(_wheel[_wheel_i]);

  _wheel_i++;
  
}

void UserIO::SaveSettings()
{
  _userio->Print("Saving settings", "", 2000);

  _userio->_userio_memento._signature = 0xABCD;
  _userio->_userio_memento._home_pos = _derotator->GetUserHome();
  // Note: CW and CCW are reversed between UserIO and DeRotator
  _userio->_userio_memento._max_cw = _derotator->GetMaxCCW();
  _userio->_userio_memento._max_ccw = _derotator->GetMaxCW();
  _userio->_userio_memento._is_clockwise = _derotator->GetCorrectionDirection()? 1:0;
  _userio->_userio_memento._is_limits_enabled = _derotator->IsEnableLimits()? 1:0;
  
  _tcpServer->GetSSID(_userio->_userio_memento._WLAN_ssid);
  _tcpServer->GetPass(_userio->_userio_memento._WLAN_pass);
  _userio->_userio_memento._WLAN_security = _tcpServer->GetSecurity();

  int address = 0;
  EEPROM.put(address, _userio->_userio_memento);

  _userio->ForceLCDPrintMenu(setup_menu, true);       	              

}


void UserIO::LoadSavedSettings()
{
  // if there is something to load from EEROM
  if(_userio->load_saved_settings() == 0){
    _userio->Print("Loading settings", "", 2000);
    _userio->ForceLCDPrintMenu(setup_menu, true);
  }
  else {
    // load default otherwise
    _userio->Print("Loading default", "settings", 2000); 
    _userio->LoadDefaultSettings();
    _userio->ForceLCDPrintMenu(setup_menu, true);    
  }
}


void UserIO::LoadDefaultSettings()
{
  _derotator->LoadLimits();

  _tcpServer->SetSSID(WLAN_SSID);
  _tcpServer->SetPass(WLAN_PASS);
  _tcpServer->SetSecurity(WLAN_SECURITY);

  // update the memento
  _userio->_userio_memento._home_pos = 0;
  _userio->_userio_memento._max_cw = static_cast<long>(90.0/MECHANICAL_STEPSIZE);
  _userio->_userio_memento._max_ccw = static_cast<long>(-90/MECHANICAL_STEPSIZE);
  _userio->_userio_memento._is_clockwise = 1;
  _userio->_userio_memento._is_limits_enabled = 0;  

  strcpy(_userio->_userio_memento._WLAN_ssid, WLAN_SSID);
  _userio->_userio_memento._WLAN_ssid[strlen(WLAN_SSID)] = '\0';

  strcpy(_userio->_userio_memento._WLAN_pass, WLAN_PASS);
  _userio->_userio_memento._WLAN_pass[strlen(WLAN_PASS)] = '\0';  
  
  _userio->_userio_memento._WLAN_security = WLAN_SECURITY;
  
}


void UserIO::ForceLCDPrintMenu(menu& m, bool drawExit)
{
  _menulcd.clear();
  _menulcd.force_printMenu(m, drawExit);
}


void UserIO::SetInitDeRotatorFlag()
{
  /*
    I have to do this really stupid if else block because the
    derotator prints out alt,az,angle on the LCD and after START is
    called the derotator menu is obscured. The only way to recover the
    menu is to check that the derotator is working,
    i.e. _is_stop_rotator == false and redraw the LCD. This also stops
    the derotator from rotating which may be what we want anyway.
  */
  if(_userio->_is_stop_derotator == true){
    _userio->_is_start_derotator = true;
    _userio->_is_stop_derotator = false;
  }
  else {
    _userio->ForceLCDPrintMenu(control_menu, true);
    _userio->_is_start_derotator = false;
    _userio->_is_stop_derotator = true;
  }
}

void UserIO::SetStopDeRotatorFlag()
{
  if(_userio->_is_stop_derotator == false){
    _userio->ForceLCDPrintMenu(control_menu, true);    
  }

  _userio->_is_stop_derotator = true;
}

void UserIO::SetGotoHallHomeFlag()
{
  _derotator->StartGoingToHallHome();
  _userio->_is_goto_hall_home = true;
}

void UserIO::SetGotoUserHomeFlag()
{
  _derotator->StartGoingToUserHome();
  _userio->_is_goto_user_home = true;
}

void UserIO::SetUserHomeFlag()
{
  _userio->_is_got_user_home = true;
}

void UserIO::SetMaxCWFlag()
{
  _userio->_is_got_max_cw = true;
}

void UserIO::SetMaxCCWFlag()
{
  _userio->_is_got_max_ccw = true;
}

void UserIO::PrintWifiInfo()
{
  uint32_t ip;

  if(_tcpServer->GetIPAddress(&ip) == 0){
    _lcd.setCursor(1,1);
    _lcd.print((uint8_t)(ip >> 24));
    _lcd.print('.');
    _lcd.print((uint8_t)(ip >> 16));
    _lcd.print('.');
    _lcd.print((uint8_t)(ip >> 8));
    _lcd.print('.');    
    _lcd.print((uint8_t)(ip));
    _lcd.print("   ");
  }
  else {
    _lcd.setCursor(1,1);
    _lcd.print("Wifi not used   ");
  }
}

void UserIO::GetSSID(char* ssid, uint8_t* security)
{
  _tcpServer->GetSSID(ssid);
  *security = _tcpServer->GetSecurity();
}

void UserIO::SetSSID(const char*ssid)
{
  _tcpServer->SetSSID(const_cast<char*>(ssid));

  strcpy(_userio->_userio_memento._WLAN_ssid, ssid);
  _userio->_userio_memento._WLAN_ssid[strlen(ssid)] = '\0';
  
}

void UserIO::SetPass(const char* pass)
{
  _tcpServer->SetPass(const_cast<char*>(pass));

  strcpy(_userio->_userio_memento._WLAN_pass, pass);
  _userio->_userio_memento._WLAN_pass[strlen(pass)] = '\0';
}

void UserIO::SetSecurity(const uint8_t security)
{
  _tcpServer->SetSecurity(security);

  _userio->_userio_memento._WLAN_security = security;
}


int UserIO::load_saved_settings()
{
  int address = 0;
  EEPROM.get(address, _userio->_userio_memento);
  
  if(_userio->_userio_memento._signature == 0xABCD){
    _derotator->SetUserHome(_userio->_userio_memento._home_pos);
    // Note: CW and CCW are reversed between UserIO and DeRotator
    _derotator->SetMaxCCW(_userio->_userio_memento._max_cw);
    _derotator->SetMaxCW(_userio->_userio_memento._max_ccw);
    _derotator->SetCorrectionDirection(_userio->_userio_memento._is_clockwise > 0);
    if(_userio->_userio_memento._is_limits_enabled){
      _derotator->EnableLimits();
    }
    else {
      _derotator->DisableLimits();
    }
  
    _tcpServer->SetSSID(_userio->_userio_memento._WLAN_ssid);
    _tcpServer->SetPass(_userio->_userio_memento._WLAN_pass);
    _tcpServer->SetSecurity(_userio->_userio_memento._WLAN_security);
#ifdef AAAAAAAAA
    Serial.print("ssid = "); Serial.println(_userio->_userio_memento._WLAN_ssid);
    Serial.print("pass = "); Serial.println(_userio->_userio_memento._WLAN_pass);    
    Serial.print("security = "); Serial.println(_userio->_userio_memento._WLAN_security, DEC);
#endif
    return 0;
  }

  return -1;
}
