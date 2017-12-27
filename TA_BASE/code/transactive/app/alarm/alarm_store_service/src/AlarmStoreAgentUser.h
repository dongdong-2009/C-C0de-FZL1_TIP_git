/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_store_service/src/AlarmStoreAgentUser.h $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class provides the Generic Agent User interface that is used by the system controller
  * It will manage the instance of the store servant.
  */

#if !defined(ALARM_STORE_AGENT_USER)
#define ALARM_STORE_AGENT_USER

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined (_MSC_VER)

#include <string>
#include <map>
#include <vector>

#include "bus/generic_agent/src/IGenericAgentUser.h"
#include "bus/generic_service/src/IServiceAgentUser.h"

#include "bus/alarm/alarm_store_library/src/AlarmStoreServant.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreUpdateListener.h"

namespace TA_Base_App
{
	class AlarmStoreAgentUser : public virtual TA_Base_Bus::IServiceAgentUser
    {

		public:

			/**
			  * Constructor
			  */
			AlarmStoreAgentUser(int argc, char* argv[]);

			/**
			  * Destructor
			  */
			virtual ~AlarmStoreAgentUser();

			//TD18706,jianghp, handle the operation mode
			virtual void checkOperationMode()
			{
				//do nothing...
			}
		    //
		    // operations required by IServiceAgentUser
		    //

		    /**
              * start
              *
              * This pure virtual method is used to start any processing
		      * after the service agent is set up
              *
              */
		    void start();

		    //
		    // operations required by IGenericAgentUser
		    //

			/**
			  * agentTerminate
			  *
			  * This pure virtual method is called from the onTerminate
			  * method in GenericAgent. This allows the agent to do any
			  * of its own cleaning up before exiting.
			  *
			  * If the agent's entities implement CORBA interfaces,
			  * this method should take care of deactivating them. It is not
			  * possible for GenericAgent to do this because the IEntity 
			  * interface is not a ServantBase.
			  *
			  * If the class implementing this interface holds the GenericAgent
			  * as a member variable, DO NOT delete it inside this method.
			  * GenericAgent waits for this call to return before shutting down
			  * the ORB. Deleting the GenericAgent from within this method will
			  * cause obvious problems.
			  */
			virtual void agentTerminate();

			/**
			  * createEntity
			  *
			  * This method takes Entity Configuration data stored in the database and
			  * generates an object that implements the IEntity interface. The returned
			  * objects are stored and managed within Generic Agent. The Agent can get a 
			  * pointer to the objects via the getEntityMap method in GenericAgent.
			  *
			  * If create entity throws an exception when Generic Agent is creating its
			  * vector of entities (in the GenericAgent::doEntityConfiguration method), 
			  * the Generic Agent construction will fail.
			  *
			  * @param Entity data retrieved from the database.
			  *
			  * @return The constructed entity
			  */
			virtual TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

			/**
			  * agentSetMonitor
			  *
			  * This pure virtual method is called by GenericAgent whenever it receives
			  * a request from the Process Monitor to change its operating state to monitor.
			  * The derived agent should respond by subscribing/unsubscribing to/from the 
			  * appropriate message channels and performing any other internal house keeping.
			  */
			virtual void agentSetMonitor();

			/**
			  * agentSetControl
			  *
			  * This pure virtual method is called by GenericAgent whenever it receives
			  * a request from the Process Monitor to change its operating state to control.
			  * The derived agent should respond by subscribing/unsubscribing to/from the 
			  * appropriate message channels and performing any other internal house keeping.
			  */
			virtual void agentSetControl();

			/** 
			  * notifyGroupOffline
			  *
			  * This request is passed on by GenericAgent when it receives a call to 
			  * notifyGroupOffline from the Process Monitor.
			  *
			  * @param The group name
			  */
			virtual void notifyGroupOffline( const std::string& group );

			/** 
			  * notifyGroupOnline
			  *
			  * This request is passed on by GenericAgent when it receives a call to 
			  * notifyGroupOnline from the Process Monitor.
			  *
			  * @param The group name
			  */
			virtual void notifyGroupOnline( const std::string& group );

			/**
			  * registerForStateUpdates
			  *
			  * This pure virtual method forces agents to subscribe for either
			  * state updates or state update requests, depending on whether or not they
			  * are operating in monitor or control mode.
			  */
			virtual void registerForStateUpdates();

			/**
			  * receiveSpecialisedMessage
			  *
			  * This method is called whenever the Agent receives a state update or
			  * state update request (depending on their mode of operation)
			  */
			virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

			/**
			  * processOnlineUpdate
			  *
			  * Generic Agent will handle the receiving of online updates. However, in some
			  * cases, it will be necessary to pass the updates on to the registered agent.
			  * For example, if a child entity needs to be updated.
			  *
			  * @param The update event
			  */
			virtual void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);

		private:
				
			TA_Base_Bus::AlarmStoreServant*          m_alarmStoreServant;
            TA_Base_Bus::AlarmStoreUpdateListener*   m_alarmStoreUpdateListener;

    };
}

#endif // !defined(ALARM_STORE_AGENT_USER)
