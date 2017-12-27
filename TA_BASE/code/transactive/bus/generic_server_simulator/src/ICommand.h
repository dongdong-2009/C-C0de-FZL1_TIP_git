#ifndef ICommand_H
#define ICommand_H


/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/ICommand.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  */
#include "bus/generic_server_simulator/src/CommandOwner.h"
#include "bus/generic_server_simulator/src/GssCommon.h"


namespace TA_Base_Bus
{
    /**
      * ICommand is an abstract base class which allows for containment of
      * simulator commands in a CommandOwner.
      *
      * The hasSubCommand allows implementing commands hierarchically. That is,
      * a command may own other ICommands. The IntermediateCommand class
      * implements sub command functionality.  To implement a command, derive
      * from LeafCommand.
      *
      * There is no default constructor, as this should only be instantiated by
      * the owning simulator (or CommandOwner), which passes itself as the
      * parent pointer. The second parameter may be provided either by the
      * owner or the LeafCommand
      */
    class ICommand
    {
    public:

        /**
          * Constructor
          */
        ICommand(CommandOwner * owner, const char * name);

        /**
          * destructor
          */
        virtual ~ICommand( );


        /** Indicates whether or not this ICommand is the command
          * with the specified name.
          *
          * @param name the std::string name of the command to indicate
          * @return bool true if this is the command with that name, false otherwise.
          */
        virtual bool isCommand(const std::string & name) const = 0;


        /** Identifies if this command has a further subcommand of the given name
          *
          * @param name the std::string name of the sub command to search for
          * @return true if the sub-command exists, false otherwise.
          */
        virtual bool hasSubCommand(const std::string & name) const = 0;


        /**
          * The primary function for which this class exists. Causes this
          * instance to act on the arguments to produce a relevant UserQuery
          *
          * @param args the list of pre-tokenised arguments
          * @param argstr the original un-tokenised argument string
          * @return bool true if execution was successful, false otherwise
          */
        virtual bool execute(TokenList & args, const std::string & argstr) = 0;

        /** Returns the usage string. Displayed if the command fails
          * execution.
          *
          * @return std::string usageString The usage string
          */
        virtual void usage(const std::string & name) const = 0;

        /** Accessor for the name of this command. Only returns this node of
          * the command "hierarchy" - if the command is a subcommand, it does
          * not return the full command name.
          *
          * @return the name of this command
          */
        virtual const std::string & getName() const;

    private:
        ///
        /// The name of this command [node].
        std::string m_name;

        ///
        /// This command's owner
        CommandOwner * m_owner;
    };


    inline ICommand::ICommand(CommandOwner * owner, const char * name)
        : m_name(name)
        , m_owner(owner)
    {
        owner->addCommand(this);
    }

    inline ICommand::~ICommand()
    {
    }

    inline const std::string & ICommand::getName() const
    {
        return m_name;
    }
} // namespace TA_Base_Bus
#endif // ICommand_H
