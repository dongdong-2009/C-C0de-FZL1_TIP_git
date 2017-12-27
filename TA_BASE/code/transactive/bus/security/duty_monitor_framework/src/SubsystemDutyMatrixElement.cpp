/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/security/duty_monitor_framework/src/SubsystemDutyMatrixElement.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This is a representation of a single element of the subsystem duty matrix.
  * It is associated with a single subsystem in a single region.
  * It contains a boolean value indicating whether the subsystem belongs to
  * the region. It also contains a list of duties for the subsystem. 
  * If the subsystem is not part of the region, the duty list is always empty.
  *
  */

#include "bus/security/duty_monitor_framework/src/DutyMonitorFramework.h"

using namespace TA_Base_Bus;


SubsystemDutyMatrixElement::SubsystemDutyMatrixElement() :
    m_isSubsystemInRegion(false), m_subsystemState(E_SS_NORMAL)
{
//    FUNCTION_ENTRY("SubsystemDutyMatrixElement()");
//    FUNCTION_EXIT;
}


SubsystemDutyMatrixElement::SubsystemDutyMatrixElement(const SubsystemDutyMatrixElement& subsystemDutyMatrixElement) :
    m_isSubsystemInRegion(subsystemDutyMatrixElement.m_isSubsystemInRegion),
    m_subsystemState(subsystemDutyMatrixElement.m_subsystemState),
    m_subsystemDuties(subsystemDutyMatrixElement.m_subsystemDuties)
{
//    FUNCTION_ENTRY("SubsystemDutyMatrixElement(const SubsystemDutyMatrixElement& SubsystemDutyMatrixElement)");
//    FUNCTION_EXIT;
}


SubsystemDutyMatrixElement::~SubsystemDutyMatrixElement()
{
//    FUNCTION_ENTRY("~SubsystemDutyMatrixElement()");

    m_subsystemDuties.clear();

//    FUNCTION_EXIT;
}


SubsystemDutyMatrixElement& SubsystemDutyMatrixElement::operator=(const SubsystemDutyMatrixElement& subsystemDutyMatrixElement)
{
//    FUNCTION_ENTRY("operator=(const SubsystemDutyMatrixElement& SubsystemDutyMatrixElement)");

    if (this != &subsystemDutyMatrixElement)
    {
        m_isSubsystemInRegion = subsystemDutyMatrixElement.m_isSubsystemInRegion;
        m_subsystemState = subsystemDutyMatrixElement.m_subsystemState;
        m_subsystemDuties = subsystemDutyMatrixElement.m_subsystemDuties;
    }

//    FUNCTION_EXIT;
    return *this;
}


bool SubsystemDutyMatrixElement::isSubsystemInRegion() const
{
//    FUNCTION_ENTRY("isSubsystemInRegion()");
//    FUNCTION_EXIT;
    return m_isSubsystemInRegion;
}


bool SubsystemDutyMatrixElement::dutiesExist() const
{
//    FUNCTION_ENTRY("dutiesExist()");
//    FUNCTION_EXIT;

    return !m_subsystemDuties.empty();
}


SubsystemDutyList SubsystemDutyMatrixElement::getSubsystemDuties() const
{
//    FUNCTION_ENTRY("getSubsystemDuties()");

    SubsystemDutyList subsystemDuties;
    subsystemDuties.reserve(m_subsystemDuties.size());   

    for (SubsystemDutySet::const_iterator subDutyIt = m_subsystemDuties.begin(); subDutyIt != m_subsystemDuties.end(); subDutyIt++)
    {
        subsystemDuties.push_back(*subDutyIt);
    }

//    FUNCTION_EXIT;
    return subsystemDuties;
}


SubsystemDutyList SubsystemDutyMatrixElement::getSubsystemDuties(const SessionId& sessionId) const
{
//    FUNCTION_ENTRY("getSubsystemDuties(const SessionId& sessionId)");

    SubsystemDutyList subsystemDuties;

    for (SubsystemDutySet::const_iterator subDutyIt = m_subsystemDuties.begin(); subDutyIt != m_subsystemDuties.end(); subDutyIt++)
    {
		//azenitha++
		//Changed == to .compare
        //if (sessionId == subDutyIt->session.in())
		if (sessionId.compare(subDutyIt->session.in()) == 0)
		//++azenitha
        {
            subsystemDuties.push_back(*subDutyIt);
        }
    }

//    FUNCTION_EXIT;
    return subsystemDuties;
}


void SubsystemDutyMatrixElement::setSubsystemInRegion(bool isSubsystemInRegion)
{
//    FUNCTION_ENTRY("setIsSubsystemInRegion(bool isSubsystemInRegion)");

    if (!isSubsystemInRegion)
    {
        m_subsystemDuties.clear();
    }

    m_isSubsystemInRegion = isSubsystemInRegion;

//    FUNCTION_EXIT;
}


bool SubsystemDutyMatrixElement::addDuty(const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty)
{
    FUNCTION_ENTRY("addDuty(const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty)");


	bool result = false;
	SubsystemDutySet::iterator subDutyIt;
	
	for ( subDutyIt = m_subsystemDuties.begin(); subDutyIt != m_subsystemDuties.end(); subDutyIt++ )
    {
		int sessionCompareRes = ::strcmp(subDutyIt->session.in(), subsystemDuty.session.in());
		
        if ( subDutyIt->profileKey == subsystemDuty.profileKey && sessionCompareRes == 0 ) 
		{
			break;
        }
    }
	
	if (subDutyIt == m_subsystemDuties.end()) 
	{
		try
		{
			SessionInfoEx sessionInfoEx = DutyDataStore::getSessionCache().getSessionInfo( subsystemDuty.session.in() );
			if ( sessionInfoEx.ProfileId[0] == subsystemDuty.profileKey )
			{
				m_subsystemDuties.push_front(subsystemDuty);
			}
			else
			{
				m_subsystemDuties.push_back(subsystemDuty);
			}
			
			result = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo,
				"Added subsystem duty for [s: %lu, r: %lu] -> session: %s / profile: %lu / action grp: %lu", 
				subsystemDuty.subsystemKey, subsystemDuty.regionKey,
				subsystemDuty.session.in(), subsystemDuty.profileKey, subsystemDuty.actionGroupKey);

		}
		catch (const DutyMonitorFrameworkException & ex)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "DutyMonitorFrameworkException", ex.what());
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Did NOT add subsystem duty for [s: %lu, r: %lu] -> session: %s / profile: %lu / action grp: %lu", 
				subsystemDuty.subsystemKey, subsystemDuty.regionKey,
				subsystemDuty.session.in(), subsystemDuty.profileKey, subsystemDuty.actionGroupKey);
		}				
	}
	else
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Did NOT add subsystem duty for [s: %lu, r: %lu] -> session: %s / profile: %lu / action grp: %lu", 
			subsystemDuty.subsystemKey, subsystemDuty.regionKey,
			subsystemDuty.session.in(), subsystemDuty.profileKey, subsystemDuty.actionGroupKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This duty is already stored in the set"); 
	}
	
    FUNCTION_EXIT;
    return result;
}


bool SubsystemDutyMatrixElement::removeDuty(const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty)
{
    FUNCTION_ENTRY("removeDuty(const DutyAgentTypeCorbaDef::SubsystemDuty& subsystemDuty)");

	bool result = false;
	SubsystemDutySet::iterator subDutyIt;
	
	for ( subDutyIt = m_subsystemDuties.begin(); subDutyIt != m_subsystemDuties.end(); subDutyIt++ )
    {
		int sessionCompareRes = ::strcmp(subDutyIt->session.in(), subsystemDuty.session.in());

        if (subDutyIt->profileKey == subsystemDuty.profileKey && sessionCompareRes == 0 ) 
		{
			m_subsystemDuties.erase(subDutyIt);
			result = true;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
				"Removed subsystem duty for [s: %lu, r: %lu] -> session: %s / profile: %lu / action grp: %lu", 
				subsystemDuty.subsystemKey, subsystemDuty.regionKey,
				subsystemDuty.session.in(), subsystemDuty.profileKey, subsystemDuty.actionGroupKey);
			break;
        }
    }
	
	if (!result)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
			"Did NOT remove subsystem duty for [s: %lu, r: %lu] -> session: %s / profile: %lu / action grp: %lu", 
			subsystemDuty.subsystemKey, subsystemDuty.regionKey,
			subsystemDuty.session.in(), subsystemDuty.profileKey, subsystemDuty.actionGroupKey);
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "This duty was not found in the set"); 
	}	
	
    FUNCTION_EXIT;
    return result;
}

void SubsystemDutyMatrixElement::setSubsystemState(const ESubsystemState subsystemState)
{
    FUNCTION_ENTRY("setSubsystemState(const ESubsystemstate subsystemState");

    m_subsystemState = subsystemState;

    FUNCTION_EXIT;
}

ESubsystemState SubsystemDutyMatrixElement::getSubsystemState() const
{
    return m_subsystemState;
}

void SubsystemDutyMatrixElement::clearDuty()
{
	m_subsystemDuties.clear();
}
