
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/command_example/src/CommandExampleSimulationModule.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * Definition of the CommandExampleSimulationModule class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/examples/command_example/src/CommandExampleSimulationModule.h"
#include "bus/generic_server_simulator/examples/command_example/src/CommandExampleCommsObserver.h"

// #include "core/utilities/src/DebugUtil.h"

using namespace TA_Base_Bus;


CommandExampleSimulationModule::CommandExampleSimulationModule
(   const std::string & name,
    const std::string & host,
    const PortList & ports
)
: GssSimulationModule( name, host, ports )
{
}


GssCommsObserver * CommandExampleSimulationModule::createCommsObserver( GssCommsObserver::ObservedSocketType * socket )
{
    return new CommandExampleCommsObserver( socket );
}


std::string CommandExampleSimulationModule::getSimDescription()
{
    return std::string( "Command Example Simulation Module" );
}

