/**
 * The source code in this file is the property of
 * Ripple Systems and is not for redistribution in any form.
 * 
 * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_agent/src/SessionProfileCache.cpp $
 * @author:   Nick Jardine
 * @version:  $Revision: #2 $
 *
 * Last modification: $DateTime: 2012/06/18 16:11:24 $
 * Last modified by:  $Author: haijun.li $
 *
 * Implements the SessionProfileCache interface.
 *
 */

#include "RightsAgent.h"
#include <algorithm>

namespace TA_Base_App
{
	SessionProfileCache::SessionProfileCache( TA_Base_Bus::SessionUpdateSubject &sessionUpdate ) : 
		ISessionUpdateObserver(sessionUpdate)
    {
	}

	void SessionProfileCache::add( const std::string& session, const std::list<unsigned long>& profileList )
	{
		FUNCTION_ENTRY("add");
		SessionCache::iterator it;

		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		std::ostringstream logInfo;
		logInfo << "session: " << session << ", profiles[";
		
		for(std::list<unsigned long>::const_iterator pit = profileList.begin(); pit != profileList.end(); ++pit)
		{
			if(pit != profileList.begin()) logInfo << ",";

			logInfo << (*pit);
		}

		logInfo << "]";
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[xxx]SessionProfileCache add, %s", logInfo.str().c_str());


		// if we already have this cached, then get rid of it
		if( (it = m_cache.find(session)) != m_cache.end() )
		{
			m_cache.erase(it);
		}

		m_cache.insert( SessionCache::value_type(session,profileList) );
		FUNCTION_EXIT;
	}

	// yanrong++
	void SessionProfileCache::remove( const std::string& session )
	{
		FUNCTION_ENTRY("remove");

		SessionCache::iterator it;
		
		ACE_Write_Guard<ACE_RW_Mutex> guard(m_mutex);

		std::ostringstream logInfo;
		logInfo << "session: " << session ;
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[xxx]SessionProfileCache remove, %s", logInfo.str().c_str());
		
		if( (it = m_cache.find(session)) == m_cache.end() )
		{
			return;
		}
		m_cache.erase(it);

		FUNCTION_EXIT;
	}
	void SessionProfileCache::removeAll()
	{
		FUNCTION_ENTRY("removeAll");
	
		
		m_cache.clear();
	
		FUNCTION_EXIT;
	}
	// ++yanrong

	bool SessionProfileCache::lookup( const std::string& session, std::list<unsigned long>& profileList ) const
	{
		FUNCTION_ENTRY("lookup");
		SessionCache::const_iterator it;

		ACE_Read_Guard<ACE_RW_Mutex> guard(m_mutex);

		if( (it = m_cache.find(session)) == m_cache.end() )
		{
			return( false );
		}
		profileList = it->second;
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Found %lu profiles in the cache", profileList.size() );
		FUNCTION_EXIT;
		return(true);
	}

	void SessionProfileCache::sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation )
	{
		// yanrong++: no need add this session info to cache, caller will update the cache if this session doesn't exist.
		/*
		FUNCTION_ENTRY("sessionBegin");
		std::list<unsigned long> profileList;
		profileList.push_back(profile);

		add( session, profileList );
		FUNCTION_EXIT;
		*/
		// ++yanrong
	}

	void SessionProfileCache::sessionEnd( const std::string& session )
	{
		// yanrong++: sessionUpdate will remove this session.
		/*
		FUNCTION_ENTRY("sessionEnd");
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "%s", session.c_str() );

		SessionCache::iterator it;

		ACE_Guard<ACE_Thread_Mutex> guard(m_mutex);
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session cache size before update: %lu", m_cache.size() );
		if( (it = m_cache.find(session)) == m_cache.end() )
		{
			return;
		}
		m_cache.erase(it);
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Session cache size after update: %lu", m_cache.size() );
		FUNCTION_EXIT;
		*/
		// ++yanrong
	}

	void SessionProfileCache::sessionOverrideBegin( const std::string& session, unsigned long profile )
	{
		// yanrong++ : sessionUpdate will be called by authentication agent when session changed. 
		/*
		SessionCache::iterator it;

		if( (it = m_cache.find(session)) != m_cache.end() )
		{
			it->second.push_back(profile);
		}
		*/
		// ++yanrong
	}

	void SessionProfileCache::sessionOverrideEnd( const std::string& session, unsigned long profile )
	{
		// yanrong++: sessionUpdate will be called by authentication agent when session changed. 
		/*
		SessionCache::iterator cacheIt;

		if( (cacheIt = m_cache.find(session)) != m_cache.end() )
		{
			ProfileList::iterator prfIt;
			
			prfIt = cacheIt->second.begin();

			prfIt = std::find_if( cacheIt->second.begin(), cacheIt->second.end(),
				std::bind2nd(std::not_equal_to<unsigned long>(),profile) );
			while( cacheIt->second.end() != prfIt )
			{
				cacheIt->second.erase(prfIt);
				prfIt = std::find_if( cacheIt->second.begin(), cacheIt->second.end(),
					std::bind2nd(std::not_equal_to<unsigned long>(),profile) );
			}
		}
		*/
		// ++yanrong
	}

} // TA_Base_App
