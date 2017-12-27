#ifndef ISimulationCommandProcessor_H
#define ISimulationCommandProcessor_H

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/ISimulationCommandProcessor.h $
  * @author:  KL Team
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * Abstract base class for a class that interprets and processes commands as
  * required by the Simulation CommandMonitor
  *
  */

#include <string>

namespace TA_Base_Bus
{
    /** For each ISimulationModule descendant there is a corresponding
      * CommandProcessor. This can interpret text commands and set/reset
      * appropriate parameters of the module - or whatever.
      *
      * The idea is to decouple the command processing from the module itself as
      * in the future it may be desirable to construct new ways to control the
      * module - like GUIs, or a network interface
      *
      * For the GenericServerSimulator, this is implemented by the
      * GssCommandProcessor, which deals with commands by way of the ICommand
      * hierarchy
      */
    class ISimulationCommandProcessor
    {
        public:
            /** Checks to see if this CommandProcessor has a command with the
              * given name
              *
              * @param  name the name of the command to be checked for
              * @return true if a command with the given name is handled by
              *         this CommandProcessor, false otherwise
              */
            virtual bool hasCommandName(const std::string & name) = 0;


            /** Causes the command with the given name to be executed.
              *
              * @param  name the name of the command to be executed
              * @param  params a string of arguments to the command
              * @return true if the command executed successfully, false otherwise
              */
            virtual bool execute(const std::string & name, const std::string & params) = 0;


            /** Retrieves the error message for the most recently executed
              * command 
              *
              * @return the error string
              */
            virtual std::string errorMsg() = 0;


            /** Retrieves the Usage: string for the given command name
              *
              * @param name the name of the command
              * @return the usage string of the command
              */
            virtual std::string usageString(const std::string & name) = 0;


            /** Virtual destruction */
            virtual ~ISimulationCommandProcessor() { };
    };
};

#endif // #ifndef ISimulationCommandProcessor_H
