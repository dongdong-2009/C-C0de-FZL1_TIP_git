/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  * 
  * Source:    $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_store_service/src/AlarmStoreAgentUser.cpp $
  * @author:   Ross Tucker
  * @version:  $Revision: #1 $
  * @see:      9999-Q01-02-0016-GenericAgent_Usage_Guidelines.doc
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last mofified by:  $Author: builder $
  *
  * This class provides the Generic Agent User interface that is used by the system controller
  * It will do nothing in this implementation.
  */

#pragma warning(disable:4284)   // return type for 'identifier::operator->' is not a UDT or reference to a UDT
#pragma warning(disable:4503)

#include "AlarmStoreAgentUser.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_App;


/**
  * Constructor
  */
AlarmStoreAgentUser::AlarmStoreAgentUser(int argc, char* argv[]) 
                    : m_alarmStoreServant(NULL)
{
}

/**
  * Destructor
  */
AlarmStoreAgentUser::~AlarmStoreAgentUser()
{

}

void AlarmStoreAgentUser::start()
{
}

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
void AlarmStoreAgentUser::agentTerminate()
{
	delete m_alarmStoreServant;
	m_alarmStoreServant=NULL;
    delete m_alarmStoreUpdateListener;
    m_alarmStoreUpdateListener = NULL;
}

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
TA_Base_Bus::IEntity* AlarmStoreAgentUser::createEntity(TA_Base_Core::IEntityDataPtr entityData)
{
	return NULL;
}

/**
  * agentSetMonitor
  *
  * This pure virtual method is called by GenericAgent whenever it receives
  * a request from the Process Monitor to change its operating state to monitor.
  * The derived agent should respond by subscribing/unsubscribing to/from the 
  * appropriate message channels and performing any other internal house keeping.
  */
void AlarmStoreAgentUser::agentSetMonitor()
{
}

/**
  * agentSetControl
  *
  * This pure virtual method is called by GenericAgent whenever it receives
  * a request from the Process Monitor to change its operating state to control.
  * The derived agent should respond by subscribing/unsubscribing to/from the 
  * appropriate message channels and performing any other internal house keeping.
  */
void AlarmStoreAgentUser::agentSetControl()
{
	if (m_alarmStoreServant==NULL)
	{
		// Get store entity name - run params should be setup now by service agent
		std::string  entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
		TA_ASSERT(entityName.size()>0,"No entity name for alarm store servant");
        m_alarmStoreUpdateListener = new TA_Base_Bus::AlarmStoreUpdateListener();
		m_alarmStoreServant = new TA_Base_Bus::AlarmStoreServant(entityName,*m_alarmStoreUpdateListener);
	}
}

/** 
  * notifyGroupOffline
  *
  * This request is passed on by GenericAgent when it receives a call to 
  * notifyGroupOffline from the Process Monitor.
  *
  * @param The group name
  */
void AlarmStoreAgentUser::notifyGroupOffline( const std::string& group )
{
}

/** 
  * notifyGroupOnline
  *
  * This request is passed on by GenericAgent when it receives a call to 
  * notifyGroupOnline from the Process Monitor.
  *
  * @param The group name
  */
void AlarmStoreAgentUser::notifyGroupOnline( const std::string& group )
{
}

/**
  * registerForStateUpdates
  *
  * This pure virtual method forces agents to subscribe for either
  * state updates or state update requests, depending on whether or not they
  * are operating in monitor or control mode.
  */
void AlarmStoreAgentUser::registerForStateUpdates()
{
}

/**
  * receiveSpecialisedMessage
  *
  * This method is called whenever the Agent receives a state update or
  * state update request (depending on their mode of operation)
  */
void AlarmStoreAgentUser::receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message)
{
}

/**
  * processOnlineUpdate
  *
  * Generic Agent will handle the receiving of online updates. However, in some
  * cases, it will be necessary to pass the updates on to the registered agent.
  * For example, if a child entity needs to be updated.
  *
  * @param The update event
  */
void AlarmStoreAgentUser::processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
}

