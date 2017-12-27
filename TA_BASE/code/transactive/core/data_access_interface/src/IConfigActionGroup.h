/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigActionGroup.h $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * IConfigActionGroup is an interface to a ConfigActionGroup object. It allows the ActionGroup object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  */

#if !defined(IConfigActionGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigActionGroup_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IActionGroup.h"

namespace TA_Base_Core
{

    class IConfigActionGroup : public IActionGroup, virtual public IConfigItem
    {

    public:
        virtual ~IConfigActionGroup() { };

		
        /**
         * setIsControlType
         *
         * Sets whether or not the isControlType flag is set in the database.
         *
         * @param boolean - True if the ActionGroup is a control type. False otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void setIsControlType(bool isControlType) = 0;

        /**
         * addAction
         *
         * Adds a new action to the ActionGroup
         *
         * @param unsigned long - The key of the action.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void addAction(unsigned long actionKey) =0;


        /**
         * removeAction
         *
         * Removes an action from the ActionGroup type
         *
         * @param unsigned long - The key of the action.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeAction(unsigned long actionKey) =0;


        /**
         * isActionIncluded
         *
         * This checks to see if the specified actionkey is currently included in the list of actions
         * for this ActionGroup.
         *
         * @param unsigned long - The action key to check for
         *
         * @return bool - This will return true if the actionkey is associated with this ActionGroup and false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isActionIncluded(unsigned long actionKey) =0;

        
        /**
         * getDateCreated
         *
         * Returns the date created for this location
         *
         * @return The date created for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateCreated() =0;


        /**
         * getDateModified
         *
         * Returns the date modified for this location.
         *
         * @return The date modified for this location as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        virtual time_t getDateModified() =0;


        /**
         * deleteThisActionGroup
         *
         * Removes this ActionGroup from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ActionGroup was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This ActionGroup has not been deleted
         */
        virtual void deleteThisActionGroup() =0;


    };
} //close namespace TA_Base_Core

#endif // !defined(IConfig_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
