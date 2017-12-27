/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/TTISEntityData.cpp $
  * @author:  Adam Radics
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * 
  * TTISEntityData is a concrete implementation of ITTISEntityData.  
  * It provides read only access to TTIS entity data.
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/TTISEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string TTISEntityData::ENTITY_TYPE = "TTis";

    TTISEntityData::TTISEntityData(unsigned long key)
		: EntityData(key, getType())
    {
    }

    
    TTISEntityData::~TTISEntityData()
    {
    }


    std::string TTISEntityData::getType()
    {
        return getStaticType();
    }


    std::string TTISEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}
    
    
    IEntityData* TTISEntityData::clone(unsigned long key)
    {
        return new TTISEntityData(key);        
    }

    
    void TTISEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
