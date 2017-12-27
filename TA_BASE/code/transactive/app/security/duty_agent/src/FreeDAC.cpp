/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/FreeDAC.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * The "free" command.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/FreeDAC.h"

namespace TA_Base_App
{
    FreeDAC::FreeDAC( TA_Base_Bus::DutyState& state, TA_Base_Bus::DutyPeerTaskManager& taskManager,
		const TA_Base_Bus::SessionInfo& session,	const TA_Base_Bus::DASType::LocationSet& locSet,
		const TA_Base_Bus::DASType::RegionSet& regSet ) :
		IDutyAgentCommand(state),
		m_state(state),
		m_taskManager(taskManager),
		m_session(session),
		m_locSet(locSet),
		m_regSet(regSet)
	{
	}

	void FreeDAC::execute()
	{
		m_taskManager.sessionEnd(m_state, m_session, m_locSet, m_regSet );

		if( m_regSet.find(TA_Base_Bus::DASConst::ALL_REGIONS) != m_regSet.end() )
		{
			TA_Base_Bus::DutySessionCache::instance()->sessionEnd( m_session );
		}
	}
}
