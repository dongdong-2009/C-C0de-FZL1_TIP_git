#ifndef PREFORMATTEDLOGTYPE_H_INCLUDED
#define PREFORMATTEDLOGTYPE_H_INCLUDED

/**
  * PreformattedLogType.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/PreformattedLogType.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This class implements the ILogType interface and represents
  * a preformatted debug log type. This log type has a preformatted
  * format string and level.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <string>
#include <stdarg.h>
#include "ILogType.h"

// Forward Declaration

namespace TA_Base_Core
{
	class DebugUtil;
}

// Class Definition: PreformattedLogType

namespace TA_Base_Core
{
	class PreformattedLogType : public ILogType
	{
	public:

		/**
		  * PreformattedLogType
		  * 
		  * Constructor
		  *
		  * @param type	  Log type
		  * @param level  Debug level
		  * @param format Format string
		  *
		  */
		PreformattedLogType(DebugUtil::EDebugType type, DebugUtil::EDebugLevel level, const std::string& format);
	
		/**
		  * ~PreformattedLogType
		  * 
		  * Destructor
		  *
		  */
		virtual ~PreformattedLogType() {}

		/**
		  * getCompleteLogMessage
		  * 
		  * Method inherited from ILogType. Constructs a string using 
		  * the preformatted format string and the list of arguments 
		  * retrieved from a variable argument list. 
		  * Call constructMessage to append the date, time, level and
		  * thread ID to the message
		  *
		  * @return Log message
		  *
		  * @param filename Name of file
		  * @param line		Line number in file
		  * @param args		Pointer to list of variable arguments
		  *
			  * @exception TA_Base_Core::DebugMsgConstructionException
		  *			   Thrown if there are problems retrieving arguments from
		  *			   the argument list
		  *
		  */
		virtual std::string getCompleteLogMessage(const std::string& filename, int line, va_list& args);

		/**
		  * getDebugLevel
		  * 
		  * Returns the debug level
		  *
		  * @return Debug level of log type
		  *
		  */
		DebugUtil::EDebugLevel getDebugLevel() const 
		{ 
			return m_level; 
		}

		/**
		  * getFormatString
		  * 
		  * Returns the preformatted format string
		  *
		  * @return Preformatted format string of log type
		  *
		  */
		std::string getFormatString() const 
		{ 
			return m_format; 
		}

        int getRequiredNumberArgs() const
        {
            return m_requiredNumberArgs;
        }


	private:

		PreformattedLogType(const PreformattedLogType& oldPreformattedLogType);

		const PreformattedLogType& operator=(const PreformattedLogType& rhs);   
        
        void setRequiredNumberArgs();


	private: 

		DebugUtil::EDebugLevel m_level;		// Debug level

		std::string m_format;				// Preformatted format string

        int m_requiredNumberArgs;           // Number of args required to pass through ...
		
	};

}		// namespace TA_Base_Core

#endif // PREFORMATTEDLOGTYPE_H_INCLUDED
