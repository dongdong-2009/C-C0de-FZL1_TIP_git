/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingAgent.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  * Creates the Scheduling Agent and starts it.
  *
  */


#ifndef SCHEDULING_AGENT_H
#define SCHEDULING_AGENT_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_agent/src/IGenericAgentUser.h"


namespace TA_Base_Core
{

class IEntityData; // Forward Declaration

class ConfigUpdateDetails; // Forward Declaration

struct StateUpdateMessageCorbaDef;

}

namespace TA_Base_Bus
{

class GenericAgent; // Forward Declaration

class IEntity; // Forward Declaration

}

namespace TA_Base_App
{

class SchedulingAgent : public virtual TA_Base_Bus::IGenericAgentUser
{
public:

    /**
      * Constructor
      *
      * @param argc  The number of command line arguments passed in.
      * @param argv  The command line arguments.
      *
      * @exception GenericAgentException thrown if GenericAgent fails to
      *            initialise.
      * @exception DatabaseException thrown if a database error occurs.
      * @exception DataException thrown if data returned from the database
      *            is not valid.
      */
    SchedulingAgent(int argc, char* argv[]);

    /**
      * Destructor
      */
    virtual ~SchedulingAgent();


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
      * This method is called by the GenericAgent. Given that the Scheduling
      * Agent has no entities, it just returns a NULL pointer.
      *
      * @param entityData  Entity data retrieved from the database.
      *
      * @return The constructed entity
      */
    TA_Base_Bus::IEntity* createEntity(TA_Base_Core::IEntityDataPtr entityData);

    /**
      * agentSetMonitor
      *
      * This method is called by GenericAgent when the Alarm Agent operating
      * mode is set to Monitor.
      */
    void agentSetMonitor();

    /**
      * agentSetControl
      *
      * This method is called by GenericAgent when the Alarm Agent operating
      * mode is set to Control.
      */
    void agentSetControl();

    /**
      * registerForStateUpdates
      *
      * This pure virtual method forces agents to subscribe for either
      * state updates or state update requests, depending on whether or not
      * they are operating in monitor or control mode.
      */
    void registerForStateUpdates();

    /**
      * receiveSpecialisedMessage
      *
      * This method is called whenever the Agent receives a state update or
      * state update request (depending on their mode of operation)
      */
    void receiveSpecialisedMessage(
        const TA_Base_Core::StateUpdateMessageCorbaDef& message);

    /**
      * processOnlineUpdate
      *
      * This is called by Generic Agent whenever it gets an online update
      * that it cant handle itself.
      *
      * @param The update event
      */
    void processOnlineUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent);


    /**
     *  This method is not used by the Scheduling Agent
     */
    void notifyGroupOffline(const std::string& group);

    /**
     *  This method is not used by the Scheduling Agent
     */
    void notifyGroupOnline(const std::string& group);


    /**
      * run
      *
      * This method calls GenericAgent::run() which blocks until the System
      * Controller shuts us down
      */
    void run();
	
	/**
     *  Registers for messages that should be received in monitor mode.
     */
    void registerForMonitorMessages();

	/**
     *  Registers for messages that should be received in control mode.
     */
    void registerForControlMessages();
	
protected:

    /**
     *  Called when a batch update is received.
     */
    void receiveBatchUpdate(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

    /**
     *  Called when a batch update request is received.
     */
    void receiveBatchUpdateRequest(
        const TA_Base_Core::StateUpdateMessageCorbaDef& message);

    /**
     *  Sends a batch update.
     */
    void sendBatchUpdateMessage();

    /**
     *  Initialises the Agent in control mode.
     */
    void initialiseAgentControlMode();

    /**
     *  Initialises the Agent in monitor mode.
     */
    void initialiseAgentMonitorMode();


    /**
     *  Initialises the singletons used by the scheduling agent.
     *
     *  @exception DatabaseExceptions from entityAccess will not be caught here
     *  @exception DataExceptions from entityAccess will not be caught here
     */
    void initialiseSingletons();


private:

    // Disable copy constructor and assignment operator
    SchedulingAgent( const SchedulingAgent& theSchedulingAgent);
    SchedulingAgent& operator=(const SchedulingAgent &);

    
    // The run param representing the workstation ID
    static const char* RPARAM_WORKSTATIONID;

    // The Generic Agent
    TA_Base_Bus::GenericAgent* m_genericAgent;

    // This is used to lock the object when a batch update request is received
    // to ensure that initialisation has completed before the batch update
    // is sent
    TA_Base_Core::NonReEntrantThreadLockable m_sendBatchUpdateLock;

    // When the agent is initialised in monitor mode, indicates if a
    // batch update has been received from the controlling agent yet.
    volatile bool m_batchUpdateReceived;

};


}; // TA_Base_App

#endif // SCHEDULING_AGENT_H
