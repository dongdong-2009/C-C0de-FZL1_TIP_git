/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyContext.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Stores contextual information that is used to apply Duty Policy.
  */

#include "DutyAgentState.h"

namespace TA_Base_Bus
{
	DutyContext::DutyContext() : 
		m_dstRights (IDutyPolicy::RT_UNDEFINED),
		m_srcDuty   (IDutyPolicy::RT_UNDEFINED),
		m_srcRights (IDutyPolicy::RT_UNDEFINED),
		m_dstPrfType(IDutyPolicy::PT_UNDEFINED),
		m_prfGrps   (IDutyPolicy::EQ_UNDEFINED),
		m_regions   (IDutyPolicy::EQ_UNDEFINED),
		m_subType   (IDutyPolicy::ST_UNDEFINED),
		m_dstPrfGrp (DASConst::NO_PROFILE_GROUP),
		m_preState  (SubsystemState::SS_NORMAL),
		m_postState (SubsystemState::SS_NORMAL),
		m_subsystemKey(ULONG_MAX)
	{
	}
}
