/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/4669_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetSubsystemStateDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2008/10/31 12:12:40 $
 * Last modified by:  $Author: grace.koh $
 *
 * The "getSubsystemState" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetSubsystemStateDAC.h"

namespace TA_Base_App
{
	
	GetSubsystemStateDAC::GetSubsystemStateDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::DASType::SubsystemSet& subsystems,const TA_Base_Bus::DASType::LocationSet& locSet ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_locSet(locSet),
		m_subsystems(subsystems)
	{
	}

	void GetSubsystemStateDAC::execute()
	{
		m_taskManager.getSubsystemState( m_state,m_subsystems, m_locSet );
	}
}
