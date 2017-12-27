#ifndef ISimulationModule_H
#define ISimulationModule_H
/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/ISimulationModule.h $
  * @author:  KL Team
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  *
  */

#include <string>

namespace TA_Base_Bus
{
    class ISimulationCommandProcessor;

    /**
      * Abstract base class for a class that is controlled by the
      * GenericSimulator class.
      */
    class ISimulationModule
    {
        public:
            /** start the module */
            virtual void startSim() = 0;


            /** stop (pause) the module */
            virtual void stopSim() = 0;

            
            /** restart the module from whatever constitutes "the beginning".
              * It should reinitialise itself as appropriate.
              */
            virtual void restartSim() = 0;


            /** As modules are probably threaded, their destructor is not
              * intended to be called directly.  Instead, call destroySim()
              * and suppose it will clean itself up properly.
              */
            virtual void destroySim() = 0;


            /** this is an informative one-line string so we can have something
              * meaningful to display in the command monitor.
              *
              * @return the brief description of this simulation module instance
              */
            virtual std::string getSimDescription() = 0;


            /** Accessor for the name for this simulation module instance. The
              * name should not contain any spaces
              *
              * @return the name of this simulation module instance
              */
            virtual std::string getName() = 0;


            /** Hook method used by CommandMonitor::parseCommand to retrieve
              * the class that processes the commands. 
              *
              * It is assumed that derived SimulationModule classes should
              * simply return the data member of the command processor.
              * 
              * @return reference to this SimulationModule's CommandProcessor
              */
            virtual ISimulationCommandProcessor & getCommandProcessor() = 0;


            /** Virtual destructor */
            virtual ~ISimulationModule(){ };

    }; // class ISimulationModule
} // namespace TA_Base_Bus

#endif // #ifndef ISimulationModule_H
