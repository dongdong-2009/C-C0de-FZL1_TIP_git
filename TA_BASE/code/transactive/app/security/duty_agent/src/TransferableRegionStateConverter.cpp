/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/TransferableRegionStateConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Converts DutyState to a list of transferable regions suitable for sending to
 * the duty manager.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/TransferableRegionStateConverter.h"

namespace TA_Base_App
{
	TransferableRegionStateConverter::TransferableRegionStateConverter() :
		m_regionSequence(new TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence ),
		m_subsystemSequence( new TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence ),
		m_regIndex(0),
		m_subIndex(0),
		m_subRegIndex(0),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_addCount(0),
		m_remCount(0),
		m_alreadyAdd(false) // gongzong++ CL-21236
	{
		m_regionSequence->length(0);
		m_subsystemSequence->length(0);
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableRegionSequence* TransferableRegionStateConverter::getRegions()
	{
		return( m_regionSequence._retn() );
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::SubsystemTreeSequence* TransferableRegionStateConverter::getSubsystems()
	{
		return( m_subsystemSequence._retn() );
	}

	void TransferableRegionStateConverter::setSession(const std::string& session)
	{
	}


	void TransferableRegionStateConverter::setAction(const std::string& action)
	{
	}

	void TransferableRegionStateConverter::setRegionCount(unsigned long count)
	{
		// there will USUALLY only be one entry per region.
		m_regionSequence->length(count);

		// there will ALWAYS be no more than one entry per region
		m_subsystemSequence->length(count);
	}

	void TransferableRegionStateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{	
		m_region   = region;
		m_addCount = 0;
		m_remCount = 0;

		m_subsystemSequence[m_subRegIndex].region = region;
	}
	
	void TransferableRegionStateConverter::setRegionAddCount( unsigned long count )
	{
        // Not always the case (operator override is the exception)
		//TA_ASSERT( count <= 1, "For a region transfer, the add count must be <= 1" );

		m_addCount = count;
		m_alreadyAdd = false; // gongzong++ CL-21236. init this flag.
	}

	void TransferableRegionStateConverter::setRegionRemoveCount( unsigned long count )
	{
		m_remCount = count;

		if( m_remCount > 1 )
		{
			// we only need to add count-1 because we already have 1 space allocated
			// from setRegionCount
			m_regionSequence->length(m_regionSequence->length()+(m_remCount-1));
		}
	}

	void TransferableRegionStateConverter::setSubsystemCount(unsigned long count)
	{
		m_subsystemSequence[m_subRegIndex].subsystems.length(count);
		m_subIndex = 0;
	}

	void TransferableRegionStateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystemSequence[m_subRegIndex].subsystems[m_subIndex] = subsystem;
		m_subIndex++;
	}


	void TransferableRegionStateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		// if we have an add AND a remove then that is a REPLACE and we just want
		// to record the replaced session, not the one doing the replacing.
		if( (m_addCount > 0) && (m_remCount > 0) )
		{
			return;
		}

		if (m_alreadyAdd)
		{
			return;
		}
		m_regionSequence[m_regIndex].regionKey   = m_region;
		m_regionSequence[m_regIndex].operatorKey = TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;
		m_regionSequence[m_regIndex].profileKey  = TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;

		m_regIndex++;

		m_alreadyAdd = true; // gongzong++ CL-21236
	}


	void TransferableRegionStateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_regionSequence[m_regIndex].regionKey   = m_region;


		TA_Base_Bus::DutyAgentTypeCorbaDef::Key operatorKey;
        try
		{
			operatorKey=TA_Base_Bus::DutySessionCache::instance()->getOperator(session.m_session);
		}
		catch(...)
		{
			operatorKey=TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Invalid session RK:%lu, PK:%lu SS:%s", m_region,  session.m_profile,session.m_session.c_str() );
		}

		m_regionSequence[m_regIndex].operatorKey = operatorKey;
		m_regionSequence[m_regIndex].profileKey  = session.m_profile;
		m_regIndex++;
	}

	void TransferableRegionStateConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void TransferableRegionStateConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void TransferableRegionStateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void TransferableRegionStateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void TransferableRegionStateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void TransferableRegionStateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void TransferableRegionStateConverter::subsystemComplete()
	{
	}

	void TransferableRegionStateConverter::regionComplete()
	{
		m_subRegIndex++;
	}
}



