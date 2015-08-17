/* $Id$ */
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

#ifndef LOGGING_HPP
#define LOGGING_HPP

/**********************************************************************
NAME
	Logging header

SYNOPSIS
	This header consolidates all the required headers that are
	required for using the Boost logger class.

	The user will use:
		LOG_TRACE()  
		LOG_DEBUG()  
		LOG_INFO()   
		LOG_WARNING()
		LOG_ERROR()  
		LOG_FATAL()

	to report the messages into both a log file and stderr.

	The name of the log file is determined by log_init() in
	main.cpp
	
INTERFACE


AUTHOR
	C.Y. Tan

SEE ALSO

**********************************************************************/

#include <fstream>
#include <iomanip>
#include <boost/date_time/posix_time/posix_time_types.hpp>
#include <boost/smart_ptr/shared_ptr.hpp>
#include <boost/smart_ptr/make_shared_object.hpp>
#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/sync_frontend.hpp>
#include <boost/log/sinks/text_ostream_backend.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/utility/setup/console.hpp>


namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace keywords = boost::log::keywords;
 
// helper macro used by the macros below
#define LOG(severity) BOOST_LOG_SEV(logger::get(),boost::log::trivial::severity)
 
// Macros that the user can use to define the type of "error"
#define LOG_TRACE   LOG(trace)
#define LOG_DEBUG   LOG(debug)
#define LOG_INFO    LOG(info)
#define LOG_WARNING LOG(warning)
#define LOG_ERROR   LOG(error)
#define LOG_FATAL   LOG(fatal)

 
namespace logging = boost::log;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace sinks = boost::log::sinks;
namespace attrs = boost::log::attributes;



#endif
