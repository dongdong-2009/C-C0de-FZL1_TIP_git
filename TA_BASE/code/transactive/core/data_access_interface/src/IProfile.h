/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/IProfile.h $
  * @author:  Karen Graham
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2015/01/28 18:24:10 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * IProfile is an interface to an profile object. It allows the profile object implementation
  * to be chagned (e.g. if necessary due to a database schema change) without changing code
  * that uses it.
  *
  * NOTE: This is temporary and must be changed later.
  */

#if !defined(IProfile_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
#define IProfile_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_

#include <string>
#include <vector>

#include "core/data_access_interface/src/IItem.h"

namespace TA_Base_Core
{
	class ILocation;

    class IProfile : public virtual IItem
    {

    public:
        virtual ~IProfile() { };


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
        virtual std::string getDefaultDisplay(unsigned long locationKey, int displayNumber) = 0;

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
        virtual std::string getParameterValue(unsigned long locationKey, const std::string& parameterName) = 0;

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
        virtual std::string getParameterActualValue(unsigned long locationKey, const std::string& parameterName) = 0;

        /**
         * requiresLocationToBeSelected
         *
         * This determines whether the profile requires a location to also be chosen.
         *
         * @return bool - True if the profile requires a location
         *                False if this profile is not location dependant.
         */
        virtual bool requiresLocationToBeSelected() = 0;

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
        virtual std::vector<ILocation*> getAssociatedLocations() = 0;

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
        virtual std::vector<unsigned long> getAssociatedLocationKeys() = 0;

		/**
         * getActionGroupAsStringForResource
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
        virtual std::string getActionGroupAsStringForResource( unsigned long resourceKey ) = 0;

		/**
         * getActionGroupAsStringForSubsystem
         *
         * This method retrieves the ActionGroup that this profile has configured on
		 * the specified subsystem.
		 *
		 * @param unsigned long subsystemKey - the pkey of the subsystem in question.
         *
         * @return A std::string containing the ActionGroup name. A std::string of size
		 *         0 will be returned if no ActionGroup is configured for the subsystem/profile
		 *         combination.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
        virtual std::string getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
			unsigned long subsystemStateKey ) = 0;

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
		virtual bool isExclusive() = 0;


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
		virtual unsigned long getType() = 0;

		/**
         * getTypeAsString
         *
         * This method retrieves a std::string indicating the profile type
         *
         * @return A std::string type name
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual std::string getTypeAsString() = 0;

		/**
         * getActionGroup
         *
         * This method retrieves the ActionGroup that this profile has configured on
		 * the specified subsystem.
		 *
		 * @param ulong subsystem - the subsystem to get the ActionGroup for.
		 * @param ulong subsystemState - the state of the subsystem to get the ActionGroup for.
		 * @param ulong actionGroup  - contains the ActionGroup (return value).
		 * @param bool isControl  - indicates if the ActionGroup implies control (return value).
         *
         * @return bool - true if a ActionGroup was found for the specified subsystem.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		virtual bool getActionGroup( unsigned long subsystem, unsigned long subsystemState,
			unsigned long& actionGroup, bool& isControl ) = 0;


        /**
         * isSystemProfile
         *
         * This indicates if this profile is a system profile or not
         *
         * @return bool - True if this profile is a system profile, false otherwise
         */
        virtual bool isSystemProfile() =0;

		/**
         * getAccessControlGroup
         *
         * The Access Control group lets profiles that belong to the same group
		 * be treated in a particular manner by Access Control.
         *
         * @return unisgned long Indicates the Access Control group to which this profile belongs
		 * unless the group is null, it then indicates the profile pkey.
         */
		virtual unsigned long getAccessControlGroup() = 0;

		/**
         * getItsiGroup
         *
		 * The ITSI Group is used by the Radio Agent to assign operator radio identifiers.
         *
         * @return  std::string Indicates the name of the ITSI group unless the group is null,
		 * the value returned in this case is "". Note that not every profile has to have an
		 * associated ITSI Group.
         */
		virtual std::string getItsiGroupName() = 0;

        /**
         * getAccessControlGroupAsString
         *
		 * Retrieves a std::string indicating the access control group name.
         *
         * @return  std::string Indicates the name of the access control group unless the group is null,
		 * the value returned in this case is "". Note that not every profile has to have an
		 * associated access control Group.
         */
        virtual std::string getAccessControlGroupAsString() = 0;
        
		/**
         * getAccessibleSubsystemKeys
         *
         * Returns the subsystems that this profile has access rights for.
         *
         * @return Accessible subsystems as a vector of pkeys.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */ 
		virtual std::vector<unsigned long> getAccessibleSubsystemKeys() = 0;

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
        virtual bool isLocationAssociated(unsigned long locationId) = 0;
		
		/*
		*
		*/
		virtual std::string getDisplayName() = 0;
    };

} //close namespace TA_Base_Core

#endif // !defined(IProfile_34315582_B0A7_4100_BD60_2E13958D7501__INCLUDED_)
