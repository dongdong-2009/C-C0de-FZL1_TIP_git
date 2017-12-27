/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/system_controller/src/IConfigGroupMonitor.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigGroupMonitor is an interface to an SC group object. It allows the SC group object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it. This particular object is a configuration SC group and allows modifications to be
  * performed in the database.
  *
  * This uses the SN_GROUP_MONITOR table
  */

#if !defined(IConfigGroupMonitor_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigGroupMonitor_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigGroupMonitor : public IConfigItem, public IItem
    {

    public:
        virtual ~IConfigGroupMonitor() { };


        /**
         * getDescription
         *
         * Returns the description of this SC group. If this is the first time data for this SC group
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this SC group as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDescription() = 0;
		

        /**
         * setDescription
         *
         * Sets the description of this SC group locally.
         *
         * @param name The description to give this SC group.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the SC group key is invalid (and this is not a new SC group).
         */
        virtual void setDescription(const std::string& description) =0;

        
        /**
         * getDateCreated
         *
         * Returns the date created for this SC group
         *
         * @return The date created for this SC group as a time_t.
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
         * Returns the date modified for this SC group.
         *
         * @return The date modified for this SC group as a time_t.
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
         * deleteThisGroup
         *
         * Removes this SC group from the database. 
         * The calling application MUST then delete this SC group object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this group was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This group has not been deleted
         */
        virtual void deleteThisGroup() =0;


        /**
         * invalidate
         *
         * Make the data contained by this SC group as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IConfigGroupMonitor_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
