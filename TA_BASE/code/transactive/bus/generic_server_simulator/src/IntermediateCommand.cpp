/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/IntermediateCommand.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/IntermediateCommand.h"

#include <stdlib.h>
#include <iostream>

using namespace TA_Base_Bus;



////////////////////////////////////////////////////////////////
//
// IntermediateCommand
//
////////////////////////////////////////////////////////////////
IntermediateCommand::IntermediateCommand(CommandOwner * owner, const char * name)
: ICommand(owner, name)
{
}


IntermediateCommand::~IntermediateCommand( )
{
}


bool IntermediateCommand::isCommand(const std::string & name) const
{
    return (name == getName());
}


bool IntermediateCommand::hasSubCommand(const std::string & name) const
{
    return (find(name) != 0);
}


bool IntermediateCommand::execute(TokenList & args, const std::string & argstr)
{
	if ( args.empty() )
	{
        std::cerr << "ERROR: \"" << getName() << "\" command requires subcommand parameters." << std::endl;
	}

    //
    // snatch the front arg, it should be the name of our subcommand
    std::string subcmdstr = args.front();
    args.pop_front();

    ICommand * subcmd = find(subcmdstr);

    //
    if ( subcmd != 0 )
    {
        return (subcmd->execute(args, argstr));
    }
	else
	{
        std::cerr << "ERROR: no subcommand \"" << subcmdstr << "\" for command " << getName() << std::endl;
	}

    return false;
}


void IntermediateCommand::usage(const std::string & name) const
{
	const CommandMap & cmds = getCommands();

    for ( CommandMap::const_iterator iter = cmds.begin() ;
            iter != cmds.end() ;
            iter++
        )
    {
        std::cout << getName() << " ";
        ((*iter).second)->usage((*iter).first);
    }
}

