/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/RegionManagerEntityData.cpp $
  * @author Bart Golab
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implement the IEntityData interface for Region Manager entity configuration access.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/RegionManagerEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
    RegionManagerEntityData::RegionManagerEntityData() :
        EntityData()
    {
    }


    RegionManagerEntityData::RegionManagerEntityData(unsigned long key) :
        EntityData(key, getStaticType())
    {
    }


    RegionManagerEntityData::~RegionManagerEntityData()
    {
    }


    void RegionManagerEntityData::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");

        getHelper()->invalidate();
    }


    std::string RegionManagerEntityData::getType()
    {
		return getStaticType();
    }


    std::string RegionManagerEntityData::getStaticType()
    {
        static const std::string REGION_MANAGER_ENTITY_TYPE("RegionManager");

        return REGION_MANAGER_ENTITY_TYPE;
    }


    IEntityData* RegionManagerEntityData::clone(unsigned long key)
    {
        return (new RegionManagerEntityData(key));
    }
} // namespace TA_Base_Core
