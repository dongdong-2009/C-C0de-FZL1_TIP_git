#ifndef __DEBUG_UTIL_H__
#define __DEBUG_UTIL_H__

#ifdef ENABLE_LOG4CXX
	#include "log4cxx/logger.h"
	#include "log4cxx/xml/domconfigurator.h"
	#include "log4Cxx/rollingfileappender.h"
	#include "log4cxx/helpers/pool.h"
	#include "log4cxx/simplelayout.h"
	#include "log4cxx/basicconfigurator.h"
	#include "log4cxx/patternlayout.h"

	using namespace log4cxx;
	using namespace log4cxx::xml;
	using namespace log4cxx::helpers;  
#else
	#include "logger.h"  
	using namespace log4cxx;  
#endif

#pragma once

namespace TA_Base_Core
{
	class DebugUtil
	{
	public:
		// Debug levels are sequentially indexed starting at 0.  It is possible to iterate 
		// through all debug levels by iterating through the integral sequence [ 0, countDebugLevels() ).
		// The current debug level determines the highest debugging level that will be logged.
		enum EDebugLevel
		{
			DebugFatal		= 0L,	// fatal error conditions
			DebugError,		    // error conditions
			DebugWarn,		        // warning conditions
			DebugInfo,		        // informational messages
			DebugDebug,		    // debug-level messages
			DebugTrace,		    // Function Entry/Exit Messages
		};


		~DebugUtil(void);
		static DebugUtil& getInstance();
		static void releaseInstance();

		void logGeneric(const std::string& file, int line, ...);
		/**
		  * setFile
		  * 
		  * Sets the log file to the file name specified.
		  *
		  * @param name Name of log file
          *
          * Pre: file is not an empty string
          */
		void setFile(const char* name);


		/**
		  * getLevel
		  * 
		  * Returns the current debug level
          *
          * @return Current debug level
		  */
		EDebugLevel getLevel() const
        {
            return m_currDebugLevel;
        }

        /**
		  * setLevel
		  * 
		  * Sets the current debug level
          *
          * @param level New debug level
		  */
        void setLevel(EDebugLevel level)
        {
            m_currDebugLevel = level;
        }

        /**
		  * getMaxSize
		  * 
		  * Returns the maximum size of the file in bytes
          *
          * @return Maximum size of file
		  */
        int getMaxSize() const
        {
            return m_maxSizeInBytes;
        }

        /**
		  * setMaxSize
		  * 
		  * Sets the maximum size of the file
          *
          * @param maxSizeInBytes Maximum size of file in bytes
		  */
        void setMaxSize(int maxSizeInBytes)
        {
            m_maxSizeInBytes = maxSizeInBytes;
        }

        /**
		  * setMaxFiles
		  * 
		  * Sets the maximum number of log files
          *
          * @param maxFiles Maximum number of log files
		  */
		void setMaxFiles(int maxFiles)
		{
			m_maxFiles = maxFiles;
		}

	private: // methods
		DebugUtil(void);
		LevelPtr getLog4CxxLevel(EDebugLevel logLevel);
		std::string getFormatedMsg(va_list& args);

	private: // variables
		static DebugUtil* m_pInstance;
		static LoggerPtr m_logger;

		EDebugLevel m_currDebugLevel;
		int m_maxSizeInBytes;
		int m_maxFiles;

	};

}

// Include macros
#include "DebugUtilMacros.h"

#endif //__DEBUG_UTIL_H__ 
