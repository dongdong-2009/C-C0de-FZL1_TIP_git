/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AtsPsdEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * <description>
  *
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/AtsPsdEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string AtsPsdEntityData::ENTITY_TYPE = "AtsPsd";

    AtsPsdEntityData::AtsPsdEntityData(unsigned long key)
		: EntityData(key, getType()), m_isValidData(false)
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AtsPsdEntityData (constructor)");
        LOG( SourceInfo, DebugUtil::FunctionExit, "AtsPsdEntityData (constructor)");
    }

    
    AtsPsdEntityData::~AtsPsdEntityData()
    {
    }


    std::string AtsPsdEntityData::getType()
    {
        return getStaticType();
    }


    std::string AtsPsdEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}
    
    
    IEntityData* AtsPsdEntityData::clone(unsigned long key)
    {
        return new AtsPsdEntityData(key);        
    }

    
    void AtsPsdEntityData::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AtsPsdEntityData::invalidate");

        getHelper()->invalidate();
        m_isValidData = false;

        LOG( SourceInfo, DebugUtil::FunctionExit, "AtsPsdEntityData::invalidate");
    }


} //close namespace TA_Base_Core
