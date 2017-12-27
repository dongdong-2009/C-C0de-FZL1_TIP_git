/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/DBRetriever.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "app/event/event_agent/src/DbEventRetriever.h"
#include "app/event/event_agent/src/DatabaseLoader.h"
#include "app/event/event_agent/src/LocalRunParamDefinitions.h"
#include "app/event/event_agent/src/EventCache.h"

#include "core/event/IDL/src/EventAgentCorbaDef.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ICombinedEventData.h"

#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/database/src/SimpleDb.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/Timer.h"
#include "core/utilities/src/StdPubUtility.h"
#include "boost/shared_ptr.hpp"

#include <time.h>

#define DEFAULT_AGENT_REFRESH_INTERVAL 500
#define DEFAULT_DB_REFRESH_INTERVAL 500
#define DEFAULT_MAX_CACHE_SIZE 50000
#define EVENT_AGENT_TIMEOUT 300
#define DATA_SYNC_LIMIT 20000

using namespace TA_Base_Core;

namespace TA_Base_App
{
	// Static variable initialization
	DbEventRetriever* DbEventRetriever::m_pInstance = 0;
	TA_Base_Core::NonReEntrantThreadLockable DbEventRetriever::m_bSingletonLock;

	/**
	 * Construction
	 */
	DbEventRetriever::DbEventRetriever()
		: m_bTerminate(false)
		, m_bDataLoaded(false)
		, m_pDbLoader(0)
		, m_ulDbRefreshInterval(0)
		, m_ulMyLocation(0)
		, m_currentDbString("")
		, m_bDatabaseChanged(false)
	{
		FUNCTION_ENTRY("DbEventRetriever");

		unsigned long ulInterval = DEFAULT_DB_REFRESH_INTERVAL;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::DATABASE_REFRESH_INTERVAL.c_str()))
		{
			ulInterval = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::DATABASE_REFRESH_INTERVAL.c_str()).c_str());
			if (ulInterval == 0)
			{
				ulInterval = DEFAULT_DB_REFRESH_INTERVAL;
			}
		}
		m_ulDbRefreshInterval = ulInterval;

		unsigned long ulAgentRefresh = DEFAULT_AGENT_REFRESH_INTERVAL;
		if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::AGENT_REFRESH_INTERVAL.c_str()))
		{
			ulAgentRefresh = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::AGENT_REFRESH_INTERVAL.c_str()).c_str());
			if (ulAgentRefresh == 0)
			{
				ulAgentRefresh = DEFAULT_DB_REFRESH_INTERVAL;
			}
		}
		m_ulAgentRefreshInterval = ulAgentRefresh;

		std::stringstream location( RunParams::getInstance().get(RPARAM_LOCATIONKEY) );
		location >> m_ulMyLocation;

		//Get Current Connection String
		m_currentDbString = getCurrentDatabaseString();

		// Register RunParam User
		registerForDbRunParamChange();

		FUNCTION_EXIT;
	}
	
	/**
	 * Destructor
	 */
	DbEventRetriever::~DbEventRetriever()
	{
		FUNCTION_ENTRY("~DbEventRetriever");
		if (m_pDbLoader != 0)
		{
			m_pDbLoader->terminateAndWait();
			delete m_pDbLoader;
			m_pDbLoader = 0;
		}

		terminateAndWait();
		FUNCTION_EXIT;
	}
	 
	/**
      * getInstance
      */
	DbEventRetriever& DbEventRetriever::getInstance()
	{
		FUNCTION_ENTRY("getInstance");
		if (0 != m_pInstance)
		{
			return *m_pInstance;
		}

		TA_Base_Core::ThreadGuard guard(m_bSingletonLock);
		if (0 == m_pInstance)
		{
			m_pInstance = new DbEventRetriever();
		}

		FUNCTION_EXIT;
		return *m_pInstance;
	}

	/**
      * releaseInstance
      */
	void DbEventRetriever::releaseInstance()
	{
		FUNCTION_ENTRY("releaseInstance");
		TA_Base_Core::ThreadGuard guard(m_bSingletonLock);
		if (0 != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = NULL;
		}

		FUNCTION_EXIT;

	}

	/**
      * run
      */
	void DbEventRetriever::run()
	{
		FUNCTION_ENTRY("run");
		if (m_refreshType == DATABASE_REFRESH)
		{
			_dbRefreshProc();
		}
		else if (m_refreshType == AGENT_REFRESH)
		{
			_agentRefreshProc();
		}
		else
		{
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Unknown event data retrieval was set.");
			throw;
		}

		FUNCTION_EXIT;
	}

    /**
      * terminate
      */
	void DbEventRetriever::terminate()
	{
		FUNCTION_ENTRY("terminate");
		m_bTerminate = true;
		
		FUNCTION_EXIT;
	}

	/** 
      * loadFromDatabase
      */
	void DbEventRetriever::loadFromDatabase()
	{
		FUNCTION_ENTRY("loadFromDatabase");
		if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_STANDBY))
		{
			m_bDataLoaded = true;
			LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "loadFromDatabase exit: Skip event Data loading for Standby mode.");
			return;
		}

		try
		{
			Timer timer;
			std::list<ICombinedEventData*> events;
			unsigned long ulLatestDbSeq = 0;
			
			unsigned long ulCacheSize = DEFAULT_MAX_CACHE_SIZE;
			if (TA_Base_Core::RunParams::getInstance().isSet(TA_EventAgent::MAX_CACHE_SIZE.c_str()))
			{
				ulCacheSize = atol(TA_Base_Core::RunParams::getInstance().get(TA_EventAgent::MAX_CACHE_SIZE.c_str()).c_str());
				if (ulCacheSize == 0)
				{
					ulCacheSize = DEFAULT_MAX_CACHE_SIZE;
				}
			}
		
			//Query the database
			events = CombinedEventAccessFactory::getInstance().getLatest24hrsEvents(ulCacheSize, ulLatestDbSeq);
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"load event from db count=%d", events.size());

			unsigned int loadfromdb=events.size();
			if (events.size() > 0)
			{
				EventCache::getInstance().proccessDbData(events, ulLatestDbSeq);
			}

			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,
				"Loaded %li events in %f seconds", events.size(), timer.elapsedTime());

			events.clear();

			{//Update current connection string	
				TA_Base_Core::ThreadGuard guard(m_bThreadLock);
				m_currentDbString = getCurrentDatabaseString();
				m_bDatabaseChanged = false;
			}
			
			startDbRefresh(); // Start the database synchronization
			m_bDataLoaded = true;

		}
		catch(const DatabaseException&)
		{
			m_bDataLoaded = true;  //force the agent start to work
		}

		FUNCTION_EXIT;
	}

    /** 
      * requestFromControllingAgent
      */
	void DbEventRetriever::requestFromControllingAgent()
	{
		FUNCTION_ENTRY("requestFromControllingAgent");
		if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
		{
			try
			{
				boost::shared_ptr<TA_Base_Core::EvDataPkgCorbaDef> eventSeq;
				
				// Stop the loader from calling this function again
				m_pDbLoader->acquire(false);

				// control event agent corba callback
				EventAgentNameObj controlEventAgent;
				// Set the named objects' corba name so that the Event Agent can be resolved correctly.
				controlEventAgent.setEntityName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(), true);
				// Extend the default corba timeout.
				controlEventAgent.setObjectTimeout(EVENT_AGENT_TIMEOUT);	

				TA_Base_Core::EvDataPkgCorbaDef * pTmpEventSeq = NULL;
				std::string strSessionID = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

				if ( controlEventAgent.getAgentName().empty() == false)
				{
					CORBA_CALL_RETURN( pTmpEventSeq,
						(controlEventAgent),
						getTopEvents,
						(DATA_SYNC_LIMIT, strSessionID.c_str()) );
					eventSeq.reset(pTmpEventSeq);
					
					EventCache::getInstance().insertEvents(eventSeq.get());					
					m_bDataLoaded = true;

					// Set the latest event ID
					// m_latestEventID = "Test";
					// Start Agent synchronization
					startAgentSync();
				}
				else
				{
					LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
						"Can't locate or initialise Control Event Agent. Try again later.");
					m_bDataLoaded = false;
					m_pDbLoader->acquire(true);
				}
			}
			catch(const CORBA::SystemException& ex)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
				m_bDataLoaded = false;
				m_pDbLoader->acquire(true);
			}
			catch(TA_Base_Core::ObjectResolutionException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
				m_bDataLoaded = false;
				m_pDbLoader->acquire(true);
			}
			catch(TA_Base_Core::OperationModeException&)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't load event Cache or Queue from Control Event Agent");
				m_bDataLoaded = false;
				m_pDbLoader->acquire(true);
			}
			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
					"Can't load Event Cache or Queue from Control Event Agent.	Caught TransactiveException: %s", ex.what());
				m_bDataLoaded = false;
				m_pDbLoader->acquire(true);
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
					"Can't load Event Cache or Queue from Control Event Agent. Caught Corba Exception");
				m_bDataLoaded = false;
				m_pDbLoader->acquire(true);

			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"Unknown exception caught: Can't load Event Cache or Queue from Control Event Agent. Caught General Exception" );
				m_bDataLoaded = false;
				m_pDbLoader->acquire(true);
			}
		}
		FUNCTION_EXIT;
	}

    /**
      * hasDataLoaded
      */
	bool DbEventRetriever::hasDataLoaded()
	{
		FUNCTION_ENTRY("hasDataLoaded");
		FUNCTION_EXIT;
		return m_bDataLoaded;
	}

    /**
      * LoadData
      */
	void DbEventRetriever::LoadData()
	{
		FUNCTION_ENTRY("LoadData");

		if( m_pDbLoader == 0 )
		{
			m_pDbLoader = new DatabaseLoader(this);
			m_pDbLoader->start();
		}

		FUNCTION_EXIT;
	}

	/**
      * ResetDataLoader
      */
	void DbEventRetriever::resetDataLoader()
	{
		FUNCTION_ENTRY("resetDataLoader");
		if( m_pDbLoader != 0 )
		{
			m_pDbLoader->terminateAndWait();
			delete m_pDbLoader;
			m_pDbLoader = 0;
		}
		m_bDataLoaded = false;
		FUNCTION_EXIT;
	}

	/**
      * startDbRefresh
      */
	void DbEventRetriever::startDbRefresh()
	{
		FUNCTION_ENTRY("startDbRefresh");
		m_refreshType = DATABASE_REFRESH;
		m_bTerminate = false;
		start();

		FUNCTION_EXIT;
	}

	/**
      * startAgentSync
      */
	void DbEventRetriever::startAgentSync()
	{
		FUNCTION_ENTRY("startAgentSync");
		m_refreshType = AGENT_REFRESH;
		m_bTerminate = false;
		start();

		FUNCTION_EXIT;
	}

	// Database refresh mechanism
	void DbEventRetriever::_dbRefreshProc()
	{
		FUNCTION_ENTRY("_dbRefreshProc");
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database Refresh process run entry.");

		while(!m_bTerminate)
		{
			Timer timer;
			std::list<ICombinedEventData*> events;
			unsigned long ullatestSeqNumber = EventCache::getInstance().getLatestDbSeqNumber();
			
			try
			{
				if (hasDatabaseChanged())
				{
					//Query the database
					events = CombinedEventAccessFactory::getInstance().getLatest24hrsEvents(20000, ullatestSeqNumber);
					
					{ // Set database changed to false after one query
						TA_Base_Core::ThreadGuard guard(m_bThreadLock);
						m_bDatabaseChanged = false;
					}
				}
				else
					//Query the database
					events = CombinedEventAccessFactory::getInstance().getLatestEvents(ullatestSeqNumber, ullatestSeqNumber);
			}
			catch(const DatabaseException& ex)
			{
				LOG1( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
					"Database exception caught: Error while performing a query to database. %s", ex.what());
				TA_Base_Core::Thread::sleep(500); //Sleep Additional 500 millisecond before trying again.
			}
			catch (...)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
					"Unknown exception caught: Error while performing a query to database." );
				TA_Base_Core::Thread::sleep(500); //Sleep Additional 500 millisecond before trying again.
			}
			
			if (events.size() > 0)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
					"Retrieved %li events from db in %f seconds", events.size(), timer.elapsedTime());
				
				EventCache::getInstance().proccessDbData(events, ullatestSeqNumber);
				events.clear();
			}

			TA_Base_Core::Thread::sleep(m_ulDbRefreshInterval);
		}

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Database Refresh process run exit.");
		FUNCTION_EXIT;
	}

	// agent refresh mechanism
	void DbEventRetriever::_agentRefreshProc()
	{
		FUNCTION_ENTRY("_agentRefreshProc");
		bool bNeedReload = false;
		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Agent Refresh process run entry.");

		// control event agent corba callback
		EventAgentNameObj controlEventAgent;
		// Set the named objects' corba name so that the Event Agent can be resolved correctly.
		controlEventAgent.setEntityName(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str(), true);
		// Extend the default corba timeout.
		controlEventAgent.setObjectTimeout(EVENT_AGENT_TIMEOUT);	
		
		while(!m_bTerminate)
		{
			if( 0 == TA_Base_Core::RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_MONITOR) )
			{
				bool bFailure = false;
				try
				{
					if ( controlEventAgent.getAgentName().empty() == false && !bNeedReload)
					{
						boost::shared_ptr<TA_Base_Core::EventSyncCorbaDef> eventSeq;
						TA_Base_Core::EventSyncCorbaDef* pTmpSyncData = NULL;

						std::string strEventID = EventCache::getInstance().getLatestEventID();
						LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "About to getSyncData to control agent with event ID %s", strEventID.c_str());
						CORBA_CALL_RETURN( pTmpSyncData,
							(controlEventAgent),
							getSyncData,
							(CORBA::string_dup(strEventID.c_str())) );
						
						eventSeq.reset(pTmpSyncData);

						EventCache::getInstance().proccessSyncData(eventSeq.get());					
					}
					else if ( controlEventAgent.getAgentName().empty() == false && bNeedReload)
					{
						boost::shared_ptr<TA_Base_Core::EvDataPkgCorbaDef> eventSeq;
						TA_Base_Core::EvDataPkgCorbaDef * pTmpEventSeq = NULL;
						CORBA_CALL_RETURN( pTmpEventSeq,
							(controlEventAgent),
							getTopEvents,
							(20000, std::string("").c_str()) );
						eventSeq.reset(pTmpEventSeq);

						// Delete all existing event data
						EventCache::getInstance().deleteAllCacheData();

						// Insert new cache events
						EventCache::getInstance().insertEvents(eventSeq.get());		

						bNeedReload = false;
					}
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
							"Can't locate or initialise Control Event Agent. Try again later.");
					}
				}
				catch(const TA_Base_Core::EventAgentCorbaDef::EventIDNotFoundException  ex )
				{
					LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Agent out of sync!!! need to reload events from control agent.");
					bNeedReload = true;
					continue;
				}
				catch(const CORBA::SystemException& ex)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
					bFailure = true;
				}
				catch(TA_Base_Core::ObjectResolutionException&)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
					bFailure = true;
				}
				catch(TA_Base_Core::OperationModeException&)
				{
					LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't refresh event Cache or Queue from Control Event Agent");
					bFailure = true;
				}
				catch (const TA_Base_Core::TransactiveException& ex)
				{
					LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
						"Can't load Event Cache or Queue from Control Event Agent.	Caught TransactiveException: %s", ex.what());
					bFailure = true;
				}
				catch( const CORBA::Exception& ex)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
						"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
						"Can't refresh Event Cache or Queue from Control Event Agent. Caught Corba Exception");
					bFailure = true;
				}
				catch (...)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"Unknown exception caught: Can't refresh Event Cache or Queue from Control Event Agent. Caught General Exception" );
					bFailure = true;
				}

				if (bFailure && !m_bTerminate)
					TA_Base_Core::Thread::sleep(5000); // Sleep additional 5 seconds before trying again
			}

			TA_Base_Core::Thread::sleep(m_ulAgentRefreshInterval);
		}

		LOG0(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Agent Refresh process run exit.");
		FUNCTION_EXIT;
	}

	void DbEventRetriever::registerForDbRunParamChange()
	{
		FUNCTION_ENTRY("registerForDbRunParamChange");

		// Load the db connection strings
		std::string dbConnectionFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
		TA_Base_Core::DbConnectionStrings dbConnectionStrings(dbConnectionFile);

		try
		{
			std::vector<TA_Base_Core::DataConnection> dataConnections = dbConnectionStrings.getConnectionList(TA_Base_Core::Event_Ad, TA_Base_Core::Read);
			std::vector<TA_Base_Core::DataConnection>::iterator iter = dataConnections.begin();
			for(; iter != dataConnections.end(); ++iter)
			{
				std::string tempString = (*iter).first;

				std::ostringstream str;
				str << RPARAM_DBPREFIX << tempString;

				TA_Base_Core::RunParams::getInstance().registerRunParamUser( this, str.str().c_str() );
			}
		}
		catch(const TA_Base_Core::DbConnectionFailed& ex)
		{
			// If there was a failure getting the connection strings, this isn't catastrophic, but it 
			// means that we won't be able to be notified when a database goes down or up.
			LOG_EXCEPTION_CATCH(SourceInfo, "DbConnectionFailed", ex.what());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Cannot listen for database status changes");
		}

		FUNCTION_EXIT;
	}

	std::string DbEventRetriever::getCurrentDatabaseString()
	{
		FUNCTION_ENTRY("getCurrentDatabaseString");
		std::string strRetVal = "";
		try
		{
			std::string connectStr;
			TA_Base_Core::DbConnection::getInstance().getConnectionString(TA_Base_Core::Event_Ad, TA_Base_Core::Read, connectStr);

			std::vector<std::string> components;
			SimpleDb::ParseConnectStr(connectStr, components);

			strRetVal = components[1];
		}
		catch(const TA_Base_Core::DbConnectionFailed& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DbConnectionFailed", ex.what());
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unable to retrieve current connection string.");
		}
		catch(...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Unknown error occurred. Unable to retrieve current connection string.");
		}
		
		FUNCTION_EXIT;
		return strRetVal;
	}

#if !defined( _WIN32 )
	#define _stricmp TA_Base_Core::CStdPubUtility::stricmp
#endif

	void DbEventRetriever::onRunParamChange(const std::string& name, const std::string& value)
	{
		FUNCTION_ENTRY("onRunParamChange");
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received new RunParam (%s = %s)", name.c_str(), value.c_str());//TD14337 yezh++

		if (0 == RunParams::getInstance().get(RPARAM_OPERATIONMODE).compare(RPARAM_STANDBY))
		{
			return; //Ignore the update if agent is in standby mode
		}

		// We need to reload the list when either:
		// 1) The current database has gone down
		// 2) The primary database has come back up and we are currently using a secondary

		std::ostringstream str;
		str << RPARAM_DBPREFIX << m_currentDbString;
		std::string currentStr(str.str().c_str());
		if ( _stricmp(currentStr.c_str(), name.c_str()) == 0 )
		{
			if ( value == RPARAM_DBOFFLINE )
			{
				TA_Base_Core::ThreadGuard guard(m_bThreadLock);
				std::string strTempDbString;
				m_bDatabaseChanged = true;
				//Change database
				strTempDbString = getCurrentDatabaseString();
				if (!strTempDbString.empty())
				{
					m_currentDbString = strTempDbString;
				}
			}
		}

		FUNCTION_EXIT;
	}
}
