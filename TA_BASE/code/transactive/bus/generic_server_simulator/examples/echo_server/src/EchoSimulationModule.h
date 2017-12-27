#ifndef ECHO_SIMULATION_MODULE_H
#define ECHO_SIMULATION_MODULE_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/echo_server/src/EchoSimulationModule.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for the EchoSimulationModule class
  */


#include "bus/generic_server_simulator/src/GssSimulationModule.h"

#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Bus
{
	class EchoCommsObserver;

    /**
      * EchoSimulationModule implements GssSimulationModule, for use with the
      * generic server simulator.
      */
    class EchoSimulationModule
		: public GssSimulationModule
    {

    public:
        /// Constructor
        EchoSimulationModule(const std::string &name, const std::string & host, const GssSimulationModule::PortList & ports);

        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                       GssSimulationModule methods                        //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * createCommsObserver
          * 
          * Used by GssSimulationModule::run() to create a CommsObserver when a
          * new socket connection is accepted.
          *
          * @return the new'd derived GssCommsObserver class instance.
          */
		virtual GssCommsObserver * createCommsObserver( GssCommsObserver::ObservedSocketType * socket );


        /**
          * getSimDescription
          * 
          * returns the description of the simulation module, for help messages.
          *
          * @return the value of the simulatorDescription
          */
        std::string getSimDescription();

    }; // class EchoSimulationModule

} // namespace TA_Base_Bus

#endif // ECHO_SIMULATION_MODULE_H
