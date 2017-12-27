/**
  * The source code in this file is the property of
  * Combuilder PTE and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/event/event_agent/src/EventAgent.h $
  * @author:   Noel R. Tajanlangit
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2013/02/06 16:15:14 $
  * Last modified by:  $Noel R. Tajanlangit $
  *
  */

#ifndef __EVENT_AGENT_H_INCLUDED__
#define __EVENT_AGENT_H_INCLUDED__

#include "app/event/event_agent/src/AgentStateController.h"

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "core/event/IDL/src/EventDetailCorbaDef.h"

#include <queue>
#include <string>

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
	class EventAgentImpl; // Forward Declaration

	class EventAgent :
		 public virtual TA_Base_Bus::IGenericAgentUser
	{
	public: //Methods
		 
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
		EventAgent( int argc, char* argv[] );
		
		/**
          * Destructor
          */
		virtual ~EventAgent();
	
		// Generic Agent virtual function implementation
		
		// handle the operation mode
		virtual void checkOperationMode(){};
	
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
          * This method is called by the GenericAgent. Given that the Event
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
          * This method is called by GenericAgent when the Event Agent operating mode is
          * set to Monitor.
          */
        void agentSetMonitor();

        /**
          * agentSetControl
          *
          * This method is called by GenericAgent when the Event Agent operating mode is
          * set to Control.
          */
        void agentSetControl();

		/**
          * agentSetControl
          *
          * This method is called by GenericAgent when the Event Agent operating mode is
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

		/*
		 * run
		 *
		*/
		void run();

		/**
		 * processStateChange
		 *
		 * Performs the state change.
		 */
		void processStateChange(const std::string& oldState, const std::string& newState);

	
	
	private: //Methods
		// Disable copy constructor and assignment operator
		EventAgent( const EventAgent& theEventAgent);
		EventAgent& operator=(const EventAgent &);

		/**
		 * setupRunParams
		 */
		void setupRunParams();
			
		/*
		 * initialize
		 *
		 * Performs all the necessary initialization
		*/
		void initialize();

	private: //Variables
		// The Generic Agent
		TA_Base_Bus::GenericAgent* m_genericAgent;

		// The event Agent Implementation that services client requests
		TA_Base_App::EventAgentImpl* m_eventAgentImpl;

		// TD 14082 queue of updates from the control agent when waiting for cache and queue update
		std::queue<const TA_Base_Core::StateUpdateMessageCorbaDef*> m_updatesQueue;
		
		// This thread lock ensures there is no concurrent access to the update queue.
		TA_Base_Core::ReEntrantThreadLockable m_updateQueueLock;	

		// Event Agent state controller
		AgentStateController m_myStateController;

	};
}


#endif //__EVENT_AGENT_H__