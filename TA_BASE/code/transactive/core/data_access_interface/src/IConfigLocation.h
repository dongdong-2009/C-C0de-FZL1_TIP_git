/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IConfigLocation.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * IConfigLocation is an interface to a Location object. It allows the Location object implementation
  * to be changed (e.g. if necessary due to a database schema change) without changing code
  * that uses it. This particular object is a configuration location and allows modifications to be
  * performed in the database.
  */

#if !defined(IConfigLocation_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IConfigLocation_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/data_access_interface/src/ConfigChangesTypes.h"

namespace TA_Base_Core
{

    class IConfigLocation : public IConfigItem, public ILocation
    {

    public:
        virtual ~IConfigLocation() { };


        /**
         * getDescription
         *
         * Returns the description of this Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The description for this location as a std::string
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
         * getOrderId
         *
         * Returns the oderId Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The physical order of this location
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual unsigned long getOrderId() = 0;
		

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

		virtual bool isDisplayOnly() = 0;

		virtual void setDisplayOnly(bool displayonly) = 0;
        /**
         * deleteThisLocation
         *
         * Removes this location from the database. 
         * The calling application MUST then delete this location object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this location was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This location has not been deleted
         */
        virtual void deleteThisLocation() =0;


        /**
         * setDescription
         *
         * Sets the description of this location locally.
         *
         * @param name The description to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setDescription(const std::string& description) =0;


        /**
         * setOrderId
         *
         * Sets the orderId of this location locally.
         *
         * @param name The physical order of this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setOrderId(const unsigned long orderId) =0;

		/**
         * setDisplayName
         *
         * Sets the description of this location locally.
         *
         * @param displayName The display name to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setDisplayName(const std::string& displayName) =0;
        
        /**
         * invalidate
         *
         * Make the data contained by this Location as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate() =0;

    };
} //close namespace TA_Base_Core

#endif // !defined(IConfigLocation_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
