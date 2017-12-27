/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/VersionCommand.cpp $
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

#include "bus/generic_server_simulator/src/VersionCommand.h"
#include "core/versioning/src/Version.h"
#include <iostream>

using namespace TA_Base_Bus;


VersionCommand::VersionCommand(CommandOwner * owner)
: LeafCommand(owner, "version")
{
}

bool VersionCommand::execute(TokenList & args, const std::string & argstr)
{
    //
    // basic argument checking
    if (args.size() > 1)
    {
        std::cerr << "WARNING: too many parameters. Expected 0, got "
                  << args.size() << ": " << argstr << std::endl;
    }
    
    TA_Base_Core::Version::displayVersionInfo();

    return true;
}


void VersionCommand::usage(const std::string & name) const
{
    std::cout << name << " - display simulator version information" << std::endl;
}

