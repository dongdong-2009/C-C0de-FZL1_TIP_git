/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/utilities/src/RunParams.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2016/11/03 18:11:38 $
  * Last modified by:  $Author: limin.zhu $
  *
  * A singleton class that stores all the runtime parameters. Runtime parameters
  * can come from several different sources - command line, process manager,
  * entity configuration tables.
  *
  * Because the class is a singleton, runtime parameters can be accessed from
  * anywhere in the code instead of passing them around as parameters
  *
  */
#include "RunParams.h"
#include "DebugUtil.h"
#include "core/versioning/src/Version.h"
#include "core/synchronisation/src/ThreadGuard.h"
#include "core/synchronisation/src/ThreadReadGuard.h"
#include "TAAssert.h"
#include "TA_String.h"
#include <algorithm>
#include <fstream>
#include <boost/foreach.hpp>
#include <boost/regex.hpp>
#include <boost/filesystem.hpp>
#include <boost/algorithm/string.hpp>

#if defined( WIN32 )
    #pragma warning( push )
    #pragma warning( disable : 4786 4244 4290 )
#endif

namespace TA_Base_Core
{
    RunParams& RunParams::getInstance()
    {
        static RunParams* instance = new RunParams;
        return *instance;
    }

    void RunParams::removeInstance()
    {
        RunParams::getInstance().cleanUp();
    }

    RunParams::RunParams()
    {
    }

    void RunParams::cleanUp()
    {
        TA_Base_Core::ThreadGuard guard(m_paramsLock);
        m_params.clear();
    }

    std::string RunParams::get(const char* name)
    {
        static const std::string empty;

        // We want to protect the find from an insert
        TA_Base_Core::ThreadGuard guard(m_paramsLock);

        ParamDetailsMap::const_iterator i = m_params.find(name);

        if (i == m_params.end() || !i->second.is_set)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParams::get(): param %s has not been set", name);
            return empty;
        }

        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParams::get(): param %s == %s", name, i->second.value.c_str());
        return i->second.value;
    }

    void RunParams::set(const char* name, const char* value)
    {
        std::string oldValue;
        UserList users;
        ChildList childRunParams;

        {
            // Prevent an insert occuring while a read is happening!
            TA_Base_Core::ThreadGuard guard(m_paramsLock);

            ParamDetails& detail = m_params[name];
            detail.is_set = true;

            // DO NOT uncomment
            //if (detail.value == value)
            //{
            //    return;
            //}

            // Remember the old value.
            oldValue = detail.value;
            users = detail.users;
            childRunParams = m_childRunParams;

            // Set the new value.
            detail.value = value;

            if (oldValue.size())
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParams::set(): param %s = %s (old value=%s)", name, value, oldValue.c_str());
            }
            else
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParams::set(): param %s = %s", name, value);
            }
        }

        // must let notified agents read the changed paramed
        // so demote write lock to a read lock

        notify(users, name, value);

        // TD10773: Finally, forward set requests onto the children.
        BOOST_FOREACH(RunParams* runParams, childRunParams)
        {
            // Don't pass it on if we're out of sync.
            if (oldValue == runParams->get(name))
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Forwarding update %s=%s onto instance %p.", name, value, runParams);
                runParams->set(name, value);
            }
        }
    }

    bool RunParams::isSet(const char* name)
    {
        // We want to protect the find from an insert
        TA_Base_Core::ThreadGuard guard(m_paramsLock);

        ParamDetailsMap::iterator findIt = m_params.find(name);
        bool isSet = (findIt != m_params.end() && findIt->second.is_set);
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParams::isSet(): isSet(%s) = %s", name, isSet ? "true" : "false");

        return isSet;
    }

    void RunParams::getAll(ParamVector& params)
    {
        params.clear();
        params.resize(m_params.size());

        // We want to protect the find from an insert
        TA_Base_Core::ThreadGuard guard(m_paramsLock);

        size_t i = 0;

        for (ParamDetailsMap::const_iterator it = m_params.begin(); it != m_params.end(); ++it, ++i)
        {
            if (it->second.is_set)
            {
                params[i].name  = it->first;
                params[i].value = it->second.value;
            }
        }
    }

    RunParams::ParamVector RunParams::getAll()
    {
        RunParams::ParamVector all;
        getAll(all);
        return all;
    }

    void RunParams::registerRunParamUser(RunParamUser* user, const char* name)
    {
        TA_ASSERT(name != NULL, "Name passed was NULL");
        TA_ASSERT(name[0] != '\0', "Name passed was NULL");
        TA_ASSERT(user != NULL, "User passed was NULL");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RunParams::registerRunParamUser: %s, %s.", gPtrToStr(user).c_str(), name);

        // We want to protect the registration from read/write
        TA_Base_Core::ThreadGuard guard(m_paramsLock);

        UserList& users = m_params[name].users;

        if (std::find(users.begin(), users.end(), user) == users.end())
        {
            users.push_back(user);
        }
    }

    void RunParams::deregisterRunParamUser(RunParamUser* user)
    {
        TA_ASSERT(user != NULL, "RunParamUser passed is NULL");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RunParams::deregisterRunParamUser: %s", gPtrToStr(user).c_str());

        // We want to protect the registration from read/write
        TA_Base_Core::ThreadGuard guard(m_paramsLock);

        for (ParamDetailsMap::iterator i = m_params.begin(); i != m_params.end(); ++i)
        {
            UserList& users = i->second.users;

            for (UserList::iterator j = users.begin(); j != users.end();)
            {
                if (*j == user)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RunParams::deregisterRunParamUser: %s, %s.", gPtrToStr(user).c_str(), i->first.c_str());
                    j = users.erase(j);
                }
                else
                {
                    ++j;
                }
            }
        }
    }

    void RunParams::forwardUpdatesTo(RunParams* child)
    {
        TA_ASSERT(NULL != child, "RunParam child is NULL!");
        TA_ASSERT(this != child, "Don't forward requests to yourself.");

        LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "RunParam instance %p is registering for update forwarding.", child);

        // Registration of children can be covered by the param details lock.
        TA_Base_Core::ThreadGuard guard(m_paramsLock);

        // Only add the child if it hasn't already been added.
        ChildList::iterator it = std::find(m_childRunParams.begin(), m_childRunParams.end(), child);

        if (it == m_childRunParams.end())
        {
            m_childRunParams.push_back(child);
        }
    }

    void RunParams::notify(const UserList& users, const std::string& name, const std::string& value)
    {
        BOOST_FOREACH(RunParamUser* user, users)
        {
            try
            {
                user->onRunParamChange(name, value);
            }
            catch (...)
            {
            }
        }
    }

    bool RunParams::parseCmdLine(int argc, char* argv[])
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "parseCmdLine(argc, argv)");

        // Convert the cmdline to a vector of string arguments and then parse
        // the args themselves

        std::vector<std::string> args;
        convertCmdLine(argc, argv, args);
        return parseCmdLine(args);
    }

    bool RunParams::parseCmdLine(const char* cmdline)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "parseCmdLine(cmdline)");
        TA_ASSERT(cmdline != NULL, "Command line passed was NULL");

        // Convert the cmdline to a vector of string arguments and then parse
        // the args themselves

        std::vector<std::string> args;
        convertCmdLine(cmdline, args);
        return parseCmdLine(args);
    }

    bool RunParams::parseCmdLine(const std::vector<std::string>& args)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "parseCmdLine(args)");

        // First arg is USUALLY the program name

        unsigned int i = 0;

        if (args.size() > 0)
        {
            if (args[0].size() > 0)
            {
                if (args[0][0] != '-')
                {
                    set(RPARAM_PROGNAME, args[0].c_str());
                    i = 1;
                }
            }
        }

        // The rest should be standard parameters

        std::string name;
        std::string value;
        const char* str;

        // Do the DebugUtil stuff in order to avoid unneccessary files being
        // created or disabled debug level messages being logged.
        std::string debugDisabledName;
        std::string debugDisabledValue;
        std::string debugLevelFileName;
        std::string debugLevelFileValue;
        std::string debugPidFileName;
        std::string debugPidFileValue;
        std::string debugFileName;
        std::string debugFileValue;

        for (; i < args.size(); i++)
        {
            str = args[i].c_str();

            if (!extractArg(str, name, value))
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "parseCmdLine(): failed");
                return false;
            }

            if (!name.empty())
            {
                // Store these and do them last
                if (name == "DebugFile")
                {
                    debugFileName = name;
                    debugFileValue = value;
                }
                else if (name == "DebugLevelOff")
                {
                    debugDisabledName = name;
                    debugDisabledValue = value;
                }
                else if (name == "DebugLevelFile")
                {
                    debugLevelFileName = name;
                    debugLevelFileValue = value;
                }
                else if (name == "DebugPidFilenames")
                {
                    debugPidFileName = name;
                    debugPidFileValue = value;
                }
                else if (name == "RunParamFile")
                {
                    if (value.empty())
                    {
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "RunParamFile Filename can not be null");
                        return false;
                    }

                    try
                    {
                        // open up the run param file
                        std::ifstream inFile(value.c_str());

                        if (!inFile.is_open())
                        {
                            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Unable to open %s", value.c_str());
                            return false;
                        }

                        // get pointer to associated buffer object
                        std::filebuf* buf = inFile.rdbuf();

                        // get file size using buffer's members
                        size_t size = buf->pubseekoff(0, std::ios::end);
                        buf->pubseekpos(0);

                        // allocate memory to contain file data
                        char* buffer = new char[size + 1];

                        for (size_t i = 0; i < size + 1; ++i)
                        {
                            buffer[i] = 0;
                        }

                        // get file data
                        buf->sgetn(buffer, size);
                        inFile.close();
                        std::string additionalCommand(buffer);
                        delete []buffer;
                        // replace \t\r\n with just a space so that we could parse it like normal command line
                        //std::replace_if(additionalCommand.begin(), additionalCommand.end(), isspace, ' '); // crash on chinese characters
                        additionalCommand = boost::regex_replace(additionalCommand, boost::regex("[\\n\\t]"), " ");

                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Command line in the file is %s", additionalCommand.c_str());

                        if (!parseCmdLine(additionalCommand.c_str()))
                        {
                            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Fail to parse the command line in the file");
                            return false;
                        }
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "Encountered error while reading from the file");
                        return false;
                    }
                }
                else
                {
                    set(name.c_str(), value.c_str());
                }
            }
        }

        // ++AZ TES ID 9968
        // Set the debug parameters in order
        if (!debugPidFileName.empty())
        {
            if (!debugPidFileValue.empty())
            {
                set(debugPidFileName.c_str(), debugPidFileValue.c_str());
            }
        }

        if (!debugDisabledName.empty())
        {
            if (!debugDisabledValue.empty())
            {
                set(debugDisabledName.c_str(), debugDisabledValue.c_str());
            }
        }

        if (!debugLevelFileName.empty())
        {
            if (!debugLevelFileValue.empty())
            {
                set(debugLevelFileName.c_str(), debugLevelFileValue.c_str());
            }
        }

        if (!debugFileName.empty())
        {
            if (!debugFileValue.empty())
            {
                set(debugFileName.c_str(), debugFileValue.c_str());
            }
        }

        // AZ++

        if (isSet(RPARAM_VERSION))
        {
            TA_Base_Core::Version::displayVersionInfo();

            return false;
        }

        return true;
    }

    bool RunParams::parseCmdLineAndReturnIt(const std::string& cmdline, std::map<std::string, std::string>& parameters)
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "parseCmdLineAndReturnIt");

        // Convert the cmdline to a vector of string arguments and then parse
        // the args themselves

        std::vector<std::string> args;
        convertCmdLine(cmdline.c_str(), args);

        // First arg is USUALLY the program name
        unsigned int i = 0;

        if (args.size() > 0)
        {
            if (args[0].size() > 0)
            {
                if (args[0][0] != '-')
                {
                    parameters[RPARAM_PROGNAME] = args[0];
                    i = 1;
                }
            }
        }

        // The rest should be standard parameters
        std::string name;
        std::string value;
        const char* str;

        for (; i < args.size(); i++)
        {
            str = args[i].c_str();

            if (!extractArg(str, name, value))
            {
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, "parseCmdLineAndReturnIt(): failed");
                LOG(SourceInfo, DebugUtil::FunctionExit, "parseCmdLineAndReturnIt");
                return false;
            }

            if (!name.empty())
            {
                parameters[name] = value;
            }
        }

        LOG(SourceInfo, DebugUtil::FunctionExit, "parseCmdLineAndReturnIt");
        return true;
    }

    bool RunParams::checkUsage(const char* usage, std::string& error)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "CheckUsage()");

        TA_ASSERT(usage != NULL, "Usage passed was NULL");

        error = "";

        // Split the usage string into whitespace-separated arguments

        std::vector<std::string> args;
        convertCmdLine(usage, args);

        if (args.size() == 0)
        {
            error = "You have a bad usage string!";
            return false;
        }

        std::string name;
        std::string value;
        unsigned int i = 0;
        bool optional;

        // First argument may be the program name (optional), in which case
        // we ignore it

        if (args[i].size() > 0 && args[i][0] != '-' && args[i][0] != '[')
        {
            i++;    // skip first arg
        }

        // Extract the name/value components of each argument. The whole name=value
        // argument can be surrounded by square brackets, in which case it is
        // optional

        for (; i < args.size(); i++)
        {
            if (!extractArg(args[i].c_str(), name, value, optional))
            {
                error = "You have a bad usage string!";
                return false;
            }

            // blank name is okay as long as there is no value

            if (name.empty())
            {
                if (value.empty())
                {
                    continue;   // extract the next argument
                }

                error = "You have a bad usage string!";
                return false;
            }

            // If the parameter has not been set then we have an error, unless
            // the parameter is optional (in which case we can just skip to the
            // next argument)

            if (!isSet(name.c_str()))
            {
                if (optional)
                {
                    continue;       // flies in the buttermilk...
                }

                error = "Parameter ";
                error += name;
                error += " has not been specified";
                return false;
            }

            if (value.empty())      // this is a flag parameter
            {
                value = get(name.c_str());

                if (!value.empty())
                {
                    error = "A value has been specified for flag parameter ";
                    error += name;
                    return false;
                }
            }
        }

        return true;
    }

    bool RunParams::extractArg(const char* str, std::string& name, std::string& value)
    {
        bool status = true;
        name = "";
        value = "";

        // Skip leading whitespace

        while (isspace(*str))
        {
            str++;
        }

        // Expect the name to start with two hyphens followed by an alpha

        if (strncmp(str, "--", 2) == 0)
        {
            str += 2;
        }
        else
        {
            status = false;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "arg '%s' not begins with --", str);
        }

        if (!isalpha(*str))
        {
            status = false;
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "arg '%s' not begins with alphabet: %d", str, (int)str[0]);
        }

        // Extract the name. Hyphens are skipped but cause the next character
        // to be upshifted

        bool upshift = true;

        while (status && *str != '\0' && *str != '=' && !isspace(*str))
        {
            if (*str == '-')
            {
                upshift = true;
            }
            else if (upshift)
            {
                name += toupper(*str);
                upshift = false;
            }
            else
            {
                name += *str;
            }

            str++;
        }

        // If we have an "=" then extract the value

        if (status && *str == '=')
        {
            str++;

            while (*str != '\0')
            {
                value += *str;
                str++;
            }
        }

        return status;
    }

    bool RunParams::extractArg(const char* str, std::string& name, std::string& value, bool& optional)
    {
        optional = false;

        // If the first non-white character is an opening square bracket then the
        // parameter declaration to follow is optional

        while (isspace(*str))
        {
            str++;
        }

        if (*str == '[')
        {
            optional = true;
            str++;
        }

        // Extract the argument from the given string

        if (!extractArg(str, name, value))
        {
            return false;
        }

        // Ignore if the name is empty

        if (name.empty())
        {
            return true;
        }

        // If we have an optional parameter then make sure we have a matching
        // bracket at the end of the name component (or at the end of the value
        // if a value has been specified)

        if (optional)
        {
            std::string::size_type i = value.size();

            if (i > 0)
            {
                // trim closing brace from value

                i--;

                if (value[i] != ']')
                {
                    return false;       // no matching brace
                }

                value.erase(i);
            }
            else
            {
                // trim closing brace from name

                i = name.size() - 1;

                if (name[i] != ']')
                {
                    return false;       // no matching brace
                }

                name.erase(i);
            }
        }

        return true;
    }

    void RunParams::convertCmdLine(int argc, char* argv[], std::vector<std::string>& args)
    {
        args.clear();

        for (int i = 0; i < argc; i++)
        {
            std::string arg = argv[i];
            boost::trim(arg);

            if (arg.size())
            {
                args.push_back(arg);
            }
        }
    }

    void RunParams::convertCmdLine(const char* cmdline, std::vector<std::string>& args)
    {
        TA_ASSERT(cmdline != NULL,  "Command line passed was NULL");

        args.resize(0);

        // Spaces are allowed in the value part of the argument.

        std::string arg;
        bool inValue;           // True if we're up to the "value" part of the arg.
        char nextChar;          // The next character in the command line.
        bool reachedDoubleDash; // True if we've reached a '--'.

        while (*cmdline != '\0')
        {
            inValue = false;
            nextChar = '\0';
            arg = "";
            reachedDoubleDash = false;

            // Skip leading whitespace
            while (isspace(*cmdline))
            {
                cmdline++;
            }

            if (*cmdline != '\0')
            {
                nextChar = *(cmdline + 1);
            }

            // The current argument ends when we reach the end of the
            // command line, or we reach a space when processing the name of
            // the arg, or when we reach a -- in the value part of the arg.
            while (*cmdline != '\0' && ((!inValue && !isspace(*cmdline))
                                        || inValue && !reachedDoubleDash))
            {
                if (*cmdline == '=')
                {
                    inValue = true;
                }

                arg += *cmdline;
                cmdline++;

                if (*cmdline != '\0')
                {
                    nextChar = *(cmdline + 1);
                }

                reachedDoubleDash = (*cmdline == '-' && nextChar == '-');
            }

            // Trim trailing whitespace
            std::string::size_type i = arg.find_last_not_of(" ");
            std::string trimmedArg = arg.substr(0, i + 1);

            if (!arg.empty())
            {
                args.push_back(trimmedArg);
            }
        }
    }

    unsigned int getRunParamValue(const std::string& name, unsigned int defaultVal)
    {
        unsigned int value = defaultVal;

        if (RunParams::getInstance().isSet(name.c_str()))
        {
            // TODO LPP: strtoul() would be more robust, as it can indicate that the conversion failed
            // (whereas atoi simply returns 0).
            value = atoi(RunParams::getInstance().get(name.c_str()).c_str());
        }

        return value;
    }

    std::string getRunParamValue(const std::string& name, const std::string& defaultVal)
    {
        if (RunParams::getInstance().isSet(name.c_str()))
        {
            std::string value = RunParams::getInstance().get(name.c_str());

            if (value.size())
            {
                return value;
            }
        }

        return defaultVal;
    }

    void parseLocalConfigurationFile()
    {
        boost::filesystem::path configFile = boost::filesystem::system_complete(RunParams::getInstance().get(RPARAM_ENTITYNAME) + ".cfg");

        if (!boost::filesystem::exists(configFile))
        {
            configFile = boost::filesystem::system_complete(RunParams::getInstance().get(RPARAM_ENTITYNAME) + ".ini");
        }

        if (boost::filesystem::exists(configFile) && boost::filesystem::is_regular_file(configFile))
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "parseLocalConfigurationFile: %s", configFile.string().c_str());

            try
            {
                std::stringstream paramStrm;
                paramStrm << "--RunParamFile=" << configFile.string();
                RunParams::getInstance().parseCmdLine(paramStrm.str().c_str());
            }
            catch (std::exception& e)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "parseLocalConfigurationFile - error: %s", e.what());
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "parseLocalConfigurationFile - unknown exception");
            }
        }
    }
}// Closes namespace TA_Base_Core

#if defined( WIN32 )
    #pragma warning( pop )
#endif  // defined( WIN32 )
