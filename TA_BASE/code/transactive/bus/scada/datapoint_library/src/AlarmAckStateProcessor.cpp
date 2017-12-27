/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:		$File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/AlarmAckStateProcessor.cpp $
  * @author:	Weimin
  * @version $Revision: #2 $
  *
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  * AlarmAckStateProcessor provides a single interface to subscribe and unsubscribe for alarm
  * acknowledgemnt from an alarm entity.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#pragma warning(disable:4250)
#endif // defined (_MSC_VER)

#include <sstream>

#include "bus/scada/datapoint_library/src/IAlarmAckStateUser.h"
#include "bus/scada/datapoint_library/src/AlarmAckStateProcessor.h"
#include "bus/scada/datapoint_library/src/LFThreadPoolSingleton.h"

#include "core/alarm/IDL/src/AlarmAgentCorbaDef.h"
#include "core/alarm/src/AlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/alarm/src/AlarmSubscriptionManager.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/message/types/MessageTypes.h"
#include "core/message/types/DataPointAlarms_MessageTypes.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "core/timers/src/SingletonTimerUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/timers/src/MonotonicTimer.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	//
	// static declaration
	//

	AlarmAckStateProcessor *							AlarmAckStateProcessor::m_singletonManager = 0;
	TA_Base_Core::ReEntrantThreadLockable				AlarmAckStateProcessor::m_singletonLock;



	AlarmAckStateProcessor::AlarmAckStateProcessor()
	:m_currentMode(TA_Base_Core::Standby),
	 m_isProcessing(false)
	{
		this->clear();
		this->init(); 
    }


	AlarmAckStateProcessor::~AlarmAckStateProcessor()
	{
		this->stopProcess();
		this->clear();        
	}


	TA_Base_Bus::AlarmAckStateProcessor& AlarmAckStateProcessor::getInstance()
	{
		// prevent multiple threads attempting to delete/create
		// simultaneously
		if ( 0 == m_singletonManager)
		{
			TA_THREADGUARD( m_singletonLock );

			// if the singleton has not been created yet
			if ( 0 == m_singletonManager )
			{
				// create one and only one instance
				m_singletonManager = new AlarmAckStateProcessor();
			}
		}
	
		return *m_singletonManager;
	}


	void AlarmAckStateProcessor::removeInstance ()
	{
		// prevent multiple threads attempting to delete/create
		// simultaneously
		if(m_singletonManager != 0)
		{
			TA_THREADGUARD( m_singletonLock );
			if(m_singletonManager != 0)
			{
				// destroy the only instance
				delete m_singletonManager;
				m_singletonManager = 0;
			}
		}
	}

	void AlarmAckStateProcessor::receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef & alarmMessage )
	{
        AlarmAckUpdate* alarmAckUpdate = NULL; 

        switch ( alarmMessage._d() )
        {
        case TA_Base_Core::NewAlarm:
            {
                // new alarm message has alarm value and alarm id, use these can build the alarm id map.
                alarmAckUpdate = new AlarmAckUpdate();
                alarmAckUpdate->m_updateType = AlarmAckUpdate::Update;

                alarmAckUpdate->m_alarmTypeKey = alarmMessage.alarm().messageTypeKey;
                alarmAckUpdate->m_ackState = ScadaAlarmNack;
                alarmAckUpdate->m_entityKey = alarmMessage.alarm().assocEntityKey;
                alarmAckUpdate->m_locationKey = alarmMessage.alarm().assocEntityKey;
                alarmAckUpdate->m_alarmId = alarmMessage.alarm().alarmID;
                alarmAckUpdate->m_user = NULL;

                // for DpGeneral alarm type, need to check the enum value of the alarm
                if (alarmAckUpdate->m_alarmTypeKey == atol(TA_Base_Core::DataPointAlarms::DpGeneral.getTypeKey().c_str()))
                {
                    alarmAckUpdate->m_valueString = alarmMessage.alarm().alarmValue;
                }
                else
                {
                    alarmAckUpdate->m_valueString = "NotUsed";
                }
                
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "receive a new alarm message: entity [%u], alarm type [%u], alarm value [%s], alarm id [%s]",
                    alarmAckUpdate->m_entityKey, alarmAckUpdate->m_alarmTypeKey, alarmAckUpdate->m_valueString.c_str(), alarmAckUpdate->m_alarmId.c_str());
                
                break;
            }
        case TA_Base_Core::UpdateAlarm:
            {
                // update alarm message has no alarm value property, so have to use alarm id to find the subscriber.
                alarmAckUpdate = new AlarmAckUpdate();
                alarmAckUpdate->m_updateType = AlarmAckUpdate::Update;
                
                alarmAckUpdate->m_alarmTypeKey = alarmMessage.update().messageTypeKey;

                std::string ackedBy(alarmMessage.update().alarmAcknowledgedBy);
                bool isAcked = !ackedBy.empty();
                bool isClosed = alarmMessage.update().alarmHasBeenClosed;
                
                if (isClosed && isAcked)
                {
                    alarmAckUpdate->m_ackState = ScadaAlarmNotPresent;
                }
                else
                {
                    alarmAckUpdate->m_ackState = isAcked ? ScadaAlarmAck : ScadaAlarmNack;
                }
                
                alarmAckUpdate->m_entityKey = alarmMessage.update().assocEntityKey;
                alarmAckUpdate->m_locationKey = alarmMessage.update().assocEntityKey;
                alarmAckUpdate->m_alarmId = alarmMessage.update().alarmID;
                alarmAckUpdate->m_user = NULL;

                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "receive an update alarm message: entity [%u], alarm type [%u], alarm ID [%s]",
                    alarmAckUpdate->m_entityKey, alarmAckUpdate->m_alarmTypeKey, alarmAckUpdate->m_alarmId.c_str());
                
                break;
            }
        default:
            {
                break;
            }
        }
        
		if (NULL != alarmAckUpdate)
		{
			boost::shared_ptr<AlarmAckUpdate> ackUpdate(alarmAckUpdate);
            //this->insertUnique(ackUpdate);
            this->insert(ackUpdate);
		}
	}

	void AlarmAckStateProcessor::requestLoadAlarms( unsigned long alarmEntityLocationKey )
	{
		// not consider multithread access, this function only need call one time
		/*if( m_hasRequestLoadAlarms == false)
		{
			AlarmAckUpdate* alarmAckUpdate = new AlarmAckUpdate();
			alarmAckUpdate->m_updateType = AlarmAckUpdate::LoadAlarms;
			alarmAckUpdate->m_locationKey = alarmEntityLocationKey;
			alarmAckUpdate->m_entityKey = 0; //Set the default value to 0 to avoid the wierd value in the log.


			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "receive a LoadAlarms request: EntityLocation [%d]",
				alarmEntityLocationKey);

			boost::shared_ptr<AlarmAckUpdate> ackUpdate(alarmAckUpdate);
			//this->insertUnique(ackUpdate);
			this->insert(ackUpdate);
			m_hasRequestLoadAlarms = true;
		}
		*/
		if( m_hasRequestLoadAlarms == false)
		{
			m_nLocationKey = alarmEntityLocationKey;
			m_hasRequestLoadAlarms = true;
		}
	}

	void AlarmAckStateProcessor::subscribeToAlarmType ( const TA_Base_Core::MessageType & alarmMessageType,
								const std::string & valueString,
                                IAlarmAckStateUser * user,
								unsigned long alarmEntityKey,
								unsigned long alarmEntityLocationKey )
	{
        AlarmAckUpdate* alarmAckUpdate = new AlarmAckUpdate();
        alarmAckUpdate->m_updateType = AlarmAckUpdate::Subscribe;
        
        alarmAckUpdate->m_alarmType = &alarmMessageType;
        alarmAckUpdate->m_alarmTypeKey = atol(alarmMessageType.getTypeKey().c_str());
        alarmAckUpdate->m_valueString = valueString;
        alarmAckUpdate->m_entityKey = alarmEntityKey;
        alarmAckUpdate->m_locationKey = alarmEntityLocationKey;
        alarmAckUpdate->m_user = user;

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "raise a subcribe request: entity [%u], alarm type [%u], alarm value [%s]",
            alarmAckUpdate->m_entityKey, alarmAckUpdate->m_alarmTypeKey, alarmAckUpdate->m_valueString.c_str());

        boost::shared_ptr<AlarmAckUpdate> ackUpdate(alarmAckUpdate);
        //this->insertUnique(ackUpdate);
		//this->insert(ackUpdate);
        {
			TA_THREADGUARD( m_subscribeListLock );
			m_subscribeList.push_back(ackUpdate);
        }
		
		
	}

	void AlarmAckStateProcessor::initializeDataPointAckStates()
	{
		if (true == m_hasRequestLoadAlarms)
		{
			if (false == m_bLoadAlarms)
			{
				//Get all of the acitve alarm information from the alarm agent.
				loadAllAlarms();

				unsigned long locationKey = m_nLocationKey;
				//save location to internal vector
				if (m_alarmLocations.find(locationKey) == m_alarmLocations.end())
				{
					m_alarmLocations.insert(locationKey);

								
			        try
			        {					
			        	// subscribe for all DataPoint alarm types from the specified location key
						TA_Base_Core::AlarmSubscriptionManager::subscribeToAlarmsFromLocation ( 
							TA_Base_Core::DataPointAlarms::Context,
							this,
							locationKey);
			        }
					catch(...)
					{
						LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "subscribeToAlarmsFromLocation." );
						m_bLoadAlarms = false;
					}					
				}

				//Process all of the subscribe from the datapoint alarmdetails.

				if (true == m_bLoadAlarms)
				{
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%d subscribe request will be processed.",m_subscribeList.size()  );
					TA_Base_Core::WorkItemExeSync* pItemSync = new TA_Base_Core::WorkItemExeSync();
					std::vector<TA_Base_Core::IThreadPoolExecuteItem*> workItems;
					workItems.reserve(m_subscribeList.size() + 1);

					DataPointAlarmSubscribes::iterator iterSub = m_subscribeList.begin();
					for (; iterSub != m_subscribeList.end(); ++iterSub)
					{
						workItems.push_back(new AlarmAckSubscribeWorkItem(this,*iterSub, pItemSync));
					}

				    LFThreadPoolSingleton& threadPool =  LFThreadPoolSingleton::getInstance();
					threadPool->queueWorkItemList(workItems);
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool begin process all alarmack subscribe" );
			 		pItemSync->wait();
					LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Thread pool end process all alarmack subscribe" );
					delete pItemSync;
				}
			}
		}
	}

	void AlarmAckStateProcessor::unsubscribeToAlarmType ( const TA_Base_Core::MessageType & alarmMessageType,
								  const std::string & valueString,
                                  unsigned long alarmEntityKey,
								  unsigned long alarmEntityLocationKey )
	{
        AlarmAckUpdate* alarmAckUpdate = new AlarmAckUpdate();
        alarmAckUpdate->m_updateType = AlarmAckUpdate::Unsubscribe;
        
        alarmAckUpdate->m_alarmType = &alarmMessageType;
        alarmAckUpdate->m_alarmTypeKey = atol(alarmMessageType.getTypeKey().c_str());
        alarmAckUpdate->m_valueString = valueString;
        alarmAckUpdate->m_entityKey = alarmEntityKey;
        alarmAckUpdate->m_locationKey = alarmEntityLocationKey;
        alarmAckUpdate->m_user = NULL;
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "receive a unsubcribe request: entity [%u], alarm type [%u], alarm value [%s]",
            alarmAckUpdate->m_entityKey, alarmAckUpdate->m_alarmTypeKey, alarmAckUpdate->m_valueString.c_str());
        
        boost::shared_ptr<AlarmAckUpdate> ackUpdate(alarmAckUpdate);
        //this->insertUnique(ackUpdate);
        this->insert(ackUpdate);
	}


    void AlarmAckStateProcessor::threadedTimerExpired(long timerId, void* userData)
    {
        // when timer expired, move all items from retry queue to working queue.
		//if ((m_bLoadAlarms == false) && (m_bLoadAlarmsError == true))
		if (false == m_bLoadAlarms)
		{
			//LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "AlarmAckStateProcessor::threadedTimerExpired. loadAllAlarms again");
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "AlarmAckStateProcessor::threadedTimerExpired. initializeDataPointAckState again");
				
			//loadAllAlarms(m_nLocationKey);
			initializeDataPointAckStates();
		}

		/*
		unsigned long returyQueSize=0;
		{
			TA_THREADGUARD( m_toRetryQueueLock );
	    	if (!m_toRetryQueue.empty())
	        {
	        	returyQueSize = m_toRetryQueue.size();
	        	std::vector<boost::shared_ptr<AlarmAckUpdate> >::iterator updateIt = m_toRetryQueue.begin();
		        for (; updateIt != m_toRetryQueue.end(); ++ updateIt)
		        {
		            this->insertUnique(*updateIt);
		        }
		        m_toRetryQueue.clear();
	        }
		}

		if(returyQueSize > 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "retry %u items", returyQueSize);
		}
		*/
    }

    void AlarmAckStateProcessor::processSubscribe(boost::shared_ptr<AlarmAckUpdate> update)
	{
		if (NULL == update.get())
		{
			return;
		}
		
        //to get alarm state from alarm agent
        TA_Base_Core::AlarmDetailCorbaDef * alarmDetail = NULL;
        EScadaAlarmAckState ackState = ScadaAlarmUnknown;
        
        try
        {
        	/*
			if ((m_bLoadAlarms == false) || (m_bLoadAlarmsError == true))
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, 
					"Cannot contact alarm agent, add the subscribe request to queue for later retry.");
				
				TA_THREADGUARD( m_toRetryQueueLock );
				m_toRetryQueue.push_back(update);
				return;
			}
			*/

			// Obtain handle to the Alarm Helper
            alarmDetail = getActiveAlarmForEntityOfType (update->m_alarmTypeKey,
				update->m_entityKey);

			//if (!m_bGotAlarmDetail)
			if (NULL == alarmDetail)
			{
				ackState = ScadaAlarmNotPresent;
			}
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", "calling getActiveAlarmForEntityOfType." );
        }

        // if can't get alarm detail from alarm agent, schedule to retry and return
        //if (!gotAlarmDetail)

        //save alarm ack user to internal map
        DataPointAlarmType dpAlarmType;
        dpAlarmType.entityKey = update->m_entityKey;
        dpAlarmType.alarmTypeKey = update->m_alarmTypeKey;
        dpAlarmType.valueString = update->m_valueString;

		{
			TA_THREADGUARD( m_alarmEntityMapLock );
        	m_alarmEntityMap[dpAlarmType] = update->m_user;
        }

		{
			/*
			TA_THREADGUARD( m_alarmLocationsLock );
	        //save location to internal vector
	        if (m_alarmLocations.find(update->m_locationKey) == m_alarmLocations.end())
	        {
	            m_alarmLocations.insert(update->m_locationKey);
	            
	            // subscribe for all DataPoint alarm types from the specified location key
	            TA_Base_Core::AlarmSubscriptionManager::subscribeToAlarmsFromLocation ( 
	                TA_Base_Core::DataPointAlarms::Context,
	                this,
	                update->m_locationKey );
	        }
	        	*/
		}
		
        std::string alarmId;

        if( NULL != alarmDetail )
        {
            std::string ackedBy( alarmDetail->alarmAcknowledgedBy );
            alarmId = alarmDetail->alarmID;

            // if the alarm type is DpGeneral alarm, 
            // need to get the alarm value to check if it is the exact enum value alarm
            if (update->m_alarmTypeKey == atol(TA_Base_Core::DataPointAlarms::DpGeneral.getTypeKey().c_str()))
            {
                std::string valueString(alarmDetail->alarmValue);
                if (valueString == update->m_valueString)
                {
                    ackState = ackedBy.empty() ? ScadaAlarmNack : ScadaAlarmAck;
                }
            }
            else
            {
                ackState = ackedBy.empty() ? ScadaAlarmNack : ScadaAlarmAck;
            }

            delete alarmDetail;
            alarmDetail = NULL;
        }

        if (ackState != ScadaAlarmUnknown)
        {
            // if the alarm is not acknowledged or not closed, save it to alarm id map.
            if (ackState == ScadaAlarmNack || ackState == ScadaAlarmAck)
            {
				TA_THREADGUARD( m_AlarmsIdMapLock );
                m_alarmIdMap[alarmId] = update->m_user;
				m_currentAlarmIdMap[update->m_entityKey] = AlarmIDAndUser(alarmId, update->m_user);
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Update alarm type [%u] with alarm value [%s] to AckState [%u] for entity [%u]",
                update->m_alarmTypeKey, update->m_valueString.c_str(), ackState, update->m_entityKey);
            
            update->m_user->updateAlarmAckState(ackState);
        }	
	}

    void AlarmAckStateProcessor::processUnsubscribe(boost::shared_ptr<AlarmAckUpdate> update)
	{
		if (NULL == update.get())
		{
			return;
		}
			
        //remove alarm ack user from internal map
        DataPointAlarmType dpAlarmType;
        dpAlarmType.entityKey = update->m_entityKey;
        dpAlarmType.alarmTypeKey = update->m_alarmTypeKey;
        dpAlarmType.valueString = update->m_valueString;

		{
			TA_THREADGUARD( m_alarmEntityMapLock );
		    DataPointAlarmTypeMap::iterator alarmEntityIt = m_alarmEntityMap.find(dpAlarmType);
		    if (alarmEntityIt != m_alarmEntityMap.end())
		    {
		        m_alarmEntityMap.erase(alarmEntityIt);
		    }
		    else
		    {
		        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Try to remove non-existed subscription for entity [%u] alarm type [%u] with alarm value [%s]",
		            update->m_entityKey, update->m_alarmTypeKey, update->m_valueString.c_str());
		    }	
		}
		
		{
			TA_THREADGUARD( m_alarmEntityMapLock );
		    // remove the related item of alarm id map
		    for (AlarmIdMap::iterator alarmIdIt = m_alarmIdMap.begin();
		        alarmIdIt != m_alarmIdMap.end(); ++ alarmIdIt)
		    {
		        if (alarmIdIt->second == update->m_user)
		        {
		            m_alarmIdMap.erase(alarmIdIt);
		            break;
		        }
		    }
			
			EntityCurrentAlarmIdMap::iterator it = m_currentAlarmIdMap.find(update->m_entityKey);
			if (m_currentAlarmIdMap.end() != it)
			{
				m_currentAlarmIdMap.erase(it);
			}	
		}
	}

    void AlarmAckStateProcessor::processEvent(boost::shared_ptr<AlarmAckUpdate> update)
	{
		if (NULL == update.get())
		{
			return;
		}

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Begin to process update [%u] for entity [%u]",
            update->m_updateType, update->m_entityKey);
        
		try
		{
			switch (update->m_updateType)
			{
			case AlarmAckUpdate::LoadAlarms:
				{
					//loadAllAlarms(update->m_locationKey);
					break;
				}
            case AlarmAckUpdate::Subscribe:
				{
                    //to get alarm state from alarm agent
					//processSubscribe(update); //Single thread to process
				}
				break;

            case AlarmAckUpdate::Unsubscribe:
				{
                    //remove alarm ack user from internal map	
                    //processUnsubscribe(update);
                }
				break;

            case AlarmAckUpdate::Update:
                {
                    // first search the alarm id map
                    AlarmIdMap::iterator alarmIdIt = m_alarmIdMap.find(update->m_alarmId);
                    if (alarmIdIt != m_alarmIdMap.end())
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Update alarm type [%u] with alarm id [%s] to AckState [%u] for entity [%u]",
                            update->m_alarmTypeKey, update->m_alarmId.c_str(), update->m_ackState, update->m_entityKey);
                        
						bool isCurrentAlarmId = false;
						EntityCurrentAlarmIdMap::iterator it = m_currentAlarmIdMap.find(update->m_entityKey);
						if (m_currentAlarmIdMap.end() != it)
						{
							isCurrentAlarmId = (it->second.first == update->m_alarmId);
							// current alarm id or another alarmDetail user;
							if (isCurrentAlarmId || it->second.second != alarmIdIt->second)
							{
								alarmIdIt->second->updateAlarmAckState(update->m_ackState);
							}
						}
						else
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Current AlarmId do't exist, this update is too old.");
						}

                        // if the alarm is already acknowledged & closed, remove it from alarm id map.
                        if (update->m_ackState != ScadaAlarmNack && update->m_ackState != ScadaAlarmAck)
                        {
                            m_alarmIdMap.erase(alarmIdIt);
							if (isCurrentAlarmId)
							{
								m_currentAlarmIdMap.erase(it);
							}
                        }
                    }
                    else
                    {
                        // search the related alarm entity and alarm type, call its callback function
                        DataPointAlarmType dpAlarmType;
                        dpAlarmType.entityKey = update->m_entityKey;
                        dpAlarmType.alarmTypeKey = update->m_alarmTypeKey;
                        dpAlarmType.valueString = update->m_valueString;

                        DataPointAlarmTypeMap::iterator alarmEntityIt = m_alarmEntityMap.find(dpAlarmType);
                        if (alarmEntityIt != m_alarmEntityMap.end())
                        {
                            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Update alarm type [%u] with alarm value [%s] to AckState [%u] for entity [%u]",
                                update->m_alarmTypeKey, update->m_valueString.c_str(), update->m_ackState, update->m_entityKey);
                        
                            alarmEntityIt->second->updateAlarmAckState(update->m_ackState);

                            // if the alarm is not acknowledged or not closed, add it to alarm id map.
                            if (update->m_ackState == ScadaAlarmNack || update->m_ackState == ScadaAlarmAck)
                            {
                                m_alarmIdMap[update->m_alarmId] = alarmEntityIt->second;
								
								// set current alarm id and ack user.
								m_currentAlarmIdMap[update->m_entityKey] = AlarmIDAndUser(update->m_alarmId, alarmEntityIt->second);
                            }
                        }
                    }
                }
				break;
			}
		}
		catch( const TA_Base_Core::TransactiveException & te )
		{
			LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
		}
        catch( const CORBA::Exception& )
        {
            LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", "Corba exception caught during processing AlarmUpdate." );
        }
		catch(...) 
		{
			LOG_EXCEPTION_CATCH(SourceInfo,"UnknownException", "Unknown exception caught during processing AlarmUpdate.");
		}
				
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Finished to process update [%u] for entity [%u]",
            update->m_updateType, update->m_entityKey);
	}

	//void AlarmAckStateProcessor::loadAllAlarms( unsigned long alarmEntityLocationKey )
	void AlarmAckStateProcessor::loadAllAlarms()
	{
		//Remove the AlarmMapLock as this is a sequence operation. Lock is not required here.
        //TA_THREADGUARD( m_AlarmsMapLock );
		//m_nLocationKey = alarmEntityLocationKey;
		if (m_bLoadAlarms == false)
		{
			//m_bLoadAlarmsError = false;
			//first clear the map
			for( EntityToAlarmMap::iterator p = m_entityToAlarmMap.begin(); p != m_entityToAlarmMap.end(); p++ )
			{
				if( p->second != NULL )
				{
					delete p->second;
					p->second = NULL;
				}
			}
			m_entityToAlarmMap.clear();

			//get all of the alarms from the related alarmagent.
			TA_Base_Core::AlarmSequence* alarmList = NULL;
			try
			{
				alarmList = TA_Base_Core::AlarmHelperManager::getInstance().getAlarmHelper().getActiveAlarms(m_nLocationKey);
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "alarmList length is: %d", alarmList->length());
			}


			catch( const TA_Base_Core::TransactiveException & te )
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "TransactiveException", te.what() );
				//m_bLoadAlarmsError = true;
			}
			catch ( const TA_Base_Core::OperationModeException & ope)
			{
				LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::OperationModeException", "Caught Operation Mode exception" );
				//m_bLoadAlarmsError = true;
			}
			catch ( const CORBA::Exception & ex )
			{
				LOG_EXCEPTION_CATCH ( SourceInfo, "CORBA::Exception", ( TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex) ).c_str() );
				//m_bLoadAlarmsError = true;
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown Exception", 
					"calling getAlarms." );
				//m_bLoadAlarmsError = true;
			}

			if (alarmList != NULL && !m_bLoadAlarmsError)
			{
				for (unsigned int iLoop = 0; iLoop < alarmList->length(); iLoop++)
				{
					TA_Base_Core::AlarmDetailCorbaDef * alarmDetail = new TA_Base_Core::AlarmDetailCorbaDef( (*alarmList)[iLoop] );

					m_entityToAlarmMap.insert(std::pair<unsigned long, TA_Base_Core::AlarmDetailCorbaDef*>(alarmDetail->assocEntityKey, alarmDetail));
				}

				delete alarmList;
				m_bLoadAlarms = true;
			}

		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Have done the batch alarms loading, no need to do again");
		}

	}

	TA_Base_Core::AlarmDetailCorbaDef* AlarmAckStateProcessor::getActiveAlarmForEntityOfType(unsigned long p_alarmTypeKey, unsigned long p_entity )
	{
        FUNCTION_ENTRY( "getActiveAlarmForEntityOfType" );
		//After getting the AlarmList, we can using the map to search the alarm. so no need to add lock here.
        //TA_THREADGUARD(m_AlarmsMapLock);
		
		//m_bGotAlarmDetail = false;
		TA_Base_Core::AlarmDetailCorbaDef* rtnAlarm = NULL;
		
		EntityToAlarmMap::const_iterator foundIterator = m_entityToAlarmMap.find(p_entity);
		if (foundIterator== m_entityToAlarmMap.end())
		{
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,"No alarm found for Type %u, Entity key %d", p_alarmTypeKey, p_entity);
			FUNCTION_EXIT;
			return rtnAlarm;
		}

		unsigned long nCount = m_entityToAlarmMap.count(p_entity);
		for (unsigned long i = 0; i < nCount; i++)
		{
			TA_ASSERT((foundIterator->second != NULL), "Alarm cannot be NULL");
			TA_ASSERT((foundIterator->second->messageTypeKey != 0), "Message type key cannot be 0");
			
			std::string alarmAckedBy(foundIterator->second->alarmAcknowledgedBy.in());
            if ( (foundIterator->second->state == TA_Base_Core::AlarmOpen || true == alarmAckedBy.empty()) && foundIterator->second->messageTypeKey ==p_alarmTypeKey )
			{
				rtnAlarm = new TA_Base_Core::AlarmDetailCorbaDef(*foundIterator->second);
				//m_bGotAlarmDetail = true;

				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
				"Returning alarm with ID %s, Type %u, Entity key %d", rtnAlarm->alarmID.in(),rtnAlarm->messageTypeKey,rtnAlarm->assocEntityKey);
				break;
			}
			foundIterator++;			
		}
		
		return rtnAlarm;
	}


	void	AlarmAckStateProcessor::setOperationMode(TA_Base_Core::EOperationMode mode)
	{
		if(m_currentMode != mode)
		{
			switch(mode)
			{
				case TA_Base_Core::Control:
					{
						this->startProcess();
						m_currentMode = mode;
					}
					break;
				case TA_Base_Core::Monitor:
					{
						this->startProcess();
						m_currentMode = mode;
					}
					break;
				case TA_Base_Core::Standby:
					{
						this->stopProcess();
						m_currentMode = mode;
					}
					break;
				default:
					break;
			}
		}
	}
		
	void AlarmAckStateProcessor::startProcess()
	{
		if(!m_isProcessing)
		{
			this->init();
        	this->start();
        	TA_Base_Core::SingletonTimerUtil::getInstance().startPeriodicTimeOutClock ( this, 10000, false );
			m_isProcessing = true;
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"Start Processing");
		}
	}

	void AlarmAckStateProcessor::stopProcess()
	{
		if(m_isProcessing)
		{
			TA_Base_Core::SingletonTimerUtil::getInstance().stopPeriodicTimeOutClock( this );
			try
        	{
            	TA_Base_Core::AlarmSubscriptionManager::unsubscribeToMessages ( this );
        	}
        	catch ( ... )
        	{
        		LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,"cause unknown exception when unsubscribeTo alarm message");
        	}
			this->terminateAndWait();
			this->clear();
			this->clearQueue();
			m_isProcessing = false;
			LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo,"Stop Processing");
		}
	}

	void AlarmAckStateProcessor::init()
	{
		m_bLoadAlarmsError = false;
		m_bLoadAlarms = false;
		//m_bGotAlarmDetail = false;
		m_hasRequestLoadAlarms = false;
		//m_nLocationKey=?
	}

	void AlarmAckStateProcessor::clear()
	{
		m_alarmLocations.clear();
        m_toRetryQueue.clear();
        m_alarmEntityMap.clear();
        m_alarmIdMap.clear();
		m_currentAlarmIdMap.clear();
		m_entityToAlarmMap.clear();
	}

	AlarmAckSubscribeWorkItem::AlarmAckSubscribeWorkItem(AlarmAckStateProcessor* processor, 
										boost::shared_ptr<AlarmAckUpdate> subscribeRequest,
										TA_Base_Core::WorkItemExeSync* workSync)
		:
	m_processor(processor),
	m_subscribeRequest(subscribeRequest),
	m_sync(workSync)
	{
		m_sync->itemBegin();
	}

	void AlarmAckSubscribeWorkItem::executeWorkItem()
	{
		//LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "begin set datanode mode");
		TA_Base_Core::MonotonicTimer timeStart1,timeStart2;
		m_processor->processSubscribe(m_subscribeRequest);
		unsigned long nLapsed1 = timeStart1.elapsed();
		m_sync->itemEnd();
		unsigned long nLapsed2 = timeStart2.elapsed();
		LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "AlarmAckSubscribeWorkItem TimeSpan: %lu, WithSyncSpan: %lu",nLapsed1,nLapsed2-nLapsed1);
	}	
}

