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

#include "logging.hpp"

using namespace std;

/* general system header files (use "" for make depend) */

/* local include files (use "") */

#include "SerialClient.hpp"

/* file global variables */

SerialClient::SerialClient(const char* devname)
  try
{
  using namespace boost;
  
  if(devname){
    _serial = new TimeoutSerial(string(devname), 115200);
    _serial->setTimeout(posix_time::seconds(5)); // 5 second timeout
  }
  else {
    _serial = NULL;
  }
}
catch(boost::system::system_error& e){

  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  LOG_ERROR << "SerialClient::SerialClient(): cannot open serial port. "
	    << "Error: " << e.what() << "\n";
}
catch(const string& message)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  LOG_ERROR << message <<"\n";
}

SerialClient::~SerialClient()
{
  if(_serial){
    _serial->close();

    delete _serial;
    _serial = NULL;
  }
}

int SerialClient::Connect(const char* devname)
{
 
  if(devname){

    // close the serial port if it is open
    if(_serial){
      _serial->close();
      delete _serial;
    }
    
    using namespace boost;
    
    boost::system::error_code ec;

    try{
      _serial = new TimeoutSerial(string(devname), 115200);
      _serial->setTimeout(posix_time::seconds(5)); // 5 second timeout    
    }
    catch(boost::system::system_error& e){
      using namespace logging::trivial;
      src::severity_logger< severity_level > lg;
      
      LOG_ERROR << "SerialClient::Connect(): cannot open serial port. "
	   << "Error: " << e.what() << "\n";
      return -1;
    }    

    return 0;
  }
  else {
    using namespace logging::trivial;
    src::severity_logger< severity_level > lg;
    
    LOG_ERROR << "SerialClient::Connect(): serial device name is NULL\n";
    return -1;
  }

  return 0;
}


int SerialClient::Send(RequestPacket* request)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;

  if(_serial == NULL){
    LOG_ERROR << "SerialClient::Send(): serial port has not been set. Cannot send request\n";
    return -1;
  }
  
  using namespace boost;

  try{
    // flush whatever is in the serial buffer first
    int sz;
    while((sz = IsGotData()) > 0){
      LOG_ERROR << "<" << sz << ">:"
		<< "flushing ... "
		<< _serial->readString(sz) << "\n";      
    }
    
    _serial->write((char*)request, sizeof(RequestPacket));
  }
  catch(boost::system::system_error& e){
    LOG_ERROR << "SerialClient::Send(): Error sending request packet. "
	      << "Error message: " << e.what() << "\n";    
    _serial->close();    
    return -1;	
  }

  return 0;
}

int SerialClient::Receive(ReplyPacket* replyPacket)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  if(_serial == NULL){
    LOG_ERROR << "SerialClient::Receive(): serial port has not been set. Cannot receive reply\n";
    return -1;
  }
  
  using namespace boost;

  try{
    _serial->read((char*)(replyPacket), sizeof(ReplyPacket));
  }
  catch(boost::system::system_error& e){
    LOG_ERROR << "SerialClient::Receive(): Error reading reply packet. "
	      << "Error message: " << e.what() << "\n";    
    _serial->close();
    return -1;
  }

  return 0;
}


int SerialClient::Receive(StatusPacket* statusPacket)
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
  
  if(_serial == NULL){
    LOG_ERROR << "SerialClient::Receive(): serial port has not been set. Cannot receive reply\n";
    return -1;
  }
  
  using namespace boost;

  try{
    _serial->read((char*)statusPacket, sizeof(StatusPacket));
  }
  catch(boost::system::system_error& e){
    LOG_ERROR << "SerialClient::Receive(): Error reading status packet. "
	      << "Error message: " << e.what() << "\n";
    _serial->close();
    return -1;
  }

  return 0;
}


int SerialClient::ReadString()
{
  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;
    
  if(_serial){

    int sz;
    while((sz = IsGotData()) > 0){
      string s;
      s = _serial->readString(sz);

      LOG_TRACE << "[" << sz << "]:"
		<< s << "\n";

    }

    return 0;
  }
  else {
    LOG_ERROR << "SerialClient::ReadString(): serial port does not exist\n";
    return -1;
  }
}

std::string SerialClient::ReadStringUntil(const std::string& delim)
{
  return _serial->readStringUntil(delim);
}
  

int SerialClient::IsGotData()
{

  boost::system::error_code e;
  
  try{
    return _serial->isGotData(e);
  }
  catch(boost::system::error_code& e){
    using namespace logging::trivial;
    src::severity_logger< severity_level > lg;
    
    LOG_ERROR << "SerialClient::IsGotData(): "
	      << "Error: " << e.message() << "\n";    
    return -1;
  }
}
