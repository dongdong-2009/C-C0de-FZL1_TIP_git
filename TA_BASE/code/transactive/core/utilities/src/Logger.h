#ifndef LOGGER_H_INCLUDED
#define LOGGER_H_INCLUDED

/**
  * Logger.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/Logger.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class handles the debug types and the logging. It also stores the 
  * debug types and a cache for lower level messages if the debug level of 
  * the application is set to a high level.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <string>
#include <vector>
#include <deque>
#include <stdarg.h>
#include "DebugUtil.h"

// Forward declaration
		
namespace TA_Base_Core
{
	class ILogType;
}

// Class Definition: Logger

namespace TA_Base_Core
{
	class Logger
	{
	public:

		/**
		  * Logger
		  * 
		  * Constructor
		  *
		  */
		Logger();

		/**
		  * ~Logger
		  * 
		  * Destructor
		  *
		  */
		virtual ~Logger();

		/**
		  * log
		  * 
		  * Constructs the log message with the variable argument list
		  * based on the log type and simpleLog flag and outputs the
		  * complete message to the specified destination
		  *
		  * @return Size of the log message in bytes
		  *
		  * @param type		 Debug type
		  * @param file		 Name of file
		  * @param line		 Line number in file
		  * @param args		 Pointer to a list of variable arguments
		  * @param outfile	 Destination to output log message
		  * @param simpleLog Flag indicating simple log or full log
          *
		  * Pre: file is not empty
		  *		 line is greater than 0
		  */
		int log(DebugUtil::EDebugType type, const std::string& file, int line, va_list args, std::ostream& outfile, bool simpleLog);
		
		/**
		  * logCache
		  * 
		  * Outputs all the cached messages to the output destination
		  * and clears the cache
		  *
		  * @return Number of bytes output
		  *
		  * @param outfile Destination to output log message
		  *
		  */
		int logCache(std::ostream& outfile);

		/**
		  * addToCache
		  * 
		  * Constructs the log message with the variable argument list
		  * based on the log type and simpleLog flag and stored the
		  * complete message in the cache
		  *
		  * @param type		 Debug type
		  * @param file		 Name of file
		  * @param line		 Line number in file
		  * @param args		 Pointer to a list of variable arguments
		  * @param simpleLog Flag indicating simple log or full log
		  *
		  * Pre: file is not empty
		  *		 line is greater than 0
		  */
		void addToCache(DebugUtil::EDebugType type, const std::string& file, int line, va_list args, bool simpleLog);

		/**
		  * clearCache
		  * 
		  * Removes all messages from the cache
		  *
		  */
		void clearCache();

		/**
		  * addLogType
		  * 
		  * Adds a new debug types to the store
		  *
		  * @param logType Debug type object
		  *
		  */
		void addLogType(ILogType& logType);

		/**
		  * getLevel
		  * 
		  * Returns the level of the debug type specified
		  *
		  * @return Debug level
		  *
		  * @param type	Debug type
		  *
		  */
		DebugUtil::EDebugLevel getLevel(DebugUtil::EDebugType type);

        int getRequiredNumArgs(DebugUtil::EDebugType type);
			

		void setMaxCacheSize(unsigned int maxCacheSize)
		{
			m_maxCacheSize = m_maxCacheSize;
		}

	private:	

		/**
		  * constructMessage
		  * 
		  * This is a helper method used by 'log' and 'addToCache'. It 
		  * constructs the log message
		  *
		  * @return Complete log string
		  *
		  * @param type		 Debug type
		  * @param file		 Name of file
		  * @param line		 Line number in file
		  * @param args		 Pointer to a list of variable arguments
		  * @param simpleLog Flag indicating simple log or full log
		  *
		  * Pre: file is not empty
		  *		 line is greater than 0
		  */
		std::string constructMessage(DebugUtil::EDebugType type, const std::string& file, int line, va_list args, bool simpleLog);

		Logger(const Logger& oldLogger);

		const Logger& operator=(const Logger& rhs);    

		
	private:

		std::vector < ILogType* > m_logTypes;	// Store of debug types

		std::deque < std::string > m_msgCache;	// Cache of lower level messages (This
												// is a deque to allow constant time
												// deletion from the front of the cache.)

		unsigned int m_maxCacheSize;			// Maximum number of messages allowed
												// in cache.
	};

}		// namespace TA_Base_Core

#endif // LOGGER_H_INCLUDED
