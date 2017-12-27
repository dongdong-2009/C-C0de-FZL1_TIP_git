/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  San Teo
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * Contains all data related to a job. 
  */

#include "app/scheduling/scheduling_manager/src/Job.h"
#include "core/utilities/src/DebugUtil.h"


namespace TA_Base_App
{
    Job::Job(const std::string& id, const std::string& type, const std::string& description, bool isInhibited, 
        const std::vector<NameValuePair>& parameters, const unsigned long profileId, const unsigned long locationId, const Schedule& schedule) :
    m_id(id.c_str()),
    m_type(type),
    m_description(description), 
    m_isInhibited(isInhibited), 
    m_parameter(parameters), 
    m_profileId(profileId), 
    m_locationId(locationId),
    m_schedule(schedule),
    m_startDate(NULL)
    {
        FUNCTION_ENTRY("Constructor: using specified values");

        FUNCTION_EXIT;
    }
 

    Job::Job(const Job& job) :
    m_id( job.m_id.c_str()),
    m_type(job.m_type.c_str()),
    m_description(job.m_description.c_str()), 
    m_isInhibited(job.m_isInhibited), 
    m_parameter(job.m_parameter),
    m_profileId(job.m_profileId), 
    m_locationId(job.m_locationId),
    m_schedule(job.m_schedule),
    m_startDate(NULL)
    {
        FUNCTION_ENTRY("Constructor: copy Job");
        if (job.m_startDate != NULL)
        {
            m_startDate = new COleDateTime(*job.m_startDate);    
        }
        FUNCTION_EXIT;
    }

 
    Job::Job(const TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaObject) :
    m_id(corbaObject.identifier.in()),
    m_type(corbaObject.type.in()),
    m_description(corbaObject.description.in()), 
    m_isInhibited(corbaObject.isInhibited),
    m_profileId(corbaObject.profileIdentifier),
    m_locationId(corbaObject.locationIdentifier),
    m_schedule(corbaObject.schedule),
    m_startDate(NULL)
    {
        FUNCTION_ENTRY("Constructor: using JobCorbaDef");

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Job has the following parameters");

        setJobData(corbaObject.data);
        setStartDate(corbaObject.runTime);

        FUNCTION_EXIT;
    }


    Job::Job(const TA_Base_Bus::SchedulingAgentCorbaDef::JobTimeCorbaDef& corbaObject) :
    m_id(corbaObject.jobIdentifer.in()),
    m_type(""),  
    m_description(""), 
    m_isInhibited(false),
    m_profileId(0),
    m_locationId(0),
    m_startDate(NULL)
    {
        setStartDate(corbaObject.runTime);
    }


    Job::Job() :
    m_id(SchedulingManager::UNSCHEDULED_JOBID.c_str()),
    m_type(""),  
    m_description(""), 
    m_isInhibited(false),
    m_profileId(0),
    m_locationId(0),
    m_startDate(NULL)
    {
        FUNCTION_ENTRY("Constructor: default");

        FUNCTION_EXIT;
    }
 
 
    Job::Job(const std::string& jobInfo) :
    m_id(SchedulingManager::UNSCHEDULED_JOBID.c_str()),
    m_type(""),  
    m_description(""), 
    m_isInhibited(false),
    m_profileId(0),
    m_locationId(0),
    m_startDate(NULL)
    {
        FUNCTION_ENTRY("Constructor: JobInfo string");

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Using JobInfo string to construct job:");

        if (jobInfo.empty())
        {
            return;
        }

        // parse <JobType>:<ParameterName1>/<ParameterValue1>,<ParameterName2>/<ParameterValue2> ...
        // search for :/, delimiters
        std::string::size_type index = jobInfo.find(SchedulingManager::DELIMITER_JOBTYPE, 0);
        if (index == std::string::npos)
        {
            // can't find the job type delimiter, so can't determine the job, no need to continue
            return;
        }
        m_type = jobInfo.substr(0, index);
        LOGMORE(SourceInfo, "Type = %s", m_type.c_str());

        while ((index != std::string::npos) && (index < jobInfo.size()))
        {
            // find out the start position of the parameter value and the start position of the next parameter name
            std::string::size_type valueIndex = jobInfo.find(SchedulingManager::DELIMITER_JOBPARAMVALUE, index);
            if (valueIndex == std::string::npos)
            {
                // if there's parameter name but no value, then don't set it, just return
                return;
            }
            std::string::size_type nextIndex = jobInfo.find(SchedulingManager::DELIMITER_JOBPARAMNAME, valueIndex);

            if (nextIndex == std::string::npos)
            {
                // if we can't find any delimitors to parameter name, then we don't have any more name value pair
                // so just have the nextIndex as the end of the string
                nextIndex = jobInfo.size();
            }

            NameValuePair pair;
            pair.name = jobInfo.substr(index + 1, valueIndex - index - 1);
            pair.value = jobInfo.substr(valueIndex + 1, nextIndex - valueIndex - 1);
            m_parameter.push_back(pair);
            LOGMORE(SourceInfo, "Parameter: %s = %s", pair.name.c_str(), pair.value.c_str());

            index = nextIndex;
        }
        FUNCTION_EXIT;
    }


    bool Job::operator==(const Job& job) const
    {
        bool identical =  ((m_id.compare(job.m_id) == 0)  &&
                           (m_isInhibited == job.m_isInhibited) &&
                           (m_profileId == job.m_profileId) &&
                           (m_locationId == job.m_locationId) &&
                           (m_type.compare(job.m_type) == 0) &&
                           (m_description.compare(job.m_description) == 0) &&
                           (m_schedule == job.m_schedule) &&
                           (*m_startDate == *job.m_startDate));
        if (!identical)
        {
            return false;
        }

        // check the parameters
        std::vector<NameValuePair> param = job.getParameters();
        if (m_parameter.size() != param.size())
        {
            return false;
        }

        for(unsigned int i = 0; i < m_parameter.size(); i++)
        {
            if ((m_parameter[i].value != param[i].value) ||
               (m_parameter[i].name != param[i].name))
            {
                return false;
            }
        }
        return true;
    }


    const Job& Job::operator=(const Job& rhs)
    {
        // gurad against self assignment
        if (this != &rhs)
        {
            m_id = rhs.m_id.c_str();
            m_type = rhs.m_type.c_str();
            m_description = rhs.m_description.c_str();
            m_isInhibited = rhs.m_isInhibited;
            m_profileId = rhs.m_profileId;
            m_locationId = rhs.m_locationId;
            m_schedule = rhs.m_schedule;
            m_parameter = rhs.m_parameter;

            delete m_startDate;
            m_startDate = NULL;
            if (rhs.m_startDate != NULL)
            {
                m_startDate = new COleDateTime(*rhs.m_startDate);
            }
        }
        return *this;
    }


    Job::~Job()
    {
        delete m_startDate;
        m_startDate = NULL;
    }
 

    void Job::update(TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef& corbaObject)
    {
        FUNCTION_ENTRY("Constructor: copy JobCorbaDef");

        // update everything except for the start date
        m_type = corbaObject.type.in();
        m_description = corbaObject.description.in(); 
        m_isInhibited = corbaObject.isInhibited;
        m_profileId = corbaObject.profileIdentifier;
        m_locationId = corbaObject.locationIdentifier;
        m_schedule = corbaObject.schedule;

        setJobData(corbaObject.data);

        FUNCTION_EXIT;
    }

 
    TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef Job::toCorbaObject() const
    {
        FUNCTION_ENTRY("toCorbaObject");

        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Job details");
        LOGMORE ( SourceInfo, "Id = %s", m_id.c_str());
        LOGMORE ( SourceInfo, "Type = %s", m_type.c_str());//TD14337 yezh++
        LOGMORE ( SourceInfo, "Description = %s", m_description.c_str());//TD14337 yezh++
        LOGMORE ( SourceInfo, "IsInhibited = %d", m_isInhibited);
        LOGMORE ( SourceInfo, "Profile = %d", m_profileId);
        LOGMORE ( SourceInfo, "Location = %d", m_locationId);

        TA_Base_Bus::SchedulingAgentCorbaDef::JobCorbaDef corbaObject;
        corbaObject.identifier = m_id.c_str(); 
        corbaObject.type = m_type.c_str();  
        corbaObject.description = m_description.c_str(); 
        corbaObject.isInhibited = m_isInhibited;
        corbaObject.profileIdentifier = m_profileId;
        corbaObject.locationIdentifier = m_locationId;

        corbaObject.data.length(m_parameter.size());
        for(unsigned int i = 0; i < m_parameter.size(); i++)
        {
            corbaObject.data[i].name = m_parameter[i].name.c_str();
            corbaObject.data[i].value = m_parameter[i].value.c_str();
            LOGMORE ( SourceInfo, "Parameter %d: %s = %s", i, m_parameter[i].name.c_str(), m_parameter[i].value.c_str());
        }

        corbaObject.schedule = m_schedule.toCorbaObject();

        // just initialise all so that we don't get corba error
        corbaObject.runTime.dayOfMonth = 0;
        corbaObject.runTime.month = TA_Base_Bus::SchedulingAgentCorbaDef::January;
        corbaObject.runTime.year = 0;
        corbaObject.runTime.hour = 0;
        corbaObject.runTime.minute = 0;

        FUNCTION_EXIT;
        return corbaObject;
    }

  
    void Job::setStartDate(const TA_Base_Bus::SchedulingAgentCorbaDef::RunTimeCorbaDef& startDate)
    {
        // If the start date given by the agent is not 0-0-0, then we can set the m_startDate to NULL
        if ((startDate.year == 0) && (startDate.month == 0) && (startDate.dayOfMonth == 0))
        {
            delete m_startDate;
            m_startDate = NULL;
        }
        else
        {
            if (m_startDate == NULL)
            {
                m_startDate = new COleDateTime(startDate.year, startDate.month+1, startDate.dayOfMonth, startDate.hour, startDate.minute, 0);
            }
            else
            {
                m_startDate->SetDateTime(startDate.year, startDate.month+1, startDate.dayOfMonth, startDate.hour, startDate.minute, 0);
            }
        }
    }

    void Job::setJobData( const TA_Base_Bus::SchedulingAgentCorbaDef::JobData& jobData )
    {
        m_parameter.clear();

        for(unsigned int i = 0; i < jobData.length(); i++)
        {
            NameValuePair pair;
            pair.name = jobData[i].name.in();
            pair.value = jobData[i].value.in();
            m_parameter.push_back(pair);
            LOGMORE(SourceInfo, "%s = %s", pair.name.c_str(), pair.value.c_str());
        }
    }
};
