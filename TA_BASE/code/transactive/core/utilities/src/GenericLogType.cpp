/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/GenericLogType.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class implements the ILogType interface and represents
  * the GenericLog debug log type. This log type can take a format
  * string and a variable number of arguments.
  *
  */

#if defined( WIN32 )
#include <afx.h>
#endif // defined ( WIN32 )

#include "GenericLogType.h"
#include "DebugUtil.h"
#include "TAAssert.h"
#include "core/exceptions/src/AssertException.h"


namespace TA_Base_Core
{
	std::string GenericLogType::getCompleteLogMessage
	(
		const std::string& filename, 
		int line, 
		va_list& args
	)
	{
		// Check preconditions

		TA_DEBUG_ASSERT ( !filename.empty() && filename.c_str() != NULL, "File name is empty" );
		TA_DEBUG_ASSERT ( line > 0 || line == -1, "Line <= 0" );

		// First try to retrieve the level, as it is the first argument

		std::string levelStr("");
		try
		{
			// Retrieving level as an int and then casting it to an enum when using it.
			// This is because when this is run under Solaris, enums are converted to 
			// ints when passed through ... (variable argument). If we try to retrieve 
			// level as an enum here, it will throw an exception.

			int level = va_arg ( args, int );
			levelStr = DebugUtil::getStringDebugLevel 
					( ( TA_Base_Core::DebugUtil::EDebugLevel ) level );
		}
		catch ( ... )
		{
		    std::ostringstream str;
		    str << filename << " (" << line << "): DebugLevel was not passed in before format string";
			throw TA_Base_Core::DebugMsgConstructionException(str.str());
		}

		// The second argument should be the format string

		char* format = "";
		try
		{
			format = va_arg ( args, char* );
			std::string test ( format );

			TA_DEBUG_ASSERT ( test.size() != 0, "Empty log message" );
			TA_DEBUG_ASSERT ( test.c_str() != NULL, "Empty log message" );
			TA_DEBUG_ASSERT ( format != "", "Empty log message" );
			TA_DEBUG_ASSERT ( format, "Empty log message" );
		}
		catch ( TA_Base_Core::AssertException& ae )
		{
			// This exception was caught from TA_DEBUG_ASSERT so throw it again, 
			// as these situations should not occur if LOG is used correctly.

			throw ae;
		}
		catch ( ... )
		{
		    std::ostringstream str;
		    str << filename << " (" << line << "): A char * was not passed in as the format string";
			throw TA_Base_Core::DebugMsgConstructionException(str.str());
		}

		// Try to construct the message using the format string and
        // the remaining arguments in the argument list

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

            // TODO LPP: Rather than just discarding the entire message, we should at least 
            // write part of it to the log file.
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
		TA_DEBUG_ASSERT ( message.c_str() != NULL, "Empty log message" );

		// Construct the full log message (append date, time, thread ID,
		// debug level, file name and line number then return the complete
		// log message
        removeEndline(message);
		return constructMessage ( filename, line, levelStr, message );
    }

}		// namespace TA_Base_Core
