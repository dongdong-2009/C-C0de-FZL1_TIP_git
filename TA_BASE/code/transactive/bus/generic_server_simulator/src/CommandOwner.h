#ifndef CommandOwner_H
#define CommandOwner_H
/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/generic_server_simulator/src/CommandOwner.h $
  * @author:  Robert van Hugten
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  */
#include <map>
#include <string>


namespace TA_Base_Bus
{
    ///
    class ICommand;

    ///
    typedef std::map< std::string, ICommand * > CommandMap;

    /** Base class for objects that own ICommands. Typically, you will not
      * need to derive from this yourself since the IntermediateCommand and
      * GssCommandProcessor classes (which you would normally derive from)
      * are CommandOwners.
      */
    class CommandOwner
    {
	public:
        /** Used to add a command to the map. Called during ICommand
          * construction, so is not necessary to call explicitly
          *
          * @param cmd the Hierarchical command.
          */
        void addCommand(ICommand * cmd);

    protected:
        /** Default constructor */
        CommandOwner();

        /** Virtual destructor */
        virtual ~CommandOwner();

        /** Fascade accessor for m_commands.find().second
          * 
          * @param name the name of the command to find
          * @return pointer to the found command if it exists, nil otherwise
          */
        ICommand * find(const std::string & name) const;

        /** Accessor for the entire map.
          *
          * TODO: provide fascade methods for common operations, like find()
          * (already provided), but also first() and last(), since full access
          * shouldn't really be either necessary or allowed
          *
          * @return const ref to the m_commands data member
          */
        const CommandMap & getCommands() const;

    private:
        /** Container of ICommand pointers, key'd by command name.
          */
        CommandMap m_commands;
    }; // class CommandOwner

}//close namespace TA_Base_Bus
#endif // CommandOwner_H
