/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/sound/sound_manager/src/RunParamFetcher.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class is used fetch the Run Params from the
  * Process Manager.
  *
  */

#include "RunParamFetcher.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/exceptions/src/ManagedProcessException.h"

using TA_Base_Core::CorbaUtil;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::IProcessManagerCorbaDef;
using TA_Base_Core::ManagedProcessException;

#include <string>
#include <algorithm>
#include <vector>
#include <sstream>
#include <winsock2.h>  // Used by getHostName

namespace TA_Base_App
{

    RunParamFetcher::RunParamFetcher(std::string hostNames)
		: m_processManager(IProcessManagerCorbaDef::_nil())
    {
		bool success = false;
		std::vector<std::string> hostNameList = parseHostnames(hostNames);

		for (std::vector<std::string>::iterator iter = hostNameList.begin();
			iter != hostNameList.end(); iter++)
		{
			try
			{
				LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, 
					"Attempting to retrieve run params from %s", (*iter).c_str());

				locateProcessManager(*iter);
				retrieveSystemParameters();

				// No exception thrown - therefore successful.
				success = true;
				break;
			}
			catch(...)
			{
			}
		}
        
		if (!success)
		{
			TA_THROW(ManagedProcessException("Could not fetch run params from any hostname specified."));
		}
    }


    void RunParamFetcher::locateProcessManager(const std::string& hostName)
    {
        FUNCTION_ENTRY("locateProcessManager");

		std::ostringstream ior;
		ior << "corbaloc::";
		ior << hostName;

		std::string::size_type pos = hostName.find(":");
		if (pos == std::string::npos)
		{
			// Host name doesn't contain a port number. Use the default.
			ior << ":" << TA_Base_Core::DEFAULT_MGR_PORT;
		}
		
		ior << "/ProcessManager";

		try
		{
            CORBA::Object_var obj;
            obj = CorbaUtil::getInstance().stringToObject(ior.str());
            if (!CORBA::is_nil(obj))
            {
                m_processManager = IProcessManagerCorbaDef::_narrow(obj);
            }
            if (CORBA::is_nil(m_processManager))
			{
				TA_THROW( ManagedProcessException("Failed to narrow to the Process Manager") );
			}
		}
		catch( const ManagedProcessException& )
		{
			throw; // Ensure we are only throwing ManagedProcessExceptions
		}
		catch( ... )
		{
			// We are not expecting to catch anything other than ManagedProcessExceptions
			// If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
			// them as ManagedProcessExceptions.
			LOG( SourceInfo, DebugUtil::ExceptionCatch,"Unknown", 
				"Caught an unknown exception, throwing a ManagedProcessException in its place" );
			TA_THROW( ManagedProcessException("Could not locate Process Manager") );
		}

        FUNCTION_EXIT;
    }


    void RunParamFetcher::retrieveSystemParameters()
    {
        FUNCTION_ENTRY("retrieveSystemParameters");

        try
        {
			// Retrieve our hostname.
			char hostName[512];
			int hNameLength = 512;

			if( gethostname(hostName, hNameLength) != 0 )
			{
	            TA_THROW( ManagedProcessException( "Host name could not be retrieved") );
		    }

            // Get system parameters from Process Manager
            TA_Base_Core::RunParamSeq_var p = m_processManager->getParams(0, hostName);
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

						//fix the defect, need to copy .csv file to c:\winnt\system32 folder.
						std::string parameterName(p[i].name.in());						
						std::transform(parameterName.begin(), parameterName.end(), parameterName.begin(), toupper); 
						if (parameterName.find("DBCONNECTIONFILE") !=std::string::npos)
						{
							std::string parameterValue(p[i].value.in());
							std::transform(parameterValue.begin(), parameterValue.end(), parameterValue.begin(), toupper); 
							if (parameterValue.find("TRANSACTIVE")==std::string::npos)
							{
								parameterValue="C:\\transActive\\config\\database\\";
								parameterValue=parameterValue +p[i].value.in() ;
								RunParams::getInstance().set(p[i].name.in(), parameterValue.c_str());
								LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug,
                                       "Setting %s to %s", p[i].name.in(),parameterValue.c_str());
							}
							else
							{
								RunParams::getInstance().set(p[i].name.in(), p[i].value.in());							
							}
						}
						else
						{
							RunParams::getInstance().set(p[i].name.in(), p[i].value.in());
						}
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
        }
        catch( ... )
        {
            // We are not expecting any exceptions to be thrown
            // If any unexpected exceptions are raised (eg CORBA::Exception), we re-throw
            // them as ManagedProcessExceptions.
            LOG( SourceInfo, DebugUtil::ExceptionCatch,"Unknown", 
                 "Caught an unknown exception, throwing a ManagedProcessException in its place" );
            TA_THROW( ManagedProcessException("Failed to retrieve system parameters from the process manager") );
        }

        FUNCTION_EXIT;
    }


	std::vector<std::string> RunParamFetcher::parseHostnames(const std::string& hostNames)
	{
		std::vector<std::string> hostNameList;
		std::string::size_type start = 0;
		std::string::size_type end = 0;

		while (end != std::string::npos)
		{
			end = hostNames.find_first_of(",", start);
			hostNameList.push_back(hostNames.substr(start, end - start));
			start = end + 1; // Skip the ","
		}

		return hostNameList;
	}

} // namespace TA_Base_App
