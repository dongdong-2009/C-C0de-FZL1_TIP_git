#ifndef ILOGTYPE_H_INCLUDED
#define ILOGTYPE_H_INCLUDED

/**
  * ILogType.h
  *
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/ILogType.h $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is a pure virtual class for the debug log types. Any class
  * representing a log type should inherit from this class.
  *
  */

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif // defined( WIN32 )

#include <string>
#include <stdarg.h>

#include "DebugUtil.h"
#include "core/exceptions/src/DebugMsgConstructionException.h"

const unsigned int MAXLOGMESGSIZE = 2000;

// Class Definition: ILogType

namespace TA_Base_Core
{
	class ILogType
	{
	public:

		/**
		  * ILogType
		  * 
		  * Constructor
		  *
		  * @param type The log type
		  *
		  */
		ILogType(DebugUtil::EDebugType type)
			: m_type(type) {}

		/**
		  * ILogType& operator=
		  * 
		  * Copy Constructor
		  *
		  * @param rhs The ILogType to copy
		  *
		  */
		const ILogType& operator=(const ILogType& rhs)
		{
			this->m_type = rhs.getType();    
			return *this;
		}

		/**
		  * ~ILogType
		  * 
		  * Destructor
		  *
		  */
		virtual ~ILogType() {}

		/**
		  * constructMessage
		  * 
		  * Constructs the final log message to be output. It contains the
		  * date, time, debug level, thread ID, file name, line number
		  * and the message itself
		  *
		  * @return The final log message
		  *
		  * @param filename Name of the file
		  *	@param line		Line number in the file
		  *	@param level	Debug level
		  *	@param message	Log message
		  *
		  * Pre: filename is not empty
		  *		 line is greater than 0
		  *		 level is not empty
		  *		 message is not empty
		  *
		  */
		std::string constructMessage(const std::string& filename, int line, const std::string& level, const std::string& message) const;

		/**
		  * getCompleteLogMessage
		  * 
		  * Abstract method that constructs a string using a list
		  * of arguments retrieved from a variable argument list. This
		  * method does not append the date, time, level or thread ID
		  * to the message
		  *
		  * @return Log message
		  *
		  * @param filename Name of the file
		  * @param line		Line number in the file
		  * @param args		Pointer to a variable argument list
		  *
		  * Pre: filename is not empty
		  *		 line is greater than 0
		  *
		  */
		virtual std::string getCompleteLogMessage(const std::string& filename, int line, va_list& args) = 0;

		/**
		  * getSimpleMessage
		  * 
		  * Constructs the final log message to be output using a list
		  * of arguments retrieved from a variable argument list. It 
		  * does not contains the date, time, debug level, thread ID, 
		  * file name and line number. It just contains the message 
		  * itself
		  *
		  * @return Log Message
		  *
		  * @param args Pointer to a variable argument list
		  *
			  * @exception TA_Base_Core::DebugMsgConstructionException Thrown if
		  *			   message constructed is empty or NULL
		  *
		  */
		std::string getSimpleMessage(va_list& args) const;

		/**
		  * getType
		  * 
		  * Returns the log type
		  *
		  * @return Log Type
		  *
		  */
		DebugUtil::EDebugType getType() const 
		{ 
			return m_type; 
		}

        static void removeEndline(std::string& str);


	private: 

		ILogType(const ILogType& oldILogType);


	private:

		DebugUtil::EDebugType m_type;
	};
}

#endif // ILOGTYPE_H_INCLUDED
