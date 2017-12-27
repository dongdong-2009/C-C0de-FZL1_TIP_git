/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetSessionStateDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/10/31 12:12:40 $
 * Last modified by:  $Author: grace.koh $
 *
 * The "getState" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetSessionStateDAC.h"

namespace TA_Base_App
{
	
	GetSessionStateDAC::GetSessionStateDAC( TA_Base_Bus::DutyState& state, const std::string& session,TA_Base_Bus::DutyPeerTaskManager& taskManager,		
		const TA_Base_Bus::DASType::LocationSet& locSet ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_locSet(locSet),
		m_session(session)
	{
	}

	void GetSessionStateDAC::execute()
	{
		m_taskManager.getSessionState( m_state,m_session, m_locSet );
	}
}
