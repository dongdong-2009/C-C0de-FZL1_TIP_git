/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/IJobFactory.h $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  *
  * This interface is used to create instances of a particular IJob subclass.
  *
  */


#ifndef I_JOB_FACTORY_H
#define I_JOB_FACTORY_H

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/ISchedulingAgentEntityData.h"
#include "bus/scheduling/SchedulingAgentCorbaDef/IDL/src/SchedulingAgentCorbaDef.h"
#include "app/scheduling/scheduling_agent/src/IJob.h"


namespace TA_Base_App
{

class IJobFactory
{

public:

    /**
     *  Initialises the job factory.
     *  This is called when the Scheduling Agent is first created.
     *  Any Run Params etc that need to be extracted should be done here.
     */
    virtual void initialise( TA_Base_Core::ISchedulingAgentEntityData& entityData ) = 0;


    /**
     *  Creates a new IJob from the JobCorbaDef and returns a pointer to it.
     *  The calling class is responsible for deleting the pointer.
     *  
     *  @return a pointer to a new IJob object, if the JobCorbaDef can be
     *          converted to the subclass of IJob this factory produces, or
     *          NULL if it cannot.
     */
    virtual IJob* createJob(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& job) = 0;


    /**
     *  Retrieves all jobs of the type this factory produces from persistent
     *  storage and creates new IJob objects from them.
     *  A vector of pointers to these objects is returned.
     *  The calling class is responsible for deleting the pointers.
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     */
    virtual IJob::IJobPtrVector createJobsFromStore() = 0;


    /**
     *  Retrieves the job with the given ID from persistent storage and
     *  creates a new IJob object for it.
     *  The calling class is responsible for deleting the pointer returned.
     *
     *  @return  a pointer to an IJob object, or NULL if there is no job with
     *           the given ID, of the subclass type.
     *           (eg no PlanRunnerJob with ID 10)
     *
     *  @exception BadScheduleException
     *  Thrown if the format of the schedule is invalid
     *  (eg it represents the 31st of February)
     *
     *  @exception DatabaseException
     *  A DatabaseException will be thrown if there is a problem establishing
     *  a connection with the database, or if an error is encountered while
     *  retrieving data.
     *
     *  @exception DataException
     *  A DataException will be thrown if the data cannot be  converted to
     *  the required format (e.g. MINUTE should be an unsigned short), or if
     *  the wrong amount of data is retrieved.
     */
    virtual IJob* getJobFromStore(const std::string& id) = 0;


    /**
     *  Destructor.
     */
    virtual ~IJobFactory() {}
};


};// TA_Base_App


#endif // I_JOB_FACTORY_H
