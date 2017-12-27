/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyPrimitive.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  The smallest unit of duty: session + profile.
  *
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyPrimitive::DutyPrimitive() :
		m_profile(ULONG_MAX)
	{
	}

	DutyPrimitive::DutyPrimitive( const std::string& session, const DASType::DbKey& profile ) :
		m_session(session),
		m_profile(profile)
	{
	}
	
	DutyPrimitive::DutyPrimitive( const DutyPrimitive& copyThis ) :
		m_session(copyThis.m_session),
		m_profile(copyThis.m_profile)
	{
	}
		
	bool DutyPrimitive::operator < ( const DutyPrimitive& rhs ) const
	{
		if( m_session < rhs.m_session )
		{
			return true;
		}

		if( m_session == rhs.m_session )
		{
			if( m_profile < rhs.m_profile )
			{
				return true;
			}
		}

		return false;
	}

	bool DutyPrimitive::operator == ( const DutyPrimitive& rhs ) const
	{
		if( (m_session == rhs.m_session) && (m_profile == rhs.m_profile) )
		{
			return true;
		}

		return false;
	}

} // TA_Base_Bus
