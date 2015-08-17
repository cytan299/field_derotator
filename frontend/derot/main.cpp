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

/* general system header files (use "" for make depend) */
#include <config.h>
#include <FL/Fl.H>

#include "boost/filesystem.hpp"
#include "boost/program_options.hpp"

/* local include files (use "") */
#include "TCPClient.hpp"
#include "SerialClient.hpp"
#include "logging.hpp"

#include "DeRotatorUI.h"
#include "MessageSink.hpp"
#include "DeRotatorCMD.hpp"

using namespace std;

/**********************************************************************
NAME

	log_init - initialize logging to write into both log file and
		   stderr
		

		   
SYNOPSIS

	log_init() sets up the boost logger so that messages are
	written into both a log file and stderr.

	The log files are named
		derot_%DATE%_%TIME%.log

	where %DATE% is in the form year-month-day
	      %TIME% is in the form hour:minute:seconds

ARGUMENTS
	log_init(
	  display	- pointer to the Messages text display window
	  message	- pointer to the text buffer of the Messages window
	  message_dir	- directory to write the messages	
	)

AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

void log_init(Fl_Text_Display* const display,
	      Fl_Text_Buffer* const message,
	      const char* message_dir)
{

  using namespace boost;
  using namespace boost::filesystem;

  // check that the directory exists
  path p(message_dir);
  if(!exists(p)){
    // doesn't exist, so create it
    create_directory(p);
  }
  
  string filename;
  posix_time::ptime t = posix_time::second_clock::local_time(); 
  filename =
    string(message_dir)
    + "/"
    + "derot_"
    + to_simple_string(t.date()) + "_"
    + to_simple_string(t.time_of_day())
    + ".log";

  typedef sinks::synchronous_sink< sinks::text_ostream_backend > text_sink;
  boost::shared_ptr< text_sink > sink = boost::make_shared< text_sink >();
  
  sink->locked_backend()->
    add_stream(
	       boost::make_shared< std::ofstream >(filename));
  
  sink->set_formatter(
		      expr::stream
		      << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
		      << ": <" << logging::trivial::severity
		      << "> " << expr::smessage
		      );
  
  sink->locked_backend()->auto_flush(true);
  logging::core::get()->add_sink(sink);
  
  namespace sinks = boost::log::sinks;

  MessageSink::Init(display, message);

  typedef sinks::synchronous_sink<MessageSink> sink_t;
  boost::shared_ptr<sink_t> sink1(new sink_t());
  boost::shared_ptr<boost::log::core> logc=boost::log::core::get();
  logc->add_sink(sink1);

#ifdef AAAAAAAA
  // spit the messages to both log file and stderr
  logging::add_console_log(std::cerr, boost::log::keywords::format = (
     		      expr::stream
		      << expr::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d %H:%M:%S")
		      << ": <" << logging::trivial::severity
		      << "> " << expr::smessage
								      ));
#endif
}

/**********************************************************************
NAME

	process_options - process the command line options

		

		   
SYNOPSIS

	process_options() processes the command line options. The
	options are:

	  -h [ --help ]          this message
	  -s [ --srange ] arg    start and stop in steps (separated by a space)
	  -d [ --drange ] arg    start and stop in degrees (separated by a space)
	  -t [ --time ] arg      time to complete from start to stop
	  -H [ --Home ]          go home
	  -v [ --version ]       print version


ARGUMENTS
	process_options(
		argc		- the number of arguments	
		argv		- the array containing the options
	)			- returns 1 if the commandline options
				  are to be processed.

AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/


int process_options(int argc, char **argv)
{
  bool is_got_range = false;
  bool is_got_time = false;
    
  // for the options
  string ip; // ip address
  string serial; // serial line
  
  int64_t sr[] = {0, 0};
  vector<int64_t> srange(&sr[0], &sr[0]+2); // start, stop in steps

  double dr[] = {0.0, 0.0};
  vector<double> drange(&dr[0], &dr[0]+2); //start, stop in degrees

  int64_t steps = 0;
  double degrees = 0;

  double time; // in seconds

  using boost::lexical_cast;    
  namespace po = boost::program_options;
  // derot command line options
  po::options_description generic("options");
  generic.add_options()
    ("help,h", "this message")
    ("ip,i", po::value<string>(&ip), "ip address of derotator")
    ("Serial,S", po::value<string>(&serial), "serial device to derotator")
    ("srange,s", po::value<vector<int64_t> >(&srange)->multitoken(),
     "start and stop in steps (separated by a space)")
    ("drange,d", po::value<vector<double> >(&drange)->multitoken(),
     "start and stop in degrees (separated by a space)")
    ("gotoD,D", po::value<double>(&degrees), "goto degrees w.r.t. home")
    ("gotoS,G", po::value<int64_t>(&steps), "goto steps w.r.t. home")        
    ("time,t", po::value<double>(&time),
     "time to complete from start to stop in seconds")
    ("version,v", "print version")
    ;

  po::options_description cmdline_options;
  cmdline_options.add(generic);

  po::variables_map vm;    
  po::notify(vm);
  po::store(po::command_line_parser(argc, argv).options(cmdline_options).run(), vm);

  po::notify(vm);

  if(vm.count("help")){
    cerr << "derot version " << PROJECT_VERSION << "\n";
    cerr << "*** To start up the GUI, do not apply any switches ***\n";
    cerr << cmdline_options << "\n";
    return 1;
  }

  if(vm.count("version")){
    cerr << "derot version " << PROJECT_VERSION << "\n";
    return 1;
  }  

  if((ip.size() == 0) && (serial.size() == 0)){
    throw string("process_options(): IP or serial device must be provided");
  }

  DeRotatorCMD dcmd;

  if(vm.count("ip")){
    if(dcmd.Connect2Wifi(ip.c_str()) != 0){
      throw string("process_options(): Connect2Wifi(): failed\n");
    }
  }

  if(vm.count("Serial")){
    if(dcmd.Connect2Serial(serial.c_str()) != 0){
      throw string("process_options(): Connect2Serial(): failed\n");      
    }
  }
  
  if(vm.count("srange")){
    // convert to degrees
    drange[0] = srange[0]*MECHANICAL_STEPSIZE;
    drange[1] = srange[1]*MECHANICAL_STEPSIZE;
    is_got_range = true;
  }


  if(vm.count("drange")){
    is_got_range = true;    
  }

  if(vm.count("gotoD")){
    if(dcmd.Goto(degrees) != 0){
      throw string("process_options(): Goto(): failed\n");      
    }
    return 1;
  }

  if(vm.count("gotoS")){
    if(dcmd.Goto(steps*MECHANICAL_STEPSIZE) != 0){
      throw string("process_options(): Goto(): failed\n");      
    }
    return 1;
  }

  if(vm.count("time")){
    is_got_time = true;    
  }


  if(is_got_range || is_got_time){
    if(dcmd.Goto(drange[0], drange[1], time) !=0){
      throw string("process_options(): Goto(): failed\n");      
    }
    return 1;
  }

  return 0;
}

/**********************************************************************
NAME

	derot - GUI to connect to the field derotator via either wifi
		or serial port.

		   
SYNOPSIS
	derot is the GUI frontend that the user uses to connect to the
	field derotator either via wifi or serial port.

AUTHOR
	C.Y. Tan

REVISION
	$Revision$

SEE ALSO

**********************************************************************/

int main(int argc, char **argv)
{

  using namespace logging::trivial;
  src::severity_logger< severity_level > lg;

  try{

    if(argc > 1){
      if(process_options(argc, argv) == 1){
	return 0;
      }
    }

    DeRotatorUI* derotatorUI = new DeRotatorUI;
    /*
      I have to give DeRotatorGraphics the pointer to the derotatorUI
      because I have to update the steps and deg boxes in the interface
      when the user moves the camera shell outline. Unfortunately,there
      is other way to do this when using the GUI builder Fluid.
    */
    derotatorUI->derotator_graphics->SetDeRotatorUI(derotatorUI);

    // Now initialize the logging system so that error messages are
    // displayed in the Messages window and to a file

    #ifdef __APPLE__
      log_init(derotatorUI->Messages,
	       derotatorUI->_message_buffer,
	       (string(getenv("HOME"))+"/Library/Logs/derot").c_str());
    #else
      log_init(derotatorUI->Messages,
	       derotatorUI->_message_buffer,
	       "/tmp/derot");
    #endif  
    logging::add_common_attributes();

    Fl::visual(FL_DOUBLE | FL_INDEX);
    /*
      show the UI.
      If command line options need to be processed, use
		derotatorUI->show(argc, argv);
      
    */
    derotatorUI->show();

    // write a start up message in the Message window
    string start_message = string("DeRotator by C.Y. Tan\nVersion: ")+string(PROJECT_VERSION)+string("\n");
    MessageSink::Write(start_message);

    Fl::run();  
  }
  catch(string& s){
    LOG_ERROR <<  "error: " << s << "\n";
    return -1;
  }  
  catch(exception& e){
      LOG_ERROR <<  "error: " << e.what() << "\n";
    return -1;
  }
  catch(...){
    LOG_ERROR << "Unknown exception\n";
    return -1;
  }

  return 0;
}
