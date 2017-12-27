/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Katherine Thomson
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Provides methods to handle the problems of accessing
  * singletons across a DLL interface.
  */

#pragma warning (disable : 4786)
#pragma warning (disable : 4284)

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/SingletonHelper.h"
#include "core/exceptions/src/UtilitiesException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/corba/src/CorbaUtil.h"

namespace TA_Base_App
{
    void SingletonHelper::initialiseUtilities(const IDllHandler& handler, const std::string& dllName)
    {
        // Pick a random runparam that always gets set and see if it is already set
        if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_APPNAME))
        {
            // We have already initialised the singletons so don't do it again
            return;
        }

        initialiseDebugUtil(dllName);
        initialiseCorba(handler.getCorbaUtil());
        setRunParams(handler.getRunParams());
        handler.getRunParamsInstance().forwardUpdatesTo(&TA_Base_Core::RunParams::getInstance());
    }

    void SingletonHelper::initialiseCorba(TA_Base_Core::CorbaUtil& corbaUtil)
    {
        TA_Base_Core::CorbaUtil::setInstance(corbaUtil);
    }

    void SingletonHelper::setRunParams(TA_Base_Core::RunParams::ParamVector& mainAppRunParams)
    {
        TA_Base_Core::RunParams& dllRunParams = TA_Base_Core::RunParams::getInstance();
        TA_Base_Core::RunParams::ParamVector::iterator it = mainAppRunParams.end();

        for (it = mainAppRunParams.begin(); it != mainAppRunParams.end(); it++)
        {
            if (!dllRunParams.isSet(it->name.c_str()))
            {
                dllRunParams.set(it->name.c_str(), it->value.c_str());
            }
        }
    }

    void SingletonHelper::initialiseDebugUtil(const std::string& dllName)
    {
        // Set the debug level
        std::string param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);

        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(param.c_str()));
        }

        // Set the maximum size of a debug file in bytes
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILEMAXSIZE);

        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxSize(atoi(param.c_str()));
        }

        // Set the maximum number of log files
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGMAXFILES);

        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().setMaxFiles(atoi(param.c_str()));
        }

        // Use Pid encoding in filenames
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGPIDFILENAMES);

        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().encryptPidInFilenames(param);
        }

        // Use debug level specific log files
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELFILE);

        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().decodeLevelFilenames(param);
        }

        // Turn off logging for debug levels
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVELOFF);

        if (!param.empty())
        {
            TA_Base_Core::DebugUtil::getInstance().decodeDisabledLevels(param);
        }

        // Set the debug file name
        // It will be the debug file name for the MapViewer + the DLL name.
        param = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE);

        if (!param.empty())
        {
            std::string::size_type pos = param.find_last_of(".");
            std::string strToInsert = "." + dllName;

            if (pos == std::string::npos)
            {
                pos = param.length();
            }

            param.insert(pos, strToInsert.c_str());

            TA_Base_Core::DebugUtil::getInstance().setFile((param).c_str());
        }
    }
}
