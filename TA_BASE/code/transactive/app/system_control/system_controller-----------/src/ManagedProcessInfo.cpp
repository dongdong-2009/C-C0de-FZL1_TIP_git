/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Justin Ebedes
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * ManagedProcessInfo maintains information about a
  * managed process (meaning a process that has control
  * and monitor modes, can be polled, etc). It inherits
  * from ProcessInfo and uses the IManagedProcessCorbaDef
  * and IManagedAgentCorbaDef interfaces to communicate
  * with the process.
  *
  */

#include "ManagedProcessInfo.h"
#include "ProcessUtilities.h"
#include "ProcessManager.h"
#include "SystemController.h"

#include "core/data_access_interface/system_controller/src/GroupMonitorAccessFactory.h"
#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "core/process_management/IDL/src/IManagedProcessCorbaDef.h"
#include "core/process_management/IDL/src/IManagedAgentCorbaDef.h"
#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/data_access_interface/entity_access/src/Process.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"

// #include "core/alarm/src/NonUniqueAlarmHelper.h"
// #include "core/alarm/src/AlarmHelperManager.h"
// #include "core/message/types/SystemAlarms_MessageTypes.h"

#include <ctime>
#include <sstream>

#include "ace/OS.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_App
{
    // The default timeouts in milliseconds.
    const unsigned int ManagedProcessInfo::DEFAULT_STARTUP_TIMEOUT        = 5000;
    const unsigned int ManagedProcessInfo::DEFAULT_TERMINATE_TIMEOUT      = 5000;
    const unsigned int ManagedProcessInfo::DEFAULT_CHANGE_CONTROL_TIMEOUT = 5000;
    const unsigned int ManagedProcessInfo::DEFAULT_CHANGE_MONITOR_TIMEOUT = 5000;

	// The timeout used to wait for SIGKILLS.
	const unsigned int ManagedProcessInfo::SIGKILL_TIMEOUT = 3000;

    //
    // Constructor
    //
    ManagedProcessInfo::ManagedProcessInfo(const std::string& entityName,
                        const std::string& childEntityName,
                        TA_Base_Core::IProcess* processData,
                        const std::vector<std::string>& peerHosts,
                        int pollPeriod,
                        const TA_Base_Core::EOperationMode& normalOperationMode,
                        int notifyPort,
                        unsigned int responseTimeout,
						std::string binDir,
						bool isStandbyProcess,
						std::string monitorGroupName,
						int startLag,
						bool hasStandbyProcess)
        : ProcessInfo(entityName, processData, pollPeriod, binDir), m_peerHosts(peerHosts),
		  m_peerState(NotInControl),
          m_managedProcess(TA_Base_Core::IManagedProcessCorbaDef::_nil()),
          m_managedAgent(TA_Base_Core::IManagedAgentCorbaDef::_nil()),
          m_normalOperationMode(normalOperationMode),
          m_isServiceProcess(false),
          m_notifyPort(notifyPort),
          m_responseTimeout(responseTimeout),
          m_statusToChangeTo(TA_Base_Core::Startup),
          m_initialOperationMode(TA_Base_Core::Control),
          m_childEntityName(childEntityName),
          m_isModeChangeComplete(false),
		  m_isStandbyProcess(isStandbyProcess),
		  m_monitorGroupName(monitorGroupName),
		  m_hasStandbyProcess(hasStandbyProcess),
		  m_isGroupOffline(false),
		  m_isGroupStartup(false)
    {
        m_shouldAutoFailback = processData->shouldAutoFailback();
		m_processPriority = processData->getPriority();
		m_boundProcessor = processData->getBoundProcessor();
        m_isServiceProcess = processData->isServiceProcess();
		m_locationKey = processData->getLocation();
		m_locationName = processData->getLocationName();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
            "%s will auto failback: %lu", entityName.c_str(), m_shouldAutoFailback);
        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
            "%s is a service process: %lu", entityName.c_str(), m_isServiceProcess);

        m_terminateTimeout = processData->getTerminateTimeout() * 1000;
        if (m_terminateTimeout == 0)
        {
            // Use a default terminate timeout.
            m_terminateTimeout = DEFAULT_TERMINATE_TIMEOUT;
        }

        m_startupTimeout = processData->getStartupTimeout() * 1000;
        if (m_startupTimeout == 0)
        {
            // Use a default startup timeout
            m_startupTimeout = DEFAULT_STARTUP_TIMEOUT;
        }

        m_changeControlTimeout = processData->getChangeControlTimeout() * 1000;
        if (m_changeControlTimeout == 0)
        {
            m_changeControlTimeout = DEFAULT_CHANGE_CONTROL_TIMEOUT;
        }

        m_changeMonitorTimeout = processData->getChangeMonitorTimeout() * 1000;
        if (m_changeMonitorTimeout == 0)
        {
            m_changeMonitorTimeout = DEFAULT_CHANGE_MONITOR_TIMEOUT;
        }

        // Initialise the process's debug level to the same as the System Controllers.
        // This can then later be changed through the System Manager.
        m_debugLevel = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
		if(startLag > 0)
		{
			m_startLag = startLag * 1000;
		}
    }

    //
    // Destructor
    //
    ManagedProcessInfo::~ManagedProcessInfo()
    {
		//TA_Base_Core::AlarmHelperManager::getInstance().cleanUp(); // gongzong++ CL-21328
    }

    //
    // notifyGroupOffline
    //
    void ManagedProcessInfo::notifyGroupOffline(const std::string& group)
    {
        try
        {
			if(group == m_monitorGroupName)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"GroupOffline: [monitorGroupName: %s] [isStandbyProcess: %d] [current status: %d]", m_monitorGroupName.c_str(), m_isStandbyProcess, getProcessStatus());
				m_isGroupOffline = true;
			}
            if (isRunning())
            {
				// yanrong++: CL-21237
				TA_Base_Core::ThreadGuard guard(m_lock);
				if(!CORBA::is_nil(m_managedAgent))
				{
					if(true == m_isStandbyProcess)
					{
						if(m_monitorGroupName == group)
						{
							if(m_peerState == RunningInControl || m_peerState == StartingInControl)
							{
								if(m_shouldAutoFailback && getNormalOperationMode() == TA_Base_Core::Control)
								{
									setStatusToChangeTo(TA_Base_Core::RunningControl);
								}
								else
								{
									setStatusToChangeTo(TA_Base_Core::RunningMonitor);
								}
							}
							else if(m_peerState == RunningInMonitor || m_peerState == StartingInMonitor)
							{
								setStatusToChangeTo(TA_Base_Core::RunningControl);
							}
							else
							{
								setStatusToChangeTo(TA_Base_Core::RunningControl);
							}
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
								"GroupOffline: StandbyProcess [EntityName: %s] [OperationMode: %d]", this->getEntityName().c_str(), m_normalOperationMode);
						}
					}
					m_managedAgent->notifyGroupOffline(group.c_str());
				}
				// ++yanrong
            }
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error calling notifyGroupOffline: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

            // The call failed so we indicate that the process should terminate
            setStatusToChangeTo(TA_Base_Core::NotRunning);

            // Now raise the alarm and allow the process to be restarted.
            raiseFailureAlarm(TA_Base_Core::CommsError);
        }
    }

    //
    // notifyGroupOnline
    //
    void ManagedProcessInfo::notifyGroupOnline(const std::string& group)
    {
        try
        {
			if(group == m_monitorGroupName)
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"GroupOnline: [monitorGroupName: %s] [isStandbyProcess: %d] [current status: %d]", m_monitorGroupName.c_str(), m_isStandbyProcess, getProcessStatus());
				m_isGroupOffline = false;
			}
            if (isRunning())
            {
				// yanrong++: CL-21237
				TA_Base_Core::ThreadGuard guard(m_lock);
				if(!CORBA::is_nil(m_managedAgent))
				{
					if(true == m_isStandbyProcess)
					{
						if(m_monitorGroupName == group)
						{
                            setStatusToChangeTo(TA_Base_Core::RunningStandby);
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
								"GroupOnline, StandbyProcess [EntityName: %s] [OperationMode: %d]", this->getEntityName().c_str(), m_normalOperationMode);
						}
					}
					m_managedAgent->notifyGroupOnline(group.c_str());
				}
				// ++yanrong
            }
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error calling notifyGroupOnline: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

            // The call failed so we indicate that the process should terminate
            setStatusToChangeTo(TA_Base_Core::NotRunning);

            // Now raise the alarm and allow the process to be restarted.
            raiseFailureAlarm(TA_Base_Core::CommsError);
        }
    }

    //
    // invalidateNotifyHosts
    //
    void ManagedProcessInfo::invalidateNotifyHosts(const std::vector<std::string>& invalidNotifyHosts)
    {
        FUNCTION_ENTRY("invalidateNotifyHosts");

        try
        {
            if (isRunning())
            {
                TA_Base_Core::IManagedAgentCorbaDef::NotifyHostSequence result;
                result.length(invalidNotifyHosts.size());

                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Informing %s that the following NotifyHosts are invalid:", getEntityName().c_str());

       	        for (unsigned int i = 0; i < invalidNotifyHosts.size(); i++)
                {
                    LOGMORE(SourceInfo, "%s", invalidNotifyHosts[i].c_str());
    				result[i] = invalidNotifyHosts[i].c_str();
    	        }

				// yanrong++: CL-21237
				TA_Base_Core::ThreadGuard guard(m_lock);
				if(!CORBA::is_nil(m_managedAgent))
				{
					m_managedAgent->invalidateNotifyHosts(result);
				}
				// ++yanrong
            }
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error calling invalidateNotifyHosts: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

            setStatusToChangeTo(TA_Base_Core::NotRunning);

            // Now raise the alarm and allow the process to be restarted.
            raiseFailureAlarm(TA_Base_Core::CommsError);
        }

        FUNCTION_EXIT;
    }

    //
    // getOperationMode
    //
    TA_Base_Core::EOperationMode ManagedProcessInfo::getOperationMode()
    {
        if (!isRunning())
        {
            return TA_Base_Core::NotApplicable;
        }

        TA_Base_Core::EProcessStatus currentStatus = getProcessStatus();
        TA_Base_Core::EProcessStatus requestedStatus = getStatusToChangeTo();

        try
        {
            // If the process is currently changing states then report
            // the state it is currently changing to
            if (requestedStatus == TA_Base_Core::RunningControl &&
                currentStatus != TA_Base_Core::RunningControl)
            {
                return TA_Base_Core::Control;
            }
            else if (requestedStatus == TA_Base_Core::RunningMonitor &&
                     currentStatus != TA_Base_Core::RunningMonitor)
            {
                return TA_Base_Core::Monitor;
            }
			else if (requestedStatus == TA_Base_Core::RunningStandby &&
				currentStatus != TA_Base_Core::RunningStandby)
			{
				return TA_Base_Core::Standby;
			}
            else if (currentStatus == TA_Base_Core::RunningControl)
            {
                return TA_Base_Core::Control;
            }
            else if (currentStatus == TA_Base_Core::RunningMonitor)
            {
                return TA_Base_Core::Monitor;
            }
			else if (currentStatus == TA_Base_Core::RunningStandby)
			{
				return TA_Base_Core::Standby;
			}
			else if (currentStatus == TA_Base_Core::Startup)
			{
				// The agent is still starting up, but if it's registered
				// we can fetch its operation mode from it directly.
				// yanrong++: CL-21237
                    TA_THREADGUARD( m_lock );
				// ++yanrong
                    if ( !CORBA::is_nil( m_managedAgent ) )
				{
					// Agent has registered.
					return m_managedAgent->getOperationMode();
				}
                }
                else
                {
                    return TA_Base_Core::NotApplicable;
			}
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error calling getOperationMode: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

			// Don't kill the agent if this doesn't succeed. This call can fail
			// if the agent has registered but GenericAgent hasn't activated
			// the servant yet.
        }

        return TA_Base_Core::NotApplicable;
    }

	//
	// isRunning
	//
    bool ManagedProcessInfo::isRunning()
    {
        TA_Base_Core::EProcessStatus current = getProcessStatus();

        if (current == TA_Base_Core::Startup ||
            current == TA_Base_Core::RunningControl ||
            current == TA_Base_Core::RunningMonitor ||
            current == TA_Base_Core::GoingToControl ||
            current == TA_Base_Core::GoingToMonitor ||
			current == TA_Base_Core::GoingToStandby ||
			current == TA_Base_Core::RunningStandby)
        {
            return true;
        }

        return false;
    }



	// gongzong++ CL-21328
// 	//
// 	// raiseFailoverAlarm
// 	//
//     void ManagedProcessInfo::raiseFailoverAlarm()
// 	{
//         // Raise an alarm if one hasn't been raised already.
//         if (m_failoverAlarmId == "")
//         {
//             TA_Base_Core::DescriptionParameters dp;
// 			TA_Base_Core::NameValuePair host("HostId",
// 				ProcessManager::getInstance().getHostName());
// 			TA_Base_Core::NameValuePair name("ProcessId", getEntityName());
// 
// 	        dp.push_back(&host);
// 	        dp.push_back(&name);
// 
//             m_failoverAlarmId = raiseAlarm(TA_Base_Core::SystemAlarms::FailoverOccurred, dp);
//         }
// 	}
// 
// 	//
// 	// closeFailoverAlarm
// 	//
//     void ManagedProcessInfo::closeFailoverAlarm()
// 	{
//         FUNCTION_ENTRY("closeFailoverAlarm");
// 
//         if (m_failoverAlarmId != "")
//         {
//             try
//             {
//                 // Need the Server entity's location to close the alarm.
//                 TA_Base_Core::ServerEntityData* serverData =
//                     ProcessManager::getInstance().getServerEntityData();
// 
// 				TA_Base_Core::AlarmHelperManager::getInstance().getNonUniqueAlarmHelper().closeAlarmAtLocation(m_failoverAlarmId, serverData->getLocation());
//                 m_failoverAlarmId = "";
//             }
//             catch(TA_Base_Core::TransactiveException& ex)
//             {
//                 LOG_EXCEPTION_CATCH(SourceInfo, "TransactiveException", ex.what());
//             }
//             catch(...)
//             {
//                 LOG_EXCEPTION_CATCH(SourceInfo, "Unknown exception", "");
//             }
//         }
// 
//         FUNCTION_EXIT;
// 	}

    //
    // waitForStartLag
    //
    bool ManagedProcessInfo::waitForStartLag()
    {
        FUNCTION_ENTRY("waitForStartLag");

        const int SLEEP_TIME = 1000; // Sleep for 1 second at a time.
        int nLoops = getStartLag() / SLEEP_TIME;

        for (int i = 0; i < nLoops; i++)
        {
            if (getStatusToChangeTo() == TA_Base_Core::Stopped)
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
    // getInitOperationMode
    //
    TA_Base_Core::EOperationMode ManagedProcessInfo::getInitOperationMode()
    {
        FUNCTION_ENTRY("getInitOperationMode");

        TA_Base_Core::ThreadGuard guard(m_lock);

		if ( true == m_hasStandbyProcess )
		{
			notifyStandbyProcess();
		}
        if (m_peerState != RunningInControl && m_peerState != StartingInControl)
        {
            m_initialOperationMode = TA_Base_Core::Control;
        }
        else
        {
            m_initialOperationMode = TA_Base_Core::Monitor;
        }

        if ( true == m_isStandbyProcess )
        {
            if ( m_peerState == RunningInControl || m_peerState == StartingInControl )
            {
                m_initialOperationMode = TA_Base_Core::Monitor;
            }
            else if ( m_peerState == RunningInMonitor || m_peerState == StartingInMonitor )
            {
                m_initialOperationMode = TA_Base_Core::Control;
            }
            else
            {
                m_initialOperationMode = TA_Base_Core::Standby;
            }
        }

        FUNCTION_EXIT;
        return m_initialOperationMode;
    }

    //
    // statusChanged
    //
    void ManagedProcessInfo::statusChanged(TA_Base_Core::EProcessStatus status)
    {
        FUNCTION_ENTRY("statusChanged");

		TA_Base_Core::EProcessStatus oldStatus = getProcessStatus();

        setProcessStatus(status);

		if ( (oldStatus == TA_Base_Core::GoingToControl && status == TA_Base_Core::RunningControl)
			|| (oldStatus == TA_Base_Core::GoingToMonitor && status == TA_Base_Core::RunningMonitor) 
			|| (oldStatus == TA_Base_Core::GoingToStandby && status == TA_Base_Core::RunningStandby))
		{
			// Set the flag indicating the agent has finished changing operation modes.
			modeChangeComplete();
		}

        FUNCTION_EXIT;
    }

    //
    // setParam
    //
    void ManagedProcessInfo::setParam(TA_Base_Core::RunParam param)
    {
        try
        {
           if (isRunning())
           {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "Setting following RunParam: (%s, %s)",
                     param.name.in(), param.value.in());
				// yanrong++: CL-21237
				TA_Base_Core::ThreadGuard guard(m_lock);
				if(!CORBA::is_nil(m_managedProcess))
				{
					m_managedProcess->setParam(param);
				}
				// ++yanrong
		   }

		   if (strcmp(param.name.in(), RPARAM_DEBUGLEVEL) == 0)
           {
               TA_Base_Core::ThreadGuard guard(m_lock);

               // Debug level being set. Update the member variable.
               m_debugLevel = param.value.in();
               LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                    "Debug level is now %s", m_debugLevel.c_str());
           }

        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error calling setParam: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

            setStatusToChangeTo(TA_Base_Core::NotRunning);

            // Now raise the alarm and allow the process to be restarted.
            raiseFailureAlarm(TA_Base_Core::CommsError);
        }
    }

    //
    // poll
    //
    void ManagedProcessInfo::poll()
    {
		// yanrong++: CL-21237
		TA_Base_Core::ThreadGuard guard(m_lock);
		if(!CORBA::is_nil(m_managedProcess))
		{
			m_managedProcess->poll();
		}
		// ++yanrong
	}

    //
    // setManagedProcess
    //
    void ManagedProcessInfo::setManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr managedProcess)
    {
       TA_Base_Core::ThreadGuard guard(m_lock);

        // Duplicate the ptr before assigning it a member variable
        m_managedProcess = TA_Base_Core::IManagedProcessCorbaDef::_duplicate(managedProcess);
    }

    //
    // setManagedAgent
    //
    void ManagedProcessInfo::setManagedAgent(TA_Base_Core::IManagedApplicationCorbaDef_ptr managedApplication)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

        // Duplicate the ptr
        TA_Base_Core::IManagedApplicationCorbaDef_var managedApp;
        managedApp = TA_Base_Core::IManagedApplicationCorbaDef::_duplicate(managedApplication);

        // Narrow the ManagedApplicationCorbaDef to a ManagedAgentCorbaDef
        m_managedAgent = TA_Base_Core::IManagedAgentCorbaDef::_narrow(managedApplication);
    }

    //
    // getProcessData
    //
    TA_Base_Core::ProcessData* ManagedProcessInfo::getProcessData()
    {
       // Instead of threadlocking this method, threadlock all of the get()
        // methods called by this method.

        TA_Base_Core::ProcessData* data = new TA_Base_Core::ProcessData;
        if (data == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                        "New failed to allocate a ProcessData object");
            exit(1);
        }

        data->ProcessId = getEntityName().c_str();
        data->started = getTimeStarted();

        std::string hostname = ProcessManager::getInstance().getHostName();
        data->host = hostname.c_str();
        data->state = getProcessStatus();
        data->requestedState = getStatusToChangeTo();
        data->operationMode = getOperationMode();

        data->restarts = getNumRestarts();
        data->debugLevel = getDebugLevel().c_str();

        return data;
    }

    //
    // isAPeer
    //
    bool ManagedProcessInfo::isAPeer(const std::string& hostname)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

        for (std::vector<std::string>::const_iterator iter = m_peerHosts.begin();
            iter != m_peerHosts.end(); iter++)
        {
            if ((*iter) == hostname)
            {
                // Hostname found in our vector of peers.
                return true;
            }
        }

        // Hostname not found, therefore not a peer.
        return false;
    }

    //
    // run
    //
    void ManagedProcessInfo::run()
    {
        FUNCTION_ENTRY("run");

        m_isRunning = true;

        while (m_isRunning)
        {
            TA_Base_Core::EProcessStatus processStatus = getProcessStatus();
            TA_Base_Core::EProcessStatus statusToChangeTo = getStatusToChangeTo();

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "ManagedProcessInfo::run[%s], status:%d, toStatus:%d",
				getEntityName().c_str(), processStatus, statusToChangeTo);
            if (processStatus == TA_Base_Core::NotRunning
				&& statusToChangeTo != TA_Base_Core::Stopped)
            {
                setStatusToChangeTo(TA_Base_Core::Startup);
                statusToChangeTo = getStatusToChangeTo();
            }

            if ( processStatus != statusToChangeTo )
            {
                changeState(processStatus, statusToChangeTo);
            }
            else if (processStatus == TA_Base_Core::RunningControl ||
                     processStatus == TA_Base_Core::RunningMonitor ||
					 processStatus == TA_Base_Core::RunningStandby)
            {
                // We only need to poll if we are running in control or monitor mode. All other states are
                // transition states and are timed so if the agent hangs we will know about it without having to poll.
                performPoll();
            }
            else
            {
                sleep(ProcessManager::SLEEP_TIME_MSECS);
            }
        }

        FUNCTION_EXIT;
    }

    //
	// setStatusToChangeTo
	//
    void ManagedProcessInfo::setStatusToChangeTo(TA_Base_Core::EProcessStatus newStatus)
    {
        TA_Base_Core::ThreadGuard guard(m_changeStateLock);

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
			"Status to change to for %s: From %d to %d", getEntityName().c_str(), m_statusToChangeTo, newStatus);

        if ( m_statusToChangeTo == TA_Base_Core::NotRunning &&
             newStatus != TA_Base_Core::Startup &&
             newStatus != TA_Base_Core::Stopped)
        {
            // Leave as NotRunning because the error that causes the shutdown
			// overrides anything except stopping and restarting.
            return;
        }

        if (m_statusToChangeTo == TA_Base_Core::Stopped && newStatus != TA_Base_Core::Startup)
        {
            // Leave this as we are already stopped and we don't want the process
			// to be restarted since the user has manually stopped it.
            return;
        }

        m_statusToChangeTo = newStatus;
    }

    //
    // getParams
    //
    TA_Base_Core::RunParams::ParamVector ManagedProcessInfo::getParams()
    {
        FUNCTION_ENTRY("getParams");

        TA_Base_Core::RunParams::ParamVector params;
        TA_Base_Core::RunParams::ParamNameValue pnv;

        pnv.name = RPARAM_DEBUGLEVEL;
        pnv.value = getDebugLevel().c_str();
        params.push_back(pnv);

        pnv.name = RPARAM_DEBUGFILE;
        std::ostringstream filename;
        filename << ProcessManager::getInstance().getDebugLogDir();
        filename << m_childEntityName;
        filename << "." << getDate();
        filename << "." << getNumRestarts();
        pnv.value = filename.str();
        params.push_back(pnv);

        pnv.name = RPARAM_OPERATIONMODE;
        TA_Base_Core::EOperationMode mode = getInitOperationMode();
        if (mode == TA_Base_Core::Control || isServiceProcess())
        {
            pnv.value = RPARAM_CONTROL;
        }
        else if (mode == TA_Base_Core::Monitor)
        {
            pnv.value = RPARAM_MONITOR;
        }
        else if (mode == TA_Base_Core::Standby)
        {
            pnv.value = RPARAM_STANDBY;
        }
        params.push_back(pnv);
		pnv.name = RPARAM_STATION_DEPLOY_KEY;
		pnv.value = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str();
        params.push_back(pnv);

        FUNCTION_EXIT;
        return params;
    }

	//
	// setPeerState
	//
	void ManagedProcessInfo::setPeerState(std::string hostName, EPeerState state)
	{
		TA_Base_Core::ThreadGuard guard(m_lock);

		if (state == RunningInControl)
		{
			// A peer is in the RunningControl state.
			m_peerState = RunningInControl;
			m_peerInControl = hostName;
		}
		else if (state == StartingInControl)
		{
			// A peer is starting up in Control.
			if (m_peerState != RunningInControl)
			{
				m_peerState = StartingInControl;
				m_peerInControl = hostName;
			}
		}
        else if (state == RunningInMonitor)
        {
            m_peerState = RunningInMonitor;
        }
        else if (state == StartingInMonitor)
        {
            if (m_peerState != RunningInMonitor)
            {
                m_peerState = StartingInMonitor;
            }
        }
        else if (state == RunningInStandby)
        {
            m_peerState = RunningInStandby;
        }
        else if (state == StartingInStandby)
        {
            if (m_peerState != RunningInStandby)
            {
                m_peerState = StartingInStandby;
			}
		}
		else if (state == NotInControl)
		{
			// A peer is not in Control. If this was the peer that was
			// formerly in Control, then update m_peerState.
			if (hostName == m_peerInControl)
			{
				m_peerState = NotInControl;
			}
		}
	}

    //
    // setPeerRunning
    //
    void ManagedProcessInfo::setPeerRunning(std::string peerRunning)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

        m_runningPeerHosts.insert(peerRunning);
    }

    //
    // setPeerNotRunning
    //
    void ManagedProcessInfo::setPeerNotRunning(std::string peerNotRunning)
    {
        TA_Base_Core::ThreadGuard guard(m_lock);

        m_runningPeerHosts.erase(peerNotRunning);
    }

    //
    // processTerminating
    //
    void ManagedProcessInfo::processTerminating(TA_Base_Core::EManagedProcessTerminateCode reason)
    {
        FUNCTION_ENTRY("processTerminating");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Process %s has instructed the System Controller that it is terminating.",
            getEntityName().c_str());

		setTerminateStatus(reason);

        if (reason == TA_Base_Core::ConfigError)
        {
            // Need to make sure an alarm is raised for this, so close any open failure alarms
            // before submitting a new one.
            closeFailureAlarm();
            raiseFailureAlarm(reason);
        }

        setProcessStatus(TA_Base_Core::Terminating);

        TA_Base_Core::EProcessStatus changingTo = getStatusToChangeTo();

        // If the status the agent is changing to is NotRunning, then it was killed and
        // should be restarted. Otherwise, it's status should be Stopped so that it's not
        // restarted.
        if (changingTo != TA_Base_Core::NotRunning)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "%s will not be restarted", getEntityName().c_str());
            setStatusToChangeTo(TA_Base_Core::Stopped);
        }

        FUNCTION_EXIT;
    }

	//
	// changeState
	//
    void ManagedProcessInfo::changeState(TA_Base_Core::EProcessStatus currentStatus,
                                          TA_Base_Core::EProcessStatus newStatus)
    {
        FUNCTION_ENTRY("changeState");
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                    "%s About to be told to change from state %d to state %d",
                    getEntityName().c_str(), currentStatus, newStatus);

        if (newStatus == TA_Base_Core::Startup &&
            !isRunning())
        {
            performStart();
        }
        else if (newStatus == TA_Base_Core::RunningControl &&
                 currentStatus != TA_Base_Core::RunningControl )
        {
            performChangeToControl();
        }
        else if (newStatus == TA_Base_Core::RunningMonitor &&
                 currentStatus != TA_Base_Core::RunningMonitor)
        {
            performChangeToMonitor();
        }
		else if (newStatus == TA_Base_Core::RunningStandby &&
			currentStatus != TA_Base_Core::RunningStandby)
		{
			performChangeToStandby();
		}
        else if( newStatus == TA_Base_Core::NotRunning ||
                 newStatus == TA_Base_Core::Stopped ||
                 newStatus == TA_Base_Core::Terminating)
        {
            if (isRunning() || currentStatus == TA_Base_Core::Terminating)
            {
                // The process is running or may have stalled while terminating
                // so we should ensure it stops
                performStop(newStatus);
            }
            else if (newStatus == TA_Base_Core::Stopped)
            {
                setProcessStatus(TA_Base_Core::Stopped);
            }
            else if (newStatus == TA_Base_Core::NotRunning)
            {
                setProcessStatus(TA_Base_Core::Startup);
            }
        }
		else if(newStatus == TA_Base_Core::RunningStandby)
		{
			setProcessStatus(TA_Base_Core::RunningStandby);
        }

        FUNCTION_EXIT;
    }

	//
	// buildCommandLine
	//
	std::vector<std::string> ManagedProcessInfo::buildCommandLine(std::string& startPath)
	{
		// Parse the process address.
        std::vector<std::string> args;
        ProcessUtilities::parseProcessAddress(startPath, args, getAddress());

        // Add some additional command line arguments.
        std::ostringstream entityNameArg;
        entityNameArg << "--entity-name=" << m_childEntityName;
        args.push_back(entityNameArg.str());

        std::ostringstream mgrPortArg;
        mgrPortArg << "--mgr-port=" << TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT);
        args.push_back(mgrPortArg.str());

        std::vector< std::pair< std::string, std::string> > databaseStatusList = ProcessManager::getInstance().getDatabaseStatus();

        for ( size_t i = 0; i < databaseStatusList.size(); ++i )
        {
            std::stringstream databaseStatusArg;
            databaseStatusArg << "--" << databaseStatusList[i].first << "=" << databaseStatusList[i].second;
            args.push_back( databaseStatusArg.str() );
            LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "Set runtime parameter %s for %s", databaseStatusArg.str().c_str(), m_childEntityName.c_str());
        }

        std::stringstream debugLevelArg;
        debugLevelArg << "--" << RPARAM_DEBUGLEVEL << "=" << getDebugLevel();
        args.push_back( debugLevelArg.str() );

        std::stringstream debugFileArg;
        debugFileArg << "--" << RPARAM_DEBUGFILE << "=" << ProcessManager::getInstance().getDebugLogDir() << m_childEntityName << "." << getDate() << "." << getNumRestarts();
        args.push_back( debugFileArg.str() );

        std::stringstream charsetArg;
        charsetArg << "--" << RPARAM_CHARSET << "=" << TA_Base_Core::RunParams::getInstance().get(RPARAM_CHARSET);
        args.push_back( charsetArg.str() );

        //// Wenguang++
        //std::ostringstream controlDbOnlineArg;
        //std::string controlDbName;
        //std::string controlDbStatus;
        //controlDbName = ProcessManager::getInstance().getControlDatabaseName(controlDbStatus);
        //if( !controlDbName.empty())
        //{
        //    controlDbOnlineArg << "--"<<RPARAM_DBPREFIX << controlDbName.c_str() << "="<< controlDbStatus.c_str();
        //    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Set runtime parameter %s for %s", controlDbOnlineArg.str().c_str(), m_childEntityName.c_str());
        //    args.push_back(controlDbOnlineArg.str());        
        //}

        //// Wenguang++
        //std::ostringstream monitoredDbOnlineArg;
        //std::string monitoredDbName;
        //std::string monitoredDbStatus;
        //monitoredDbName = ProcessManager::getInstance().getMonitoredDatabaseName(monitoredDbStatus);
        //if( !monitoredDbName.empty() )
        //{
        //    monitoredDbOnlineArg << "--"<< RPARAM_DBPREFIX <<monitoredDbName.c_str() << "="<< monitoredDbStatus.c_str();
        //    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Set runtime parameter %s for %s", monitoredDbOnlineArg.str().c_str(), m_childEntityName.c_str());
        //    args.push_back(monitoredDbOnlineArg.str());
        //}

        //std::ostringstream centralDbOnlineArg;
        //std::string centralDbName;
        //std::string centralDbStatus;
        //centralDbName = ProcessManager::getInstance().getCentralDatabaseName(centralDbStatus);
        //if( !centralDbName.empty() )
        //{
        //    centralDbOnlineArg << "--"<< RPARAM_DBPREFIX <<centralDbName.c_str() << "="<< centralDbStatus.c_str();
        //    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Set runtime parameter %s for %s", centralDbOnlineArg.str().c_str(), m_childEntityName.c_str());
        //    args.push_back(centralDbOnlineArg.str());
        //}

        std::ostringstream databaseConnectionStringArg;
        std::string databaseConnectionString = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
        if( !databaseConnectionString.empty() )
        {
            // TD18636
            std::string entityLocationName = ProcessManager::getInstance().getServerEntityData()->getLocationName();

            if (entityLocationName.compare(m_locationName) != 0)
            {
                // find last occurrence of location in case it also appears in the path
                size_t loc = databaseConnectionString.rfind(entityLocationName);
                if (loc != std::string::npos)
                {
                    databaseConnectionString.erase(loc, entityLocationName.size());
                    databaseConnectionString.insert(loc, m_locationName);
                }
            }
            
            databaseConnectionStringArg << "--db-connection-file="<< databaseConnectionString.c_str();
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Set runtime parameter %s for %s", databaseConnectionStringArg.str().c_str(), m_childEntityName.c_str());
            args.push_back(databaseConnectionStringArg.str());
            // TD18636
        }

        if (isServiceProcess())
        {
            // Need a notify port param.
            std::ostringstream notifyPortArg;
            notifyPortArg << " --notify-port=" << m_notifyPort;
            args.push_back(notifyPortArg.str());

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
                "%s will be started with a notify port of %d",
                getEntityName().c_str(), m_notifyPort);
        }

		return args;
	}

    //
    // performStart
    //
    void ManagedProcessInfo::performStart()
    {
        FUNCTION_ENTRY("performStart");

        if (getProcessStatus() == TA_Base_Core::Unstarted)
        {
            // Wait for the start lag to expire.
            if ( !waitForStartLag() )
            {
                // Told to terminate before start lag is up.
                setProcessStatus(TA_Base_Core::Stopped);
                setStatusToChangeTo(TA_Base_Core::Stopped);
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

        // Start the process.
		TA_Base_Core::ProcessId pid = ProcessUtilities::launchProcess(startPath, args, getUserName(),getPassWord());
		setProcessId(pid);
        if (pid == 0)
        {
            // Process didn't start. Raise an alarm.
			raiseFailureAlarm(TA_Base_Core::InitError);

            // Cannot run the application so there is no point continuing to try
            setStatusToChangeTo(TA_Base_Core::Stopped);
            setProcessStatus(TA_Base_Core::Stopped);

            FUNCTION_EXIT;
            return;
        }

        // Record the start time.
		updateStartTime();

        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Started process %s with pid %lu",
			getEntityName().c_str(), pid);

		// Set the process's priority
		ProcessUtilities::setPriority(getProcessId(), m_processPriority);

		// Bind the process to a processor, if necessary.
		if (m_boundProcessor != TA_Base_Core::Process::NOT_BOUND)
		{
			ProcessUtilities::bindToProcessor(getProcessId(), m_boundProcessor);
		}

        bool stateReportedSuccessfully = false;

		// Sleep until the timeout expires, or until a signal has been
		// received for the process.
		int nLoops = m_startupTimeout / ProcessManager::SLEEP_TIME_MSECS;
		for (int i = 0; i < nLoops; i++)
		{
			sleep(ProcessManager::SLEEP_TIME_MSECS);

			if (hasBeenSignalled())
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
					"%s with pid %lu Process was stopped or died while starting so it will be stopped",
					getEntityName().c_str(), pid);

				// The process has stopped itself (or has been stopped manually)
				// so we can stop waiting for it
				FUNCTION_EXIT;
				return;
			}

            if ( ( isRunning() || TA_Base_Core::Terminating == getProcessStatus() || TA_Base_Core::RequestedTerminate == getTerminateStatus() ) && !stateReportedSuccessfully )
            {
                // Agent has reported status is changing
                stateReportedSuccessfully = true;

                // Now we need to tell the peers that we are changing state so they change too
                ProcessManager::getInstance().notifyPeers(getEntityName());
			}

			if (stateReportedSuccessfully)
			{
				if (getProcessStatus() == TA_Base_Core::RunningStandby && true == m_isStandbyProcess)
				{
					if(true == m_isGroupOffline)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Standby Process %s successfully started in Control or Monitor mode",
							getEntityName().c_str());

						TA_Base_Core::ThreadGuard guard(m_lock);

						// We have started in monitor mode so now see if we have to failback
						if( m_shouldAutoFailback && getNormalOperationMode() == TA_Base_Core::Control)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Failing back %s", getEntityName().c_str());
							setStatusToChangeTo(TA_Base_Core::RunningControl);
						}
						else if (m_peerState != RunningInControl)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Peer is no longer in Control. Starting %s in Control", getEntityName().c_str());
							setStatusToChangeTo(TA_Base_Core::RunningControl);
						}
						else
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Process %s status to change to will be changed to Running Monitor", getEntityName().c_str());
							setStatusToChangeTo(TA_Base_Core::RunningMonitor);
						}
					} 
					else
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Process %s successfully started in Standby mode",
							getEntityName().c_str());
						if (m_peerState == RunningInControl || m_peerState == StartingInControl)
						{
							if(m_shouldAutoFailback && getNormalOperationMode() == TA_Base_Core::Control)
							{
								setStatusToChangeTo(TA_Base_Core::RunningControl);
							}
							else
							{
								setStatusToChangeTo(TA_Base_Core::RunningMonitor);
							}
						}
						else if (m_peerState == RunningInMonitor || m_peerState == StartingInMonitor)
						{
							setStatusToChangeTo(TA_Base_Core::RunningControl);
						}
						else	
						{
							setStatusToChangeTo(TA_Base_Core::RunningStandby);
						}
					}
                    ProcessManager::getInstance().notifyPeers(getEntityName());
					FUNCTION_EXIT;
					return;
				}
				if (getProcessStatus() == TA_Base_Core::RunningControl)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Process %s successfully started in Control mode",
						getEntityName().c_str());

					if (getStatusToChangeTo() == TA_Base_Core::Startup)
					{
						LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Process %s status to change to will be changed to Running Control", getEntityName().c_str());
						// Indicate that we are now in the correct state so a change isn't made next time
						setStatusToChangeTo(TA_Base_Core::RunningControl);
					}
					if(true == m_isGroupStartup)
					{
						m_isGroupStartup = false;
						setStatusToChangeTo(TA_Base_Core::RunningStandby);
					}
					ProcessManager::getInstance().notifyPeers(getEntityName());

					FUNCTION_EXIT;
					return;
				}

				if (getProcessStatus() == TA_Base_Core::RunningMonitor)
				{
					{
						TA_Base_Core::ThreadGuard guard(m_lock);

						LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Process %s successfully started in Monitor mode",
							getEntityName().c_str());

						// We have started in monitor mode so now see if we have to failback
						if( m_shouldAutoFailback && getNormalOperationMode() == TA_Base_Core::Control)
						{
							// We've restarted in Monitor, and we're configured to autofailback.
							LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Failing back %s", getEntityName().c_str());

							setStatusToChangeTo(TA_Base_Core::RunningControl);
						}
						else if (m_peerState != RunningInControl)
						{
							// We started in Monitor, but there's no longer a peer in control.
							// Switch to Control.
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
								"Peer is no longer in Control. Starting %s in Control", getEntityName().c_str());

							setStatusToChangeTo(TA_Base_Core::RunningControl);

							// gongzong++ CL-21328
							//raiseFailoverAlarm();
						}
						else if (getStatusToChangeTo() == TA_Base_Core::Startup)
						{
							LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Process %s status to change to will be changed to Running Monitor", getEntityName().c_str());
							// Indicate that we are now in the correct state so a change isn't made next time
							setStatusToChangeTo(TA_Base_Core::RunningMonitor);
						}
					} // Release threadlock
					if(true == m_isGroupStartup)
					{
						m_isGroupStartup = false;
						setStatusToChangeTo(TA_Base_Core::RunningStandby);
					} // Release threadlock

					ProcessManager::getInstance().notifyPeers(getEntityName());

					FUNCTION_EXIT;
					return;
				}

				if (getProcessStatus() == TA_Base_Core::Terminating ||
					getProcessStatus() == TA_Base_Core::NotRunning ||
					getStatusToChangeTo() == TA_Base_Core::NotRunning ||  // This means an error has occurred and the process has to be terminated
					getStatusToChangeTo() == TA_Base_Core::Stopped)
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "%s with pid %lu Process was stopped or died while starting so it will be stopped",
						getEntityName().c_str(), pid);
					// The process has stopped itself (or has been stopped manually)
					// so we can stop waiting for it
					FUNCTION_EXIT;
					return;
				}
			}
		}

        // The process has not started successfully in the allocated time.
        // We should now indicate that the process should be killed
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The process %s did not complete startup in time. It will be terminated", getEntityName().c_str());

 	    // Now raise the alarm and allow the process to be restarted.
		setStatusToChangeTo(TA_Base_Core::NotRunning);
        raiseFailureAlarm(TA_Base_Core::TimeoutError);

        // Also update the status of the app so that we ensure it gets killed
        setProcessStatus(TA_Base_Core::Startup);

        // The process has finished starting up
        ProcessManager::getInstance().notifyPeers(getEntityName());

        FUNCTION_EXIT;
    }

    //
    // performPoll
    //
    void ManagedProcessInfo::performPoll()
    {
		const int NUM_ATTEMPTS = 2;	 // The number of times we should attempt polling
									 // the agent before killing it.
		int attemptNum = 0;

		while (attemptNum < NUM_ATTEMPTS)
		{

			// Need to sleep for getPollPeriod() milliseconds.
			int nLoops = getPollPeriod() / ProcessManager::SLEEP_TIME_MSECS;
			for (int i = 0; i < nLoops; i++)
			{
				sleep(ProcessManager::SLEEP_TIME_MSECS);

				if( getStatusToChangeTo() == TA_Base_Core::RunningControl &&
					(getProcessStatus() != TA_Base_Core::RunningControl) )
				{
					// State change required so don't waste time on a poll
					return;
				}
				else if (getStatusToChangeTo() == TA_Base_Core::RunningMonitor &&
						 (getProcessStatus() != TA_Base_Core::RunningMonitor) )
				{
					return;
				}
				else if (getStatusToChangeTo() == TA_Base_Core::RunningStandby &&
					(getProcessStatus() != TA_Base_Core::RunningStandby) )
				{
					// State change required so don't waste time on a poll
					return;
				}
				else if (getStatusToChangeTo() == TA_Base_Core::Stopped ||
						 getStatusToChangeTo() == TA_Base_Core::NotRunning ||
						 getProcessStatus() == TA_Base_Core::NotRunning )
				{
					// No need to continue cos the process needs to be terminated anyway
					return;
				}
			}

			// Now poll the agent.
			try
			{
				attemptNum++;
				poll();

				// Poll succeeded.
				return;
			}
            catch(TA_Base_Core::IManagedProcessCorbaDef::PollException&)
            {
            	// The poll failed due to the agent detecting an internal problem!
				LOG_EXCEPTION_CATCH( SourceInfo, "PollException",
                    "Error polling agent. The agent detected an internal problem and threw a PollException");
            }
			catch(CORBA::SystemException& e)
			{
				// The poll failed.
				std::string details = "Error calling poll: ";
				details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
				LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);
			}
		}

		// The agent could not be polled in NUM_ATTEMPT attempts. Kill it.
		setStatusToChangeTo(TA_Base_Core::NotRunning);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
			"Cannot poll process %s. Will terminate it and then restart it.",
			getEntityName().c_str());

		// Now raise the alarm and allow the process to be restarted.
		raiseFailureAlarm(TA_Base_Core::CommsError);
	}

    //
    // performChangeToControl
    //
    void ManagedProcessInfo::performChangeToControl()
    {
        FUNCTION_ENTRY("performChangeToControl");

        if (getProcessId() == 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Process %s asked to change into control mode but is not currently running", getEntityName().c_str());
            FUNCTION_EXIT;
            return;
        }

        // Ask the agent to change into Control mode
        try
        {
			// yanrong++: CL-21237
			TA_Base_Core::ThreadGuard guard(m_lock);
			if(!CORBA::is_nil(m_managedAgent))
            {
				m_managedAgent->setOperationMode(TA_Base_Core::Control);
			}
			// ++yanrong
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error asking the agent to change operation mode: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

            setStatusToChangeTo(TA_Base_Core::NotRunning);
            raiseFailureAlarm(TA_Base_Core::CommsError);

            FUNCTION_EXIT;
            return;
        }

        // Ensure that the agent begins to change state
        int nLoops = m_responseTimeout / ProcessManager::SLEEP_TIME_MSECS;
        bool hasChangedState = false;
        for(int i =0; i < nLoops; i++)
        {
            sleep(ProcessManager::SLEEP_TIME_MSECS);
            if (getProcessStatus() == TA_Base_Core::RunningControl ||
                getProcessStatus() == TA_Base_Core::GoingToControl)
            {
                // Agent has reported status is changing
                hasChangedState = true;

                // Agent has reported status is changing so lets let the
                // peer know so it can start changing too
                ProcessManager::getInstance().notifyPeers(getEntityName());

                break;
            }
        }

        if (hasChangedState)
        {
            // Sleep until the timeout expires, or until a signal has been
            // received for the process.
            nLoops = m_changeControlTimeout / ProcessManager::SLEEP_TIME_MSECS;
            for (int i = 0; i < nLoops; i++)
            {
                sleep(ProcessManager::SLEEP_TIME_MSECS);

                if (isModeChangeComplete())
                {
					// Reset the flag
					m_isModeChangeComplete = false;

                    // Agent has successfully changed
                    ProcessManager::getInstance().notifyPeers(getEntityName());

                    FUNCTION_EXIT;
                    return;
                }

                if (getProcessStatus() == TA_Base_Core::Terminating ||
                    getProcessStatus() == TA_Base_Core::NotRunning ||
                    getStatusToChangeTo() == TA_Base_Core::NotRunning || // This means an error has occurred and the process has to be terminated
                    getStatusToChangeTo() == TA_Base_Core::Stopped)
                {
                    // The process has stopped itself so we can stop waiting for it
                    FUNCTION_EXIT;
                    return;
                }
            }
        }

        // The process has not changed successfully in the allocated time.
        // We should now indicate that the process should be killed
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The process %s did not complete changing to Control in time. It will be terminated", getEntityName().c_str());

		// Now raise the alarm and allow the process to be restarted.
		setStatusToChangeTo(TA_Base_Core::NotRunning);
        raiseFailureAlarm(TA_Base_Core::TimeoutError);

        // Process failed to change so we must know the peers know
        ProcessManager::getInstance().notifyPeers(getEntityName());

        FUNCTION_EXIT;
    }

    //
    // performChangeToMonitor
    //
    void ManagedProcessInfo::performChangeToMonitor()
    {
        FUNCTION_ENTRY("performChangeToMonitor");

        if (getProcessId() == 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Process %s asked to change into monitor mode but is not currently running", getEntityName().c_str());
            FUNCTION_EXIT;
            return;
        }

        // Ask the agent to change into Monitor mode
        try
        {
			// yanrong++: CL-21237
			TA_Base_Core::ThreadGuard guard(m_lock);
			if(!CORBA::is_nil(m_managedAgent))
            {
				m_managedAgent->setOperationMode(TA_Base_Core::Monitor);
			}
			// ++yanrong
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error asking the agent to change operation mode: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

            setStatusToChangeTo(TA_Base_Core::NotRunning);
            raiseFailureAlarm(TA_Base_Core::CommsError);

            FUNCTION_EXIT;
            return;
        }

        // Ensure that the agent begins to change state
        int nLoops = m_responseTimeout / ProcessManager::SLEEP_TIME_MSECS;
        bool hasChangedState = false;
        for(int i =0; i < nLoops; i++)
        {
            sleep(ProcessManager::SLEEP_TIME_MSECS);
            if (getProcessStatus() == TA_Base_Core::RunningMonitor ||
                getProcessStatus() == TA_Base_Core::GoingToMonitor )
            {
                // Agent has reported status is changing
                hasChangedState = true;

                // Agent has reported status is changing so lets let the
                // peer know so it can start changing too
                ProcessManager::getInstance().notifyPeers(getEntityName());

                break;
            }
        }

        if (hasChangedState)
        {
            // Sleep until the timeout expires, or until a signal has been
            // received for the process.
            nLoops = m_changeMonitorTimeout / ProcessManager::SLEEP_TIME_MSECS;
            for (int i = 0; i < nLoops; i++)
            {
                sleep(ProcessManager::SLEEP_TIME_MSECS);

                if (isModeChangeComplete())
                {
					// Reset the flag
					m_isModeChangeComplete = false;

                    // Agent has successfully changed
                    ProcessManager::getInstance().notifyPeers(getEntityName());

                    FUNCTION_EXIT;
                    return;
                }

                if (getProcessStatus() == TA_Base_Core::Terminating ||
                    getProcessStatus() == TA_Base_Core::NotRunning ||
                    getStatusToChangeTo() == TA_Base_Core::NotRunning || // This means an error has occurred and the process has to be terminated
                    getStatusToChangeTo() == TA_Base_Core::Stopped)
                {
                    // The process has stopped itself so we can stop waiting for it
                    FUNCTION_EXIT;
                    return;
                }
            }
        }

        // The process has not changed successfully in the allocated time.
        // We should now indicate that the process should be killed
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The process %s did not complete changing to Monitor in time. It will be terminated", getEntityName().c_str());

   	    // Now raise the alarm and allow the process to be restarted.
		setStatusToChangeTo(TA_Base_Core::NotRunning);
        raiseFailureAlarm(TA_Base_Core::TimeoutError);

        // Process failed to change so we must know the peers know
        ProcessManager::getInstance().notifyPeers(getEntityName());

        FUNCTION_EXIT;
    }

    //
    // performStop
    //
    void ManagedProcessInfo::performStop(TA_Base_Core::EProcessStatus statusToChangeTo)
    {
        FUNCTION_ENTRY("performStop");

        if (getProcessId() != 0)
        {
            if (statusToChangeTo != TA_Base_Core::NotRunning && statusToChangeTo != TA_Base_Core::Terminating)
            {
	            // First try to terminate the process nicely. This is only done if
				// the state to change to isn't NotRunning. NotRunning means an error
                // has occurred and we do not need to terminate nicely (as it probably
                // isn't going to work).
	            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Calling CORBA terminate()");
                terminateProcess();

                // Ensure that the agent begins to terminate
                int nLoops = m_responseTimeout / ProcessManager::SLEEP_TIME_MSECS;
                for(int i =0; i < nLoops; i++)
                {
                    sleep(ProcessManager::SLEEP_TIME_MSECS);
                    if (getProcessStatus() == TA_Base_Core::Terminating)
                    {
                        // Agent has reported status is changing so notify the peers
                        ProcessManager::getInstance().notifyPeers(getEntityName());
                        break;
                    }
                }
            }

            // If the process has successfully switched to terminating then give
            // it some time to shutdown
            if (getProcessStatus() == TA_Base_Core::Terminating)
            {
                // Sleep until the timeout expires, or until a signal has been
                // received for the process.
                int nLoops = m_terminateTimeout / ProcessManager::SLEEP_TIME_MSECS;
                for (int i = 0; i < nLoops; i++)
                {
                    sleep(ProcessManager::SLEEP_TIME_MSECS);
                    if (hasBeenSignalled())
                    {
                        setProcessId(0);
                        if (statusToChangeTo == TA_Base_Core::NotRunning)
                        {
                            setProcessStatus(TA_Base_Core::NotRunning);
                            setStatusToChangeTo(TA_Base_Core::Startup);
                        }
                        else
                        {
                            setProcessStatus(TA_Base_Core::Stopped);
                            setStatusToChangeTo(TA_Base_Core::Stopped);
                        }

                        ProcessManager::getInstance().notifyPeers(getEntityName());
                        FUNCTION_EXIT;
                        return;
                    }
                }
            }

			bool isSuccessful = false;

            if ( statusToChangeTo != TA_Base_Core::NotRunning )
            {
                // Process status still isn't stopped, so send a SIGTERM. Again
                // only do this if the state to change to isn't NotRunning.
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Sending SIGTERM to process %lu",
					getProcessId());

                #ifdef WIN32
                    isSuccessful = ProcessUtilities::terminateProcess(getProcessId());

                    if ( !isSuccessful )
                    {
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "call TerminateProcess failed, just sleep a while");
                        sleep(getKillTimeout());
                    }
                #else
                    /*isSuccessful = */ProcessUtilities::terminateProcess(getProcessId());

                    //if ( !isSuccessful )
                    //{
                        int nLoops = getKillTimeout() / ProcessManager::SLEEP_TIME_MSECS;
                        for ( int i = 0; i < nLoops; i++ )
                        {
                            sleep(ProcessManager::SLEEP_TIME_MSECS);

                            if ( hasBeenSignalled() )
                            {
                                isSuccessful = true;
                                break;
                            }
                        }
                    //}
                #endif
            }

            if (!isSuccessful)
            {
				// Finally, send a SIGKILL

				// There's no reason a SIGKILL should ever not work, so just a
				// normal constant is used to decide how long to wait.

                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "performStop Sending SIGKILL to process %lu",
                    getProcessId());
                ProcessUtilities::killProcess(getProcessId());

				// Sleep for SIGKILL_TIMEOUT milliseconds, or until a signal
				// is received. We don't really need to wait any amount of time,
				// but catching the signal before we reset its Process ID allows
				// the logs to at least contain which process the SIGKILL was for.
				int nLoops = SIGKILL_TIMEOUT / ProcessManager::SLEEP_TIME_MSECS;
				for (int i = 0; i < nLoops; i++)
				{
					sleep(ProcessManager::SLEEP_TIME_MSECS);

					if (hasBeenSignalled())
					{
						break;
					}
				}
            }

            // TD18783
            // setProcessId(0);
        }

        if(statusToChangeTo == TA_Base_Core::NotRunning)
        {
            setProcessStatus(TA_Base_Core::NotRunning);
            setStatusToChangeTo(TA_Base_Core::Startup);
        }
        else
        {
            setProcessStatus(TA_Base_Core::Stopped);
            setStatusToChangeTo(TA_Base_Core::Stopped);
        }

        ProcessManager::getInstance().notifyPeers(getEntityName());

        FUNCTION_EXIT;
    }

    //
    // terminateProcess
    //
    void ManagedProcessInfo::terminateProcess()
    {
        FUNCTION_ENTRY("terminateProcess");

        try
        {
            if (getProcessStatus() != TA_Base_Core::Unstarted &&
                getProcessStatus() != TA_Base_Core::NotRunning &&
                getProcessStatus() != TA_Base_Core::Stopped &&
                getProcessStatus() != TA_Base_Core::Terminating)
            {				
				// yanrong++: CL-21237
				TA_Base_Core::ThreadGuard guard(m_lock);
				if(!CORBA::is_nil(m_managedProcess))
				{
					m_managedProcess->terminate();
				}
				// ++yanrong
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError,
                    "Cannot terminate process as status is %d", getProcessStatus());
            }
        }
        catch (CORBA::SystemException& e)
        {
            std::string details = "Error calling terminateProcess: ";
            details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
            LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);
        }
    }

	//
	// startProcess
	//
    void ManagedProcessInfo::startProcess()
    {
        FUNCTION_ENTRY("startProcess");

        if (getProcessStatus() == TA_Base_Core::Stopped ||
            getProcessStatus() == TA_Base_Core::Unstarted )
        {
            setStatusToChangeTo(TA_Base_Core::Startup);
        }

        FUNCTION_EXIT;
    }

	//
	// stopProcess
	//
    void ManagedProcessInfo::stopProcess(TA_Base_Core::EManagedProcessTerminateCode reason)
    {
        FUNCTION_ENTRY("stopProcess");

		setTerminateStatus(reason);
        setStatusToChangeTo(TA_Base_Core::Stopped);

        FUNCTION_EXIT;
    }

    //
    // getDate
    //
    std::string ManagedProcessInfo::getDate() const
    {
        // Get the current time.
        time_t currentTime;
        struct tm timeStruct;
        time(&currentTime);
        ACE_OS::localtime_r(&currentTime, &timeStruct);

        // Convert it to a string.
        std::ostringstream timeStr;
        timeStr << timeStruct.tm_mday << ".";
        timeStr << (timeStruct.tm_mon + 1) << ".";
        timeStr << (timeStruct.tm_year + 1900);

        return timeStr.str();
    }

	void ManagedProcessInfo::performChangeToStandby()
	{
		FUNCTION_ENTRY("performChangeToStandby");

		if (getProcessId() == 0)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Process %s asked to change into standby mode but is not currently running", getEntityName().c_str());
			FUNCTION_EXIT;
			return;
		}

		// Ask the agent to change into standby mode
		try
		{
			// yanrong++: CL-21237
			TA_Base_Core::ThreadGuard guard(m_lock);
			if(!CORBA::is_nil(m_managedAgent))
			{
				m_managedAgent->setOperationMode(TA_Base_Core::Standby);
			}
			// ++yanrong
		}
		catch (CORBA::SystemException& e)
		{
			std::string details = "Error asking the agent to change operation mode: ";
			details += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
			LOG_EXCEPTION_CATCH( SourceInfo, "CORBA::SystemException", details);

			setStatusToChangeTo(TA_Base_Core::NotRunning);
			raiseFailureAlarm(TA_Base_Core::CommsError);

			FUNCTION_EXIT;
			return;
		}

		// Ensure that the agent begins to change state
		int nLoops = m_responseTimeout / ProcessManager::SLEEP_TIME_MSECS;
		bool hasChangedState = false;
		for(int i =0; i < nLoops; i++)
		{
			sleep(ProcessManager::SLEEP_TIME_MSECS);
			if (getProcessStatus() == TA_Base_Core::RunningStandby ||
				getProcessStatus() == TA_Base_Core::GoingToStandby )
			{
				// Agent has reported status is changing
				hasChangedState = true;
				break;
			}
		}

		if (hasChangedState)
		{
			// Sleep until the timeout expires, or until a signal has been
			// received for the process.
			nLoops = m_changeMonitorTimeout / ProcessManager::SLEEP_TIME_MSECS;
			for (int i = 0; i < nLoops; i++)
			{
				sleep(ProcessManager::SLEEP_TIME_MSECS);

				if (isModeChangeComplete())
				{
					// Reset the flag
					m_isModeChangeComplete = false;

                    // Agent has successfully changed
                    ProcessManager::getInstance().notifyPeers(getEntityName());

					FUNCTION_EXIT;
					return;
				}

				if (getProcessStatus() == TA_Base_Core::Terminating ||
					getProcessStatus() == TA_Base_Core::NotRunning ||
					getStatusToChangeTo() == TA_Base_Core::NotRunning || // This means an error has occurred and the process has to be terminated
					getStatusToChangeTo() == TA_Base_Core::Stopped)
				{
					// The process has stopped itself so we can stop waiting for it
					FUNCTION_EXIT;
					return;
				}
			}
		}

		// The process has not changed successfully in the allocated time.
		// We should now indicate that the process should be killed
		LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "The process %s did not complete changing to Standy in time. It will be terminated", getEntityName().c_str());

		// Now raise the alarm and allow the process to be restarted.
		setStatusToChangeTo(TA_Base_Core::NotRunning);
		raiseFailureAlarm(TA_Base_Core::TimeoutError);

		FUNCTION_EXIT;
	}

	//for station logic
	void ManagedProcessInfo::notifyStandbyProcess()
	{
		FUNCTION_ENTRY("notifyStandbyProcess");

		std::string myHostname = ProcessUtilities::getHostname();

		std::map<std::string, TA_Base_Core::ISystemControllerAdminCorbaDef_var> m_systemControllers = resolveStandbySystemControllerAdmin();

		//both occ systemController are down
		if(m_systemControllers.empty())
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Resolve Standby SystemController Failed");
			return;
		}
		for ( std::map<std::string, TA_Base_Core::ISystemControllerAdminCorbaDef_var>::iterator it = m_systemControllers.begin(); it != m_systemControllers.end(); ++it )
		{
			if ( !CORBA::is_nil( it->second ) )
			{
				try
				{
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "[notifyStandbyProcess] notify Standby Process on %s to change to standby mode.", it->first.c_str() );
					omniORB::setClientCallTimeout(it->second, 3000 );
					it->second->notifyStandbyAgent(getEntityName().c_str(), myHostname.c_str());
					//it->second->changeAgentOperationMode(getEntityName().c_str(), TA_Base_Core::Standby);
				}
				catch (CORBA::SystemException& e)
				{
					std::string desc = "System exception occurred while calling notifyStandbyAgent on occ systemcontroller";
					desc += it->first + " - ";
					desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
					LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", desc);
				}
				catch ( ... )
				{
					std::string desc = "Error occurred calling notifyStandbyAgent on occ systemcontroller ";
					desc += it->first;
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc);
				}

			}
		}

		FUNCTION_EXIT;
	}

	std::map<std::string, TA_Base_Core::ISystemControllerAdminCorbaDef_var> ManagedProcessInfo::resolveStandbySystemControllerAdmin()
	{
		FUNCTION_ENTRY("resolveStandbySystemControllerAdmin");

		std::map<std::string, TA_Base_Core::ISystemControllerAdminCorbaDef_var> m_systemControllers;

		int port = atoi(TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT).c_str());

		std::string myHostname = ProcessUtilities::getHostname();

		TA_Base_Core::ISystemControllerData* myData = TA_Base_Core::SystemControllerAccessFactory:: getInstance().getSystemControllerDataByName(myHostname);
		std::string myGroupName = myData->getGroupByName();

		std::vector<std::string> monitorsOfMyGroup = TA_Base_Core::GroupMonitorAccessFactory::
			getInstance().getSystemControllersMonitoringGroup(myGroupName);

		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "resolved systemControllers which monitoring group: %s", myGroupName.c_str());

		for ( std::vector<std::string>::iterator it = monitorsOfMyGroup.begin(); it != monitorsOfMyGroup.end(); ++it )
		{
			// Create a string in the form: "corbaloc::hostname:port/SystemControllerAdmin"
			std::stringstream systemControllerAdminIor;
			systemControllerAdminIor << "corbaloc::" << *it << ":" << port << "/SystemControllerAdmin";

			// Attempt to resolve an IShutdownCorbaDef object
			CORBA::Object_var obj = TA_Base_Core::CorbaUtil::getInstance().stringToObject( systemControllerAdminIor.str() );

			if ( !CORBA::is_nil(obj) )
			{
				try
				{
					omniORB::setClientCallTimeout(obj, 3000 );
					TA_Base_Core::ISystemControllerAdminCorbaDef_var controllerAdmin = TA_Base_Core::ISystemControllerAdminCorbaDef::_narrow( obj );
					m_systemControllers.insert( std::make_pair(*it, controllerAdmin) );
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "resolved a systemControllerAdmin: %s", systemControllerAdminIor.str().c_str());
				}
				catch (CORBA::SystemException& e)
				{
					std::string desc = "System exception occurred while resolving systemControllerAdmin ";
					desc += systemControllerAdminIor.str() + " - ";
					desc += TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e);
					LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException", desc);
				}
				catch ( ... )
				{
					std::string desc = "Error occurred narrowing ";
					desc += systemControllerAdminIor.str();
					LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", desc);
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot resolve systemControllerAdmin %s", systemControllerAdminIor.str().c_str());
			}
		}

		FUNCTION_EXIT;
		return m_systemControllers;
	}

    void ManagedProcessInfo::resetProcess()
    {
        ProcessInfo::resetProcess();
        m_isGroupStartup = false;
    }

} // namespace TA_Base_App
