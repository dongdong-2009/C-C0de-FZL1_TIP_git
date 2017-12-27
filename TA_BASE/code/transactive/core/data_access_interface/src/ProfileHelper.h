/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileHelper.h $
  * @author:  Cameron Rochester
  * @version: $Revision: #5 $
  *
  * Last modification: $DateTime: 2017/06/16 10:39:06 $
  * Last modified by:  $Author: limin.zhu $
  *
  * This is the base Profile class that supports all get and set operaion on profiles
  *
  * NOTE: This will also directly read the SE_PROFILE_TYPE table to decide if the profile requires a display
  */

#if !defined(PROFILE_HELPER_H)
#define PROFILE_HELPER_H

#include <string>
#include <map>
#include <climits>
#include "core/synchronisation/src/NonReEntrantThreadLockable.h"

#define DISPLAY                     "Display"
#define DISPLAY_1                   "Display 1"
#define DISPLAY_2                   "Display 2"
#define DISPLAY_3                   "Display 3"
#define LeftBannerDefaultPage       "LeftBannerDefaultPage"
#define CentreBannerDefaultPage     "CentreBannerDefaultPage"
#define RightBannerDefaultPage      "RightBannerDefaultPage"


namespace TA_Base_Core
{
	// forward declarations
	class IData;
    class ILocation;

    class ProfileHelper
    {

    public:
		static const unsigned long CENTRAL_PROFILE_TYPE;
		static const unsigned long LOCAL_PROFILE_TYPE;
        /**
         * Constructor
         * 
         * This constructor creates a new ProfileHelper for the specified key.
         *
         * @param key The key into the SE_PROFILE table for this Gui
         */
        ProfileHelper(const unsigned long key);

        /**
         * Constructor
         * 
         * This constructor creates a new ProfileHelper for configuration, initiailising
         * the key to 0 - it will be specified once this Profile is written to the database
         * for the first time.
         */
        ProfileHelper();

	    /**
	      * ProfileHelper
	      * 
	      * This constructor creates a new object using the input data
          * which is representative of a row returned from SQL statement
	      * 
	      * @param : const unsigned long row
          *          - the row of data in the data object that we should collect data from
          *          in order to construct ourselves
	      * @param : TA_Base_Core::IData& data
          *          - the IData interface that should have been obtained using the 
          *          getBasicQueryData function
	      * 
	      * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
	      */
		ProfileHelper(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Copy Constructor
         */
        ProfileHelper( const ProfileHelper& theHelper);


        /**
         * Destructor
         */
        virtual ~ProfileHelper();


        /**
         * isNew
         *
         * This returns whether this is a new operator or not
         *
         * @return bool - True if this is a new operator
         */
        virtual bool isNew() const
        {
            return m_isNew;
        }

        /**
         * getKey
         *
         * Returns the key for this Profile.
         *
         * @return The key for this Profile as an unsigned long.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        unsigned long getKey();

		/**
         * getName
         *
         * Returns the name of this Profile. If this is the first time data for this profile
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
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this profile. This should only be used by the ConfigProfile class
         *
         * @param name The name to give this profile.
         *
         * pre: deleteThisObject() has NOT been called
         */
		void setName( const std::string& name );

		/**
         * getDisplayName
         *
         * Returns the display name of this Profile. If this is the first time data for this profile
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
         */
        std::string getDisplayName();


        /**
         * setDisplayName
         *
         * Sets the display name of this profile. This should only be used by the ConfigProfile class
         *
         * @param name The name to give this profile.
         *
         * pre: deleteThisObject() has NOT been called
         */
		void setDisplayName( const std::string& displayName );

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
		unsigned long getType();

        
		/**
		  * See IProfile.h for a description of this method.
		  */
		std::string getTypeAsString();

		/**
         * getTypeName
         *
         * This method returns the type name when passed a type key
         *
         * @param unsigned long - The key of the profile type.
         *
         * @return std::string - A type name
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        std::string getTypeName(unsigned long typeKey);

        
        /**
         * setType
         *
         * Sets the type of this profile
         *
         * @param unsigned long - The type of profile
         *
         * pre: This profile has not been deleted
         */
        void setType(const unsigned long );

        
        /**
         * isSystemProfile
         *
         * This indicates if this profile is a system profile or not
         *
         * @return bool - True if this profile is a system profile, false otherwise
         */
        bool isSystemProfile();

        
        /**
         * getDisplay
         *
         * Returns the path for the preferred display for this profile. This is so GraphWorx
         * will load the correct display for the current profile.
         *
         * @param int displayNumber - A profile can have several displays configured for it
         *                            This indicates which display to return a value for.
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
		std::string getDefaultDisplay( unsigned long locationKey, int displayNumber );


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
        std::string getParameterValue(unsigned long locationKey, const std::string& parameterName);


        /**
         * getParameterActualValue
         *
         * Returns the actual value of the given parameter as a string. If the parameter is a number, this
         * will be returned as a string.
         *
         * @param const std::string parameterName - The name of the parameter
		 * @param unsigned long locationKey - the locationKey for the profile is not required)
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
        std::string getParameterActualValue(unsigned long locationKey, const std::string& parameterName);


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
        unsigned long getDisplayKey(unsigned long locationKey, int displayNumber);

        /**
         * getDisplayKey
         *
         * Returns the scada display key for the preferred display for this profile.
         *
         * @param int parameterName - A profile can have several displays configured for it
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
        unsigned long getDisplayKey(unsigned long locationKey, const std::string& parameterName);

        
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
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void setDisplay(unsigned long location, unsigned long screenNumber, long display);

        /**
         * setDisplay
         *
         * This will set one of the displays for the profile.
         *
         * @param unsigned long location - This is the location key that this display is for
         * @param unsigned long parameterName - This is the display name that distinguishes between the displays. 
         * @param long display - This is the key of the display from the SC_SCADA_SCHEMATIC table. If this is
         *                       a negative value then that means that no display should be set.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual void setParameter(unsigned long location, const std::string& parameterName, const std::string& parameterValue);

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
         * @param unsigned long parameterName - This is the display is to be removed.
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
         * isLocalProfile
         *
         * Returns the delegation status of this Profile. If this is the first time data for this profile
         * has been requested, all the data is loaded from the database.
         *
         * @return True if this is a local profile
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - setLocalProfile() has already been called
         */
		bool isLocalProfile( );
        

        /**
         * setIsLocalProfile
         *
         * This sets whether this is a local or central profile
         *
         * @param bool - This is true if this profile is local
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        void setIsLocalProfile(bool isLocalProfile);


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
        time_t getDateCreated();


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
        time_t getDateModified();

        
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
		  * See IProfile.h for a description of this method.
		  */
		std::string getActionGroupAsStringForResource( unsigned long resourceKey );

		/**
		  * See IProfile.h for a description of this method.
		  */
		std::string getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
			   unsigned long subsystemStateKey	);

		/**
		  * See IProfile.h for a description of this method.
		  */
		bool getActionGroup( unsigned long subsystem, unsigned long subsystemState,
				unsigned long& actionGroup, bool& isControl );

		/**
		  * See IProfile.h for a description of this method.
		  *
		  */
		unsigned long getAccessControlGroup();

        /**
		  * See IProfile.h for a description of this method.
		  *
		  */
        std::string getAccessControlGroupAsString();

		/**
          * See IProfile.h for a description of this method.
          */ 
		std::vector<unsigned long> getAccessibleSubsystemKeys();

        /**
         * getAccessControlGroupName
         *
         * This method returns the access control group name when passed a key
         *
         * @param unsigned long - The key of the access control group.
         *
         * @return std::string - A group name
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        std::string getAccessControlGroupName(unsigned long accessControlGroupKey);

		/**
		  * See IProfile.h for a description of this method.
		  *
		  */
		std::string getItsiGroupName();

\
        /**
         * invalidate
         *
         * Make the data contained by this profile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeProfileData() has been called prior to calling
         * this method on a new gui, as it does not make any sense to invalidate an
         * gui that has not yet been written to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        void invalidate();


        /**
         * writeData
         * 
         * Write this profile to the database. If this is a new profile, this will populate
         * the m_key field (which can be retrieved using getKey()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Profile object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeData();


        /**
         * deleteProfile
         *
         * Remove this profile from the database. Once this method has been executed, the
         * ProfileHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - writeProfileData() has already been called
         */
        void deleteProfile();

        /**
         * setAccessControlGroup
         *
         * Sets the access control group of this profile
         *
         * @param unsigned long - The access control group key
         *
         * pre: This profile has not been deleted
         */
        void setAccessControlGroup(unsigned long accessControlGroupKey);

        /**
         * setItsiGroup
         *
         * Sets the access control group of this profile
         *
         * @param unsigned long - The radio login group key
         *
         * pre: This profile has not been deleted
         */
        void setItsiGroup(const std::string& itsiGroupName);


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
         * addLocationAssociation
         *
         * Adds a location association
         *
         * @param locationId The ID of the location for which to add an association.
         */
        void addLocationAssocation(unsigned long locationId);

        /**
         * removeLocationAssociation
         *
         * Removes a location association
         *
         * @param locationId The ID of the location for which to remove an association
         */
        void removeLocationAssociaiton(unsigned long locationId);

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

        // Made private as it is not needed
		ProfileHelper& operator=(const ProfileHelper &);

		static const unsigned long NORMAL_SUBSYSTEM_STATE;

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - writeProfile() has already been called
         */
        void reload();

        /**
          * reloadUsing
          *
          * Fills out the local members with data contained in the input data object
          *
          * @param : const unsigned long row - the row to query in the data object
          * @param : TA_Base_Core::IData& data - the IData interface that should 
          *          have been obtained using the getBasicQueryData function
          *         
          * @exception  DatabaseException 
          *             - if there is a problem establishing a connection with the database.
          *             - if an error is encountered while retrieving data.
          * @exception  DataException
          *             - if the data cannot be converted to the required format 
          *             - NO_VALUE if the record cannot be found for the helper 
          *             constructed with a key.
          *             - NOT_UNIQUE if the primary key returns multiple records 
          */
        void reloadUsing(const unsigned long row, TA_Base_Core::IData& data);


		/** 
		  * reloadDisplayData
		  *
		  * This is used to load the display data for this profile
		  *
		  */
		void reloadDisplayData();


		/** 
		  * reloadAccessibleSubsystemData
		  *
		  * This is used to load the list of subsystems that this profile has access rights for.
		  *
		  */
        void reloadAccessibleSubsystemData();

        /**
         * reloadAssociatedLocationData
         *
         * This is used to load the list of locations this profile is able to log in to.
         */
        void reloadAssociatedLocationData();


        /**
         * modifyExistingProfile
         *
         * This will update an existing profile in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the profile object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void modifyExistingProfile();

        
        /**
         * addNewProfile
         *
         * This will add a new profile in the database with the settings currently saved here
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Location object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         */
        void addNewProfile();


        /**
         * retrieveDeletedField
         *
         * When a profile is deleted certain fields must be appended with #number. The number is incremented for
         * each profile deleted with the same name. This determines what the next number will be
         * for this profile when it is deleted.
         *
         * @param string - This is the current value for the field we are checking. So this is the current name
         *                 (ie m_name)
         * @param string - This is the name of the column in the database that stores this field.
         * @param unsigned long - This is the maximum size of the field that the database will allow.
         *
         * @return string - The new name for the field when it is deleted
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        std::string retrieveDeletedField(const std::string& currentField, const std::string& columnName, unsigned long maxFieldSize);


        /**
         * updateLocationDisplays
         *
         * This updates the database with the details stored in m_defaultDisplays.
         *
         */
        void updateLocationDisplays();


		/**
		 * resetLocationDisplays
		 *
		 * Clears the location displays that are stored in the default display map.
		 * The map will contain an entry for each location, but none of them will be
		 * mapped to a display.
		 *
		 */
		void resetLocationDisplays();

		unsigned long getSubsystemForResource( unsigned long resourceKey );

        void updateProfileGroups();

        /**
         * initialiseMaxFieldLengths
         *
         * This retrieves the maximum field lengths for the name and description fields from the database.
         *
         * @exception DataException A DataException will be thrown if there is a 
         *            problem retrieving the data from the database.
         */
        void initialiseMaxFieldLengths();

        /**
         * updateLocationAssociations
         *
         * This update the database with the details stored in m_associatedLocationKeys
         */
        void updateLocationAssociations();


        /** 
          * getLocationKeys
          *
          * Returns the location keys associated with this profile
          *
          *
          * @return the location keys associated with this profile
          */
        std::vector<unsigned long> getLocationKeys();


        static const std::string DELETED_FLAG;

        static const unsigned long DEFAULT_NAME_MAX_LENGTH;

        static const unsigned long INVALID_ACCESS_CONTROL_GROUP_KEY;
        
        bool m_isValidData;
		bool m_isValidDisplayData;
        bool m_isValidAccessibleSubsystemData;
        bool m_isValidAssociatedLocationData;
        bool m_isNew;

        unsigned long m_key;
        std::string m_name;
		std::string m_displayName;
        unsigned long m_type;
		unsigned long m_accessControlGroup;
        std::string m_paPriorityGroup;
		std::string m_itsiGroup;
        bool m_isSystemProfile;

		bool m_localProfile;
        time_t m_dateCreated;
        time_t m_dateModified;

	    static TA_Base_Core::NonReEntrantThreadLockable m_nameMaxLengthLock;
        static volatile unsigned long m_nameMaxLength;

		class ActionGroupEntry
		{
		public:

			ActionGroupEntry();
			ActionGroupEntry( unsigned long actionGroupKey, bool isControl );
			ActionGroupEntry( const ActionGroupEntry& rhs );
			ActionGroupEntry& operator=(const ActionGroupEntry& rhs );

			unsigned long m_actionGroupKey;
			bool          m_isControl;
		};

		typedef std::vector<unsigned long> ActionGroupKey;

		typedef std::map<ActionGroupKey, ActionGroupEntry> ActionGroupMap;
		ActionGroupMap m_actionGroupMap;

		/**
		* store the profile parameters, indexed by location. For central profiles the 0 location of
        * <unallocated> will be used
		*/
        typedef struct
        {
            unsigned long display1;
            unsigned long display2;
            unsigned long display3;
            std::string leftBanner;
            std::string centreBanner;
            std::string rightBanner;
        } ProfileParameters;
        typedef std::map< unsigned long, ProfileParameters > DefaultProfileParamMap;
		DefaultProfileParamMap m_defaultProfileParams;

        
		typedef std::map<ActionGroupKey, std::string> ActionGroupNameMap;
		ActionGroupNameMap m_actionGroupNameMap;


		typedef std::map<unsigned long, unsigned long> SubsystemMap;
		SubsystemMap m_subsystemMap;

        std::map<unsigned long, std::string> m_profileTypes;

        std::vector<unsigned long> m_accessibleSubsystems;
        std::vector<unsigned long> m_associatedLocationKeys;
    };

} // closes TA_Base_Core

#endif // !defined(PROFILE_HELPER_H)
