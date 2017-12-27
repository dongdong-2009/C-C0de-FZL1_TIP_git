/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/TransferRegionsMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of TransferRegionsMO.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	TransferRegionsMO::TransferRegionsMO( DutyPeerTask& task, const ACE_Future<bool>& future,
		DutyState& state, const TA_Base_Bus::SessionInfo& session, const DASType::RegionSet& regions ) : DutyPeerTaskMO(future),
		m_task(task),
		m_state(state),
		m_session(session),
		m_regions(regions)

	{
	}

	void TransferRegionsMO::execute( void )
	{
		m_task.transferImpl( m_state, m_session, m_regions );
	}

} // TA_Base_Bus
