#ifndef GENERIC_SERVER_SIMULATOR_COMMAND_PROCESSOR_H
#define GENERIC_SERVER_SIMULATOR_COMMAND_PROCESSOR_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/GssCommandProcessor.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  */
#include <string>
#include <vector>

#include "bus/generic_server_simulator/src/ISimulationCommandProcessor.h"
#include "bus/generic_server_simulator/src/CommandOwner.h"


namespace TA_Base_Bus
{
	class GssSimulationModule;

    /**
      * GssCommandProcessor implements ISimulationCommandProcessor,
      * for use with the generic simulator. It also derives from CommandOwner
      * so as to ... be able to seamlessly own ICommands
      *
      * There is no default constructor, as this is intended to be instantiated
      * by the parent simulator
      */
    class GssCommandProcessor
        : public ISimulationCommandProcessor
        , public CommandOwner
    {
    public:

        /**
          * Preferred constructor
          *
          * @param parentSimulator the owner of this object
          */
        GssCommandProcessor( GssSimulationModule * parentSimulator );

        /**
          * Preferred constructor
          *
          * @param parentSimulator the owner of this object
          */
        virtual ~GssCommandProcessor();


        //////////////////////////////////////////////////////////////////////////////
        //                                                                          //
        //                    ISimulationCommandProcessor methods                   //
        //                                                                          //
        //////////////////////////////////////////////////////////////////////////////

        /**
          * hasCommandName
          * 
          * hasCommandName indicates whether or not the CommandProcessor has a command
          * with the name specified in the name parameter.  Returns true if a command
          * does exist with that name, false otherwise.
          *
          * @param std::string &name The name of the command to search for
          * @return bool hasCommand True if the command exists, false otherwise.
          */
        bool hasCommandName(const std::string &name);


        /**
          * execute
          * 
          * performs the processing required to run the command specified in the name parameter.
          * The params string is a space separated list of parameters to the command.
          *
          * @param std::string &name The name of the command to execute
          * @param std::string &params Parameters to the command
          * @return bool successful True if the command executes successfully, false otherwise.
          */
        bool execute(const std::string &name, const std::string &params);


        /**
          * errorMsg
          * 
          * returns the error message to be displayed if the command fails to execute successfully
          *
          * @return std::string errorMsg The error message
          */
        std::string errorMsg();


        /**
          * usageString
          * 
          * returns the usage string to be displayed if the command is called with incorrect parameters
          *
          * @return std::string usageString The usage string
          */
        std::string usageString(const std::string &name);



        private:
        /** Upward navigation. Holds the instance of the parent simulator.  */
        GssSimulationModule * m_parentSimulator;

        
        /** 
          * updated every time a command is executed.
          */
        std::string m_errMsg;

    }; // class GssCommandProcessor
}// namespace TA_Base_Bus

#endif // GENERIC_SERVER_SIMULATOR_COMMAND_PROCESSOR_H
