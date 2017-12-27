/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationPAAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * StationPAAgentEntityData implements the interface IStationPAAgentEntityData to provide read-only access
  * to StationPAAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/StationPAAgentEntityData.h"


namespace TA_Base_Core
{

    const std::string StationPAAgentEntityData::ENTITY_TYPE = "StationPAAgent";
	StationPAAgentEntityData::StationPAAgentEntityData()
    {
    }

	
    StationPAAgentEntityData::StationPAAgentEntityData ( unsigned long key )
	: EntityData( key, getType() )
    {
    }

    StationPAAgentEntityData::~StationPAAgentEntityData()
    {
    }

    bool StationPAAgentEntityData::reloadSpecificParameter( IData& data )
    {
        // Nothing processed for now ...
        return false;
    }

    std::string StationPAAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string StationPAAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

} //close namespace TA_Base_Core
