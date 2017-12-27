/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/GetTransferableRegionsDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "getTransferableRegions" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/GetTransferableRegionsDAC.h"

namespace TA_Base_App
{
	GetTransferableRegionsDAC::GetTransferableRegionsDAC( TA_Base_Bus::DutyState& state,
		TA_Base_Bus::DutyPeerTaskManager& taskManager, const TA_Base_Bus::SessionInfo& session,
		const TA_Base_Bus::DASType::LocationSet& locSet, bool checkSubs ) : IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_locSet(locSet),
		m_checkSubs(checkSubs)
	{
	}

	void GetTransferableRegionsDAC::execute()
	{
		TA_Base_Bus::DutySessionCache::instance()->cacheSession( m_session );
		m_taskManager.getTransferableRegions( m_state, m_session, m_locSet, m_checkSubs );
	}
}
