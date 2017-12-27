/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/dummy_agent/src/TestAgent.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "TestAgent.h"
#include "ManagedAgentCorbaDef.h"
#include "core/process_management/IDL/src/IProcessObserverCorbaDef.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/src/ProcessIdResolver.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

#include <sstream>

TestAgent::TestAgent()
{
    TA_Base_Core::CorbaUtil::getInstance().setServantKey(this, "TestAgent");
    activateServant(); 

    // Register for run param updates
    RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILE);
    RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVEL);
    RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGFILEMAXSIZE);
    RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGMAXFILES);
	RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGPIDFILENAMES);
	RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELOFF);
	RunParams::getInstance().registerRunParamUser(this, RPARAM_DEBUGLEVELFILE);

    m_managedAgent = new ManagedAgentCorbaDef();
    m_managedAgent->start();

    CORBA::Object_var obj;

	// Get the port to use.
	unsigned short port;
	if (RunParams::getInstance().isSet("PortNumber"))
	{
		port = atoi(RunParams::getInstance().get("PortNumber").c_str());
	}
	else
	{
		port = TA_Base_Core::DEFAULT_MGR_PORT;
	}

    // Create a string in the form: 
    // "corbaloc::localhost:port/ProcessManager"
    std::ostringstream systemControllerString;
    systemControllerString << "corbaloc::localhost:"
                           << port
                           << "/ProcessManager";

    // Attempt to resolve an IProcessManagerCorbaDef object
    try
    {
        obj = CorbaUtil::getInstance().stringToObject(systemControllerString.str());
        if (!CORBA::is_nil(obj))
		{
            TA_Base_Core::IProcessManagerCorbaDef_var temp = TA_Base_Core::IProcessManagerCorbaDef::_narrow(obj);
            m_systemController = TA_Base_Core::IProcessManagerCorbaDef::_narrow(temp);
        }   
    }
    catch (CORBA::SystemException& e)
    {
        CORBA::Any any;
        any <<= e;
        CORBA::TypeCode_var tc = any.type();
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"System exception: %s", tc->name());
        throw;
    }
    catch ( ... )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"Error resolving System Controller");
        throw;
    }

    if (CORBA::is_nil(m_systemController))
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
			"Error resolving System Controller");
    }
    else
    {
        try
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "Successfully resolved remote System Controller");

            // Get our process id.
            unsigned long pid = TA_Base_Core::ProcessIdResolver::getProcessId();
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
                "TestAgent has found its pid to be %lu", pid);

            std::string entity = RunParams::getInstance().get(RPARAM_ENTITYNAME);
            m_systemController->registerManagedProcess(this->_this(),
                pid, 7, CORBA::string_dup(entity.c_str()));
            m_systemController->registerManagedApplication(m_managedAgent->getManagedAgent(), pid);

            TA_Base_Core::RunParamSeq* params = m_systemController->getParams(pid, "");

            std::string debugFile = std::string((*params)[4].value);
            TA_Base_Core::DebugUtil::getInstance().setFile(debugFile.c_str());

            std::string opMode = std::string((*params)[5].value);
            if (opMode == "Control")
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Setting operation mode to Control");
                m_managedAgent->setOperationMode(TA_Base_Core::Control);
            }
            else
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
                    "Setting operation mode to Monitor");
                m_managedAgent->setOperationMode(TA_Base_Core::Monitor);
            }

			try
			{
				if(!CORBA::is_nil(m_systemController))
				{
					m_systemController->statusChanged(TA_Base_Core::RunningControl, pid);
				}
			}
			catch(...)
			{
				LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
					"Exception raised when notifying Process Manager of state change");
			}
   

        }
        catch(...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                "Error registering with System Controller.");
        }
        
    }
}

TestAgent::~TestAgent()
{
    m_managedAgent->deactivateAndDeleteServant();
}

void TestAgent::terminate()
{
}

void TestAgent::setParam(const TA_Base_Core::RunParam& param)
{
    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugFatal, "Setting runparam (%s, %s)",
        param.name.in(), param.value.in());

    TA_Base_Core::RunParams::getInstance().set(param.name, param.value);
}

void TestAgent::onRunParamChange(const std::string& name,const std::string& value)
{
    // New debug file name
    if (name.compare(RPARAM_DEBUGFILE) == 0)
    {
        DebugUtil::getInstance().setFile(value.c_str());
    }
    // New debug level
    else if (name.compare(RPARAM_DEBUGLEVEL) == 0)
    {
        DebugUtil::getInstance().setLevel( DebugUtil::getDebugLevelFromString(value.c_str()));
    }
    // New max file size
    else if (name.compare(RPARAM_DEBUGFILEMAXSIZE) == 0)
    {
        DebugUtil::getInstance().setMaxSize(atoi(value.c_str()));
    }
    // New max number of debug files
    else if (name.compare(RPARAM_DEBUGMAXFILES) == 0)
    {
        DebugUtil::getInstance().setMaxFiles(atoi(value.c_str()));
    }
	// Use pid in filenames
	else if (name.compare(RPARAM_DEBUGPIDFILENAMES) == 0)
	{
		DebugUtil::getInstance().encryptPidInFilenames(value);
	}
	// Turn off logging for these debug levels
	else if (name.compare(RPARAM_DEBUGLEVELOFF) == 0)
	{
		DebugUtil::getInstance().decodeDisabledLevels(value);
	}
	// Filenames for debug levels
	else if (name.compare(RPARAM_DEBUGLEVELFILE) == 0)
	{
		DebugUtil::getInstance().decodeLevelFilenames(value);
	}
    else
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
            "Received change notification for unsupported parameter %s", name.c_str());
    }
}

void TestAgent::poll()
{
}

void TestAgent::servicesRestarted()
{
}

TA_Base_Core::RunParamSeq* TestAgent::getParams()
{
    return NULL;
}

int main(int argc, char* argv[])
{
   	TA_Base_Core::CorbaUtil::getInstance().initialise();
    TA_Base_Core::CorbaUtil::getInstance().activate();

    RunParams::getInstance().parseCmdLine(argc, argv);
    
    TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugDebug);

    TestAgent* agent = new TestAgent();

    TA_Base_Core::CorbaUtil::getInstance().run();
    return 0;
}
