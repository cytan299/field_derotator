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
#include <iostream>

/* general system header files (use "" for make depend) */

#include "boost/property_tree/ptree.hpp"
#include "boost/property_tree/ini_parser.hpp"

/* local include files (use "") */
#include "constants.h"
#include "DeRotatorUI.h"
#include "DeRotatorConfig.hpp"

/**********************************************************************
NAME
        DeRotatorConfig - handles reading and writing the derot
		          configuration.

SYNOPSIS
	See DeRotatorConfig.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS


LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

using namespace std;


DeRotatorConfig::DeRotatorConfig(DeRotatorUI* const derotatorUI)
{
  _derotatorUI = derotatorUI;

  _prefs = new Fl_Preferences(Fl_Preferences::USER, "cytech", "derot");

  // create the user data path and preferences
  _prefs->getUserdataPath(_path, FL_PATH_MAX);
}

DeRotatorConfig::~DeRotatorConfig()
{
  delete _prefs;
}

int DeRotatorConfig::Load(const char* filename)
{
  using namespace boost::filesystem;

  // if filename is NULL, load global to derot.prefs
  if(filename){
    if(exists(filename)){
      delete _prefs;
      _prefs = new Fl_Preferences(filename, "cytech", NULL);
      _filename = filename;
    }
  }

  // if the filename exists, load user file
  char buf[256];
  _prefs->get("IPAddress", buf, "192.168.1.37", 255);
  _derotatorUI->IPAddress->value(buf);

  _prefs->get("SerialDevice", buf, "/dev/cu.usbmodem1a1231", 255);
  _derotatorUI->SerialDevice->value(buf);

  int ival;
  _prefs->get("is_correction_clockwise", ival, 1);
  if(ival > 0){
    _derotatorUI->IsClockWise->set();
  }
  else {
    _derotatorUI->IsClockWise->clear();      
  }

  float fval;
  _prefs->get("Display camera angle", fval, 0.0);
  _derotatorUI->derotator_graphics->SetDisplayCameraAngle(fval);

  
  _prefs->get("wlan_ssid", buf, "CYTECH", 255);
  _derotatorUI->WLANSSID->value(buf);

  _prefs->get("wlan_security", ival, 0);
  _derotatorUI->WLANSecurity->value(ival);


  _prefs->get("Earth's tweaked rotation rate", fval, OMEGA);
  sprintf(buf, "%2.7e", fval);
  _derotatorUI->EarthOmegaInput->value(buf);
  sprintf(buf, "%4.4f", fval/OMEGA);
  _derotatorUI->TweakValueInput->value(buf);

  return 0;
}


int DeRotatorConfig::Save(const char* filename)
{
  if(filename){
    delete _prefs;
    _prefs = new Fl_Preferences(filename, "cytech", NULL);
    _filename = filename;
  }
  
  _prefs->set("IPAddress", _derotatorUI->IPAddress->value());
  _prefs->set("SerialDevice", _derotatorUI->SerialDevice->value());
  _prefs->set("is_correction_clockwise", _derotatorUI->IsClockWise->value() !=0?1:0);
  _prefs->set("Display camera angle", _derotatorUI->derotator_graphics->GetDisplayCameraAngle());
  
  _prefs->set("Earth's tweaked rotation rate", _derotatorUI->EarthOmegaInput->value());
  _prefs->flush();

  return 0;
}


bool DeRotatorConfig::IsGotFilename() const
{
  return _filename.size() > 0;
}

char* DeRotatorConfig::GetUserConfigPath()
{
  return _path;
}
  
