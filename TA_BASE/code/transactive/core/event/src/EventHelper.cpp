/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/core/event/src/EventHelper.cpp $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/06/21 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#include "core/event/src/EventHelper.h"
#include "core/event/src/EventHelperException.h"
#include "core/event/src/EventUtilities.h"
#include "core/event/IDL/src/EventDetailCorbaDef.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EventAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#define DEFAULT_EV_SUBMIT_TIMEOUT 2
#define DEFAULT_EV_QUERY_TIMEOUT 20
#define DEFAULT_EV_CHECK_TIMEOUT 1

namespace TA_Base_Core
{
	EventHelper* EventHelper::m_pInstance = 0;
	ReEntrantThreadLockable EventHelper::m_singletonLock; 

	// Get Instance
	EventHelper& EventHelper::getInstance()
	{
		if (0 != m_pInstance)
			return *m_pInstance;

		ThreadGuard guard(m_singletonLock);
		if (0 == m_pInstance)
		{
			m_pInstance = new EventHelper();
		}

		return *m_pInstance;
	}

	// Release Instance
	void EventHelper::releaseInstance()
	{
		ThreadGuard guard(m_singletonLock);
		if (0 != m_pInstance)
		{
			delete m_pInstance;
			m_pInstance=0;
		}
	}

	EventHelper::EventHelper()
		:m_ulMyLocation(0)
		,m_SubmitTimeout(DEFAULT_EV_SUBMIT_TIMEOUT)
		,m_QueryTimeout(DEFAULT_EV_QUERY_TIMEOUT)
		,m_CheckTimeout(DEFAULT_EV_CHECK_TIMEOUT)
	{
		FUNCTION_ENTRY("Constructor");
		//Empty
		FUNCTION_EXIT;
	}

	void EventHelper::init(unsigned long _agentLocation)
	{
		FUNCTION_ENTRY("Init");
		m_ulMyLocation = _agentLocation;

		try
		{
			// Set Event Agent 
			std::vector<std::string> vecAgentNames = EntityAccessFactory::getInstance().getAgentNamesOfTypeAtLocation(
				EventAgentEntityData::getStaticType(),
				m_ulMyLocation
				);

			if (vecAgentNames.size() > 1)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "There is more than one Event agent servant at location %li", m_ulMyLocation );
				return;
			}
			else if (vecAgentNames.size() == 0)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No Event agent servant configured at this location %li", m_ulMyLocation );
				return;
			}

			std::string strEventAgent = vecAgentNames[0];
			m_eventAgentNameObj.setEntityName(strEventAgent, true);

		}
		catch (TA_Base_Core::DatabaseException& ex)
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", ex.what());
		}
		catch ( TA_Base_Core::DataException& ex )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );
		}
		catch (...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "getAgentNamesOfTypeAtLocation() failed.");
		}
		
		FUNCTION_EXIT;
	}


	// Push events
	void EventHelper::submitEvent(TA_Base_Core::EventItemInfo* event)
	{
		FUNCTION_ENTRY("submitEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		try
		{
			EventInfoCorbaDef eventDetail;
			EventUtilities::EventInfoToCorbaDef(event, eventDetail);
			
			//Set call timeout
			m_eventAgentNameObj.setObjectTimeout(m_SubmitTimeout);
			//Do the actual corba call here
			CORBA_CALL(m_eventAgentNameObj, 
				submitEvent, 
				(eventDetail));
		}
		catch(CORBA::TRANSIENT &timeout)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TRANSIENT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(timeout).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_TIMEOUT));
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_SYSTEM_EX));
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OBJECT_RES));
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't submit events to Event Agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OPERATION_MODE));
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Can't submit events to Event Agent. Caught TransactiveException: %s", ex.what());
			TA_THROW(EventHelperException(EventHelperException::ERR_TRANSACTIVE_EX));
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Can't submit events to Event Agent. Caught Corba Exception");
			TA_THROW(EventHelperException(EventHelperException::ERR_EXCEPTION));
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Can't submit events to Event Agent. Caught General Exception" );
			TA_THROW(EventHelperException(EventHelperException::ERR_UNKNOWN));
		}

		FUNCTION_EXIT;
	}

	void EventHelper::submitEvents(TA_Base_Core::T_EventItemInfoList& events)
	{
		FUNCTION_ENTRY("submitEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		try
		{
			EventInfoSequence eventSeq;
			eventSeq.length(events.size());
			//Convert to CorbaObjects
			unsigned long ulIdx=0;
			T_EventItemInfoList::const_iterator itr = events.begin();
			for(itr; itr!=events.end(); ++itr)
			{
				EventItemInfo* pEvSource = (*itr);
				EventUtilities::EventInfoToCorbaDef(pEvSource, eventSeq[ulIdx]);

				ulIdx++;
			}

			//Set call timeout
			m_eventAgentNameObj.setObjectTimeout(m_SubmitTimeout);
			//Do the actual corba call here
			CORBA_CALL(m_eventAgentNameObj, 
				submitEvents, 
				(eventSeq));
		}
		catch(CORBA::TRANSIENT &timeout)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TRANSIENT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(timeout).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_TIMEOUT));
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_SYSTEM_EX));
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OBJECT_RES));
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't submit events to Event Agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OPERATION_MODE));
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Can't submit events to Event Agent. Caught TransactiveException: %s", ex.what());
			TA_THROW(EventHelperException(EventHelperException::ERR_TRANSACTIVE_EX));
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Can't submit events to Event Agent. Caught Corba Exception");
			TA_THROW(EventHelperException(EventHelperException::ERR_EXCEPTION));
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Can't submit events to Event Agent. Caught General Exception" );
			TA_THROW(EventHelperException(EventHelperException::ERR_UNKNOWN));
		}

		FUNCTION_EXIT;
	}

	// Retrieve all events
	void EventHelper::getAllEvents(std::string sessionID, TA_Base_Core::T_EventItemList& eventList)
	{
		FUNCTION_ENTRY("getAllEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		std::auto_ptr<TA_Base_Core::EvDataPkgCorbaDef> eventPkg;
		try
		{
			TA_Base_Core::EvDataPkgCorbaDef* pTmpEventData = NULL;

			//Set call timeout
			m_eventAgentNameObj.setObjectTimeout(m_QueryTimeout);
			//Do the actual corba call
			CORBA_CALL_RETURN( pTmpEventData,
				(m_eventAgentNameObj),
				getAllEvents,
				(CORBA::string_dup("Super Session ID")) );

			eventPkg.reset(pTmpEventData);				
		
		}
		catch(const TA_Base_Core::EventAgentCorbaDef::EventIDNotFoundException  ex )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Event ID not found!");
			TA_THROW(EventHelperException(EventHelperException::ERR_ID_NOT_FOUND))
		}
		catch(CORBA::TRANSIENT &timeout)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TRANSIENT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(timeout).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_TIMEOUT));
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_SYSTEM_EX));
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OBJECT_RES));
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't load Event Cache or Queue from Control Event Agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OPERATION_MODE));
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Can't load Event Cache or Queue from Control Event Agent.	Caught TransactiveException: %s", ex.what());
			TA_THROW(EventHelperException(EventHelperException::ERR_TRANSACTIVE_EX));
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Can't load Event Cache or Queue from Control Event Agent. Caught Corba Exception");
			TA_THROW(EventHelperException(EventHelperException::ERR_EXCEPTION));
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Can't load Event Cache or Queue from Control Event Agent. Caught General Exception" );
			TA_THROW(EventHelperException(EventHelperException::ERR_UNKNOWN));
		}

		//De serialize Data
		EventUtilities::UnpackPackage(eventPkg.get(), eventList);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getAllEvents: Retrieve Events from agent size = %d", eventList.size());
		FUNCTION_EXIT;
	}

	// Retrieve the latest events
	void EventHelper::getLatestEvents(std::string eventID, std::string sessionID, TA_Base_Core::T_EventItemList& eventList)
	{
		FUNCTION_ENTRY("getLatestEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		std::auto_ptr<TA_Base_Core::EvDataPkgCorbaDef> eventPkg;

		try
		{
			
			TA_Base_Core::EvDataPkgCorbaDef* pTmpEventData = NULL;

			//Set call timeout
			m_eventAgentNameObj.setObjectTimeout(m_QueryTimeout);
			// do actual corba call
			CORBA_CALL_RETURN( pTmpEventData,
				(m_eventAgentNameObj),
				getLatestEvents,
				(CORBA::string_dup(eventID.c_str()),
				CORBA::string_dup("Super Session ID")) );

			eventPkg.reset(pTmpEventData);				
			
		}
		catch(const TA_Base_Core::EventAgentCorbaDef::EventIDNotFoundException  ex )
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Event ID not found!!! need to reload events from control agent.");
			TA_THROW(EventHelperException(EventHelperException::ERR_ID_NOT_FOUND))
		}
		catch(CORBA::TRANSIENT &timeout)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TRANSIENT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(timeout).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_TIMEOUT));
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_SYSTEM_EX));
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OBJECT_RES));
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't refresh event Cache or Queue from Control Event Agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OPERATION_MODE));
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Can't load Event Cache or Queue from Control Event Agent.	Caught TransactiveException: %s", ex.what());
			TA_THROW(EventHelperException(EventHelperException::ERR_TRANSACTIVE_EX));
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Can't refresh Event Cache or Queue from Control Event Agent. Caught Corba Exception");
			TA_THROW(EventHelperException(EventHelperException::ERR_EXCEPTION));
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Can't refresh Event Cache or Queue from Control Event Agent. Caught General Exception" );
			TA_THROW(EventHelperException(EventHelperException::ERR_UNKNOWN));
		}

		//De serialize Data
		EventUtilities::UnpackPackage(eventPkg.get(), eventList);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getLatestEvents: Retrieve Events from agent size = %d", eventList.size());

		FUNCTION_EXIT;
	}

	// Retrieves the latest N events;
	void EventHelper::getTopEvents(unsigned long ulEventSize, std::string sessionID, TA_Base_Core::T_EventItemList& eventList)
	{
		FUNCTION_ENTRY("getTopEvents");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		std::auto_ptr<TA_Base_Core::EvDataPkgCorbaDef> eventPkg;

		try
		{

			TA_Base_Core::EvDataPkgCorbaDef* pTmpEventData = NULL;

			//Set call timeout
			m_eventAgentNameObj.setObjectTimeout(m_QueryTimeout);
			// do actual corba call
			CORBA_CALL_RETURN( pTmpEventData,
				(m_eventAgentNameObj),
				getTopEvents,
				(ulEventSize,
				CORBA::string_dup("Super Session ID")) );
			
			eventPkg.reset(pTmpEventData);				

		}
		catch(CORBA::TRANSIENT &timeout)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TRANSIENT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(timeout).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_TIMEOUT));
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_SYSTEM_EX));
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OBJECT_RES));
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Can't refresh event Cache or Queue from Control Event Agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OPERATION_MODE));
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Can't load Event Cache or Queue from Control Event Agent.	Caught TransactiveException: %s", ex.what());
			TA_THROW(EventHelperException(EventHelperException::ERR_TRANSACTIVE_EX));
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Can't refresh Event Cache or Queue from Control Event Agent. Caught Corba Exception");
			TA_THROW(EventHelperException(EventHelperException::ERR_EXCEPTION));
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Can't refresh Event Cache or Queue from Control Event Agent. Caught General Exception" );
			TA_THROW(EventHelperException(EventHelperException::ERR_UNKNOWN));
		}

		//De serialize Data
		EventUtilities::UnpackPackage(eventPkg.get(), eventList);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "getTopEvents: Retrieve Events from agent size = %d", eventList.size());

		FUNCTION_EXIT;
	}

	// Check event status
	bool EventHelper::checkStatus()
	{
		FUNCTION_ENTRY("checkStatus");
		TA_Base_Core::ThreadGuard guard(m_threadLock);
		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		bool bRetVal = false;
		try
		{	
			//Set call timeout
			m_eventAgentNameObj.setObjectTimeout(m_CheckTimeout);
			// do actual corba call
			CORBA_CALL_RETURN( bRetVal,
				(m_eventAgentNameObj),
				checkStatus,());
					
		}
		catch(CORBA::TRANSIENT &timeout)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::TRANSIENT", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(timeout).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_TIMEOUT));
		}
		catch(const CORBA::SystemException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
			TA_THROW(EventHelperException(EventHelperException::ERR_SYSTEM_EX));
		}
		catch(TA_Base_Core::ObjectResolutionException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve the Event agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OBJECT_RES));
		}
		catch(TA_Base_Core::OperationModeException&)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", "Error while trying to contact Event Agent");
			TA_THROW(EventHelperException(EventHelperException::ERR_OPERATION_MODE));
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG1(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
				"Error while trying to contact Event Agent.	Caught TransactiveException: %s", ex.what());
			TA_THROW(EventHelperException(EventHelperException::ERR_TRANSACTIVE_EX));
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Error while trying to contact Event Agent. Caught Corba Exception");
			TA_THROW(EventHelperException(EventHelperException::ERR_EXCEPTION));
		}
		catch (...)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				"Unknown exception caught: Error while trying to contact Event Agent. Caught General Exception" );
			TA_THROW(EventHelperException(EventHelperException::ERR_UNKNOWN));
		}
		FUNCTION_EXIT;

		return bRetVal;
	}

	void EventHelper::setAgentCallTimeout(unsigned long ulTimeout)
	{
		FUNCTION_ENTRY("setAgentCallTimeout");
		TA_ASSERT(ulTimeout<=300, ""); 

		if (m_eventAgentNameObj.getAgentName().empty())
			TA_THROW(EventHelperException(EventHelperException::ERR_INIT_AGENT));

		// adjust corba timeout.
		m_eventAgentNameObj.setObjectTimeout(ulTimeout);	

		FUNCTION_EXIT;
	}
}