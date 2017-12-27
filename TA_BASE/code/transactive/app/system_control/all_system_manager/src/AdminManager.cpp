/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/AdminManager.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * AdminManager is a singleton class that is the central class of the 
  * System Manager. It retreives data from a System Controller and
  * sends the GUI's orders to that System Controller.
  */

#pragma warning(disable:4786)

#include "stdafx.h"
#include <winsock2.h> 

#include "SystemManager.h"
#include "AdminManager.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IProcess.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"

#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/process_management/IDL/src/IHeartbeatReceiverCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/SystemControllerException.h"
#include "core/exceptions/src/DataException.h"
#include "core/threads/src/Thread.h"
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"

// Forward declarations
using TA_Base_Core::SystemControllerException;

// Define statics.

namespace TA_Base_App
{
    //TD8617 Mintao++
    const unsigned int AdminManager::DEFAULT_REFRESH_RATE = 0; 
    const int MAX_LOGIN_RETRY_TIMES = 3;
    
    bool AdminManager::m_bStaticMemberInitialized = false;
    std::list<AdminObserver*> AdminManager::m_observers;
    std::vector<int> AdminManager::m_refreshIntervals;
    //TD8617 Mintao++
    //default refresh rate is 1 sec
    unsigned int AdminManager::m_currentRefreshRate = AdminManager::DEFAULT_REFRESH_RATE;
    std::string AdminManager::m_sessionId = "";
    std::string AdminManager::m_operatorName = "";
    std::string AdminManager::m_strPortNumber = "";
    TA_Base_Core::NonReEntrantThreadLockable AdminManager::m_observerLock;
    TA_Base_Bus::RightsLibrary* AdminManager::m_rightsLibrary = NULL;
    CWnd* AdminManager::m_mainWnd = NULL;
    
    void AdminManager::InitializeStaticMember( CWnd* pMainWnd )
    {
        if ( !m_bStaticMemberInitialized )
        {
            m_bStaticMemberInitialized = true;
            m_refreshIntervals.clear();
            m_refreshIntervals.push_back( 1 );
            m_refreshIntervals.push_back( 5 );
            m_refreshIntervals.push_back( 10 );
            m_refreshIntervals.push_back( 20 );
            m_refreshIntervals.push_back( 60 );

            m_mainWnd = pMainWnd;
        }
    }
    
    AdminManager::AdminManager()
        : m_hostName(""), 
        m_isRunning(false),
        m_errorOccurred(false),
        m_nThreshhold( 0 ),
        m_nLoginResult( ERR_LOGIN_NO_PROCESS )
    {
    }

    AdminManager::AdminManager( const std::string& strHostName, unsigned long ulEntityKey ) : 
    m_hostName( strHostName ), 
    m_isRunning(false),
    m_errorOccurred(false),
    m_nThreshhold( 0 ),
    m_nLoginResult( ERR_LOGIN_NO_PROCESS ),
    m_isStartProcessAllowed( false ),
    m_isStopProcessAllowed( false ),
    m_isSetProcessParamAllowed( false ),
    m_isShutdownAllowed( false ),
    m_isSetOperatingModeAllowed( false ),
    m_isSetOptionsAllowed( false ),
    m_resourceKey( 0 ),
    m_hostEntityKey( ulEntityKey ),
    m_loginSemaphore( 0 )
    {
    }
    
    AdminManager::~AdminManager()
    {
        logout();
    }
    
    void AdminManager::registerObserver(const AdminObserver* observer)
    {
        if (observer != NULL)
        {
            {
                TA_Base_Core::ThreadGuard guard(m_observerLock);
                m_observers.push_back((AdminObserver*)observer);
            }
        }
    }
    
    
    void AdminManager::unregisterObserver(const AdminObserver* observer)
    {
        TA_Base_Core::ThreadGuard guard(m_observerLock);
        
        if (observer == NULL)
        {
            return;
        }
        
        std::list<AdminObserver*>::iterator it;
        for (it = m_observers.begin(); it != m_observers.end(); it++)
        {
            if (observer == *it)
            {
                m_observers.erase(it);
                break;
            }
        }
    }
    
    void AdminManager::checkPermissions()
    {
        FUNCTION_ENTRY("checkPermissions");
        
        bool errorOccurred = false;
        
        if ( NULL == m_rightsLibrary )
        {
            errorOccurred = true;
        }
        
        if ( !errorOccurred && 0 == m_resourceKey )
        {
            // Get the resource key from the entity key
            try
            {
                TA_Base_Core::IResource* resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_hostEntityKey);
                if ( NULL != resource )
                {
                    m_resourceKey = resource->getKey();
                    delete resource;
                    resource = NULL;
                }
                else
                {
                    errorOccurred = true;
                }
            }
            catch(TA_Base_Core::DataException&)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", 
                    "Errors occurred while trying to retrieve a resource from the database.");
                errorOccurred = true;
            }
            catch(TA_Base_Core::DatabaseException&)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", 
                    "Errors occurred while trying to retrieve the resource from the database.");
                errorOccurred = true;
            }
        }

        // Now check the permissions for each rights-controlled function.
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;
        if (!errorOccurred)
        {
            try
            {
                m_isStartProcessAllowed = m_rightsLibrary->isActionPermittedOnResource(m_sessionId, m_resourceKey, TA_Base_Bus::aca_START_PROCESS, reason, decisionModule);
                m_isStopProcessAllowed = m_rightsLibrary->isActionPermittedOnResource(m_sessionId, m_resourceKey, TA_Base_Bus::aca_STOP_PROCESS, reason, decisionModule);
                m_isSetProcessParamAllowed = m_rightsLibrary->isActionPermittedOnResource(m_sessionId, m_resourceKey, TA_Base_Bus::aca_SET_PROCESS_RUNTIME_PARAMETER, reason, decisionModule);
                m_isShutdownAllowed = m_rightsLibrary->isActionPermittedOnResource(m_sessionId, m_resourceKey, TA_Base_Bus::aca_SHUTDOWN_SYSTEM_CONTROLLER, reason, decisionModule);
                m_isSetOperatingModeAllowed = m_rightsLibrary->isActionPermittedOnResource(m_sessionId, m_resourceKey, TA_Base_Bus::aca_SET_PROCESS_OPERATING_MODE, reason, decisionModule);
                m_isSetOptionsAllowed = m_rightsLibrary->isActionPermittedOnResource(m_sessionId, m_resourceKey, TA_Base_Bus::aca_SYSTEM_MANAGER_OPTIONS, reason, decisionModule);
            }
            catch(TA_Base_Core::RightsException&)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException", 
                    "Errors occurred while trying to determine which actions are permitted.");
                errorOccurred = true;
            }
            catch(...)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown", 
                    "Errors occurred while trying to determine which actions are permitted.");
                errorOccurred = true;
            }
        }
        
        // If something has gone wrong, let the user know and disable everything.
        if (errorOccurred)
        {
            m_isStartProcessAllowed = true;
            m_isStopProcessAllowed = true;
            m_isSetProcessParamAllowed = true;
            m_isShutdownAllowed = true;
            m_isSetOperatingModeAllowed = true;
            m_isSetOptionsAllowed = true;
        }
        
        FUNCTION_EXIT;
    }
    
    int AdminManager::login()
    {
        // Get the local machine
        const int nameLength = 512;
        char localHostName[nameLength];
        if ((gethostname(localHostName, nameLength) != 0))
        {
            return ERR_AUTH_SOCKET_ERROR;
        }
        
        CORBA::Object_var tmpVar;
        TA_Base_Core::IHeartbeatReceiverCorbaDef_var heartbeatReceiverVar;
        TA_Base_Core::IProcessManagerCorbaDef_var processManagerVar;
        
        // First, attempt to resolve the two basic System Controller objects
        std::string connectionString;
        try
        {
            connectionString = "corbaloc::";
            connectionString += m_hostName + ":";
            connectionString += m_strPortNumber + "/SystemControllerMonitor";
            tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
            heartbeatReceiverVar = TA_Base_Core::IHeartbeatReceiverCorbaDef::_narrow(tmpVar);
            
            connectionString = "corbaloc::";
            connectionString += m_hostName + ":";
            connectionString += m_strPortNumber + "/ProcessManager";
            tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
            processManagerVar = TA_Base_Core::IProcessManagerCorbaDef::_narrow(tmpVar);
        }
        catch(...)  // Doesn't seem to be one on the host specified
        {
            return ERR_NO_SYSTEM_CONTROLLER_ERROR;
        }
        
        connectionString = "corbaloc::";
        connectionString += m_hostName + ":";
        connectionString += m_strPortNumber + "/SystemControllerAdmin";
        tmpVar = TA_Base_Core::CorbaUtil::getInstance().stringToObject(connectionString.c_str());
        m_systemController = TA_Base_Core::ISystemControllerAdminCorbaDef::_narrow(tmpVar);
        
        if (CORBA::is_nil(m_systemController))
        {
            return ERR_GET_ADMIN_ERROR;
        }

        m_entityNameToDetails.clear();
        m_entityDescriptionToDetails.clear();

        return ERR_AUTH_NO_ERROR;
    }
    
    
    int AdminManager::loginSetup( const std::string& portNumber,
        const std::string& sessionId,
        const std::string& operatorName)
    {
        if (atoi(portNumber.c_str()) < 1024)
        {
            return ERR_AUTH_SERVICE_ERROR;
        }
        
        m_strPortNumber = portNumber;
        m_sessionId = sessionId;
        m_operatorName = operatorName;
        
        return ERR_AUTH_NO_ERROR;
    }
    
    
    void AdminManager::logout()
    {
        terminateAndWait();
        
        TA_Base_Core::ThreadGuard guard(m_processLock);
        m_processes.clear();
    }

    void AdminManager::run()
    {
        m_isRunning = true;

        m_nLoginResult = ERR_LOGIN_NO_PROCESS;
        int nTryTimes = 0;
        int nResult = ERR_LOGIN_NO_PROCESS;
        while (( ERR_AUTH_NO_ERROR != nResult ) && m_isRunning )
        {
            try
            {
                nResult = login();
                ++nTryTimes;
                if ( m_isRunning && ( ERR_AUTH_NO_ERROR != nResult ) && ( MAX_LOGIN_RETRY_TIMES <= nTryTimes ))
                {
                    m_nLoginResult = nResult;
                    nTryTimes = MAX_LOGIN_RETRY_TIMES;
                    if ( NULL != m_mainWnd )
                    {
                        m_mainWnd->PostMessage( WM_LOGIN_DONE, reinterpret_cast<WPARAM>(this), NULL );
                    }
                    m_loginSemaphore.wait();
                    continue;
                }
                sleep( 100 * nTryTimes );
            }
            catch (...)
            {
            }
        }

        m_nLoginResult = ERR_AUTH_NO_ERROR;
        if (( NULL != m_mainWnd ) && m_isRunning )
        {
            m_mainWnd->PostMessage( WM_LOGIN_DONE, reinterpret_cast<WPARAM>(this), NULL );
        }

        if ( m_isRunning )
        {
            // exception catch
            checkPermissions();
        }
        
        const int SLEEP_TIME_MSECS = 500;

        int periodMsecs = m_refreshIntervals[m_currentRefreshRate] * 1000;
        int nIterations = periodMsecs / SLEEP_TIME_MSECS;
        int currIteration;

        while (m_isRunning)
        {
            try
            {
                // Fetch process data.
                refreshData();
                m_errorOccurred = false;
            }
            catch(...)
            {
                // Couldn't retrieve process data. Notify observers, so
                // they get an exception when they call getProcesses().
                m_errorOccurred = true;
                notifyObservers();
            }
            
            // Sleep for the required amount of time.
            currIteration = 0;
            while (m_isRunning && currIteration < nIterations)
            {
                sleep(SLEEP_TIME_MSECS);
                currIteration++;
            }
        }
    }
    
    
    void AdminManager::terminate()
    {
        m_isRunning = false;
        m_loginSemaphore.post();
    }
    
    
    void AdminManager::refreshData()
    {
        ProcessDataSeq_var pProcessSeq;
        
        try
        {
            // Get new process data.
            pProcessSeq = m_systemController->getProcessData();
            m_nThreshhold = 0;
        }
        catch(const CORBA::SystemException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "CORBA::SystemException",
                TA_Base_Core::CorbaUtil::getInstance().exceptionToString(ex).c_str());
            m_nThreshhold += 1;
            // Pass the exception on as a SystemControllerException.
            if(m_nThreshhold >= 3)
            {
                m_nLoginResult = ERR_RETRIEVING_PROCESS_DATA;
                TA_THROW(SystemControllerException("Error calling getProcessData"));
            }
            return;
        }
        catch(...)
        {
            // Pass the exception on as a SystemControllerException.
            m_nLoginResult = ERR_RETRIEVING_PROCESS_DATA;
            TA_THROW(SystemControllerException("Error calling getProcessData"));
        }

        m_nLoginResult = ERR_AUTH_NO_ERROR;

        TA_Base_Core::ThreadGuard guard(m_processLock);
        
        if (pProcessSeq->length() != m_processes.size())
        {
            // Size of sequence doesn't match vector. Repopulate vector.
            buildData(pProcessSeq);
            notifyObservers();
        }
        else if ( !m_errorOccurred )
        {
            for (int i = 0; i < pProcessSeq->length(); i++)
            {
                if (!compareProcesses(pProcessSeq[i], m_processes[i]))
                {
                    // Process vector out of date. Update it.
                    buildData(pProcessSeq);
                    notifyObservers();
                    return;
                }
            }
        }
        else if ( m_errorOccurred )
        {
            m_errorOccurred = true;
            notifyObservers();
        }
    }
    
    
    std::vector<int> AdminManager::getRefreshIntervals()
    {
        return m_refreshIntervals;
    }
    
    unsigned int AdminManager::getCurrentRefreshRate()
    {
        return m_currentRefreshRate;
    }
    
    
    void AdminManager::setRefreshRate(unsigned int newRate)
    {
        if (newRate < m_refreshIntervals.size() && 
            newRate != m_currentRefreshRate)
        {
            m_currentRefreshRate = newRate;
        }
    }
    
    
    void AdminManager::startProcess(const std::string& processId)
    {
        FUNCTION_ENTRY("startProcess");
        
        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;
            
            m_systemController->startProcess(entityName.c_str());
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling startProcess"));
        }
        
        FUNCTION_EXIT;
    }
    
    
    void AdminManager::changeProcessOperationMode(const std::string& processId,
        TA_Base_Core::EOperationMode opMode)
    {
        FUNCTION_ENTRY("changeProcessOperationMode");
        
        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;
            
            m_systemController->changeAgentOperationMode(entityName.c_str(), opMode);
        }
        catch(TA_Base_Core::ISystemControllerAdminCorbaDef::ModeChangeException& ex)
        {
            
            TA_THROW(SystemControllerException(ex.reason.in()));
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Unknown error."));
        }
        
        FUNCTION_EXIT;
    }
    
    
    void AdminManager::stopProcess(const std::string& processId)
    {
        FUNCTION_ENTRY("stopProcess");
        
        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;
            
            m_systemController->stopProcess(entityName.c_str());
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling stopProcess"));
        }
        
        FUNCTION_EXIT;
    }
    
    
    void AdminManager::shutdownSystemController()
    {
        FUNCTION_ENTRY("shutdownSystemController");
        
        try
        {
            m_systemController->shutdown(m_hostName.c_str());
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling shutdown"));
        }
        
        FUNCTION_EXIT;
    }
    
    
    std::vector<ProcessExactData> AdminManager::getProcesses()
    {
        if (m_errorOccurred)
        {
            TA_THROW(SystemControllerException("Error calling getProcessData"));
        }

        TA_Base_Core::ThreadGuard guard(m_processLock);
        std::vector<ProcessExactData> vecProcessDatas;
        for ( std::vector<TA_Base_Core::ProcessData>::iterator itLoop = m_processes.begin();
              m_processes.end() != itLoop; ++itLoop )
        {
            ProcessExactData stProcessData;
            stProcessData.pOrgData = &(*itLoop);
            stProcessData.strProcessName = getEntityDescription( itLoop->ProcessId.in() );
            stProcessData.bCanBeManagered = isManagedProcessFromName( itLoop->ProcessId.in() );
            vecProcessDatas.push_back( stProcessData );
        }
        return vecProcessDatas;
    }
    
    std::string AdminManager::getEntityDescription(const std::string& entityName) const
    {
        ProcessInformationMap::const_iterator iter = m_entityNameToDetails.find(entityName);
        
        if (iter != m_entityNameToDetails.end())
        {
            return iter->second.entityDescription;
        }
        else
        {
            // Entity name not found. Just return "".
            return "";
        }
    }
    
    
    bool AdminManager::isManagedProcessFromName(const std::string& entityName) const
    {
        ProcessInformationMap::const_iterator iter = m_entityNameToDetails.find(entityName);
        
        if (iter != m_entityNameToDetails.end())
        {
            return iter->second.isManagedProcess;
        }
        
        // Entity name not found. Just return false to be safe
        return false;
    }
    
    
    bool AdminManager::isManagedProcessFromDescription(const std::string& entityDescription) const
    {
        ProcessInformationMap::const_iterator iter = m_entityDescriptionToDetails.find(entityDescription);
        
        if (iter != m_entityDescriptionToDetails.end())
        {
            return iter->second.isManagedProcess;
        }
        
        // Entity description not found. Just return false to be safe
        return false;
    }
    
    
    bool AdminManager::isServiceProcessFromName(const std::string& entityName) const
    {
        ProcessInformationMap::const_iterator iter = m_entityNameToDetails.find(entityName);
        
        if (iter != m_entityNameToDetails.end())
        {
            return iter->second.isServiceProcess;
        }
        
        // Entity name not found. Just return false to be safe
        return false;
    }
    
    
    bool AdminManager::isServiceProcessFromDescription(const std::string& entityDescription) const
    {
        ProcessInformationMap::const_iterator iter = m_entityDescriptionToDetails.find(entityDescription);
        
        if (iter != m_entityDescriptionToDetails.end())
        {
            return iter->second.isServiceProcess;
        }
        
        // Entity description not found. Just return false to be safe
        return false;
    }
    bool AdminManager::isAutoFailBackFromDescription(const std::string& entityDescription) const
    {
        ProcessInformationMap::const_iterator iter = m_entityDescriptionToDetails.find(entityDescription);
        
        if (iter != m_entityDescriptionToDetails.end())
        {
            return iter->second.isAutoFailBack;
        }
        
        // Entity description not found. Just return false to be safe
        return false;
    }
    
    void AdminManager::buildData(ProcessDataSeq_var processes)
    {
        m_processes.clear();
        std::string entityName;
        
        for (int i = 0; i < processes->length(); i++)
        {
            entityName = processes[i].ProcessId;
            
            ProcessInformationMap::iterator iter = m_entityNameToDetails.find(entityName);
            if (iter == m_entityNameToDetails.end())
            {
                // No details for this entity name. Try to fetch it from
                // the database.
                TA_Base_Core::IProcess* entityData = NULL;
                ProcessInformation info;
                
                try
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                        "Fetching entity details for %s from database",
                        entityName.c_str());
                    
                    entityData = dynamic_cast<TA_Base_Core::IProcess*>(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName));
                    
                    if (entityData != NULL)
                    {
                        info.entityName = entityName;
                        info.entityDescription = entityData->getDescription();
                        info.isManagedProcess = entityData->isManagedProcess();
                        info.isServiceProcess = entityData->isServiceProcess();
                        info.isAutoFailBack = entityData->shouldAutoFailback();
                    }
                    else
                    {
                        // Error getting the details. Just use the default instead.
                        info.entityName = entityName;
                        info.entityDescription = entityName;
                        info.isManagedProcess = false;
                        info.isServiceProcess = false;
                        info.isAutoFailBack = false;
                    }
                }
                catch(...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,
                        "Error retrieving entity details for entity %s",
                        entityName.c_str());
                    
                    // Error getting the details. Just use the default instead.
                    info.entityName = entityName;
                    info.entityDescription = entityName;
                    info.isManagedProcess = false;
                    info.isServiceProcess = false;
                }
                
                m_entityNameToDetails[entityName] = info;
                m_entityDescriptionToDetails[info.entityDescription] = info;
                
                if (entityData != NULL)
                {
                    delete entityData;
                    entityData = NULL;
                }
            }
            
            m_processes.push_back(processes[i]);
            
        }
    }
    
    void AdminManager::notifyObservers()
    {
        TA_Base_Core::ThreadGuard guard(m_observerLock);
        
        // Notify the registered observers
        std::list<AdminObserver*>::iterator it;
        for (it = m_observers.begin(); it != m_observers.end(); it++)
        {
            (*it)->onNewData( this );
        }
    }
    
    
    void AdminManager::setProcessRunParam(const std::string& processId, const TA_Base_Core::RunParam& param)
    {
        FUNCTION_ENTRY("setProcessRunParam");
        
        try
        {
            // Convert the ProcessId (entity description) into an entity name.
            std::string entityName = m_entityDescriptionToDetails[processId].entityName;
            
            m_systemController->setRunParam(entityName.c_str(), param);
        }
        catch(...)
        {
            TA_THROW(SystemControllerException("Error calling setRunParam"));
        }
        
        FUNCTION_EXIT;
    }
    
    
    std::string AdminManager::getProcessDebugLevel(const std::string& processId)
    {
        TA_Base_Core::ThreadGuard guard(m_processLock);
        
        // Convert the ProcessId (entity description) into an entity name.
        std::string entityName = m_entityDescriptionToDetails[processId].entityName;
        
        for (std::vector<ProcessData>::iterator iter = m_processes.begin();
        iter != m_processes.end(); iter++)
        {
            if (iter->ProcessId.in() == entityName)
            {
                return iter->debugLevel;
            }
        }
        return "";
    }
    
    
    bool AdminManager::compareProcesses(const ProcessData& process1, const ProcessData& process2) const
    {
        if (strcmp(process1.ProcessId.in(), process2.ProcessId.in()) != 0)
        {
            return false;
        }
        if (process1.state != process2.state)
        {
            return false;
        }
        if (process1.requestedState != process2.requestedState)
        {
            return false;
        }
        if (strcmp(process1.host.in(), process2.host.in()) != 0)
        {
            return false;
        }
        if (process1.started != process2.started)
        {
            return false;
        }
        if (process1.restarts != process2.restarts)
        {
            return false;
        }
        if (process1.operationMode != process2.operationMode)
        {
            return false;
        }
        if (strcmp(process1.debugLevel.in(), process2.debugLevel.in()) != 0)
        {
            return false;
        }
        
        return true;
    }
    
    void AdminManager::retryLogin()
    {
        if ( ERR_AUTH_NO_ERROR == m_nLoginResult )
        {
            return;
        }

        m_nLoginResult = ERR_LOGIN_NO_PROCESS;
        m_loginSemaphore.post();
    }
    
} // namespace TA_Base_App
