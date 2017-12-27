#ifndef LeafCommand_H
#define LeafCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/LeafCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for classes used by the GenericServerSimulator's
  * GssCommandProcessor to handle sub commands, being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/ICommand.h"

namespace TA_Base_Bus
{

    /** Leaf node "base" class.  Of the command hierarchy, that is.
      * Leaves execute() and usage() pure for concrete subclasses.
      */
    class LeafCommand : public ICommand
    {
    public:
        LeafCommand(CommandOwner * owner, const char * name);

        bool isCommand(const std::string & name) const;

        bool hasSubCommand(const std::string & name) const;

    protected:
        //
        // insert here useful methods for LeafCommand classes
        //

    }; // class LeafCommand


} // namespace TA_Base_Bus

#endif // #ifndef LeafCommand_H
