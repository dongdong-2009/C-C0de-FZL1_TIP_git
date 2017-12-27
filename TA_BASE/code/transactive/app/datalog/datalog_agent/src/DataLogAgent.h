///////////////////////////////////////////////////////////
//  DataLogAgent.h
//  Implementation of the Class DataLogAgent
//  Created on:      13-Ê®¶þÔÂ-2010 11:16:03
//  Original author: luohuirong
///////////////////////////////////////////////////////////

#if !defined(EA_D3851E74_0EE7_4ee8_BF44_FE96B5E5B076__INCLUDED_)
#define EA_D3851E74_0EE7_4ee8_BF44_FE96B5E5B076__INCLUDED_

#include "bus/generic_agent/src/IGenericAgentUser.h"
 

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
namespace TA_Base_Bus
{
	class DataLogAgentImpl; // Forward Declaration
}

namespace TA_Base_App
{
	
	class DataLogAgent : public virtual TA_Base_Bus::IGenericAgentUser
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
        DataLogAgent(int argc, char* argv[]);
        /**
		* Destructor
          */
		virtual ~DataLogAgent();
		/**
		* agentTerminate
		*
		* This method is called by the GenericAgent when it is time to
		* terminate.
		  */
		void agentTerminate();
	 
		
		virtual void checkOperationMode()
		{ 
		}
 
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
		* processMessageUpdate
		*
		* This method process the message submitted which can be a state update message		 
		*
		* @param TA_Base_Core::StateUpdateMessageCorbaDef&. The update message
		*/
		void processMessageUpdate(const TA_Base_Core::StateUpdateMessageCorbaDef& message);
		
        void unsubscribeToMessages(); //limin, failover
        // Disable copy constructor and assignment operator
        DataLogAgent( const DataLogAgent& theDataLogAgent);
        DataLogAgent& operator=(const DataLogAgent &);
		
        // The Generic Agent
        TA_Base_Bus::GenericAgent* m_genericAgent;
		 
		 

	};
}
#endif // !defined(EA_D3851E74_0EE7_4ee8_BF44_FE96B5E5B076__INCLUDED_)
