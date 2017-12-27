/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutyContext.h $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Stores contextual information that is used to apply Duty Policy.
  */

#ifndef DUTYCONTEXT_H_INCLUDED
#define DUTYCONTEXT_H_INCLUDED

namespace TA_Base_Bus
{
	class DutyContext
	{
	public:

		friend class IDutyContextBuilder;
		friend class DutyContextBuilderFactoryImpl::RailDutyContextBuilder;
		friend class DutyContextBuilderFactoryImpl::RoadDutyContextBuilder;

		DutyContext();

		// Destination session attributes
		IDutyPolicy::RIGHTS_TYPE m_dstRights;

		// Source session attributes
		IDutyPolicy::RIGHTS_TYPE m_srcDuty;
		IDutyPolicy::RIGHTS_TYPE m_srcRights;

		// Profile attributes
		IDutyPolicy::PROFILE_TYPE m_dstPrfType;
		IDutyPolicy::EQUALITY     m_prfGrps;
		
		// Region attributes
		IDutyPolicy::EQUALITY m_regions;
		
		// Subsystem attributes
		IDutyPolicy::SUBSYSTEM_TYPE m_subType;

        // TD15075
        SubsystemState::State getPreState() const { return m_preState; };
        SubsystemState::State getPostState() const { return m_postState; };

	private:

		DASType::DbKey        m_dstPrfGrp;
		SubsystemState::State m_preState;
		SubsystemState::State m_postState;
		DASType::DbKey        m_subsystemKey;
	};
}

#endif //  DUTYCONTEXT_H_INCLUDED
