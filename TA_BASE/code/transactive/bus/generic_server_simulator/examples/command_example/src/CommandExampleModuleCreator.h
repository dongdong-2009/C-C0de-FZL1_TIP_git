/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/examples/command_example/src/CommandExampleModuleCreator.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for the CommandExampleModuleCreator class
  */
#ifndef CommandExampleModuleCreator_H
#define CommandExampleModuleCreator_H

#ifdef WIN32
#pragma warning(disable : 4786)
#pragma warning(disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/GssAbstractModuleCreator.h"

namespace TA_Base_Bus
{

    class ISimulationModule;

    class CommandExampleModuleCreator : public GssAbstractModuleCreator
    {

    public:
        virtual ~CommandExampleModuleCreator();

        virtual ISimulationModule * createSimulationModule(const std::string & name, const std::string & parameters);

        CommandExampleModuleCreator(const std::string & type);

    }; // class CommandExampleModuleCreator

} // namespace TA_Base_Bus

#endif // #ifndef CommandExampleModuleCreator_H
