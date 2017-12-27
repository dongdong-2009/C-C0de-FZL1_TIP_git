/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/rights_library/src/RightsCache.cpp $
  *  @author:  Gregg Kirkpatrick
  *  @version: $Revision: #2 $
  *
  *  Last modification: $DateTime: 2015/02/11 18:28:17 $
  *  Last modified by:  $Author: liwei.gong $
  *
  *  Caches rights.
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER

#include "RightsLibraryInternal.h"
#include "core/configuration_updates/src/IOnlineUpdatable.h"
#include "core/configuration_updates/src/OnlineUpdateListener.h"
#include "core/utilities/src/Hostname.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/threads/src/ThreadPoolManager.h"

namespace TA_Base_Bus
{
	// match all cached values related to the duty update
	template<class T>
    struct match_duty : public std::binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const
		{
			if( (x.m_session   == y.m_session   ) &&
				(x.m_location  == y.m_location  ) && 
				(x.m_subsystem == y.m_subsystem ))
			{
				return( true );
			}
			return( false );
		}
    };

	// match all entries with a matching session
	template<class T>
    struct match_session : public std::binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const
		{
			if( x.m_session == y.m_session )
			{
				return( true );
			}
			return( false );
		}
    };
	template<class T>
		struct match_location : public std::binary_function<T, T, bool>
	{
		bool operator()(const T& x, const T& y) const
		{
			if( x.m_location == y.m_location )
			{
				return( true );
			}
			return( false );
		}
    };

    // [[Modify start by HuangQi for TD17256 on 2009-07-20
	// RightsCache::RightsCache(SessionUpdateSubject& sessionUpdate) : ISessionUpdateObserver(sessionUpdate)
    RightsCache::RightsCache(SessionUpdateSubject& sessionUpdate) : 
    ISessionUpdateObserver(sessionUpdate),
    TA_Base_Core::IOnlineUpdatable(),
	m_threadPool(NULL),
	m_isSetSession(false),
	m_rightsLibrary(NULL)
    {
		FUNCTION_ENTRY("RightsCache::RightsCache");
        TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
            TA_Base_Core::ACTION_GROUP, *this );
        TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(
            TA_Base_Core::SECURITY, *this );

		m_hostname = TA_Base_Core::Hostname::getHostname();
		
		try
		{
			std::auto_ptr<IConsole> console ( TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromAddress(m_hostname));
			m_consoleKey = console->getKey();
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError, "Fail to get Console %s", m_hostname.c_str());
		}

		// Get the entity name based on the locationKey
		unsigned long locationKey = strtoul( TA_Base_Core::RunParams::getInstance().get( RPARAM_LOCATIONKEY ).c_str(), NULL, 10 );
		
		std::string agentType = TA_Base_Core::RightsAgentEntityData::getStaticType();
		
		TA_Base_Core::CorbaNameList rightsAgent;
		
		try
		{
			rightsAgent = 
				TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNamesOfTypeAtLocation( agentType, locationKey,true);
		}
		catch( const TA_Base_Core::TransactiveException& ex )
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "TA_Base_Core::TransactiveException", ex.what() );
			
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(ex.what()) );
		}
		catch(...)
		{
			LOG( SOURCE_INFO, EXCEPTION_CATCH_LOG_TYPE, "Unknown", "Unhandled" );
			
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException("Non-TransActive, Non-CORBA error") );
		}
		
		std::ostringstream msg;
		msg << "Failed to find agent of type " << agentType << " at location " << locationKey;
		if( 0 >= rightsAgent.size() )
		{
			FUNCTION_EXIT;
			TA_THROW( TA_Base_Core::RightsException(msg.str()) );
		}
		
		boost::shared_ptr<RightsAgentType> agent(new RightsAgentType( rightsAgent[0] ));
		m_rightsAgent = agent;

	}
    // [[Add start by HuangQi for TD17256 on 2009-07-20
    RightsCache::~RightsCache()
    {
    	this->cleanWorkItem();
		
		m_threadPool = NULL;
		m_rightsLibrary = NULL;
		
        TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
            TA_Base_Core::ACTION_GROUP, *this );
        TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(
            TA_Base_Core::SECURITY, *this );
		LOG_GENERIC( SOURCE_INFO, TA_Base_Core::DebugUtil::DebugInfo, "~RightsCache" );
    }

    // Response for online update listener
    void RightsCache::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
    {
        FUNCTION_ENTRY("processUpdate");

        if (TA_Base_Core::ACTION_GROUP == updateEvent.getType())
        {
            switch(updateEvent.getModifications())
            {
                case TA_Base_Core::Create:
                    // Cache no need to clear when new action is created.
                    break;
                case TA_Base_Core::Update:
                case TA_Base_Core::Delete:
                {
                    reset();
                    break;
                }
                default:
                    LOG_GENERIC( SOURCE_INFO, TA_Base_Core::DebugUtil::DebugWarn, "No such config update modification type" );
                    // do nothing
                    break;
            }
        }
        else if (TA_Base_Core::SECURITY == updateEvent.getType())
        {
            switch(updateEvent.getModifications())
            {
                case TA_Base_Core::Create:
                case TA_Base_Core::Delete:
                    // It won't be these types
                    break;
                case TA_Base_Core::Update:
                    {
                        reset();
                        break;
                    }
                default:
                    LOG_GENERIC( SOURCE_INFO, TA_Base_Core::DebugUtil::DebugWarn, "No such config update modification type" );
                    // do nothing
                    break;
            }
        }

        FUNCTION_EXIT;
    }
    // Add end by HuangQi on 2009-07-20]]

	void RightsCache::add( const std::string& session, unsigned long resource, unsigned long action,
			bool result, const std::string& reason, EDecisionModule module )
	{
		FUNCTION_ENTRY("add");
		TA_Base_Core::ThreadGuard guard(m_lock);
		
		unsigned long location, subsystem;

		m_dbCache.getLocSub(resource, location, subsystem);
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "ADD: S:%s, L:%lu, S:%lu, A:%lu (Result:%d)", session.c_str(),
				location, subsystem, action, result );
		
		RightsQuery newQuery( session, location,subsystem, action,
				result, reason, module );

		m_cache.insert( newQuery );
	
		FUNCTION_EXIT;
	}

	void RightsCache::add( const std::string& session, unsigned long location, unsigned long subsystem, unsigned long action,
			bool result, const std::string& reason, EDecisionModule module )
	{
		FUNCTION_ENTRY("add");
		TA_Base_Core::ThreadGuard guard(m_lock);
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "ADD: S:%s, L:%lu, S:%lu, A:%lu (Result:%d)", session.c_str(),
			location, subsystem, action, result );
		
		RightsQuery newQuery( session, location, subsystem, action,
				result, reason, module );

		m_cache.insert( newQuery );
	
		FUNCTION_EXIT;
	}

	void RightsCache::invalidateSession()
	{
		FUNCTION_ENTRY("setSession");
		m_session = "";
		m_isSetSession = false;
		FUNCTION_EXIT;
	}
	const std::string& RightsCache::getSession() const
	{
		return this->m_session;
	}
	void RightsCache::setSession(const std::string& session) const
	{
		FUNCTION_ENTRY("setSession");
		
		if(session.length()> 0 )
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo, "set session from previous ses: %s to ses: %s", m_session.c_str(), session.c_str());
			m_session = session.c_str();
			m_isSetSession = true;			
		}		
			
		FUNCTION_EXIT;
	}
	bool RightsCache::lookup( const std::string& session, unsigned long resource, unsigned long action,
			bool& result, std::string& reason, EDecisionModule& module ) const
	{
		FUNCTION_ENTRY("lookup");
		TA_Base_Core::ThreadGuard guard(m_lock);

		unsigned long location, subsystem;

		m_dbCache.getLocSub(resource, location, subsystem);
		RightsQuery query( session, location, subsystem, action );

		Cache::const_iterator it;

		it = m_cache.find( query );

		if( it == m_cache.end() )
		{
			FUNCTION_EXIT;
			return( false );
		}

		result = it->m_result;
		reason = it->m_reason;
		module = it->m_module;

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "CACHED: S:%s, R:%lu, A:%lu (result:%d)", session.c_str(),
				resource, action, result );

		FUNCTION_EXIT;
		return( true );
	}

	bool RightsCache::lookup( const std::string& session, unsigned long resource, unsigned long action,
		unsigned long & location, unsigned long& subsystem,
		bool& result, std::string& reason, EDecisionModule& module ) const
	{
		FUNCTION_ENTRY("lookup");
		TA_Base_Core::ThreadGuard guard(m_lock);		
	
		
		m_dbCache.getLocSub(resource, location, subsystem);
		RightsQuery query( session, location, subsystem, action );
		
		Cache::const_iterator it;
		
		it = m_cache.find( query );
		
		if( it == m_cache.end() )
		{
			FUNCTION_EXIT;
			return( false );
		}
		
		result = it->m_result;
		reason = it->m_reason;
		module = it->m_module;
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "CACHED: S:%s, R:%lu, A:%lu (result:%d)", session.c_str(),
			resource, action, result );
		
		FUNCTION_EXIT;
		return( true );
	}
	bool RightsCache::lookup( const std::string& session, unsigned long location, unsigned long subsystem, unsigned long action,
			bool& result, std::string& reason, EDecisionModule& module ) const
	{
		FUNCTION_ENTRY("lookup");
		TA_Base_Core::ThreadGuard guard(m_lock);
		

		RightsQuery query( session, location, subsystem, action );

		Cache::const_iterator it;

		it = m_cache.find( query );

		if( it == m_cache.end() )
		{
			FUNCTION_EXIT;
			return( false );
		}

		result = it->m_result;
		reason = it->m_reason;
		module = it->m_module;

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "CACHED: S:%s, Loc:%lu, Sub:%lu A:%lu (result:%d)", session.c_str(),
				location, subsystem, action, result );

		FUNCTION_EXIT;
		return( true );
	}

	bool  RightsCache::processDutyUpdate( const std::string& session, unsigned long location,
			unsigned long subsystem )
	{
		FUNCTION_ENTRY("processDutyUpdate");
		TA_Base_Core::ThreadGuard guard(m_lock);

		bool result = false;

		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "processDutyUpdate Duty update: SES:%s,  CurrentSession: %s", session.c_str(),m_session.c_str());

	
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Duty update: SES:%s, REG:%lu, SUB:%lu",
			session.c_str(), location, subsystem );
		
		Cache::iterator it;
		RightsQuery queryToMatch(session, location, subsystem);
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cache size before update: %lu", m_cache.size() );
		it = std::find_if( m_cache.begin(), m_cache.end(), std::bind1st(match_duty<RightsQuery>(), queryToMatch) );
		while( m_cache.end() != it )
		{
			m_cache.erase(it);
			it = std::find_if( m_cache.begin(), m_cache.end(), std::bind1st(match_duty<RightsQuery>(), queryToMatch) );
			result = true;
		}
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cache size after update : %lu", m_cache.size() );


		FUNCTION_EXIT;
		return result;
	}
	void RightsCache::sessionBegin( const std::string& session, unsigned long user,
		unsigned long profile, unsigned long location,
								   unsigned long workstation )
	{
		FUNCTION_ENTRY("sessionBegin");

		if(m_consoleKey == workstation)
		{
			setSession(session);

			boost::shared_ptr<DutyChanges> dutyChanges ( new DutyChanges);
			TA_Base_Core::IWorkItemPtr workItem(new RightsCacheWorkItem(*this,session, RC_SessionBegin, dutyChanges));

			if (m_threadPool == NULL)
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "ThreadPool not initialized, failed to finish RightsCache::sessionBegin");
				return;
			}
			m_threadPool->queueWorkItem (workItem);
			this->saveWorkItem(workItem);
		}

		FUNCTION_EXIT;
	}
	void RightsCache::changeSessionRights( const std::string& session ,ERightsChangeType changeType)
	{
		
		bool changed = false;
		Cache::iterator it;
		RightsQuery queryToMatch(session);
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cache size before update: %lu", m_cache.size() );
		it = std::find_if( m_cache.begin(), m_cache.end(), std::bind1st(match_session<RightsQuery>(), queryToMatch) );
		while( m_cache.end() != it )
		{
			m_cache.erase(it);
			it = std::find_if( m_cache.begin(), m_cache.end(), std::bind1st(match_session<RightsQuery>(), queryToMatch) );
			changed = true;
		}
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cache size after update : %lu", m_cache.size() );
		

		if(changed )
		{
			boost::shared_ptr<DutyChanges> dutyChanges ( new DutyChanges);
			TA_Base_Core::IWorkItemPtr workItem(new RightsCacheWorkItem(*this,session, changeType, dutyChanges));
			if (m_threadPool == NULL)
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "ThreadPool not initialized, failed to finish RightsCache::changeSessionRights");
				return;
			}
			m_threadPool->queueWorkItem (workItem);
			this->saveWorkItem(workItem);
		}			
		
	}
	void RightsCache::sessionEnd( const std::string& session )
	{
		FUNCTION_ENTRY("sessionEnd");
		TA_Base_Core::ThreadGuard guard(m_lock);

		changeSessionRights(session,RC_SessionEnd);
		if( 0 == m_session.compare(session))
		{
			invalidateSession();
		}
		
		FUNCTION_EXIT;
	}

	void RightsCache::sessionOverrideBegin( const std::string& session, unsigned long profile )
	{
		FUNCTION_ENTRY("sessionOverrideBegin");
	
		changeSessionRights( session,RC_SessionOverrideBegin);
		
		FUNCTION_EXIT;
	}

	void RightsCache::sessionOverrideEnd( const std::string& session, unsigned long profile )
	{
		FUNCTION_ENTRY("sessionOverrideEnd");
	
		changeSessionRights( session,RC_SessionOverrideEnd);	
	
		FUNCTION_EXIT;
	}

	void RightsCache::getSubsystems(unsigned long location, std::list<unsigned long>& subsystems)
	{
		FUNCTION_ENTRY("getSubsystems");

		std::vector<TA_Base_Core::ISubsystem*> regionSubs =
			TA_Base_Core::SubsystemAccessFactory::getInstance().getSubsystemsByRegion(location);
		
		std::vector<TA_Base_Core::ISubsystem*>::iterator subIt;
		
		bool foundUnallocated = false;
		
		for( subIt = regionSubs.begin(); subIt != regionSubs.end(); subIt++ )
		{
			if( !foundUnallocated )
			{
				if( "<Unallocated>" == (*subIt)->getName() )
				{
					foundUnallocated = true;
					delete *subIt;
					*subIt = NULL;
					continue;
				}
			}
			
			subsystems.push_back((*subIt)->getKey());
			
			// This is wasteful but there is currently no way to
			// simply get a list of keys.
			delete *subIt;
			*subIt = NULL;
		}

		FUNCTION_EXIT;
	}
	void RightsCache::reset()
	{
		TA_Base_Core::ThreadGuard guard(m_lock);
		m_cache.clear();
	}
	void RightsCache::reset(unsigned long location)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		bool changed = false;
		Cache::iterator it;
		RightsQuery queryToMatch(location);
		
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cache size before update: %lu", m_cache.size() );
		it = std::find_if( m_cache.begin(), m_cache.end(), std::bind1st(match_location<RightsQuery>(), queryToMatch) );
		while( m_cache.end() != it )
		{
			m_cache.erase(it);
			it = std::find_if( m_cache.begin(), m_cache.end(), std::bind1st(match_location<RightsQuery>(), queryToMatch) );
			changed = true;
		}
		LOG( SOURCE_INFO, GENERIC_LOG_TYPE, DEBUG_LOG_LEVEL, "Cache size after update : %lu", m_cache.size() );

		if(changed )
		{
			std::list<unsigned long> emptySubsystems; 

			boost::shared_ptr<DutyChanges> dutyChanges ( new DutyChanges);

			std::pair<DutyChanges::iterator, bool> result = dutyChanges->insert(DutyChanges::value_type(location, emptySubsystems));			
			
			this->getSubsystems(location,(result.first->second));
			
			TA_Base_Core::IWorkItemPtr workItem(new RightsCacheWorkItem(*this,m_session, RC_DutyRecovery, dutyChanges));
			if (m_threadPool == NULL)
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "ThreadPool not initialized, failed to finish RightsCache::reset");
				return;
			}
			m_threadPool->queueWorkItem (workItem);
			this->saveWorkItem(workItem);
		}
	}
	bool RightsCache::isSuperSession( const std::string& sessionId )
    {
        return ( sessionId == TA_Base_Core::SessionAccessFactory::getSuperSessionId() );
    }
	void RightsCache::reload()
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "begin RightsCache::reload");
		std::set<unsigned long> emptySubsystemSet;
		
		DutyChangesSet dutyChanges;
		SessionDutyChangesMap sessionDutyChanges;
		
		{
			TA_Base_Core::ThreadGuard guard(m_lock);
			
			Cache::const_iterator end = m_cache.end();
			
			CORBA::String_var corbaReason("");
			IRightsAgentCorbaDef::EDecisionModule corbaDm = IRightsAgentCorbaDef::Rights;
			IRightsAgentCorbaDef::ERightsResponse corbaRsp = IRightsAgentCorbaDef::Indeterminate;
			
			std::string reasonString;
			
			
			EDecisionModule decisionModule = UNDEFINED_DM;
			long count = 0;
			for(Cache::iterator itr = m_cache.begin(); itr != end; ++ itr)
			{
				
				if(!isSuperSession((*itr).m_session))
				{
					count ++;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "count: %d", count);
					bool response  = false;
					//recheck the rights
					try
					{
						
						RightsAgentType* rightsAgent = m_rightsAgent.get();
						CORBA_CALL_RETURN( corbaRsp,
							(*rightsAgent),
							isActionPermittedOnLocSub,
							( (*itr).m_session.c_str(), (*itr).m_action,(*itr).m_location,(*itr).m_subsystem, corbaReason.out(), corbaDm ) );
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "ses:%s, loc: %lu, sub:%lu, action:%lu, rsp:%d, result:%d",
							(*itr).m_session.c_str(), (*itr).m_location, (*itr).m_subsystem, (*itr).m_action, corbaRsp, (*itr).m_result);
						
						
					}
					catch ( ...)
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "unknown exception occured!");
							
					}
					// assign the string
					//reasonString = corbaReason.in();
					
					// convert the response
					switch (corbaRsp)
					{
					case IRightsAgentCorbaDef::Permit:
						response = true;
						break;
						
					case IRightsAgentCorbaDef::Deny:
						response = false;
						break;
						
					case IRightsAgentCorbaDef::Indeterminate:
						//TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::INDETERMINATE) );
						response = false;
						break;
						
					case IRightsAgentCorbaDef::NotApplicable:
						//TA_THROW( TA_Base_Core::RightsException(reasonString, TA_Base_Core::RightsException::UNAPPLICABLE) );
						response = false;
						break;
						
					default:
						response = false;
						break;
					}
					
					// convert the decision module
					switch (corbaDm)
					{
					case IRightsAgentCorbaDef::Rights:
						decisionModule = RIGHTS_DM;
						break;
						
					case IRightsAgentCorbaDef::Mutex:
						decisionModule = MUTEX_DM;
						break;
						
					default:
						break;
					}
					if((*itr).m_result != response)
					{
						std::pair<SessionDutyChangesMap::iterator, bool> sessionResult = sessionDutyChanges.insert(
							SessionDutyChangesMap::value_type((*itr).m_session,dutyChanges));
						std::pair<DutyChangesSet::iterator, bool> dutyResult = sessionResult.first->second.insert(
							DutyChangesSet::value_type((*itr).m_location, emptySubsystemSet));
						dutyResult.first->second.insert((*itr).m_subsystem);			 
						itr->m_result = response;
					}
					
				}
			}
			
		}
		
		LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after processing cache");

		if(sessionDutyChanges.size()> 0 )
		{
			std::list<unsigned long> emptySubsystemList;
			

			SessionDutyChangesMap::const_iterator end = sessionDutyChanges.end();

			long sessionCounter = 0;
			long regionDutyCounter = 0; 
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sessionDutyChanges.size: %d", sessionDutyChanges.size());
			for(SessionDutyChangesMap::iterator itr = sessionDutyChanges.begin(); itr != end; ++itr)
			{
				std::string session = itr->first;

				DutyChangesSet& dutyChangeSet = itr->second;

				DutyChanges dutyChanges;

				DutyChangesSet::iterator  dutyChangeEnd = dutyChangeSet.end();

				sessionCounter ++;
				regionDutyCounter = 0;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "sessionCounter: %d", sessionCounter);
				for(DutyChangesSet::iterator itrDutyChange = dutyChangeSet.begin(); itrDutyChange != dutyChangeEnd; ++itrDutyChange)
				{
					regionDutyCounter++;

					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "regionDutyCounter: %d", regionDutyCounter);

					std::pair<DutyChanges::iterator, bool> rtnDutyChanges = dutyChanges.insert(
							DutyChanges::value_type(itrDutyChange->first, emptySubsystemList));

					std::set<unsigned long> & subsystems = itrDutyChange->second;

					std::list<unsigned long> & subsystemList = rtnDutyChanges.first->second;

					subsystemList.resize(subsystems.size());
					std::copy(subsystems.begin(), subsystems.end(), subsystemList.begin());
					notifyRightsChange(session,RC_DutyUpdate, dutyChanges);
				}
			}
		}
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "end RightsCache::reload");

	}

	void RightsCache::notifyRightsChange(std::string& session,ERightsChangeType changeType, DutyChanges& changes)
	{
		FUNCTION_ENTRY("notifyRightsChange");
		if(m_rightsLibrary!=NULL)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"ses: %s, changetype:%d", session.c_str(), changeType);
			
			DutyChanges::const_iterator  rgnEnd  = changes.end();
			for(DutyChanges::iterator rgnItr = changes.begin(); rgnItr != rgnEnd; ++rgnItr )
			{		
				
				std::list<unsigned long>& subsystems = rgnItr->second;
				std::list<unsigned long>::iterator subEnd = subsystems.end();
				for(std::list<unsigned long>::iterator itr = subsystems.begin(); itr != subEnd; ++itr)
				{
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"rgn: %lu, subsystem:%lu", rgnItr->first,*itr );
				}
			}
			if(changeType == RC_DutyUpdate)
			{
				std::string sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
				if(sessionId.length() > 0 )
				{
					RunParams::getInstance().set(RPARAM_SESSIONID, sessionId.c_str());
				}
			}
			this->m_rightsLibrary->notifyObservers(session,changeType,changes);
		}
		FUNCTION_EXIT;
	}

	void RightsCache::setRightsLibrary(CorbaRightsLibrary* rightsLibrary)
	{
		FUNCTION_ENTRY("setRightsLibrary");
		this->m_rightsLibrary = rightsLibrary;
		FUNCTION_EXIT;
	}

	void RightsCache::setThreadPoolManager(TA_Base_Core::ThreadPoolManager* pThreadPool)
	{
		FUNCTION_ENTRY("setThreadPoolManager");
		this->m_threadPool = pThreadPool;
		FUNCTION_EXIT;
	}

	void	RightsCache::saveWorkItem(TA_Base_Core::IWorkItemPtr pWorkItem)
	{
		TA_Base_Core::ThreadGuard guard(m_workItemLock);
		m_workItemList.push_back(pWorkItem);
	}

	void	RightsCache::removeWorkItem(IWorkItem* pWorkItem)
	{
		TA_Base_Core::ThreadGuard guard(m_workItemLock);
		std::vector<TA_Base_Core::IWorkItemPtr>::iterator iter = m_workItemList.begin();
		for (;iter != m_workItemList.end(); ++iter )
        {
        	if ( &(**iter) == pWorkItem )
			{
				m_workItemList.erase(iter);
				break;
			}
		}
	}
	
	void	RightsCache::cleanWorkItem()
	{
		TA_Base_Core::ThreadGuard guard(m_workItemLock);
		for(int index = 0; index < m_workItemList.size(); ++index)
		{
			RightsCacheWorkItem* pRightsCacheWorkItem = dynamic_cast<RightsCacheWorkItem*> (m_workItemList[index].get());
			if(NULL == pRightsCacheWorkItem)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"cannt cast to RightsCacheWorkItem");
			}
			else
			{
				pRightsCacheWorkItem->invalid();
			}
		}
	}

	
		
	RightsCacheWorkItem::RightsCacheWorkItem(RightsCache& parent,std::string session,
		ERightsChangeType changeType, boost::shared_ptr<DutyChanges> dutyChanges) :
	parent_(parent),session_(session),
		changeType_(changeType), dutyChanges_(dutyChanges),m_isValid(true)
	{
		
	}
	RightsCacheWorkItem::~RightsCacheWorkItem()
	{
		FUNCTION_ENTRY("RightsCacheWorkItem::~RightsCacheWorkItem");
		FUNCTION_EXIT;
	}

	void RightsCacheWorkItem::invalid()
	{
		m_isValid = false;
	}
	
	void RightsCacheWorkItem::executeWorkItem()
	{
		//callback from ThreadPool for scheduled work
		if(m_isValid == true)
		{
			parent_.notifyRightsChange(session_,changeType_, *dutyChanges_);
			parent_.removeWorkItem(this);
		}
	}
	
}
