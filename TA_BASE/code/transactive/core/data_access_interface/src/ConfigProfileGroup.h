/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigProfileGroup.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigProfileGroup is a concrete implementation of IConfigProfileGroup, which is in turn an implementation
  * of IProfileGroup. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for profile groups.
  */

// ConfigProfileGroup.h: interface for the ConfigProfileGroup class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_CONFIGPROFILEGROUP_H__E5A7E5AD_4A9E_4162_A345_4E5DB4BE529C__INCLUDED_)
#define AFX_CONFIGPROFILEGROUP_H__E5A7E5AD_4A9E_4162_A345_4E5DB4BE529C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <vector>

#include "core/data_access_interface/src/IConfigProfileGroup.h"
#include "core/data_access_interface/src/ProfileGroup.h"
#include "core/data_access_interface/radio/src/ITSIGroup.h"

namespace TA_Base_Core
{
    class ProfileGroupHelper;

	class ConfigProfileGroup : public IConfigProfileGroup  
	{
		public:

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of the ConfigProfileGroup with no key. This is used when
         * creating a *new* profile - that is, one that does not yet exist in the database.
         */
		ConfigProfileGroup();

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of the ConfigProfileGroup with no key. This is used when
         * creating a *new* profile - that is, one that does not yet exist in the database.
         */
		ConfigProfileGroup(std::string type);

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of ConfigProfileGroup with the specified key. This is used 
         * when creating a ConfigProfileGroup based around an existing profile in the database.
         *
         * @param key The key to this profile in the database.
         */
		ConfigProfileGroup(const unsigned long key);

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of ConfigProfileGroup with the specified key. This is used 
         * when creating a ConfigProfileGroup based around an existing profile in the database.
         *
         * @param key The key to this profile in the database.
         */
		ConfigProfileGroup(std::string type, const unsigned long key);

		/**
         * Constructor
         *
         * Construct a ConfigProfileGroup class based around dataset
         */
		ConfigProfileGroup(const unsigned long row, TA_Base_Core::IData& data);

        /**
         * ConfigProfileGroup (copy constructor)
         *
         * @param ConfigProfileGroup& - The profile to make a copy of.
         */
        ConfigProfileGroup( const ConfigProfileGroup& theConfigProfileGroup);

		/**
		 * Destructor
		 */
		virtual ~ConfigProfileGroup();

		/**********************************
		 * Virtural fucntions from IProfileGroup
		 */

        /**
         * getKey
         *
         * Returns the profile group key for this profile group.
         *
         * @return The profile group key for this profile group as an unsigned long
         */
        virtual unsigned long getKey();

        
        /**
         * getName
         *
         * Returns the profile group name for this profile group.
         *
         * @return The profile group name as a string
         */
        virtual std::string getName();


        /**
         * getType
         *
         * Returns the profile group type name for this profile group.
         * Currently it is either PROFILE or ITSI
         *	
         * @return The profile group type name as a string
         */
		virtual std::string getType();

        /**
         * invalidate
         *
         * Mark the data contained by this subsystemState as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        virtual void invalidate();

		/**********************************
		 * Virtural fucntions from IConfigItem
		 */

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
        virtual bool hasChanged()
		{
            // If the profile group changes is empty then nothing has changed
            return !m_profileGroupChanges.empty();
		};


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


		/**********************************
		 * Virtural fucntions from IConfigProfileGroup
		 */

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
         * Returns the date modified for this profile group.
         *
         * @return The date modified for this profile group as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be only 
         *            either Profile or ITSI).
         */
        virtual time_t getDateModified();

        /**
         * setType
         *
         * Sets the type of this profile group either ITSI or PROFILE or BOTH
         *
         * @param unsigned long - The type of profile group
         *
         * pre: This profile group has not been deleted
         */
        virtual void setType(const std::string);

        /**
         * deleteThisObject
         *
         * Removes this profile group from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group has not been deleted
         */
        virtual void deleteThisObject();

        /**
         * isGroupIsUsedInProfile
         *
         * Checks whether this profile group is used in profile table from the database. 
         * The calling application MUST NOT delete this object, as it will affect the
		 * constraint / relationship in the profile table.
         *
		 * @param key is the profile group primary key for check in profile table
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group is to be deleted
         */
		bool isGroupUsedInProfile(unsigned long key);


		private:

        // Assignment profile not used so it is made private
		ConfigProfileGroup& operator=(const ConfigProfileGroup&);

        ProfileGroupHelper* m_profileGroupHelper;

        // These are the names of the fixed fields that will be used in places where we are creating
        // strings that will be shown to the user.
        static const std::string NAME;
        static const std::string TYPE;

		std::string m_type; // By changing the type the config item will load the appropriate helper and data
		unsigned long m_key;
        unsigned long m_uniqueIdentifier; // We cannot use keys for identification of profile groups in the
                                          // Configuration Editor because new profile groups do not have keys.
                                          // Therefore we use this method for identification.
        static long s_nextAvailableIdentifier; // The next unique identifier available

        ItemChanges m_profileGroupChanges; // This is a map of all changes made to the profile group so that they are
                                       // recorded and can be sent in audit or online update messages.
 
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
	};
}

#endif // !defined(AFX_CONFIGPROFILEGROUP_H__E5A7E5AD_4A9E_4162_A345_4E5DB4BE529C__INCLUDED_)
