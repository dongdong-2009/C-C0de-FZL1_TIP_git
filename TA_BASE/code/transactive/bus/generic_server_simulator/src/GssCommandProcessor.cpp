/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssCommandProcessor.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include <iostream>
#include <sstream>

#include "bus/generic_server_simulator/src/GssCommandProcessor.h"
#include "bus/generic_server_simulator/src/GssSimulationModule.h"
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "bus/generic_server_simulator/src/ICommand.h"
#include "bus/generic_server_simulator/src/StatusCommand.h"

using namespace TA_Base_Bus;


GssCommandProcessor::GssCommandProcessor(GssSimulationModule * parentSimulator)
: m_parentSimulator(parentSimulator)
, m_errMsg("")
{
    // only need to make the status command if we have a parent simulator
    if ( parentSimulator != 0 )
    {
        new StatusCommand(this, parentSimulator);
    }
}

GssCommandProcessor::~GssCommandProcessor()
{

}

//////////////////////////////////////////////////////////////////////////////
//                                                                          //
//                    ISimulationCommandProcessor methods                   //
//                                                                          //
//////////////////////////////////////////////////////////////////////////////

bool GssCommandProcessor::hasCommandName( const std::string &name )
{
    if (name == "help")
    {
        return true;
    }
    else
    {
        return ( find(name) != 0 );
    }

    return false;
}


std::string GssCommandProcessor::usageString(const std::string &name)
{
    ICommand * cmd = find(name);
    if ( cmd != 0 )
    {
        std::stringstream streem;
        cmd->usage(name);

        return streem.str();
    }

    return "";
}


std::string GssCommandProcessor::errorMsg()
{
    return m_errMsg;
}


bool GssCommandProcessor::execute(const std::string & name, const std::string & params)
{

    //
    // First, cover the commands for which we have no [need for a] ICommand.
    if( name == "help" )
    {
        const CommandMap & cmds = getCommands();

        for ( CommandMap::const_iterator iter = cmds.begin() ;
                iter != cmds.end() ;
                iter++
            )
        {
            ((*iter).second)->usage((*iter).first);
        }

        return true;
    }
    else
    {
        ICommand * cmd = find(name);
        if ( cmd != 0 )
        {
            TokenList parameters;
            int numtoks = GssUtility::tokenise(params.c_str(), parameters);

            return cmd->execute( parameters, params );
        }
    }

    return false;
}

