#if !defined(IACTION_H_INCLUDED)
#define IACTION_H_INCLUDED

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IAction.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IAction is an interface to a action object. It allows the action object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  */

#include <string>
#include <vector>

namespace TA_Base_Core
{
    class IAction
    {

    public:
        virtual ~IAction(){}

        /**
         * getKey
         *
         * Returns the key for this Action.
         *
         * @return The key for this action as an unsigned long.
         */
        virtual unsigned long getKey() =0;

        /**
         * getName
         *
         * Returns the name of this Action. If this is the first time data for this action
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this action as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName() =0;


        /**
         * getDescription
         *
         * Returns the description of this Action. If this is the first time data for this action
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this action as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription() =0;
        
        
        /**
         * getActionGroups
         *
         * This method retrieves the ActionGroup that this action has.
		 *
         *
         * @return A std::vector<std::string> containing all the ActionGroup
		 *         names. A std::auto_ptr<std::vector<std::string> > of size
		 *         0 will be returned if no ActionGroups are configured for the resource/action
		 *         combination.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<std::string> getActionGroups() =0;

        /**
         * invalidate
         *
         * Mark the data contained by this action as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

		virtual bool isDisplayOnly()=0;
    };
} //close namespace TA_Base_Core

#endif // !defined(IACTION_H_INCLUDED)
