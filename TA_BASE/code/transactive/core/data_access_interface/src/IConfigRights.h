/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigRights.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigRights is an interface to a Rights object. It allows the Rights object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it. This particular object is for configuring rights and allows modifications to be
  * performed in the database.
  */

#if !defined(IConfigRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include "core/data_access_interface/src/IRights.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigRights : public IRights
    {

    public:
        virtual ~IConfigRights() { };


        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier() = 0;


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a ActionGroup names etc cannot be found
         */
        virtual ItemChanges getAllItemChanges() = 0;

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged() =0;


        /**
         * getFormattedName
         *
         * This generates a name that can be displayed to the user to let them know what this right
         * represents
         *
         * @return string - The name in the format 'Profile <profilename>, Subsystem <subsystemname>'
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a ActionGroup names etc cannot be found
         */
        virtual std::string getFormattedName() =0;


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew() =0;

        
        /**
         * setActionGroupKey
         *
         * Sets the ActionGroup key for this rights configuration.
         *
         * @param The ActionGroup key for these rights as an unsigned long
         *
         */
        virtual void setActionGroupKey(long actionGroupKey) = 0;


        /**
         * deleteThisRight
         *
         * Removes this right from the database. 
         * The calling application MUST then delete this rights object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this rights was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This rights has not been deleted
         */
        virtual void deleteThisRight() =0;

        
        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Item object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This item has not been deleted
         */
        virtual void applyChanges() =0;

        /**
         * setSubsystemState
         *
         * Set the subsystem state for these rights.
         *
         * @param unsigned long - subsystem state key
         */
        virtual void setSubsystemState(unsigned long subsystemStateKey) = 0;


        // This is the value given to a ActionGroup when it is invalid.
        static const long INVALID_ACTIONGROUP;
        
    };
} //close namespace TA_Base_Core

#endif // !defined(IConfigRights_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
