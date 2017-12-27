/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/rights_service/src/AccessRights.cpp $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * Implementation for COM interface that returns the access control decision
  */


#include "stdafx.h"
#include "app/security/rights_service/src/RippleRights.h"
#include "app/security/rights_service/src/AccessRights.h"
#include <winsock2.h>  // Used by getHostName
#include <COMDEF.H.>
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "bus\security\rights_library\src\RightsLibrary.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/ManagedProcessException.h"
#include "core\exceptions\src\RightsException.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\DebugUtil.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RightsException;
using TA_Base_Core::RunParams;
using TA_Base_Core::ManagedProcessException;
using TA_Base_Core::CorbaUtil;

const char* CAccessRights::RIGHTS_INDETERMINATE = "Indeterminate";
const char* CAccessRights::RIGHTS_NOT_APPLICABLE = "Not Applicable";
const char* CAccessRights::LOG_FILE_NAME = "AccessRights";

/////////////////////////////////////////////////////////////////////////////
// CAccessRights
CAccessRights::CAccessRights()  
: m_rightsMgr(NULL)
{
    try
    {
        // We need to get the run params from the process manager
        retrieveRunParams();

        // Initialising rights library
        TA_Base_Bus::RightsLibraryFactory rightsFactory;
        m_rightsMgr = rightsFactory.buildRightsLibrary();
	}
    catch(...)
    {
        // we got either ManagedProcessException, RightsException or something else
        if (m_rightsMgr != NULL)
        {
            delete m_rightsMgr;
            m_rightsMgr = NULL;
        }
    }
}


CAccessRights::~CAccessRights()
{
	TA_Base_Core::CorbaUtil::getInstance().shutdown();
	TA_Base_Core::CorbaUtil::cleanup();
}


STDMETHODIMP CAccessRights::isActionPermittedOnResource(BSTR sessionId, long resourceKey, long actionKey, ResponseEnum *response)
{
    ResponseEnum& permission = *response;

    // If we didn't manage to initialise properly, then just inform caller of the error
    if (m_rightsMgr == NULL)
    {
        permission = TAError;
    	return S_OK;
    }

    TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
    std::string reason;
    try
    {
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
        std::string reason;

        _bstr_t tmpbstr(sessionId, FALSE); //necessary to avoid a memory leak

        bool allowed = m_rightsMgr->isActionPermittedOnResource(
            std::string(tmpbstr), resourceKey, actionKey, reason, decisionModule);

        if (allowed)
        {
            permission = Permit;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Permitted to perform action=%d", 
                actionKey);
        }
        else
        {
            permission = Deny;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Not permitted to perform action=%d - %s", 
                actionKey, reason.c_str());
        }
    }
    catch (const RightsException& ex)
    {
        // The library throws this exception when the right is either
        // indeterminate or not applicable, so figure out the type of error 
        // based on the exception string
        std::string err = ex.what();
        if (err.compare(RIGHTS_INDETERMINATE) == 0)
        {
            permission = Indeterminate;
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Unable to determine rights to perform action=%d - %s", 
                actionKey, reason.c_str());
        }
        else if (err.compare(RIGHTS_INDETERMINATE) == 0)
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
                "Access rights are not applicable for action=%d - %s", 
                actionKey, reason.c_str());
            permission = NotApplicable;
        }
        else
        {
            LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                "Unable to determine contact rights agent for determining rights - %s", 
                reason.c_str());
            permission = TAError;
        }
    }
    catch (...)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
            "Some error has occurred");
        // The library does not catch any exception, so any exception 
        // other than the RightsException is error
        permission = TAError;
    }
	return S_OK;
}


void CAccessRights::retrieveRunParams()
{
    FUNCTION_ENTRY("locateProcessManager");

    // get console name
    char hostName[512];

    if(gethostname(hostName, sizeof(hostName)) != 0)
    {
        int error = WSAGetLastError();
        std::string errStr;
        switch (error)
        {
        case WSAEFAULT:
            errStr = "The name parameter is not a valid part of the user address space, or the buffer size specified by namelen parameter is too small to hold the complete host name. ";
            break;
        case WSANOTINITIALISED:
        {
            // The Windows sockets library is not smart enough to figure out
            // that it should be automatically started, therefore we must start
            // Windows sockets and then try again.
            WORD wVersionRequested;
            WSADATA wsaData;
            int err;
            wVersionRequested = MAKEWORD( 2, 2 );
            err = WSAStartup( wVersionRequested, &wsaData );
            if( 0 != err )
            {
                errStr = "Unable to start up WSA";
                break;
            }

            // Try again.
            if(gethostname(hostName, sizeof(hostName)) != 0)
            {
                errStr = "A successful WSAStartup call must occur before using this function. ";
            }
            break;
        }
        case WSAENETDOWN:
            errStr = "The network subsystem has failed. ";
            break;
        case WSAEINPROGRESS:
            errStr = "A blocking Windows Sockets 1.1 call is in progress, or the service provider is still processing a callback function. ";
            break;
        default:
            break;
        }

        if (!errStr.empty())
        {
            TA_THROW(ManagedProcessException(errStr));
        }
    }
    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "This console is: %s",hostName );

    unsigned long port = strtoul(RunParams::getInstance().get(RPARAM_MGRPORT).c_str(), NULL, 0);
    if (port == 0)
    {
        // Port was not already assigned, use our default
        port = TA_Base_Core::DEFAULT_MGR_PORT;
    }
    char ior[100];

    sprintf(ior, "corbaloc::localhost:%lu/ProcessManager", port);
    RunParams::getInstance().set("ProcessManagerIor", ior);

	TA_Base_Core::CorbaUtil::getInstance().initialise();
	TA_Base_Core::CorbaUtil::getInstance().activate();

    CORBA::Object_var obj;
    obj = CorbaUtil::getInstance().stringToObject(ior);
    TA_Base_Core::IProcessManagerCorbaDef_ptr processManager;
    if (!CORBA::is_nil(obj))
    {
        processManager = TA_Base_Core::IProcessManagerCorbaDef::_narrow(obj);
    }
    if (CORBA::is_nil(processManager))
	{
		TA_THROW( ManagedProcessException("Failed to narrow to the Process Manager") );
	}

    // Get system parameters from Process Manager
    TA_Base_Core::RunParamSeq_var p = processManager->getParams(0, hostName);
    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
        "Number of parameters fetched from ProcessManager is %d", p->length());

    for (unsigned int i = 0; i < p->length(); i++)
    {
        if ((p[i].name.in())[0] == '\0')
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn,
                "Received nameless parameter from process manager");
        }
        else
        {
            // For each parameter - if not already set, set it
            if (!RunParams::getInstance().isSet(p[i].name.in()))
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                    "Setting %s to %s", p[i].name.in(), p[i].value.in());
                RunParams::getInstance().set(p[i].name.in(), p[i].value.in());
            }
            else
            {
                if( RunParams::getInstance().get(p[i].name.in()).empty() )
                {
                    RunParams::getInstance().set(p[i].name.in(), p[i].value.in());
                }
            }
        }
    }

    // Set up the debug file for the application from the DebugPath if we didn't get the
    // debug file from runparam.
    std::string debugPath = RunParams::getInstance().get(TA_Base_App::TA_ControlStation::DEBUG_LOG_DIR.c_str());
    if (RunParams::getInstance().get(RPARAM_DEBUGFILE).empty() && !debugPath.empty()) 
    {
        // Check if there is a slash at the end of the debug path specified. If there isn't then add one
        if ( (debugPath[debugPath.length() - 1] != '/') &&
             (debugPath[debugPath.length() - 1] != '\\') )
        {
            debugPath += "\\";
        }
        debugPath += TA_Base_App::TA_ControlStation::LOG_PRE_STRING;
        debugPath += LOG_FILE_NAME;
        debugPath += TA_Base_App::TA_ControlStation::LOG_POST_STRING;
        setDebugFile(debugPath);
    }

    FUNCTION_EXIT;
}


void CAccessRights::setDebugFile(std::string file)
{
    // get the debug file name
    if (file.empty())
    {
        return;
    }

    DebugUtil::getInstance().setFile(file.c_str());

    std::string level = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
    if (!level.empty())
    {
        DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(level.c_str()));
    }
}
