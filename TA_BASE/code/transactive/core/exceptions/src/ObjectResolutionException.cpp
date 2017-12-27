/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/exceptions/src/ObjectResolutionException.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
// UtilitiesException.cpp: implementation of the UtilitiesException class.
//
//////////////////////////////////////////////////////////////////////

#include "ObjectResolutionException.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

namespace TA_Base_Core
{

	ObjectResolutionException::ObjectResolutionException(ObjectResolutionExceptionType eType,
		const std::string& msg) throw()
		: TransactiveException(msg), m_eType(eType)
	{
		// Add the type of exception to the msg
		m_msg = toString();
		m_msg.append( " " + msg );
	}

	
	ObjectResolutionException::ObjectResolutionException( ObjectResolutionExceptionType eType,
		const std::string& agentName,
		const std::string& objectName,
		const std::string& message ) throw()
		: m_eType(eType)
	{
		// Add the type of exception to the msg
		m_msg = "Error while resolving [";
		m_msg.append( agentName );
		m_msg.append( ", " );
		m_msg.append( objectName );
		m_msg.append( " ] - \"");
		m_msg.append( toString() );
		m_msg.append( "\"." );
		if ( !message.empty() )
		{ 
			m_msg.append( " More Info: " );
			m_msg.append( message );
		}
	}

	ObjectResolutionException::~ObjectResolutionException() throw()
	{
	}

	const char* ObjectResolutionException::what() const throw()
	{
		return m_msg.c_str();
	}

	
	std::string ObjectResolutionException::toString() const
	{
		std::string result;
		switch (m_eType)
		{
		case OPERATION_MODE:
			result = "Agent hosting Object is not in Control Mode";
			break;
		case NO_AGENT:
			result = "Agent hosting Object is not available";
			break;
		case NO_OBJECT:
			result = "Agent does not hold Object reference";
			break;
		case RESOLUTION_FAILED:
			result = "Agent contacted and Object found but resolution failed for some reason";
			break;
		case WRONG_INTERFACE:
			result = "Resolved Object does not support the desired interface";
			break;
		case UNKNOWN_ERROR:
			result = "Unknown resolution failure";
			break;
		}
		return result;
	}
}
