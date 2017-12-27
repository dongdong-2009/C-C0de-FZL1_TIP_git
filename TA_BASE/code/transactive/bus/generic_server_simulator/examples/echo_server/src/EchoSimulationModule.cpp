
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/echo_server/src/EchoSimulationModule.cpp $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * Definition of the EchoSimulationModule class.
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/examples/echo_server/src/EchoSimulationModule.h"
#include "bus/generic_server_simulator/examples/echo_server/src/EchoCommsObserver.h"



using namespace TA_Base_Bus;


EchoSimulationModule::EchoSimulationModule
(   const std::string & name,
    const std::string & host,
    const PortList & ports
)
: GssSimulationModule( name, host, ports )
{
}


GssCommsObserver * EchoSimulationModule::createCommsObserver( GssCommsObserver::ObservedSocketType * socket )
{
    return new EchoCommsObserver( socket );
}


std::string EchoSimulationModule::getSimDescription()
{
    return std::string( "Echo Server" );
}

