/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyResponseSubject.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements DutyResponseSubject.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	void DutyResponseSubjectImpl::update( const std::string& uniqueId, const IDutyAgentRespCorbaDef::EDutyResponse& response )
	{
		FUNCTION_ENTRY("update");
		m_uniqueId = uniqueId;
		m_response = response;

		notify(*this);
		FUNCTION_EXIT;
	}

	void DutyResponseSubjectImpl::getResponse( std::string& uniqueId, IDutyAgentRespCorbaDef::EDutyResponse& response ) const
	{
		FUNCTION_ENTRY("getResponse");
		uniqueId = m_uniqueId;
		response = m_response;
		FUNCTION_EXIT;
	}
}
