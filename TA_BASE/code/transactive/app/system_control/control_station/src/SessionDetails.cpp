/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/SessionDetails.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class holds all session details about the current login as well as the lists of
  * operators, profiles etc.
  */

#pragma warning(disable:4786)

#include "app\system_control\control_station\src\ControlStationRootDialog.h"
#include "app\system_control\control_station\src\SessionDetails.h"

#include <algorithm>   

#include "core\configuration_updates\src\OnlineUpdateListener.h"
#include "core\data_access_interface\entity_access\src\IConsole.h"
#include "core\data_access_interface\src\IOperator.h"
#include "core\data_access_interface\src\ILocation.h"
#include "core\data_access_interface\src\IProfile.h"
#include "core\data_access_interface\src\OperatorAccessFactory.h"
#include "core\data_access_interface\src\ProfileAccessFactory.h"
#include "core\data_access_interface\src\LocationAccessFactory.h"
#include "core\data_access_interface\entity_access\src\ConsoleAccessFactory.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"
#include "core\utilities\src\TAAssert.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\EntityTypeException.h"
#include "core\exceptions\src\SessionException.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_agent/idl/src/IAuthenticationAgentCorbaDef.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::DatabaseException;
using TA_Base_Core::DataException;
using TA_Base_Core::EntityTypeException;
using TA_Base_Core::SessionException;
using TA_Base_Core::RunParams;
using TA_Base_Core::IProfile;
using TA_Base_Core::ILocation;
using TA_Base_Core::ProfileAccessFactory;
using TA_Base_Core::LocationAccessFactory;
using TA_Base_Core::OperatorAccessFactory;
using TA_Base_Core::OnlineUpdateListener;

SessionDetails::SessionDetails()
    : m_console(NULL),
      m_operator(NULL),
      m_profile(NULL),
      m_location(NULL),
      m_consoleLocation(NULL),
	  m_operatorOverride(NULL),
	  m_profileOverride(NULL),
      m_hasOperatorUpdated(false),
      m_hasLocationUpdated(false),
      m_hasProfileUpdated(false),
      m_isSessionValid(false),
      m_isOverrideValid(false),
      m_shouldChangeDisplay(false)
{
    IF_LOCAL_TEST
    {
        m_isSessionValid = true;
    }
}


SessionDetails::~SessionDetails()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"Destructor");
    try
    {
        if (m_console != NULL)
        {
            delete m_console;
            m_console = NULL;
        }

        //
        // Delete all the operators
        //
        OperatorVector::iterator operatorIter = m_allOperators.begin();
        while (operatorIter != m_allOperators.end())
        {
            if (*operatorIter != NULL)
            {
                delete *operatorIter;
                *operatorIter = NULL;
            }
            ++operatorIter;
        }
        m_allOperators.erase( std::remove(m_allOperators.begin(), m_allOperators.end(), static_cast<TA_Base_Core::IOperator*>(NULL)), m_allOperators.end() );

        //
        // Delete all the profiles
        //
        for (ProfileMap::iterator prof = m_profileMap.begin(); prof != m_profileMap.end(); prof++)
        {
            delete prof->second;
            prof->second = NULL;
        }

        //
        // Delete all the locationss
        //
        for (LocationMap::iterator loc = m_locationMap.begin(); loc != m_locationMap.end(); loc++)
        {
            delete loc->second;
            loc->second = NULL;
        }
    }
    catch ( ... )
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of SessionDetails");
    }

    LOG( SourceInfo, DebugUtil::FunctionExit,"Destructor");
}


void SessionDetails::initialise(const std::string& hostName)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"initialise");

    // 
    // Retrieve the console name and check it is in the database
    // 
    try
    {
        m_console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromAddress( hostName );
        unsigned long locationKey = m_console->getLocation();
        m_consoleLocation = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(locationKey);
        // TD1362 - Need to set the location key so that applications get the right location key
        RunParams::getInstance().set( RPARAM_LOCATIONKEY, convertULtoString(locationKey).c_str());
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to look up console in the database");
        TA_THROW( SessionException("Console does not exist or has errors in the database") );
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to look up console in the database");
        TA_THROW( SessionException("Console does not exist or has errors in the database") );
    }
    catch ( EntityTypeException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "EntityTypeException", "Error occurred while attempting to look up console in the database");
        TA_THROW( SessionException("Console does not exist or has errors in the database") );
    }

    //
    // Load all the operators and locations.
    //
    try
    {
        // listen to all location and operator changes
        OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::LOCATION, *this);
        OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::PROFILE, *this);
        OnlineUpdateListener::getInstance().registerAllInterests(TA_Base_Core::OPERATOR, *this);
        // only get the non-system operators PW2480
        m_allOperators = OperatorAccessFactory::getInstance().getAllNonSystemOperators(false, false); 

        ProfileVector profiles = ProfileAccessFactory::getInstance().getAllNonSystemProfiles(false);
		unsigned int i = 0;
        for (; i < profiles.size(); i++)
        {
            m_profileMap.insert(ProfileMap::value_type(profiles[i]->getKey(), profiles[i]));
        }

        LocationVector locations = LocationAccessFactory::getInstance().getAllLocations(false);
        for (i = 0; i < locations.size(); i++)
        {
            m_locationMap.insert(LocationMap::value_type(locations[i]->getKey(), locations[i]));
        }

    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not load the list of available operators or locations.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not load the list of available operators or locations.");
    }
    // We cannot load all the profiles as they are based on the selected operator.

    LOG( SourceInfo, DebugUtil::FunctionExit,"initialise");
}


void SessionDetails::sessionLogin( std::string sessionId,
								   TA_Base_Core::IOperator* theOperator,
                                   TA_Base_Core::IProfile*  theProfile,
                                   TA_Base_Core::ILocation*  theLocation)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"sessionLogin");

	// We must have a session ID that is not ""
    TA_ASSERT(sessionId.compare("") != 0, "The sessionId is empty");

    // We must have operator, profile and sessionId
    TA_ASSERT( (theOperator != NULL)&&(theProfile != NULL) , "The operator or profile is NULL");

	m_sessionId = sessionId;
    // Set up the RunParams so that everyone knows about the current session ID
    RunParams::getInstance().set(RPARAM_SESSIONID, m_sessionId.c_str());

    m_operator = theOperator;

    // Set up profile. 
    m_profile = theProfile; // reference the profile, just make sure we don't delete it when we repopulate

    // TD1362, TD2405 - reset the location key to the one specified
    // If not location specific (i.e. NULL), use the console location, otherwise use the one specified 
    m_location = (theLocation == NULL) ? m_consoleLocation : theLocation;
    RunParams::getInstance().set( RPARAM_LOCATIONKEY, convertULtoString(m_location->getKey()).c_str());

    m_isSessionValid = true;

    m_shouldChangeDisplay = true;
}


void SessionDetails::renewSession(std::string sessionId)
{
    FUNCTION_ENTRY("renewSession");
    TA_ASSERT(sessionId.compare("") != 0, "The sessionId is empty");

    m_sessionId = sessionId;
    // Set up the RunParams so that everyone knows about the current session ID
    RunParams::getInstance().set(RPARAM_SESSIONID, sessionId.c_str());
    FUNCTION_EXIT;
}


void SessionDetails::profileChanged(std::string sessionId, 
									TA_Base_Core::IProfile* theProfile,
                                    TA_Base_Core::ILocation* theLocation)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"profileChanged");

	// We must have a session ID that is not ""
    TA_ASSERT(sessionId.compare("") != 0, "The sessionId is empty");
    TA_ASSERT(theProfile != NULL, "The profile passed was equal to NULL");

	m_sessionId = sessionId;
    // Set up the RunParams so that everyone knows about the new session ID
    RunParams::getInstance().set(RPARAM_SESSIONID, m_sessionId.c_str());

    m_profile = theProfile;

    // TD1362, TD2405 - reset the location key to the one specified
    m_location = (theLocation == NULL) ? m_consoleLocation : theLocation;
    RunParams::getInstance().set( RPARAM_LOCATIONKEY, convertULtoString(m_location->getKey()).c_str());

    // since we've changed the profile, we need to remove operator override
    m_profileOverride = NULL;
    m_operatorOverride = NULL;
    m_isOverrideValid = false;

    m_shouldChangeDisplay = true;
}


void SessionDetails::operatorOverride( TA_Base_Core::IOperator* theOperator,
                                       TA_Base_Core::IProfile*  theProfile,
                                       TA_Base_Core::ILocation*  theLocation)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"operatorOverride");

    // We must have operator and profile
    TA_ASSERT( (theOperator != NULL) && (theProfile != NULL), "The operator or profile is NULL");

    // Set the RunParams so that the applications will recheck their security
    RunParams::getInstance().set(RPARAM_SESSIONID, m_sessionId.c_str());

    m_operatorOverride = theOperator;

    m_profileOverride = theProfile;

    m_isOverrideValid = true;

    m_shouldChangeDisplay = false;
}


void SessionDetails::sessionLogout()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"sessionLogout");

    // set the session id to "" so that we know that someone is not logged in
	m_sessionId = "";
    RunParams::getInstance().set(RPARAM_SESSIONID, "");

    removeOperatorOverride();

    // reset all login session details
    m_operator = NULL;

    if (m_location != m_consoleLocation)
    {
        m_location = m_consoleLocation;
    }
    // TD1362 - use the console location 
    RunParams::getInstance().set( RPARAM_LOCATIONKEY, convertULtoString(m_location->getKey()).c_str());

    m_profile = NULL;

    m_isSessionValid = false;

    // don't bother updating the displays since we're going to hide the display anyway
    m_shouldChangeDisplay = false;
}


void SessionDetails::removeOperatorOverride()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"removeOperatorOverride");

    // reset all login session details
    m_operatorOverride = NULL;

    m_profileOverride = NULL;

    m_isOverrideValid = false;

    // if there is no longer a valid session to return to (the configuration 
    // settings that the login session used have been deleted)
    if (m_isSessionValid)
    {
        // Set the RunParams so that the applications will recheck their security
        TA_Base_Core::RunParams::getInstance().set(RPARAM_SESSIONID, m_sessionId.c_str());
    }
    else
    {
        ControlStationRootDialog::getInstance().SendMessage(WM_FORCE_LOGOUT);
    }

    m_shouldChangeDisplay = false;
}


SessionDetails::ProfileVector SessionDetails::getListOfProfiles(TA_Base_Core::IOperator* theOperator, const bool onlyOverridable /*=false*/)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"getListOfProfiles");

    TA_ASSERT(theOperator != NULL, "The operator passed was equal to NULL");

    ProfileVector associatedProfiles;

    // Load the new profiles
    try
    {
        // TD2844 - The operator override is only allowed to operator as follows:
        //              - central profile cannot be overriden by local profile
        //              - local profile can be overriden by central profile
        //              - local profile can override another profile on the same location
        //              - a central profile can override another central profile
        //
        // So, see if the current profile is central, 
        //     if it is, only return central profiles
        // Otherwise, return all other profiles
        bool onlyGetCentralProfiles = onlyOverridable && (m_profile != NULL) && (!m_profile->requiresLocationToBeSelected());
        std::vector<unsigned long> profiles = theOperator->getAssociatedProfileKeys();
        for (unsigned int i = 0 ; i < profiles.size(); i++)
        {
            ProfileMap::iterator prfIter = m_profileMap.find(profiles[i]);
            if (prfIter == m_profileMap.end())
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,
                    "Unable to find the profile, %d, that is associated with operator, %d.  Could be system profile. Ignoring", profiles[i], theOperator->getKey());
            }
            else
            {
                if (!(onlyGetCentralProfiles && prfIter->second->requiresLocationToBeSelected()))
                {
                    associatedProfiles.push_back(prfIter->second);
                }
            }
        }
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve a list of profiles from the database");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve a list of profiles from the database");
    }

    // returning an array which points to our internal copies
    return associatedProfiles;
}


SessionDetails::LocationVector SessionDetails::getListOfLocations(TA_Base_Core::IProfile* theProfile)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry,"getListOfLocations");

    TA_ASSERT(theProfile != NULL, "The profile passed was equal to NULL");

    LocationVector associatedLocations;

    // Load the new locations
    try
    {
        std::vector<unsigned long> locations = theProfile->getAssociatedLocationKeys();
        for (unsigned int i = 0 ; i < locations.size(); i++)
        {
            LocationMap::iterator locIter = m_locationMap.find(locations[i]);
            if (locIter == m_locationMap.end())
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,
                    "Unable to find the location, %d, that is associated with profile, %d.  Ignoring", locations[i], theProfile->getKey());
            }
            else
            {
                associatedLocations.push_back(locIter->second);
            }
        }
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve a list of locations from the database");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve a list of locations from the database");
    }

    // returning an array which points to our internal copies
    return associatedLocations;
}


std::string SessionDetails::getOperatorName()
{
    if (m_operator == NULL)
    {
        return "";
    }

    std::string name = "Unknown";
    try
    {
        name = m_operator->getName();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the operator name from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the operator name from the database.");
    }
    
    return name;
}


std::string SessionDetails::getOperatorDescription()
{
    if (m_operator == NULL)
    {
        return "";
    }

    std::string description = "Unknown";
    try
    {
        description = m_operator->getDescription();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the operator description from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the operator description from the database.");
    }
    
    return description;
}


std::string SessionDetails::getOverrideOperatorName()
{
    if (m_operatorOverride == NULL)
    {
        return "";
    }

    std::string name = "Unknown";
    try
    {
        name = m_operatorOverride->getName();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the operator override name from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the operator override name from the database.");
    }
    
    return name;
}


std::string SessionDetails::getOverrideOperatorDescription()
{
    if (m_operatorOverride == NULL)
    {
        return "";
    }

    std::string description = "Unknown";
    try
    {
        description = m_operatorOverride->getDescription();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the operator override description from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the operator override description from the database.");
    }
    
    return description;
}


std::string SessionDetails::getProfileName()
{
    if (m_profile == NULL)
    {
        return "";
    }

    std::string name = "Unknown";
    try
    {
        name = m_profile->getName();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the profile name from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the profile name from the database.");
    }
    
    return name;
}


std::string SessionDetails::getOverrideProfileName()
{
    if (m_profileOverride == NULL)
    {
        return "";
    }

    std::string name = "Unknown";
    try
    {
        name = m_profileOverride->getName();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the profile override name from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the profile override name from the database.");
    }
    
    return name;
}


std::string SessionDetails::getProfileAndLocationDisplayString()
{
    if (m_profile == NULL)
    {
        return "";
    }

    std::string displayString = "Unknown";
    try
    {
        displayString = m_profile->getName();
        if (m_profile->requiresLocationToBeSelected() && (m_location != NULL))
        {
            displayString += TA_Base_App::TA_ControlStation::LOCATION_PROFILE_SEPARATOR;
            displayString += m_location->getName();
        }
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the profile and location names from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the profile and location names from the database.");
    }

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Operator profile and location display string is %s", displayString.c_str());
    return displayString;
}


std::string SessionDetails::getOverrideProfileAndLocationDisplayString()
{
    if (m_profileOverride == NULL)
    {
        return "";
    }

    std::string displayString = "Unknown";
    try
    {
        displayString = m_profileOverride->getName();
        if (m_profileOverride->requiresLocationToBeSelected() && (m_location != NULL))
        {
            displayString += TA_Base_App::TA_ControlStation::LOCATION_PROFILE_SEPARATOR;
            displayString += m_location->getName();
        }
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the profile and location override names from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the profile and location override names from the database.");
    }

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Operator profile and location override display string is %s", displayString.c_str());

    return displayString;
}


std::string SessionDetails::getProfileDisplay(unsigned long screenNumber)
{
    // TD341 - should only change display when there is a change in primary operator or profile 
    // i.e. new login, profile or operator change.  Should not change display when the override
    // is applied or removed
    if (!m_shouldChangeDisplay || (m_profile == NULL))
    {
        return "";
    }

    try
    {
        std::string display(m_profile->getDefaultDisplay(m_location->getKey(), screenNumber));
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Profile display on screen %d is %s", screenNumber, display.c_str());
        return display;
    }
    catch( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Caught while trying to retrieve profile or location display details");
    }
    catch( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Caught while trying to retrieve profile or location display details");
    }

    return "Unknown";
}

std::string SessionDetails::getProfileDisplayAtLocation(unsigned long screenNumber, const std::string& locationName)
{
    if ( m_profile == NULL)
    {
        return "";
    }

    try
    {
		unsigned long locationKey = TA_Base_Core::LocationAccessFactory::getInstance().getLocationKeyFromName( locationName );
        std::string display(m_profile->getDefaultDisplay(locationKey, screenNumber));
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Profile display on screen %d at location %d is %s", screenNumber, locationKey, display.c_str());
        return display;
    }
    catch( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Caught while trying to retrieve profile or location display details");
    }
    catch( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Caught while trying to retrieve profile or location display details");
    }

    return "Unknown";
}



std::string SessionDetails::getLocationName()
{
    if (m_location == NULL)
    {
        return "";
    }

    try
    {
        return m_location->getName();
    }
    catch( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Caught while trying to retrieve location name");
    }
    catch( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Caught while trying to retrieve location name");
    }
    return "Unknown";
}

unsigned long SessionDetails::getLocationKey()
{
    FUNCTION_ENTRY("getLocationKey");
    FUNCTION_EXIT;
    return m_location->getKey();
}


std::string SessionDetails::getConsoleName()
{
    if (m_console == NULL)
    {
        return "";
    }

    try
    {
        return m_console->getName();
    }
    catch( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Caught while trying to retrieve console name");
    }
    catch( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Caught while trying to retrieve console name");
    }
    return "Unknown";
}


std::string SessionDetails::getConsoleLocationName()
{
    if (m_console == NULL)
    {
        return "";
    }

    std::string name = "Unknown";
    try
    {
        name = m_console->getLocationName();
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Error occurred while attempting to retrieve the location name from the database.");
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Error occurred while attempting to retrieve the location name from the database.");
    }

    return name;
}


std::string SessionDetails::getSessionId()
{
    return m_sessionId;
}


unsigned long SessionDetails::getConsoleId()
{
    TA_ASSERT(m_console != NULL, "The console is NULL");
    return m_console->getKey();
}

unsigned long SessionDetails::getServerLocation()
{
    TA_ASSERT(m_console != NULL, "The console is NULL");
    return m_console->getLocation();
}

unsigned long SessionDetails::getConsoleLocationId()
{
    TA_ASSERT(m_consoleLocation != NULL, "The console location is NULL");
    return m_consoleLocation->getKey();
}


void SessionDetails::processUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "processUpdate" );

    switch (updateEvent.getType())
    {
    case TA_Base_Core::LOCATION:
        processLocationUpdate(updateEvent);
        break;
    case TA_Base_Core::OPERATOR:
        processOperatorUpdate(updateEvent);
        break;
    case TA_Base_Core::PROFILE:
        processProfileUpdate(updateEvent);
        break;
    default:
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "We're not registering for updates of this object type. Ignore");
        break;
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "processUpdate" );
}


void SessionDetails::processOperatorUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    unsigned long key = updateEvent.getKey();

    try
    {
        switch(updateEvent.getModifications())
        {
        case TA_Base_Core::Create:
        case TA_Base_Core::Update:
            {
                bool found = false;
                // see if it is already there first
                for (OperatorVector::iterator iter = m_allOperators.begin(); iter != m_allOperators.end(); iter++)
                {
                    if ((*iter)->getKey() == key)
                    {
                        // update the operator object in our cache
                        (*iter)->invalidate();
                        found = true;
                        break;
                    }
                }

                if (!found)
                {
                    // if it's not there, just add it and just out of the case block
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Adding Operator %d", key);
                    m_allOperators.push_back(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(key, false));
                    break;
                }

				// TD 15092
                // what's this used for???
				// if(key == m_operator->getKey())
                if ((m_operator != NULL) && (key == m_operator->getKey()))
				{

				}

                const std::vector<std::string> changes = updateEvent.getChangedParams();
				bool hasNameUpdated = false;

                for (unsigned int i = 0; i < changes.size(); i++)
                {
                    // see if we got profile association changes
                    if (changes[i].compare("Profiles") == 0)
                    {
                        m_hasProfileUpdated = true;

						// we can break if we've found a name change and a profile change.
						// otherwise the search must go on..
						if(hasNameUpdated)
						{
	                        break;
						}
                    }
					else if (changes[i].compare("Name") == 0)
					{
                        hasNameUpdated = true;
						if(m_hasProfileUpdated)
						{
	                        break;
						}
					}
                }

				// TD #11810
				// has the name updated for the current operator?
				// if so, we need to refresh the transactive taskbar to display the new name
				// TD 15092
				// if(hasNameUpdated && (m_operator->getKey() == key))
                if(hasNameUpdated && (m_operator != NULL) && (m_operator->getKey() == key))
				{
					ControlStationRootDialog::getInstance().PostMessage(WM_LOGIN_DETAILS_CHANGED);
				}

                if (!m_hasProfileUpdated)
                {
                    // if the profile has not been updated, we don't need to continue with the rest
                    break;
                }

                // check to see if the login session or overriding session is using a profile that is no longer associated with the operator
                if ((m_operator != NULL) && (m_operator->getKey() == key))
                {
                    std::vector<unsigned long> profileKeys = m_operator->getAssociatedProfileKeys();
                    unsigned long currentProfile = m_profile->getKey();
                    // If we can't find the current profile in the list of profile associated with the operator,
                    // then the session is no longer valid
                    if (std::find(profileKeys.begin(), profileKeys.end(), currentProfile) == profileKeys.end())
                    {
                        ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_LOGOUT);
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The current profile of the login operator has been disassociated from that operator");
                        m_isSessionValid = false;
                    }
                }
                else if ((m_operatorOverride != NULL) && (m_operatorOverride->getKey() == key))
                {
                    std::vector<unsigned long> profileKeys = m_operatorOverride->getAssociatedProfileKeys();
                    unsigned long overrideProfile = m_profile->getKey();
                    // If we can't find the current profile in the list of profile associated with the operator,
                    // then the session is no longer valid
                    if (std::find(profileKeys.begin(), profileKeys.end(), overrideProfile) == profileKeys.end())
                    {
                        ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_END_OVERRIDE);
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The current profile of the overriding operator has been disassociated from that operator");
                        m_isOverrideValid = false;
                    }
                }
            }
            break;
        case TA_Base_Core::Delete:
            {
                // PW2743 - terminate the active session when the current operator is deleted
                // check if the operator is used by the current sessions first before deleting them
                if ((m_operatorOverride != NULL) && (key == m_operatorOverride->getKey()))
                {
                    ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_END_OVERRIDE);
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Configuration of the operator that has overriden the current operator %d has been deleted", key);
                    m_isOverrideValid = false;
                }
                else if ((m_operator != NULL) && (key == m_operator->getKey()))
                {
                    ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_LOGOUT);
                    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Current operator %d has been deleted", key);
                    m_isSessionValid = false;
                }

                for (OperatorVector::iterator iter = m_allOperators.begin(); iter != m_allOperators.end(); iter++)
                {
                    if ((*iter)->getKey() == key)
                    {
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Deleting Operator %d from the Operator list", key);
                        delete (*iter);
                        m_allOperators.erase(iter);
                        break;
                    }
                }
            }
            break;
        default:
            break;
        }
        m_hasOperatorUpdated = true;
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not access operator, %d", key);
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not access operator, %d", key);
    }
}


void SessionDetails::processLocationUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    unsigned long key = updateEvent.getKey();
    LocationMap::iterator loc = m_locationMap.find(key);

    try
    {
        switch(updateEvent.getModifications())
        {
        case TA_Base_Core::Update:
            if (m_consoleLocation->getKey() == key)
            {
                m_consoleLocation->invalidate();
            }
            // just continue on to the create switch to update the location map
        case TA_Base_Core::Create:
            if (loc == m_locationMap.end())
            {
                // we haven't got it yet, so just reload
                LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
                    "Got online update for Location.  Adding new location, %d, into map", key);
                ILocation* newLoc = LocationAccessFactory::getInstance().getLocationByKey(key);
                m_locationMap.insert(LocationMap::value_type(key, newLoc));
            }
            else
            {
                // already got it so just update
                LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
                    "Got online update for Location.  Updating location, %d", key);
                loc->second->invalidate();
            }
            break;
        case TA_Base_Core::Delete:
            // PW2743 - terminate the active session when the current location is deleted
            // check if the location is used by the current sessions first before deleting them
            if ((m_location != NULL) && (key == m_location->getKey()))
            {
                ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_LOGOUT);
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Current location %d has been deleted", key);
                m_isSessionValid = false;
            }
			//TD17832 - marc_bugfix
			//Check first if a valid location was found 
			if (loc != m_locationMap.end())
			{
		        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
						"Got online update for Location.  Removing location, %d", key);

			    delete loc->second;
				loc->second = NULL;
				m_locationMap.erase(key);
			}
            break;
        default:
            break;
        }
        m_hasLocationUpdated = true;
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not access location, %d", key);
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not access location, %d", key);
    }
}


void SessionDetails::processProfileUpdate(const TA_Base_Core::ConfigUpdateDetails& updateEvent)
{
    unsigned long key = updateEvent.getKey();
    ProfileMap::iterator prof = m_profileMap.find(key);
    bool isFound = ( prof != m_profileMap.end() );

    try
    {
        switch(updateEvent.getModifications())
        {
        case TA_Base_Core::Create:
        case TA_Base_Core::Update:
            {
				if (prof == m_profileMap.end())
				{
					// we haven't got it yet, so just reload
					LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
						"Got online update for Profile.  Adding new profile, %d, into map", key);
					IProfile* newProf = ProfileAccessFactory::getInstance().getProfile(key);
					m_profileMap.insert(ProfileMap::value_type(key, newProf));
					break;
				}

				// already got it so just update
				LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,
					"Got online update for Profile.  Updating profile, %d", key);
				prof->second->invalidate();

                const std::vector<std::string> changes = updateEvent.getChangedParams();
                for (unsigned int i = 0; i < changes.size(); i++)
                {
                    // see if we got location association changes
					if (changes[i].find("Location") != std::string::npos)
                    {
                        m_hasLocationUpdated = true;
                        break;
                    }
                }

                if (!m_hasLocationUpdated)
                {
                    // if the location has not been updated, we don't need to continue with the rest
                    break;
                }

                // check to see if the login session or overriding session is using a location that is no longer associated with the profile
                if ((m_profile != NULL) && (m_profile->getKey() == key) && (m_profile->requiresLocationToBeSelected())) // sant TD14528
                {
                    std::vector<unsigned long> locationKeys = m_profile->getAssociatedLocationKeys();
                    unsigned long currentLocation = m_location->getKey();
                    // If we can't find the current location in the list of location associated with the profile,
                    // then the session is no longer valid
                    if (std::find(locationKeys.begin(), locationKeys.end(), currentLocation) == locationKeys.end())
                    {
                        ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_LOGOUT);
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The current location of the login profile has been disassociated from that profile");
                        m_isSessionValid = false;
                    }
                }
                else if ((m_profileOverride != NULL) && (m_profileOverride->getKey() == key) && (m_profileOverride->requiresLocationToBeSelected())) //sant TD14528
                {
                    std::vector<unsigned long> locationKeys = m_profileOverride->getAssociatedLocationKeys();
                    unsigned long overrideLocation = m_location->getKey();
                    // If we can't find the current location in the list of location associated with the profile,
                    // then the session is no longer valid
                    if (std::find(locationKeys.begin(), locationKeys.end(), overrideLocation) == locationKeys.end())
                    {
                        ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_END_OVERRIDE);
                        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "The current location of the overriding profile has been disassociated from that profile");
                        m_isOverrideValid = false;
                    }
                }
            }
            break;
        case TA_Base_Core::Delete:
            // PW2743 - terminate the active session when the current profile is deleted
            // check if the profile is used by the current sessions first before deleting them
            if ((m_profileOverride != NULL) && (key == m_profileOverride->getKey()))
            {
                ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_END_OVERRIDE);
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Configuration of the profile that has overriden the current profile %d has been deleted", key);
                m_isOverrideValid = false;
            }
            else if ((m_profile != NULL) && (key == m_profile->getKey()))
            {
                ControlStationRootDialog::getInstance().PostMessage(WM_FORCE_LOGOUT);
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Current profile %d has been deleted", key);
                m_isSessionValid = false;
            }
			// TD 12050
            if (prof != m_profileMap.end())
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Got online update for Profile.  Removing profile, %d", key);
			    delete prof->second;
			    prof->second = NULL;
			    m_profileMap.erase(key);
            }
            else
            {
                LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,"Got online update (type=delete) for unknown profile: %d", key);
            }
            break;
        default:
            break;
        }

        // TD 12050
        if ( isFound )
        {
            m_hasProfileUpdated = true;
        }
    }
    catch ( DataException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not access profile, %d", key);
    }
    catch ( DatabaseException& )
    {
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not access profile, %d", key);
    }
}


std::string SessionDetails::convertULtoString(unsigned long num)
{
    char str[11] = {0}; // big enough to hold all the characters of the number
    sprintf(str, "%lu", num);
    return std::string(str);
}



SessionDetails::SessionInfo SessionDetails::getSessionInfo()
{
    TA_ASSERT(m_sessionId.compare("") != 0, "The sessionId is empty");

    SessionInfo info;

    LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Got a session i.e. someone is logged in");

    TA_ASSERT(m_operator != NULL, "Operator shouldn't be NULL");
    TA_ASSERT(m_profile != NULL, "Profile shouldn't be NULL");
    TA_ASSERT(m_console != NULL, "Console shouldn't be NULL");

    info.operatorId = m_operator->getKey();
    info.profileId = m_profile->getKey();
    info.consoleId = m_console->getKey();
    if (m_profile->requiresLocationToBeSelected())
    {
        TA_ASSERT(m_location != NULL, "Location shouldn't be NULL");
        info.locationId = m_location->getKey();
    }
    info.password = m_operator->getPassword();

    if (m_operatorOverride == NULL)
    {
        LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"No operator override, so just returning the login info");
        return info;
    }

    info.isOverriden = true;
    info.overrideOperatorId = m_operatorOverride->getKey();
    info.overridePassword = m_operatorOverride->getPassword();
    info.overrideProfileId = m_profileOverride->getKey();

    LOG_GENERIC(SourceInfo,DebugUtil::DebugInfo,"Got operator override so returning both login and override info");
    return info;
}
TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef SessionDetails::toCorbaSessionInfo()
{
	TA_ASSERT(m_sessionId.compare("") != 0, "The sessionId is empty");
	TA_ASSERT(m_operator != NULL, "Operator shouldn't be NULL");
	TA_ASSERT(m_profile != NULL, "Profile shouldn't be NULL");

	TA_Base_Bus::SessionInfo result; 
	result.SessionId = m_sessionId;
	result.LocationId = m_location->getKey();
	result.UserId = m_operator->getKey();
	result.WorkstationId = m_console->getKey();
	result.OperatorId.push_back(m_operator->getKey());
	result.ProfileId.push_back(m_profile->getKey());

	if(m_operatorOverride != NULL)
	{
		result.OperatorId.push_back(m_operatorOverride->getKey());
		result.ProfileId.push_back(m_profileOverride->getKey());
	}

	TA_Base_Bus::IAuthenticationAgentCorbaDef::SessionInfoCorbaDef  corbaSessionInfo = TA_Base_Bus::AuthenticationLibrary::convertCSessionToCORBASession(result);
	return corbaSessionInfo;
}
