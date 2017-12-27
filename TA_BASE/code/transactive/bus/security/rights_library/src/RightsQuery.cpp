/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/RightsQuery.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Rights query structure.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "RightsLibraryInternal.h"

namespace TA_Base_Bus
{

	RightsQuery::RightsQuery( const std::string& session, unsigned long location,
			unsigned long subsystem, unsigned long action, bool result,
			const std::string& reason, EDecisionModule module ) :
		m_session(session),
		m_location(location),
		m_subsystem(subsystem),
		m_action(action),
		m_result(result),
		m_reason(reason),
		m_module(module)
	{
	}

	RightsQuery::RightsQuery( const std::string& session, unsigned long location,
			unsigned long subsystem, unsigned long action ) :
		m_session(session),
		m_location(location),
		m_subsystem(subsystem),
		m_action(action),
		m_result(false),
		m_module(UNDEFINED_DM)
	{
	}

	RightsQuery::RightsQuery( const std::string& session, unsigned long location,
			unsigned long subsystem ) :
		m_session(session),
		m_location(location),
		m_subsystem(subsystem),
		m_action(0),
		m_result(false),
		m_module(UNDEFINED_DM)
	{
	}

	RightsQuery::RightsQuery( const std::string& session ) :
		m_session(session),
		m_location(0),
		m_subsystem(0),
		m_action(0),
		m_result(false),
		m_module(UNDEFINED_DM)
	{
	}

	RightsQuery::RightsQuery( unsigned long location) :
	m_session(""),
		m_location(location),
		m_subsystem(0),
		m_action(0),
		m_result(false),
		m_module(UNDEFINED_DM)
	{
	}

	bool RightsQuery::operator==( const RightsQuery& rhs ) const
	{
		if( (m_session   == rhs.m_session   ) &&
			(m_location  == rhs.m_location  ) &&
			(m_subsystem == rhs.m_subsystem ) &&
			(m_action    == rhs.m_action    ) )
		{
			return( true );
		}
		return( false );
	}

	bool RightsQuery::operator< ( const RightsQuery& rhs ) const
	{
		if( m_session < rhs.m_session )
		{
			return( true );
		}
		if( m_session == rhs.m_session )
		{
			if( m_location < rhs.m_location )
			{
				return( true );
			}
			if( m_location == rhs.m_location )
			{
				if( m_subsystem < rhs.m_subsystem )
				{
					return( true );
				}
				if( m_subsystem == rhs.m_subsystem )
				{
					if( m_action < rhs.m_action )
					{
						return( true );
					}
				}
			}
		}
		return( false );
	}
}
