/**
  * The source code in this file is the property of ST Electronics and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/PEnergyAgentEntityData.cpp $
  * @author Liu Yong
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * PEnergyAgentEntityData implements the interface IEntityData to provide read-only access
  * to PEnergyAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/PEnergyAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

namespace TA_Base_Core
{

    const std::string PEnergyAgentEntityData::ENTITY_TYPE    = "PEnergyAgent";
    const std::string PEnergyAgentEntityData::STATIONNAME    = "StationName";
	const std::string PEnergyAgentEntityData::ASSET_NAME     = "AssetName";

	PEnergyAgentEntityData::PEnergyAgentEntityData()
    {
    }

	
    PEnergyAgentEntityData::PEnergyAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }

    PEnergyAgentEntityData::~PEnergyAgentEntityData()
    {
    }

    const std::string PEnergyAgentEntityData::getStationName()
	{
        return getHelper()->getParameter(STATIONNAME);
	}


    const std::string PEnergyAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}


	std::string PEnergyAgentEntityData::getType()
	{
		return getStaticType();
	}

    std::string PEnergyAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    IEntityData* PEnergyAgentEntityData::clone ( unsigned long key )
    {
        return new PEnergyAgentEntityData ( key );
    }

    void PEnergyAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

} //close namespace TA_Base_Core
