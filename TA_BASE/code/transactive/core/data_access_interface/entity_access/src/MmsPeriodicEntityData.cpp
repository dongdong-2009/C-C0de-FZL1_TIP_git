/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MmsPeriodicEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of MmsPeriodicEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MmsPeriodicEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string MmsPeriodicEntityData::ENTITY_TYPE = "MmsPeriodicSubmitter";
	const std::string MmsPeriodicEntityData::SCAN_PERIOD_MIN = "ScanPeriodInMinutes";
	const std::string MmsPeriodicEntityData::BACKLOG_CHECK_PERIOD_HOURS = "BacklogCheckPeriodInHours";

    MmsPeriodicEntityData::MmsPeriodicEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }

    MmsPeriodicEntityData::~MmsPeriodicEntityData()
    { // Nothing to clean up
    }

    IEntityData* MmsPeriodicEntityData::clone(unsigned long key)
    {
        return new MmsPeriodicEntityData(key);        
    }

    void MmsPeriodicEntityData::invalidate()
    {
		getHelper()->invalidate();
    }

    unsigned int MmsPeriodicEntityData::getScanPeriodMinutes()
    {
		unsigned int scanPeriodMinutes = 10;
		std::string value = getHelper()->getParameter(SCAN_PERIOD_MIN);
		if (!value.empty())
		{
			scanPeriodMinutes = EntityHelper::getIntegerData(value);
		}

        return scanPeriodMinutes;
    }

    unsigned int MmsPeriodicEntityData::getBackLogCheckPeriodHours()
    {
		unsigned int backlogPeriodHours = 1;
		std::string value = getHelper()->getParameter(BACKLOG_CHECK_PERIOD_HOURS);
		if (!value.empty())
		{
			backlogPeriodHours = EntityHelper::getIntegerData(value);
		}

        return backlogPeriodHours;
    }

    std::string MmsPeriodicEntityData::getType()
    {
        return getStaticType();
    }
	
    std::string MmsPeriodicEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}
