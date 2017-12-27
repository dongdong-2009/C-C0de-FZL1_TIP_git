/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigLocation.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigLocation is an implementation of IConfigLocation. It holds the data specific to a Location entry
  * in the database, and allows read-write access to that data.
  */

#if !defined(ConfigLocation_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define ConfigLocation_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>
#include <ctime>

#include "core/data_access_interface/src/IConfigLocation.h"

namespace TA_Base_Core
{
	class IData;
    class LocationHelper;

    class ConfigLocation : public IConfigLocation
    {

    public:

        /**
         * Constructor
         *
         * Construct an empty ConfigLocation class ready to be populated.
         */
        ConfigLocation( );


        /**
         * Constructor
         *
         * Construct a Location class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Location in the database
         */
        ConfigLocation( const unsigned long key );

        
        /**
         * Constructor
         *
         * Construct a complete location, this will set isValidData to true
         *
         * @param key The key of this Location in the database
         * @param name The name of this Location in the database
         * @param description The description of this Location in the database
         * @param dateCreated The date and time the location was created
         * @param dateModified The date and time the location was last modified
         */
        ConfigLocation( const unsigned long key, const std::string& name, const std::string& description, const unsigned long orderId,
                         const std::string & displayName, const time_t dateCreated, const time_t dateModified,bool displayOnly);

		/**
         * Constructor
         *
         * Construct a ConfigLocation class based around dataset
         */
		ConfigLocation(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Constructor
         *
         * Construct a Location class based around an existing location. This will
		 * copy all the paramters of the existing location
         *
         * @param theLocation The location to copy
         */
		ConfigLocation( const ConfigLocation& theLocation);            	
		

        /**
         * Destructor
         */
        virtual ~ConfigLocation();


        /**
         * getKey
         *
         * Returns the key for this Location.
         *
         * @return The key for this Location as an unsigned long.
         */
        virtual unsigned long getKey();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges()
        {
            return m_locationChanges;
        }

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the location changes is empty then nothing has changed
            return !m_locationChanges.empty();
        }


        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        virtual bool isNew();

        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };


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
        virtual void applyChanges();


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
        virtual void deleteThisLocation();


        /**
         * getName
         *
         * Returns the name of this Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getName();
		

		/**
         * getDisplayName
         *
         * Returns the display name of this Location. If this is the first time data for this Location
         * has been requested, all the data is loaded from the database.
         *
         * @return The display name for this operator as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual std::string getDisplayName();


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
        virtual std::string getDescription();
		

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
        virtual unsigned long getOrderId();
		
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
        virtual time_t getDateCreated();


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
        virtual time_t getDateModified();

        virtual bool isDisplayOnly();
		virtual void setDisplayOnly(bool displayonly);
        /**
         * setName
         *
         * Sets the name of this location locally.
         *
         * @param name The name to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setName(const std::string& name);


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
        virtual void setDescription(const std::string& description);


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
        virtual void setOrderId(const unsigned long orderId);

		/**
         * setDisplayName
         *
         * Sets the display name of this location locally.
         *
         * @param name The display name to give this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the location key is invalid (and this is not a new location).
         */
        virtual void setDisplayName(const std::string& displayName);

        
        /**
         * invalidate
         *
         * Make the data contained by this Location as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        
        /**
         * getDefaultDisplay
         *
         * Returns the path for the preferred display for this profile. This is so GraphWorx
         * will load the correct display for the current profile. This is not really needed in the
         * ConfigLocation but we must implement it to meet the interface.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long profileKey - the profileKey for the display
         *
         * @return The display for this profile as a std::string. This string should be a path.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getDefaultDisplay(unsigned long profileKey, int displayNumber);
        

        /**
         * isProfileAssociated
         *
         * Determines if the specified profile is associated with this location. This can be used to determine if a 
         * profile is valid at the given location.
         *
         * @return true if the profile is associated with the location, otherwise false.
         *
         * @param profileId The key to the profile for which to check the association.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isProfileAssociated(unsigned long profileId);


        /**
         * getLocationType
         *
         * NB: This is a method used only by applications, and should never be called on the config location 
         * object.
         *
         * @return The location type for this location.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual ILocation::ELocationType getLocationType();

        virtual std::string getTypeName();

private:
	
        /**
         * updateLocationChanges
         *
         * This updates the map recording the changes to the location. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateLocationChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
		
        // Assignment operator not used so it is made private
		ConfigLocation& operator=(const ConfigLocation&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string DESCRIPTION;
		static const std::string ORDER_ID;
		static const std::string DISPLAYNAME;
		static const std::string DISPLAYONLY;

        LocationHelper* m_locationHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of locations in the
                                          // Configuration Editor because new locations do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_locationChanges; // This is a map of all changes made to the location so that they are
                                       // recorded and can be sent in audit or online update messages.
        
    };
} // closes TA_Base_Core

#endif // !defined(ConfigLocation_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
