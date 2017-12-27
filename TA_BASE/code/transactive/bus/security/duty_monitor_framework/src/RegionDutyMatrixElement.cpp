/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/RegionDutyMatrixElement.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is a representation of a single element of the region duty matrix.
  * It is associated with a single session and a single region.
  * It contains a list of duties allocated to the session for the region. 
  * The list may be empty.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


RegionDutyMatrixElement::RegionDutyMatrixElement()
{
//    FUNCTION_ENTRY("RegionDutyMatrixElement()");
//    FUNCTION_EXIT;
}


RegionDutyMatrixElement::RegionDutyMatrixElement(const RegionDutyMatrixElement& regionDutyMatrixElement) :
    m_regionDuties(regionDutyMatrixElement.m_regionDuties)
{
//    FUNCTION_ENTRY("RegionDutyMatrixElement(const RegionDutyMatrixElement& regionDutyMatrixElement)");
//    FUNCTION_EXIT;
}


RegionDutyMatrixElement::~RegionDutyMatrixElement()
{
//    FUNCTION_ENTRY("~RegionDutyMatrixElement()");

    m_regionDuties.clear();

//    FUNCTION_EXIT;
}


RegionDutyMatrixElement& RegionDutyMatrixElement::operator=(const RegionDutyMatrixElement& regionDutyMatrixElement)
{
//    FUNCTION_ENTRY("operator=(const RegionDutyMatrixElement& regionDutyMatrixElement)");

    if (this != &regionDutyMatrixElement)
    {
        m_regionDuties = regionDutyMatrixElement.m_regionDuties;
    }

//    FUNCTION_EXIT;
    return *this;
}


bool RegionDutyMatrixElement::dutiesExist() const
{
//    FUNCTION_ENTRY("dutiesExist()");
//    FUNCTION_EXIT;

    return !m_regionDuties.empty();
}


RegionDutyList RegionDutyMatrixElement::getRegionDuties() const
{
//    FUNCTION_ENTRY("getRegionDuties()");

    RegionDutyList regionDuties;
    regionDuties.reserve(m_regionDuties.size());   

    for (RegionDutySet::const_iterator regionDutyIt = m_regionDuties.begin(); regionDutyIt != m_regionDuties.end(); regionDutyIt++)
    {
        regionDuties.push_back(*regionDutyIt);
    }

//    FUNCTION_EXIT;
    return regionDuties;
}


bool RegionDutyMatrixElement::addDuty(const DutyAgentTypeCorbaDef::RegionDuty& regionDuty)
{
    FUNCTION_ENTRY("addDuty(const DutyAgentTypeCorbaDef::RegionDuty& regionDuty)");

	bool result = false;
	RegionDutySet::iterator regionDutyIt;

	for ( regionDutyIt = m_regionDuties.begin(); regionDutyIt != m_regionDuties.end(); regionDutyIt++ )
    {
        if (regionDutyIt->profileKey == regionDuty.profileKey) 
		{
			break;
        }
    }

	if (regionDutyIt == m_regionDuties.end()) 
	{
		m_regionDuties.push_back(regionDuty);
		result = true;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                    "Added region duty for [s: %s, r: %lu] -> profile: %lu", 
                    regionDuty.session.in(), regionDuty.regionKey, regionDuty.profileKey);

	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Did NOT add region duty for [s: %s, r: %lu] -> profile: %lu",
					regionDuty.session.in(), regionDuty.regionKey, regionDuty.profileKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This duty is already stored in the set"); 
	}

    FUNCTION_EXIT;
    return result;
}


bool RegionDutyMatrixElement::removeDuty(const DutyAgentTypeCorbaDef::RegionDuty& regionDuty)
{
    FUNCTION_ENTRY("removeDuty(const DutyAgentTypeCorbaDef::RegionDuty& regionDuty)");

	bool result = false;
	RegionDutySet::iterator regionDutyIt;
	
	for ( regionDutyIt = m_regionDuties.begin(); regionDutyIt != m_regionDuties.end(); regionDutyIt++ )
    {
        if (regionDutyIt->profileKey == regionDuty.profileKey) 
		{
			m_regionDuties.erase(regionDutyIt);
			result = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
						"Removed region duty for [s: %s, r: %lu] -> profile: %lu", 
						regionDuty.session.in(), regionDuty.regionKey, regionDuty.profileKey);
			break;
        }
    }

	if (!result)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
					"Did NOT remove region duty for [s: %s, r: %lu] -> profile: %lu",
					regionDuty.session.in(), regionDuty.regionKey, regionDuty.profileKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This duty was not found in the set"); 
	}

	
    FUNCTION_EXIT;
    return result;
}


void RegionDutyMatrixElement::clearDuty()
{
	m_regionDuties.clear();
}
