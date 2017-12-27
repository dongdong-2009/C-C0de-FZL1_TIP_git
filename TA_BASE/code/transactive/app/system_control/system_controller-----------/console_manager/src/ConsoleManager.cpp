/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/system_control/system_controller/console_manager/src/ConsoleManager.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2012/02/10 17:45:28 $
  * Last modified by:  $Author: haijun.li $
  *
  */
// ConsoleManager.cpp
//
// This program is designed to run as a console application, not as a GUI.
// User interface added and ability to have a different log file for each application
//
// Author:      Bruce Fountain 08-Feb-2001
// Modified by: $Author: haijun.li $
// Modified on: $DateTime: 2012/02/10 17:45:28 $
// Version:     $Revision: #3 $
//

#include "SystemControllerAdmin.h"
#include "ProcessManager.h"
#include "ControlStationCorbaImpl.h"
#include "SystemControllerMonitor.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/process_management/IDL/src/IProcessManagerCorbaDef.h"
#include "core/exceptions/src/AuthenticationSecurityException.h"
#include "core/exceptions/src/AuthenticationAgentException.h"
#include "core/exceptions/src/InvalidCommandLineException.h"
#include "core/threads/src/Thread.h"
#include "core/data_access_interface/entity_access/src/TTISManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/MmsAgentEntityData.h"
#include <iostream>

using namespace TA_Base_Core;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::CorbaUtil;

const char* PASSWORD_DEFAULT = "<empty>";
const char* RPARAM_USERID = "UserId";
const char* RPARAM_PROFILEID = "ProfileId";
const char* RPARAM_LOCATIONID = "LocationId";
const char* RPARAM_WORKSTATIONID = "WorkstationId";
const char* RPARAM_PASSWORD = "Password";
const char* RPARAM_ISSYSTEMCONTROL = "IsSystemControl";
const unsigned int MAX_FAILED_TIMES = 20;


// Forward declaration
bool retrieveSessionId();
void changeProfile(const std::string&, const std::string&, const std::string&);
bool loadParamFromConfigFile();
bool init(int argc, char* argv[]);
std::string getHelpString();
void startInputLoop(ProcessManager*& mp, SystemControllerAdmin*& sca, SystemControllerMonitor*& scm, ControlStationCorbaImpl*& cs);

int main(int argc, char* argv[])
{
    try
    {
        init(argc, argv);
    }
    catch (...)
    {
        std::cout << "could not load initialize parameter correctly, please check it" << std::endl;
        return -1;
    }

    // 	LOGLARGESTRING(SourceInfo, DebugUtil::DebugInfo, "long content");
    // 	return -1;

    bool isSystemControl = RunParams::getInstance().isSet(RPARAM_ISSYSTEMCONTROL);

    // 	TA_Base_Core::IEntityData* guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(TA_Base_Core::TTISManagerEntityData::getStaticType());
    //
    // 	TA_Base_Core::TTISManagerEntityData* TTISEntityData = dynamic_cast<TA_Base_Core::TTISManagerEntityData*>(guiEntity);

    // If the sessionId parameter is not set, try retrieving a valid session id
    // with the given parameters
    bool sessionRequestSuccessful = false;

    if (!isSystemControl)
    {
        if (!RunParams::getInstance().isSet(RPARAM_SESSIONID))
        {
            sessionRequestSuccessful = retrieveSessionId();
        }
    }

    ProcessManager* mp = new ProcessManager;
    SystemControllerAdmin* sca = new SystemControllerAdmin;
    SystemControllerMonitor* scm = new SystemControllerMonitor;
    ControlStationCorbaImpl* cs = new ControlStationCorbaImpl("");

    std::string helpStr = getHelpString();
    std::cout << helpStr << std::endl;

    startInputLoop(mp, sca, scm, cs);

    // If we get to here it means an error has occurred.
    return 1;
}


bool retrieveSessionId()
{
    if (RunParams::getInstance().isSet(RPARAM_USERID) || RunParams::getInstance().isSet(RPARAM_PROFILEID) ||
            RunParams::getInstance().isSet(RPARAM_LOCATIONID) || RunParams::getInstance().isSet(RPARAM_WORKSTATIONID) ||
            RunParams::getInstance().isSet(RPARAM_PASSWORD))
    {
        unsigned long userId;
        unsigned long profileId;
        unsigned long locationId;
        unsigned long workstationId;
        std::string password;

        std::string userIdParam = RunParams::getInstance().get(RPARAM_USERID);

        if (!userIdParam.empty())
        {
            userId = strtoul(userIdParam.c_str(), NULL, 10);
        }
        else
        {
            std::cout << "Couldn't retrieve SessionId: Missing UserId parameter." << std::endl;
            return false;
        }

        std::string profileIdParam = RunParams::getInstance().get(RPARAM_PROFILEID);

        if (!profileIdParam.empty())
        {
            profileId = strtoul(profileIdParam.c_str(), NULL, 10);
        }
        else
        {
            std::cout << "Couldn't retrieve SessionId: Missing ProfileId parameter." << std::endl;
            return false;
        }

        std::string locationIdParam = RunParams::getInstance().get(RPARAM_LOCATIONID);

        if (!locationIdParam.empty())
        {
            locationId = strtoul(locationIdParam.c_str(), NULL, 10);
        }
        else
        {
            std::cout << "Couldn't retrieve SessionId: Missing LocationId parameter." << std::endl;
            return false;
        }

        std::string workstationIdParam = RunParams::getInstance().get(RPARAM_WORKSTATIONID);

        if (!workstationIdParam.empty())
        {
            workstationId = strtoul(workstationIdParam.c_str(), NULL, 10);
        }
        else
        {
            std::cout << "Couldn't retrieve SessionId: Missing WorkstationId parameter." << std::endl;
            return false;
        }

        std::string passwordParam = RunParams::getInstance().get(RPARAM_PASSWORD);

        if (!passwordParam.empty())
        {
            password = passwordParam;
        }
        else
        {
            password = PASSWORD_DEFAULT;
        }

        std::string locationParam = RunParams::getInstance().get(RPARAM_LOCATIONKEY);

        if (locationParam.empty())
        {
            std::cout << "Couldn't retrieve SessionId: Missing LocationKey parameter." << std::endl;
            return false;
        }

        const unsigned long sessionRetryPeriod = 5000; // 5 seconds
        bool resolvedAuthenticationAgent = false;
        std::string sessionId = "";

        TA_Base_Bus::AuthenticationLibrary authLib;

        unsigned int failedCount = 0;

        while (!resolvedAuthenticationAgent)
        {
            try
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Requesting session from Authentication Agent");
                sessionId = authLib.requestSession(userId, profileId, locationId, workstationId, password);

                resolvedAuthenticationAgent = true;
            }
            catch (TA_Base_Core::AuthenticationAgentException& ae)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Failed to request session from Authentication Agent");

                LOG(SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationAgentException",
                    ae.what());

                resolvedAuthenticationAgent = false;
            }
            catch (TA_Base_Core::AuthenticationSecurityException& ase)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Failed to request session from Authentication Agent");

                LOG(SourceInfo, DebugUtil::ExceptionCatch, "AuthenticationSecurityException",
                    ase.what());

                resolvedAuthenticationAgent = false;
            }
            catch (...)
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "An unexpected error occured while requesting session from Authentication Agent");
                resolvedAuthenticationAgent = false;
            }

            if (!resolvedAuthenticationAgent)
            {
                failedCount++;

                if (failedCount > MAX_FAILED_TIMES)
                {
                    std::cout << "could not request session, retry " << failedCount << " times" << std::endl;
                }

                TA_Base_Core::Thread::sleep(sessionRetryPeriod);
            }
        }

        RunParams::getInstance().set(RPARAM_SESSIONID, sessionId.c_str());
        return true;
    }

    return false;
}

void changeProfile(const std::string& profileIdParam, const std::string& locationIdParam, const std::string& passwordParam)
{
    if (!RunParams::getInstance().isSet(RPARAM_SESSIONID))
    {
        return;
    }

    try
    {
        TA_Base_Bus::AuthenticationLibrary authLib;
        std::string sessionId = authLib.changeProfile(RunParams::getInstance().get(RPARAM_SESSIONID),
                                strtoul(profileIdParam.c_str(), NULL, 10),
                                strtoul(locationIdParam.c_str(), NULL, 10),
                                passwordParam);

        RunParams::getInstance().set(RPARAM_PROFILEID, profileIdParam.c_str());
        RunParams::getInstance().set(RPARAM_LOCATIONID, locationIdParam.c_str());
        RunParams::getInstance().set(RPARAM_PASSWORD, passwordParam.c_str());
        RunParams::getInstance().set(RPARAM_SESSIONID, sessionId.c_str());
    }
    catch (const TA_Base_Core::AuthenticationSecurityException& ex)
    {
        std::cerr << "Couldn't change profile: " << ex.what() << std::endl;
    }
    catch (const TA_Base_Core::AuthenticationAgentException& ex)
    {
        std::cerr << "Caught TA_Base_Core::AuthenticationAgentException: " << ex.what() << std::endl;
    }
    catch (const TA_Base_Core::ObjectResolutionException& e)
    {
        std::cerr << "Caught TA_Base_Core::ObjectResolutionException: " << e.what() << std::endl;
    }
    catch (const std::exception& e)
    {
        std::cerr << "Caught std::exception: " << typeid(e).name() << ", " << e.what() << std::endl;
    }
    catch (const CORBA::Exception& e)
    {
        std::cerr << "Caught CORBA::Exception: " << TA_Base_Core::CorbaUtil::getInstance().exceptionToString(e).c_str() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Caught unknown exception." << std::endl;
    }
}

bool loadParamFromConfigFile()
{
    if (RunParams::getInstance().isSet("ConfigFile"))
    {
        std::ifstream cfgfile(RunParams::getInstance().get("ConfigFile").c_str());

        if (cfgfile.bad())
        {
            std::cout << "Unable to open ConfigFile: " << RunParams::getInstance().get("ConfigFile").c_str() << std::endl;
            return false;
        }

        const int MAX_LINE_LEN = 1000;
        char line[MAX_LINE_LEN + 1];
        char* str;
        std::string name;
        std::string value;

        while (!cfgfile.eof())
        {
            cfgfile.getline(line, MAX_LINE_LEN);
            str = line;
            name = "";
            value = "";

            while (*str != '\0' && *str != '=')
            {
                name += *str;
                str++;
            }

            if (!name.empty())
            {
                if (*str == '=')
                {
                    str++;

                    while (*str != '\0' && *str != '\n' && *str != '\r')
                    {
                        value += *str;
                        str++;
                    }
                }

                RunParams::getInstance().set(name.c_str(), value.c_str());
            }
        }
    }

    return true;
}

bool init(int argc, char* argv[])
{
    if (false == RunParams::getInstance().parseCmdLine(argc, argv))
    {
        if (RunParams::getInstance().isSet(RPARAM_VERSION))
        {
            // Versioning info printed to stdout. Not an exceptional condition,
            // but there's no need to continue.
            return false;
        }

        TA_THROW(TA_Base_Core::InvalidCommandLineException
                 ("Command line arguments are in invalid format"));
    }

    if (!loadParamFromConfigFile())
    {
        return false;
    }

    // Pin the port that this process will use. We can set this from the
    // cmdline or from an environment variable, or drop through to the default
    int mgrPort = atoi(RunParams::getInstance().get(RPARAM_MGRPORT).c_str());

    if (mgrPort == 0)
    {
        mgrPort = TA_Base_Core::DEFAULT_MGR_PORT;
    }

    TA_Base_Core::gSetDebugUtilFromRunParams();
    TA_Base_Core::gInitStructuredExceptionHandler();   // Only initialises the calling thread
    CorbaUtil::getInstance().initialise(mgrPort);

    CorbaUtil::getInstance().activate();
    return true;
}

std::string getHelpString()
{
    std::string helpStr;

    helpStr = "\n\nGeneral Commands:\n";
    helpStr += "list - Lists all currently running processes\n";
    helpStr += "terminate <id> - Terminates the specified application\n";

    if (RunParams::getInstance().isSet(RPARAM_SESSIONID))
    {
        helpStr += "chprofile <profile id> <location id> [<password>] - Changes current session profile (default password: ";
        helpStr += PASSWORD_DEFAULT;
        helpStr += ")\n";
    }

    helpStr += "help - Print this message\n";
    helpStr += "exit - This will close the Console Manager\n";

    helpStr += "\nAgent Commands:\n";
    helpStr += "set control <id> - This will tell the agent to switch to control mode\n";
    helpStr += "set monitor <id> - This will tell the agent to switch to monitor mode\n";

    helpStr += "\nGUI Commands:\n";
    helpStr += "maximise <id> - This will tell the GUI to maximise\n";
    helpStr += "minimise <id> - This will tell the GUI to minimise\n";
    helpStr += "restore <id> - This will tell the GUI to restore and gain the focus\n";
    helpStr += "server up <id> - This will tell the GUI the server has come up\n";
    helpStr += "server down <id> - This will tell the GUI the server has gone down\n";
    helpStr += "setparam <id> <paramName> [paramValue] - Set a runparam (and its optional value) for the specified Managed Process. \n";
    helpStr += "setparamthis <paramName> [paramValue] - Set a runparam (and its optional value) for this(ConsoleManager). \n";
    helpStr += "deactivatesystemcontroller - Deactivate SystemController. \n";
    helpStr += "activatesystemcontroller - Activate SystemController. \n";
    helpStr += "\n";

    return helpStr;
}

void startInputLoop(ProcessManager*& mp, SystemControllerAdmin*& sca, SystemControllerMonitor*& scm, ControlStationCorbaImpl*& cs)
{
    std::string commands;

    do
    {
        std::cin >> commands;

        // help
        if (0 == commands.compare("help"))
        {
            std::cout << getHelpString();
        }

        // list
        else if (0 == commands.compare("list"))
        {
            std::cout << mp->getRunningApps();
        }

        else if (0 == commands.compare("terminate"))
        {
            unsigned long id;
            std::cin >> id;
            mp->terminateApp(id);
        }
        else if (0 == commands.compare("set"))
        {
            unsigned long id;
            std::string mode;
            std::cin >> mode >> id;

            if (0 == mode.compare("control"))
            {
                mp->changeMode(true, id);
            }
            else if (0 == mode.compare("monitor"))
            {
                mp->changeMode(false, id);
            }
            else
            {
                std::cout << "Invalid mode specified\n";
            }
        }
        else if (0 == commands.compare("maximise"))
        {
            unsigned long id;
            std::cin >> id;
            mp->changeGUI(TA_Base_Core::Maximise, id);
        }
        else if (0 == commands.compare("minimise"))
        {
            unsigned long id;
            std::cin >> id;
            mp->changeGUI(TA_Base_Core::Minimise, id);
        }
        else if (0 == commands.compare("restore"))
        {
            unsigned long id;
            std::cin >> id;
            mp->changeGUI(TA_Base_Core::GetFocus, id);
        }
        else if (0 == commands.compare("server"))
        {
            unsigned long id;
            std::string mode;
            std::cin >> mode >> id;

            if (0 == mode.compare("up"))
            {
                mp->serverChange(true, id);
            }
            else if (0 == mode.compare("down"))
            {
                mp->serverChange(false, id);
            }
            else
            {
                std::cout << "Invalid server state specified\n";
            }
        }
        else if (0 == commands.compare("setparam"))
        {
            unsigned long id;
            std::string paramName;
            std::string paramValue;
            std::cin >> id >> paramName;

            //note that ASCII char 10 is a line feed
            //the last param is optional
            if (std::cin.peek() != 10)
            {
                std::cin >> paramValue;
            }

            mp->setClientRunParam(id, paramName, paramValue);
        }
        else if (0 == commands.compare("chprofile"))
        {
            std::string profileIdParam("");
            std::string locationIdParam("");
            std::string passwordParam(PASSWORD_DEFAULT);

            std::cin >> profileIdParam >> locationIdParam;

            //note that ASCII char 10 is a line feed
            //the last param is optional
            if (std::cin.peek() != 10)
            {
                std::cin >> passwordParam;
            }

            changeProfile(profileIdParam, locationIdParam, passwordParam);
        }
        else if (0 == commands.compare("setparamthis"))
        {
            std::string paramName;
            std::string paramValue;
            std::cin >> paramName;

            //note that ASCII char 10 is a line feed
            //the last param is optional
            if (std::cin.peek() != 10)
            {
                std::cin >> paramValue;
            }

            std::cout << "\t" << paramName << " = " << paramValue << std::endl;
            RunParams::getInstance().set(paramName.c_str(), paramValue.c_str());
            mp->onRunParamChange(paramName, paramValue);
        }
        else if (0 == commands.compare("deactivatesystemcontroller"))
        {
            if (sca != NULL)
            {
                sca->deactivateAndDeleteServant();
                sca = NULL;
            }
        }
        else if (0 == commands.compare("activatesystemcontroller"))
        {
            if (NULL == sca)
            {
                sca = new SystemControllerAdmin;
            }
        }
    }
    while (0 != commands.compare("exit"));
}
