/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/alarm/alarm_store_library/src/AlarmAgentInterface.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/22 17:50:12 $
  * Last modified by:  $Author: Noel $
  *
  */
///////////////////////////////////////////////////////////
//  AlarmAgentInterface.cpp
//  Implementation of the Class AlarmAgentInterface
//  Created on:      24-Jan-2004 04:17:11 PM
///////////////////////////////////////////////////////////

#include "boost/shared_ptr.hpp"
#include "AlarmAgentInterface.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/types/AlarmUpdate_MessageTypes.h"
#include "core/message/types/Alarm_MessageTypes.h"
#include "core/message/types/VisualAlarms_MessageTypes.h"
#include "core/exceptions/src/TransactiveException.h"
#include "bus/alarm/shared_alarm/src/SharedAlarm.h"
//#include "SharedAlarmAccessFactory.h"
#include "AlarmAccessAdaptor.h"

#include "core/synchronisation/src/ProcessGuard.h"
#include "core/synchronisation/src/NonReEntrantProcessLockable.h"
#include "core/synchronisation/src/IProcessLockable.h"
#include <ctime>

using namespace I_ALARM_PROVIDER;
using namespace TA_Base_Bus;

/**
 * Constructor
 * @param callback    Callback used to notify new alarm details
 * @param alarmAgentEntityName
 * 
 */
AlarmAgentInterface::AlarmAgentInterface(IAlarmUpdate& callback, std::string entityName, 
                                         unsigned long myLocation, unsigned long agentLocation,bool alarmStoreStart)
	: m_callback(callback),
	  m_terminate(false),
	  m_entityName(entityName),
      m_myLocation(myLocation),
	  m_agentLocation(agentLocation),
	  m_subscribed(false),
	  m_reload(false),
	  m_notificationSequence(0),
      m_queueUpdates(false),
	  m_CctvTriggeringAlarmTypeKey(0),
	  m_CctvTriggeringAlarmAMSTypeKey(0),
	  m_CctvTriggeringAlarmIntercomTypeKey(0),
	  m_CctvTriggeringAlarmESPTypeKey(0),
	  m_CctvTriggeringAlarmPATTypeKey(0),
	  m_CctvTriggeringAlarmBLSTypeKey(0),//m_alarmMessageQueueProcess(NULL),
	  m_AlarmStoreStart(alarmStoreStart)
{
	// Get alarm agent entity for the specified location
 	FUNCTION_ENTRY("Constructor");
	// Connect to the alarm agent
	m_alarmAgent.setEntityName(m_entityName,true);
	m_visualAlarmContext = TA_Base_Core::VisualAlarms::Context.getContextName();

	std::stringstream stringStream;
	stringStream << TA_Base_Core::VisualAlarms::CctvTriggeringAlarm.getTypeKey();
	stringStream >>	m_CctvTriggeringAlarmTypeKey;
	stringStream.str("");

	stringStream << TA_Base_Core::VisualAlarms::CctvTriggeringAlarmAMS.getTypeKey();
	stringStream >>	m_CctvTriggeringAlarmAMSTypeKey;
	stringStream.str("");

	stringStream << TA_Base_Core::VisualAlarms::CctvTriggeringAlarmIntercom.getTypeKey();
	stringStream >>	m_CctvTriggeringAlarmIntercomTypeKey;
	stringStream.str("");

	stringStream << TA_Base_Core::VisualAlarms::CctvTriggeringAlarmESP.getTypeKey();
	stringStream >>	m_CctvTriggeringAlarmESPTypeKey;
	stringStream.str("");

	stringStream << TA_Base_Core::VisualAlarms::CctvTriggeringAlarmPAT.getTypeKey();
	stringStream >>	m_CctvTriggeringAlarmPATTypeKey;
	stringStream.str("");

	stringStream << TA_Base_Core::VisualAlarms::CctvTriggeringAlarmBLS.getTypeKey();
	stringStream >>	m_CctvTriggeringAlarmBLSTypeKey;
	stringStream.str("");
	FUNCTION_EXIT;
}

AlarmAgentInterface::~AlarmAgentInterface()
{
 	FUNCTION_ENTRY("Destructor");
	terminateAndWait();
	stopSubscription();
// 	if (m_alarmMessageQueueProcess!=NULL)
// 	{
// 		m_alarmMessageQueueProcess->terminateAndWait();
// 		delete m_alarmMessageQueueProcess;
// 		m_alarmMessageQueueProcess=NULL;
// 	}	
	
	FUNCTION_EXIT;
}

/**
 * receiveSpecialisedMessage  This is overriden by the client to receive their
 * specialise message
 * @param	T	The narrowed Message
 * @param message
 * 
 */
void AlarmAgentInterface::receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef& notificationMessage )
{
 	FUNCTION_ENTRY("receiveSpecialisedMessage");

	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received alarm notificationMessage %s",m_entityName.c_str() );

    // 4669-specific behaviour.
	if (notificationMessage._d() == TA_Base_Core::NewAlarm)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received alarm notificationMessage %s AlarmSeq=%ld",m_entityName.c_str(),notificationMessage.alarm().messageSequenceNumber );
		if ( shouldDiscardAlarm(notificationMessage.alarm()) )
		{
			return;
		}
		unsigned long locationKey = static_cast<unsigned long >(notificationMessage.alarm().locationKey);
		if (locationKey != m_agentLocation)
		{
			return;
		}
	}else if (notificationMessage._d() == TA_Base_Core::UpdateAlarm)
	{
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received alarm notificationMessage %s AlarmSeq=%ld",m_entityName.c_str(),notificationMessage.update().messageSequenceNumber );
		unsigned long locationKey = static_cast<unsigned long >(notificationMessage.update().locationKey);
		if (locationKey != m_agentLocation)
		{
			return;
		}
	}

    // Get the update queue lock.
    TA_THREADGUARD(m_updateQueueLock);

    // TD10773: If loading is in progress we want to queue the updates instead of waiting on a thread lock
    //          that can cause excessive thread counts if a lot of updates are received while the alarms
    //          are being loaded from the agent.
    if ( m_queueUpdates )
    {
        LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Received update while loading alarms, pushed onto update queue." );
        m_updateQueue.push( notificationMessage );
    }
    else
    {
        // Alarms aren't being loaded so we can process the alarm message directly.
         processUpdate( notificationMessage );
    }

    FUNCTION_EXIT;
}

void AlarmAgentInterface::processUpdate( const TA_Base_Core::AlarmMessageCorbaDef& notificationMessage )
{
    FUNCTION_ENTRY("processUpdate");

	if ( m_reload )
	{
		std::string info("");
		if (notificationMessage._d() == TA_Base_Core::NewAlarm)
		{
			info = "AlarmID=" + std::string(notificationMessage.alarm().alarmID.in());
		}
		else if (notificationMessage._d() == TA_Base_Core::UpdateAlarm)
		{
			info = "AlarmID=" + std::string(notificationMessage.update().alarmID.in());
		}
		else
		{
			info = "Is a NotificationMessage";
		}
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, " ============= Need to reload Alarms, return directly. [%s] ============", info.c_str());
		return;
	}
    


	switch ( notificationMessage._d() )
	{
		case TA_Base_Core::NewAlarm:	// New Alarm
			try
			{	
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "New alarm %s notification from %s", notificationMessage.alarm().alarmID.in() ,m_entityName.c_str());
				if ( isMessageInSequence(notificationMessage.alarm().messageSequenceNumber) )
				{
					m_callback.newAlarm(notificationMessage.alarm());

				}
			}

			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Alarm Store Manager %s failed to create new Alarm id %s. Caught TransactiveException: %s",m_entityName.c_str(), notificationMessage.alarm().alarmID.in() , ex.what());
				TA_ASSERT (false, "Transactive Exception: Failed to create new Alarm in Alarm Store Manager");
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to create new Alarm in Alarm Store Manager" );
				TA_ASSERT (false, "CORBA Exception: Failed to create new Alarm in Alarm Store Manager");
			}
			catch (...)
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "Exception handling new alarm notification, alarm id is %s from agent %s", notificationMessage.alarm().alarmID.in() ,m_entityName.c_str());
				TA_ASSERT (false, "Failed to create new Alarm in Alarm Store Manager");
			}
			break;

		case TA_Base_Core::UpdateAlarm: // Updated alarm
			try
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Alarm %s update notification from %s", notificationMessage.update().alarmID.in() ,m_entityName.c_str());
				if ( isMessageInSequence(notificationMessage.update().messageSequenceNumber) )
				{
                    // before updateAlarm, check if AlarmManager is busy loading				
                        m_callback.updateAlarm(notificationMessage.update());

			    }
			}

			catch (const TA_Base_Core::TransactiveException& ex)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                    "Alarm Store Manager %s failed to update Alarm id %s. Caught TransactiveException: %s",m_entityName.c_str(), notificationMessage.alarm().alarmID.in() , ex.what());
				TA_ASSERT (false, "Transactive Exception: Failed to update Alarm in Alarm Store Manager");
			}
			catch( const CORBA::Exception& ex)
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, 
					"CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str(), "Failed to update Alarm in Alarm Store Manager" );
				TA_ASSERT (false, "CORBA Exception: Failed to update Alarm in Alarm Store Manager");
			}
			catch (...)
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "Exception handling update alarm notification, alarm id is %s from agent %s", notificationMessage.alarm().alarmID.in() ,m_entityName.c_str());
				TA_ASSERT (false, "Failed to update Alarm in Alarm Store Manager");
			}
			break;

        case TA_Base_Core::AlarmsLoadedNotificationMessage: // Alarms loaded
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                    "Alarm agent %s has finished loading all alarms. Will refresh Alarm Store.",
                    m_entityName.c_str());

                // Reset the "reload" flag, causing all the alarms for this agent to be reloaded.
                reloadAlarms();
            }
            break;

		default:
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,"Received an unknown alarm message type");
	}
	FUNCTION_EXIT;
}


bool AlarmAgentInterface::isMessageInSequence(unsigned long sequenceNumber)
{
 	FUNCTION_ENTRY("isMessageInSequence");

	bool inSequence = true;

	// Check the sequence number
	if (m_notificationSequence==0)
	{
		// First message
		m_notificationSequence = sequenceNumber + 1;
	}
	else
	{
		if (m_notificationSequence!=sequenceNumber)
		{
			// Out of sequence !
            LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
				 "Alarm sequence number invalid, expected %ld, got %ld",m_notificationSequence , sequenceNumber);
			m_notificationSequence=sequenceNumber;  // Resync
            reloadAlarms();  // Better reload the alarms as this is a problem!
			inSequence = false;
		}

		if (m_notificationSequence==0xFFFFFFFF)
		{
			m_notificationSequence=1;
		}
		else
		{
			m_notificationSequence++;
		}
	}

	FUNCTION_EXIT;
	return inSequence;
}

/**
 * Define a run() method to do your work. This should block until terminate() is
 * called eg: CorbaUtil::GetInstance().Run() or m_dialog->DoModal()  NOTE: do not
 * call this method directly. It needs to be public so that it can be called from
 * the thread-spawning function
 */
/************************************************************************/
/* New Alarm Store                                                      */
/************************************************************************/
void AlarmAgentInterface::run()
{
 	FUNCTION_ENTRY("run");

	while (!m_terminate)
	{
		// At this point there should be no chance of any updates being received and therefore
		// no chance of concurrent threads accessing this object.
		
		// We are now going to start loading the alarms so updates will need to be queued.
		m_queueUpdates = true;
		
		// Start subscription
		startSubscription();
		
		// wait a moment to make sure alarm messages are subscribed before loading alarms 
		sleep(500); 

//		//add by luohuirong 30-10-2009 --to fix alarm manager stops updating
//		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "start alarm message queue process %s",m_entityName.c_str() );
//		if (m_alarmMessageQueueProcess!=NULL)
//		{
//			//clear the cache data
//			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "re-start alarm message queue process %s",m_entityName.c_str() );
//			m_alarmMessageQueueProcess->terminateAndWait();
//			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "finish re-start alarm message queue process %s",m_entityName.c_str() );
//			delete m_alarmMessageQueueProcess;
//			m_alarmMessageQueueProcess=NULL;
//		}
//		m_alarmMessageQueueProcess=new AlarmMessageQueue(m_callback,m_entityName);
//		m_alarmMessageQueueProcess->start();
//		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "start alarm message queue process %s",m_entityName.c_str() );
//     	//add by luohuirong 30-10-2009
		// Load current list of alarms
		loadCurrentAlarms();

        // TD10773: Process any updates that were received while the alarms were being loaded. If further
        //          updates are received while the queue is being processed then those threads will block
        //          until the queue has been cleared.
        {
			TA_THREADGUARD(m_updateQueueLock);
			m_notificationSequence=0;
			// Now process any updates that came through while the loading was taking place.
			while ( !m_updateQueue.empty() )
			{
				LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
					"Process alarm in queue for location %lu", m_agentLocation);
				processUpdate( m_updateQueue.front() );
				m_updateQueue.pop();
			}
			
			// Finally, loading has completed so any update can now be processed directly.
			m_queueUpdates = false;
        }

		// Sleep until required to terminate
		while (!m_terminate&&!m_reload)
		{
			//sleep(1000);
			sleep(200); // decrease the sleep interval to improve terminating speed.
		}

		if (m_reload && !m_terminate)
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, 
				 "Reloading alarms for location %lu",m_agentLocation);

			stopSubscription();
			sleep(1000);  // Let any queued notifications clear (should not be required)

			// reset reload flag ready for next time
			m_reload = false;
		}
	}

	stopSubscription();

	// Clear existing alarms from this location
	// m_callback.clearAlarms(m_agentLocation);  
	// no need to call clear here because alarms will be cleared from AlarmAgentInterfaceManager 
	// by calling directly to AlarmAccessAdaptor::removelocationsAlarms

	// Reset so start can be called again
	m_terminate = false;

	FUNCTION_EXIT;
}


/**
 * The terminate() method should cause run() to return. eg: CorbaUtil::
 * GetInstance().Shutdown() or EndDialog(m_dialog)  NOTE: probably not a good idea
 * to call this method directly - call terminateAndWait() instead
 */
void AlarmAgentInterface::terminate()
{
 	FUNCTION_ENTRY("terminate");
	m_terminate = true;
	FUNCTION_EXIT;
}


void AlarmAgentInterface::startSubscription()
{
 	FUNCTION_ENTRY("startSubscription");

    // TD10773: No need to lock the thread in here as any messages received before alarms have been
    //          loaded will be queued instead of being processed directly.

	if (!m_subscribed)
	{
		try
		{
			// Start subscription to changes
			TA_Base_Core::AlarmSubscriptionManager::subscribeToAlarmsFromLocation(
					 TA_Base_Core::Alarm::Context, // All alarms
					 this,
					 m_agentLocation ); // specified location

		    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Subscribe for alarms from location %lu",m_agentLocation);

			m_subscribed = true;
			m_notificationSequence = 0;  // Reset sequence checker
		}
		catch (...)
		{
		    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "Failed to subscribe for alarms from location %lu",m_agentLocation);
		}
	}

	FUNCTION_EXIT;
}

void AlarmAgentInterface::stopSubscription()
{
 	FUNCTION_ENTRY("stopSubscription");

    // TD10773: No need to lock the thread in here as the message library itself will ensure that
    //          subscribers are not removed while a message is being forwarded to all members of the
    //          subscriber list.

	if (m_subscribed)
	{
		try
		{
			// Stop subscription to changes
			TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages( this );
			m_subscribed = false;
			while ( !m_updateQueue.empty() )
			{		 
				m_updateQueue.pop();
			}
		    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Unsubscribed for alarms from location %lu",m_agentLocation);
		}
		catch (...)
		{
		    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugError, 
				 "Failed to unsubscribe for alarms from location %lu",m_agentLocation);
		}
	}

	FUNCTION_EXIT;
}


void AlarmAgentInterface::loadCurrentAlarms()
{
 	FUNCTION_ENTRY("loadCurrentAlarms");

	TA_Base_Core::AlarmSequence* alarmList=NULL;
	std::string             entityName;
    bool firstreload=true;
	bool loaded = false;
	int retryTimes = 3;
    // TD10773: No need to lock the thread in here as queueing of update messages that are recieved
    //          during loaded ensures that concurrent calls to the callback are not made.
	
	while (!loaded && !m_terminate && retryTimes-- > 0)
	{
		TA_Base_Core::ThreadGuard guard(m_loadLock);

		try
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Corba call to AlarmAgent to getAlarms, agentLocationKey=%lu %s" , m_agentLocation,m_entityName.c_str());
			// Get the current list of alarms
			CORBA_CALL_RETURN( alarmList,
			                   m_alarmAgent,
			                   getAlarms,
			                   () );
			if (alarmList==NULL)
            {
                TA_THROW(TA_Base_Core::TransactiveException("Alarm List retrieved from alarm store emtpy. Cannot continue."));
            }

			for (unsigned int iLoop=0;iLoop<alarmList->length();iLoop++)
			{
				(*alarmList)[iLoop].alarmDescription= CORBA::string_dup(
				 				TA_Base_Core::AlarmTypeTableAccessor::getInstance().getExpandedDescription(
				 				(*alarmList)[iLoop].messageTypeKey,  
				 				std::string((*alarmList)[iLoop].alarmParameterList.in())).c_str()
 								);
			}

			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "+++++++++++ allalarms +loadCurrentAlarms from myLocation %li, agentLocation %li, alarm count: %li",
				m_myLocation, m_agentLocation, alarmList->length() );
			/*------------------------------------- New Alarm Store -----------------------------------*/
			//TA_Base_Bus::TA_SharedAlarm cs;
			//cs.insertAlarm(alarmList);
// 			unsigned long rc = SharedAlarmAccessFactory::getInstance().removeLocationAlarms(m_agentLocation);
// 			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "remove %li alarms from sharedAlarm, locationKey=%d", rc, m_agentLocation);
			 
			 
			{		
				std::vector<bool> alarmVisibles;			 			
				{
					//lock prefile object, so the function point can work, otherwise the function point may invalided via updating AlarmStoreManager::setPreFilter				
					unsigned long startTime=time(NULL);
					TA_Base_Core::NonReEntrantProcessLockable DutyLock(DUTYCACHELOCK);  
 					TA_Base_Core::ProcessGuard   DutyGuard (DutyLock );					
					AlarmStoreManager& asManager = *dynamic_cast<AlarmStoreManager*>(&m_callback);
					bool isFirstTimeout=true;
					for (unsigned int i=0; i < alarmList->length(); ++i)
					{
						bool isPassPreFilter=true;
						unsigned long currTime=time(NULL);
						if (currTime-startTime<200)
						{						 
							try{
								isPassPreFilter= asManager.passesPrefilter((*alarmList)[i]);
							}
							catch( const std::exception& e )
							{
								LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, typeid(e).name(), e.what(), "loadCurrentAlarms passesprefilter error" );							 
							}
							catch(...)
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "loadCurrentAlarms passesPrefilter error i=%d", i);
							}
						}
						else
						{
							if (isFirstTimeout)
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "loadCurrentAlarms time out loc=%s", m_entityName.c_str());
								isFirstTimeout=false;
							}
						}
						alarmVisibles.push_back(isPassPreFilter);
						//LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Insert alarm to alarm store, isPassPreFilter=%d",   (int)isPassPreFilter);				
					}
					AlarmAccessAdaptor::getInstance().refreshAlarmbyAgent(m_agentLocation, alarmList, alarmVisibles);
				}
//				SharedAlarmAccessFactory::getInstance().insertAlarm(alarmList, alarmVisibles);
				delete alarmList;
				alarmList = NULL;
			}
			loaded = true;
			/*---------------------------------------------------------------------------------------*/
			/*
			// Clear current alarms in store
			m_callback.clearAlarms(m_agentLocation);

			// Pass each alarm to the store
			for (unsigned int iLoop=0;iLoop<alarmList->length() && !m_terminate && !m_reload;iLoop++)
			{
                try
                {
                    LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
				     "Loading alarm %s from %s", (*alarmList)[iLoop].alarmID.in() ,m_entityName.c_str());

                    // 4669-specific behaviour.
                    if ( !shouldDiscardAlarm((*alarmList)[iLoop]) )
                    {
                        m_callback.newAlarm((*alarmList)[iLoop]);
                    }
                }
                catch(...)
                {
                    // Put a catch all at this level for saftey's sake...
                    LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugError,"An error occurred while adding alarm with ID %s - skipping",
                        (*alarmList)[iLoop].alarmID.in() );

                    // And continue through the loop
                }
			}

			// Release the corba message
			delete alarmList;
			alarmList = NULL;*/

			// Enable exit from function
			if (m_terminate)
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Load stopped prematurely by terminate request %s", m_entityName.c_str());
				break;
			}
			else if (m_reload)
			{
                LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "Load stopped prematurely by reload request", m_entityName.c_str());
				break;
			}
			else
			{
				loaded = true;
			}
			
		}// end of try
		catch( const TA_Base_Core::ObjectResolutionException &e )
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "TA_Base_Core::ObjectResolutionException", e.what(), "Failed to load list of alarms from the store" ); 
		    // Wait a while before retrying		
			if (firstreload)
			{
//				m_callback.informCallbackCompleteLoading();
				firstreload=false;
			}
			for (int n=0;n<150 && m_terminate==false;n++)
			{
				Sleep(200);
			}			
			TA_THREADGUARD(m_updateQueueLock);			 
			while ( !m_updateQueue.empty() )
			{				 
				m_updateQueue.pop();
			}
		}
		catch( const std::exception& e )
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, typeid(e).name(), e.what(), "Failed to load list of alarms from the store" );

		    // Wait a while before retrying
//			m_callback.informCallbackCompleteLoading();
			return;
		    //sleep(10000);
		}
		catch( const CORBA::Exception& e )
		{
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str(), "Failed to load list of alarms from the store" );
//			m_callback.informCallbackCompleteLoading();
			return;
		    // Wait a while before retrying
		    //sleep(10000);
		}
		catch( ... )
		{
			LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionDetails, "Unknown", "Failed to load list of alarms from the store" );
//			m_callback.informCallbackCompleteLoading();
			return;
		    // Wait a while before retrying
		    //sleep(10000);
		}
		/*
		if( NULL != alarmList )
		{
			// Release the corba message
			delete alarmList;
			alarmList = NULL;
		
		}*/
		
	}// end of while loop
	
    if (loaded)
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "All alarms loaded from store");
	}
	else
	{
		LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, 
				 "All alarms loaded from store failed and try to reload");
		for (int n=0;n<100 && m_terminate==false;n++)
		{
			Sleep(300);
		}
		m_reload=true;
	}
	
//	m_callback.informCallbackCompleteLoading(); // inform callback that it has completed the loading

	FUNCTION_EXIT;
}

/** 
  * AlarmAgentInterface::reloadAlarms
  *
  * This method will reload the alarms from the agent.
  *
  */
void AlarmAgentInterface::reloadAlarms()
{
 	FUNCTION_ENTRY("reloadAlarms");
	// Thread must now reload all the alarms
	m_reload = true;

	FUNCTION_EXIT;
}


bool AlarmAgentInterface::shouldDiscardAlarm(const TA_Base_Core::AlarmDetailCorbaDef& alarm)
{
	// Discard (ie. do nothing) if the notification message is related to
	// Context: Visual Alarm, Message Type: CctvTriggeringAlarm AND message is Not from the current Location
	// This is an exception requirement to rectify issue raised in TD 8466

	//TD17994,jianghp
	std::string messageContext ( alarm.messageContext.in() );
	//std::string messageTypeKey ( alarm.messageTypeKey.in() );
	unsigned long locationKey = static_cast<unsigned long >(alarm.locationKey);
	//TD17994,jianghp
	
	//TD16164++
	//Add in new alarm types for cctv triggering events
	/*
    if ( messageContext.compare(TA_Base_Core::VisualAlarms::Context.getContextName()) == 0 && 
        (messageTypeKey.compare(TA_Base_Core::VisualAlarms::CctvTriggeringAlarm.getTypeKey()) == 0 ||
		 messageTypeKey.compare(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmAMS.getTypeKey()) == 0 ||
		 messageTypeKey.compare(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmIntercom.getTypeKey()) == 0 ||
		 messageTypeKey.compare(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmESP.getTypeKey()) == 0 ||
		 messageTypeKey.compare(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmPAT.getTypeKey()) == 0 ||
		 messageTypeKey.compare(TA_Base_Core::VisualAlarms::CctvTriggeringAlarmBLS.getTypeKey()) == 0) &&
		locationKey != m_myLocation )
	{
	//++TD16164
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"Received alarm message of Context: %s, Type: %s, from location id: %lu will not be registered in this location id: %lu", 
			messageContext.c_str(), messageTypeKey.c_str(), locationKey, m_myLocation);
		return true;
	} 
	*/
	if ( messageContext.compare(m_visualAlarmContext) == 0 && 
        ( alarm.messageTypeKey == m_CctvTriggeringAlarmTypeKey ||
		 alarm.messageTypeKey == m_CctvTriggeringAlarmAMSTypeKey ||
		 alarm.messageTypeKey == m_CctvTriggeringAlarmIntercomTypeKey ||
		 alarm.messageTypeKey == m_CctvTriggeringAlarmESPTypeKey ||
		 alarm.messageTypeKey == m_CctvTriggeringAlarmPATTypeKey ||
		 alarm.messageTypeKey == m_CctvTriggeringAlarmBLSTypeKey) &&
		locationKey != m_myLocation )
	{
	//++TD16164
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
			"Received alarm message of Context: %s, Type: %u, from location id: %lu will not be registered in this location id: %lu", 
			messageContext.c_str(), alarm.messageTypeKey, locationKey, m_myLocation);
		return true;
	}   
    
    return false;
}
