/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_agent/duty_agent_state/src/DutySessionCache.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  <description>
  */
#include "DutyAgentState.h"

namespace TA_Base_Bus
{

	void DutySessionCacheImpl::sessionBegin(const TA_Base_Bus::SessionInfo& session)
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_lock);
		m_sessionMap.insert( SessionMap::value_type(session.SessionId, session) );
	}

	void DutySessionCacheImpl::sessionOverrideBegin(const TA_Base_Bus::SessionInfo& session )
	{
		SessionMap::iterator it;
		
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_lock);


		if( (it = findSession(session.SessionId)) != m_sessionMap.end() )
		{
			it->second.ProfileId.assign( session.ProfileId.begin(), session.ProfileId.end() );
		}
		else
		{
			m_sessionMap.insert( SessionMap::value_type(session.SessionId, session) );
		}
	
	}

	void DutySessionCacheImpl::sessionOverrideEnd(const TA_Base_Bus::SessionInfo& session )
	{
		SessionMap::iterator it;
		
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_lock);

		
		if( (it = findSession(session.SessionId)) != m_sessionMap.end() )
		{
			std::vector<DASType::DbKey>::iterator prfIt;
			bool found=false;

			prfIt = it->second.ProfileId.begin();

			// unsuccessfully tried to use remove_if...
			while( !found && prfIt!=it->second.ProfileId.end() )
			{

				if( session.ProfileId.front() == *prfIt )
				{
					it->second.ProfileId.erase(prfIt);
					found = true;
				}
				else
				{
					prfIt++;
				}
			}

			//std::remove_if( it->second.ProfileId.begin(), it->second.ProfileId.end(),
			//    std::bind2nd(std::equal_to<DASType::DbKey>(), session.ProfileId.front()) );

		}
		else
		{
			//keep the session info for later other possible usages such as getProfiles etc
			m_sessionMap.insert( SessionMap::value_type(session.SessionId, session) );
		}
	}

	void DutySessionCacheImpl::sessionEnd(const TA_Base_Bus::SessionInfo& session )
	{
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_lock);

		m_sessionMap.erase( session.SessionId );
	}

	bool DutySessionCacheImpl::isOverrideBegin(const TA_Base_Bus::SessionInfo& session )
	{
		bool isBegin = false;
		SessionMap::iterator it;
		
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_lock);

		/*
		if( (it = findSession(session.SessionId)) != m_sessionMap.end() )
		{
			if( it->second.ProfileId.size() < session.ProfileId.size() )
			{
				isBegin = true;
				
			}
			else
			{
				isBegin = false;
			}
		}
		else
		{
			std::ostringstream msg;
			msg << "Invalid session override for: " << session.SessionId;
			TA_THROW( TA_Base_Core::DutyAgentException(msg.str()) );
		}
		*/

		/*
		there is no nested override, so the parameter session has enough info to determine 
		if it is the begin of overriding
		*/
		isBegin = (session.ProfileId.size()> 1);

		return( isBegin );
	}

	TA_Base_Bus::SessionInfo DutySessionCacheImpl::getSessionInfo( const std::string& session )
	{
		SessionMap::iterator it;

		if( (it = findSession(session,true)) != m_sessionMap.end() )
		{
			return( it->second );
		}

		return( TA_Base_Bus::SessionInfo() );
	}

	DASType::DbKey DutySessionCacheImpl::getOperator( const std::string& session )
	{
		SessionMap::iterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_lock);

		if( (it = findSession(session,true)) != m_sessionMap.end() )
		{
			return( it->second.UserId );
		}

		std::ostringstream msg;
		msg << "Unable to get operator for session: " << session;
		TA_THROW( TA_Base_Core::DutyAgentException(msg.str()) );
	}

	DASType::DbKey DutySessionCacheImpl::getLocation( const std::string& session )
	{	
		SessionMap::iterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_lock);

		if( (it = findSession(session,true)) != m_sessionMap.end() )
		{
			return( it->second.LocationId );
		}

		std::ostringstream msg;
		msg << "Unable to get region for session: " << session;
		TA_THROW( TA_Base_Core::DutyAgentException(msg.str()) );
	}

	void DutySessionCacheImpl::getProfiles( const std::string& session,
		std::vector<DASType::DbKey>& profiles )
	{
		SessionMap::iterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_lock);

		if( (it = findSession(session,true)) != m_sessionMap.end() )
		{
			profiles.assign( it->second.ProfileId.begin(), it->second.ProfileId.end() );
		}
		else
		{
			std::ostringstream msg;
			msg << "Unable to get profiles for session: " << session;
			TA_THROW( TA_Base_Core::DutyAgentException(msg.str()) );
		}
	}

	bool DutySessionCacheImpl::isValidRegion( const DutyPrimitive& dp, const DASType::DbKey& region )
	{
		SessionMap::iterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_lock);

		bool isValid = false;

		if( (it = findSession(dp.m_session,true)) != m_sessionMap.end() )
		{
			if( IDutyPolicy::PT_LOCAL == DutyDbCache::instance()->getProfileType(dp.m_profile) )
			{
				if( it->second.LocationId == region )
				{
					isValid = true;
				}
			}
			else
			{
				isValid = true;
				/*
				DASType::RegionSet regions;

				DutyDbCache::instance()->getOperatorRegions( it->second.UserId, regions );

				if( (regions.find(region) != regions.end()) ||
					(regions.find(DASConst::ALL_REGIONS) != regions.end()) )
				{
					isValid = true;
				}
				*/
			}

			return( isValid );
		}
		else
		{
			std::ostringstream msg;
			msg << "Unable to get regions for session: " << dp.m_session;
			TA_THROW( TA_Base_Core::DutyAgentException(msg.str()) );
		}
	}

	DutySessionCacheImpl::SessionMap::iterator DutySessionCacheImpl::findSession( const std::string& session, bool retrieveFromAuth /* = false */ )
	{
		SessionMap::iterator it = m_sessionMap.end();
		
		if( (it = m_sessionMap.find(session)) == m_sessionMap.end() )
		{
			if(retrieveFromAuth)
			{		
				try
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, TA_Base_Core::DebugUtil::DebugWarn, "DutySessionCacheImpl::findSession sessionID: %s", session.c_str());
					TA_Base_Bus::SessionInfo newSessionInfo = m_authLibrary.getSessionInfo(session,session);

					std::pair<SessionMap::iterator,bool> insertResult =
						m_sessionMap.insert( SessionMap::value_type(session,newSessionInfo) );

					it = insertResult.first;
				}
				catch(...)
				{
					LOG( SOURCE_INFO, GENERIC_LOG_TYPE, TA_Base_Core::DebugUtil::DebugError, "failed to get session from AuthLib sessionID: %s", session.c_str());
					it = m_sessionMap.end();
				}
			}
		}
		
		return( it );
	}
	void DutySessionCacheImpl::cacheSession(const TA_Base_Bus::SessionInfo& session)
	{
		ACE_Read_Guard<ACE_RW_Mutex> guard(m_lock);
		
		if( m_sessionMap.find(session.SessionId) == m_sessionMap.end() )
		{
			std::pair<SessionMap::iterator,bool> insertResult =
				m_sessionMap.insert( SessionMap::value_type(session.SessionId,session) );
		}
	}
}
