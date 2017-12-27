/**
  *  The source code in this file is the property of
  *  Ripple Systems and is not for redistribution
  *  in any form.
  *
  *  Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/security_utilities/src/SessionDetailsCache.cpp $
  *  @author:  <your name>
  *  @version: $Revision: #1 $
  *
  *  Last modification: $DateTime: 2015/01/19 17:43:23 $
  *  Last modified by:  $Author: CM $
  *
  *  Provides a cached list of all operators that were/are associated with
  *  a given session at a given time. 
  */

#ifdef _MSC_VER
	#pragma warning(disable: 4503)  // decorated name length exceeded, name was truncated
#endif // _MSC_VER


#include "SecurityUtilitiesInternal.h"

namespace TA_Base_Bus
{
	const std::string SessionDetailsCache::SUPERVISOR_ID("SupervisorID");

	SessionDetailsCache::SessionDetailsCache( SessionUpdateSubject& sessionUpdate, const std::string& delim/*="/"*/ ) :
		ISessionUpdateObserver(sessionUpdate),
		m_operatorLookup(m_operatorMap ,TA_Base_Core::OperatorAccessFactory::getInstance() ,&TA_Base_Core::OperatorAccessFactory::getOperator),
		m_delim(delim)
	{
		TA_Base_Core::OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::OPERATOR , *this);
	}

	SessionDetailsCache::~SessionDetailsCache()
	{
		TA_Base_Core::OnlineUpdateListener::getInstance().deregisterAllInterests(TA_Base_Core::OPERATOR , *this);

		OperatorMap::iterator it;

		TA_Base_Core::ThreadGuard guard(m_accessLock);
		for( it = m_operatorMap.begin(); it != m_operatorMap.end(); it++ )
		{
			delete it->second;
		}
		m_operatorMap.clear();

	}

	std::string SessionDetailsCache::getOperatorDelimList( const std::string& session, time_t time )
	{
		TA_Base_Core::ThreadGuard guard(m_accessLock);

		RequiresUpdateMap::iterator it;

		if( (it = m_requiresUpdate.find(session)) != m_requiresUpdate.end() )
		{
			if( it->second )
			{
				getDatabaseInfo(session);
			}

		}
		else
		{
			getDatabaseInfo(session);
		}
		
		std::vector<std::string> ops;

		m_mapBuilder.getOperators( ops, session, time );

		std::string rval = "";
		
		std::vector<std::string>::const_iterator opIt;

		for( opIt=ops.begin(); opIt!=ops.end(); opIt++ )
		{
			if( opIt != ops.begin() )
			{
				rval += m_delim;
			}
			rval += *opIt;
		}

		return( rval );

	}

	void SessionDetailsCache::sessionBegin( const std::string& session, unsigned long user,
			                       unsigned long profile, unsigned long location,
								   unsigned long workstation )
	{
		TA_Base_Core::ThreadGuard guard(m_accessLock);

		// can't update directly from the update because there is no time information
		updateUpdateRequired( session, true );
	}

	void SessionDetailsCache::sessionEnd( const std::string& session )
	{
		TA_Base_Core::ThreadGuard guard(m_accessLock);

		// can't update directly from the update because there is no time information
		updateUpdateRequired( session, true );
	}

	void SessionDetailsCache::sessionOverrideBegin( const std::string& session, unsigned long profile )
	{
		TA_Base_Core::ThreadGuard guard(m_accessLock);

		// can't update directly from the update because there is no time or overriding operator information
		updateUpdateRequired( session, true );
	}

	void SessionDetailsCache::sessionOverrideEnd( const std::string& session, unsigned long profile )
	{
		TA_Base_Core::ThreadGuard guard(m_accessLock);

		// can't update directly from the update because there is no time information
		updateUpdateRequired( session, true );
	}

	void SessionDetailsCache::getDatabaseInfo( const std::string& session )
	{
		std::vector<TA_Base_Core::IMessageData*> msgs;

		msgs = TA_Base_Core::MessageAccessFactory::getInstance().getAuthMessagesForSession(session);

		std::vector<TA_Base_Core::IMessageData*>::iterator msgsIt;

		if( msgs.size() > 0 )
		{
			updateUpdateRequired( session, false );
		}
			
		for( msgsIt=msgs.begin(); msgsIt!=msgs.end(); msgsIt++ )
		{
			if( TA_Base_Core::MessageAccessFactory::SESSION_STARTED_EVENT_NAME == (*msgsIt)->getTypeName() )
			{
				m_mapBuilder.sessionBegin( session, getOperatorFromDb(session), (*msgsIt)->getCreatetime().time );
			}
			else if( TA_Base_Core::MessageAccessFactory::SESSION_ENDED_EVENT_NAME == (*msgsIt)->getTypeName() )
			{
				m_mapBuilder.sessionEnd( session, (*msgsIt)->getCreatetime().time );
			}
			else if( TA_Base_Core::MessageAccessFactory::OVERRIDE_STARTED_EVENT_NAME == (*msgsIt)->getTypeName() )
			{
				m_mapBuilder.overrideBegin( session, getOperatorFromEvent( (*msgsIt)->getParamList() ),
					(*msgsIt)->getCreatetime().time );
			}
			else if( TA_Base_Core::MessageAccessFactory::OVERRIDE_ENDED_EVENT_NAME == (*msgsIt)->getTypeName() )
			{
				m_mapBuilder.overrideEnd( session, (*msgsIt)->getCreatetime().time );
			}
		}
	}

	std::string SessionDetailsCache::getOperatorFromDb( const std::string& session )
	{
		std::auto_ptr<TA_Base_Core::ISession> dbSession(
			TA_Base_Core::SessionAccessFactory::getInstance().getSession(session));

		unsigned long opKey(ULONG_MAX);

		if( dbSession.get() == NULL )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Session %s not in database", session.c_str() );
			return("");
		}

		opKey = dbSession->getOperatorKey();

		TA_Base_Core::IOperator* op =  m_operatorLookup.lookup( opKey );

		if( 0 == op )
		{
			LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Operator %lu not in database", opKey );
			return("");
		}
	
		return( op->getName() );
	}

	std::string SessionDetailsCache::getOperatorFromEvent( const std::string& paramList )
	{
		TA_Base_Core::DescriptionParameters params;

		TA_Base_Core::gGetParametersFromString( paramList, params );

		TA_Base_Core::DescriptionParametersIterator it;

		std::string opName;

		for( it=params.begin(); it!=params.end(); it++ )
		{
			if( (*it)->name == SUPERVISOR_ID )
			{
				opName = (*it)->value;
			}
			delete *it;
		}

		return( opName );
	}

	void SessionDetailsCache::updateUpdateRequired( const std::string& session, bool state )
	{
		std::pair<RequiresUpdateMap::iterator,bool> insertResult;

		insertResult = m_requiresUpdate.insert( RequiresUpdateMap::value_type(session,state) );

		if( false == insertResult.second)
		{
			insertResult.first->second = state;
		}
	}

	void SessionDetailsCache::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
	{
		switch( updateEvent.getType() )
		{	
			case TA_Base_Core::OPERATOR:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, INFO_LOG_LEVEL, "Update received for operator %lu", updateEvent.getKey() );
				
				if( TA_Base_Core::Create !=  updateEvent.getModifications() )
				{
					TA_Base_Core::ThreadGuard guard(m_accessLock);

					OperatorMap::iterator it;

					if( (it=m_operatorMap.find(updateEvent.getKey())) != m_operatorMap.end() )
					{
						delete it->second;
						m_operatorMap.erase(it);
					}
				}
			}
			break;

			default:
			{
				LOG( SOURCE_INFO, GENERIC_LOG_TYPE, ERROR_LOG_LEVEL, "Unexpected update type %lu", updateEvent.getType() );
			}
			break;
		}
	}
}
