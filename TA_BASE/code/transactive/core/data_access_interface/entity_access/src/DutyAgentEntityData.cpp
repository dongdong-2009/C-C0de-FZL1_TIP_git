/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/DutyAgentEntityData.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/DutyAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string DutyAgentEntityData::ENTITY_TYPE                  = "DutyAgent";
	const std::string DutyAgentEntityData::UNCONTROLLED_TIMEOUT_NAME = "UncontrolledTimeout";
	const std::string DutyAgentEntityData::POLICY_TYPE_NAME  = "PolicyType";
	const std::string DutyAgentEntityData::RAIL_POLICY_VALUE = "Rail";
	const std::string DutyAgentEntityData::ROAD_POLICY_VALUE = "Road";
	const std::string DutyAgentEntityData::ASSET_NAME = "AssetName";

    DutyAgentEntityData::DutyAgentEntityData(unsigned long key) : EntityData(key,getStaticType())
    {
    }

	
    DutyAgentEntityData::~DutyAgentEntityData()
    { // Nothing to clean up
    }


    IEntityData* DutyAgentEntityData::clone(unsigned long key)
    {
        return new DutyAgentEntityData(key);        
    }


    void DutyAgentEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string DutyAgentEntityData::getType()
    {
        return getStaticType();
    }


    std::string DutyAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


	unsigned long DutyAgentEntityData::getUncontrolledTimeout()
	{
		std::string value = getHelper()->getParameter(UNCONTROLLED_TIMEOUT_NAME);
		unsigned long uncontrolledTimeout = 0;
		if (!value.empty())
		{
			uncontrolledTimeout = EntityHelper::getUnsignedLongData(value);
		}
		
        return uncontrolledTimeout;
	}


	std::string DutyAgentEntityData::getPolicyType()
	{
		return getHelper()->getParameter(POLICY_TYPE_NAME);
	}


	std::string DutyAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}

} //close namespace TA_Base_Core

