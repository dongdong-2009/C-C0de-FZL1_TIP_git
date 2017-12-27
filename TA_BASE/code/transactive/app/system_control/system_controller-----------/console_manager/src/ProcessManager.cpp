/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/console_manager/src/ProcessManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/02/06 16:15:14 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ProcessManager.cpp
//
// Demonstration implementation of the ProcessManagerDmIf interface
//
// Author:      Bruce Fountain 08-Feb-2001
// Modified by: $Author: haijun.li $
// Modified on: $DateTime: 2012/02/06 16:15:14 $
// Version:     $Revision: #2 $
//

#include "app/system_control/system_controller/console_manager/src/ProcessManager.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/process_management/IDL/src/IManagedAgentCorbaDef.h"
#include "core/process_management/IDL/src/IManagedGUICorbaDef.h"
#include <boost/foreach.hpp>

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaUtil;
using TA_Base_Core::ThreadGuard;

ProcessManager::ProcessManager()
{
    // Build up the set of runtime parameters that will be passed to registering
    // processes

    RunParams::getInstance().getAll(m_defaultParams);

    std::cout << "\nRunParams:\n";

    for (unsigned int i = 0; i < m_defaultParams.size(); i++)
    {
        RunParams::getInstance().registerRunParamUser(this, m_defaultParams[i].name.c_str());

        std::cout << "   " << m_defaultParams[i].name << "="
                  << m_defaultParams[i].value << "\n";
    }

    std::cout << "\n\n";

    // Give this object a user-friendly key, so that the managed process can
    // access it using a URL-style IOR
    // (eg "corbaloc::localhost:6666/ProcessManager")
    // See section 6.4.1 "corbaloc: URLs" in the ORBacus manual
    CorbaUtil::getInstance().setServantKey(this, "ProcessManager");

    activateServant();
}

// Methods to meet the ProcessManagerCorbaDef interface requirements.

TA_Base_Core::RunParamSeq* ProcessManager::getParams(TA_Base_Core::ProcessId id, const char* hostname)
{
    std::cout << "GetParams(id=" << id << ")" << std::endl;

    // An id of zero is allowable - we just the default parameters
    const RunParams::ParamVector* params = &m_defaultParams;

    ThreadGuard guard(m_mapLock);

    // Find this application Id so we can get the entity name
    ClientMap::iterator iter = m_clients.find(id);

    // Does not exist so don't return it any parameters
    if ((iter == m_clients.end()) && (id != 0))
    {
        return NULL;
    }

    TA_Base_Core::RunParamSeq* result = new TA_Base_Core::RunParamSeq;

    for (unsigned int i = 0; i < params->size(); i++)
    {
        result->length(i + 1);

        // If this is the debug file path parameter then we want to change this to a DebugFile parameter
        if (0 == (*params)[i].name.compare("DebugFilePath"))
        {
            if (!RunParams::getInstance().isSet(RPARAM_DEBUGFILE))
            {
                char idStr[20];
                sprintf(idStr, "%lu", id);
                std::string debugFile = (*params)[i].value;
                debugFile += "log";
                debugFile += iter->second.entity;
                debugFile += "_";
                debugFile += idStr;
                debugFile += ".log";
                (*result)[i].name = CORBA::string_dup("DebugFile");
                (*result)[i].value = CORBA::string_dup(debugFile.c_str());
            }
        }
        else
        {
            (*result)[i].name = CORBA::string_dup((*params)[i].name.c_str());
            (*result)[i].value = CORBA::string_dup((*params)[i].value.c_str());
        }
    }

    return result;
}

void ProcessManager::terminating(TA_Base_Core::ProcessId p_processID,
                                 TA_Base_Core::EManagedProcessTerminateCode p_code)
{
    std::cout << "Terminating(id=" << p_processID << ", reason=" << StatusCodeToStr(p_code) << ")" << std::endl;

    ThreadGuard guard(m_mapLock);

    ClientMap::iterator it = m_clients.find(p_processID);

    if (it == m_clients.end())
    {
        std::cout << "No client matching id " << p_processID << std::endl;
        return;
    }

    m_clients.erase(it);
}

void ProcessManager::registerManagedProcess(TA_Base_Core::IManagedProcessCorbaDef_ptr p_managedProcess,
        TA_Base_Core::ProcessId p_processID,
        CORBA::ULong applicationType,
        const char* entity)
{
    std::cout << "Register(apptype=" << applicationType << ", entity=" << entity << ")";

    if (CORBA::is_nil(p_managedProcess))
    {
        std::cout << "Received registration from a nil managed process" << std::endl;
        return;
    }

    ThreadGuard guard(m_mapLock);
    m_clients[p_processID].managedProcess   = TA_Base_Core::IManagedProcessCorbaDef::_duplicate(p_managedProcess);
    m_clients[p_processID].entity           = entity;
    m_clients[p_processID].applicationType  = applicationType;
    m_clients[p_processID].type = ""; // Type is currently unknown. This is filled out if hte app calls registerManagedApplication
}

void ProcessManager::registerManagedApplication(TA_Base_Core::IManagedApplicationCorbaDef_ptr p_managedApp,
        TA_Base_Core::ProcessId p_processId)
{
    std::cout << "Register Managed App(id=" << p_processId << ")" << std::endl;

    if (CORBA::is_nil(p_managedApp))
    {
        std::cout << "Received registration from a nil managed application" << std::endl;
        return;
    }

    ThreadGuard guard(m_mapLock);
    m_apps[p_processId] = TA_Base_Core::IManagedApplicationCorbaDef::_duplicate(p_managedApp);

    ClientMap::iterator it = m_clients.find(p_processId);

    if (it == m_clients.end())
    {
        std::cout << "No client matching id " << p_processId << std::endl;
        return;
    }

    try
    {
        TA_Base_Core::IManagedAgentCorbaDef_var agent = TA_Base_Core::IManagedAgentCorbaDef::_narrow(p_managedApp);

        if (!CORBA::is_nil(agent))
        {
            it->second.type = "Agent";
        }
        else
        {
            TA_Base_Core::IManagedGUICorbaDef_var managedProcess = TA_Base_Core::IManagedGUICorbaDef::_narrow(p_managedApp);

            if (!CORBA::is_nil(managedProcess))
            {
                it->second.type = "GUI";
            }
        }
    }
    catch (...)
    {
        return;
    }
}

void ProcessManager::poll()
{
}

void ProcessManager::statusChanged(TA_Base_Core::EProcessStatus status, TA_Base_Core::ProcessId p_processId)
{
    ClientMap::iterator iter = m_clients.find(p_processId);

    if (iter == m_clients.end())
    {
        std::cout << "Status Change Failed. Could not find id\n";
        return;
    }

    std::cout << "Status update received for " << iter->second.entity << ". Changed to state ";

    switch (status)
    {
    case (TA_Base_Core::Startup):
        std::cout << "Startup";
        break;

    case (TA_Base_Core::GoingToControl):
        std::cout << "Going to Control";
        break;

    case (TA_Base_Core::GoingToMonitor):
        std::cout << "Going to Monitor";
        break;

    case (TA_Base_Core::RunningMonitor):
        std::cout << "Running Monitor";
        break;

    case (TA_Base_Core::RunningControl):
        std::cout << "Running Control";
        break;

    case (TA_Base_Core::Terminating):
        std::cout << "Terminating";
        break;

    default:
        std::cout << "Unknown";
        break;
    }

    std::cout << "\n";
}

void ProcessManager::onRunParamChange(const std::string& paramName, const std::string& paramValue)
{
    if (!m_clients.empty())
    {
        // Reset default parameters for future clients.
        m_defaultParams.clear();
        RunParams::getInstance().getAll(m_defaultParams);

        // Notify existing clients.
        for (ClientMap::const_iterator iter = m_clients.begin(); iter != m_clients.end(); ++iter)
        {
            setClientRunParam(iter->first, paramName, paramValue);
        }

        std::cout << "   " << paramName << "=" << paramValue << std::endl;
    }
}

std::string ProcessManager::getRunningApps()
{
    ThreadGuard guard(m_mapLock);

    std::string listOfApps("----------------------------------------------------\n");
    listOfApps += "Id\tAppType\tType\tEntity\n";
    listOfApps += "----------------------------------------------------\n";

    ClientMap::iterator iter(m_clients.begin());
    ClientMap::iterator iterEnd(m_clients.end());

    while (iter != iterEnd)
    {
        char appType[10];
        char id[10];
        sprintf(appType, "%lu", iter->second.applicationType);
        sprintf(id, "%lu", iter->first);
        listOfApps += id;
        listOfApps += "\t";
        listOfApps += appType;
        listOfApps += "\t";
        listOfApps += iter->second.type;
        listOfApps += "\t";
        listOfApps += iter->second.entity;
        listOfApps += "\n";

        ++iter;
    }

    listOfApps += "----------------------------------------------------\n";

    return listOfApps;
}

void ProcessManager::terminateApp(unsigned long id)
{
    std::cout << "Attempting to terminate " << id;
    ThreadGuard guard(m_mapLock);

    ClientMap::iterator iter = m_clients.find(id);

    if (iter == m_clients.end())
    {
        std::cout << " - Failed. Could not find id\n";
        return;
    }

    try
    {
        if (iter->second.managedProcess != NULL)
        {
            TA_Base_Core::IManagedProcessCorbaDef_ptr app = TA_Base_Core::IManagedProcessCorbaDef::_narrow(iter->second.managedProcess);

            if (CORBA::is_nil(app))
            {
                std::cout << " - Failed. Could not narrow\n";
                return;
            }

            app->terminate();
            std::cout << " - Success\n";
        }
    }
    catch (...)
    {
        std::cout << " - Failed. Exception thrown\n";
    }
}

void ProcessManager::changeMode(bool isControl, unsigned long id)
{
    if (isControl)
    {
        std::cout << "Attempting to change to control mode ";
    }
    else
    {
        std::cout << "Attempting to change to monitor mode ";
    }

    ThreadGuard guard(m_mapLock);

    AppMap::iterator iter = m_apps.find(id);

    if (iter == m_apps.end())
    {
        std::cout << " - Failed. Could not find id\n";
        return;
    }

    try
    {
        if (iter->second != NULL)
        {
            TA_Base_Core::IManagedAgentCorbaDef_ptr app = TA_Base_Core::IManagedAgentCorbaDef::_narrow(iter->second);

            if (CORBA::is_nil(app))
            {
                std::cout << " - Failed. App is not an agent.\n";
                return;
            }

            if (isControl)
            {
                app->setOperationMode(TA_Base_Core::Control);
            }
            else
            {
                app->setOperationMode(TA_Base_Core::Monitor);
            }

            std::cout << " - Success\n";
        }
    }
    catch (...)
    {
        std::cout << " - Failed. Exception thrown\n";
    }
}

void ProcessManager::changeGUI(TA_Base_Core::EFocusType focus, unsigned long id)
{
    std::cout << "Attempting to change to focus type ";

    ThreadGuard guard(m_mapLock);

    AppMap::iterator iter = m_apps.find(id);

    if (iter == m_apps.end())
    {
        std::cout << " - Failed. Could not find id\n";
        return;
    }

    try
    {
        if (iter->second != NULL)
        {
            TA_Base_Core::IManagedGUICorbaDef_ptr app = TA_Base_Core::IManagedGUICorbaDef::_narrow(iter->second);

            if (CORBA::is_nil(app))
            {
                std::cout << " - Failed. App is not a GUI.\n";
                return;
            }

            app->changeFocus(focus);
            std::cout << " - Success\n";
        }
    }
    catch (...)
    {
        std::cout << " - Failed. Exception thrown\n";
    }
}

void ProcessManager::serverChange(bool isUp, unsigned long id)
{
    std::cout << "Attempting to alert GUI of server change ";

    ThreadGuard guard(m_mapLock);

    AppMap::iterator iter = m_apps.find(id);

    if (iter == m_apps.end())
    {
        std::cout << " - Failed. Could not find id\n";
        return;
    }

    try
    {
        if (iter->second != NULL)
        {
            TA_Base_Core::IManagedGUICorbaDef_ptr app = TA_Base_Core::IManagedGUICorbaDef::_narrow(iter->second);

            if (CORBA::is_nil(app))
            {
                std::cout << " - Failed. App is not a GUI.\n";
                return;
            }

            app->serverStateChange(isUp);
            std::cout << " - Success\n";
        }
    }
    catch (...)
    {
        std::cout << " - Failed. Exception thrown\n";
    }
}

const char* ProcessManager::StatusCodeToStr(TA_Base_Core::EManagedProcessTerminateCode p_code)
{
    switch (p_code)
    {
    case (TA_Base_Core::RequestedTerminate):
        return "RequestedTerminate";

    case (TA_Base_Core::InitError):
        return "InitError";

    case (TA_Base_Core::CommsError):
        return "CommsError";

    case (TA_Base_Core::UserExit):
        return "UserExit";

    case (TA_Base_Core::DependencyFailure):
        return "DependencyFailure";

    case (TA_Base_Core::TimeoutError):
        return "TimeoutError";

    case (TA_Base_Core::FatalError):
        return "FatalError";

    case (TA_Base_Core::AccessDenied):
        return "AccessDenied";

    case (TA_Base_Core::NoActiveSession):
        return "NoActiveSession";

    case (TA_Base_Core::RightsError):
        return "RightsError";

    case (TA_Base_Core::ConfigError):
        return "ConfigError";

    case (TA_Base_Core::NotKnown):
        return "NotKnown";
    }

    return "???Unrecognised code???";
}

bool ProcessManager::setClientRunParam(const int id, const std::string& paramName, const std::string& paramValue)
{
    std::cout << "Attempting to set run-param for specified process " << id;
    ThreadGuard guard(m_mapLock);

    ClientMap::iterator iter = m_clients.find(id);

    if (iter == m_clients.end())
    {
        std::cout << " - Failed. Could not find specified managed process id:" << id << std::endl;
        return false;
    }

    try
    {
        if (iter->second.managedProcess != NULL)
        {
            omniORB::setClientCallTimeout(iter->second.managedProcess, 20);

            TA_Base_Core::IManagedProcessCorbaDef_var app = TA_Base_Core::IManagedProcessCorbaDef::_narrow(iter->second.managedProcess);

            if (CORBA::is_nil(app))
            {
                std::cout << " - Failed. Could not narrow\n";
                return false;
            }

            omniORB::setClientCallTimeout(app, 20);
            TA_Base_Core::RunParam param;
            param.name = paramName.c_str();
            param.value = paramValue.c_str();
            app->setParam(param);
            std::cout << " - Param has been set\n";
            return true;
        }
    }
    catch (...)
    {
        std::cout << " - Failed. Exception thrown\n";
    }

    return false;
}
