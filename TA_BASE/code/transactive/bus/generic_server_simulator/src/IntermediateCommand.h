#ifndef IntermediateCommand_H
#define IntermediateCommand_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/IntermediateCommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Declaration file for classes used by ATSSimulator to handle sub commands,
  * being 
  */

#ifdef WIN32
#pragma warning (disable : 4786)
#pragma warning (disable : 4284)
#endif // #ifdef WIN32

#include "bus/generic_server_simulator/src/ICommand.h"

namespace TA_Base_Bus
{

    /** Intermediate node of the command hierarchy
      */
    class IntermediateCommand
        : public ICommand
        , public CommandOwner
    {

    public:
        /** constructor */
        IntermediateCommand(CommandOwner * owner, const char * name);

        /** destructor */
        ~IntermediateCommand( );

        //////////////////////////////////////////////////////////////////
        //                                                              //
        //                ICommand methods                   //
        //                                                              //
        //////////////////////////////////////////////////////////////////

        bool isCommand(const std::string & name) const;

        bool hasSubCommand(const std::string & name) const;

        bool execute(TokenList & args, const std::string & argstr);

        void usage(const std::string & name) const;
    }; // class IntermediateCommand

} // namespace TA_Base_Bus

#endif // #ifndef IntermediateCommand_H
