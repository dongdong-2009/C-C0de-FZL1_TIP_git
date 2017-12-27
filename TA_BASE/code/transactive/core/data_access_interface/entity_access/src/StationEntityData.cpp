/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * StationEntity implements the interface IStationEntityData to provide read-only access
  * to Station entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/StationEntityData.h"

namespace TA_Base_Core
{

    const std::string StationEntityData::ENTITY_TYPE = "Station";

	StationEntityData::StationEntityData()
    {
    }

	
    StationEntityData::StationEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    StationEntityData::~StationEntityData()
    {
    }


	std::string StationEntityData::getType()
	{
		return getStaticType();
	}


    std::string StationEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* StationEntityData::clone ( unsigned long key )
    {
        return new StationEntityData ( key );
    }


    void StationEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
