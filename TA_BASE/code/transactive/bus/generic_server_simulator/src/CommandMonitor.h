#ifndef CommandMonitor_H
#define CommandMonitor_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/CommandMonitor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Based heavily on the original bus/generic_simulator/src/CommandMonitor.h
  *
  * CommandMonitor provides a new thread for the simulator, to listen for and 
  * respond to user commands in runtime. Modifications from the original include:
  * - Uses ostreams to print stuff to, so that stuff can be sent over TCP/IP or
  *   to a GUI
  * - Uses GssUtility functions for parsing and tokenising.
  * - lotsa javadoc/doxygen friendly comments
  * - refactored command handling
  */
#include <string>
#include "core/synchronisation/src/ReEntrantThreadLockable.h"

namespace TA_Base_Bus
{
    class ISimulationModule;
    class GenericSimulator;
    class GenericCommandProcessor;

    class CommandMonitor  
    {
        public:
            /** Constructor.
              * 
              * @param desc the description of this..erm..wait, i'll go find out
              * @param simulator the GenericSimulator that owns this CommandMonitor
              */
            CommandMonitor(const std::string & desc, GenericSimulator * simulator);

            /** Aaaahhh. Virtual destruction.
              */
            virtual ~CommandMonitor();

            /** The main loop function. Accepts user input with std::cin.getline()
              * and executes the parseCommand() method.
              *
              * @return true. Always. TODO: return something useful
              */
            virtual bool monitorUserInput();


            /** This method reads the specified script file and executes the
              * parseCommand() method for each line.
              *
              * This method may be called recursively by another script.
              *
              * @param filename the path and name of the script file
              * @return true if the execution was successful, false otherwise
              */
            virtual bool executeScript(const std::string& filename);

            /** Override this method to create the supported simulation classes
              *
              * For the GenericeServerSimulator this is overridden by
              * GssCommandMonitor, override the GssModuleCreator to create your
              * derived ISimulationModule. See
              *
              * code/bus/generic_server_simulator/examples/echo_server
              *
              * for an example of how it works
              */
            virtual ISimulationModule* createSimulationModule(const std::string& type,
                                                              const std::string& name,
                                                              const std::string& parameters)=0;

        private:
            // NOTE:
            // making private methods virtual may seem pointless, but remember
            // that the access control is enforced at compile time, whereas the
            // virtual despatch (a.k.a. polymorphism) is a runtime feature.  So
            // overriding these private functions will work as expected.

            /**
              * The original comment from generic_simulator:
              * This method takes the input string from the user and attempts to
              * create a new ICommand object based on that instruction.  It
              * splits the command string into a name/value pair and calls
              * CommandFactory::createCommand.
              *
              * A more likely comment:
              * This method takes the input string, splits into name and
              * parameters. Checks if the name is a valid command with this
              * CommandMonitor (by calling doCommand()) and then the current
              * simulation module's CommandProcessor.
              *
              * @param inputString the command line
              * @return false if the command could not be parsed, otherwise the
              *         result of executing the command.
              */
            virtual bool parseCommand(std::string& inputString);

            /** Retrieves a list of the modules.
              */
            virtual void listModules();

            /** implements the "module <name>" command
              *
              * @param moduleName the name of the module to switch to, ie.
              *        make active, the focus/destination of commands
              * @return true if a module with the given name exists, false otherwise
              */
            virtual bool switchModule(const std::string & moduleName);

            /** Prints the usage of all commands from this CommandMonitor and
              * active module's
              */
            virtual void printHelp();

            /** Called by parseCommand to initially attempt to handle commands.
              * Demultiplexes various commands to the other doXXX functions in
              * this class.
              *
              * @param cmdName the name of the command
              * @param cmdValue a string with any further paramters
              * @param wasHandled a return parameter set to true if the command
              *         was handled by this function
              * @return the return value of the command if executed, false otherwise
              */
            virtual bool doCommand(const std::string & cmdName, const std::string & cmdValue, bool & wasHandled);

            /** Implements the "exec <filename>" command */
            virtual bool doExec(const std::string& cmdValue);

            /** Implements the "start" command */
            virtual bool doStart(const std::string& cmdValue);

            /** Implements the "restart" command */
            virtual bool doRestart(const std::string& cmdValue);

            /** Implements the "stop" command */
            virtual bool doStop(const std::string& cmdValue);

            /** Implements the "sleep <msecs>" command */
            virtual bool doSleep(const std::string& cmdValue);

            /** Implements the "addmod <name> <type>" command */
            virtual bool doAddmod(const std::string& cmdValue);

            /** Implements the "reset" command */
            virtual bool doReset(const std::string& cmdValue);

            /** Prints usage info for the "addmod" command */
            void addmodUsage();

            /// The description of the simulator
            std::string         m_desc;

            /// the current, or active, simulation module. The module to which
            /// commands are directed
            ISimulationModule * m_currentModule;

            /// The owning simulator
            GenericSimulator  * m_simulator;

            /// flag to indicate that the simulator is to quit
            bool m_doQuit;

            ///
            /// Lock for a ThreadGuard to only allow one thread at a time to
            /// parse a command.
            TA_Base_Core::ReEntrantThreadLockable m_commandLock;

            ///
            /// CommandProcessor for our own internal commands
            GenericCommandProcessor * m_commandProcessor;

    }; // class CommandMonitor
} // namespace TA_Base_Bus

#endif // #ifndef CommandMonitor_H

