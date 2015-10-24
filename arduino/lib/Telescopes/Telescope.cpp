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
#include <stdlib.h>
#include <stdio.h>
#include <math.h>

/* general system header files (use "" for make depend) */

/* local include files (use "") */

#include "Telescope.h"

#define DEG2RAD	M_PI/180.0
#define RAD2DEG 180/M_PI

#define OMEGA	4.178e-3*M_PI/180 // rotation frequency of the Earth in rad/s


/**********************************************************************
NAME
        Telescope - class that allows the user to communicate with the
		    LX200.

SYNOPSIS
	See Telescope.hpp

                                                
PROTECTED FUNCTIONS

PRIVATE FUNCTIONS

LOCAL TYPES AND CLASSES

AUTHOR

        C.Y. Tan

SEE ALSO

REVISION
	$Revision$

**********************************************************************/

Telescope::Telescope()
{
  _alt0_rad = 0;
  _az0_rad = 0;

  _start_time = 0;
  _is_debug = true;
}


Telescope::~Telescope()
{
}


int Telescope::Connect()
{
  // enable serial port 2 of arduino mega at 9600 baud
  Serial2.begin(9600);
  while(!Serial2);
  Serial.println(F("Telescope serial port is open"));

  int data[32];

  //See if there is any telescope connected
  send(F("#:GC#")); // get the date stored in the LX200
  _is_debug = receive(data) > 0? false:true;


  if(!_is_debug){

    // toggle LX200 to return high precision alt/az data
    int len = 0;
    do{
      send(F("#:U#")); // toggle precision
    
      // test by getting alt
      send(F("#:GA#")); // get alt
      len = receive(data);
    }while (len <= 7);


    // get current site information
    send(F("#:Gt#"));
    receive(data);
    double sign = data[0] == '-'? -1.0:1.0;
    double d = data[1]*10+data[2];
    double m = data[4]*10+data[5];

    _latitude_rad = sign*(d + m*0.01666666667)*DEG2RAD;        
  }
  else {
    _latitude_rad = CHICAGO_LATITUDE*DEG2RAD;    
  }
  
  return _is_debug? -1:0;
}

int Telescope::Init()
{

  double alt, az;

  if(!_is_debug){
    if(get_altaz(&alt, &az) < 0){
      return -1;
    }
  }
  else {
    // for fast change
    alt = 88.2032;
    az = 300.938;
#ifdef AAAAAA
    // for slow change
    alt = 30.2032;
    az = 300.938;
#endif    
  }
#ifdef AAAAAAA
      Serial.print("alt = ");
      Serial.println(alt, 7);

      Serial.print("az = ");
      Serial.println(az, 7);
#endif  

  _alt0_rad = alt*DEG2RAD;
  _az0_rad = az*DEG2RAD;

  _X0 = cos(_alt0_rad)*cos(_az0_rad);
  _Y0 = -cos(_alt0_rad)*sin(_az0_rad);
  _Z0 = sin(_alt0_rad);

  _start_time = static_cast<double>(millis())*1e-3;

  return 0;
}

int Telescope::GetAltAz(const double time, double* alt, double* az) 
{
  if(!_is_debug){
    if(get_altaz(alt, az) < 0){
      return -1;
    }
  }
  else {
    const double Omegat = OMEGA*(time - _start_time);

    const double X = cos(_latitude_rad)*(_X0*cos(_latitude_rad)+_Z0*sin(_latitude_rad))*(1-cos(Omegat))+
      _X0*cos(Omegat)-_Y0*sin(_latitude_rad)*sin(Omegat);
    const double Y = _Y0*cos(Omegat)+(_X0*sin(_latitude_rad)-_Z0*cos(_latitude_rad))*sin(Omegat);
    const double Z = sin(_latitude_rad)*(_X0*cos(_latitude_rad)+_Z0*sin(_latitude_rad))*(1-cos(Omegat))+
      _Z0*cos(Omegat)+_Y0*cos(_latitude_rad)*sin(Omegat);
  
    *az = atan2(-Y, X)*RAD2DEG;
    *alt = atan2(Z*cos(*az*DEG2RAD), X)*RAD2DEG;

    if(*az < 0)
      *az += 360.0;
  }
  
  return 0;
}

double Telescope::GetLatitude() const
{
  return _latitude_rad*RAD2DEG;
}

int Telescope::send(const __FlashStringHelper* cmd) const
{
#ifdef AAAAAAA
  Serial.print("Sending: ");
  Serial.println(cmd);
#endif
  
  Serial2.println(cmd);
  Serial2.flush();

  return 0;
}

int Telescope::receive(int* const data) const
{

  int* pdata = data;
  int len = 0;
  const long wait_limit = 1000000;
  long wait_i = 0;
#ifdef AAAAAAA
  Serial.print("Received: ");
#endif
  bool is_done = false;
    while(!is_done && wait_i < wait_limit){
    int num_bytes;
    if((num_bytes=Serial2.available()) > 0){
      wait_i = 0; // reset wait
      
      len += num_bytes;
      for(int i=0; i<num_bytes; i++){
	int ch = Serial2.read();
	*pdata++ = ch;
        // write the char
#ifdef AAAAAA	
	Serial.write(ch);
#endif	
	if(ch == '#')
	  is_done = true;
      }
    }
    else {
          wait_i++;
    }
  }
#ifdef AAAAAA    
  Serial.println("");
  Serial.print("wait_i=");
  Serial.println(wait_i, DEC);
#endif  
  return wait_i >= wait_limit? -1: len;

}

int Telescope::get_altaz(double* alt, double* az) const
{
  int data[32];
  
  send(F("#:GA#")); // get alt
  if(receive(data) > 0){
    *alt = convert2alt(data);
  }
  else {
    return -1;
  }

  send(F("#:GZ#")); // get az
  if(receive(data) > 0){
    *az = convert2az(data);
  }
  else {
    return -2;
  }
  
  return 0;
}

double Telescope::convert2alt(const int* deg_min_s) const
{
  double sign = deg_min_s[0] == '-'? -1: 1;
  
  double d = (deg_min_s[1] - '0')*10+(deg_min_s[2] - '0');
  double m = (deg_min_s[4] - '0')*10+(deg_min_s[5] - '0');
  double s = (deg_min_s[7] - '0')*10+(deg_min_s[8] - '0');
  // conversion is  sign*(d + m/60.0 + s/3600.0)
  return sign*(d + m*0.01666666667 + s*0.0002777777778);
}

double Telescope::convert2az(const int* deg_min_s) const
{
  double d = (deg_min_s[0] - '0')*100+(deg_min_s[1] - '0')*10+(deg_min_s[2] - '0');
  double m = (deg_min_s[4] - '0')*10+(deg_min_s[5] - '0');
  double s = (deg_min_s[7] - '0')*10+(deg_min_s[8] - '0');

  // conversion is  (d + m/60.0 + s/3600.0)  
  return (d + m*0.01666666667 + s*0.0002777777778);  
}
