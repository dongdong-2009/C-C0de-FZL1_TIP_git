/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/src/ConfigProfile.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2015/01/29 17:51:30 $
  * Last modified by:  $Author: liwei.gong $
  * 
  * ConfigProfile is a concrete implementation of IConfigProfile, which is in turn an implementation
  * of IProfile. It is to be used by the Configuration Editor, and other applications
  * that wish to change the database content for profiles.
  */


#ifdef __WIN32__
#pragma warning(disable:4786)
#endif

#include <sstream>
#include <algorithm>

#include "core/data_access_interface/src/ConfigProfile.h"
#include "core/data_access_interface/src/ProfileHelper.h"

#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ScadaDisplayAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/IScadaDisplay.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    // These are the names of the fixed fields that will be used in places where we are creating
    // strings that will be shown to the user.
    const std::string ConfigProfile::NAME = "Name";
    const std::string ConfigProfile::TYPE = "Type";
    const std::string ConfigProfile::ACCESS_CONTROL_GROUP = "Profile Group";
    const std::string ConfigProfile::RADIO_LOGIN_GROUP = "Radio Login Group";
    const std::string ConfigProfile::PA_PRIORITY_GROUP = "PA Priority Group";
    const std::string ConfigProfile::PROFILE_LOCATION_ASSOCIATION = "Profile/Location Association";

    // The next unique identifier available
    long ConfigProfile::s_nextAvailableIdentifier = 0;


    ConfigProfile::ConfigProfile()
     : m_profileHelper( new ProfileHelper() ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigProfile::ConfigProfile(const unsigned long key)
     : m_profileHelper( new ProfileHelper(key) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


	ConfigProfile::ConfigProfile(const unsigned long row, TA_Base_Core::IData& data)
	 : m_profileHelper( new ProfileHelper(row, data) ),
	   m_uniqueIdentifier(s_nextAvailableIdentifier++)
	{
	}


    ConfigProfile::ConfigProfile( const ConfigProfile& theConfigProfile)
     : m_profileHelper( new ProfileHelper( *(theConfigProfile.m_profileHelper)) ),
       m_uniqueIdentifier(s_nextAvailableIdentifier++)
    {
    }


    ConfigProfile::~ConfigProfile()
    {
        try
        {
            if (m_profileHelper != NULL)
            {
                delete m_profileHelper;
                m_profileHelper = NULL;
            }
        }
        catch ( ... )
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor");
        }
    }


    bool ConfigProfile::isNew()
    {
        TA_ASSERT(m_profileHelper != NULL, "isNew() has been called after profile was deleted");

        return m_profileHelper->isNew();
    }

    
    ItemChanges ConfigProfile::getAllItemChanges()
    {
        FUNCTION_ENTRY("getAllItemChanges");

        // We must go through and find all location and display changes and convert them into displayable
        // strings for the user.
        ItemChanges changesToReturn;

        for (ItemChanges::iterator entry = m_profileChanges.begin(); entry != m_profileChanges.end(); ++entry)
        {
            if ((entry->first == NAME) || (entry->first == TYPE) )
            {
                // Don't need to modify this entry so just add it and continue onto the next one
                changesToReturn.insert( ItemChanges::value_type( entry->first, entry->second ) );
                continue;
            }

            std::string locationLabel = getNiceLocationLabel( entry->first );
            Values values;
            values.newValue = getNiceDisplayLabel( entry->second.newValue );
            values.oldValue = getNiceDisplayLabel( entry->second.oldValue );
            changesToReturn.insert( ItemChanges::value_type( locationLabel, values ) );
        }

        FUNCTION_EXIT;
        return changesToReturn;
    }


    std::string ConfigProfile::getNiceLocationLabel(std::string oldLabel)
    {
        FUNCTION_ENTRY("getNiceLocationLabel");

        // The oldLabel will be "locationKey displayNumber" so we must break this up
        unsigned long locationKey;
        unsigned long screenNumber;

        // Extract the parts from the label
        std::istringstream labelName(oldLabel);
        labelName >> locationKey >> screenNumber;

        // Look up the location name from the database
        try
        {
            std::ostringstream newLabelName;
            ILocation* location = LocationAccessFactory::getInstance().getLocationByKey(locationKey);

            // Set up the new, nice to look at label and set it in the map
            newLabelName << "Location " << location->getName() << ", Display " << screenNumber;
        
            delete location;
            location = NULL;

            FUNCTION_EXIT;
            return newLabelName.str();
        }
        catch( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the location name. The user will see an icky location key in the audit message.");
        }
        catch( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the location name. The user will see an icky location key in the audit message.");
        }

        FUNCTION_EXIT;
        return oldLabel;
    }


    std::string ConfigProfile::getNiceDisplayLabel(std::string oldLabel)
    {
        FUNCTION_ENTRY("getNiceDisplayLabel");

        // If we just have an empty string then just return an empty string.
        if (oldLabel.empty())
        {
            return "";
        }

        // The oldLabel will be "displayKey" so we must pull this out
        unsigned long displayKey;

        // Extract the parts from the label
        std::istringstream labelName(oldLabel);
        labelName >> displayKey;

        // Look up the display name from the database
        try
        {
            std::string newLabelName;
            IScadaDisplay* display = ScadaDisplayAccessFactory::getInstance().getScadaDisplay( displayKey );

            newLabelName = display->getName();
            
            delete display;
            display = NULL;

            FUNCTION_EXIT;
            return newLabelName;
        }
        catch( const DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Could not retrieve the display name. The user will see an icky display key in the audit message.");
        }
        catch( const DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the display name. The user will see an icky display key in the audit message.");
        }

		FUNCTION_EXIT;
        return oldLabel;
       
    }

        
    void ConfigProfile::setName(const std::string& name)
    {
        FUNCTION_ENTRY("setName");

        TA_ASSERT(m_profileHelper != NULL, "Helper was NULL");
    
        updateChanges(NAME, m_profileHelper->getName(), name);

        m_profileHelper->setName(name);

        FUNCTION_EXIT;
    }

    
    void ConfigProfile::setType(const unsigned long type)
    {
        FUNCTION_ENTRY("setType");

        TA_ASSERT(m_profileHelper != NULL, "Helper was NULL");

        std::string newTypeName = m_profileHelper->getTypeName( type );
        std::string oldTypeName = m_profileHelper->getTypeName( m_profileHelper->getType() );
        updateChanges(TYPE, oldTypeName, newTypeName);

        m_profileHelper->setType(type);

        FUNCTION_EXIT;
    }


    void ConfigProfile::removeDisplay(unsigned long location, unsigned long screenNumber)
    {
        FUNCTION_ENTRY("removeDisplay");

        TA_ASSERT(m_profileHelper != NULL, "Helper was NULL");

        // Store the name of the field as locationkey,screenNumber. We will update to nice strings when
        // the user requests them
        std::ostringstream updateName;
        updateName << location << " " << screenNumber;

        // Now store the display keys as the new and old values. Once again we will update to nice strings when
        // the user requests them. For this example the new display will be an empty string

        std::ostringstream oldDisplay;
        try
        {
            oldDisplay << m_profileHelper->getDisplayKey(location,screenNumber);
        }
        catch ( const DataException& ex)
        {
            if (ex.getFailType() == DataException::NO_VALUE )
            {
                // This means there was no display
                oldDisplay.str("");
            }
        }

        updateChanges( updateName.str(), oldDisplay.str(), "");

        m_profileHelper->removeDisplay(location,screenNumber);

        FUNCTION_EXIT;
    }


    void ConfigProfile::removeParameter(unsigned long location, const std::string& parameterName)
    {
        FUNCTION_ENTRY("removeDisplay");

        TA_ASSERT(m_profileHelper != NULL, "Helper was NULL");

        // Store the name of the field as locationkey,screenNumber. We will update to nice strings when
        // the user requests them
        std::ostringstream updateName;
        updateName << location << " " << parameterName;

        // Now store the display keys as the new and old values. Once again we will update to nice strings when
        // the user requests them. For this example the new display will be an empty string

        std::string oldDisplay;
        try
        {
            oldDisplay = m_profileHelper->getParameterActualValue(location,parameterName);
        }
        catch ( const DataException& ex)
        {
            if (ex.getFailType() == DataException::NO_VALUE )
            {
                // This means there was no display
                oldDisplay = "";
            }
        }

        updateChanges( updateName.str(), oldDisplay, "");

        m_profileHelper->removeParameter(location, parameterName);

        FUNCTION_EXIT;
    }

        
    void ConfigProfile::setDisplay(unsigned long location, unsigned long screenNumber, long display)
    {
        FUNCTION_ENTRY("setDisplay");

        TA_ASSERT(m_profileHelper != NULL, "Helper was NULL");

        // Store the name of the field as locationkey,screenNumber. We will update to nice strings when
        // the user requests them
        std::ostringstream updateName;
        updateName << location << " " << screenNumber;

        // Now store the display keys as the new and old values. Once again we will update to nice strings when
        // the user requests them
        std::ostringstream newDisplay;
        newDisplay << display;

        std::ostringstream oldDisplay;
        try
        {
            oldDisplay << m_profileHelper->getDisplayKey(location,screenNumber);
        }
        catch ( const DataException& ex)
        {
            if (ex.getFailType() == DataException::NO_VALUE )
            {
                // This means there was no display
                oldDisplay.str("");
            }
        }

        updateChanges( updateName.str(), oldDisplay.str(), newDisplay.str());

        m_profileHelper->setDisplay(location,screenNumber, display);

        FUNCTION_EXIT;
    }


    void ConfigProfile::setParameterValue(unsigned long location, const std::string& parameterName, const std::string& parameterValue)
    {
        FUNCTION_ENTRY("setDisplay");

        TA_ASSERT(m_profileHelper != NULL, "Helper was NULL");

        // Store the name of the field as locationkey,screenNumber. We will update to nice strings when
        // the user requests them
        std::ostringstream updateName;
        updateName << location << " " << parameterName;

        // Now store the display keys as the new and old values. Once again we will update to nice strings when
        // the user requests them
        
        std::string oldDisplay;
        try
        {
            oldDisplay = m_profileHelper->getParameterActualValue(location,parameterName);
        }
        catch ( const DataException& ex)
        {
            if (ex.getFailType() == DataException::NO_VALUE )
            {
                // This means there was no display
                oldDisplay = "";
            }
        }

        updateChanges( updateName.str(), oldDisplay, parameterValue);

        m_profileHelper->setParameter(location,parameterName, parameterValue);

        FUNCTION_EXIT;
    }

    unsigned long ConfigProfile::getKey()
    {
        TA_ASSERT(m_profileHelper != NULL, "getKey() has been called after profile was deleted");

        return m_profileHelper->getKey();
    }


    std::string ConfigProfile::getName()
    {
        TA_ASSERT(m_profileHelper != NULL, "getName() has been called after profile was deleted");

        return m_profileHelper->getName();
    }

	 std::string ConfigProfile::getDisplayName()
	 {
        TA_ASSERT(m_profileHelper != NULL, "getDisplayName() has been called after profile was deleted");

        return m_profileHelper->getDisplayName();
	 }


	unsigned long ConfigProfile::getType()
	{
        TA_ASSERT(m_profileHelper != NULL, "getType() has been called after profile was deleted");

        return m_profileHelper->getType();
	}

	std::string ConfigProfile::getTypeAsString()
	{
        TA_ASSERT(false, "Not used in the ConfigProfile" );

        return( "" );
	}

	unsigned long ConfigProfile::getAccessControlGroup()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getAccessControlGroup() );
	}

    std::string ConfigProfile::getAccessControlGroupAsString()
    {
        TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getAccessControlGroupAsString() );
    }

    std::vector<unsigned long> ConfigProfile::getAccessibleSubsystemKeys()
    {
        TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getAccessibleSubsystemKeys() );
    }

    bool ConfigProfile::isLocationAssociated(unsigned long locationId)
    {
        TA_ASSERT(m_profileHelper != NULL, "The ProfileHelper is NULL");

        return m_profileHelper->isLocationAssociated(locationId);
    }

	std::string ConfigProfile::getItsiGroupName()
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return( m_profileHelper->getItsiGroupName() );
    }

    bool ConfigProfile::isSystemProfile()
    {
        TA_ASSERT(m_profileHelper != NULL, "getName() has been called after profile was deleted");

        return m_profileHelper->isSystemProfile();
    }


    std::string ConfigProfile::getDefaultDisplay(unsigned long locationKey, int displayNumber)
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getDefaultDisplay( locationKey, displayNumber );
    }

    
    std::string ConfigProfile::getParameterValue(unsigned long locationKey, const std::string& parameterName)
    {
        TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getParameterValue( locationKey, parameterName );
    }

    
    std::string ConfigProfile::getParameterActualValue(unsigned long locationKey, const std::string& parameterName)
    {
        TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getParameterActualValue( locationKey, parameterName );
    }


    unsigned long ConfigProfile::getDisplayKey(unsigned long locationKey, int displayNumber)
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getDisplayKey( locationKey, displayNumber );
    }

    
    bool ConfigProfile::requiresLocationToBeSelected()
    {
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->isLocalProfile();
    }


    std::vector<ILocation*> ConfigProfile::getAssociatedLocations() 
    { 
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getAssociatedLocations();
    }


    std::vector<unsigned long> ConfigProfile::getAssociatedLocationKeys() 
    { 
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getAssociatedLocationKeys();
    }


	std::string ConfigProfile::getActionGroupAsStringForResource( unsigned long resourceKey )
	{
		TA_ASSERT( m_profileHelper != NULL, "The ProfileHelper was NULL" );

		return m_profileHelper->getActionGroupAsStringForResource( resourceKey );
	}


	std::string ConfigProfile::getActionGroupAsStringForSubsystem( unsigned long subsystemKey,
		   unsigned long subsystemStateKey	)
	{
		TA_ASSERT( false, "Not used in the ConfigProfile" );

		return( "" );
	}

	bool ConfigProfile::isExclusive()
	{
		TA_ASSERT( false, "Not used in the ConfigProfile" );
        return false;
	}


	bool ConfigProfile::getActionGroup( unsigned long subsystem, unsigned long subsystemStateKey,
			unsigned long& actionGroup, bool& isControl )
	{
		TA_ASSERT( false, "Not used in the ConfigProfile" );

		return( false );
	}

    
    time_t ConfigProfile::getDateCreated()
    {
        TA_ASSERT(m_profileHelper != NULL,"The OperatorHelper pointer is null.");
		return m_profileHelper->getDateCreated();
    }


    time_t ConfigProfile::getDateModified()
    {
        TA_ASSERT(m_profileHelper != NULL,"The OperatorHelper pointer is null.");
		return m_profileHelper->getDateModified();
    }
 
	
    void ConfigProfile::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        TA_ASSERT(m_profileHelper != NULL, "invalidate() has been called after profile was deleted");

        m_profileHelper->invalidate();
        m_profileChanges.clear();

        FUNCTION_EXIT;
    }


    void ConfigProfile::deleteThisObject()
    {
        FUNCTION_ENTRY("deleteThisObject");

        TA_ASSERT(m_profileHelper != NULL, "deleteThisObject() has been called after profile was deleted");

        // Delete the profile from the database
        m_profileHelper->deleteProfile();
        
        delete m_profileHelper;
        m_profileHelper = NULL;

        FUNCTION_EXIT;
    }


    void ConfigProfile::applyChanges()
    {
        FUNCTION_ENTRY("applyChanges");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        m_profileHelper->writeData();

        m_profileChanges.clear();

        LOG( SourceInfo, DebugUtil::FunctionExit, "ConfigProfile::applyChanges");
    }


    void ConfigProfile::updateChanges(const std::string& name, const std::string& oldValue, const std::string& newValue)
    {
        FUNCTION_ENTRY("updateChanges");

        // Search for this name in the map
        ItemChanges::iterator matching = m_profileChanges.find(name);

        if (matching != m_profileChanges.end())
        {
            // We already have this attribute name in the list.
            if (newValue == matching->second.oldValue)
            {
                // The new value matches the old value so we might as well remove this from the list as it
                // is not a valid change
                m_profileChanges.erase(matching);
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
            m_profileChanges.insert( ItemChanges::value_type( name, values ) );
        }

        FUNCTION_EXIT;
    }


    void ConfigProfile::setAccessControlGroup(unsigned long accessControlGroupKey)
    {
        FUNCTION_ENTRY("setAccessControlGroup");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        std::string oldName = m_profileHelper->getAccessControlGroupAsString();

        m_profileHelper->setAccessControlGroup(accessControlGroupKey);

        updateChanges(ACCESS_CONTROL_GROUP, oldName, m_profileHelper->getAccessControlGroupAsString());

        FUNCTION_EXIT;
    }


    void ConfigProfile::setRadioLoginGroup(const std::string& radioGroupName)
    {
        FUNCTION_ENTRY("setRadioLoginGroup");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        updateChanges(RADIO_LOGIN_GROUP, m_profileHelper->getItsiGroupName(), radioGroupName);

        m_profileHelper->setItsiGroup(radioGroupName);

        FUNCTION_EXIT;
    }


    void ConfigProfile::setPaPriorityGroup(const std::string& paGroupName)
    {
        //FUNCTION_ENTRY("setPaPriorityGroup");

        //TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        //updateChanges(PA_PRIORITY_GROUP, m_profileHelper->getItsiGroupName(), paGroupName);

        //m_profileHelper->setPaPriorityGroup(paGroupName);

        //FUNCTION_EXIT;
    }

    void ConfigProfile::setLocationAssociationStatus(const unsigned long locationId, const bool isAssociated)
    {
        FUNCTION_ENTRY("setLocationAssociationStatus");

        TA_ASSERT(m_profileHelper != NULL, "applyChanges() has been called after profile was deleted");

        // Need to determine WHAT (if anything) has changed, so can update the map of changes correctly.
        std::vector<unsigned long> associatedLocations = m_profileHelper->getAssociatedLocationKeys();

        // If the passed in location can be found in the vector of associated locations, then it is currently 
        // assocaited to this profile
        std::vector<unsigned long>::iterator searchResult = std::find(associatedLocations.begin(),
            associatedLocations.end(),locationId);

        if (searchResult != associatedLocations.end())
        {
            // The location is currently associated, so if the "change" is to make the location associated,
            // then don't have to do anything!
            if (isAssociated)
            {
                FUNCTION_EXIT;
                return;
            }
            else
            {
                // But if not, then need to note that we've updated it.
                updateChanges(PROFILE_LOCATION_ASSOCIATION,"true","false");

                // And remove the association
                m_profileHelper->removeLocationAssociaiton(locationId);
            }
        }
        else
        {
            // If the location is NOT currently associated, then do the opposite...
            if (isAssociated)
            {
                updateChanges(PROFILE_LOCATION_ASSOCIATION,"false","true");

                // Add the association
                m_profileHelper->addLocationAssocation(locationId);
            }
            else
            {
                FUNCTION_EXIT;
                return;
            }
        }

        FUNCTION_EXIT;
    }

} // closes TA_Base_Core

