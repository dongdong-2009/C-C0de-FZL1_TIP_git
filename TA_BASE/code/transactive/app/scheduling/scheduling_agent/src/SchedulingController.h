/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/SchedulingController.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #4 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  *
  * This class encapsulates the logic for the Scheduling Agent in control
  * mode.
  *
  */


#ifndef SCHEDULING_CONTROLLER_H
#define SCHEDULING_CONTROLLER_H


#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/corba/src/ServantBase.h"

#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"


namespace TA_Base_App
{

class SchedulingController : public virtual POA_TA_Base_Bus::SchedulingAgentCorbaDef, public virtual TA_Base_Core::ServantBase
{

public:

    static SchedulingController& getInstance();
	

    /**
     *  Enables the object, so that it handles processing instead of
     *  the monitor.
     */
    void enable();

    /**
     *  Disables the object, so that the monitor will handle processing.
     */
    void disable();

	

protected:

    /////////////////////////////////////////////////////////////////////////
    //                       SchedulingAgentCorbaDef                       //
    /////////////////////////////////////////////////////////////////////////


    /**
    *   get the list of all supported job types
    */
    TA_Base_Bus::SchedulingAgentCorbaDef::JobTypeSeq* getSupportedJobTypes( const char* sessionIdentifier );

    /**
     *  Adds the given job to the queue of scheduled jobs.
     */
    char* scheduleJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& scheduledJob, const char* sessionIdentifier );


    /**
     *  Remove the job with the given ID from the queue of scheduled
     *  jobs.
     */
    void removeJob( const char* jobID, const char* sessionID );


    /**
     *  Updates a job description and schedule.
     */
    void updateJob( const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job, const char* sessionIdentifier,
        CORBA::Boolean  changeEnabled, CORBA::Boolean  checkIfInPast = true);

	//TD16352++
	/**
	*   Lock the job for edit
    */
	void SchedulingController::updateEditLock( const char* jobID, const char* sessionID, CORBA::Boolean  bLock);

	bool SchedulingController::isJobLockedForEdit( const char* jobID, const char* sessionID);
	//++TD16352
    
    /** 
      * Gets the job with the given ID.
      */
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef* SchedulingController::getJob( const char* jobID, const char* sessionID );

    /**
	 *  Gets all scheduled jobs that have not expired.
	 */
    TA_Base_Bus::SchedulingAgentCorbaDef::JobSequence* getAllJobs( const char* sessionID );

private:


    /**
     *  Checks if the operation mode is monitor. If so, throws an exception
     *
     *  @exception OperationModeException
     *  Thrown if the Agent is not in control mode
     */
    void checkOperationMode();

   	SchedulingController();
	virtual ~SchedulingController();

    // Disabled
    SchedulingController(const SchedulingController&);
    SchedulingController operator=(const SchedulingController&);


    /** The single instance of this class */
    static SchedulingController* m_instance;

    /** The lock used to stop multiple access to the instance of this class */
    static TA_Base_Core::NonReEntrantThreadLockable m_singletonLock;

    bool m_bEnabled;
};


}; // TA_Base_App

#endif // SCHEDULING_CONTROLLER_H
