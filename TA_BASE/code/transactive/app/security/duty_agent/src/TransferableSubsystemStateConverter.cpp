/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/TransferableSubsystemStateConverter.cpp $
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
#include "app/security/duty_agent/src/TransferableSubsystemStateConverter.h"

namespace TA_Base_App
{
	TransferableSubsystemStateConverter::TransferableSubsystemStateConverter() :
		m_subsystemSequence(new TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableSubsystemSequence ),
		m_index(0),
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_addCount(0),
		m_remCount(0),
		m_added(false)
	{
			m_subsystemSequence->length(0);
	}

	TA_Base_Bus::DutyAgentTypeCorbaDef::TransferableSubsystemSequence* TransferableSubsystemStateConverter::getResult()
	{
		return( m_subsystemSequence._retn() );
	}

	void TransferableSubsystemStateConverter::setSession(const std::string& session)
	{
	}


	void TransferableSubsystemStateConverter::setAction(const std::string& action)
	{
	}

	void TransferableSubsystemStateConverter::setRegionCount(unsigned long count)
	{
	}

	void TransferableSubsystemStateConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}
	
	void TransferableSubsystemStateConverter::setRegionAddCount( unsigned long count )
	{
	}

	void TransferableSubsystemStateConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void TransferableSubsystemStateConverter::setSubsystemCount(unsigned long count)
	{
		m_subsystemSequence->length(m_subsystemSequence->length() + count);
	}

	void TransferableSubsystemStateConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem = subsystem;
		m_addCount = 0;
		m_remCount = 0;
	}


	void TransferableSubsystemStateConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void TransferableSubsystemStateConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void TransferableSubsystemStateConverter::setSubsystemAddCount( unsigned long count )
	{
		// Not always the case (operator override is the exception)
		//TA_ASSERT( count <= 1, "For a subsystem transfer, the add count must be <= 1" );

		m_addCount = count;
		m_added = false;
	}

	void TransferableSubsystemStateConverter::setSubsystemRemoveCount( unsigned long count )
	{
		m_remCount = count;

		if( m_remCount > 1 )
		{
			m_subsystemSequence->length(m_subsystemSequence->length()+(m_remCount-1));
		}
	}

	void TransferableSubsystemStateConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_remCount > 0 )
		{
			return;
		}

		if( !m_added )
		{
			m_subsystemSequence[m_index].regionKey    = m_region;
			m_subsystemSequence[m_index].subsystemKey = m_subsystem;
			m_subsystemSequence[m_index].operatorKey  = TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;
			m_subsystemSequence[m_index].profileKey   = TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;

			//TD15346++
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"TD15346: (COUNT ME) subsystemAdd(): RK:%lu, SK:%lu, OK:%lu, PK:%lu", 
				m_region, m_subsystem, TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY, TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY );
			std::string temp = "TD15346: session: ";
			temp += session.m_session;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
			//++TD15346

			m_added = true;
			m_index++;
		}		
	}


	void TransferableSubsystemStateConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_subsystemSequence[m_index].regionKey    = m_region;
		m_subsystemSequence[m_index].subsystemKey = m_subsystem;
         
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key operatorKey;
        try
		{
			operatorKey=TA_Base_Bus::DutySessionCache::instance()->getOperator(session.m_session);
		}
		catch(...)
		{
			operatorKey=TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Invalid session RK:%lu, SK:%lu, PK:%lu SS:%s ", m_region, m_subsystem,  session.m_profile,session.m_session.c_str());
		}
		m_subsystemSequence[m_index].operatorKey  = operatorKey;
		m_subsystemSequence[m_index].profileKey   = session.m_profile;

		//TD15346++
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"TD15346: (COUNT ME) subsystemRemove(): RK:%lu, SK:%lu, OK:%lu, PK:%lu", 
			m_region, m_subsystem, operatorKey, session.m_profile);
		std::string temp = "TD15346: session: ";
		temp += session.m_session;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, temp.c_str());
		//++TD15346

		m_index++;
	}


	void TransferableSubsystemStateConverter::subsystemDeny(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void TransferableSubsystemStateConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void TransferableSubsystemStateConverter::subsystemComplete()
	{
	}

	void TransferableSubsystemStateConverter::regionComplete()
	{
	}
}



