/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/ISessionUpdateObserver.cpp $
  * @author:  Gregg Kirkpatrick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Implements the update() method of ISessionUpdateObserver.
  *
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	ISessionUpdateObserver::ISessionUpdateObserver( SessionUpdateSubject& sessionUpdate ) :
		SeObserver<SessionUpdateSubject>(sessionUpdate)
	{
	}

	void ISessionUpdateObserver::update( SessionUpdateSubject& sessionUpdate )
	{
		sessionUpdate.interpretUpdate(*this);
	}

	void ISessionUpdateObserver::sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session begin ignored (%s)", session.c_str() );
	}

	void ISessionUpdateObserver::sessionEnd( const std::string& session )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session end ignored (%s)", session.c_str() );
	}

	void ISessionUpdateObserver::sessionOverrideBegin( const std::string& session, unsigned long profile )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session override begin ignored (%s)", session.c_str() );
	}

	void ISessionUpdateObserver::sessionOverrideEnd( const std::string& session, unsigned long profile )
	{
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session override end ignored (%s)", session.c_str() );
	}
}
