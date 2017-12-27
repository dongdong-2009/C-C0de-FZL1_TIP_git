/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:    $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/AbstractJobInstance.cpp $
  * @author:   Dirk McCormick
  * @version:  $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/05/30 15:41:36 $
  * Last modified by:  $Author: qi.huang $
  *
  * This abstract class represents an instance of an executable job.
  * It implements the getJob and getMinuteStarted methods of the
  * IJobInstance interface.
  *
  */

#include "app/scheduling/scheduling_agent/src/IJob.h"
#include "app/scheduling/scheduling_agent/src/SchedulingSessionManager.h"
#include "app/scheduling/scheduling_agent/src/SchedulingMessageSender.h"
#include "app/scheduling/scheduling_agent/src/SchedulingAlarmSender.h"
#include "app/scheduling/scheduling_agent/src/JobInstanceExecutionException.h"
#include "app/scheduling/scheduling_agent/src/AbstractJobInstance.h"


namespace TA_Base_App
{

AbstractJobInstance::AbstractJobInstance( const IJob& refJob, const unsigned long minuteStarted) :
m_refJob(refJob),
m_minuteStarted(minuteStarted)
{
}

AbstractJobInstance::~AbstractJobInstance()
{
}

std::string AbstractJobInstance::getJobId() const
{
    return m_refJob.getID();
}

std::string AbstractJobInstance::getDescription() const
{
    return m_refJob.getDescription();
}

unsigned long AbstractJobInstance::getLocationId() const
{
    return m_refJob.getLocationID();
}

unsigned long AbstractJobInstance::getProfileId() const
{
    return m_refJob.getProfileID();
}

int AbstractJobInstance::getJobInstanceState(unsigned long minuteStarted, std::string& errorMessage) const
{
    return m_refJob.getJobInstanceState( m_minuteStarted, errorMessage );
}

unsigned long AbstractJobInstance::getMinuteStarted() const
{
    return m_minuteStarted;
}

void AbstractJobInstance::execute()
{
    // Get the session ID
    std::string sessionID = "";
    try
    {
        sessionID = SchedulingSessionManager::getInstance().getSessionKey( m_refJob.getProfileID(), m_refJob.getLocationID() );
    }
    catch (const std::exception& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "UnknownException", "Unknown exception when get session key");
    }

    std::string strJobId = m_refJob.getID().c_str();
    std::string strDescription = m_refJob.getDescription().c_str();

    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Job starting with ID %s at %d minutes after the epoch.", strJobId.c_str(), m_minuteStarted);

    // Note that this object listens for the job starting and job completed
    // events instead of the SchedulingMessageSender as only the controlling
    // agent should send the corresponding messages (not the monitoring agent).
    SchedulingMessageSender::getInstance().jobStarting( strJobId, m_minuteStarted, sessionID );

    std::string strErrorMessage = "";
    try
    {
        executeJob();
    }
    catch(const JobInstanceExecutionException& jiee)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "JobInstanceExecutionException", jiee.what());

        strErrorMessage = jiee.what();
    }
    catch(const std::exception& ex)
    {
        strErrorMessage = "JobThread::run exception : "; 
        strErrorMessage += ex.what();
        LOG_EXCEPTION_CATCH(SourceInfo, typeid(ex).name(), ex.what());
    }
    catch(const CORBA::Exception& ex)
    {
        strErrorMessage = "JobThread::run exception : "; 
        strErrorMessage += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex);
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", strErrorMessage.c_str());
    }
    catch(...)
    {
        strErrorMessage = "Unknown exception caught by JobThread";
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", strErrorMessage.c_str());
    }

    LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
        "Job with id %s that was started at minute %d has completed", strJobId.c_str(), m_minuteStarted );

    try
    {
        JobRunner::getInstance().jobCompleted( m_minuteStarted, strJobId );
    }
    catch(const TA_Base_Core::TransactiveException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH( SourceInfo, "UnknownException", "Exception caught in JobThread run method.");
    }

    // Note that this object listens for the job starting and job completed
    // events instead of the SchedulingMessageSender as only the controlling
    // agent should send the corresponding messages (not the monitoring agent).
    SchedulingMessageSender::getInstance().jobCompleted( strJobId, m_minuteStarted, sessionID, strErrorMessage );

    // If the error message is not blank, raise an alarm
    if( ! strErrorMessage.empty() )
    {
        SchedulingAlarmSender::getInstance().jobExecutionFailed(strJobId, m_minuteStarted, strDescription, strErrorMessage);
    }
}

}; // TA_Base_App
