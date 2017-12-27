/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DegradeMO.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implementation of DegradeMO.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DegradeMO::DegradeMO( DutyPeerTask& task, const ACE_Future<bool>& future,
		DutyState& state, const DASType::SubsystemSet& subsystems ) : DutyPeerTaskMO(future),
		m_task(task),
		m_state(state),
		m_subsystems(subsystems)

	{
	}

	void DegradeMO::execute( void )
	{
		m_task.degradeImpl( m_state, m_subsystems );
	}

} // TA_Base_Bus
