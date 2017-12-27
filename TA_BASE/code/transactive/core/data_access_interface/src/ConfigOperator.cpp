/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigOperator.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * ConfigOperator is a concrete implementation of IConfigOperator, which is in turn an implementation
  * of IOperator. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for operators.
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <algorithm>
#include <sstream>
#include <iterator>

#include "core/data_access_interface/src/ConfigOperator.h"
#include "core/data_access_interface/src/OperatorHelper.h"

#include "core/data_access_interface/src/ProfileAccessFactory.h"
#include "core/data_access_interface/src/IProfile.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    const std::string ConfigOperator::NAME = "Name";
    const std::string ConfigOperator::DESCRIPTION = "Description";
    const std::string ConfigOperator::PASSWORD = "Password";
    const std::string ConfigOperator::PROFILES = "Profiles";

    // The next unique identifier available
    long ConfigOperator::s_nextAvailableIdentifier = 0;


    ConfigOperator::ConfigOperator()
     : m_operatorHelper( new OperatorHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigOperator::ConfigOperator(const unsigned long key)
     : m_operatorHelper( new OperatorHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        std::vector<unsigned long> profiles = m_operatorHelper->getAssociatedProfileKeys();
        m_originalProfiles.insert(profiles.begin(),profiles.end());
        m_newProfiles.insert(profiles.begin(),profiles.end());
    }


    ConfigOperator::ConfigOperator(const unsigned long key, 
                 const std::string& name, 
                 const std::string& description,
                 const std::string& password,
                 bool isSystemOperator,
                 time_t dateCreated,
                 time_t dateModified,
                 bool loadProfile /*=false*/) 
     : m_operatorHelper( new OperatorHelper(key, name, description, password, isSystemOperator, dateCreated, dateModified, loadProfile) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        std::vector<unsigned long> profiles = m_operatorHelper->getAssociatedProfileKeys();
        m_originalProfiles.insert(profiles.begin(),profiles.end());
        m_newProfiles.insert(profiles.begin(),profiles.end());
    }


	ConfigOperator::ConfigOperator(const unsigned long row, TA_Base_Core::IData& data, std::vector<unsigned long>& profiles, std::vector<unsigned long>& regions)
     : m_operatorHelper( new OperatorHelper( row, data, profiles, regions )),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
		//TD18029 marc_bugfix Initialize these two vectors. These are used to check if the
		//associated profiles to the operator were changed
        m_originalProfiles.insert(profiles.begin(),profiles.end());
        m_newProfiles.insert(profiles.begin(),profiles.end());
		
    }


    ConfigOperator::ConfigOperator( const ConfigOperator& theConfigOperator)
     : m_operatorHelper( new OperatorHelper( *(theConfigOperator.m_operatorHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
        std::vector<unsigned long> profiles = m_operatorHelper->getAssociatedProfileKeys();
        // Don't insert anything in the original profiles since all the ones we have are new
        m_newProfiles.insert(profiles.begin(),profiles.end());
    }


    ConfigOperator::~ConfigOperator()
    {
        try
        {
            delete m_operatorHelper;
            m_operatorHelper = NULL;
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigOperator::isNew()
    {
        TA_ASSERT(m_operatorHelper != NULL, "isNew() has been called after Operator was deleted");

        return m_operatorHelper->isNew();
    }

    
    void ConfigOperator::setName(const std::string& name)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigOperator::setName");
        TA_ASSERT(m_operatorHelper != NULL, "setName() has been called after Operator was deleted");
    
        updateOperatorChanges(NAME, m_operatorHelper->getName(), name);

        m_operatorHelper->setName(name);

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigOperator::setName");
    }


    void ConfigOperator::setDescription(const std::string& description)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigOperator::setDescription");
        TA_ASSERT(m_operatorHelper != NULL, "setDescription() has been called after Operator was deleted");

        updateOperatorChanges(DESCRIPTION, m_operatorHelper->getDescription(), description);

        m_operatorHelper->setDescription(description);

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigOperator::setDescription");
    }


	void ConfigOperator::setPassword( const std::string password )
	{
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigOperator::setPassword");

        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");

        updateOperatorChanges(PASSWORD, m_operatorHelper->getPassword(), password);

		m_operatorHelper->setPassword( password );

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigOperator::setPassword");
	}


    void ConfigOperator::setAssociatedProfile( unsigned long profileKey, bool isAssociated)
    {
        FUNCTION_ENTRY("setAssociatedProfile");

        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
        
        // We just simulate the adding and removing of changes from the list as we need to check the
        // profiles manually when the list of changes is retrieved. It is too difficult to try and
        // manage profile changes in strings. Instead we use the m_originalProfiles and m_newProfiles
        // sets. The haveProfilesChanged() call below will maintain these sets.

        bool haveChanged = haveProfilesChanged(profileKey, isAssociated);

        if (haveChanged)
        {
            updateOperatorChanges(PROFILES, "old", "new");
        }
        else
        {
            updateOperatorChanges(PROFILES, "old", "old");
        }

        m_operatorHelper->setAssociatedProfile(profileKey, isAssociated);

        FUNCTION_EXIT;
    }
	

    unsigned long ConfigOperator::getKey()
    {
        TA_ASSERT(m_operatorHelper != NULL, "getKey() has been called after Operator was deleted");

        return m_operatorHelper->getKey();
    }


    std::string ConfigOperator::getName()
    {
        TA_ASSERT(m_operatorHelper != NULL, "getName() has been called after Operator was deleted");

        return m_operatorHelper->getName();
    }


    std::string ConfigOperator::getDescription()
    {
        TA_ASSERT(m_operatorHelper != NULL, "getDescription() has been called after Operator was deleted");
        return m_operatorHelper->getDescription();
    }


	std::string ConfigOperator::getPassword()
	{
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->getPassword();
	}
 

    bool ConfigOperator::isSystemOperator()
	{
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->isSystemOperator();
	}

        
    time_t ConfigOperator::getDateCreated()
    {
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->getDateCreated();
    }


    time_t ConfigOperator::getDateModified()
    {
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->getDateModified();
    }
 
	
	std::vector<IProfile*> ConfigOperator::getAssociatedProfiles()
	{
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->getAssociatedProfiles();
	}

    
    std::vector<unsigned long> ConfigOperator::getAssociatedProfileKeys()
    {
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->getAssociatedProfileKeys();
    }

    std::vector<unsigned long> ConfigOperator::getAssociatedRegionKeys()
    {
        TA_ASSERT(m_operatorHelper != NULL,"The OperatorHelper pointer is null.");
		return m_operatorHelper->getAssociatedRegionKeys();
    }


    void ConfigOperator::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigOperator::invalidate");
        TA_ASSERT(m_operatorHelper != NULL, "invalidate() has been called after Operator was deleted");

        m_operatorHelper->invalidate();
        m_operatorChanges.clear();
        m_newProfiles = m_originalProfiles;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigOperator::invalidate");
    }


    ItemChanges ConfigOperator::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");

        ItemChanges copyOfItemChanges = m_operatorChanges;

        // First see if the profiles have changed. If the profiles have changed we need to generate nice lists
        ItemChanges::iterator matching = m_operatorChanges.find(PROFILES);
        if (matching != m_operatorChanges.end())
        {
            copyOfItemChanges[PROFILES].newValue = getProfileNames(m_newProfiles);
            copyOfItemChanges[PROFILES].oldValue = getProfileNames(m_originalProfiles);
        }

        // Now if the password has changed we need to make sure we don't display it in the messages.
        // Instead just display some question marks.
        matching = m_operatorChanges.find(PASSWORD);
        if (matching != m_operatorChanges.end())
        {
            copyOfItemChanges[PASSWORD].newValue = "???";
            copyOfItemChanges[PASSWORD].oldValue = "???";
        }

        // Now return these changes
        FUNCTION_EXIT;
        return copyOfItemChanges;
    }


    void ConfigOperator::deleteThisObject()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigOperator::deleteThisObject");
        TA_ASSERT(m_operatorHelper != NULL, "deleteThisObject() has been called after Operator was deleted");

        // Delete the operator from the database
        m_operatorHelper->deleteOperator();
        
        LOG( SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "About to delete OperatorHelper pointer m_operatorHelper");

        delete m_operatorHelper;
        m_operatorHelper = NULL;

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigOperator::deleteThisObject");
    }


    void ConfigOperator::applyChanges()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "ConfigOperator::applyChanges");
        TA_ASSERT(m_operatorHelper != NULL, "applyChanges() has been called after Operator was deleted");

        m_operatorHelper->writeOperatorData();

        m_originalProfiles = m_newProfiles;
        m_operatorChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigOperator::applyChanges");
    }


    void ConfigOperator::updateOperatorChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateOperatorChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_operatorChanges.find(name);

        if (matching != m_operatorChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_operatorChanges.erase(matching);
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
            m_operatorChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }

    
    bool ConfigOperator::haveProfilesChanged(unsigned long profileKey, bool isAssociated)
    {
        FUNCTION_ENTRY("haveProfilesChanged");

        if (isAssociated)
        {
            // This means we're adding a new profile so add it to the list of new profiles
            m_newProfiles.insert(profileKey);
        }
        else
        {
            // This means we're removing a profile so remove it from the list of new profiles
            m_newProfiles.erase( profileKey );
        }

        // Now figure out if the two sets (original and new) are different.
        std::ostringstream setDifference;
        std::set_symmetric_difference(m_newProfiles.begin(), m_newProfiles.end(),
                                      m_originalProfiles.begin(), m_originalProfiles.end(),
                                      std::ostream_iterator<unsigned long>(setDifference,""));

        // If this is empty then there were no set differences
        if (setDifference.str().empty())
        {
            FUNCTION_EXIT;
            return false;
        }

        // Otherwise there were set differences
        FUNCTION_EXIT;
        return true;

    }


    std::string ConfigOperator::getProfileNames(std::set<unsigned long>& profileKeys)
    {
        FUNCTION_ENTRY("getProfileNames");
        std::string names("");

        for (std::set<unsigned long>::iterator iter = profileKeys.begin(); iter != profileKeys.end(); ++iter)
        {
            if (iter != profileKeys.begin())
            {
                // If this is not the first element then we need to insert a comma
                names += ",";
            }

            try
            {
                IProfile* profile = ProfileAccessFactory::getInstance().getProfile(*iter);
                names += profile->getName();
            }
            catch ( const DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the profile name. Using 'Unknown'");
                names += "Unknown";
            }
            catch ( const DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the profile name. Using 'Unknown'");
                names += "Unknown";
            }
        }

		FUNCTION_EXIT;
        return names;
    }

} // closes TA_Base_Core

