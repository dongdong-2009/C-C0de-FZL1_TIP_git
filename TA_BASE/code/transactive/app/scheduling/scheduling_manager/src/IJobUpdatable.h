/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/IJobUpdatable.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the interface through which the JobManager communicates with the document.  Whenever there is
  * any changes to the job list, the JobManager shall call the methods on this class to inform the 
  * document of the updates.
  */

#if !defined(IJobUpdatable_0C928AE0_FF72_4e24_A1E4_6D0C2C79BAED__INCLUDED_)
#define IJobUpdatable_0C928AE0_FF72_4e24_A1E4_6D0C2C79BAED__INCLUDED_

/**
 * The interface that will be called by the JobManager when there is a change in
 * the job list.
 */

namespace TA_Base_App
{
    class Job;

    class IJobUpdatable
    {

    public:
        
        virtual ~IJobUpdatable() {};

        /** 
          * updateJobs
          *
          * Calls by the JobManager to update the job list whenever there is a job update.
          *
          */
	    virtual void updateJobs() = 0;


        /** 
          * createJob
          *
          * Displays the job dialog for creating a job.
          *
          * @param job The job object that contains the preliminary data to be displayed on the dialog.
          */
	    virtual void createJob(const Job& job) = 0;


        /** 
          * updateRights
          *
          * Update the operator privileges on the selected job.
          *
          */
	    virtual void updateRights() = 0;


        /** 
          * jobDeleted
          *
          * This is called when a job has been removed from the job list.  
          *
          * @param job The job that has been removed.  
          * @param isAssociatedWithSessionProfiles True if the job is associated with profiles available to this session
          */
        virtual void jobDeleted(const Job& job, const bool isAssociatedWithSessionProfiles) = 0;


        /** 
          * jobUpdated
          *
          * This is called when a job has been updated in the job list.  
          *
          * @param job The job that has updated
          * @param isAssociatedWithSessionProfiles True if the job is associated with profiles available to this session
          */
        virtual void jobUpdated(const Job& job, const bool isAssociatedWithSessionProfiles) = 0;
    };
};
#endif // !defined(IJobUpdatable_0C928AE0_FF72_4e24_A1E4_6D0C2C79BAED__INCLUDED_)
