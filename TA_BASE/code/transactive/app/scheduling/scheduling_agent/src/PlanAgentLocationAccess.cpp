/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_TIP_NEW/TA_BASE/transactive/app/scheduling/scheduling_agent/src/PlanAgentLocationAccess.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2013/03/19 17:25:16 $
  * Last modified by:  $Author: qi.huang $
  * 
  * Thread Safe Singleton to extract location data for Plan Agent entities.
  */

//#include "StdAfx.h"

#ifdef __WIN32__
	#pragma warning(disable: 4786)  // return type for 'identifier::operator ->' is not a UDT or reference to a UDT
#endif

#include <sstream>

#include "core/synchronisation/src/ThreadGuard.h"
#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/ProfileHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "bus/security/authentication_library/src/AuthenticationLibrary.h"
#include "bus/security/authentication_library/src/SessionInfo.h"
#include "app/scheduling/scheduling_agent/src/PlanAgentLocationAccess.h"

namespace TA_Base_App
{

// Initialise Statics
TA_Base_Core::NonReEntrantThreadLockable TA_Base_App::PlanAgentLocationAccess::m_singletonLock;
PlanAgentLocationAccess* TA_Base_App::PlanAgentLocationAccess::m_locationAccess = NULL;

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
        if ( NULL == *locIter )
        {
            continue;
        }

        // Check whether there are any Plan Agent entities configured for this location before adding it to the location map
        TA_Base_Core::IEntityDataList paEntities = 
            TA_Base_Core::EntityAccessFactory::getInstance().getEntitiesOfTypeAtLocation("PlanAgent", (*locIter)->getKey());

        if (paEntities.size() > 0)
        {
            m_locationMap.insert(std::make_pair((*locIter)->getKey(), (*locIter)->getName()));

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

}
