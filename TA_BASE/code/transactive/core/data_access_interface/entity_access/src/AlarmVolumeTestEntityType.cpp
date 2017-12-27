/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmVolumeTestEntityType.cpp $
  * @author Nick Jardine
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementation for Alarm Volume Testing. Entities of this type contian no useful data.
  */

#ifdef __WIN32__
#pragma warning(disable:4786) // disable the "identifier truncated" warning
#endif

#include "core/data_access_interface/entity_access/src/AlarmVolumeTestEntityType.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{
    const std::string AlarmVolumeTestEntityType::ENTITY_TYPE = "AlarmVolumeTestEntityType";
    
    AlarmVolumeTestEntityType::AlarmVolumeTestEntityType(unsigned long key) 
        : EntityData(key,getStaticType())
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AlarmVolumeTestEntityType (constructor)");       
        LOG( SourceInfo, DebugUtil::FunctionExit, "AlarmVolumeTestEntityType (constructor)");
    }

    AlarmVolumeTestEntityType::~AlarmVolumeTestEntityType()
    { // Nothing to clean up
    }

    
    
    IEntityData* AlarmVolumeTestEntityType::clone(unsigned long key)
    {
        return new AlarmVolumeTestEntityType(key);        
    }

    void AlarmVolumeTestEntityType::invalidate()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "AlarmVolumeTestEntityType::invalidate");
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        getHelper()->invalidate();
        LOG( SourceInfo, DebugUtil::FunctionExit, "AlarmVolumeTestEntityType::invalidate");
    }


    std::string AlarmVolumeTestEntityType::getType()
    {
        return getStaticType();
    }

    std::string AlarmVolumeTestEntityType::getStaticType()
    {
        return ENTITY_TYPE;
    }

} //close namespace TA_Base_Core
