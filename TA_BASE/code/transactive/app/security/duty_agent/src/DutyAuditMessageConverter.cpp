/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/DutyAuditMessageConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #3 $
 *
 * Last modification: $DateTime: 2012/07/16 10:52:42 $
 * Last modified by:  $Author: liwei.gong $
 *
 * Implements DutyAuditMessageConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/DutyAuditMessageConverter.h"

namespace
{
	const std::string REGION_NAME_LIST("RegionNameList");
	const std::string REGION_NAME("RegionName");
	const std::string SUBSYSTEM_NAME_LIST("SubsystemNameList");
	const std::string SUBSYSTEM_ID("SubsystemId");
	const std::string LOCATION_ID("LocationId");
}


namespace TA_Base_App
{
	DutyAuditMessageConverter::DutyAuditMessageConverter() :
		m_region   (TA_Base_Bus::DASConst::NO_REGION   ),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM)
	{
	}

	DutyAuditMessageConverter::~DutyAuditMessageConverter()
	{
		TA_Base_Core::DescriptionParameters::iterator descIt;

		for( descIt=m_summaryDesc.begin(); descIt!=m_summaryDesc.end(); descIt++ )
		{
			delete *descIt;
			*descIt = NULL;
		}

		std::vector<TA_Base_Core::DescriptionParameters*>::iterator descVecIt;

		for( descVecIt=m_detailDesc.begin(); descVecIt!=m_detailDesc.end(); descVecIt++ )
		{
			for( descIt=(*descVecIt)->begin(); descIt!=(*descVecIt)->end(); descIt++ )
			{
				delete *descIt;
				*descIt = NULL;
			}

			delete *descVecIt;
			*descVecIt = NULL;
		}
	}

	TA_Base_Core::DescriptionParameters& DutyAuditMessageConverter::buildSummaryDesc()
	{
		TA_Base_Bus::DASType::SubsystemMap::const_iterator regIt;

		std::stringstream regionNameList;

		for( regIt=m_regions.begin(); regIt!=m_regions.end(); regIt++ )
		{
			if( regIt != m_regions.begin() )
			{
				regionNameList << ", ";
			}
			regionNameList << TA_Base_Bus::DutyDbCache::instance()->getRegionName( regIt->first );
		}
				
		if( !regionNameList.str().empty() )
		{
			std::auto_ptr<TA_Base_Core::NameValuePair> rePair( new TA_Base_Core::NameValuePair(REGION_NAME_LIST,regionNameList.str()) );

			m_summaryDesc.push_back(rePair.release());
		}

		return( m_summaryDesc );
	}
		
	std::vector<TA_Base_Core::DescriptionParameters*>& DutyAuditMessageConverter::buildDetailDesc()
	{
		TA_Base_Bus::DASType::SubsystemMap::const_iterator regIt;
		TA_Base_Bus::DASType::SubsystemSet::const_iterator subIt;

		std::stringstream regionNameList;

		for( regIt=m_regions.begin(); regIt!=m_regions.end(); regIt++ )
		{
			std::string regionName = TA_Base_Bus::DutyDbCache::instance()->getRegionName( regIt->first );

			std::stringstream subsystemNameList;

			for( subIt=regIt->second.begin(); subIt!=regIt->second.end(); subIt++ )
			{
				if( subIt != regIt->second.begin() )
				{
					subsystemNameList << ", ";
				}
				subsystemNameList << TA_Base_Bus::DutyDbCache::instance()->getSubsystemName( *subIt );
			}

			if( regIt->second.size() > 0 )
			{
				std::auto_ptr<TA_Base_Core::NameValuePair> subPair(
					new TA_Base_Core::NameValuePair(SUBSYSTEM_ID,subsystemNameList.str()) );
				std::auto_ptr<TA_Base_Core::NameValuePair> regPair(
					new TA_Base_Core::NameValuePair(LOCATION_ID,regionName) );

				std::auto_ptr<TA_Base_Core::DescriptionParameters> newDesc( new TA_Base_Core::DescriptionParameters );

				newDesc->push_back(regPair.release());
				newDesc->push_back(subPair.release());

				m_detailDesc.push_back(newDesc.release());
			}
		}

		return( m_detailDesc );
	}

	std::vector<TA_Base_Core::DescriptionParameters*>& DutyAuditMessageConverter::buildSubsystemDetailDesc()
	{
		TA_Base_Bus::DASType::SubsystemMap::const_iterator regIt;
		TA_Base_Bus::DASType::SubsystemSet::const_iterator subIt;

		std::stringstream regionNameList;

		for( regIt=m_regions.begin(); regIt!=m_regions.end(); regIt++ )
		{
			std::string regionName = TA_Base_Bus::DutyDbCache::instance()->getRegionName( regIt->first );

			std::stringstream subsystemNameList;

			for( subIt=regIt->second.begin(); subIt!=regIt->second.end(); subIt++ )
			{
				if( subIt != regIt->second.begin() )
				{
					subsystemNameList << ", ";
				}
				subsystemNameList << TA_Base_Bus::DutyDbCache::instance()->getSubsystemName( *subIt );
			}

			if( regIt->second.size() > 0 )
			{
				//std::auto_ptr<TA_Base_Core::NameValuePair> regPair(
				//	new TA_Base_Core::NameValuePair(REGION_NAME,regionName) );
				//std::auto_ptr<TA_Base_Core::NameValuePair> subPair(
				//	new TA_Base_Core::NameValuePair(SUBSYSTEM_NAME_LIST,subsystemNameList.str()) );
				std::auto_ptr<TA_Base_Core::NameValuePair> regPair(
					new TA_Base_Core::NameValuePair(REGION_NAME,regionName) );
				std::auto_ptr<TA_Base_Core::NameValuePair> subPair(
					new TA_Base_Core::NameValuePair(SUBSYSTEM_NAME_LIST,subsystemNameList.str()) );

				std::auto_ptr<TA_Base_Core::DescriptionParameters> newDesc( new TA_Base_Core::DescriptionParameters );

				newDesc->push_back(regPair.release());
				newDesc->push_back(subPair.release());

				m_detailDesc.push_back(newDesc.release());
			}
		}

		return( m_detailDesc );
	}

	void DutyAuditMessageConverter::setSession(const std::string& session)
	{
		m_session.m_session = session;

		std::vector<TA_Base_Bus::DASType::DbKey> profiles;

		TA_Base_Bus::DutySessionCache::instance()->getProfiles( session, profiles );

		if( profiles.size() > 0 )
		{
			m_session.m_profile = profiles.front();
		}
	}

	void DutyAuditMessageConverter::setAction(const std::string& action)
	{
	}

	void DutyAuditMessageConverter::setRegionCount(unsigned long count)
	{
	}

	void DutyAuditMessageConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}

	void DutyAuditMessageConverter::setSubsystemCount(unsigned long count)
	{
	}

	void DutyAuditMessageConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem = subsystem;
	}


	void DutyAuditMessageConverter::setRegionAddCount( unsigned long count )
	{
	}

	void DutyAuditMessageConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void DutyAuditMessageConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_session == session )
		{
			m_regions.insert(TA_Base_Bus::DASType::SubsystemMap::value_type(m_region,m_emptySubSet));
		}
	}


	void DutyAuditMessageConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void DutyAuditMessageConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void DutyAuditMessageConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void DutyAuditMessageConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		if( m_session == session )
		{
			std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> insertResult = m_regions.insert(
				TA_Base_Bus::DASType::SubsystemMap::value_type(m_region,m_emptySubSet) );

			insertResult.first->second.insert( m_subsystem );
		}
	}

	void DutyAuditMessageConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void DutyAuditMessageConverter::subsystemDeny( const TA_Base_Bus::DutyPrimitive& session )
	{
	}

	void DutyAuditMessageConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void DutyAuditMessageConverter::subsystemComplete()
	{
	}

	void DutyAuditMessageConverter::regionComplete()
	{
	}
}



