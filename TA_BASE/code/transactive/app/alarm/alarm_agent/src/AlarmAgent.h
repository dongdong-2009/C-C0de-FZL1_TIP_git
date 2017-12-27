/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_agent/src/AlarmAgent.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(ALARMAGENT)
#define ALARMAGENT

#include <queue>
#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmUpdateCorbaDef.h"
#include "core/alarm/IDL/src/AlarmAgentQueueItemCorbaDef.h"

namespace TA_Base_Bus
{
    class GenericAgent; // Forward Declaration

    class IEntity; // Forward Declaration
}

namespace TA_Base_Core
{
    class IEntityData; // Forward Declaration

    class ConfigUpdateDetails; // Forward Declaration

    struct StateUpdateMessageCorbaDef; // Forward Declaration
}

namespace TA_Base_App
{
    class AlarmAgentImpl; // Forward Declaration

    class AlarmAgent : public virtual TA_Base_Bus::IGenericAgentUser
    {
    public:

        /**
          * Constructor
          *
          * @param The number of command line arguments passed in
          * @param The command line
          *
          * @exception GenericAgentException thrown if GenericAgent fails to
          *            initialise.
          * @exception DatabaseException thrown if a database error occurs
          * @exception DataException thrown if data returned from the database is not valid
          */
        AlarmAgent(int argc, char* argv[]);

        /**
          * Destructor
          */
        virtual ~AlarmAgent();

        /**
          * agentTerminate
          *
          * This method is called by the GenericAgent when it is time to
          * terminate.
          */


		virtual void checkOperationMode()
		{//TD18706,jianghp
			//do nothing
		}
        /**
          * agentTerminate
          *
          * This method is called by the GenericAgent when it is time to
          * terminate.
          */
		void agentTerminate();

        /**
          * createEntity
          *
          * This method is called by the GenericAgent. Given that the Alarm
          * Agent has no entities, it just returns a NULL pointer.
          *
          * @param Entity data retrieved from the database.
          *
          * @return The constructed entity
          */
        TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

        /**
          * agentSetMonitor
          *
          * This method is called by GenericAgent when the Alarm Agent operating mode is
          * set to Monitor.
          */
        void agentSetMonitor();

        /**
          * agentSetControl
          *
          * This method is called by GenericAgent when the Alarm Agent operating mode is
          * set to Control.
          */
        void agentSetControl();

		/**
          * agentSetControl
          *
          * This method is called by GenericAgent when the Alarm Agent operating mode is
          * set to Standby.
          */
		void agentSetStandby();

        /**
          * registerForStateUpdates
          *
          * This pure virtual method forces agents to subscribe for either
          * state updates or state update requests, depending on whether or not they
          * are operating in monitor or control mode.
          */
        void registerForStateUpdates();

		/** 
		  * notifyGroupOffline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOffline from the Process Monitor.
          *
          * @param The group name
		  */
        void notifyGroupOffline( const std::string& group );

		/** 
		  * notifyGroupOnline
		  *
		  * This request is passed on by GenericAgent when it receives a call to 
          * notifyGroupOnline from the Process Monitor.
          *
          * @param The group name
		  */
        void notifyGroupOnline( const std::string& group );

        /**
          * receiveSpecialisedMessage
          *
          * This method is called whenever the Agent receives a state update or
          * state update request (depending on their mode of operation)
          */
        void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

        /**
          * processOnlineUpdate
          *
          * This is called by Generic Agent whenever it gets an online update that it
          * cant handle itself.
          *
          * @param The update event
          */
        void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

        /**
          * run
          *
          * This method calls GenericAgent::run() which blocks until the System
          * Controller shuts us down
          */
        void run();
		void cleanAlarmsAndRules();

    private:

        /**
          * registerForControlMessages
          *
          * This method registers for the messages we expect to receive while in 
          * control mode.
          */
        void registerForControlMessages();

        /**
          * registerForMonitorMessages
          *
          * This method registers for the messages we expect to receive while in 
          * monitor mode.
          */
        void registerForMonitorMessages();

        /**
          * receiveNewAlarmStateUpdateMsg
          *
          * This method is called when we receive a new
          * alarm via a state update message (this should only happen if we are
          * in Monitor mode)
          *
          * @param The new alarm
          */
        void receiveNewAlarmStateUpdateMsg( const TA_Base_Core::AlarmDetailCorbaDef& p_alarm );

        /**
          * receiveAlarmUpdateStateUpdateMsg
          *
          * This method is called when we receive an alarm
          * update via a state update message (this should only happen if we are
          * in Monitor mode)
          *
          * @param The alarm update
          */
        void receiveAlarmUpdateStateUpdateMsg( const TA_Base_Core::AlarmUpdateCorbaDef& p_update );

        /**
          * receiveQueueUpdateStateUpdateMsg
          *
          * This method is called we receive an alarm
          * update via a state update message (this should only happen if we are
          * in Monitor mode)
          *
          * @param The alarm update
          */
        void receiveQueueUpdateStateUpdateMsg( const TA_Base_Core::AlarmAgentQueueItemSequence& p_update );

		/* TD 14082 Remove batch cache and queue updates from control agent, use direct corba call instead
        /**
          * receiveBatchCacheStateUpdateMsg
          *
          * This method is called we receive a batch of cached
          * alarms via a state update message (this should only happen if we are
          * in Control mode)
          *
          * @param The alarms
          *
        void receiveBatchCacheStateUpdateMsg( const TA_Base_Core::AlarmSequence& p_alarms );

        /**
          * receiveBatchQueueStateUpdateMsg
          *
          * This method is called we receive a batch of queue
          * items via a state update message (this should only happen if we are
          * in Control mode)
          *
          * @param The queue items
          *
        void receiveBatchQueueStateUpdateMsg( const TA_Base_Core::AlarmAgentQueueItemSequence& m_queue );
		*/

        /**
          * receiveRequestForStateUpdateMsg
          *
          * This method is called we receive a request for
          * state information (this should only happen if we are in Control mode)
          */
        void receiveRequestForStateUpdateMsg();

        /**
          * setupRunParams
          *
          * This method reads the entity data for the Alarm Agent entity and 
          * sets up the appropriate run time parameters
          *
          * @exception DatabaseExceptions from entityAccess will not be caught here
          * @exception DataExceptions from entityAccess will not be caught here
          */
        void setupRunParams();

        /**
          * getSession
          *
          * This method gets a session key from the authentication agent and sets the
          * session key run param. If the auth agent is not available, it loops until
          * it gets a session successfully (or until it's killed by the system controller)
          */
        void getSession();

        /**
          * processMessageUpdate
          *
          * This method process the message submitted which can be a new alarm
		  * updated alarm or queue update accordingly
		  *
		  * @param TA_Base_Core::StateUpdateMessageCorbaDef&. The update message
          */
		void processMessageUpdate(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

        void unsubscribeToMessages(); //limin, failover

		/**
          * loadAlarmAgentData
          *
          * This method will load all alarms and rules either from database or from control agent
		  * depending to the current state.
		  *
		  * @param bool bSkipAlarmLoading - exclude alarm loading from database
		  */
		void loadAlarmAgentData(bool bSkipAlarmLoading=false);

        // Disable copy constructor and assignment operator
        AlarmAgent( const AlarmAgent& theAlarmAgent);
        AlarmAgent& operator=(const AlarmAgent &);

        // The Generic Agent
        TA_Base_Bus::GenericAgent* m_genericAgent;

        // The Alarm Agent Impl that services client requests
        TA_Base_App::AlarmAgentImpl* m_alarmAgentImpl;

		// TD 14082 queue of updates from the control agent when waiting for cache and queue update
		std::queue<const TA_Base_Core::StateUpdateMessageCorbaDef*> m_updatesQueue;
        // This thread lock ensures there is no concurrent access to the update queue.
        TA_Base_Core::ReEntrantThreadLockable m_updateQueueLock;		
    };
}

#endif // !defined(ALARMAGENT)
