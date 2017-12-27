/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/PreformattedLogType.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  * This class implements the ILogType interface and represents
  * a preformatted debug log type. This log type has a preformatted
  * format string and level.
  *
  */

#if defined( WIN32 )
#include <afx.h>
#endif // defined ( WIN32 )

#include "DebugUtil.h"
#include "PreformattedLogType.h"
#include "TAAssert.h"

namespace TA_Base_Core
{
	PreformattedLogType::PreformattedLogType
	(
		DebugUtil::EDebugType type, 
		DebugUtil::EDebugLevel level, 
		const std::string& format
	)
		: ILogType(type), m_level(level), m_format(format) 
	{
		TA_DEBUG_ASSERT ( !format.empty() && format.c_str() != NULL, "Empty format string" );

        setRequiredNumberArgs();
	}


	std::string PreformattedLogType::getCompleteLogMessage
	(
		const std::string& filename, 
		int line, 
		va_list& args
	)
	{
		TA_DEBUG_ASSERT ( filename.c_str() != NULL && !filename.empty(), "File name is NULL" );
		TA_DEBUG_ASSERT ( line > 0 || line == -1, "Line <= 0" );

		// Convert level to a string

		std::string levelStr = DebugUtil::getStringDebugLevel ( m_level );

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

            CString formatCStr(m_format.c_str());
            finalCStr.FormatV(formatCStr, args);

            if(finalCStr.GetLength() > MAXLOGMESGSIZE)
            {
		        std::ostringstream str;
		        str << "ERROR: DebugMsgConstructionException: Message length exceeds " << MAXLOGMESGSIZE << " characters: " << m_format;
				message = str.str();
                //throw TA_Base_Core::DebugMsgConstructionException(str.str());
            }
            else
            {
                message = finalCStr;
            }

#else // SOLARIS

            int ret = vsnprintf ( buff, MAXLOGMESGSIZE, m_format.c_str(), args );
			if ( ret > MAXLOGMESGSIZE || ret < 0 )
			{
		        std::ostringstream str;
		        str << "ERROR: DebugMsgConstructionException: Message length exceeds " << MAXLOGMESGSIZE << " characters: " << m_format;
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
		    str << "ERROR: DebugMsgConstructionException: Invalid argument types (was a std::string used?): " << m_format;
			message = str.str();
            //throw TA_Base_Core::DebugMsgConstructionException(str.str());
		}

		TA_DEBUG_ASSERT ( message.c_str() != NULL, "Message is empty");
		TA_DEBUG_ASSERT ( !message.empty(), "Message is empty");

		// Construct the full log message (append date, time, thread ID,
		// debug level, file name and line number then return the complete
		// log message

		std::string constructedMsg = constructMessage ( filename, line, levelStr, message );

		TA_DEBUG_ASSERT ( constructedMsg.c_str() != NULL, "Message is empty");
		TA_DEBUG_ASSERT ( !constructedMsg.empty(), "Message is empty");

		return constructedMsg;
	}


    void PreformattedLogType::setRequiredNumberArgs()
    {
        // Parse the format string for '%' to count how many args
        // are expected to be passed through ...

        int count = 0;
        
        std::string::size_type pos = m_format.find ( "%", 0 );
		while ( std::string::npos != pos )
        {      
            // If there is an escape character before the "%" then the
            // "%" is part of the format string and not an argument
            // placeholder

            char c[2];
            c[0] = m_format.at ( pos - 1 );
            c[1] = '\0';
            std::string temp(c);
            if ( temp.compare ( "\\" ) != 0 )
            {
                ++count;
            }

            pos = m_format.find ( "%", pos + 1 );
        }

        m_requiredNumberArgs = count;
    }


}		// namespace TA_Base_Core
