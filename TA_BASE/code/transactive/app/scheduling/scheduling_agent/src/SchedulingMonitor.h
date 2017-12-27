/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingMonitor.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class encapsulates the logic for the Scheduling Agent in monitor
  * mode.
  *
  */


#ifndef SCHEDULING_MONITOR_H
#define SCHEDULING_MONITOR_H

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/message/IDL/src/StateUpdateMessageCorbaDef.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"




namespace TA_Base_App
{


class SchedulingMonitor : public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>,
    public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::StateUpdateMessageCorbaDef>
{

public:

    /**
     *  Gets the single instance of this class.
     */
    static SchedulingMonitor& getInstance();
	
    /**
     *  Initialises the object.
     *
     *  This method is used instead of a constructor.
     *  It must be called the first time getInstance is invoked, or
     *  subsequent invocations will cause an assertion.
     *
     *  @param entityKey     the entity key of the Scheduling Agent.
     *  @param subsystemKey  the subsystem key of the Scheduling Agent.
     *  @param locationKey   the location key of the Scheduling Agent.
     */
    void SchedulingMonitor::initialise(unsigned long entityKey, unsigned long subsystemKey, unsigned long locationKey);

    /**
     *  Called when the Controller sends a batch update of its state.
     *
     *  @param jobs  the jobs that are currently scheduled.
     */
    void batchUpdateReceivedFromController(const TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence& jobs);
	

    /**
     *  Called to activate the monitor.
     */
	void activate();
    

    /**
     *  Called to deactivate the monitor.
     */
	void deactivate();


    /**
     *  Called when a Comms message arrives from the controller.
     */
	virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);


    /**
     *  Called when a StateUpdate message arrives from the controller.
     */
	virtual void receiveSpecialisedMessage(const TA_Base_Core::StateUpdateMessageCorbaDef& message);

    
    /**
     *  Destructor.
     */
    virtual ~SchedulingMonitor();


protected:


    /**
     *  Called when a Job Added message is received.
     */
    void SchedulingMonitor::receiveJobAddedMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaJob);

    /**
     *  Called when a Job Updated message is received.
     */
    void SchedulingMonitor::receiveJobUpdatedMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& updatedJob);

    /**
     *  Called when a Job Deleted message is received.
     */
    void SchedulingMonitor::receiveJobDeletedMessage(const std::string& jobID);

    /**
     *  Called when a Job Starting message is received.
     */
    void SchedulingMonitor::receiveJobStartingMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef& jobInstance);

    /**
     *  Called when a Job Completed message is received.
     */
    void SchedulingMonitor::receiveJobCompletedMessage(const TA_Base_Bus::SchedulingAgentCorbaDef::JobInstanceCorbaDef& jobInstance);


private:

   	SchedulingMonitor();

    // Disabled
    SchedulingMonitor(const SchedulingMonitor&);
    SchedulingMonitor operator=(const SchedulingMonitor&);

    /** The single instance of this class */
    static SchedulingMonitor* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;


    /** The entity, subsystem and location keys of the Scheduling Agent */
    unsigned long m_entityKey;
    unsigned long m_subsystemKey;
    unsigned long m_locationKey;
};


}; // TA_Base_App

#endif // SCHEDULING_MONITOR_H
