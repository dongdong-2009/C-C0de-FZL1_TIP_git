/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmStoreEntityData.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * AlarmEntityData is a concrete implementation of IAlarmStoreEntityData, which is in turn an implementation of
  * IEntityData. It proves read-only access to alarm store data, and a specific framework
  * for alarm store parameters.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/AlarmStoreEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string AlarmStoreEntityData::ENTITY_TYPE = "AlarmStore";
    AlarmStoreEntityData::AlarmStoreEntityData(unsigned long key)
		: EntityData(key, getType())
    {
        FUNCTION_ENTRY("Constructor");
		FUNCTION_EXIT;
    }

    AlarmStoreEntityData::~AlarmStoreEntityData()
    {
        FUNCTION_ENTRY("Destructor");
		FUNCTION_EXIT;
    }

    
    std::string AlarmStoreEntityData::getType()
    {
        FUNCTION_ENTRY("getType");
		FUNCTION_EXIT;
        return getStaticType();
    }


	std::string AlarmStoreEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

    
    IEntityData* AlarmStoreEntityData::clone(unsigned long key)
    {
        FUNCTION_ENTRY("clone");
		FUNCTION_EXIT;
		return new AlarmStoreEntityData(key);        
    }


    void AlarmStoreEntityData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");

        getHelper()->invalidate();

		FUNCTION_EXIT;
    }


} //close namespace TA_Base_Core

