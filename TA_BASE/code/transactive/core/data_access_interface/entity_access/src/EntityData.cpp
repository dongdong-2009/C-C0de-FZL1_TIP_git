/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/EntityData.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2015/01/19 17:43:23 $
 * Last modified by:  $Author: CM $
 * 
 * EntitData is an implementation of the IEntityData interface. It can be used by specific
 * to handle the actions of retrieving the standard (default) entity data from the 
 * EntityHelper. It should be sub-classed by the specific entity class.
 */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/Subsystem.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/DataException.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string EntityData::ALARM_DISPLAY_PARAMETER ("AlarmDisplay");

    EntityData::EntityData(const unsigned long key,const std::string& typeName) 
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "EntityData (constructor)");
        // Construct the entity helper based upon this key and type
        m_entityHelper = new EntityHelper(key,typeName);  
        LOG( SourceInfo, DebugUtil::FunctionExit, "EntityData (constructor)");
    }

    EntityData::~EntityData()
    {
        if (m_entityHelper != NULL)
        {
            delete m_entityHelper;
            m_entityHelper = NULL;
        }
    }
          

    unsigned long EntityData::getKey()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getKey();
    }

    unsigned long EntityData::getTypeKey()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getTypeKey();
    }

    std::string EntityData::getName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getName();
    }

    std::string EntityData::getAddress()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getAddress();
    }

    std::string EntityData::getDescription()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getDescription();
    }

    unsigned long EntityData::getSubsystem()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getSubsystem(true);
    }


    std::string EntityData::getSubsystemName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getSubsystemName(true);
    }


    unsigned long EntityData::getPhysicalSubsystem()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getSubsystem(false);
    }


    std::string EntityData::getPhysicalSubsystemName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getSubsystemName(false);
    }

    unsigned long EntityData::getLocation()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getLocation();
    }

	std::string EntityData::getLocationName()
	{
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getLocationName();
	}

	unsigned long EntityData::getRegion()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getRegion();
    }

	std::string EntityData::getRegionName()
	{
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getRegionName();
	}

    unsigned long EntityData::getParent()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getParent();
    }


    std::string EntityData::getParentName()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getParentName();
    }

	
    unsigned long EntityData::getAgent()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getAgent();
    }


	std::string EntityData::getAgentName()
	{
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getAgentName();
	}


    time_t EntityData::getDateCreated()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getDateCreated();
    }

    time_t EntityData::getDateModified()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getDateModified();
    }

    std::string EntityData::getAlarmDisplay()
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
		return m_entityHelper->getParameter(ALARM_DISPLAY_PARAMETER);
    }


	void EntityData::assignDefaultData( IData* data, unsigned long row )
	{
		m_entityHelper->assignDefaultData( data, row );
	}


    EntityHelper* EntityData::getHelper()
    {
        return m_entityHelper;
    }

    //TD15302 Mintao++
    EntityStatusData EntityData::getEntityStatusData(EEntityStatusType statustype)
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");      
        return m_entityHelper->getEntityStatusData(statustype);
    }

    EntityStatusData EntityData::getEntityStatusData(unsigned int statustype)
    {
		TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getEntityStatusData((EEntityStatusType)statustype);
    }

    //caller must catch the DataException and unknown exception when call this function
    bool EntityData::getBoolEntityStatusValue(EEntityStatusType statustype)
    {
        TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
		return m_entityHelper->getBoolEntityStatusValue(statustype);
    }

    bool EntityData::getBoolEntityStatusValue(unsigned int statustype)
    {
		TA_ASSERT(m_entityHelper != NULL, "The EntityHelper pointer is null");
        return m_entityHelper->getBoolEntityStatusValue((EEntityStatusType)statustype);
    }
	
	//zhou yuan++
	void EntityData::assignDefaultParameterValueData( IData* data, unsigned long row )
	{
		m_entityHelper->assignDefaultParameterValueData(data, row);
	}
	//++zhou yuan

    void EntityData::assignDefaultParameterValueData( const std::string& parameterName, const std::string& parameterValue )
    {
        m_entityHelper->assignDefaultParameterValueData(parameterName, parameterValue);
    }

} //close namespace TA_Base_Core


