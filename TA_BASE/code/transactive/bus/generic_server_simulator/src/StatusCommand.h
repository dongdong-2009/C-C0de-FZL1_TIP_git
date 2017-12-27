#ifndef StatusCommand_H
#define StatusCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/StatusCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for classes used by GssCommandProcessor to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/LeafCommand.h"

namespace TA_Base_Bus
{
    class GssSimulationModule;

    /** 
      * 
      * 
      */
    class StatusCommand : public LeafCommand
    {
	public:
        StatusCommand(CommandOwner * owner, GssSimulationModule * simulation_module);

        bool execute(TokenList & args, const std::string & argstr);

        void usage(const std::string & name) const;

    private:
		GssSimulationModule * m_simulationModule;
	
	}; // class StatusCommand

} // namespace TA_Base_Bus

#endif // #ifndef StatusCommand_H
