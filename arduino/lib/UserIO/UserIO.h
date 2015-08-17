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

#ifndef USERIO_HPP
#define USERIO_HPP

#include "Wire.h"
#include "Adafruit_MCP23017.h"
#include "Adafruit_RGBLCDShield.h"
#include "menuADAFRUITLCD.h"
#include "keyADAFRUITStream.h"
#include "chainStream.h"


#include "Telescope.h"
#include "DeRotator.h"
#include "TCPServer.h"

#ifndef WIFI_MAX_STR_LEN
	#define WIFI_MAX_STR_LEN	32
#endif

/**********************************************************************
NAME

        UserIO - class that prints out messages to the LCD and reads
		 push buttons


SYNOPSIS
	UserIO is the class prints out messages to the LCD and reads
	the push buttons. 


CONSTRUCTOR

        UserIO(		- constructor
	)	

        
INTERFACE
	Init(			- init to set up the lcd.
				  Must be called in setup()
				  because doing it in the constructor
				  hangs the lcd.
	  derotator		- pointers to the user created derotator,
	  telescope		- telescope
          tcpServer		- and tcpServer objects.
       )

	ShowStartupMessage()	- Print a startup message

	Print(			- Print a message
	  message1,		- to line 1
	  message2		- and line 2 of the LCD
	  delay_ticks		- how long to wait in ms. Default=0
	  is_clear		- clear the LCD before writing.
				  Default: true
	)

	PrintAltAz(		- Print the alt az position
	  alt,			  of the telescope	
	  az
	)

	ServiceButtons()	- See if any of the buttons are pushed
				- up, down, enter buttons are used to service
				- menus. Returns 0 in this case.
				- when left or right buttons are
                                  pushed, the stepper motor is turned
                                  in the anti-clockwise or clockwise
                                  direction respectively. Returns -1
				  in this case.

	ServiceSetup()		- Service the setup menu items.
				- returns 0 on success.

	ServiceDeRotator()	- Service the derotator.
				- returns 0 on success.

	ServiceWifi()		- Service the wifi shield
				- returns 0 on success

	PrintProgressWheel()	- Print a progress wheel on the LCD

	SaveSettings()		- save the userio and tcp server
				  settings into EEPROM memory.

	LoadSavedSettings()	- load the userio settings that was
				  previously saved into the EEPROM
				  back into the derotator.

	LoadDefaultSettings()	- load derotator load limits back
				  into the derotator.

	ForceLCDPrinterMenu(	- Force printing of the 
	  m			- menu 
	  drawExit		- and exit if true
				- to the LCD
	)

	SetInitDeRotatorFlag()	- Callback of "START" in control_menu

	SetStopDeRotatorFlag()	- Callback of "STOP" in control menu

	SetConnect2TCPServerFlag() -

	SetGotoHallHomeFlag()	- Callback of "Goto HALL Home"
				  in control menu

	SetGotoUserHomeFlag()	- Callback of "Goto USER Home"
				  in control menu

	SetUserHomeFlag()	- Callback of "Set User home"
				  in setup menu

	SetMaxCWFlag()		- Callback of "Set Max CW"
				  in setup menu

	SetMaxCCWFlag()		- Callback of "Set Max CCW"
				  in setup menu

	PrintWifiInfo()		- print out the current IP address to LCD.

	GetSSID(		- get the ssid and security setting
		ssid		- of the hardware WLAN
		security
	)

	SetSSID(		- set the ssid of the hardware WLAN
		ssid		- to this SSID. 32 chars max.
	)

	SetPass(		- set the password of the hardware WLAN
		pass		- with this password. 32 chars max.
	)

	SetSecurity(		- set the secruity of the hardware WLAN
		security	- with this security setting
	)

	Possible security settings are:
		WLAN_SEC_UNSEC, WLAN_SEC_WEP, WLAN_SEC_WPA or WLAN_SEC_WPA2
AUTHOR                                          

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

using namespace std;

class TCPServer;

class UserIO
{
public:
  UserIO();
  
  ~UserIO();

public:
  void Init(DeRotator* const derotator,
	    Telescope* const telescope,
	    TCPServer* const tcpServer);
  void ShowStartupMessage();
  
public:  
  void Print(const char* message1,
	     const char* message2 = NULL,
	     const int delay_ticks=0,
	     const bool is_clear = true);
  
  void PrintAltAzRot(const double alt,
		     const double az,
		     const double angle);

  void PrintProgressWheel();

public:
  static void SaveSettings();
  static void LoadSavedSettings();
  static void LoadDefaultSettings();

public:

  void ForceLCDPrintMenu(menu& m, bool drawExit);

public:
  static void Red() {_lcd.setBacklight(0x1);}
  static void Yellow() {_lcd.setBacklight(0x3);}
  static void Green() {_lcd.setBacklight(0x2);}
  static void Teal() {_lcd.setBacklight(0x6);}    
  static void Blue() {_lcd.setBacklight(0x4);}
  static void Violet() {_lcd.setBacklight(0x5);}
  static void White() {_lcd.setBacklight(0x7);}

  static void SetInitDeRotatorFlag();
  static void SetStopDeRotatorFlag();

  static void SetGotoHallHomeFlag();
  static void SetGotoUserHomeFlag();

  static void SetUserHomeFlag();
  static void SetMaxCWFlag();
  static void SetMaxCCWFlag();

  static void PrintWifiInfo();
  
public:  
  int ServiceButtons();
  int ServiceSetup();
  int ServiceDeRotator();
  int ServiceWifi();
  int ServiceOtherCommands();

private:  
  int load_saved_settings();
  
public:
  bool _is_start_derotator;
  bool _is_stop_derotator;

  static bool _is_wifi_selected;
  static bool _is_connected_to_wifi;
  
  static bool _is_clockwise;

  static bool _is_got_user_home;
  static bool _is_got_max_cw;
  static bool _is_got_max_ccw;

  static bool _is_enable_limits;

  static bool _is_goto_hall_home;
  static bool _is_goto_user_home;

  static bool _is_goto_user_angle;

private:
  static Adafruit_RGBLCDShield _lcd;

private:
  // for progress wheel
  static const uint8_t _wheel[];
  int _wheel_i;
  
private:
  // extra LCD characters
  static const uint8_t _eta[];
  static const uint8_t _xi[];  
  static const uint8_t _zeta[];
  static const uint8_t _bs[];

private:
  // for LCD menu
  menuLCD _menulcd;
  keyLook<3> _keybuttons;
  static keyMap _keymap[];

private:
  static DeRotator* _derotator;
  static Telescope* _telescope;
  static TCPServer* _tcpServer;
  static UserIO* _userio; // save ourselves for static callbacks

private:
  struct UserIOMemento {
    UserIOMemento()
    {
      _signature = 0xABCD;
      _home_pos = 0;
      _max_cw = 0;
      _max_ccw = 0;
      _is_clockwise = 1;
      _is_limits_enabled = 0;
      _WLAN_ssid[0] = '\0';
      _WLAN_pass[0] = '\0';
      _WLAN_security = 0;
    };

    uint16_t _signature;
    long _home_pos;
    long _max_cw, _max_ccw;
    uint8_t _is_clockwise;
    uint8_t _is_limits_enabled;
    char _WLAN_ssid[WIFI_MAX_STR_LEN];
    char _WLAN_pass[WIFI_MAX_STR_LEN];
    uint8_t _WLAN_security;
  };

  UserIOMemento _userio_memento;

public:
  void GetSSID(char* ssid, uint8_t* security);
  void SetSSID(const char*ssid);
  void SetPass(const char* pass);
  void SetSecurity(const uint8_t security);  
};
#endif
