/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetTransferableSubsystemsDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "getTransferableSubsystems" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetTransferableSubsystemsDAC.h"

namespace TA_Base_App
{
	
	GetTransferableSubsystemsDAC::GetTransferableSubsystemsDAC( TA_Base_Bus::DutyState& state,
		TA_Base_Bus::DutyPeerTaskManager& taskManager, const TA_Base_Bus::SessionInfo& session,
		const TA_Base_Bus::DASType::RegionSet& regSet ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_regSet(regSet)
	{
	}

	void GetTransferableSubsystemsDAC::execute()
	{
		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: Before call to m_taskManager.getTransferableSubsystems(): region size:%lu", 
			m_regSet.size() );
		//++TD15346
		TA_Base_Bus::DutySessionCache::instance()->cacheSession( m_session );
		m_taskManager.getTransferableSubsystems( m_state, m_session, m_regSet );

		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: After call to m_taskManager.getTransferableSubsystems(): region size:%lu", 
			m_regSet.size() );
		//++TD15346
	}
}
