/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/DetachCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Definition/Implementation file for CommandExample classes
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/DetachCommand.h"
#include "bus/generic_server_simulator/src/BackgroundSimulationModule.h"
#include "bus/generic_server_simulator/src/GssSimulationModuleFactory.h"
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "bus/generic_server_simulator/src/ConsoleDetachUtility.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#include <iostream>

using namespace TA_Base_Bus;


DetachCommand::DetachCommand(CommandOwner * owner, const char * name)
: LeafCommand(owner, name)
{
}

bool DetachCommand::execute(TokenList & args, const std::string & argstr)
{
    //
    // basic argument checking
    if (args.size() > 2)
    {
        std::cerr << "WARNING: too many parameters. Expected 2, got "
                  << args.size() << ": " << argstr << std::endl;
    }

    if (ConsoleDetachUtility::isDetached())
    {
        std::cout << "Already detached!" << std::endl;
        return true;
    }
    
    std::cout << "This will detach the simulator from the console. Is this what you want to do [y|N] ? ";
    std::cout.flush();

    char c;
    std::cin >> c;

    if ( (c!='Y') && (c!='y') )
    {
        std::cout << "Detachment cancelled" << std::endl;
        return true;
    }

    std::string s;
    TokenList parameters;
    TokenList::const_iterator argiter = args.begin();
    std::string hoststr="";
    std::string portstr="";

    while ( argiter != args.end() )
    {
        bool ok = false;
        s = *argiter;
        parameters.clear();
        GssUtility::tokenise(s.c_str(), parameters, "=");

        if (parameters.size() == 2)
        {

            std::string fieldstr = parameters.front();
            parameters.pop_front();

            std::string valuestr = parameters.front();
            parameters.pop_front();

            if (fieldstr == "host")
            {
                hoststr = valuestr;
            }
            else if (fieldstr == "port")
            {
                portstr = valuestr;
            }
            else
            {
                std::cerr << "ERROR: unrecognised " << getName() << " argument: " << s << std::endl;
            }
        }
        else
        {
            //TODO log a warning
            std::cerr << "WARNING: Could not parse. Expected \"field=value\" pair, got: " << s << std::endl;
        }

        argiter++;
    }

    GssUtility::getBackgroundHostPort(hoststr,portstr);

    ConsoleDetachUtility::detach(hoststr,portstr);

    return true;
}


void DetachCommand::usage(const std::string & name) const
{
    std::cout << name << " [host=<ipaddress>] [port=<number>]" << std::endl;
}

