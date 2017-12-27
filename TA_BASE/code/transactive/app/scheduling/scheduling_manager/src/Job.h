/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_manager/src/Job.h $
  * @author:  San Teo
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Contains all data related to a job. 
  */

#if !defined(Job_F88638BB_825D_4173_9889_9F82695F1EE8__INCLUDED_)
#define Job_F88638BB_825D_4173_9889_9F82695F1EE8__INCLUDED_

#include "app/scheduling/scheduling_manager/src/Schedule.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerStructs.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include <string>
#include <vector>


namespace TA_Base_App
{
    class Job
    {
    public:
        /**
         * Constructs an instance with specified values
         */
        Job(const std::string& Id, 
            const std::string& type, 
            const std::string& description, 
            const bool isInhibited, 
            const std::vector<NameValuePair>& parameters, 
            const unsigned long profileId, 
            const unsigned long locationId, 
            const Schedule& schedule);


        /**
         * Constructs an instance using the the Corba object.  
         */
        Job(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaObject);


        /** 
          * Job
          *
          * Initialise a job with just the job id and the next start date
          *
          * @param corbaObject The object that contains the job id and next start date
          */
        Job(const TA_Base_Bus::SchedulingAgentCorbaDef::JobTimeCorbaDef& corbaObject);


        /**
         * Constructs an instance using the jobInfo string.  It will parse the job type
         * and parameters from the string and sets the members accordingly.  
         * The jobInfo is expected to tbe in the form of:
         *    <Type>:<ParameterName1>/<ParameterValue1>,<ParameterName2>/<ParameterValue2> ...
         *
         */
        Job(const std::string& jobInfo);


        /** 
          * Job
          *
          * Default constructor
          *
          */
        Job();


        /** 
          * Job
          *
          * Copy Constructor
          *
          * @param job The job to copy from
          */
        Job(const Job& job);


        /** 
          * operator==
          *
          * Returns true if the jobs are equal
          *
          * @param job The job to compare with
          *
          * @return true if jobs are equal
          */
        bool operator==(const Job& job) const;


        /** 
          * operator=
          *
          * The assignment operator.
          *
          * @param rhs The Job that we're copying from
          *
          * @return the current job
          */
        const Job& operator=(const Job& rhs);


        /** 
          * ~Job
          *
          * Destructor
          *
          */
	    virtual ~Job();


        /**
         * Returns a JobCorbaDef containing the information in this object. 
         */
        TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef toCorbaObject() const;


        /** 
          * update
          *
          * This method is used to update all details of the Job object except
          * for the next schedule date.  
          *
          * @param corbaObject
          */
        void update(TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaObject);


        /**
         * Gets the inhibition status of this job.
         * @return  true if the job is being inhibited, false otherwise. 
         */
        bool isInhibited() const
        {
            return m_isInhibited;
        }


        /**
         * Returns an Id unique to all IJob objects. 
         */
        std::string getId() const
        {
            return m_id;
        }


        /**
         * Returns true if the job is new
         */
        bool isNew() const
        {
            return ( m_id.compare( SchedulingManager::UNSCHEDULED_JOBID ) == 0 );
        }


        /**
         * Gets the type of job that this is, eg PlanRunnerJob 
         */
        std::string getType() const
        {
            return m_type;
        }


        /**
         * Gets the Id of the location associated with the profile used by this job, or 0
         * is there isn't one. 
         */
        unsigned long getLocationId() const
        {
            return m_locationId;
        }


        /**
         * Gets the description of this job. 
         */
        std::string getDescription() const
        {
            return m_description;
        }


        /**
         * Gets the Id of the profile used by this job. 
         */
        unsigned long getProfileId() const
        {
             return m_profileId;
        }


        /**
         * Gets the Schedule associated with this job. 
         */
        Schedule& getSchedule()
        {
            return m_schedule;
        }
        

        /** 
          * getParameters
          *
          * Returns the name value pairs representing the job specific parameters
          *
          *
          * @return The name value pairs representing the job specific parameters
          */
        const std::vector<NameValuePair> getParameters() const
        {
            return m_parameter;
        }


        /** 
          * getStartDate
          *
          * Returns the start date as given by the agent.  If the job has already
          * finished all its schedules, it will return NULL.
          *
          * @return The start date
          */
        COleDateTime* getStartDate() const
        {
            return m_startDate;
        }


        /**
         * Sets the Id of this job.
         */
        void setId(const std::string& jobId)
        {
            m_id = jobId.c_str();
        }


        /**
         * Sets the inhibition status of this job.
         */
        void setInhibited(const bool inhibited)
        {
            m_isInhibited = inhibited;
        }


        /**
         * Sets the type of job that this is, eg PlanRunnerJob 
         */
        void setType(const std::string& type)
        {
            m_type = type;
        }


        /**
         * Sets the Id of the location associated with the profile used by this job, or 0
         * is there isn't one. 
         */
        void setLocationId(const unsigned long locationId)
        {
            m_locationId = locationId;
        }


        /**
         * Sets the description of this job. 
         */
        void setDescription(const std::string& description)
        {
            m_description = description;
        }


        /**
         * Sets the Id of the profile used by this job. 
         */
        void setProfileId(const unsigned long profileId)
        {
            m_profileId = profileId;
        }


        /**
         * Sets the Schedule associated with this job. 
         */
        void setSchedule(const Schedule& schedule)
        {
            m_schedule = schedule;
        }


        /**
         * Sets the parameters associated with this job. 
         */
        void setParameters(const std::vector<NameValuePair>& params)
        {
            m_parameter = params;
        }


        /** 
          * setStartDate
          *
          * Sets the start date of this job
          *
          * @param startDate The corbo object that specifies the start date
          */
        void setStartDate(const TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef& startDate);

        void setJobData(const TA_Base_Bus::SchedulingAgentCorbaDef::JobData& jobData);


    private:
        std::string m_id;
        std::string m_type;
        std::string m_description;
        bool m_isInhibited;
        std::vector<NameValuePair> m_parameter;
        unsigned long m_profileId;
        unsigned long m_locationId;
        Schedule m_schedule;
        COleDateTime* m_startDate;
    };

    typedef std::vector<Job*> JobPtrVector;
	//typedef std::map<TA_Base_Bus::SchedulingAgentCorbaDef::JobID, Job*> JobPtrMap;
	typedef std::map<std::string, Job*> JobPtrMap;  // std::string JobID
};
#endif // !defined(Job_F88638BB_825D_4173_9889_9F82695F1EE8__INCLUDED_)
