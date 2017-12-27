/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/plans/src/EventPlanAssocData.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * EventPlanAssocData is an implementation of IEventPlanAssocData. It holds the data specific to 
  * event plan associations.
  */

#ifdef WIN32
#pragma warning(disable:4786)
#endif

#include "core/data_access_interface/plans/src/EventPlanAssocData.h"

namespace TA_Base_Core
{
    EventPlanAssocData::EventPlanAssocData( unsigned long key ) : m_helper(key)
    {
    }


    EventPlanAssocData::EventPlanAssocData( unsigned long key, bool enabled, unsigned long eventTypeKey,
        unsigned long entityKey, unsigned long entityTypeKey,
        const std::string& planPath, time_t dateCreated, time_t dateModified ) :
        m_helper( key, enabled, eventTypeKey, entityKey, entityTypeKey, planPath, dateCreated, dateModified )
    {
    }


    EventPlanAssocData::~EventPlanAssocData()
    {
    }


    void EventPlanAssocData::invalidate()
    {
        m_helper.invalidate();
    }


    unsigned long EventPlanAssocData::getEventTypeKey()
    {
        return m_helper.getEventTypeKey();
    }


    bool EventPlanAssocData::isEnabled()
    {
        return m_helper.isEnabled();
    }


    unsigned long EventPlanAssocData::getEntityKey()
    {
        return m_helper.getEntityKey();
    }


    unsigned long EventPlanAssocData::getEntityTypeKey()
    {
        return m_helper.getEntityTypeKey();
    }


	std::string EventPlanAssocData::getPlanPath()
	{
        return m_helper.getPlanPath();
	}


    time_t EventPlanAssocData::getDateCreated()
    {
        return m_helper.getDateCreated();
    }


    time_t EventPlanAssocData::getDateModified()
    {
        return m_helper.getDateModified();
    }

} // Closes TA_Base_Core
