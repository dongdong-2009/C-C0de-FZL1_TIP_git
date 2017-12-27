/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/duty_agent/src/HandoverPopupConverter.cpp $
 * @author:   Gregg Kirkpatrick
 * @version:  $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 *
 * Implements HandoverPopupConverter.
 */

#include "app/security/duty_agent/src/DutyAgent.h"
#include "app/security/duty_agent/src/HandoverPopupConverter.h"
#include "core/utilities/src/DebugUtil.h" //TD15566

namespace TA_Base_App
{
	HandoverPopupConverter::HandoverPopupConverter() :
		m_region(TA_Base_Bus::DASConst::NO_REGION),
		m_subsystem(TA_Base_Bus::DASConst::NO_SUBSYSTEM),
		m_remState(TA_Base_Bus::SubsystemState::SS_NORMAL),
		m_subAdded(false),
		m_subRemoved(false),
		m_regAdded(false),
		m_regRemoved(false)

	{
	}

	void HandoverPopupConverter::buildPopupData(PopupData& popupData) const
	{
		// Gained/Denied (only one of these)
		TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup gainedData;

		bool gainedPopupHasData = false;

		// zeroise unused parts
		gainedData.lost.length(0);

		if( m_gainedMap.size() > 0 )
		{
			buildCorbaStruct( m_gainedMap.begin()->second, gainedData.gained );
			gainedData.targetSession = CORBA::string_dup( m_gainedMap.begin()->first.m_session.c_str() );
			gainedPopupHasData = true;
		}
		else
		{
			gainedData.gained.length(0);
		}

		if( m_deniedMap.size() > 0 )
		{
			buildCorbaStruct( m_deniedMap.begin()->second, gainedData.denied );
			gainedData.targetSession = CORBA::string_dup( m_deniedMap.begin()->first.m_session.c_str() );
			gainedPopupHasData = true;
		}
		else
		{
			gainedData.denied.length(0);
		}

		if( gainedPopupHasData )
		{
			popupData.push_back( gainedData );
		}

		// Lost (there could be one or more of these)
		InternalPopupMap::const_iterator lostIt;
		
		for( lostIt=m_lostMap.begin(); lostIt!=m_lostMap.end(); lostIt++ )
		{
			TA_Base_Bus::DutyAgentTypeCorbaDef::DutyPopup lostData;

			lostData.targetSession = CORBA::string_dup( lostIt->first.m_session.c_str() );

			lostData.gained.length(0);
			lostData.denied.length(0);

			buildCorbaStruct( lostIt->second, lostData.lost );

			popupData.push_back( lostData );
		}
	}
	
	void HandoverPopupConverter::buildCorbaStruct( const DutyTree& internalData,
		TA_Base_Bus::DutyAgentTypeCorbaDef::DutyTreeSequence& corbaData ) const
	{
		corbaData.length( internalData.size() );

		DutyTree::const_iterator sesIt;
		TA_Base_Bus::DASType::SubsystemMap::const_iterator regIt;
		TA_Base_Bus::DASType::SubsystemSet::const_iterator subIt;

		unsigned long sesIdx = 0;
		unsigned long regIdx = 0;
		unsigned long subIdx = 0;

		for( sesIt=internalData.begin(), sesIdx=0; sesIt!=internalData.end(); sesIt++, sesIdx++ )
		{
			corbaData[sesIdx].session = CORBA::string_dup( sesIt->first.m_session.c_str() );
			corbaData[sesIdx].profile = sesIt->first.m_profile;

			corbaData[sesIdx].subsystems.length( sesIt->second.size() );
			
			for( regIt=sesIt->second.begin(), regIdx=0; regIt!=sesIt->second.end(); regIt++, regIdx++ )
			{
				corbaData[sesIdx].subsystems[regIdx].region = regIt->first;

				corbaData[sesIdx].subsystems[regIdx].subsystems.length(regIt->second.size());

				for( subIt=regIt->second.begin(),subIdx=0; subIt!=regIt->second.end(); subIt++,subIdx++ )
				{
					corbaData[sesIdx].subsystems[regIdx].subsystems[subIdx] = *subIt;
				}
			}
		}
	}

	void HandoverPopupConverter::setSession(const std::string& session)
	{
		m_sessionToAdd.m_session = session;

		m_sessionToAddIsActive = isSessionActive(session); //TD15566

		std::vector<TA_Base_Bus::DASType::DbKey> profiles;

		TA_Base_Bus::DutySessionCache::instance()->getProfiles( session, profiles );

		if( profiles.size() > 0 )
		{
			m_sessionToAdd.m_profile = profiles.front();
		}
	}

	void HandoverPopupConverter::setAction(const std::string& action)
	{
		TA_ASSERT( (DutyAction::REQUEST_SUBSYSTEM_DUTY_TRANSFER == action) ||
				   (DutyAction::REQUEST_SUBSYSTEM_DUTY_ALL_TRANSFER == action) ||
				   (DutyAction::REQUEST_REGION_DUTY_TRANSFER == action), "Can only be used for subsystem/region duty transfers" );
	}

	void HandoverPopupConverter::setRegionCount(unsigned long count)
	{
	}

	void HandoverPopupConverter::setRegion(const TA_Base_Bus::DASType::DbKey& region)
	{
		m_region     = region;
		m_regAdded   = false;
		m_regRemoved = false;
	}

	void HandoverPopupConverter::setSubsystemCount(unsigned long count)
	{
	}

	void HandoverPopupConverter::setSubsystem( const TA_Base_Bus::DASType::DbKey& subsystem,
		const TA_Base_Bus::SubsystemState::State& addState, const TA_Base_Bus::SubsystemState::State& remState )
	{
		m_subsystem  = subsystem;
		m_remState   = remState;
		m_subAdded   = false;
		m_subRemoved = false;
	}


	void HandoverPopupConverter::setRegionAddCount( unsigned long count )
	{
	}

	void HandoverPopupConverter::setRegionRemoveCount( unsigned long count )
	{
	}

	void HandoverPopupConverter::regionAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_regAdded = true;
	}


	void HandoverPopupConverter::regionRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		//TD15566++
		bool sessionIsActive = isSessionActive(session.m_session);
		if (m_sessionToAddIsActive && sessionIsActive) //only insert if session is still active 
		{
			mapInsertReg( m_lostMap  , session       , m_sessionToAdd ); 
			mapInsertReg( m_gainedMap, m_sessionToAdd, session        ); 
			m_regRemoved = true;
		}
		//++TD15566
	}


	void HandoverPopupConverter::setSubsystemAddCount( unsigned long count )
	{
		m_addedSessions.clear();
	}

	void HandoverPopupConverter::setSubsystemRemoveCount( unsigned long count )
	{
	}

	void HandoverPopupConverter::subsystemAdd(const TA_Base_Bus::DutyPrimitive& session)
	{
		m_addedSessions.insert(session);
		m_subAdded = true;
	}

	void HandoverPopupConverter::subsystemRemove(const TA_Base_Bus::DutyPrimitive& session)
	{
		// if the session is being removed and not added then send a popup message
		if( m_addedSessions.find(session) == m_addedSessions.end() )
		{
			//TD15566++
			bool sessionIsActive = isSessionActive(session.m_session);
			if (m_sessionToAddIsActive && sessionIsActive && session.m_session != m_sessionToAdd.m_session) //only insert if session is still active and the gain and lost is not same session when operator override
			{
				mapInsertSub( m_lostMap  , session       , m_sessionToAdd ); //TD15566
				mapInsertSub( m_gainedMap, m_sessionToAdd, session        ); //TD15566
				m_subRemoved = true;
				return;
			}
			//++TD15566
		}

		// TD12131: If control duty is being lost for an exclusive subsystem then send a popup message.
        TA_Base_Bus::IDutyPolicy::SUBSYSTEM_TYPE lostSubsystem = TA_Base_Bus::DutyDbCache::instance()->getSubsystemType( m_subsystem );
		TA_Base_Bus::IDutyPolicy::RIGHTS_TYPE lostDuty = TA_Base_Bus::DutyDbCache::instance()->getRightsType( session.m_profile,
			m_subsystem, m_remState );

        if( ( TA_Base_Bus::IDutyPolicy::ST_EXCLUSIVE == lostSubsystem ) && 
            ( TA_Base_Bus::IDutyPolicy::RT_EXCLUSIVE == lostDuty ) )
		{
			//TD15566++
			bool sessionIsActive = isSessionActive(session.m_session);
			if (m_sessionToAddIsActive && sessionIsActive && session.m_session != m_sessionToAdd.m_session) //only insert if session is still active and the gain and lost is not same session when operator override
			{	
				mapInsertSub( m_lostMap  , session       , m_sessionToAdd ); //TD15566
				mapInsertSub( m_gainedMap, m_sessionToAdd, session        ); //TD15566
				m_subRemoved = true;
			}
			//++TD15566
		}
	}

	void HandoverPopupConverter::subsystemDeny( const TA_Base_Bus::DutyPrimitive& session )
	{
		//TD15566++
		bool sessionIsActive = isSessionActive(session.m_session);
		if (m_sessionToAddIsActive && sessionIsActive) //only insert if session is still active 
		{
			mapInsertSub( m_deniedMap, m_sessionToAdd, session ); //TD15566
			m_subRemoved = true;
		}
		//++TD15566
	}

	void HandoverPopupConverter::subsystemTimeout(const TA_Base_Bus::DutyPrimitive& session)
	{
		// same processing
		subsystemRemove( session );
	}

	void HandoverPopupConverter::subsystemComplete()
	{
		// if the subsystem has not been 'taken' from another session then fill in
		// a blank session
		if( m_subAdded && !m_subRemoved )
		{
			TA_Base_Bus::DutyPrimitive emptyDp;

			//TD15566++
			if (m_sessionToAddIsActive) //only insert if session is still active 
			{
				mapInsertSub( m_gainedMap, m_sessionToAdd, emptyDp ); //TD15566
			}
			//++TD15566
		}
	}

	void HandoverPopupConverter::regionComplete()
	{
		// pick up the cases where there were no subsystems transferred but region duty
		// was gained
		if( m_regAdded && !m_regRemoved )
		{
			TA_Base_Bus::DutyPrimitive emptyDp;

			//TD15566++
			if (m_sessionToAddIsActive) //only insert if session is still active 
			{
				mapInsertReg( m_gainedMap, m_sessionToAdd, emptyDp ); //TD15566
			}
			//++TD15566
		}
	}

	std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> HandoverPopupConverter::mapInsertReg(
		InternalPopupMap& popupMap, const TA_Base_Bus::DutyPrimitive& keySession,
		const TA_Base_Bus::DutyPrimitive& valSession )
	{
		std::pair<InternalPopupMap::iterator,bool>              keySesInsertResult;
		std::pair<DutyTree::iterator,bool>                      valSesInsertResult;
		std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> regInsertResult;

		// insert or locate the key session
		keySesInsertResult = popupMap.insert( InternalPopupMap::value_type(keySession, m_emptyDutyTree) );

		// set the value session
		valSesInsertResult = keySesInsertResult.first->second.insert(
			DutyTree::value_type(valSession, m_emptySubMap) );

		// insert or locate the region
		regInsertResult = valSesInsertResult.first->second.insert(
			TA_Base_Bus::DASType::SubsystemMap::value_type(m_region, m_emptySubSet) );

		return( regInsertResult );
	}

	void HandoverPopupConverter::mapInsertSub( InternalPopupMap& popupMap, const TA_Base_Bus::DutyPrimitive& keySession,
		const TA_Base_Bus::DutyPrimitive& valSession )
	{
		std::pair<TA_Base_Bus::DASType::SubsystemMap::iterator,bool> regInsertResult;


		regInsertResult = mapInsertReg( popupMap, keySession, valSession );

		// insert the subsystem
		regInsertResult.first->second.insert( m_subsystem );
	}

	bool HandoverPopupConverter::isSessionActive(std::string session)
	{
		bool isSesAct = true;

		try
		{
			// find the session in the DB
			TA_Base_Core::ISession* isession;
			isession = TA_Base_Core::SessionAccessFactory::getInstance().getSession( session );        

			// check if it has ended or not
			if (isession->getHasEnded())
			{
				isSesAct = false;
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD15566] session hasEnded in DB");
			}
			else
			{
				isSesAct = true;
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD15566] session NOT hasEnded in DB");
			}
		}
		catch(const TA_Base_Core::DatabaseException& ex)
        {
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD15566] DatabaseException, force isSesAct to true");
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", ex.what() );
		}
		catch (...)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "[TD15566] Unknown exception, force isSesAct to true");
		}
		
		return isSesAct;
	}
}