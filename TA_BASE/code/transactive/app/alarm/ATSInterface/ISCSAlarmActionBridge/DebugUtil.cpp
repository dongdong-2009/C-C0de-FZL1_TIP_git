#include "StdAfx.h"
#include "DebugUtil.h"
#include <atlconv.h>
#include <assert.h>
#include <atlstr.h>

const std::string g_strDefaultConfigPath = "C:\\Transactive\\Config\\Log4CxxConfig.xml";
const std::string g_strDefaultLogPath = "C:\\Transactive\\Logs\\ISCSAlarmActionBridge.log";
const std::string g_strRollingAppenderName = "appxRollingAppenderSize";

const unsigned int MAXLOGMESGSIZE = 2000;

namespace TA_Base_Core
{
	DebugUtil* DebugUtil::m_pInstance = NULL;
	LoggerPtr DebugUtil::m_logger(Logger::getLogger( "AppLogger"));

	DebugUtil& DebugUtil::getInstance()
	{
		if (NULL == m_pInstance)
		{
			m_pInstance = new DebugUtil();
		}

		return *m_pInstance;
	}

	void DebugUtil::releaseInstance()
	{
		if (NULL != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}
	}

	DebugUtil::DebugUtil(void) : m_currDebugLevel(DebugInfo)
	{
		USES_CONVERSION;

#ifdef ENABLE_LOG4CXX
		DWORD dwAttr = GetFileAttributes(g_strDefaultConfigPath.c_str());
		if(dwAttr != 0xffffffff)
		{
			DOMConfigurator::configure(g_strDefaultConfigPath);

			LevelPtr logLevel = log4cxx::Logger::getRootLogger()->getLevel();

			if(logLevel == log4cxx::Level::getFatal())
			{
				m_currDebugLevel = DebugFatal;
			}
			else if(logLevel == log4cxx::Level::getError())
			{
				m_currDebugLevel = DebugError;
			}
			else if(logLevel == log4cxx::Level::getWarn())
			{
				m_currDebugLevel = DebugWarn;
			}
			else if(logLevel == log4cxx::Level::getInfo())
			{
				m_currDebugLevel = DebugInfo;
			}
			else if(logLevel == log4cxx::Level::getDebug())
			{
				m_currDebugLevel = DebugDebug;
			}
			else if(logLevel == log4cxx::Level::getTrace())
			{
				m_currDebugLevel = DebugTrace;
			}
		}
		else // we will create an appender manually
		{
			// Create the layout format
			log4cxx::PatternLayoutPtr objLayOut = new log4cxx::PatternLayout();
			objLayOut->setConversionPattern(A2W("%d [Thrd: %t] %-5p %l - %m%n"));

			// Create the rolling file appender
			log4cxx::RollingFileAppenderPtr fileAppender = new log4cxx::RollingFileAppender(log4cxx::LayoutPtr(objLayOut),
				A2W(g_strDefaultLogPath.c_str()/*"myRollingAppenderSize"*/), true);

			fileAppender->setName(A2W(g_strRollingAppenderName.c_str()));

			// Set Maximum file size
			fileAppender->setMaxFileSize(A2W("2000KB"));
			m_maxSizeInBytes = 2000;

			// Set Maximum Backup Index
			fileAppender->setMaxBackupIndex(10);
			m_maxFiles = 10;

			log4cxx::helpers::Pool p;
			fileAppender->activateOptions(p);

			log4cxx::BasicConfigurator::configure(log4cxx::AppenderPtr(fileAppender));

			// Set Log Level
			m_logger->setLevel(log4cxx::Level::getInfo());
			m_currDebugLevel = DebugInfo;
		}
#endif

	}

	DebugUtil::~DebugUtil(void)
	{
	}

	LevelPtr DebugUtil::getLog4CxxLevel(EDebugLevel logLevel)
	{
		LevelPtr plevel = log4cxx::Level::getInfo();

		switch (logLevel)
		{
		case DebugFatal:
			plevel = log4cxx::Level::getFatal();
			break;
		case DebugError:
			plevel = log4cxx::Level::getError();
			break;
		case DebugWarn:
			plevel = log4cxx::Level::getWarn();
			break;
		case DebugInfo:
			plevel = log4cxx::Level::getInfo();
			break;
		case DebugDebug:
			plevel = log4cxx::Level::getDebug();
			break;
		case DebugTrace:
			plevel = log4cxx::Level::getTrace();
			break;
		}
		return plevel;
	}

	std::string DebugUtil::getFormatedMsg(va_list& args)
	{
		char* format = "";
		try
		{
			format = va_arg ( args, char* );
			std::string test ( format );

			assert ( test.size() != 0);
			assert ( test.c_str() != NULL);
			assert ( format != "");
			assert ( format);
		}
		catch ( ... )
		{
			std::ostringstream str;
			str << " A char * was not passed in as the format string";
			throw (str.str());
		}

		// Try to construct the message using the format string and
		// the remaining arguments in the argument list

		std::string message("");
		CString finalCStr("");    
		try
		{
			CString formatCStr(format);
			finalCStr.FormatV(formatCStr, args);

			// TODO LPP: Rather than just discarding the entire message, we should at least 
			// write part of it to the log file.
			if(finalCStr.GetLength() > MAXLOGMESGSIZE)
			{
				std::ostringstream str;
				str << "ERROR: DebugMsgConstructionException: Message length exceeds " << MAXLOGMESGSIZE << " characters: " << format;
				message = str.str();
				throw (str.str());
			}
			else
			{
				message = finalCStr;
			}

		}
	
		catch ( ... )
		{
			std::ostringstream str;
			str << "ERROR: DebugMsgConstructionException: Invalid argument types (was a std::string used?): " << format;
			message = str.str();
			//throw TA_Base_Core::DebugMsgConstructionException(str.str());
		}

		assert ( !message.empty());
		assert ( message.c_str() != NULL);

		return message;
	}

	void DebugUtil::logGeneric(const std::string& file, int line, ...)
	{
		// Get the variable args
		va_list args;
		va_start ( args, line );

		try
		{
			// Ensure that args is pointing to something

			assert( args != NULL);//, "Pointer to variable args is NULL" );

			// Determine the level of the log message. If the debug type is
			// GenericLog, the level SHOULD be the first argument in the 
			// variable argument.
			//
			// We retrieve the level as an int and then cast it to an enum 
			// when using it. This is because when this is run under Solaris, 
			// enums are converted to ints when passed through ... (variable 
			// argument). If we try to retrieve level as an enum here, it will 
			// throw an exception.

			int level;
			level = va_arg ( args, int );

			// Get the equivalent level in LOG4CXX
			LevelPtr pLevel = getLog4CxxLevel((EDebugLevel)level);

			// Get the formated Message
			std::string strMessage = getFormatedMsg(args);

#ifdef ENABLE_LOG4CXX
			// Get the filename & line number
			log4cxx::spi::LocationInfo location(file.c_str(), "", line);

			if ( level <= m_currDebugLevel )
			{
				m_logger->forcedLog(pLevel, strMessage, location);
			}
#endif			
			va_end ( args );
		}
		catch ( ... )
		{
			// Write to stderr that an unknown exception has been caught. It is
			// caught here rather than rethrowing it because it seems inconvenient
			// to do a try/catch everywhere LOG() is called.
			//LOG4CXX_ERROR(m_logger, "DebugUtil::logGeneric(): Caught an unknown exception. Message will not be logged" << std::endl;
			// Reset the arg pointer
			va_end ( args );
		}

	}

	void DebugUtil::setFile(const char* name)
	{
		USES_CONVERSION;
#ifdef ENABLE_LOG4CXX

		log4cxx::helpers::Pool p;  // buffer pool that for activateOptions() to use
		FileAppenderPtr fa=m_logger->getLoggerRepository()->
			getRootLogger()->getAppender(A2W(g_strRollingAppenderName.c_str()));
		if (NULL != fa)
		{
			// ReConfigures the output log file name.
			fa->setFile(A2W(name));
			fa->activateOptions(p);	
		}
#endif	

	}

}


