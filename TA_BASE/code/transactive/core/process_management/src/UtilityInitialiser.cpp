/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/process_management/src/UtilityInitialiser.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */
#ifdef _MSC_VER
    #pragma warning(disable:4786)
#endif

#include "core/utilities/src/DebugSETranslator.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/TAAssert.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/data_access_interface/src/NamingPortAccessFactory.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::CorbaUtil;

namespace TA_Base_Core
{
    //
    // initialiseUtilities
    //
    bool UtilityInitialiser::initialiseUtilities(int argc, char* argv[])
    {
        try
        {
            if (false == RunParams::getInstance().parseCmdLine(argc, argv))
            {
                if (RunParams::getInstance().isSet(RPARAM_VERSION))
                {
                    // Versioning info printed to stdout. Not an exceptional condition,
                    // but there's no need to continue.
                    return false;
                }

                TA_THROW(TA_Base_Core::InvalidCommandLineException("Command line arguments are in invalid format"));
            }

            parseLocalConfigurationFile();
            gSetDebugUtilFromRunParams();
            gInitStructuredExceptionHandler();   // Only initialises the calling thread
            initialiseCorba();
        }
        catch (const TA_Base_Core::UtilitiesException&)
        {
            throw; // Ensuring we only throw the exceptions listed in the documentation
        }
        catch (const TA_Base_Core::InvalidCommandLineException&)
        {
            throw; // Ensuring we only throw the exceptions listed in the documentation
        }
        catch (...)
        {
            TA_THROW(TA_Base_Core::UtilitiesException("One or more utilities failed to initialise"));
        }

        return true;
    }

    //
    // initialiseUtilities
    //
    bool UtilityInitialiser::initialiseUtilities(const std::string& commandLine)
    {
        try
        {
            if (false == RunParams::getInstance().parseCmdLine(commandLine.c_str()))
            {
                if (RunParams::getInstance().isSet(RPARAM_VERSION))
                {
                    // Versioning info printed to stdout. Not an exceptional condition,
                    // but there's no need to continue.
                    return false;
                }

                TA_THROW(TA_Base_Core::InvalidCommandLineException("Command line arguments are in invalid format"));
            }

            parseLocalConfigurationFile();
            gSetDebugUtilFromRunParams();
            gInitStructuredExceptionHandler();   // Only initialises the calling thread
            initialiseCorba();
        }
        catch (const TA_Base_Core::UtilitiesException&)
        {
            throw; // Ensuring we only throw the exceptions listed in the documentation
        }
        catch (const TA_Base_Core::InvalidCommandLineException&)
        {
            throw; // Ensuring we only throw the exceptions listed in the documentation
        }
        catch (...)
        {
            TA_THROW(TA_Base_Core::UtilitiesException("One or more utilities failed to initialise"));
        }

        return true;
    }

    //
    // initialiseCorba
    //
    void UtilityInitialiser::initialiseCorba()
    {
        int port = 0;

        if (RunParams::getInstance().isSet(RPARAM_CORBA_NAMING_PORT))
        {
            port = TA_Base_Core::getRunParamValue(RPARAM_CORBA_NAMING_PORT, 0);
        }
        else
        {
            if (RunParams::getInstance().isSet(RPARAM_NSHOSTNAME) && RunParams::getInstance().isSet(RPARAM_ENTITYNAME))
            {
                std::string hostName = RunParams::getInstance().get(RPARAM_NSHOSTNAME);
                std::string agentName = RunParams::getInstance().get(RPARAM_ENTITYNAME);

                try
                {
                    port = NamingPortAccessFactory::getInstance().getNamingPort(agentName).getNsPort(hostName);
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "NS port = %lu, agnetName=%s, hostName=%s", port, agentName.c_str(), hostName.c_str());
                }
                catch (const DataException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "DataException", ex.what());
                }
            }
        }

        if (false == CorbaUtil::getInstance().initialise(port))
        {
            TA_THROW(TA_Base_Core::UtilitiesException("Failed to initialise corba"));
        }

        try
        {
            CorbaUtil::getInstance().activate();
        }
        catch (...)
        {
            TA_THROW(TA_Base_Core::UtilitiesException("Failed to initialise corba"));
        }
    }
}
