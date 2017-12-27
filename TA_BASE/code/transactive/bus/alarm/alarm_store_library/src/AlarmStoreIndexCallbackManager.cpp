/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmStoreIndexCallbackManager.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class managers the list of callbacks.  It has a background
  * thread which checks call backs are still ok.
  *
  */

///////////////////////////////////////////////////////////
//  AlarmStoreIndexCallbackManager.cpp
//  Implementation of the Class AlarmStoreIndexCallbackManager
//  Created on:      24-Jan-2004 04:17:28 PM
///////////////////////////////////////////////////////////

#include "AlarmStoreIndexCallbackManager.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "AlarmStoreException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/corba/src/CorbaUtil.h"

#include <time.h>
using namespace TA_Base_Bus;


AlarmStoreIndexCallbackManager::AlarmStoreIndexCallbackManager(IAlarmStoreIndexDelete& deleteCallback)
								: m_deleteCallback(deleteCallback),
								  m_terminate(false)
{
	FUNCTION_ENTRY("Constructor");
	time_t now;
	time( &now );
	m_nextCallbackID = (unsigned long)now;
	start();
	FUNCTION_EXIT;
}



AlarmStoreIndexCallbackManager::~AlarmStoreIndexCallbackManager()
{
	FUNCTION_ENTRY("Destructor");

	// Free up all the call back pointers
	TA_Base_Core::ThreadGuard guard(m_callbackLock);
	CorbaCallbackList::iterator it = m_corbaCallbackList.begin();
	while (it != m_corbaCallbackList.end())
	{
		TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr  callback;
		callback = it->second;
		if (CORBA::is_nil(callback))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
			return;
		}
		CORBA::release(callback);
		it++;
	}

	m_corbaCallbackList.clear();
	FUNCTION_EXIT;
}


/**
 * Add a new callback for this filter.  Returns the unique id for the callback.
 * The alarm store will update the indexID to be associated with the callbackID
 * once the index has been identified or created.
 */
unsigned long AlarmStoreIndexCallbackManager::addCallback(TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback)
{
	FUNCTION_ENTRY("addCallback");

	// Protect from other updates
	TA_Base_Core::ThreadGuard guard(m_callbackLock);

	if (CORBA::is_nil(callback))
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
		FUNCTION_EXIT;
		return m_nextCallbackID;
	}
	
	long callbackID = m_nextCallbackID;
	m_nextCallbackID++;

	// Make a copy of the pointer or it will not work properly!
	// Without the copy it does not work, invalid call back on first createIndex call but subsequent ones
	// work ok, which is the opposite to what we expected!
	m_corbaCallbackList[callbackID] = TA_Base_Core::IAlarmStoreCallbackCorbaDef::_duplicate(callback);

	FUNCTION_EXIT;
    return callbackID;
}


/**
 * Returns the callbackID associated with a callback pointer from Corba.
 */
unsigned long AlarmStoreIndexCallbackManager::getCallback(TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback)
{
	FUNCTION_ENTRY("getCallback");
	// Protect from other updates
	TA_Base_Core::ThreadGuard guard(m_callbackLock);
	
	if (CORBA::is_nil(callback))
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
		FUNCTION_EXIT;
		return 0;
	}
	

	CorbaCallbackList::iterator it = m_corbaCallbackList.begin();
	while (it != m_corbaCallbackList.end())
	{
		if (it->second == callback)
		{
			FUNCTION_EXIT;
			return it->first;
		}
		it++;
	}

	throw AlarmStoreIndexNotFoundException();
	FUNCTION_EXIT;
	return 0;
}

/**
 * Remove a callback for this filter
 * @param callbackID    ID for callback to be removed.
 * 
 */
void AlarmStoreIndexCallbackManager::removeCallback(unsigned long callbackID)
{
	FUNCTION_ENTRY("removeCallback");

	// Protect from other updates
	TA_Base_Core::ThreadGuard guard(m_callbackLock);

	CorbaCallbackList::iterator it = m_corbaCallbackList.find(callbackID);
	if (it!=m_corbaCallbackList.end())
	{
		// Release the copy of the call back pointer
		if (CORBA::is_nil(it->second))
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
			FUNCTION_EXIT;
			return;
		}
		CORBA::release(it->second);
		m_corbaCallbackList.erase(it);
	}
	FUNCTION_EXIT;
}


/**
 * @param callbackID    Individual callback to be notified
 * @param indexID    Identity of index being updated
 * 
 */
void AlarmStoreIndexCallbackManager::indexCleared(unsigned long callbackID, long indexID)
{
	FUNCTION_ENTRY("indexCleared");
	TA_Base_Core::ThreadGuard guard(m_callbackLock);

	// Get the CORBA pointer to call back on
	CorbaCallbackList::iterator it = m_corbaCallbackList.find(callbackID);
	if (it!=m_corbaCallbackList.end())
	{
		try
		{
			TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback = it->second;
			if (CORBA::is_nil(callback))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
				FUNCTION_EXIT;
				return;
			}
			callback->indexCleared();
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Alarm Store Index Callback Manager failed to clear index id %ul for callback id %ul. Caught TransactiveException: %s", 
				indexID, callbackID, ex.what());
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Alarm Store Index Callback Manager failed to clear index id %ul for callback id %ul. Caught Corba Exception",
				indexID, callbackID );
		}
		catch (...)
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 "Unknown exception caught: Alarm Store Index Callback Manager failed to clear index id %ul for callback id %ul. Caught General Exception", 
			 indexID, callbackID );
		}
	}

	FUNCTION_EXIT;
}


/**
 * Index Moved
 * @param callbackID    Individual callback to be notified
 * @param indexID
 * @param position    Position in index
 * @param alarmID    Alarm UUID
 * 
 */
void AlarmStoreIndexCallbackManager::indexMoved(unsigned long callbackID, long indexID, unsigned long oldPosition, 
												unsigned long newPosition, std::string alarmID)
{
	FUNCTION_ENTRY("indexMoved");

	TA_Base_Core::ThreadGuard guard(m_callbackLock);
	// Get the CORBA pointer to call back on
	CorbaCallbackList::iterator it = m_corbaCallbackList.find(callbackID);
	if (it!=m_corbaCallbackList.end())
	{
		try
		{
			TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback = it->second;
			if (CORBA::is_nil(callback))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
				FUNCTION_EXIT;
				return;
			}
			callback->itemMoved(alarmID.c_str(),oldPosition,newPosition);
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Alarm Store Index Callback Manager failed to move Alarm id %s from old position %d to new position %d for callback id: %ul.	Caught TransactiveException: %s", 
				alarmID.c_str(), oldPosition, newPosition, callbackID, ex.what());
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Alarm Store Index Callback Manager failed to move Alarm id %s from old position %d to new position %d for callback id: %ul. Caught Corba Exception",
				alarmID.c_str(), oldPosition, newPosition, callbackID );
		}
		catch (...)
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 "Unknown exception caught: Alarm Store Index Callback Manager failed to move Alarm id %s from old position %d to new position %d for callback id: %ul. Caught General Exception", 
			 alarmID.c_str(), oldPosition, newPosition, callbackID);
		}
	}

	FUNCTION_EXIT;
}

/**
 * Index deleted
 * @param callbackID    Individual callback to be notified
 * @param indexID
 * @param position    Position in index
 * @param alarmID    Alarm UUID
 * 
 */
void AlarmStoreIndexCallbackManager::indexDeleted(unsigned long callbackID, long indexID, unsigned long position, 
												   std::string alarmID)
{
	FUNCTION_ENTRY("indexDeleted");

	// Get the CORBA pointer to call back on
	TA_Base_Core::ThreadGuard guard(m_callbackLock);
	CorbaCallbackList::iterator it = m_corbaCallbackList.find(callbackID);
	if (it!=m_corbaCallbackList.end())
	{
		try
		{
			TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback = it->second;
			if (CORBA::is_nil(callback))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
				FUNCTION_EXIT;
				return;
			}
			callback->itemDeleted(alarmID.c_str(),position);
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Alarm Store Index Callback Manager failed to delete Alarm id %s from position %d of index id %ul for callback id: %ul.	Caught TransactiveException: %s", 
				alarmID.c_str(), position, indexID, callbackID, ex.what());
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Alarm Store Index Callback Manager failed to delete Alarm id %s from position %d of index id %ul for callback id: %ul. Caught Corba Exception",
				alarmID.c_str(), position, indexID, callbackID );
		}
		catch (...)
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 "Unknown exception caught: Alarm Store Index Callback Manager failed to delete Alarm id %s from position %d of index id %ul for callback id: %ul. Caught General Exception", 
			 alarmID.c_str(), position, indexID, callbackID );
		}
	}

	FUNCTION_EXIT;
}
/**
 * Called when an index entry is inserted
 * @param callbackID    Individual callback to be notified
 * @param indexID
 * @param position    Position in index where alarm inserted
 * @param alarmID
 * 
 */
void AlarmStoreIndexCallbackManager::indexInserted(unsigned long callbackID, long indexID, unsigned long position, 
												   std::string alarmID)
{
	FUNCTION_ENTRY("indexInserted");

	// Get the CORBA pointer to call back on
	TA_Base_Core::ThreadGuard guard(m_callbackLock);
	CorbaCallbackList::iterator it = m_corbaCallbackList.find(callbackID);
	if (it!=m_corbaCallbackList.end())
	{
		try
		{
			TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback = it->second;
			if (CORBA::is_nil(callback))
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
				FUNCTION_EXIT;
				return;
			}
			callback->itemInserted(alarmID.c_str(),position);
		}
		catch (const TA_Base_Core::TransactiveException& ex)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                "Alarm Store Index Callback Manager failed to insert Alarm id %s from position %d of index id %ul for callback id: %ul.	Caught TransactiveException: %s", 
				alarmID.c_str(), position, indexID, callbackID, ex.what());
		}
		catch( const CORBA::Exception& ex)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
				"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), 
				"Alarm Store Index Callback Manager failed to insert Alarm id %s from position %d of index id %ul for callback id: %ul. Caught Corba Exception",
				alarmID.c_str(), position, indexID, callbackID );
		}
		catch (...)
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
			 "Unknown exception caught: Alarm Store Index Callback Manager failed to insert Alarm id %s from position %d of index id %ul for callback id: %ul. Caught General Exception", 
			 alarmID.c_str(), position, indexID, callbackID );
		}
	}

	FUNCTION_EXIT;
}


/**
 * The terminate() method should cause run() to return. eg: CorbaUtil::
 * GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
 * to call this method directly - call terminateAndWait() instead
 */
void AlarmStoreIndexCallbackManager::terminate()
{
 	FUNCTION_ENTRY("terminate");
	m_terminate = true;
	FUNCTION_EXIT;
}


/**
 * Define a run() method to do your work. This should block until terminate() is
 * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
 * call this method directly. It needs to be public so that it can be called from
 * the thread-spawning function
 */
void AlarmStoreIndexCallbackManager::run()
{
 	FUNCTION_ENTRY("run");

	while (!m_terminate)
	{
		// Check callbacks every 10 seconds - but check terminate every second
		for (unsigned int iLoop=0;iLoop<10;iLoop++)
		{
			if (m_terminate) break;
			sleep(1000);
		}

		// Test all the callbacks
		bool retry;
		do
		{
			retry = false;

			TA_Base_Core::ThreadGuard guard(m_callbackLock);
			CorbaCallbackList::iterator it = m_corbaCallbackList.begin();

			while (it != m_corbaCallbackList.end()&&!m_terminate)
			{
				try
				{
					TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback = it->second;
					if (CORBA::is_nil(callback))
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"the TA_Base_Core::IAlarmStoreCallbackCorbaDef_ptr callback is nil!");
						continue;
					}
					if (callback->callbackCheck())
					{
						// Ok, so continue checking next callback entry
						it++;
						continue;
					}
				}
				catch(...)
				{
					LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
						"caught an exception in it->second->callbackCheck()");
				}

				// Must have been a problem to get here (exception or false callback check)
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
					 "Removing index following callback check.");
				
				guard.release();
				m_deleteCallback.deleteIndex(it->first);
				guard.acquire();
				retry = true;
				break;
			}
		}
		while (retry);
	}

	FUNCTION_EXIT;
}




		
