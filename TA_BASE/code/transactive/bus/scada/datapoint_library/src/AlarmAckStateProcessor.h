/**
  * The source code in this file is the property of Ripple Systems and is not for redistribution in
  * any form.
  *
  * Source:     $File: //depot/TA_Common_V1_TIP/transactive/bus/scada/datapoint_library/src/AlarmAckStateProcessor.h $
  * @author:    Weimin
  * @version $Revision: #3 $
  *
  * Last modification : $DateTime: 2017/05/26 20:08:22 $
  * Last modified by : $Author: yong.liu $
  *
  *
  * AlarmAckStateProcessor provides a single interface to subscribe and unsubscribe for alarm
  * acknowledgemnt from an alarm entity.
  *
  */

#ifndef ALARM_ACK_STATE_SINGLETON_MANAGER_H
#define ALARM_ACK_STATE_SINGLETON_MANAGER_H

#include <set>

#include "bus/scada/common_library/src/CommonDefs.h"

#include "core/threads/src/LFThreadPoolManager.h"
#include "core/alarm/src/AlarmReceiver.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/QueueProcessor.h"
#include "core/timers/src/AbstractThreadedTimeoutUser.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

namespace TA_Base_Core
{
    class AlarmMessageCorbaDef;
    class MessageContext;
    class MessageType;
};

namespace TA_Base_Bus
{
    // forward declaration
    class IAlarmAckStateUser;

    struct AlarmAckUpdate
    {
		//Add LoadAlarms EUpdateType for batch loading alarms
        enum EUpdateType {Subscribe, Unsubscribe, Update, LoadAlarms};
        
        EUpdateType m_updateType;
        
        unsigned long m_entityKey;
        unsigned long m_locationKey;

        const TA_Base_Core::MessageType* m_alarmType;
        long m_alarmTypeKey;
        std::string m_valueString;
        std::string m_alarmId;
        
        IAlarmAckStateUser* m_user;
        EScadaAlarmAckState m_ackState;

        bool operator==(const AlarmAckUpdate& rhs) { return false; }
    };

    /**
    * This class is a singleton class without reference counted,
    *     so it's removeInstance function can only be called once, and after that, all references become invalid.
    *
    * This class maintenances a alarm type map for all DataPointAlarmDetails for callback.
    * For BooleanAlarm and AnalogueAlarm type, the DataPointAlarmDetails object is unique with alarm type,
    * but for DpGeneral alarm, the map item must be identified by enum value.
    *
    * When DataPointAlarmDetails object is set to control, it will call subscribe function to register
    *     for receiving alarm ack state update callback. 
    *     And also, when it is set to monitor, it must call unsubscribe function to deregister.
    *
    * This class receives all alarm messages for registered locations, normally only one local location.
    *
    * When NEW alarm message is received, the alarm message includes alarm type and alarm value, so it's 
    *     easy to map the alarm to DataPointAlarmDetail objects and to notify the alarm ack state.
    * When UPDATE alarm message is received, the alarm message doesn't include alarm value, so can not use
    *     the alarm type map to find the exact DataPointAlarmDetail object. How to do?
    *
    * Here uses another map, alarm id map, to solve the problem. For all non-acknowledged or non-closed alarm, 
    *     there is an item to map the alarm id to DataPointAlarmDetail object, 
    *     and when the alarm is acknowledged and closed, the item will be removed.
    */

    class AlarmAckStateProcessor : public TA_Base_Core::AlarmReceiver,
								   public TA_Base_Core::QueueProcessor<AlarmAckUpdate>,
                                   public TA_Base_Core::AbstractThreadedTimeoutUser
    {
    public:
       
        /**
          *
          * getInstance
          *
          * Get an instance of AlarmAckStateProcessor.  Caller is responsible for destroying
          * the instance by calling removeInstance(), passing in the instance created
          * by this method.
          *
          */
        static AlarmAckStateProcessor& getInstance();

        /**
          *
          * removeInstance
          *
          * Remove an instance of AlarmAckStateProcessor that has been created by the getInstance()
          *
          *
          */
        static void removeInstance();

		//
		// operations required by the AbstractThreadedTimeoutUser class
		//


		/**
		*
		* threadedTimerExpired
		*
		* To be implemented by users of TimerUtil.  This operation will be called
		* when the registered timeout period has expired.
		*
		*/
		virtual void threadedTimerExpired(long timerId, void* userData);

        //
        // Operations required by TA_Base_Core::AlarmReceiver
        //

        /** 
          * receiveAlarmMessage
          *
          * This function is overridden by the client that wishes to receive AlarmMessages
          *
          * @param TA_Base_Core::AlarmMessageCorbaDef& alarmMessage
          *
          */
        virtual void receiveAlarmMessage( const TA_Base_Core::AlarmMessageCorbaDef & alarmMessage );


        //
        // Operations specific to AlarmAckStateProcessor
        //

        /**
          * subscribeToAlarmType
          *
          * The specified subscriber wishes to receive alarm updates from the alarm entity,
          * identified by the entity key and location key, in the given alarm context/alarm type
          *
          * @param    alarmMessageType        The type of alarm to subscribe to
          * @param    subscriber              The subscriber who wants to receive alarms notification
          * @param    alarmEntityKey          The key of the alarm entity
          * @param    alarmEntityLocationKey  The location key of the alarm entity
          * 
          */
        void subscribeToAlarmType( const TA_Base_Core::MessageType & alarmMessageType,
                                   const std::string & valueString,
                                   IAlarmAckStateUser * user,
                                   unsigned long alarmEntityKey,
                                   unsigned long alarmEntityLocationKey );

        
		/**
          * requestLoadAlarms
          *
          * put a loadalarms request to queue for processor
          *
          * @param    alarmEntityLocationKey  The location key of the alarm entity
          * 
          */
		void requestLoadAlarms( unsigned long alarmEntityLocationKey );
        
		/**
          * loadLocatioinAllAlarms
          *
          * load all of the alarms for this locaiton from alarmagent
          *
          * @param    alarmEntityLocationKey  The location key of the alarm entity
          * 
          */
        //void loadAllAlarms( unsigned long alarmEntityLocationKey );
        void loadAllAlarms();
		
		virtual void processEvent(boost::shared_ptr<AlarmAckUpdate> update);
		
		/**
          * unsubscribeToAlarmType
          *
          * The specified subscriber wishes not to receive alarm updates from the alarm entity,
          * identified by the entity key and location key, in the given alarm context/alarm type
          *
          * @param    alarmMessageType        The type of alarm to unsubscribe
          * @param    subscriber              The subscriber who does not want to receive alarms notification
          * @param    alarmEntityKey          The key of the alarm entity
          * @param    alarmEntityLocationKey  The location key of the alarm entity
          * 
          */
		
        void unsubscribeToAlarmType( const TA_Base_Core::MessageType & alarmMessageType,
                                    const std::string & valueString,
                                    unsigned long alarmEntityKey,
                                    unsigned long alarmEntityLocationKey );

		
		void	setOperationMode(TA_Base_Core::EOperationMode mode);
		void	stopProcess();

	void processSubscribe(boost::shared_ptr<AlarmAckUpdate> update);
	void processUnsubscribe(boost::shared_ptr<AlarmAckUpdate> update);

	void initializeDataPointAckStates();
		
    private:

        /**
          * subscribeToAlarmsFromLocation
          *
          * Subscribe to receive all alarm updates from the given location
          *
          * @param    messageContext  The context/type of alarm to subscribe to
          * @param    locationKey     The locationKey of the entity alarms that are interested in
          * 
          */
        void subscribeToAlarmsFromLocation ( unsigned long locationKey );

        
        /**
          * unsubscribeToAlarmMessages
          *
          * Unsubscribe in order not to receive all alarm updates from the given location
          *
          */
        void unsubscribeToAlarmMessages();

		TA_Base_Core::AlarmDetailCorbaDef* getActiveAlarmForEntityOfType(unsigned long p_alarmTypeKey, unsigned long p_entity );

		void			init();
		void			clear();
		void			startProcess();

        virtual ~AlarmAckStateProcessor();
        AlarmAckStateProcessor();
        AlarmAckStateProcessor ( const AlarmAckStateProcessor & theInstance );
        AlarmAckStateProcessor & operator= ( const AlarmAckStateProcessor & rhs );
		
    private:

        static AlarmAckStateProcessor *					        m_singletonManager; 
        static TA_Base_Core::ReEntrantThreadLockable			m_singletonLock;

        std::set<unsigned long>                                 m_alarmLocations;
        TA_Base_Core::ReEntrantThreadLockable			        m_alarmLocationsLock;

        std::vector< boost::shared_ptr<AlarmAckUpdate> >        m_toRetryQueue;
        TA_Base_Core::ReEntrantThreadLockable			        m_toRetryQueueLock;

        struct DataPointAlarmType
        {
            unsigned long entityKey;
            long alarmTypeKey;
            std::string valueString;

            //override operator '<' to allow this struct can be used as std::map key.
            bool operator < (const DataPointAlarmType& right) const
            {
                return entityKey < right.entityKey ||
                    (entityKey == right.entityKey && alarmTypeKey < right.alarmTypeKey) ||
                    (entityKey == right.entityKey && alarmTypeKey == right.alarmTypeKey && valueString < right.valueString);
            }
        };

        typedef std::map<DataPointAlarmType, IAlarmAckStateUser*>    DataPointAlarmTypeMap;
        DataPointAlarmTypeMap                                        m_alarmEntityMap;
        TA_Base_Core::ReEntrantThreadLockable			        	 m_alarmEntityMapLock;
        typedef std::map<std::string, IAlarmAckStateUser*>           AlarmIdMap;
        AlarmIdMap                                                   m_alarmIdMap;

		// <entitykey, <alarm id, IAlarmAckStateUser*>>, 
		typedef std::pair<std::string, IAlarmAckStateUser*>			AlarmIDAndUser;
		typedef std::map<unsigned long, AlarmIDAndUser>				EntityCurrentAlarmIdMap;
		EntityCurrentAlarmIdMap										m_currentAlarmIdMap;
        TA_Base_Core::ReEntrantThreadLockable						m_AlarmsIdMapLock;

        typedef std::multimap<unsigned long, TA_Base_Core::AlarmDetailCorbaDef*> EntityToAlarmMap;
        EntityToAlarmMap											m_entityToAlarmMap;
		volatile bool												m_bLoadAlarmsError;
		volatile bool												m_bLoadAlarms;
		//volatile bool													m_bGotAlarmDetail;
		volatile bool												m_hasRequestLoadAlarms;
		volatile unsigned long										m_nLocationKey;
        TA_Base_Core::ReEntrantThreadLockable						m_AlarmsMapLock;

		TA_Base_Core::EOperationMode								m_currentMode;
		bool														m_isProcessing;
		
		typedef std::vector<boost::shared_ptr<AlarmAckUpdate> >		DataPointAlarmSubscribes;
		DataPointAlarmSubscribes									m_subscribeList;
		TA_Base_Core::ReEntrantThreadLockable						m_subscribeListLock;
		
    };

	class AlarmAckSubscribeWorkItem : public TA_Base_Core::IThreadPoolExecuteItem
	{
	public:
		AlarmAckSubscribeWorkItem(AlarmAckStateProcessor* processor, 
										boost::shared_ptr<AlarmAckUpdate> subscribeRequest,
										TA_Base_Core::WorkItemExeSync* workSync);
		void executeWorkItem();
	private:
		AlarmAckStateProcessor*				m_processor;
		boost::shared_ptr<AlarmAckUpdate>	m_subscribeRequest;
		TA_Base_Core::WorkItemExeSync*  	m_sync;
	};	
};

#endif
