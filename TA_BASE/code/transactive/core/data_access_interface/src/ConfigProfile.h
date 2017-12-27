/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigProfile.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/01/28 18:24:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * ConfigProfile is a concrete implementation of IConfigProfile, which is in turn an implementation
  * of IProfile. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for profiles.
  */


#if !defined(ConfigProfile_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
#define ConfigProfile_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_

#include <string>
#include <set>

#include "core/data_access_interface/src/IConfigProfile.h"

namespace TA_Base_Core
{
	class IData;
    class ProfileHelper;

    class ConfigProfile : public IConfigProfile
    {

    public:
        /**
         * ConfigProfile (constructor)
         *
         * Constructs a new instance of the ConfigProfile with no key. This is used when
         * creating a *new* profile - that is, one that does not yet exist in the database.
         */
        ConfigProfile();


        /**
         * ConfigProfile (constructor)
         *
         * Constructs a new instance of ConfigProfile with the specified key. This is used 
         * when creating a ConfigProfile based around an existing profile in the database.
         *
         * @param key The key to this profile in the database.
         */
        ConfigProfile(const unsigned long key);

		/**
         * Constructor
         *
         * Construct a ConfigProfile class based around dataset
         */
		ConfigProfile(const unsigned long row, TA_Base_Core::IData& data);


        /**
         * ConfigProfile (copy constructor)
         *
         * @param ConfigProfile& - The profile to make a copy of.
         */
        ConfigProfile( const ConfigProfile& theConfigProfile);

        
        virtual ~ConfigProfile();


    public:

        /**
         * getKey
         *
         * Returns the key for this profile.
         *
         * @return The key for this profile as an unsigned long.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - writeOperatorData() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        unsigned long getKey();


        /**
         * getName
         *
         * Returns the name of this profile. If this is the first time data for this profile
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this profile as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - setName() has already been called
         *      AND    - deleteThisObject() has NOT been called
         */
        std::string getName();


		virtual std::string getDisplayName();

        /**
         * isSystemProfile
         *
         * This indicates if this profile is a system profile or not
         *
         * @return bool - True if this profile is a system profile, false otherwise
         */
        virtual bool isSystemProfile();


        /**
         * getDefaultDisplay
         *
         * Returns the path for the preferred display for this profile. This is so GraphWorx
         * will load the correct display for the current profile.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long locationKey - the locationKey for the display (If 0, the location
		 *													is not required)
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
        virtual std::string getDefaultDisplay(unsigned long locationKey, int displayNumber);


        /**
         * getParameterValue
         *
         * Returns the path for the preferred display for this profile. This is so GraphWorx
         * will load the correct display for the current profile.
         *
         * @param const std::string parameterName - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long locationKey - the locationKey for the display (If 0, the location
		 *													is not required)
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
        virtual std::string getParameterValue(unsigned long locationKey, const std::string& parameterName);

        /**
         * getParameterActualValue
         *
         * Returns the actual value of the given parameter as a string. If the parameter is a number, this
         * will be returned as a string.
         *
         * @param const std::string parameterName - The name of the parameter
		 * @param unsigned long locationKey - the locationKey for the profile 													is not required)
         *
         * @return The value for this parameter at the specified location as a std::string. 
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getParameterActualValue(unsigned long locationKey, const std::string& parameterName);

        /**
         * requiresLocationToBeSelected
         *
         * This determines whether the profile requires a location to also be chosen.
         *
         * @return bool - True if the profile requires a location
         *                False if this profile is not location dependant.
         */
        virtual bool requiresLocationToBeSelected();


        /**
         * getAssociatedLocations
         *
         * This method retrieves the locations associated with the profile. Only these locations are valid
         * log-in locations for this profile.
         *
         * @return A vector containing pointers to ILocation objects. If this profile does not require locations,
         * the vector will be empty (size 0).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<ILocation*> getAssociatedLocations();
        
        /**
         * getAssociatedLocationKeys
         *
         * This method retrieves the keys of the locations associated with the profile. Only these locations are 
         * valid log-in locations for this profile.
         *
         * @return A vector containing keys to locations. If this profile does not require locations,
         * the vector will be empty (size 0).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::vector<unsigned long> getAssociatedLocationKeys();


		/**
         * getActionGroupAsStingForResource
         *
         * This method retrieves the ActionGroup that this profile has configured on
		 * the specified resource.
		 *
		 * @param unsigned long resourceKey - the pkey of the resource in question.
         *
         * @return A std::string containing the ActionGroup name. A std::string of size
		 *         0 will be returned if no ActionGroup is configured for the resource/profile
		 *         combination.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getActionGroupAsStringForResource( unsigned long resourceKey );


		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual std::string getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
			unsigned long subsystemStateKey );

		/**
         * isExclusive
         *
         * This method retrieves a boolean indicating if the profile is an 
		 * exclusive profile.
		 *
         *
         * @return A bool containing the result of the query.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual bool isExclusive();


		/**
         * getType
         *
         * This method retrieves an unsigned long indicating the profile type
         *
         * @return An unsigned long key
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual unsigned long getType();

        
		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual std::string getTypeAsString();

		/**
         * getActionGroup
         *
         * This method retrieves the ActionGroup that this profile has configured on
		 * the specified subsystem.
		 *
		 * @param ulong subsystem - the subsystem to get the ActionGroup for.
		 * @param ulong actionGroup  - contains the ActionGroup (return value).
		 * @param bool isControl  - indicates if the ActionGroup implies control (return value).
         *
         * @return void.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual bool getActionGroup( unsigned long subsystem, unsigned long subsystemState,
			unsigned long& actionGroup, bool& isControl );

		/**
		  * See IProfile.h for a description of this method.
		  *
		  */
		virtual unsigned long getAccessControlGroup();

		/**
		  * See IProfile.h for a description of this method.
		  *
		  */
		virtual std::string getItsiGroupName();

        /**
		  * See IProfile.h for a description of this method.
		  *
		  */
        virtual std::string getAccessControlGroupAsString();

		/**
          * See IProfile.h for a description of this method.
          */ 
		virtual std::vector<unsigned long> getAccessibleSubsystemKeys();

        /**
         * getDateCreated
         *
         * Returns the date created for this profile
         *
         * @return The date created for this profile as a time_t.
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
         * Returns the date modified for this profile.
         *
         * @return The date modified for this profile as a time_t.
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
		

        /**
         * getDisplayKey
         *
         * Returns the scada display key for the preferred display for this profile.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
		 * @param unsigned long locationKey - the locationKey for the display (If 0, the location
		 *                                    is not required)
         *
         * @return The display key for this profile as a unsigned long.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *            It will be thrown with the type NO_VALUE if there is no display for
         *            the specified display and location
         */
        virtual unsigned long getDisplayKey(unsigned long locationKey, int displayNumber);

        /**
         * setName
         *
         * Sets the name of this profile. This should only be used by the ConfigProfile class
         *
         * @param name The name to give this profile.
         *
         * pre: deleteThisObject() has NOT been called
         */
        virtual void setName(const std::string& name);
      
        
        /**
         * setType
         *
         * Sets the type of this profile
         *
         * @param unsigned long - The type of profile
         *
         * pre: This profile has not been deleted
         */
        virtual void setType(const unsigned long );


        /**
         * setDisplay
         *
         * This will set one of the displays for the profile.
         *
         * @param unsigned long location - This is the location key that this display is for
         * @param unsigned long screenNumber - This is the screen number the display is for. Numbering starts
         *                                     at 1.
         * @param long display - This is the key of the display from the SC_SCADA_SCHEMATIC table. If this is
         *                       a negative value then that means that no display should be set.
         */
        virtual void setDisplay(unsigned long location, unsigned long screenNumber, long display);


        /**
         * setDisplay
         *
         * This will set one of the displays for the profile.
         *
         * @param unsigned long location - This is the location key that this display is for
         * @param const std::string parameterName - This is the display name representing the display.
         * @param long display - This is the key of the display from the SC_SCADA_SCHEMATIC table. If this is
         *                       a negative value then that means that no display should be set.
         */
        virtual void setParameterValue(unsigned long location, const std::string& parameterName, const std::string& parameterValue);

        
        /**
         * removeDisplay
         *
         * This will remove a display from the profile for the specified location and screenNumber
         *
         * @param unsigned long location - This is the location key that the display is to be removed for
         * @param unsigned long screenNumber - This is the screen number the display is to be removed for.
         *                                      Numbering starts at 1.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeDisplay(unsigned long location, unsigned long screenNumber);


        /**
         * removeDisplay
         *
         * This will remove a display from the profile for the specified location and screenNumber
         *
         * @param unsigned long location - This is the location key that the display is to be removed for
         * @param unsigned long parameterName - This is the display name representing the display.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void removeParameter(unsigned long location, const std::string& parameterName);
        
        /**
         * getUniqueIdentifier
         *
         * This retrieves the unqiue identifier for this profile. We cannot use keys for identification
         * in the configuration editor because new profiles do not have keys. Therefore we use this
         * method for identification
         *
         * @return unsigned long - The unique identifier for this profile
         */
        virtual unsigned long getUniqueIdentifier()
        {
            return m_uniqueIdentifier;
        };

        
        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the profile. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all profile changes
         */
        virtual ItemChanges getAllItemChanges();

        
        /**
         * hasChanged
         *
         * This should be called to determine if any part of the profile has been changed by the user.
         *
         * @return bool - This will return true if the profile does not match the one in the database. It 
         *                will return false if the data has not changed from that in the database.
         */
        virtual bool hasChanged()
        {
            // If the profile changes is empty then nothing has changed
            return !m_profileChanges.empty();
        };


        /**
         * isNew
         *
         * This should be called to determine if this profile is new. A new profile is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the profile has not been applied to the database.
         */
        virtual bool isNew();


        /**
         * invalidate
         *
         * Make the data contained by this profile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         *
         * pre: deleteThisObject() has NOT been called
         */
        virtual void invalidate();



        /**
         * deleteThisObject
         *
         * Removes this profile from the database. 
         * The calling application MUST then delete this profile object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - applyChanges() has already been called
         *      This profile has not been deleted
         */
        virtual void deleteThisObject();


        /**
         * applyChanges
         *
         * This will apply all changes made to the database.
         *
         * @exception DatabaseException Thrown if there is a database error.
         * @exception DataException Thrown if the a parameter name cannot be found, or if
         *            there is more than one value for a parmeter.
         * @exception DataConfigurationException If the data contained in the Operator object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         * pre: This profile has not been deleted
         */
        virtual void applyChanges();
        
        
        /**
         * setAccessControlGroup
         *
         * Sets the access control group of this profile
         *
         * @param unsigned long - The access control group key
         *
         * pre: This profile has not been deleted
         */
        virtual void setAccessControlGroup(unsigned long accessControlGroupKey);


        /**
         * setRadioLoginGroup
         *
         * Sets the radio login group of this profile
         *
         * @param unsigned long - The radio login group key
         *
         * pre: This profile has not been deleted
         */
        virtual void setRadioLoginGroup(const std::string& radioGroupName);


        /**
         * setPaPriorityGroup
         *
         * Sets the pa priority group of this profile
         *
         * @param unsigned long - The pa priority group key
         *
         * pre: This profile has not been deleted
         */
        virtual void setPaPriorityGroup(const std::string& paGroupName);

        /**
         * setLocationAssociationStatus
         *
         * Sets the status of the association with the specified location
         *
         * @param locationId The ID of the location for which the assocation status is to be set
         * @param isAssociated True if the profile is associated with the specified location
         *
         * pre: This profile has not bene deleted
         */
        virtual void setLocationAssociationStatus(const unsigned long locationId, const bool isAssociated);

        /**
         * isLocationAssociated
         *
         * Determines if the specified location is associated with this profile. This can be used to determine if a 
         * location is valid for the given profile.
         *
         * @return true if the lcoation is associated with the profile, otherwise false.
         *
         * @param locationId The key to the location for which to check the association.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual bool isLocationAssociated(unsigned long locationId);

        
        private:
            
	    /**
         * updateChanges
         *
         * This updates the map recording the changes to the profile. Whenever a call to set is made in this
         * class then this method must be called to store the changes.
         *
         * @param const string& - The name of the attribute that has been given a new value
         * @param const string& - The old value that was stored for this attribute
         * @param const string& - The new value to be stored for this attribute
         */
        void updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue);

        
        /**
         * getNiceLocationLabel
         *
         * This takes a string in the format "locationkey screenNumber" and turns it into a user-readable
         * string such as "Location Name Display ScreenNumber".
         *
         * @param string - This is the label to convert. This should be in the format described above.
         *
         * @return string - The new user-friendly label
         */
        std::string getNiceLocationLabel(std::string oldLabel);


        /**
         * getNiceDisplayLabel
         *
         * This takes a string in the format "displayKey" and turns it into a user-readable
         * string such as "Display Name".
         *
         * @param string - This is the label to convert. This should be in the format described above.
         *
         * @return string - The new user-friendly label
         */
        std::string getNiceDisplayLabel(std::string oldLabel);


        // Assignment profile not used so it is made private
		ConfigProfile& operator=(const ConfigProfile&);


        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string TYPE;
        static const std::string ACCESS_CONTROL_GROUP;
        static const std::string RADIO_LOGIN_GROUP;
        static const std::string PA_PRIORITY_GROUP;
        static const std::string PROFILE_LOCATION_ASSOCIATION;


        ProfileHelper* m_profileHelper;

        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of profiles in the
                                          // Configuration Editor because new profiles do not have keys.
                                          // Therefore we use this method for identification.

        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_profileChanges; // This is a map of all changes made to the profile so that they are
                                       // recorded and can be sent in audit or online update messages.
    };
} // closes TA_Base_Core

#endif // !defined(ConfigProfile_BB23A710_0466_469f_952A_E7A090EFB3CD__INCLUDED_)
