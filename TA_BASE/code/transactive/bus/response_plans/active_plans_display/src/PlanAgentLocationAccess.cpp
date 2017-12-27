/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/response_plans/active_plans_display/src/PlanAgentLocationAccess.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * Thread Safe Singleton to extract location data for Plan Agent entities.
  */

#include "StdAfx.h"

#ifdef __WIN32__
	#pragma warning(disable: 4786)  // return type for 'identifier::operator ->' is not a UDT or reference to a UDT
#endif

#include <sstream>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/ProfileHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "PlanAgentLocationAccess.h"


// Initialise Statics
TA_Base_Core::NonReEntrantThreadLockable TA_Base_Bus::PlanAgentLocationAccess::m_singletonLock;
TA_Base_Bus::PlanAgentLocationAccess* TA_Base_Bus::PlanAgentLocationAccess::m_locationAccess = NULL;


using namespace TA_Base_Bus;



////////////////////////////////////////////////////////////////////////////
// PlanAgentLocationAccess

PlanAgentLocationAccess::PlanAgentLocationAccess()
{
    initialise();
}


PlanAgentLocationAccess::~PlanAgentLocationAccess()
{
}


void PlanAgentLocationAccess::initialise()
{
    // Get the default Plan Agent location
    TA_Base_Bus::AuthenticationLibrary authLibrary;
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
    TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
    TA_ASSERT(sessionInfo.ProfileId.size() > 0, "Session does not have an associated profile");
    TA_Base_Core::ProfileHelper profileHelper(sessionInfo.ProfileId[0]);

    if (profileHelper.isLocalProfile())
    {
        // Local profiles use the login location
        m_defaultLocation = sessionInfo.LocationId;
    }
    else
    {
        // Central profiles use the physical location
        TA_ASSERT(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY), "LocationKey runtime param not set");
        m_defaultLocation = ::atol(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str());
    }

    // Set up the Plan Agent location map
    typedef std::vector<TA_Base_Core::ILocation *> LocationList;
    LocationList locations = TA_Base_Core::LocationAccessFactory::getInstance().getAllLocations();

    for (LocationList::const_iterator locIter = locations.begin(); locIter != locations.end(); locIter++)
    {
        TA_Base_Core::Location* location = static_cast<TA_Base_Core::Location *>(*locIter);

        // Check whether there are any Plan Agent entities configured for this location before adding it to the location map
        TA_Base_Core::IEntityDataList paEntities = 
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("PlanAgent", location->getKey());

        if (paEntities.size() > 0)
        {
            m_locationMap.insert(std::make_pair(location->getKey(), location->getDisplayName()));

            for (TA_Base_Core::IEntityDataList::const_iterator paIter = paEntities.begin(); paIter != paEntities.end(); paIter++)
            {
                delete *paIter;
            }

            paEntities.clear();
        }
         
        delete *locIter;
    }

    locations.clear();
}


PlanAgentLocationAccess& PlanAgentLocationAccess::getInstance()
{
	TA_Base_Core::ThreadGuard guard(m_singletonLock);

    if ( m_locationAccess == NULL )
    {
		m_locationAccess = new PlanAgentLocationAccess();
    }

	return *m_locationAccess;
}


unsigned long PlanAgentLocationAccess::getDefaultPlanAgentLocation()
{
    return m_defaultLocation;
}

     
std::vector<unsigned long> PlanAgentLocationAccess::getAllPlanAgentLocations()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    std::vector<unsigned long> locations;
    locations.reserve(m_locationMap.size());

    for (LocationMap::const_iterator iter = m_locationMap.begin(); iter != m_locationMap.end(); iter++)
    {
        locations.push_back(iter->first);
    }

    return locations;
}


std::vector<unsigned long> PlanAgentLocationAccess::getNonDefaultPlanAgentLocations()
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    std::vector<unsigned long> locations;
    locations.reserve(m_locationMap.size());

    for (LocationMap::const_iterator iter = m_locationMap.begin(); iter != m_locationMap.end(); iter++)
    {
        if (iter->first != getDefaultPlanAgentLocation())
        {
            locations.push_back(iter->first);
        }
    }

    return locations;
}

    
std::string PlanAgentLocationAccess::getDefaultPlanAgentLocationName()
{
    return getLocationName(getDefaultPlanAgentLocation());
}


std::string PlanAgentLocationAccess::getLocationName(unsigned long locationKey)
{
    TA_Base_Core::ThreadGuard guard(m_singletonLock);

    LocationMap::const_iterator iter = m_locationMap.find(locationKey);

    if (iter != m_locationMap.end())
    {
        return iter->second;
    }

    std::ostringstream locationKeyStr;
    locationKeyStr << "<invalid Plan Agent location ID " << locationKey << ">";

    return locationKeyStr.str();
}
