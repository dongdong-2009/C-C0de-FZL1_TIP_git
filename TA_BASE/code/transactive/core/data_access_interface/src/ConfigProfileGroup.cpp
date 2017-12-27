/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigProfileGroup.cpp $
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

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

// ConfigProfileGroup.cpp: implementation of the ConfigProfileGroup class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>
#include <algorithm>

#include "core/data_access_interface/src/ConfigProfileGroup.h"
#include "core/data_access_interface/src/ProfileGroupHelper.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;


namespace TA_Base_Core
{

		// These are the names of the fixed fields that will be used in places where we are creating
		// strings that will be shown to the user.
		const std::string ConfigProfileGroup::NAME = "Name";
		const std::string ConfigProfileGroup::TYPE = "Type";

	    // The next unique identifier available
	    long ConfigProfileGroup::s_nextAvailableIdentifier = 0;

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of the ConfigProfileGroup with no key. This is used when
         * creating a *new* profile - that is, one that does not yet exist in the database.
         */
		ConfigProfileGroup::ConfigProfileGroup()
			: m_profileGroupHelper( new ProfileGroupHelper(ProfileGroupHelper::PROFILE) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_type(ProfileGroupHelper::PROFILE),
		   m_key(0)
		{
		}

		/*
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of the ConfigProfileGroup with no key. This is used when
         * creating a *new* profile - that is, one that does not yet exist in the database.
         */
		ConfigProfileGroup::ConfigProfileGroup(std::string type)
			: m_profileGroupHelper( new ProfileGroupHelper(type) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_type(type),
		   m_key(0)
		{
		}

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of ConfigProfileGroup with the specified key. This is used 
         * when creating a ConfigProfileGroup based around an existing profile in the database.
         *
         * @param key The key to this profile in the database.
         */
        ConfigProfileGroup::ConfigProfileGroup(const unsigned long key)
			: m_profileGroupHelper( new ProfileGroupHelper(ProfileGroupHelper::PROFILE, key) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_type(ProfileGroupHelper::PROFILE),
		   m_key(key)
		{
		}

        /**
         * ConfigProfileGroup (constructor)
         *
         * Constructs a new instance of ConfigProfileGroup with the specified key. This is used 
         * when creating a ConfigProfileGroup based around an existing profile in the database.
         *
         * @param key The key to this profile in the database.
         */
        ConfigProfileGroup::ConfigProfileGroup(std::string type, const unsigned long key)
			: m_profileGroupHelper( new ProfileGroupHelper(type, key) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_type(type),
		   m_key(key)
		{
		}


		/**
         * Constructor
         *
         * Construct a ConfigProfileGroup class based around dataset
         */
		ConfigProfileGroup::ConfigProfileGroup(const unsigned long row, TA_Base_Core::IData& data)
			:
			m_profileGroupHelper( new ProfileGroupHelper(ProfileGroupHelper::PROFILE, row, data) ), 
			m_uniqueIdentifier(s_nextAvailableIdentifier++)
		{
		}

        /**
         * ConfigProfileGroup (copy constructor)
         *
         * @param ConfigProfileGroup& - The profile to make a copy of.
         */
        ConfigProfileGroup::ConfigProfileGroup( const ConfigProfileGroup& theConfigProfileGroup)
			: m_profileGroupHelper( new ProfileGroupHelper( *(theConfigProfileGroup.m_profileGroupHelper)) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_type(theConfigProfileGroup.m_type),
		   m_key(theConfigProfileGroup.m_key)
		{
		}

		/**
		 * Destructor
		 */
		ConfigProfileGroup::~ConfigProfileGroup()
		{
			try
			{
				if (m_profileGroupHelper != NULL)
				{
					delete m_profileGroupHelper;
					m_profileGroupHelper = NULL;
				}
				m_profileGroupChanges.clear();
			}
			catch ( ... )
			{
				LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
					"Caught failure to destroy ProfileGroupHelper object in destructor");
			}
		}

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
        unsigned long ConfigProfileGroup::getKey()
		{
			TA_ASSERT(m_profileGroupHelper != NULL, "getKey() has been called after helper was deleted");

			return m_profileGroupHelper->getKey();
		}
        
        /**
         * getName
         *
         * Returns the profile group name for this profile group.
         *
         * @return The profile group name as a string
         */
        std::string ConfigProfileGroup::getName()
		{
			TA_ASSERT(m_profileGroupHelper != NULL, "getName() has been called after helper was deleted");

			return m_profileGroupHelper->getName();
		}

        /**
         * getType
         *
         * Returns the profile group type name for this profile group.
         * Currently it is either PROFILE or ITSI
         *	
         * @return The profile group type name as a string
         */
		std::string ConfigProfileGroup::getType()
		{
			TA_ASSERT(m_profileGroupHelper != NULL, "getType() has been called after helper was deleted");

			return m_profileGroupHelper->getGroupType();
		}

        /**
         * invalidate
         *
         * Mark the data contained by this subsystemState as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void ConfigProfileGroup::invalidate()
		{
			FUNCTION_ENTRY("invalidate");

			TA_ASSERT(m_profileGroupHelper != NULL, "invalidate() has been called after helper was deleted");

			m_profileGroupHelper->invalidate();
			m_profileGroupChanges.clear();

			FUNCTION_EXIT;
		}

		/**********************************
		 * Virtural fucntions from IConfigItem
		 */

        /**
         * getAllItemChanges
         *
         * This is called to retrieve all changes made to the item. If the map returned is empty then
         * no changes have been made. This must be called before the changes are applied or it will be
         * cleared.
         *
         * @return ItemChanges - The map containing all item changes
         */
        ItemChanges ConfigProfileGroup::getAllItemChanges()
		{
			FUNCTION_ENTRY("getAllItemChanges");

			// We must go through and find name and group type into displayable
			// strings for the user.
			ItemChanges changesToReturn;

			for (ItemChanges::iterator entry = m_profileGroupChanges.begin(); entry != m_profileGroupChanges.end(); ++entry)
			{
				if ( (entry->first == NAME) || (entry->first == TYPE) )
				{
					// Don't need to modify this entry so just add it and continue onto the next one
					changesToReturn.insert( ItemChanges::value_type( entry->first, entry->second ) );
					continue;
				}
			}

			FUNCTION_EXIT;
			return changesToReturn;
 		}
        
        /**
         * isNew
         *
         * This should be called to determine if this item is new. A new item is one that has not
         * yet been applied to the database and been given a pkey and create date.
         *
         * @return bool - This will return true if the item has not been applied to the database.
         */
        bool ConfigProfileGroup::isNew()
		{
			FUNCTION_ENTRY("isNew()");

			TA_ASSERT(m_profileGroupHelper != NULL, "ProfileGroupHelper is NULL");

			FUNCTION_EXIT;
			return m_profileGroupHelper->isNew();
		}

        
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
        void ConfigProfileGroup::setName(const std::string& name)
		{
			FUNCTION_ENTRY("setName");

			TA_ASSERT(m_profileGroupHelper != NULL, "Helper was NULL");
    
			updateChanges(NAME, m_profileGroupHelper->getName(), name);

			m_profileGroupHelper->setName(name);

			FUNCTION_EXIT;
		}


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
        void ConfigProfileGroup::applyChanges()
		{
			FUNCTION_ENTRY("applyChanges");

			TA_ASSERT(m_profileGroupHelper != NULL, "applyChanges() has been called after helper was deleted");

			m_profileGroupHelper->writeData();

			m_profileGroupChanges.clear();

			FUNCTION_EXIT;
		}


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
        time_t ConfigProfileGroup::getDateCreated()
		{
			TA_ASSERT(m_profileGroupHelper != NULL,"The helper pointer is null.");
			return m_profileGroupHelper->getDateCreated();
		}


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
        time_t ConfigProfileGroup::getDateModified()
		{
			TA_ASSERT(m_profileGroupHelper != NULL,"The helper pointer is null.");
			return m_profileGroupHelper->getDateModified();
		}

        /**
         * setType
         *
         * Sets the type of this profile group either ITSI or PROFILE or BOTH
         *
         * @param unsigned long - The type of profile group
         *
         * pre: This profile group has not been deleted
         */
        void ConfigProfileGroup::setType(const std::string type)
		{
			FUNCTION_ENTRY("setType");

			TA_ASSERT(m_profileGroupHelper != NULL, "Helper was NULL");

			std::string newTypeName = type;
			std::string oldTypeName = m_profileGroupHelper->getGroupType();
			updateChanges(TYPE, oldTypeName, newTypeName);

			m_profileGroupHelper->setGroupType(type);

			FUNCTION_EXIT;
		}

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
        void ConfigProfileGroup::deleteThisObject()
		{
			FUNCTION_ENTRY("deleteThisObject");

			TA_ASSERT(m_profileGroupHelper != NULL, "deleteThisObject() has been called after helper was deleted");

			// Delete the profile from the database
			m_profileGroupHelper->deleteGroup();
        
			delete m_profileGroupHelper;
			m_profileGroupHelper = NULL;

			FUNCTION_EXIT;
		}

        /**
         * isGroupIsUsedInProfile
         *
         * Checks whether this profile group is used in profile table from the database. 
         * The calling application MUST NOT delete this object, as it will affect the
		 * constraint / relationship in the profile table.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this profile was initially loaded from the database
         *      OR     - apply() has already been called
         *      This profile group is to be deleted
         */
		bool ConfigProfileGroup::isGroupUsedInProfile(unsigned long key)
		{
			FUNCTION_ENTRY("isGroupUsedInProfile");

			TA_ASSERT(m_profileGroupHelper != NULL, "Helper was NULL");

			FUNCTION_EXIT;
			return m_profileGroupHelper->isGroupUsedInProfile(key);
		}

		void ConfigProfileGroup::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
		{
			FUNCTION_ENTRY("updateChanges");

			// Search for this name in the map
			ItemChanges::iterator matching = m_profileGroupChanges.find(name);

			if (matching != m_profileGroupChanges.end())
			{
				// We already have this attribute name in the list.
				if (newValue == matching->second.oldValue)
				{
					// The new value matches the old value so we might as well remove this from the list as it
					// is not a valid change
					m_profileGroupChanges.erase(matching);
				}
				else
				{
					//Update the stored new value.
					matching->second.newValue = newValue;
				}

				FUNCTION_EXIT;
				return;
			}

			// The first time we come in we should get to here and we set the old value to be the original value of the attribute
			if (oldValue != newValue)
			{
				// If the old and new values are different then we can add this item to the map
				Values values;
				values.newValue = newValue;
				values.oldValue = oldValue;
				m_profileGroupChanges.insert( ItemChanges::value_type( name, values ) );
			}

			FUNCTION_EXIT;
		}
}