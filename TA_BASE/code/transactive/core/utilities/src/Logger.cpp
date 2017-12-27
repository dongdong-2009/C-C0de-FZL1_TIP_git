/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/Logger.cpp $
  * @author:  Ripple
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

#include "Logger.h"
#include "DebugUtil.h"
#include "PreformattedLogType.h"
#include "GenericLogType.h"
#include "ILogType.h"
#include "TAAssert.h"
#include "RunParams.h"

#if defined( WIN32 )
#pragma warning( push )
#pragma warning( disable : 4715 ) // Not all control path return a value
#endif							  // defined( WIN32 ) 

namespace TA_Base_Core
{

	Logger::Logger()
		: m_maxCacheSize(100)
	{
		m_logTypes.clear();
		m_msgCache.clear();
	}


	Logger::~Logger()
	{
		clearCache();

		// Delete all the pointers stored in m_logTypes and set pointer to 0
		// to safeguard against any possible future calls to delete.

		std::vector< ILogType* >::iterator itLogType;
		for ( itLogType = m_logTypes.begin(); itLogType != m_logTypes.end(); ++itLogType )
		{
			// If one delete throws an exception, we still want to try to
			// continue deleting the rest of the pointers to prevent
			// memory leaks.

			try
			{
				delete *itLogType;
                *itLogType = NULL;
			}
			catch ( ... )
			{
			}
		}
	}


	int Logger::log
	(
	    DebugUtil::EDebugType type, 
        const std::string& file, 
        int line, 
        va_list args, 
        std::ostream& outfile, 
        bool simpleLog
    )
	{	
		// If it's a simple log, don't care whether file is empty or not, or what the line is.
		// If it's a full log, file must not be empty, file must not be NULL, and line
        // must be greater than 0.

		TA_DEBUG_ASSERT ( simpleLog || ( !simpleLog && !file.empty() ), "File is empty string" );
		TA_DEBUG_ASSERT ( simpleLog || ( !simpleLog && file.c_str() != NULL ), "File is NULL" );
		TA_DEBUG_ASSERT ( simpleLog || ( !simpleLog && (line > 0 || line == -1) ), "Line is <= 0" );

		// Construct the log message

		std::string logMsg = constructMessage ( type, file, line, args, simpleLog );

		// Ensure that an empty log message has not been provided
		
        TA_DEBUG_ASSERT ( logMsg != "", "Log string is empty");

        // Print the log message
		
        outfile << logMsg.c_str();
		outfile.flush();

		// Return the number of characters written
		
        return logMsg.size();
	}


	int Logger::logCache(std::ostream& outfile)
	{
		// No need to assert that the cache is not empty. If it's
		// empty that's fine, we just won't print out anything

		int byteCnt = 0;

		std::deque< std::string >::iterator itMsgCache;
		itMsgCache = m_msgCache.begin();
		while ( itMsgCache != m_msgCache.end() )
		{
			// This should not happen as the string is checked before 
            // it is cached.

			TA_DEBUG_ASSERT ( itMsgCache->c_str() != "", "Cached message is empty" );

			// Output the cached message

			outfile << itMsgCache->c_str();
			outfile.flush();

			byteCnt += itMsgCache->size();

			// Remove the message from the cache

			itMsgCache = m_msgCache.erase ( itMsgCache );
		}

        // Return the number of bytes written

		return byteCnt;
	}


	void Logger::addToCache
    (
        DebugUtil::EDebugType type, 
        const std::string& file, 
        int line, 
        va_list args, 
        bool simpleLog
    )
	{	
		// If it's a simple log, don't care whether file is empty or not, or what the line is.
		// If it's a full log, file must not be empty, file must not be NULL, and line
        // must be greater than 0.
        
		TA_DEBUG_ASSERT ( simpleLog || ( !simpleLog && !file.empty() ), "File is empty string" );
        TA_DEBUG_ASSERT ( simpleLog || ( !simpleLog && file.c_str() != NULL ), "File is NULL" );
		TA_DEBUG_ASSERT ( simpleLog || ( !simpleLog && (line > 0 || line == -1) ), "Line is <= 0" );

		// Construct the log message

		std::string logMsg = constructMessage ( type, file, line, args, simpleLog );

		// Ensure that an empty log string has not been provided

		TA_DEBUG_ASSERT ( logMsg != "", "Log string is empty");

		if ( m_msgCache.size() >= m_maxCacheSize )
		{
			// Reached the maximum size of the cache. Delete the oldest log message.
			m_msgCache.pop_front();
		}

		// Add the message to the cache

		m_msgCache.push_back ( logMsg );
		
		TA_DEBUG_ASSERT ( !m_msgCache.empty(), "Log message was not added to cache" );
	}


	void Logger::clearCache()
	{
		if ( !m_msgCache.empty() )
		{
			m_msgCache.clear();
		}
	}


	void Logger::addLogType(ILogType& logType)
	{
		m_logTypes.push_back ( &logType );
	}


	DebugUtil::EDebugLevel Logger::getLevel(DebugUtil::EDebugType type)
	{
		TA_DEBUG_ASSERT ( !m_logTypes.empty(), "Log types have not been loaded" );

		std::vector< ILogType* >::iterator itLogType;
		for ( itLogType = m_logTypes.begin(); itLogType != m_logTypes.end(); ++itLogType )
		{
			if ( (*itLogType)->getType() == type )
			{
				PreformattedLogType* tmpPL = ( PreformattedLogType* ) ( *itLogType );
				return tmpPL->getDebugLevel();
			}
		}

		// This is an impossible situation

		TA_DEBUG_ASSERT ( false, "DebugType not found" );
	}


    int Logger::getRequiredNumArgs(DebugUtil::EDebugType type)
    {
        TA_DEBUG_ASSERT ( !m_logTypes.empty(), "Log types have not been loaded" );

		std::vector< ILogType* >::iterator itLogType;
		for ( itLogType = m_logTypes.begin(); itLogType != m_logTypes.end(); ++itLogType )
		{
			if ( (*itLogType)->getType() == type )
			{
				PreformattedLogType* tmpPL = ( PreformattedLogType* ) ( *itLogType );
				return tmpPL->getRequiredNumberArgs();
			}
		}

		// This is an impossible situation

		TA_DEBUG_ASSERT ( false, "DebugType not found" );

    }


	std::string Logger::constructMessage
    (
        DebugUtil::EDebugType type, 
        const std::string& file,
        int line, 
        va_list args, 
        bool simpleLog
    )
	{
		TA_DEBUG_ASSERT ( !m_logTypes.empty(), "Log types have not been loaded" );

		std::vector< ILogType* >::iterator itLogType;
		for ( itLogType = m_logTypes.begin(); itLogType != m_logTypes.end(); ++itLogType )
		{
			DebugUtil::EDebugType eType = (*itLogType)->getType();
			if ( eType == type )
			{
				// Construct the log message

				std::string logMsg ( "" );
				if ( simpleLog )
				{
					// 'file' and 'line' are not used here so it
					// doesn't matter what values they are - no need
					// to validate them

					logMsg = (*itLogType)->getSimpleMessage( args );
				}
				else
				{
					// Ensure that 'file' and 'line' are valid

					TA_DEBUG_ASSERT ( file.c_str() != NULL, "File is an empty string" );
					TA_DEBUG_ASSERT ( line != 0, "Line number is 0" );

					logMsg = (*itLogType)->getCompleteLogMessage ( file, line, args );
				}

				return logMsg;
			}
		}

		// This is an impossible situation

		TA_DEBUG_ASSERT ( false, "DebugType not found" );
	}

}       // namespace TA_Base_Core

#if defined( WIN32 )
#pragma warning( pop )
#endif  // defined( WIN32 ) 
