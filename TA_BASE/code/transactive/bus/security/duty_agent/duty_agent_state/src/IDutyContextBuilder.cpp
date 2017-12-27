/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/IDutyContextBuilder.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Implements IDutyContextBuilder.
  */

#include "DutyAgentState.h"
#include <algorithm>

namespace TA_Base_Bus
{
	
	void IDutyContextBuilder::setRegionEquality(DutyContext& context, const DutyPrimitive& session,
		const DASType::DbKey& regionKey ) const
	{
		DASType::RegionSet regions;
		
		DutyDbCache::instance()->getOperatorRegions(
			DutySessionCache::instance()->getOperator(session.m_session), regions );

		switch(*(regions.begin()))
		{
			case DASConst::ALL_REGIONS:
			{
				context.m_regions = IDutyPolicy::EQ_EQ;
			}
			break;

			case DASConst::NO_REGION:
			{
				context.m_regions = IDutyPolicy::EQ_NEQ;
			}
			break;

			default:
			{
				if( std::find(regions.begin(), regions.end(), regionKey) != regions.end() )
				{
					context.m_regions = IDutyPolicy::EQ_EQ;
				}
				else
				{
					context.m_regions = IDutyPolicy::EQ_NEQ;
				}
			}
			break;
		}
	}

	void IDutyContextBuilder::setSubContext(DutyContext& context, const DutyPrimitive& session,
			const DASType::DbKey& subsystemKey, const SubsystemState::State& preState,
			const SubsystemState::State& postState ) const
	{
		context.m_subsystemKey = subsystemKey;
		context.m_preState     = preState;
		context.m_postState    = postState;

		context.m_dstRights = DutyDbCache::instance()->getRightsType(session.m_profile, subsystemKey, postState);
		context.m_subType   = DutyDbCache::instance()->getSubsystemType(subsystemKey);
	}

	void IDutyContextBuilder::setProfileGroupEquality( DutyContext& context, const DutyPrimitive& session ) const
	{
		if( DASConst::NO_PROFILE_GROUP == context.m_dstPrfGrp )
		{
			context.m_prfGrps = IDutyPolicy::EQ_NA;
		}
		else
		{
			DASType::DbKey srcPrfGrp = DutyDbCache::instance()->getProfileGroup(session.m_profile);
			if( DASConst::NO_PROFILE_GROUP == srcPrfGrp )
			{
				context.m_prfGrps = IDutyPolicy::EQ_NA;
				return;
			}

			if( srcPrfGrp == context.m_dstPrfGrp )
			{
				context.m_prfGrps = IDutyPolicy::EQ_EQ;
			}
			else
			{
				context.m_prfGrps = IDutyPolicy::EQ_NEQ;
			}
		}
	}

	void IDutyContextBuilder::setSrc( DutyContext& context, const DutyPrimitive& session ) const
	{
		if( ULONG_MAX != context.m_subsystemKey )
		{
			context.m_srcDuty   = DutyDbCache::instance()->getRightsType(session.m_profile,	context.m_subsystemKey,
				context.m_preState );
			context.m_srcRights = DutyDbCache::instance()->getRightsType(session.m_profile, context.m_subsystemKey,
				context.m_postState);
		}
	}

	void IDutyContextBuilder::setNullSrc( DutyContext& context ) const
	{
		context.m_prfGrps   = IDutyPolicy::EQ_NA;
		context.m_srcDuty   = IDutyPolicy::RT_NONE;
		context.m_srcRights = IDutyPolicy::RT_NONE;
	}
}
