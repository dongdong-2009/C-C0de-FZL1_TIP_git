/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/radio/src/ConfigITSIGroup.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * ConfigITSIGroup is a concrete implementation of IConfigITSIGroup, which is in turn an implementation
  * of IITSIGroup. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for ITSI groups.
  */

#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

// ConfigITSIGroup.cpp: implementation of the ConfigITSIGroup class.
//
//////////////////////////////////////////////////////////////////////

#include <sstream>

#include "core/data_access_interface/radio/src/ConfigITSIGroup.h"
#include "core/data_access_interface/src/ProfileGroupHelper.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
		// These are the names of the fixed fields that will be used in places where we are creating
		// strings that will be shown to the user.
		const std::string ConfigITSIGroup::NAME = "Name";

			// The next unique identifier available
		long ConfigITSIGroup::s_nextAvailableIdentifier = 0;

		//////////////////////////////////////////////////////////////////////
		// Construction/Destruction
		//////////////////////////////////////////////////////////////////////
		/**
		 * ConfigITSIGroup (constructor)
		 *
		 * Constructs a new instance of the ConfigITSIGroup with no key. This is used when
		 * creating a *new* ITSI - that is, one that does not yet exist in the database.
		 */
		ConfigITSIGroup::ConfigITSIGroup()
			: m_ITSIGroupHelper( new ProfileGroupHelper(ProfileGroupHelper::ITSI) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_key(0)
		{
		}

        /**
         * ConfigITSIGroup (constructor)
         *
         * Constructs a new instance of ConfigITSIGroup with the specified key. This is used 
         * when creating a ConfigITSIGroup based around an existing profile in the database.
         *
         * @param key The key to this profile in the database.
         */
        ConfigITSIGroup::ConfigITSIGroup(const unsigned long key)
			: m_ITSIGroupHelper( new ProfileGroupHelper(ProfileGroupHelper::ITSI, key) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_key(key)
		{
		}

        /**
         * ConfigITSIGroup (copy constructor)
         *
         * @param ConfigITSIGroup& - The profile to make a copy of.
         */
        ConfigITSIGroup::ConfigITSIGroup( const ConfigITSIGroup& theConfigITSIGroup)
			: m_ITSIGroupHelper( new ProfileGroupHelper( *(theConfigITSIGroup.m_ITSIGroupHelper)) ), 
		   m_uniqueIdentifier(s_nextAvailableIdentifier++),
		   m_key(theConfigITSIGroup.m_key)
		{
		}

		/**
		 * Destructor
		 */
		ConfigITSIGroup::~ConfigITSIGroup()
		{
			try
			{
				if (m_ITSIGroupHelper != NULL)
				{
					delete m_ITSIGroupHelper;
					m_ITSIGroupHelper = NULL;
				}
				m_itsiGroupChanges.clear();
			}
			catch ( ... )
			{
				LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", 
					"Caught failure to destroy ITSIGroupHelper object in destructor");
			}
		}

		/**********************************
		 * Virtural fucntions from IProfileGroup
		 */

        /**
         * getKey
         *
         * Returns the ITSI group key for this ITSI group.
         *
         * @return The ITSI group key for this ITSI group as an unsigned long
         */
        unsigned long ConfigITSIGroup::getKey()
		{
			TA_ASSERT(m_ITSIGroupHelper != NULL, "getKey() has been called after helper was deleted");

			return m_ITSIGroupHelper->getKey();
		}
        
        /**
         * getName
         *
         * Returns the ITSI group name for this ITSI group.
         *
         * @return The ITSI group name as a string
         */
        std::string ConfigITSIGroup::getName()
		{
			TA_ASSERT(m_ITSIGroupHelper != NULL, "getName() has been called after helper was deleted");

			return m_ITSIGroupHelper->getName();
		}

        /**
         * invalidate
         *
         * Mark the data contained by this group as invalid. The next call to get...() 
         * following a call to invalidate() will cause all the data to be reloaded from
         * the database.
         */
        void ConfigITSIGroup::invalidate()
		{
			FUNCTION_ENTRY("invalidate");

			TA_ASSERT(m_ITSIGroupHelper != NULL, "invalidate() has been called after helper was deleted");

			m_ITSIGroupHelper->invalidate();
			m_itsiGroupChanges.clear();

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
        ItemChanges ConfigITSIGroup::getAllItemChanges()
		{
			FUNCTION_ENTRY("getAllItemChanges");

			// We must go through and find name and group type into displayable
			// strings for the user.
			ItemChanges changesToReturn;

			for (ItemChanges::iterator entry = m_itsiGroupChanges.begin(); entry != m_itsiGroupChanges.end(); ++entry)
			{
				if ( entry->first == NAME )
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
        bool ConfigITSIGroup::isNew()
		{
			FUNCTION_ENTRY("isNew()");

			TA_ASSERT(m_ITSIGroupHelper != NULL, "ITSIGroupHelper is NULL");

			return m_ITSIGroupHelper->isNew();

			FUNCTION_EXIT;
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
        void ConfigITSIGroup::setName(const std::string& name)
		{
			FUNCTION_ENTRY("setName");

			TA_ASSERT(m_ITSIGroupHelper != NULL, "Helper was NULL");
    
			updateChanges(NAME, m_ITSIGroupHelper->getName(), name);

			m_ITSIGroupHelper->setName(name);

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
        void ConfigITSIGroup::applyChanges()
		{
			FUNCTION_ENTRY("applyChanges");

			TA_ASSERT(m_ITSIGroupHelper != NULL, "applyChanges() has been called after helper was deleted");

			m_ITSIGroupHelper->writeData();

			m_itsiGroupChanges.clear();

			FUNCTION_EXIT;
		}


		/**********************************
		 * Virtural fucntions from IConfigProfileGroup
		 */

        /**
         * getDateCreated
         *
         * Returns the date created for this ITSI
         *
         * @return The date created for this ITSI as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be an 
         *            unsigned long), or if the wrong ammount of data is retrieved, or if the
         *            the entity key is invalid (and this is not a new entity).
         */
        time_t ConfigITSIGroup::getDateCreated()
		{
			TA_ASSERT(m_ITSIGroupHelper != NULL,"The helper pointer is null.");
			return m_ITSIGroupHelper->getDateCreated();
		}


        /**
         * getDateModified
         *
         * Returns the date modified for this ITSI group.
         *
         * @return The date modified for this ITSI group as a time_t.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a
         *            problem establishing a connection with the database, or if an
         *            error is encountered while retrieving data.
         * @exception DataException A DataException will be thrown if the data cannot be 
         *            converted to the required format (e.g. TYPEKEY should be only 
         *            either Profile or ITSI).
         */
        time_t ConfigITSIGroup::getDateModified()
		{
			TA_ASSERT(m_ITSIGroupHelper != NULL,"The helper pointer is null.");
			return m_ITSIGroupHelper->getDateModified();
		}

		        /**
         * deleteThisObject
         *
         * Removes this ITSI group from the database. 
         * The calling application MUST then delete this object, as it makes no sense
         * to keep it any longer.
         *
         * @exception DatabaseException A DatabaseException will be thrown if there is a 
         *            problem writing the data to the database.
         *
         * pre: Either - this ITSI was initially loaded from the database
         *      OR     - apply() has already been called
         *      This ITSI group has not been deleted
         */
        void ConfigITSIGroup::deleteThisObject()
		{
			FUNCTION_ENTRY("deleteThisObject");

			TA_ASSERT(m_ITSIGroupHelper != NULL, "deleteThisObject() has been called after helper was deleted");

			// Delete the profile from the database
			m_ITSIGroupHelper->deleteGroup();
        
			delete m_ITSIGroupHelper;
			m_ITSIGroupHelper = NULL;

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
		bool ConfigITSIGroup::isGroupUsedInProfile(unsigned long key)
		{
			FUNCTION_ENTRY("isGroupUsedInProfile");

			TA_ASSERT(m_ITSIGroupHelper != NULL, "Helper was NULL");

			FUNCTION_EXIT;
			return m_ITSIGroupHelper->isGroupUsedInProfile(key);
		}

		void ConfigITSIGroup::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
		{
			FUNCTION_ENTRY("updateChanges");

			// Search for this name in the map
			ItemChanges::iterator matching = m_itsiGroupChanges.find(name);

			if (matching != m_itsiGroupChanges.end())
			{
				// We already have this attribute name in the list.
				if (newValue == matching->second.oldValue)
				{
					// The new value matches the old value so we might as well remove this from the list as it
					// is not a valid change
					m_itsiGroupChanges.erase(matching);
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
				m_itsiGroupChanges.insert( ItemChanges::value_type( name, values ) );
			}

			FUNCTION_EXIT;
		}

}