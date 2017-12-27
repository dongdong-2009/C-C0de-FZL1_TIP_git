/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/Profile.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/01/28 18:24:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * Profile is an implementation of IProfile. It holds the data specific to an profile entry
  * in the database, and allows read-only access to that data.
  *
  */

#if !defined(Profile_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
#define Profile_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_

#include <string>

#include "core/data_access_interface/src/IProfile.h"
#include "core/data_access_interface/src/ProfileHelper.h"


namespace TA_Base_Core
{
    class Profile : public IProfile
    {

    public:

        /**
         * Constructor
         *
         * Construct an Profile class based around a key.
         *
         * @param key The key of this Profile in the database
         * @param name The name of the Profile.
         * @param display A path to the GraphworX display
         * NOTE: The second two params are temporary and must be removed
         */
        Profile( const unsigned long key );

		/**
		 * Constructor
		 *
		 * Construct a Profile class based around dataset
		 */
		Profile(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * Destructor
         */
        virtual ~Profile();

        /**
         * getKey
         *
         * Returns the key for this Profile.
         *
         * @return The key for this profile as an unsigned long.
         */
        virtual unsigned long getKey();

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
         */
        virtual std::string getName();

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
        bool isLocationAssociated(unsigned long locationId);

		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual std::string getActionGroupAsStringForResource( unsigned long resourceKey );

		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual std::string getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
			   unsigned long subsystemStateKey	);

		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual bool isExclusive();

		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual unsigned long getType();

		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual std::string getTypeAsString();

		/**
		  * See IProfile.h for a description of this method.
		  */
		virtual bool getActionGroup( unsigned long subsystem, unsigned long subsystemState,
			unsigned long& actionGroup, bool& isControl );


        /**
         * isSystemProfile
         *
         * This indicates if this profile is a system profile or not
         *
         * @return bool - True if this profile is a system profile, false otherwise
         */
        virtual bool isSystemProfile();


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
		virtual std::string getDisplayName();

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
         * invalidate
         *
         * Make the data contained by this profile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

        /**
		  * See IProfile.h for a description of this method.
		  *
		  */
        virtual void setAccessControlGroup(unsigned long accessControlGroupKey);

        /**
		  * See IProfile.h for a description of this method.
		  *
		  */
        virtual void setItsiGroup(const std::string& itsiGroupName);

    private:
        // Assignment operator not used so it is made private
		Profile& operator=(const Profile&);

        // Copy constructor should not be used and has therefore been made private. If a copy
        // constructor is required care should be taken. The copy constructor of the ProfileHelper
        // has only been written for ConfigProfile objects and will not copy the profile helper
        // for a read-only object (ie this one).
        Profile( const Profile& theProfile);            

		ProfileHelper* m_profileHelper;
    };
} // closes TA_Base_Core

#endif // !defined(Profile_6AAB77B6_9DD3_4b98_A7B1_665FDAC65A0A__INCLUDED_)
