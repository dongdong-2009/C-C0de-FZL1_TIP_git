/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/simulator/example/src/main.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef WIN32
#pragma warning (disable : 4786)
#endif // #ifdef WIN32

//System Includes
#include <iostream>

//TransActive Includes
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/TransactiveException.h"

//Threads to be started
#include "bus/generic_server_simulator/src/GenericSimulator.h"
#include "bus/generic_server_simulator/src/GssCommandMonitor.h"
#include "bus/generic_server_simulator/src/GssUtility.h"
#include "bus/generic_server_simulator/examples/echo_server/src/EchoModuleCreator.h"
//#include "bus/generic_server_simulator/examples/command_example/src/CommandExampleModuleCreator.h"

using TA_Base_Core::DebugUtil;
using namespace TA_Base_Bus;

int main(int argc, char* argv[])
{

    try
    {
        if( GssUtility::initialiseUtilities(argc, argv) )
        {
            srand(0);		//seed the random number generator

            //
            // Module Creator instantiation
            //
            new TA_Base_Bus::EchoModuleCreator("ECHO");

            TA_Base_Bus::GenericSimulator * simulator = new TA_Base_Bus::GenericSimulator();

            GssCommandMonitor * monitor = new GssCommandMonitor("Generic Server Simulator", simulator);
            monitor->monitorUserInput();

            //once the command monitor returns (user has typed exit), kill the simulator 
            std::cout << "Shutting down simulator..." << std::endl;
            delete simulator;

            std::cout << "done." << std::endl;
        }
    }
    catch (TA_Base_Core::TransactiveException te)
    {
        std::cerr << "Transactive Exception caught: " << te.what() << std::endl;
    }
    catch (...)
    {
        std::cerr << "Non-TA Exception caught" << std::endl;
        throw;
    }

    return false;

	return 0;
}

