/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ProfileGroupHelper.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This is the base Profile Group class that supports all get and set operaion on profiles
  *
  * NOTE: This will also directly read the SE_PROFILE_GROUP or RA_ITSI_GROUP table to decide 
  * if the profile group requires a display. It will also access the SE_PROFILE to check whether
  * the profile or ITSI group is being used before allowing delete
  */


// ProfileGroupHelper.h: interface for the ProfileGroupHelper class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PROFILEGROUPHELPER_H__982C4F5E_664A_4E00_A57D_E5F7926028C2__INCLUDED_)
#define AFX_PROFILEGROUPHELPER_H__982C4F5E_664A_4E00_A57D_E5F7926028C2__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>
#include <climits>

namespace TA_Base_Core
{
	class IData;

	class ProfileGroupHelper  
	{
	public:

		static const std::string PROFILE;
		static const std::string ITSI;
		
        /**
         * Constructor
         * 
         * This constructor creates a new ProfileGroupHelper for the specified key.
         *
         * @param key The key into the SE_PROFILE_GROUP or RA_ITSI_GROUP table for this Gui
         */
        ProfileGroupHelper(std::string type, unsigned long key);

        /**
         * Constructor
         * 
         * This constructor creates a new ProfileGroupHelper for configuration, initiailising
         * the key to 0 - it will be specified once this Profile Group is written to the database
         * for the first time. The default for group type is PROFILE
         */
        ProfileGroupHelper(std::string type=ProfileGroupHelper::PROFILE);


	    /**
	      * ProfileGroupHelper
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
		ProfileGroupHelper(const std::string& type, const unsigned long row, TA_Base_Core::IData& data);


        /**
         * Copy Constructor
         */
        ProfileGroupHelper(const ProfileGroupHelper& theHelper);


        /**
         * Destructor
         */
		virtual ~ProfileGroupHelper();

        /**
         * isNew
         *
         * This returns whether this is a new profile group or not
         *
         * @return bool - True if this is a new profile group
         */
        virtual bool isNew() const
        {
            return m_isNew;
        }

        /**
         * getKey
         *
         * Returns the keys for this Profile group.
         *
         * @return All keys for this Profile group as an unsigned long.
         *
         * pre: Either - this gui was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
		unsigned long getKey();

		/**
         * getName
         *
         * Returns the name of this Profile Group. If this is the first time data for this profile group
         * has been requested, all the data is loaded from the database.
         *
         * @return The name for this profile group as a std::string
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         *
         * pre: Either - this profile group was initially loaded from the database
         *      OR     - setName() has already been called
         */
        std::string getName();


        /**
         * setName
         *
         * Sets the name of this profile group. This should only be used by the ConfigProfileGroup class
         *
         * @param name The name to give this profile group.
         *
         * pre: deleteThisObject() has NOT been called
         */
		void setName( const std::string& name );


		/**
         * getGroupType
         *
         * This method retrieves indicating the profile group type
         *
         * @return A string / name of the profile group type either PROFILE or ITSI currently
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. ACCESSLEVEL should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved.
         */
		std::string getGroupType();

		/**
         * setGroupType
         *
         * Sets the type of this profile group which is either PROFILE or ITSI currently
         *
         * @param string - The type of profile group
         *
         */
        void setGroupType(std::string type);

        /**
         * getDateCreated
         *
         * Returns the date created for this profile group
         *
         * @return The date created for this profile group as a time_t.
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
         * Returns the date modified for this profile group.
         *
         * @return The date modified for this profile group as a time_t.
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
         * invalidate
         *
         * Make the data contained by this profile as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         * It is a precondition that writeProfileGroupData() has been called prior to calling
         * this method on a new gui, as it does not make any sense to invalidate an
         * gui that has not yet been written to the database.
         *
         * pre: Either - this profile group was initially loaded from the database
         *      OR     - writeGuiData() has already been called
         */
        void invalidate();


        /**
         * writeData
         * 
         * Write this profile group to the database. If this is a new profile group, this will populate
         * the m_key field (which can be retrieved using getKey()).
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         * @exception DataConfigurationException If the data contained in the Profile group object
         *            is not sufficent to create an entry in the database, a 
         *            DataConfigurationException will be thrown. This is thrown as an exception
         *            and not tested as an assert as it can be directly linked to GUI components
         *            and may be the result of a user action that occurs prior to filling in 
         *            all sections.
         *
         */
        void writeData();


        /**
         * deleteGroup
         *
         * Remove this profile group from the database. Once this method has been executed, the
         * ProfileHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile group was initially loaded from the database
         *      OR     - writeProfileData() has already been called
         */
        void deleteGroup();

        /**
         * isGroupIsUsedInProfile
         *
         * Checks whether this profile group is used in profile table from the database. 
         * The calling application MUST NOT delete this object, as it will affect the
		 * constraint / relationship in the profile table.
         *
		 * @param key - This is the profile or ITSI group ID/pkey to be checked in the
		 *              Profile table
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group is to be deleted
         */
  		bool isGroupUsedInProfile(unsigned long key); 

	private:

        // Made private as it is not needed
		ProfileGroupHelper& operator=(const ProfileGroupHelper &);

		// get column name list
		void getColumnNames(std::vector<std::string>& columnNames);

        /**
         * reload()
         *
         * This method reloads the data from the database. It is called when a get... method
         * is called and the data state is not valid.
         *
         * pre: Either - this profile group was initially loaded from the database
         *      OR     - writeProfileGroup() has already been called
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
         * deleteProfileGroup & deleteITSIGroup depending on the initialisation of which type
         *
         * Remove this profile group from the database. Once this method has been executed, the
         * ProfileHelper object should be destroyed.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile group was initially loaded from the database
         *      OR     - writeProfileData() has already been called
         */
		void deleteProfileGroup();
		void deleteITSIGroup();

        /**
         * modifyExistingProfileGroup & modifyExistingITSIGroup
         *
         * This will update an existing profile group in the database with the settings currently saved here
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
        void modifyExistingProfileGroup();
		void modifyExistingITSIGroup();

        /**
         * addNewProfileToProfileGroup & addNewProfileToITSIGroup
         *
         * This will add a new profile in the database with the settings currently saved here
		 * The check functions are to check the database for repeated name which should be unique
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
        void addNewProfileToProfileGroup();
		void addNewProfileToITSIGroup();

        /**
         * initialiseMaxFieldLengths
         *
         * This retrieves the maximum field lengths for the name fields from the database.
         *
         * @exception DataException A DataException will be thrown if there is a 
         *            problem retrieving the data from the database.
         */
        void initialiseMaxFieldLengths();
		unsigned long getProfileGroupNameLength();
		unsigned long getITSIGroupNameLength();

        static const unsigned long DEFAULT_NAME_MAX_LENGTH;
        unsigned long m_nameMaxLength;
        
        bool m_isValidData;
        bool m_isNew;
		bool m_isProfileGroup;
		bool m_isITSIGroup;

        unsigned long m_profileKey;
		unsigned long m_ITSIkey;
        std::string m_name;
		std::string m_groupType;
        time_t m_dateCreated;
        time_t m_dateModified;
	};
}
#endif // !defined(AFX_PROFILEGROUPHELPER_H__982C4F5E_664A_4E00_A57D_E5F7926028C2__INCLUDED_)
