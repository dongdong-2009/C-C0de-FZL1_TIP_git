/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/HandoverAlarmConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements HandoverAlarmConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/HandoverAlarmConverter.h"

namespace TA_Base_App
{
	const std::string HandoverAlarmConverter::SRC_OP ("SrcOperatorName");
	const std::string HandoverAlarmConverter::SRC_PR ("SrcProfileName");
	const std::string HandoverAlarmConverter::DST_OP ("DstOperatorName");
	const std::string HandoverAlarmConverter::DST_PR ("DstProfileName");
	const std::string HandoverAlarmConverter::REGIONS("RegionNameList");

	HandoverAlarmConverter::HandoverAlarmConverter() :
		m_region(TA_Base_Bus::DASConst::NO_REGION)
	{
	}

	HandoverAlarmConverter::~HandoverAlarmConverter()
	{
		deleteDesc(m_deniedDesc);
		deleteDesc(m_timedOutDesc);
	}

	void HandoverAlarmConverter::deleteDesc( std::vector<TA_Base_Core::DescriptionParameters*>& desc )
	{
		std::vector<TA_Base_Core::DescriptionParameters*>::iterator descVecIt;
		TA_Base_Core::DescriptionParameters::iterator descIt;

		for( descVecIt=desc.begin(); descVecIt!=desc.end(); descVecIt++ )
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

	std::vector<TA_Base_Core::DescriptionParameters*>& HandoverAlarmConverter::buildDeniedAlarmDesc()
	{
		buildDescVector( m_denied, m_deniedDesc );

		return( m_deniedDesc );
	}

	std::vector<TA_Base_Core::DescriptionParameters*>& HandoverAlarmConverter::buildTimedOutAlarmDesc()
	{
		buildDescVector( m_timedOut, m_timedOutDesc );

		return( m_timedOutDesc );
	}

	void HandoverAlarmConverter::buildDescVector( Recalcitrants& recals,
		std::vector<TA_Base_Core::DescriptionParameters*>& descVector )
	{
		Recalcitrants::const_iterator recIt;
		std::set<TA_Base_Bus::DASType::DbKey>::const_iterator regIt;

		std::string dstOp;
		std::string dstPr;

		dstOp = TA_Base_Bus::DutyDbCache::instance()->getOperatorName(
			TA_Base_Bus::DutySessionCache::instance()->getOperator( m_affectedSession.m_session ) );
		
		dstPr = TA_Base_Bus::DutyDbCache::instance()->getProfileName( m_affectedSession.m_profile );

		for( recIt=recals.begin(); recIt!=recals.end(); recIt++ )
		{
			std::string srcOp;
			std::string srcPr;
			
			srcOp = TA_Base_Bus::DutyDbCache::instance()->getOperatorName(
				TA_Base_Bus::DutySessionCache::instance()->getOperator( recIt->first.m_session ) );

			srcPr = TA_Base_Bus::DutyDbCache::instance()->getProfileName( recIt->first.m_profile );

			std::stringstream regions;
			
			for( regIt=recIt->second.begin(); regIt!=recIt->second.end(); regIt++ )
			{
				if( regIt != recIt->second.begin() )
				{
					regions << ", ";
				}
				regions << TA_Base_Bus::DutyDbCache::instance()->getRegionName( *regIt );
			}
				
			std::auto_ptr<TA_Base_Core::NameValuePair> srcOpPair( new TA_Base_Core::NameValuePair(SRC_OP,srcOp) );
			std::auto_ptr<TA_Base_Core::NameValuePair> srcPrPair( new TA_Base_Core::NameValuePair(SRC_PR,srcPr) );
			std::auto_ptr<TA_Base_Core::NameValuePair> dstOpPair( new TA_Base_Core::NameValuePair(DST_OP,dstOp) );
			std::auto_ptr<TA_Base_Core::NameValuePair> dstPrPair( new TA_Base_Core::NameValuePair(DST_PR,dstPr) );

			std::auto_ptr<TA_Base_Core::NameValuePair> regionsPair( new TA_Base_Core::NameValuePair(REGIONS,regions.str()) );

			std::auto_ptr<TA_Base_Core::DescriptionParameters> newDesc( new TA_Base_Core::DescriptionParameters );

			newDesc->push_back(srcOpPair.release());
			newDesc->push_back(srcPrPair.release());
			newDesc->push_back(dstOpPair.release());
			newDesc->push_back(dstPrPair.release());
			newDesc->push_back(regionsPair.release());

			descVector.push_back(newDesc.release());
		}
	}
		
	void HandoverAlarmConverter::setSession(const std::string& session)
	{
		m_affectedSession.m_session = session;

		std::vector<TA_Base_Bus::DASType::DbKey> profiles;

		TA_Base_Bus::DutySessionCache::instance()->getProfiles( session, profiles );

		if( profiles.size() > 0 )
		{
			m_affectedSession.m_profile = profiles.front();
		}
	}

	void HandoverAlarmConverter::setAction(const std::string& action)
	{
		TA_ASSERT( DutyAction::REQUEST_SUBSYSTEM_DUTY_TRANSFER == action || DutyAction::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER == action, 
            "Can only be used for subsystem transfers" );
	}

	void HandoverAlarmConverter::setRegionCount(unsigned long count)
	{
	}

	void HandoverAlarmConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region = region;
	}

	void HandoverAlarmConverter::setSubsystemCount(unsigned long count)
	{
	}

	void HandoverAlarmConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
	}


	void HandoverAlarmConverter::setRegionAddCount( unsigned long count )
	{
	}

	void HandoverAlarmConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void HandoverAlarmConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void HandoverAlarmConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}


	void HandoverAlarmConverter::setSubsystemAddCount( unsigned long count )
	{
	}

	void HandoverAlarmConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void HandoverAlarmConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void HandoverAlarmConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
	}

	void HandoverAlarmConverter::subsystemDeny( const TA_Base_Bus::DutyPrimitive& session )
	{
		mapInsert( m_denied, session );
	}

	void HandoverAlarmConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		mapInsert( m_timedOut, session );
	}

	void HandoverAlarmConverter::subsystemComplete()
	{
	}

	void HandoverAlarmConverter::regionComplete()
	{
	}

	void HandoverAlarmConverter::mapInsert( Recalcitrants& recals, const TA_Base_Bus::DutyPrimitive& session )
	{
		std::pair<Recalcitrants::iterator,bool> insertResult;

		insertResult = recals.insert( Recalcitrants::value_type(session,m_emptyRegSet) );

		insertResult.first->second.insert(m_region);
	}
}



