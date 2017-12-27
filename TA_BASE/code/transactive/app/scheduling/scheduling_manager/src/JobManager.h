/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_manager/src/JobManager.h $
  * @author:  San Teo
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Keeps the list of jobs and handles the following business logics:
  * - Schedule/modify/delete a job via the Scheduling Agent
  * - Handle updates from the Scheduling Agent
  * - Handle run param changes.
  *
  */

#if !defined(JobManager_0F5A4B6C_BD42_4de3_B2E0_C7554C8857FA__INCLUDED_)
#define JobManager_0F5A4B6C_BD42_4de3_B2E0_C7554C8857FA__INCLUDED_

#include "app/scheduling/scheduling_manager/src/Job.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h"
#include <string>
#include <map>
#include "bus/scheduling/schedulingagentcorbadef/IDL/src/SchedulingAgentCorbaDef.h"
#include "core/message/src/SpecialisedMessageSubscriber.h"
#include "core/message/IDL/src/CommsMessageCorbaDef.h"
#include "core/naming/src/NamedObject.h"
#include "core/utilities/src/RunParams.h"

namespace TA_Base_Bus
{
    class RightsLibrary;
    class AuthenticationLibrary;
}

namespace TA_Base_App
{
    class TA_SchedulingAgent;
    class IJobUpdatable;

    class JobManager : public TA_Base_Core::RunParamUser, 
                       public TA_Base_Core::SpecialisedMessageSubscriber<TA_Base_Core::CommsMessageCorbaDef>
    {

    public:

        /** 
          * JobManager
          *
          * Constructs the object that communicates with the agent to handle job logics.
          *
          * @param jobUpdatable The callback object that will be called by this object when there is any 
          *        changes to the job list
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          */
        JobManager(TA_Base_App::IJobUpdatable* jobUpdatable = NULL);


        /** 
          * ~JobManager
          *
          * Desctructor
          *
          */
	    virtual ~JobManager();


        /** 
          * onRunParamChange
          *
          * The method that is called when there is a change in the registered runparams (JobInfo and SessionId)
          *
          * @param name The name of the runparam
          * @param value The new value of the runparam
          *
          */
        virtual void onRunParamChange(const std::string& name, const std::string& value);


        /**
         * receiveSpecialisedMessage  This is overriden by the client to receive their
         * specialise message
         * @param	T	The narrowed Message
         */
        virtual void receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message);


        /** 
          * saveJob
          *
          * Adds the job to the agent's schedule if it's not already there, update it otherwise.
          *
          * @param job The job that is going to be added or updated
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
        void saveJob(const Job& job);


        /** 
          * saveNewJob
          *
          * Adds the job to the agent's schedule if it is not identical to the existing job that 
          * has the specified job's Id.
          *
          * @param job The job that is going to be added
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
        void saveNewJob(Job& job);


        /** 
          * deleteJob
          *
          * Removes the specified job from the agent's scheduling queue.
          *
          * @param id The id of the job to delete
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
        void deleteJob(const std::string& id);


        /** 
          * enableJob
          *
          * Enables/disables the specified job.
          *
          * @param id The id of the job to enable/disable
          * @param enable The enabling condition
          *
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
        void enableJob(const std::string& id, const bool enable);

		//TD16352++
		/** 
          * updateEditLock
          *
          * Enables/disables the job lock
          *
          * @param id The id of the job to enable/disable
          * @param enable The enabling condition
          *
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
		void updateEditLock(const std::string& jobId, const bool bLock);

		/** 
          * isJobLockedForEdit
          *
          * checks wether a job is locked for edit
          *
          * @param id The id of the job to enable/disable
          *
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
		bool isJobLockedForEdit(const std::string& jobId);
		//++TD16352


        /** 
          * getAllJobs
          *
          * Returns all jobs
          *
          *
          * @return all jobs
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
        JobPtrMap getAllJobs() const;


        /** 
          * getAllJobsByProfile
          *
          * Returns all jobs that the operator has the privilege to handle 
          *
          *
          * @return all jobs that the operator has the privilege to handle 
          */
        JobPtrMap getAllJobsByProfile() const;


        /** 
          * getJobTypes
          *
          * Returns all job types the scheduling agent can handle.
          *
          *
          * @return all job types the scheduling agent can handle.
          *
          * @throw SchedulingManagerExceptions All exceptions are converted into this type of exception
          *
          */
        std::vector<std::string> getJobTypes();


        /** 
          * setJobUpdatable
          *
          * Sets the object that will be notified of changes to the job list.
          *
          * @param jobUpdatable The object that wishes to be informed of any job changes
          */
        void setJobUpdatable(TA_Base_App::IJobUpdatable& jobUpdatable)
        {
            m_jobUpdatable = &jobUpdatable;
        }


        /** 
          * isActionOnJobPermitted
          *
          * Returns true if the operator has the rights to perform the action *AND*
          * one of operator's profile is the profile associated with the job.
          *
          * @param action The action to be performed on the job
          * @param job The job
          *
          * @return true if the operator has the rights to perform the action *AND*
          * one of operator's profile is the profile associated with the job.
          *
          */
        bool isActionOnJobPermitted(SchedulingManager::EAction action, JobPtrVector* job);


        /** 
          * getSessionProfiles
          *
          * All profile keys associated with this profile.  It also returns the 
          * super profile if the operator has rights to use that
          *
          * @return All profile keys associated with this profile
          */
        std::vector<unsigned long> getSessionProfiles() const
        {
            return m_sessionProfiles;
        }


        /** 
          * getSessionLocation
          *
          * Returns the session location
          *
          *
          * @return the session location
          */
        unsigned long getSessionLocation() const
        {
            return m_sessionLocation;
        }


        /** 
          * getAllOperatorProfiles
          *
          * Returns all profiles that are associated with the login operator
          *
          *
          * @return all profiles that are associated with the login operator
          */
        ProfileVector getAllOperatorProfiles();


        /** 
          * getAllSchedulableProfiles
          *
          * Returns a map of profiles that could be used to schedule a job
          *
          *
          * @return a map of profiles that could be used to schedule a job
          */
        ProfileMap getAllSchedulableProfiles() const
        {
            return m_schedulableProfileMap;
        }


        /** 
          * getAllLocations
          *
          * Returns a map of all locations
          *
          *
          * @return a map of all locations
          */
        LocationMap getAllLocations() const
        {
            return m_locationMap;
        }


        /** 
          * getTotalJobCount
          *
          * Returns the total number of jobs
          *
          *
          * @return the total number of jobs
          */
        unsigned long getTotalJobCount() const;


        /** 
          * getTotalJobCountByProfile
          *
          * Returns the total number of jobs that are associated with the login profile
          *
          *
          * @return the total number of jobs that are associated with the login profile
          */
        unsigned long getTotalJobCountByProfile() const;


        /** 
          * getLastUpdateTime
          *
          * The last time that the job list has been updated
          *
          *
          * @return The last time that the job list has been updated
          */
        time_t getLastUpdateTime() const
        {
            return m_lastUpdateTime;
        }

    private:
        //
        // Hide the copy constructor and assignment operator as they
        // are not needed.
        //
        JobManager( const JobManager& theJobManager);
        JobManager& operator=(const JobManager &);

        /** 
          * loadProfileList
          *
          * Loads all profiles associated with the operator.
          *
          * @throw SchedulingManagerException::UNKNOWN_EXCEPTION when the application couldn't 
          * retrieve the session profiles from the session Id.
          *
          */
        void loadProfileList();


        /** 
          * operatorHasJobProfile
          *
          * Returns true if the operator has the profile.used to create the job
          *
          * @param job The job
          *
          * @return true if the operator has the specified profile.
          */
        bool operatorHasJobProfile(const Job& job);

        
        /** 
          * cleanUp
          *
          * Clears the memory and remove all subscriptions
          *
          */
        void cleanUp();


        /** 
          * canUseSuperProfile
          *
          * If the session has rights to use the super profile, this method will return true
          *
          *
          * @return true if session ahs rights to use the super profile
          */
        bool canUseSuperProfile();

	    
        // This is the interface that wishes to be informed of any job changes
        TA_Base_App::IJobUpdatable* m_jobUpdatable;

        // Talk to the rights agent through this
        TA_Base_Bus::RightsLibrary* m_rightsMgr;

        // This is used to get the current session profiles
        TA_Base_Bus::AuthenticationLibrary* m_authMgr;

        // The login operator for the session
        TA_Base_Core::IOperator* m_sessionOperator;

        // The login operator for the session
        TA_Base_Core::IProfile* m_transActiveSuperProfile;

	    // Guards the job/profileJob list.
	    mutable TA_Base_Core::NonReEntrantThreadLockable m_jobLock;

	    // Stores the complete job list.
        JobPtrMap m_jobs;

        // Stores the jobs that are configured with the session profiles
        JobPtrMap m_profileJobs;

        // This is used to communicate with the scheduling agent
	    typedef TA_Base_Core::NamedObject<TA_Base_Bus::SchedulingAgentCorbaDef,
		    TA_Base_Bus::SchedulingAgentCorbaDef_ptr,
		    TA_Base_Bus::SchedulingAgentCorbaDef_var> SchedulingAgentNamedObj;
        SchedulingAgentNamedObj m_scheduler;

        // Keep a local copy of the session Id so that we don't have to get it from 
        // the run params each time we need it
        std::string m_sessionId;

        // True if server is up.  If this value is false, then all access related actions are disallowed
        bool m_isServerUp;

        // The resource of the Scheduling Manager that is required by the Rights Library
        unsigned long m_resourceKey;

        // All profile keys associated with this session
        std::vector<unsigned long> m_sessionProfiles;

        // The login location for the session
        unsigned long m_sessionLocation;

        // Location map that caches all locations so that this class doesn't need
        // to get associated locations from the database each time the profile changes
        LocationMap m_locationMap;

        // Profile map that caches all non-system profiles 
        ProfileMap m_nonSystemProfileMap;

        // Profile map that caches all profiles that could be used to schedule a job
        ProfileMap m_schedulableProfileMap;

        // The timestamp for the last job list update
        time_t m_lastUpdateTime;
        static const int CORBA_TIMEOUT;
    };

};
#endif // !defined(JobManager_0F5A4B6C_BD42_4de3_B2E0_C7554C8857FA__INCLUDED_)
