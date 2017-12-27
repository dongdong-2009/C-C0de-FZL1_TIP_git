/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/InspectorPanelEntityData.cpp $
  * @author:  Wen Ching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  */

#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/InspectorPanelEntityData.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{

    const std::string InspectorPanelEntityData::ENTITY_TYPE     = "InspectorPanel";
    const std::string InspectorPanelEntityData::m_maxInstances  = "MaxInstances";
	const std::string InspectorPanelEntityData::m_showQuality   = "ShowQuality";
	const std::string InspectorPanelEntityData::m_showTimestamp = "ShowTimestamp";

	InspectorPanelEntityData::InspectorPanelEntityData()
	{

	}

	InspectorPanelEntityData::InspectorPanelEntityData( unsigned long key ) :
		EntityData( key, getStaticType() )
    {
    }

	InspectorPanelEntityData::~InspectorPanelEntityData()
	{

	}

	void InspectorPanelEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

    std::string InspectorPanelEntityData::getType()
    {
        return getStaticType();
    }

    std::string InspectorPanelEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    TA_Base_Core::IEntityData* InspectorPanelEntityData::clone(unsigned long key)
    {
        return( new InspectorPanelEntityData(key));
    }

	int InspectorPanelEntityData::getMaxInstances()
    {
		int maxInstances = 3;
		std::string value = getHelper()->getParameter(m_maxInstances);
		if (!value.empty())
		{
			maxInstances = EntityHelper::getIntegerData(value);
		}

        return maxInstances;
    }

    bool InspectorPanelEntityData::getShowQuality()
    {
		bool showQuality;
		std::string value = getHelper()->getParameter(m_showQuality);
		if (0 == EntityHelper::getIntegerData(value))
		{
			showQuality = false;
		}
		else
		{
			showQuality = true;
		}

		return showQuality;
    }

	bool InspectorPanelEntityData::getShowTimestamp()
    {
		bool showTimestamp;
		std::string value = getHelper()->getParameter(m_showTimestamp);
		if (0 == EntityHelper::getIntegerData(value))
		{
			showTimestamp = false;
		}
		else
		{
			showTimestamp = true;
		}
		
		return showTimestamp;
    }

}
