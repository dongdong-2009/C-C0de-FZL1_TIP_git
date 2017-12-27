/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/ProcessInfo.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class contains information about a single (managed or
  * unmanaged) process that is local to this System Controller. 
  * It allows processes to be started and stopped, and contains
  * information on the time it was started, number of restarts,
  * etc.
  *
  */

#include "ProcessInfo.h"
#include "ProcessUtilities.h"
#include "ProcessManager.h"

#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IProcess.h"

#include "core/alarm/src/NonUniqueAlarmHelper.h"
#include "core/alarm/src/AlarmHelperManager.h"
#include "core/message/types/SystemAlarms_MessageTypes.h"

#include <string>
#include <vector>
#include <memory>
#include <ctime>
#ifdef WIN32
	#define DIRECTORY_SEPARATOR		"\\"
#else
	#define DIRECTORY_SEPARATOR		"/"
#endif

using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{
    //
    // Constructor
    //
    ProcessInfo::ProcessInfo(const std::string& entityName, 
		TA_Base_Core::IProcess* processData, int pollPeriod, std::string binDir)
        : m_isRunning(false), m_timeStarted(0),
        m_terminateStatus(TA_Base_Core::NotKnown), m_failureAlarmId(""), 
        m_wasSignalled(false), m_entityName(entityName), 
        m_numRestarts(0), m_processStatus(TA_Base_Core::Unstarted)
    {
        m_pollPeriod = pollPeriod * 1000;

        m_entityDescription = processData->getDescription();
		std::stringstream addressStream;
		addressStream << binDir << DIRECTORY_SEPARATOR << processData->getAddress();

        m_address = addressStream.str();
        m_userName = processData->getUserName();
		m_passWord = processData->getPassWord();

        m_startLag = processData->getStartLag() * 1000;
        m_killTimeout = processData->getKillTimeout() * 1000;

        if (m_startLag == 0)
        {
            // Use a default start lag of 5 seconds.
            m_startLag = 5000;
        }
        if (m_killTimeout == 0)
        {
            // Use a default kill timeout of 5 seconds.
            m_killTimeout = 5000;
        }
    }

    //
    // Destructor
    //
    ProcessInfo::~ProcessInfo()
    {
		TA_Base_Core::AlarmHelperManager::getInstance().cleanUp();
    }

    //
    // setProcessStatus
    //
    void ProcessInfo::setProcessStatus(TA_Base_Core::EProcessStatus processStatus)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Process %s state changing from %d to %d",
            m_entityName.c_str(), m_processStatus, processStatus);
        m_processStatus = processStatus;
    }


    //
    // startProcess
    //
    void ProcessInfo::startProcess()
    {
        FUNCTION_ENTRY("startProcess");

        setProcessStatus(TA_Base_Core::NotRunning);

        FUNCTION_EXIT;
    }
    
    //
    // stopProcess
    //
    void ProcessInfo::stopProcess(TA_Base_Core::EManagedProcessTerminateCode reason)
    {
        FUNCTION_ENTRY("stopProcess");

        m_terminateStatus = reason;

        setProcessStatus(TA_Base_Core::Terminating);

        FUNCTION_EXIT;
    }

    //
    // handleProcessFailure
    //
    void ProcessInfo::handleProcessFailure(TA_Base_Core::EManagedProcessTerminateCode reason)
    {
        FUNCTION_ENTRY("handleProcessFailure");

        TA_Base_Core::EProcessStatus status = getProcessStatus();

        if ((status == TA_Base_Core::Terminating) && (reason == TA_Base_Core::RequestedTerminate))
        {
            // Signal received due to successful termination.
            setProcessStatus( TA_Base_Core::Stopped );
        }
        else if (status != TA_Base_Core::NotRunning)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                "Process %s has failed", getEntityName().c_str());

            // Update the process's status. This will result in the process being
            // restarted in the run() method.
            setProcessStatus(TA_Base_Core::NotRunning);

            // The process has died so notify all peers
            ProcessManager::getInstance().notifyPeers(getEntityName());

            // Update the terminate status.
            m_terminateStatus = reason;
        }

        raiseFailureAlarm(reason);
        
        FUNCTION_EXIT;
    }

    //
    // getProcessData
    //
    TA_Base_Core::ProcessData* ProcessInfo::getProcessData()
    {
        FUNCTION_ENTRY("getProcessData");

        TA_Base_Core::ProcessData* data = new TA_Base_Core::ProcessData;

        data->ProcessId = m_entityName.c_str();
        data->started = getTimeStarted();
        std::string hostname = ProcessManager::getInstance().getHostName();
        data->host = hostname.c_str();
        data->state = getProcessStatus();
        data->requestedState = getProcessStatus(); 
        data->operationMode = TA_Base_Core::NotApplicable;
        data->restarts = getNumRestarts();
        data->debugLevel = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL).c_str();

        FUNCTION_EXIT;
        return data;
    }

    //
    // run
    //
    void ProcessInfo::run()
    {
        m_isRunning = true;
        while (m_isRunning)
        {
            switch(m_processStatus)
            {
                case(TA_Base_Core::Unstarted):
                case(TA_Base_Core::NotRunning):
                    performStart();
                    break;

                case(TA_Base_Core::RunningControl): // Non-managed processes only use 
											   // RunningControl when they are running
                    performPoll();
                    break;

                case(TA_Base_Core::Terminating):
                    performStop();
                    break;

                default: // Status is stopped
                    sleep(3000);
            }
        }
    }

    //
    // terminate
    //
    void ProcessInfo::terminate()
    {
        m_isRunning = false;
    }

    //
    // raiseAlarm
    //
	std::string ProcessInfo::raiseAlarm(const TA_Base_Core::MessageType& messageType,
										const TA_Base_Core::DescriptionParameters& descParam)
    {
        FUNCTION_ENTRY("raiseAlarm");
		
		std::string alarmId = "";

		try
		{
			// Need data about the Server entity to raise an alarm.
			TA_Base_Core::ServerEntityData* serverData = 
				ProcessManager::getInstance().getServerEntityData();
			
			alarmId = TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().submitAlarmWithAsset(messageType, 
				serverData->getKey(), serverData->getTypeKey(), descParam, 
				serverData->getName(), serverData->getLocation(), serverData->getSubsystem(),
				serverData->getAssetName(), TA_Base_Core::AlarmConstants::defaultTime);			
		}
		catch(TA_Base_Core::TransactiveException& ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
		}

		FUNCTION_EXIT;
		return alarmId;
    }

	//
	// raiseFailureAlarm
	//
    void ProcessInfo::raiseFailureAlarm(TA_Base_Core::EManagedProcessTerminateCode reason)
    {
        // Raise an alarm if one hasn't been raised already.
        if (m_failureAlarmId == "")
        {    
            TA_Base_Core::DescriptionParameters dp;
            TA_Base_Core::NameValuePair name("ProcessId", m_entityName);
            TA_Base_Core::NameValuePair host("HostId",
                ProcessManager::getInstance().getHostName());
            TA_Base_Core::NameValuePair terminateStatus("TerminateStatus", 
                ProcessManager::getInstance().terminateCodeToString(reason));

	        dp.push_back(&name);
            dp.push_back(&host);
	        dp.push_back(&terminateStatus);

            m_failureAlarmId = raiseAlarm(TA_Base_Core::SystemAlarms::ProcessFailure, dp);
        }
    }

    //
    // closeFailureAlarm
    //
    void ProcessInfo::closeFailureAlarm()
    {
        FUNCTION_ENTRY("closeFailureAlarm");

        if (m_failureAlarmId != "")
        {
            try
            {
                // Need the Server entity's location to close the alarm.
                TA_Base_Core::ServerEntityData* serverData = 
                    ProcessManager::getInstance().getServerEntityData();

				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(m_failureAlarmId, serverData->getLocation());
                m_failureAlarmId = "";
            }
            catch(TA_Base_Core::TransactiveException& ex)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
            }
        }

        FUNCTION_EXIT;
    }
    
    //
    // killProcess
    //
    void ProcessInfo::killProcess()
    {
        FUNCTION_ENTRY("killProcess");

        if (m_processId != 0)
        {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "ProcessInfo::killProcess() %lu", m_processId);
            ProcessUtilities::killProcess(m_processId);
        }

        FUNCTION_EXIT;
    }

    //
    // waitForStartLag
    //
    bool ProcessInfo::waitForStartLag()
    {
        FUNCTION_ENTRY("waitForStartLag");

        const int SLEEP_TIME = 1000; // Sleep for 1 second at a time.
        int nLoops = getStartLag() / SLEEP_TIME;

        for (int i = 0; i < nLoops; i++)
        {
            if (getProcessStatus() == TA_Base_Core::Terminating)
            {
                // We've been told to terminate. Stop sleeping.
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Start lag interrupted");
                return false;
            }

            sleep(SLEEP_TIME);
        }

        FUNCTION_EXIT;
        return true;
    }

	//
	// resetProcess
	//
	void ProcessInfo::resetProcess()
	{
		// Increment the number of restarts.
		incNumRestarts();

        // Indicate that a signal has not been received and that 
        // the process shouldn't failback.
        m_wasSignalled = false;

        // Reset the terminate status (so if handleProcessDeath is called,
        // it doesn't assume the status from the previous death).
		setTerminateStatus(TA_Base_Core::NotKnown);
	}
	
	//
	// buildCommandLine
	//
	std::vector<std::string> ProcessInfo::buildCommandLine(std::string& startPath)
	{
        // Parse the entity address field.
        std::vector<std::string> args;
        ProcessUtilities::parseProcessAddress(startPath, args, getAddress());
		return args;
	}

    //
    // performStart
    //
    void ProcessInfo::performStart()
    {
        if (m_processStatus == TA_Base_Core::Unstarted)
        {
            // Wait for the start lag to expire.
            if ( !waitForStartLag() )
            {
                // Told to terminate before start lag is up.
                setProcessStatus(TA_Base_Core::Stopped);
                return;
            }
        }
        else
        {
			// Update the process object now that the process is restarting.
            resetProcess();
        }
        
		// Build the process's command line.
		std::string startPath;
		std::vector<std::string> args = buildCommandLine(startPath);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Starting non-managed process: %s", m_entityName.c_str());
        
        // Start the process.
        m_processId = ProcessUtilities::launchProcess(startPath, args, getUserName(), getPassWord());
        if (m_processId == 0)
        {
            // Process didn't start. Raise an alarm.
			raiseFailureAlarm(TA_Base_Core::InitError);

			// Cannot run the application so there is no point continuing to try
            setProcessStatus(TA_Base_Core::Stopped);
        }
        else
        {
            // Record the start time.
			updateStartTime();
            
			// Update the process status.
            m_processStatus = TA_Base_Core::RunningControl;

            // Close the alarm if one has been opened.
            closeFailureAlarm();

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Started process with pid %lu",
                        m_processId);
        }
    }

    //
    // performPoll
    //
    void ProcessInfo::performPoll()
    {
#ifdef WIN32
        sleep(m_pollPeriod);
        bool isRunning = ProcessUtilities::checkProcess(m_processId);

        if (!isRunning)
        {
            // The poll failed. Better terminate the process.
            performStop();

            // Now raise the alarm and allow the process to be restarted.
            handleProcessFailure(TA_Base_Core::CommsError);
        }
#endif
    }

    //
    // performStop
    //
    void ProcessInfo::performStop()
    {
        if (m_processId != 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Terminating process: %s",m_entityName.c_str());

            bool isSuccessful = false;
            
#ifdef WIN32
            isSuccessful = ProcessUtilities::terminateProcess(m_processId);
#else
            isSuccessful = ProcessUtilities::terminateProcess(m_processId);
#endif
            if (!isSuccessful)
            {
                int nLoops = m_killTimeout / ProcessManager::SLEEP_TIME_MSECS;
                for (int i = 0; i < nLoops; i++)
                {
                    sleep(ProcessManager::SLEEP_TIME_MSECS);

                    if (getProcessStatus() == TA_Base_Core::Stopped)
                    {
                        isSuccessful = true;
                        break;
                    }
                }
            }

            if (!isSuccessful)
            {
                ProcessUtilities::killProcess(m_processId);
            }
            m_processId = 0;
        }

        setProcessStatus(TA_Base_Core::Stopped);
    }

	//
	// updateStartTime
	//
	void ProcessInfo::updateStartTime()
	{
        TA_Base_Core::ThreadGuard guard(m_lock);

		long startTime = 0;
		time(&startTime);
		m_timeStarted = startTime;
	}

} // namespace TA_Base_App
