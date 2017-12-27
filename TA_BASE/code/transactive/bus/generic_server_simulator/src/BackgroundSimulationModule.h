#ifndef BackgroundSimulationModule_H
#define BackgroundSimulationModule_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/BackgroundSimulationModule.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for the BackgroundSimulationModule class
  */


#include "bus/generic_server_simulator/src/GssSimulationModule.h"

namespace TA_Base_Bus
{
	class BackgroundCommsObserver;

    /**
      * BackgroundSimulationModule implements GssSimulationModule, for use with the
      * generic server simulator.
      */
    class BackgroundSimulationModule
		: public GssSimulationModule
    {

    public:
        /// Constructor
        BackgroundSimulationModule(const std::string &name, const std::string & host, const GssSimulationModule::PortList & ports);


        /// Destructor
        ~BackgroundSimulationModule();


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

    private:

        /** preLoop
          *
          * One of three hook functions. Called within run(), before any of the
          * sockets are checked for accepting a connection.
          *
          * Our purpose for overriding this is to periodically send everything
          * in stdout and stderr to all comms observers.
          */
        void preLoop();

        /** adds the passed in CommsObserver to the vector of CommsObserver objects held by the 
          * simulator. Overridden here to put a CR to the WriteEnd of the stdin
          * stream after the initial terminal output.
          *
          * @param CommsObserver* com A pointer to the CommsObserver object to be added
          */
        virtual void addCommsObserver( GssCommsObserver * com); 
    }; // class BackgroundSimulationModule

} // namespace TA_Base_Bus

#endif // BackgroundSimulationModule_H
