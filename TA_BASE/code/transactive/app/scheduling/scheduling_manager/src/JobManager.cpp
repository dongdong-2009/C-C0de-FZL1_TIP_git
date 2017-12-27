/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/JobManager.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Keeps the list of jobs and handles the following business logics:
  * - Schedule/modify/delete a job via the Scheduling Agent
  * - Handle updates from the Scheduling Agent
  * - Handle run param changes.
  *
  */

#include "app/scheduling/scheduling_manager/src/JobManager.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "app/scheduling/scheduling_manager/src/IJobUpdatable.h"
#include <algorithm>
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/SchedulingAgentEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/Session.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/types/SchedulingComms_MessageTypes.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/SchedulingManagerException.h"
#include "core/exceptions/src/RightsException.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using namespace TA_Base_App::SchedulingManager;
using TA_Base_App::IJobUpdatable;
using TA_Base_Bus::SchedulingAgentCorbaDef; 
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::SchedulingManagerException;
using namespace TA_Base_Core::SchedulingComms;
using TA_Base_Core::ThreadGuard;

namespace TA_Base_App
{
    const int JobManager::CORBA_TIMEOUT = 300;

    #define DO_CATCH(needCleanUp)                                                                                       \
        if (needCleanUp)                                                                                                \
        {                                                                                                               \
            cleanUp();                                                                                                  \
        }                                                                                                               \
    //end DO_CATCH

    // repackage all exceptions that could possibly occur into SchedulingManagerExceptions
    #define CATCH_ALL_EXCEPTIONS(needCleanUp)                                                                           \
        catch (TA_Base_Core::OperationModeException& ex)                                                                \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "OperationModeException", CORBA::string_dup(ex.reason));                    \
            TA_THROW(SchedulingManagerException("Converting OperationModeException to SCHEDULING_AGENT_FAILURE",        \
                    SchedulingManagerException::SCHEDULING_AGENT_FAILURE));                                             \
        }                                                                                                               \
        catch (TA_Base_Core::AccessDeniedException& ex)                                                                 \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "AccessDeniedException", CORBA::string_dup(ex.reason));                     \
            TA_THROW(SchedulingManagerException("Converting AccessDeniedException to ACCESS_DENIED",                    \
                    SchedulingManagerException::ACCESS_DENIED));                                                        \
        }                                                                                                               \
        catch (TA_Base_Core::DatabaseErrorException& ex)                                                                \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseErrorException", CORBA::string_dup(ex.reason));                    \
            TA_THROW(SchedulingManagerException("Converting DatabaseErrorException to DATABASE_ERROR",                  \
                    SchedulingManagerException::DATABASE_ERROR));                                                       \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::NoRightsAgentException& ex)                                                     \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "NoRightsAgentException", CORBA::string_dup(ex.what));                      \
            TA_THROW(SchedulingManagerException("Converting NoRightsAgentException to RIGHTS_AGENT_FAILURE",            \
                    SchedulingManagerException::RIGHTS_AGENT_FAILURE));                                                 \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::UnrecognisedJobTypeException& ex)                                               \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "UnrecognisedJobTypeException", CORBA::string_dup(ex.what));                \
            TA_THROW(SchedulingManagerException("Unrecognised Job type",                  \
            SchedulingManagerException::INVALID_JOB_PARAMETER));                                                       \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::JobInPastException& ex)                                                         \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "JobInPastException", CORBA::string_dup(ex.what));                          \
            TA_THROW(SchedulingManagerException("Converting JobInPastException to JOB_IN_PAST",                         \
                    SchedulingManagerException::JOB_IN_PAST));                                                          \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::BadScheduleException& ex)                                                       \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            switch (ex.reason)                                                                                          \
            {                                                                                                           \
                case SchedulingAgentCorbaDef::MissingDayOfWeek:                                                         \
                    LOG_EXCEPTION_CATCH(SourceInfo, "BadScheduleException", CORBA::string_dup(ex.what));                \
                    TA_THROW(SchedulingManagerException("Converting BadScheduleException:MissingDayOfWeek to NO_DAY_OF_WEEK", \
                            SchedulingManagerException::NO_DAY_OF_WEEK));                                               \
                    break;                                                                                              \
                case SchedulingAgentCorbaDef::InvalidMinute:                                                            \
                case SchedulingAgentCorbaDef::InvalidHour:                                                              \
                case SchedulingAgentCorbaDef::InvalidDate:                                                              \
                case SchedulingAgentCorbaDef::InvalidDayOfMonth:                                                        \
                case SchedulingAgentCorbaDef::InvalidMonth:                                                             \
                case SchedulingAgentCorbaDef::InvalidYear:                                                              \
                case SchedulingAgentCorbaDef::EndDateNotRequired:                                                       \
                default:                                                                                                \
                    LOG_EXCEPTION_CATCH(SourceInfo, "BadScheduleException", CORBA::string_dup(ex.what));                \
                    TA_THROW(SchedulingManagerException("Converting BadScheduleException to BAD_SCHEDULE",              \
                            SchedulingManagerException::BAD_SCHEDULE));                                               \
                    break;                                                                                              \
            }                                                                                                           \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::BadJobException& ex)                                                            \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "BadJobException", CORBA::string_dup(ex.what));                             \
            TA_THROW(SchedulingManagerException("Converting BadJobException to INVALID/MISSING_JOB_PARAMETER",          \
                (ex.reason == SchedulingAgentCorbaDef::MissingParameter) ? SchedulingManagerException::MISSING_JOB_PARAMETER : SchedulingManagerException::INVALID_JOB_PARAMETER)); \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::JobDoesNotExistException& ex)                                                   \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "JobDoesNotExistException", CORBA::string_dup(ex.what));                    \
            TA_THROW(SchedulingManagerException("Converting JobDoesNotExistException to JOB_NOT_EXIST",                 \
                    SchedulingManagerException::JOB_NOT_EXIST));                                                        \
        }                                                                                                               \
        catch (SchedulingAgentCorbaDef::TooManyJobsException & ex)                                                      \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "TooManyJobsException", CORBA::string_dup(ex.what));                        \
            TA_THROW(SchedulingManagerException("Converting TooManyJobsException to TOO_MANY_JOBS",                     \
                    SchedulingManagerException::TOO_MANY_JOBS));                                                        \
        }                                                                                                               \
		catch (SchedulingAgentCorbaDef::EditLockException & ex)                                                         \
		{                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "EditLockException", CORBA::string_dup(ex.what));                           \
            TA_THROW(SchedulingManagerException("Converting EditLockException to EDIT_LOCK",                            \
                    SchedulingManagerException::EDIT_LOCK));                                                            \
        }																											    \
        catch (CORBA::Exception& ex)                                                                                    \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); \
            TA_THROW(SchedulingManagerException("Converting CORBA::Exception to SCHEDULING_AGENT_FAILURE",              \
                    SchedulingManagerException::SCHEDULING_AGENT_FAILURE));                                             \
        }                                                                                                               \
        catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    \
            TA_THROW(SchedulingManagerException("Converting ObjectResolutionException to SCHEDULING_AGENT_FAILURE",     \
                    SchedulingManagerException::SCHEDULING_AGENT_FAILURE));                                             \
        }                                                                                                               \
        catch (SchedulingManagerException&)                                                                             \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            throw;                                                                                                      \
        }                                                                                                               \
        catch (...)                                                                                                     \
        {                                                                                                               \
            DO_CATCH(needCleanUp);                                                                                      \
            TA_THROW(SchedulingManagerException(SchedulingManagerException::UNKNOWN_EXCEPTION) );                       \
        }
    // END OF CATCH_ALL_EXCEPTION MACRO



    JobManager::JobManager(IJobUpdatable* jobUpdatable /*= NULL*/) 
    :
    m_jobUpdatable(jobUpdatable),
    m_rightsMgr(NULL),
    m_authMgr(NULL),
    m_sessionOperator(NULL),
    m_transActiveSuperProfile(NULL),
    m_sessionId(""),
    m_isServerUp(true),
    m_resourceKey(0),
    m_sessionLocation(0)
    {
	    FUNCTION_ENTRY( "Constructor" );
        using TA_Base_Core::MessageSubscriptionManager;
        using TA_Base_Core::EntityAccessFactory;
        using TA_Base_Core::SchedulingAgentEntityData;
        using TA_Base_Core::IEntityData;

        try
        {
            // create the rights library
            TA_Base_Bus::RightsLibraryFactory rightsFactory;
            m_rightsMgr = rightsFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER);
        }
        catch (TA_Base_Core::RightsException& ex)                                                                            
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", ex.what());                                              
            TA_THROW(SchedulingManagerException("Converting RighstException to RIGHTS_AGENT_FAILURE", 
                    SchedulingManagerException::RIGHTS_AGENT_FAILURE)); 
        }                                                                                                               
        catch (CORBA::Exception& ex)                                                                                     
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
            TA_THROW(SchedulingManagerException("Converting CORBA::Exception to RIGHTS_AGENT_FAILURE", 
                    SchedulingManagerException::RIGHTS_AGENT_FAILURE)); 
        }                                                                                                               
        catch (TA_Base_Core::ObjectResolutionException& ex)                                                                   
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    
            TA_THROW(SchedulingManagerException("Converting ObjectResolutionException to RIGHTS_AGENT_FAILURE", 
                    SchedulingManagerException::RIGHTS_AGENT_FAILURE)); 
        }                                                                                                               
        catch(...)                                                                                                      
        {                                                                                                               
            TA_THROW(SchedulingManagerException(SchedulingManagerException::UNKNOWN_EXCEPTION) );                                                   
        }

        try
        {
            RunParams::getInstance().registerRunParamUser(this, RPARAM_JOBINFO.c_str());
            RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);
            RunParams::getInstance().registerRunParamUser(this, RPARAM_SERVERSTATUS.c_str());
            m_sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
            TA_ASSERT(!m_sessionId.empty(), "SessionId must not be empty");

            // register to all agents
            std::vector<IEntityData*> agent = EntityAccessFactory::getInstance().getEntitiesOfType(
                SchedulingAgentEntityData::SCHEDULING_AGENT_ENTITY_TYPE, false, true, true );

            if (agent.size() == 0)
            {
                TA_THROW(TA_Base_Core::DataException("At least 1 scheduling agent must be configured in the database", TA_Base_Core::DataException::NO_VALUE, "Scheduling Agent Entity"));
            }

			//jinhua++
			// no scheduling agent
            if ( agent.size() < 1 )
            {
                TA_THROW(TA_Base_Core::DataException("No scheduling Agent ", TA_Base_Core::DataException::NO_VALUE, "") );
            }
            // multiple scheduling agents
            else if ( agent.size() > 1 )
            {
                TA_THROW(TA_Base_Core::DataException("Multiple scheduling Agent", TA_Base_Core::DataException::NOT_UNIQUE, "") );
            }

            // Set the first agent in the list as the agent to contact
            m_scheduler.setCorbaName(TA_Base_Core::EntityAccessFactory::getInstance().getCorbaNameOfEntity(agent[0]->getName(),true));
            m_scheduler.setObjectTimeout(CORBA_TIMEOUT);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "First agent to contact is %s", agent[0]->getName().c_str());

            // subscribe to the first agent.
	        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
		        SchedulingCommsJobDeleted,this,agent[0]->getKey(),agent[0]->getSubsystem(),agent[0]->getLocation(),NULL);
	        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
		        SchedulingCommsJobAdded,this,agent[0]->getKey(),agent[0]->getSubsystem(),agent[0]->getLocation(),NULL);
	        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
		        SchedulingCommsJobUpdated,this,agent[0]->getKey(),agent[0]->getSubsystem(),agent[0]->getLocation(),NULL);
	        MessageSubscriptionManager::getInstance().subscribeToCommsMessage(
		        SchedulingCommsJobScheduled,this,agent[0]->getKey(),agent[0]->getSubsystem(),agent[0]->getLocation(),NULL);

			unsigned int i = 0;
            for ( i = 0; i < agent.size(); i++)
            {
                delete agent[i];    // no longer needed
                agent[i] = NULL;
            }

            // get the SchedulingManager entity so that we can get the resource key
            std::string entityName = TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYNAME);
            TA_ASSERT(!entityName.empty(), "Entity name must have been reset to empty during initialisation");
            TA_Base_Core::IEntityData* entity = EntityAccessFactory::getInstance().getEntity( entityName );
            int entityKey = entity->getKey();
            delete entity;  // entity no longer needed
            entity = NULL;

            // get resource key
            TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(entityKey);
            TA_ASSERT(resource != NULL, "Resource associated with the entity is NULL");
            m_resourceKey = resource->getKey();
            delete resource;    // resource no longer needed
            resource = NULL;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Resource key = %d", m_resourceKey);

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "About to fetch jobs from agent");
			SchedulingAgentCorbaDef::JobSequence* jobs ;
			//jinhua++ for corba call failure so We will call two times
			try
			{
			//++jinhua
				CORBA_CALL_RETURN( jobs,
								   m_scheduler,
								   getAllJobs,
								   ( m_sessionId.c_str() ) );
			//jinhua++ for corba call failure so We will call two times
			}
			catch (TA_Base_Core::ObjectResolutionException& ex)                                                                  
			{                                                                                                               
				LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());

				CORBA_CALL_RETURN( jobs,
						           m_scheduler,
								   getAllJobs,
								   ( m_sessionId.c_str() ) );
			}
			//++jinhua

            m_lastUpdateTime = time(NULL);
            if (jobs == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Fetched no jobs from the agent");
                return;
            }
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Fetched %d jobs from the agent", jobs->length());

            {// build the job list
                // protect the job list
                ThreadGuard guard(m_jobLock);
                for (i = 0; i < jobs->length(); i++)
                {
                    Job* job = new Job((*jobs)[i]);
                    m_jobs.insert(JobPtrMap::value_type(std::string(job->getId()), job));
                }
            }

            std::vector<TA_Base_Core::ILocation*> locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations(false);
            for (i = 0; i < locations.size(); i++)
            {
                m_locationMap.insert(LocationMap::value_type(locations[i]->getKey(), locations[i]));
            }

            std::vector<TA_Base_Core::IProfile*> profiles = TA_Base_Core::ProfileAccessFactory::getInstance().getAllNonSystemProfiles(false);
            for (i = 0; i < profiles.size(); i++)
            {
                m_nonSystemProfileMap.insert(ProfileMap::value_type(profiles[i]->getKey(), profiles[i]));
            }

            m_schedulableProfileMap = m_nonSystemProfileMap;
            m_transActiveSuperProfile = TA_Base_Core::ProfileAccessFactory::getInstance().getTransActiveSuperProfile();
            if (m_transActiveSuperProfile != NULL)
            {
                m_schedulableProfileMap.insert(ProfileMap::value_type(m_transActiveSuperProfile->getKey(), m_transActiveSuperProfile));
            }

            m_authMgr = new TA_Base_Bus::AuthenticationLibrary(true);

            // load all profiles associated with the operator
            loadProfileList();
        }
        catch (TA_Base_Core::RightsException& ex)
        {
            cleanUp();
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", ex.what());
            TA_THROW(SchedulingManagerException("Converting RightsException to ACCESS_DENIED", 
                    SchedulingManagerException::ACCESS_DENIED));
        }
        catch (TA_Base_Core::DataException& ex)
        {
            cleanUp();
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            TA_THROW(SchedulingManagerException("Converting DataException to DATA_ERROR", 
                    SchedulingManagerException::DATA_ERROR));
        }
        catch (TA_Base_Core::DatabaseException& ex)
        {
            cleanUp();
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            TA_THROW(SchedulingManagerException("Converting DatabaseException to DATABASE_ERROR", 
                    SchedulingManagerException::DATABASE_ERROR));
        }
        CATCH_ALL_EXCEPTIONS(true);
	    FUNCTION_EXIT;
    }


    JobManager::~JobManager()
    {
	    FUNCTION_ENTRY( "Destructor" );
        cleanUp();
	    FUNCTION_EXIT;
    }


    void JobManager::cleanUp()
    {
	    FUNCTION_ENTRY( "cleanUp" );
        ThreadGuard guard(m_jobLock);
        try
        {
            m_subscriptions.removeAllSubscriptions();

            if (m_rightsMgr != NULL)
            {
                delete m_rightsMgr;
                m_rightsMgr = NULL;
            }

            if (m_authMgr != NULL)
            {
                delete m_authMgr;
                m_authMgr = NULL;
            }

            for (LocationMap::iterator loc = m_locationMap.begin(); loc != m_locationMap.end(); loc++)
            {
                delete loc->second;
                loc->second = NULL;
            }

            for (ProfileMap::iterator prof = m_nonSystemProfileMap.begin(); prof != m_nonSystemProfileMap.end(); prof++)
            {
                delete prof->second;
                prof->second = NULL;
            }

            if (m_sessionOperator != NULL)
            {
                delete m_sessionOperator;
                m_sessionOperator = NULL;
            }

            if (m_transActiveSuperProfile != NULL)
            {
                delete m_transActiveSuperProfile;
                m_transActiveSuperProfile = NULL;
            }

            for (JobPtrMap::iterator job = m_jobs.begin(); job != m_jobs.end(); job++)
            {
                delete job->second;
                job->second = NULL;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught some unknown exception when cleaning up");
        }
	    FUNCTION_EXIT;
    }


    void JobManager::onRunParamChange(const std::string& name, const std::string& value)
    {
	    FUNCTION_ENTRY( "onRunParamChange" );
        if (STRINGS_IDENTICAL == name.compare(RPARAM_SESSIONID))
        {
            m_sessionId = value;

            try
            {
                // load all profiles associated with the operator
                loadProfileList();
            }
            catch (...)
            {
                // Can't get the session profile from the Authentication Agent and can't 
                // connect to the database, so the m_sessionProfiles is going to be empty
            }

            if (m_jobUpdatable != NULL)
            {
                m_jobUpdatable->updateRights();
            }

        }
        else if (STRINGS_IDENTICAL == name.compare(RPARAM_JOBINFO))
        {
            if (m_jobUpdatable != NULL)
            {
                Job job(value);
                m_jobUpdatable->createJob(job);
            }
        }
        else if (STRINGS_IDENTICAL == name.compare(RPARAM_SERVERSTATUS))
        {
            m_isServerUp = (STRINGS_IDENTICAL == value.compare(RVALUE_SERVER_UP));
            if (m_jobUpdatable != NULL)
            {
                m_jobUpdatable->updateRights();
            }
        }

	    FUNCTION_EXIT;
    }


    void JobManager::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
    {
	    FUNCTION_ENTRY( "receiveSpecialisedMessage" );

        // protect the job list
        ThreadGuard guard(m_jobLock);

        m_lastUpdateTime = time(NULL); // record the current time;
        std::string mesgTypeKey = message.messageTypeKey;

		LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD16357]Recieve specialised message %s",mesgTypeKey.c_str());

        if (STRINGS_IDENTICAL == mesgTypeKey.compare(SchedulingCommsJobDeleted.getTypeKey()))
        {
            //const SchedulingAgentCorbaDef::JobID id;
			const char* id;

            // can't extract to JobCorbaDef, ignore
	        if ((message.messageState >>= id ) == 0)
	        {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Received a SchedulingCommsJobDeleted but couldn't extract JobID.  Ignore");
                return;
            }

			std::string jobIDStr(id);

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Processing job deleted message");
            JobPtrMap::iterator existingJob = m_jobs.find(jobIDStr);
            if (existingJob != m_jobs.end())
            {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Found job, deleting it from caches");

                // temporarily assign it to a variable so that we don't lose it
                Job* job = existingJob->second;

                // remove it from our job list but not actually deleting the job
                m_jobs.erase(existingJob);

                // remove it from our profile cache if it's there
                existingJob = m_profileJobs.find(jobIDStr);
                bool isAssociatedWithSessionProfile = false;
                if (existingJob != m_profileJobs.end())
                {
                    m_profileJobs.erase(existingJob);
                    isAssociatedWithSessionProfile = true;
                }

                // leave it up to the consumer to delete it.  We're not going to delete it here
                if (m_jobUpdatable != NULL)
                {
                    m_jobUpdatable->jobDeleted(*job, isAssociatedWithSessionProfile);
                }
            }
            return;
        }
        else if (STRINGS_IDENTICAL == mesgTypeKey.compare(SchedulingCommsJobScheduled.getTypeKey()))
        {
            SchedulingAgentCorbaDef::JobTimeCorbaDef* data = NULL;

            // can't extract to JobCorbaDef, ignore
	        if ((message.messageState >>= data ) == 0)
	        {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Received a SchedulingCommsJobScheduled but couldn't extract JobTimeCorbaDef.  Ignore");
                return;
            }

            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Processing job scheduled message");

			std::string jobIDStr = data->jobIdentifer.in();

            JobPtrMap::iterator existingJob;
			existingJob = m_jobs.find(jobIDStr);;
            if (existingJob == m_jobs.end())
            {
                // it hasn't been added, so we can now add it
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Got an update but the job hasn't been added to our list.  So adding job %s", jobIDStr.c_str());
                Job* job = new Job(*data);
                m_jobs.insert(JobPtrMap::value_type(jobIDStr, job));

                // no need to update the GUI yet since we've only got the job start time but not the rest of the job details
                return;
            }
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating job %s", jobIDStr.c_str());
            existingJob->second->setStartDate(data->runTime);
            // don't have to update the m_profileJobs here as JobTimeCorbaDef does not update the profile/location

            if (m_jobUpdatable != NULL)
            {
                m_jobUpdatable->jobUpdated(*(existingJob->second), operatorHasJobProfile(*(existingJob->second)));
            }
            return;
        }
        else if ((STRINGS_IDENTICAL == mesgTypeKey.compare(SchedulingCommsJobAdded.getTypeKey())) ||
            (STRINGS_IDENTICAL == mesgTypeKey.compare(SchedulingCommsJobUpdated.getTypeKey())))
        {
            SchedulingAgentCorbaDef::JobCorbaDef* data = NULL;

            // can't extract to JobCorbaDef, ignore
	        if ((message.messageState >>= data ) == 0)
	        {
                LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                    "Received a SchedulingCommsJobAdded but couldn't extract JobCorbaDef.  Ignore");
                return;
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Processing job added message");

            // check to see if it's already in the job list.  It is likely SchedulingCommsJobScheduled
            // will be sent before this message, so check to see if the job has already been added

			std::string jobIDStr = data->identifier.in();
            JobPtrMap::iterator existingJob = m_jobs.find(jobIDStr);
            if (existingJob == m_jobs.end())
            {
                // it hasn't been added, so we can now add it
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Adding job %s", jobIDStr.c_str());
                Job* job = new Job(*data);
                m_jobs.insert(JobPtrMap::value_type(jobIDStr, job));
                bool isAssociatedWithSessionProfile = operatorHasJobProfile(*job);
                if (isAssociatedWithSessionProfile) 
                {
                    // if the session now has the same profile as the job, then we can update the jobs associated with this session's profiles
                    // m_profileJobs[id] = job;
					m_profileJobs[jobIDStr] = job;
                }
                else
                {
                    // otherwise, the job no longer has the same profile as the session, remove it from the list if it's there
					// m_profileJobs.erase(id);
                    m_profileJobs.erase(jobIDStr);
                }

                // update the GUI display
                if (m_jobUpdatable != NULL)
                {
                    m_jobUpdatable->jobUpdated(*job, isAssociatedWithSessionProfile);
                }
                return;
            }

            // otherwise the job already exists
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Updating an added job %s", jobIDStr.c_str());
            // update m_jobs
            existingJob->second->update(*data);

            // update m_profileJobs 
            bool isAssociatedWithSessionProfile = operatorHasJobProfile(*(existingJob->second));
            if (isAssociatedWithSessionProfile)
            {
                // if the session now has the same profile as the job, then we can update the jobs associated with this session's profiles
                // m_profileJobs[id] = existingJob->second;
				m_profileJobs[jobIDStr] = existingJob->second;
            }
            else
            {
                // otherwise, the job no longer has the same profile as the session, remove it from the list if it's there
                // m_profileJobs.erase(id);
				m_profileJobs.erase(jobIDStr);
            }

            // update the GUI display
            if (m_jobUpdatable != NULL)
            {
                m_jobUpdatable->jobUpdated(*(existingJob->second), isAssociatedWithSessionProfile);
            }
        }
        FUNCTION_EXIT;
    }


    void JobManager::deleteJob(const std::string& strJobId)
    {
	    FUNCTION_ENTRY( "deleteJob" );

        // protect the job list
        ThreadGuard guard(m_jobLock);

        try
        {
			JobPtrMap::iterator it = m_jobs.find(strJobId); 
            TA_ASSERT( it != m_jobs.end(), "Trying to delete a job that is not in our job list");

            if (!operatorHasJobProfile(*(it->second)))
            {
                TA_THROW(SchedulingManagerException(SchedulingManagerException::SESSION_PROFILES_NOT_MATCH_JOB_PROFILE));
            }
			CORBA_CALL( m_scheduler, removeJob, ( strJobId.c_str(), m_sessionId.c_str() ) );
        }
        CATCH_ALL_EXCEPTIONS(false);

	    FUNCTION_EXIT;
    }


    void JobManager::saveJob(const Job& job)
    {
	    FUNCTION_ENTRY( "saveJob" );

        // protect the job list
        ThreadGuard guard(m_jobLock);

        // throw an exception if the parameters are not specified
        if (job.getParameters().size() == 0)
        {
            TA_THROW(SchedulingManagerException(SchedulingManagerException::NO_JOB_PARAMETER));
        }

        try
        {
			if (job.isNew())
			{
                CORBA_CALL( m_scheduler,
                            scheduleJob,
                            ( job.toCorbaObject(), m_sessionId.c_str() ) );
			}
			else
			{
                CORBA_CALL( m_scheduler,
                            updateJob,
                            ( job.toCorbaObject(), m_sessionId.c_str(), false, true ) );
			}
        }
        CATCH_ALL_EXCEPTIONS(false);

	    FUNCTION_EXIT;
    }


    void JobManager::saveNewJob(Job& job)
    {
	    FUNCTION_ENTRY( "saveNewJob" );

        // protect the job list
        ThreadGuard guard(m_jobLock);

        // throw an exception if the parameters are not specified
        if (job.getParameters().size() == 0)
        {
            TA_THROW(SchedulingManagerException(SchedulingManagerException::NO_JOB_PARAMETER));
        }
        
		JobPtrMap::iterator it = m_jobs.find(job.getId());

        if (( it != m_jobs.end()) && ( job == *(it->second) ))
        {
            // cannot add a job that is identical to the existing one
            TA_THROW(SchedulingManagerException(SchedulingManagerException::JOB_EXIST));
        }

        try
        {
			// otherwise, the job might have been deleted or the job is different to the existing one.
			char* jobId = NULL;
			CORBA_CALL_RETURN( jobId, m_scheduler, scheduleJob, ( job.toCorbaObject(), m_sessionId.c_str() ) );
            job.setId( jobId );
            CORBA::string_free( jobId );
        }
        CATCH_ALL_EXCEPTIONS(false);

        FUNCTION_EXIT;
    }

	//TD16352++
	void JobManager::updateEditLock(const std::string& jobId, const bool bLock)
	{
		FUNCTION_ENTRY( "updateEditLock" );

		// protect the job list
        ThreadGuard guard(m_jobLock);

		JobPtrMap::iterator it = m_jobs.find(jobId);

		if(it == m_jobs.end())
		{
			//job is not in map so safe to edit
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "[TD16352]Job id %s submitted for edit lock not found", jobId.c_str());
			return;
		}

		try
		{
			CORBA_CALL( m_scheduler, updateEditLock, ( jobId.c_str(), m_sessionId.c_str(), bLock ) );
		}
		CATCH_ALL_EXCEPTIONS(false);
		
		FUNCTION_EXIT;
	}

	bool JobManager::isJobLockedForEdit(const std::string& jobId)
	{
		FUNCTION_ENTRY( "isJobLockedForEdit" );

		// protect the job list
        ThreadGuard guard(m_jobLock);

		JobPtrMap::iterator it = m_jobs.find(jobId);

		if(it == m_jobs.end())
		{
			//job is not in map so safe to edit
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[TD16352]Job id %s is not edit locked since it is not in cache", jobId.c_str());
			return false;
		}

        bool isJobLocked = false;
		try
		{
			CORBA_CALL_RETURN( isJobLocked, m_scheduler, isJobLockedForEdit, ( jobId.c_str(), m_sessionId.c_str() ) );

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "[TD16352]Job id %s is edit %s", jobId.c_str(), isJobLocked ? "locked" : "unlocked" );
		}
        CATCH_ALL_EXCEPTIONS(false);

        FUNCTION_EXIT;
        return isJobLocked;
	}

	//++TD16352

    void JobManager::enableJob(const std::string& strJobId, const bool enable)
    {
	    FUNCTION_ENTRY( "enableJob" );

        // protect the job list
        ThreadGuard guard(m_jobLock);

		JobPtrMap::iterator it = m_jobs.find(strJobId);
        TA_ASSERT( it != m_jobs.end(), "Trying to enable/disable a job that is not in our job list");

        try
        {
            if (!operatorHasJobProfile(*(it->second)))
            {
                TA_THROW(SchedulingManagerException(SchedulingManagerException::SESSION_PROFILES_NOT_MATCH_JOB_PROFILE));
            }

            bool isInhibited = it->second->isInhibited();

            // should only enable a job if it is inhibited and its date is some time in the future
			if (enable && isInhibited && (it->second->getStartDate() != NULL))
			{
				it->second->setInhibited(!enable);
                CORBA_CALL( m_scheduler, updateJob, ( it->second->toCorbaObject(), m_sessionId.c_str(), true, true ) );
			}
			// disable a job if it is not inhibited
			else if (!enable && !isInhibited)
			{
				it->second->setInhibited(!enable);
                CORBA_CALL( m_scheduler, updateJob, ( it->second->toCorbaObject(), m_sessionId.c_str(), true, false ) );
			}
        }
        CATCH_ALL_EXCEPTIONS(false);

        FUNCTION_EXIT;
    }


    JobPtrMap JobManager::getAllJobs() const
    {
        // protect the job list
        ThreadGuard guard(m_jobLock);
        return m_jobs;
    }


    JobPtrMap JobManager::getAllJobsByProfile() const
    {
        // protect the job list
        ThreadGuard guard(m_jobLock);
        return m_profileJobs;
    }


    unsigned long JobManager::getTotalJobCount() const
    {
        // protect the job list
        ThreadGuard guard(m_jobLock);
        return m_jobs.size();
    }

    
    unsigned long JobManager::getTotalJobCountByProfile() const
    {
        // protect the job list
        ThreadGuard guard(m_jobLock);
        return m_profileJobs.size();
    }


    std::vector<std::string> JobManager::getJobTypes()
    {
	    FUNCTION_ENTRY( "getJobTypes" );
        std::vector<std::string> jobTypes;

        try
        {
			SchedulingAgentCorbaDef::JobTypeSeq* types;
			CORBA_CALL_RETURN( types,
			                   m_scheduler,
			                   getSupportedJobTypes,
			                   ( m_sessionId.c_str() ) );
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "The agent recognised the following job types:");
            for (unsigned int i = 0; i < types->length(); i++)
            {
                std::string str((*types)[i]);
                jobTypes.push_back(str);
                LOGMORE(SourceInfo, str.c_str());
            }
            TA_ASSERT(jobTypes.size() > 0, "Can't get the job types from the agent.  Must have at least 1 job type");
        }
        CATCH_ALL_EXCEPTIONS(false);

	    FUNCTION_EXIT;
        return jobTypes;
    }


    bool JobManager::isActionOnJobPermitted(SchedulingManager::EAction action, JobPtrVector* jobs)
    {
	    FUNCTION_ENTRY( "isActionOnJobPermitted" );

        // always allow operator to Print, Filter and view a jobs 
        if ((action == Print) || (action == Filter) || ((action == View) && (jobs != NULL) && (jobs->size() == 1)))
        {
            return true;
        }

        // if no jobs is selected and the action requires at least 1 jobs to be selected, just disallow action
        if (((jobs == NULL) || jobs->empty()) && (action == Delete || action == Enable || action == Disable))
        {
            return false;
        }

        // if it is an action that could only be performed on one and only one item, diallow action if no item or more than one items are selected
        if (((jobs == NULL) || (jobs->size() != 1)) && (action == View || action == Edit))
        {
            return false;
        }

        //
        // The following actions are access controlled, we need to see if we have valid session profiles first,
        // if we do, then we see if we have the rights to perform the action.
        //
        if (!m_isServerUp)
        {
            // if the server is down, then disable all access control related actions
            return false;
        }

        if (m_sessionProfiles.empty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "No session profile, so returning false");
            return false;
        }

        if (m_rightsMgr == NULL)
        {
            // the rest of the actions require access rights, so if we can't get the rights manager, we'll just assume no rights
            return false;
        }

        bool allowed = false;
        std::string reason;
        TA_Base_Bus::EDecisionModule decision;
        try
        {
            unsigned long actualAction;
            // convert our internal action enumerated type to the one recognised by the rights library
            switch (action)
            {
            case Add:
                actualAction = TA_Base_Bus::aca_SCHEDULE_ADD;
                break;
            case Delete:
                actualAction = TA_Base_Bus::aca_SCHEDULE_DELETE;
                break;
            case Enable:
            case Disable:
                actualAction = TA_Base_Bus::aca_SCHEDULE_ENABLEDISABLE;
                break;
            case Edit:
                actualAction = TA_Base_Bus::aca_SCHEDULE_MODIFY;
                break;
            default:
                TA_ASSERT(false, "Shouldn't get up to here if it doens't require rights to be checked by rights lib");
                break;
            }

			//TD17144++
            //allowed = m_rightsMgr->isActionPermittedOnResource(m_sessionId.c_str(), m_resourceKey, actualAction, reason, decision);
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_resourceKey);	
			allowed = m_rightsMgr->isActionPermittedOnResourceAndLocSub(m_sessionId.c_str(), m_resourceKey, 
				entityData->getLocation(), entityData->getSubsystem(), actualAction, reason, decision);
			delete entityData;
			//++TD17144

        }
        catch (TA_Base_Core::RightsException& ex)
        { 
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", ex.what());
        }
        catch(CORBA::Exception& ex)                                                                                     
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
        }                                                                                                               
        catch(TA_Base_Core::ObjectResolutionException& ex)                                                                   
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    
        }                                                                                                               
        catch (...)
        { 
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown error", "Some unknown exception has occurred so assume no rights");
        }

        // Add operation is not dependent on whether a jobs is selected, so just return the result straightaway
        if (action == Add)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "%s to Add", allowed?"Allowed":"Not allowed");
            return allowed;
        }

        if (!allowed)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Not allowed. Reason: %s", reason.c_str());
            return false;
        }

        // put a try catch here as the jobs might have been deleted by the view by the time it gets here to prevent
        // the app from crashing due to access violation.
        try
        {
            if (action == Edit)
            {
                if ((jobs != NULL) && (jobs->size() == 1) && ((*jobs)[0] != NULL))
                {
                    // only allow action is the operator has profile associated with the jobs.
                    return operatorHasJobProfile(*((*jobs)[0]));
                }
                else
                {
                    return false;
                }
            }
            else if (action == Enable)
            {
                COleDateTime currentTime = COleDateTime::GetCurrentTime();

                // check to see if at least one jobs that is selected is disabled and has a schedule start date/time in future
                for (unsigned int i = 0; i < jobs->size(); i++)
                {
                    if (((*jobs)[i] != NULL) && 
                        (*jobs)[i]->isInhibited() &&
                        operatorHasJobProfile(*(*jobs)[i]))
                    {
                        COleDateTime* startDate = (*jobs)[i]->getStartDate();
                        if ((startDate != NULL) && (*startDate > currentTime))
                        {
                            return true;
                        }
                    }
                }
                return false;
            }
            else if (action == Disable)
            {
                // check to see if at least one jobs that is selected is enabled
                for (unsigned int i = 0; i < jobs->size(); i++)
                {
                    if (((*jobs)[i] != NULL) &&
                        !(*jobs)[i]->isInhibited()&&
                        operatorHasJobProfile(*(*jobs)[i]))
                    {
                        return true;
                    }
                }
                return false;
            }
            else if (action == Delete)
            {
                // check to see if at least one jobs that is selected is deletable by the session profiles
                for (unsigned int i = 0; i < jobs->size(); i++)
                {
                    if (((*jobs)[i] != NULL) &&
                        operatorHasJobProfile(*(*jobs)[i]))
                    {
                        return true;
                    }
                }
                return false;
            }
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Exception", "Error occurred while using jobs object");
        }
        return false; // keep the compiler happy
	    FUNCTION_EXIT;
    }


    void JobManager::loadProfileList()
    {
	    FUNCTION_ENTRY( "loadProfileList" );

        m_sessionProfiles.clear();
        m_sessionLocation = 0;

        try
        {
            // get the new list of profiles associated with the session
            m_sessionProfiles = m_authMgr->getProfiles(m_sessionId);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Successfully obtained session profiles from the Authentication Agent");
        }
        catch(CORBA::Exception& ex)                                                                                     
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
        }                                                                                                               
        catch(TA_Base_Core::ObjectResolutionException& ex)                                                                   
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    
        }                                                                                                               
        catch (...) 
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Corba Exception", "Fail to get session profiles from the Authentication Agent");
        }

        try
        {
            // if the Authentication Library can't contact the Authentication Agent or if it cannot get 
            // the profile list then just use the profile of the current session Id
            TA_Base_Core::Session session(m_sessionId);
            if (m_sessionProfiles.empty())
            {
                m_sessionProfiles.push_back(session.getProfileKey());
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Using session profile from database as it couldn't be fetched from the Authentication Agent");
            }
            m_sessionLocation = session.getLocationKey();
            unsigned long operatorKey = session.getOperatorKey();
            if ((m_sessionOperator == NULL) || (operatorKey != m_sessionOperator->getKey()))
            {
                delete m_sessionOperator;
                m_sessionOperator = NULL;
                m_sessionOperator = TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(operatorKey, false);
            }
        }
        catch (TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
            TA_THROW(SchedulingManagerException("Converting DataException to DATA_ERROR", 
                    SchedulingManagerException::DATA_ERROR));
        }
        catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
            TA_THROW(SchedulingManagerException("Converting DatabaseException to DATABASE_ERROR", 
                    SchedulingManagerException::DATABASE_ERROR));
        }
        catch (...)
        {
            // if we have problem getting the profiles from current session and can't get the
            // profile of this session from the database, then there's no point continuing since
            // we can't determine the operator's rights on the jobs.
            // Just throw an unknown exception since this scenario should never happen.  This
            // method is only called when there is a session change 
            TA_THROW(SchedulingManagerException(SchedulingManagerException::UNKNOWN_EXCEPTION));
        }

        if ((m_transActiveSuperProfile != NULL) && canUseSuperProfile())
        {
            unsigned long superProfileKey = m_transActiveSuperProfile->getKey();
            if (std::find(m_sessionProfiles.begin(), m_sessionProfiles.end(), superProfileKey) == m_sessionProfiles.end())
            {
                m_sessionProfiles.push_back(m_transActiveSuperProfile->getKey());
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Added the Transactive Super Profile into the list of session profile");
            }
        }

        // protect the job list
        ThreadGuard guard(m_jobLock);

        // at the same time update the list of jobs that are associated with the session profiles as well
        m_profileJobs.clear();

        // update the list of jobs that are modified
        for (JobPtrMap::iterator job = m_jobs.begin(); job != m_jobs.end(); job++)
        {
            if ( NULL == job->second )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Pointing to a NULL job" );
                continue;
            }

            if (operatorHasJobProfile(*(job->second)))
            {
                m_profileJobs.insert(JobPtrMap::value_type(job->first, job->second));
            }
        }

        FUNCTION_EXIT;
    }


    bool JobManager::canUseSuperProfile()
    {
	    FUNCTION_ENTRY( "canUseSuperProfile" );
        if (m_rightsMgr == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Can't determine if the Transactive Super Profile should be included in the session profile as rights library is NULL");
            FUNCTION_EXIT;
            return false;
        }
        try
        {
            std::string reason;
            TA_Base_Bus::EDecisionModule decision;
			//TD17144++
            //return m_rightsMgr->isActionPermittedOnResource(m_sessionId.c_str(), m_resourceKey, TA_Base_Bus::aca_USE_TRANSACTIVE_SUPER_PROFILE, reason, decision);
			TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(m_resourceKey);	
			bool allowed = m_rightsMgr->isActionPermittedOnResourceAndLocSub(m_sessionId.c_str(), m_resourceKey, 
				entityData->getLocation(), entityData->getSubsystem(), TA_Base_Bus::aca_USE_TRANSACTIVE_SUPER_PROFILE, reason, decision);
			delete entityData;
			return allowed;
			//++TD17144
		
        }
        catch (TA_Base_Core::RightsException& ex)
        { 
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", ex.what());
        }
        catch(CORBA::Exception& ex)                                                                                     
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::Exception", TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str()); 
        }                                                                                                               
        catch(TA_Base_Core::ObjectResolutionException& ex)                                                                   
        {                                                                                                               
            LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());                                    
        }                                                                                                               
        catch (...)
        { 
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown error", "Some unknown exception has occurred so assume no rights");
        }
        FUNCTION_EXIT;
        return false;
    }


    bool JobManager::operatorHasJobProfile(const Job& job)
    {
	    FUNCTION_ENTRY( "operatorHasJobProfile" );
        unsigned long profileId = job.getProfileId();
        for (unsigned int i = 0; i < m_sessionProfiles.size(); i++)
        {
            if (profileId == m_sessionProfiles[i])
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Operator has the specified profile");
                return true;
            }
        }
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Operator does not have the specified profile");
	    FUNCTION_EXIT;
        return false;
    }


    ProfileVector JobManager::getAllOperatorProfiles()
    {
	    FUNCTION_ENTRY( "getAllOperatorProfiles" );
        ProfileVector operatorProfiles;
        try
        {
            std::vector<unsigned long> profileKeys = m_sessionOperator->getAssociatedProfileKeys();
            for (unsigned int i = 0; i < profileKeys.size(); i++)
            {
                if (m_nonSystemProfileMap.find(profileKeys[i]) != m_nonSystemProfileMap.end())
                {
                    operatorProfiles.push_back(m_nonSystemProfileMap[profileKeys[i]]);
                }
            }

            if ((m_transActiveSuperProfile != NULL) && canUseSuperProfile())
            {
                unsigned long superProfileKey = m_transActiveSuperProfile->getKey();
                // just run a check to make sure the TransActiveSuperProfile isn't added into the
                // list already, just in case it has been configured as a non-system profile
                for (unsigned int i = 0; i < operatorProfiles.size(); i++)
                {
                    if (operatorProfiles[i]->getKey() == superProfileKey)
                    {
                        return operatorProfiles;
                    }
                }
                operatorProfiles.push_back(m_transActiveSuperProfile);
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Added the Transactive Super Profile into the list of session profile");
            }

        }
        catch (TA_Base_Core::DataException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
        }
        catch (TA_Base_Core::DatabaseException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", ex.what());
        }
	    FUNCTION_EXIT;
        return operatorProfiles;
    }

};
