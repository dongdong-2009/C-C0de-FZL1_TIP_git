/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/ILogType.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * This is a pure virtual class for the debug log types. Any class
  * representing a log type should inherit from this class.
  *
  */

#include <sys/timeb.h>
#include <time.h>

#if defined ( SOLARIS ) || defined ( LINUX )
#include <pthread.h>    // Enables thread ID to be included in the log
#else
//#include <windows.h>	// To use GetCurrentThreadId()	
#include <afx.h>
#endif					// defined ( SOLARIS ) || defined ( LINUX )

#include "ILogType.h"
#include "DebugUtil.h"
#include "TAAssert.h"
#include "core/exceptions/src/DebugMsgConstructionException.h"
#include "core/exceptions/src/AssertException.h"

	
namespace TA_Base_Core
{
	std::string ILogType::constructMessage 
	(
		const std::string& filename, 
		int line, 
		const std::string& level, 
		const std::string& message
	) const
	{
		TA_DEBUG_ASSERT ( filename.c_str() != NULL && !filename.empty(), "Filename is NULL" );
		TA_DEBUG_ASSERT ( line > 0 || line == -1, "Line number <= 0" );
		TA_DEBUG_ASSERT ( level.c_str() != NULL && !level.empty(), "Level is NULL" );
		TA_DEBUG_ASSERT ( message.c_str() != NULL && !message.empty(), "Log message is empty" );

		// Retrieve Time and Date

		struct timeb timebuffer;
		ftime ( &timebuffer );

		struct tm newtime;

    #if defined ( SOLARIS ) || defined( LINUX )
		newtime = *localtime_r ( &timebuffer.time, &newtime );
	#else
		newtime = *localtime ( &timebuffer.time );
	#endif

		std::string dateTime ( "" );
		if ( ( &newtime ) != NULL )
		{
			char buff1[100] = {0};
			sprintf ( buff1, "%02d/%02d/%02d %02d:%02d:%02d:%03d", newtime.tm_mday, newtime.tm_mon + 1, newtime.tm_year + 1900, 
				newtime.tm_hour, newtime.tm_min, newtime.tm_sec, timebuffer.millitm );
			dateTime = buff1;
		}

		// Retrieve Thread Id
		char buff2[100] = {0};
		std::string threadId ( "" );
	#if defined ( SOLARIS ) || defined ( LINUX )
		sprintf ( buff2, "[Thrd: %d]", pthread_self() );
		threadId = buff2;
	#endif
	#if defined( WIN32 )
		sprintf ( buff2, "[Thrd: %d]", GetCurrentThreadId() ); 
		threadId = buff2;
	#endif

		// Convert line to string

		char buff3[100] = {0};
		sprintf ( buff3, "%d", line );
		std::string lineStr ( buff3 );

		// Construct the log message

		std::string logMesg("\t");    // Indent so that the first line of the log message is obvious when word wrapping is enabled
        logMesg += dateTime;
		logMesg += " [";
		logMesg += level;
		logMesg += "] ";
		logMesg += threadId;
		logMesg += " ";
		logMesg += filename;
		logMesg += ":";
		logMesg +=  lineStr;
		logMesg += ": ";
		logMesg += message;
		logMesg += "\n";

		if ( logMesg.empty() || logMesg.c_str() == NULL )
		{
		    std::ostringstream str;
		    str << filename << " (" << line << "): An (impossible) error occurred while formatting the log message";
			throw TA_Base_Core::DebugMsgConstructionException(str.str());
		}

		return logMesg;
	}


	std::string ILogType::getSimpleMessage ( va_list& args ) const
	{
		// Format string is the first arg

		char* format = "";
		try
		{
			format = va_arg ( args, char* );
			std::string test ( format );

			TA_DEBUG_ASSERT ( test.size() != 0, "Empty log message" );
			TA_DEBUG_ASSERT ( format != "", "Empty log message" );
			TA_DEBUG_ASSERT ( format, "Empty log message" );
			TA_DEBUG_ASSERT ( test.c_str() != NULL, "Empty log message" );
		}
		catch ( TA_Base_Core::AssertException& ae )
		{
			// This exception was caught from TA_DEBUG_ASSERT so throw it again, 
			// as these situations should not occur if LOG is used correctly.

			throw ae;
		}
		catch ( ... )
		{
				throw TA_Base_Core::DebugMsgConstructionException 
				( "A char * was not passed in as the format string" ); 
		}

		// Construct message from format string and args

		std::string message("");

#if defined( WIN32 )

        CString finalCStr("");    

#else // SOLARIS

		char buff[MAXLOGMESGSIZE+1];

#endif

        try
		{

#if defined( WIN32 )

            CString formatCStr(format);
            finalCStr.FormatV(formatCStr, args);

            if(finalCStr.GetLength() > MAXLOGMESGSIZE)
            {
		        std::ostringstream str;
		        str << "ERROR: DebugMsgConstructionException: Message length exceeds " << MAXLOGMESGSIZE << " characters: " << format;
				message = str.str();
                //throw TA_Base_Core::DebugMsgConstructionException(str.str());
            }
            else
            {
                message = finalCStr;
            }

#else // SOLARIS

            int ret = vsnprintf ( buff, MAXLOGMESGSIZE, format, args );
			if ( ret > MAXLOGMESGSIZE || ret < 0 )
			{
		        std::ostringstream str;
		        str << "ERROR: DebugMsgConstructionException: Message length exceeds " << MAXLOGMESGSIZE << " characters: " << format;
                message = str.str();
				//throw TA_Base_Core::DebugMsgConstructionException(str.str());
			}
            else
            {
                message = buff;
            }
#endif
        }
			catch ( TA_Base_Core::DebugMsgConstructionException& )
		{
			// This was the exception thrown above, rethrow...
			throw;
		}
		catch ( ... )
		{
		    std::ostringstream str;
		    str << "ERROR: DebugMsgConstructionException: Invalid argument types (was a std::string used?): " << format;
			message = str.str();
            //throw TA_Base_Core::DebugMsgConstructionException(str.str());
		}

		TA_DEBUG_ASSERT ( !message.empty(), "Empty log message" );
		
		message = message.insert ( 0, "    "); // Preprend indentation
        removeEndline(message);
		message += "\n";
		return message;
	}


    void ILogType::removeEndline(std::string& str)
    {
        size_t cnt = 0;

        for (std::string::reverse_iterator it = str.rbegin();
             (it != str.rend()) && (('\r' == *it) || ('\n' == *it));
             ++it)
        {
            cnt++;
        }

        if (cnt)
        {
            str.resize(str.size() - cnt);
        }
    }
} // namespace TA_Base_Core
