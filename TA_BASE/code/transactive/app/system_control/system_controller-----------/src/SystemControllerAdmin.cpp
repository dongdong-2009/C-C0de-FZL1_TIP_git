/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_controller/src/SystemControllerAdmin.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Implementation of the ISystemControllerAdminCorbaDef
  * interface, used by the System Manager.
  *
  */

#include "SystemControllerAdmin.h"
#include "ProcessManager.h"
#include "SystemController.h"
#include "SystemControllerMonitor.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/ISystemControllerAdminCorbaDef.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

#include <vector>

namespace TA_Base_App
{
    //
    // Constructor
    //
    SystemControllerAdmin::SystemControllerAdmin(SystemController* systemController)
    {
		m_systemController = systemController;
        TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "SystemControllerAdmin");
        activateServant();
    }
    
    //
    // Destructor
    //
    SystemControllerAdmin::~SystemControllerAdmin()
    {
    }
    
    //
    // getProcessData
    //
    TA_Base_Core::ProcessDataSeq* SystemControllerAdmin::getProcessData()
    {
        // Get the process data from ProcessManager
        std::vector<TA_Base_Core::ProcessData> data =
            ProcessManager::getInstance().getAllProcesses();

        TA_Base_Core::ProcessDataSeq* dataSeq = new TA_Base_Core::ProcessDataSeq();
        if (dataSeq == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugFatal,
                    "New failed to allocate ProcessDataSeq");
            exit(1);
        }
        
        dataSeq->length(data.size());
        
        // Convert the data to a ProcessDataSeq
        for (unsigned int pos = 0; pos < data.size(); pos++)
        {
            (*dataSeq)[pos] = data[pos];
        }

        return dataSeq;
    }

    //
    // shutdown
    //
    void SystemControllerAdmin::shutdown(const char* hostname)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to shutdown");

        if (hostname == ProcessManager::getInstance().getHostName())
        {
            m_systemController->shutDownRequested();
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn,
                "Instructed by System Manager to shutdown a different System Controller (%s)!",
                hostname);
        }
    }

    //
    // startProcess
    //
    void SystemControllerAdmin::startProcess(const char* entityName)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to start %s", entityName);

        ProcessManager::getInstance().startProcess(entityName);
    }

    //
    // changeAgentOperationMode
    //
    void SystemControllerAdmin::changeAgentOperationMode(const char* entityName,
                                                TA_Base_Core::EOperationMode operationMode)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to change operation mode of %s to %d",
            entityName, operationMode);

        try
        {
            ProcessManager::getInstance().changeAgentOperationMode(entityName, operationMode);
        }
        catch(TA_Base_Core::SystemControllerException& sce)
        {
            // Operation mode cannot be changed. Notify the System Manager.
            LOG( SourceInfo, DebugUtil::ExceptionConstruction, "ISystemControllerAdminCorbaDef::ModeChangeException", "Operation mode of agent could be changed.");
            TA_Base_Core::ISystemControllerAdminCorbaDef::ModeChangeException mce;
            mce.reason = sce.what();
            throw mce; 
        }
    }

    //
    // stopProcess
    //
    void SystemControllerAdmin::stopProcess(const char* entityName)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to stop %s", entityName);

        ProcessManager::getInstance().stopProcess(entityName);
    }

    //
    // setRunParam
    //
    void SystemControllerAdmin::setRunParam(const char* entityName, const TA_Base_Core::RunParam& param)
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
            "Instructed by System Manager to set run param on %s", entityName);
        ProcessManager::getInstance().setRunParam(entityName, param);
    }

    //
	// setAgentsStatus
	//
	TA_Base_Core::AgentStatusEnum SystemControllerAdmin::getAgentsStatus()
	{
		return (ProcessManager::getInstance().getAgentStatusSummary());
	}

	void SystemControllerAdmin::notifyStandbyAgent( const char* entityName, const char* hostName )
	{
		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
			"[notifyStandbyAgent] entityName = %s , hostName = %s!", entityName, hostName);

		SystemControllerMonitor::getInstance().setGroupStarting(hostName);

		changeAgentOperationMode(entityName, TA_Base_Core::Standby);
	}
} // namespace TA_Base_App
