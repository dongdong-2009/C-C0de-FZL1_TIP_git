/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigSubsystem.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 12:15:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * This class implements the IConfigSubsystem interface.
  *
  */

#ifndef CONFIG_SUBSYSTEM_H
#define CONFIG_SUBSYSTEM_H

#include <string>
#include "core/data_access_interface/src/IConfigSubsystem.h"
#include "core/data_access_interface/src/SubsystemHelper.h"


namespace TA_Base_Core
{

    class ConfigSubsystem : public IConfigSubsystem
    {

    public:
        virtual ~ConfigSubsystem();


        /**
         * Constructor
         *
         * Construct an empty ConfigSubsystem class ready to be populated.
         */
        ConfigSubsystem();


        /**
         * Constructor
         *
         * Construct a Subsystem class based around a key, this will read
		 * the data from the database and throw the any DatabaseException
		 * if not succesful
         *
         * @param key The key of this Subsystem in the database
         */
        ConfigSubsystem(const unsigned long key);

        
        /**
         * Constructor
         *
         * Construct a complete subsystem, this will set isValidData to true
         *
         * @param key The key of this Subsystem in the database
         * @param name The name of this Subsystem in the database
         * @param isExclusive Indicates whether the subsystem has exclusive control
         * @param isPhysical Indicates whether this is a physical subsystem
         * @param dateCreated The date and time the subsystem was created
         * @param dateModified The date and time the subsystem was last modified
         */
        ConfigSubsystem(
            const unsigned long key,
            const std::string& name,
            const bool isExclusive,
            const bool isPhysical,
            const time_t dateCreated,
            const time_t dateModified);


		/**
         * Constructor
         *
         * Construct a ConfigSubsystem class based around dataset
         */
		ConfigSubsystem(const unsigned long row, TA_Base_Core::IData& data);


        /**
         * Constructor
         *
         * Construct a Subsystem class based around an existing subsystem. This will
		 * copy all the paramters of the existing subsystem
         *
         * @param theSubsystem The subsystem to copy
         */
		ConfigSubsystem( const ConfigSubsystem& theSubsystem);            	

        
        /**
         * getKey
         *
         * Returns the key for this item.
         *
         * @return The key for this item as an unsigned long.
         */
        virtual unsigned long getKey();

		 /**
         * getSystemKey
         *
         * Returns the key for this system.
         *
         * @return The key for this system as an unsigned long.
         */
        virtual unsigned long getSystemKey();

        /**
         * getName
         *
         * Returns the name of this item. If this is the first time data for this item
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this item as a std::string
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
		* Returns the display Name of this item. If this is the first time data for this item
		* has been requested, all the data is loaded from the database.
		*
		* @return The display name for this item as a std::string
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
         * isPhysical
         *
         * This returns whether this is a physical subsystem or not.
         *
         * @return bool - True if this is a physical subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isPhysical();


		/** 
         * isExclusive
         *
         * This returns whether this is an exclusive subsystem or not.
         *
         * @return bool - True if this is an exclusive subsystem, false otherwise
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isExclusive();


		 virtual bool isLocationExclusive();

        
        /**
         * getDateCreated
         *
         * Returns the date created for this subsystem.
         *
         * @return The date created for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateCreated();


        /**
         * getDateModified
         *
         * Returns the date modified for this subsystem.
         *
         * @return The date modified for this subsystem as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual time_t getDateModified();

        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this item. We cannot use keys for identification
         * in the configuration editor because new items do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this item
         */
        virtual unsigned long getUniqueIdentifier();


        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the item has been changed by the user.
         *
         * @return bool - This will return true if the item does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged();


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
         * setName
         *
         * Sets the name of this item locally.
         *
         * @param name The name to give this item.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the item key is invalid (and this is not a new item).
         */
        virtual void setName(const std::string& name);


		/**
		* setDisplayName
		*
		* Sets the display name of this item locally.
		*
		* @param display name The name to give this item.
		*
		* @exception DatabaseException A DatabaseException will be thrown if there is a
		*            problem establishing a connection with the database, or if an
		*            error is encountered while retrieving data.
		* @exception DataException A DataException will be thrown if the data cannot be 
		*            converted to the required format (e.g. TYPEKEY should be an 
		*            unsigned long), or if the wrong ammount of data is retrieved, or if the
		*            the item key is invalid (and this is not a new item).
		*/
		virtual void setDisplayName(const std::string& name);
        /**
         * setExclusiveControlStatus
         * 
         * Sets whether the subsystem has exclusive control.
         *
         * @param isExclusive  true if the subsystem has exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setExclusiveControlStatus(bool isExclusive);

        /**
         * setLocationExclusiveControlStatus
         * 
         * Sets whether the subsystem has location exclusive control.
         *
         * @param isExclusive  true if the subsystem has location exclusive control,
         *                     false otherwise.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the subsystem key is invalid (and this is not a new subsystem).
         */
        virtual void setLocationExclusiveControlStatus(bool isExclusive);
		

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
         * deleteThisSubsystem
         *
         * Removes this subsystem from the database. 
         * The calling application MUST then delete this subsystem object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this subsystem was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This subsystem has not been deleted
         */
        virtual void deleteThisSubsystem();


        /**
         * canDelete
         *
         * This determines if this item can be deleted or not. Some items are reserved and can
         * therefore not be deleted
         *
         * @return bool - true if this item can be deleted, false otherwise
         */
        virtual bool canDelete();


        /**
         * invalidate
         *
         * Make the data contained by this item as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();


        /**
         *  Gets a boolean as a string. Use getBoolFromString to convert
         *  this value back to a bool.
         */
        std::string getBoolAsString(bool theBool);

        /**
         *  Gets a boolean from a string created using getBoolAsString.
         */
        bool getBoolFromString(
            const std::string& theString);



    private:

        /**
         * updateSubsystemChanges
         *
         * This updates the map recording the changes to the subsystem. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        virtual void updateSubsystemChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);
 
		
        // Assignment operator not used so it is made private
		ConfigSubsystem& operator=(const ConfigSubsystem&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string EXCLUSIVE;
		static const std::string DISPLAY_NAME;
		static const std::string LOCATION_EXCLUSIVE;


        SubsystemHelper* m_helper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of subsystems in the
                                          // Configuration Editor because new subsystems do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_subsystemChanges; // This is a map of all changes made to the subsystem so that they are
                                       // recorded and can be sent in audit or online update messages.
        
    
    };

} //close namespace TA_Base_Core

#endif // CONFIG_SUBSYSTEM_H
