/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisAgentEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of MisAgentEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MisAgentEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string MisAgentEntityData::ENTITY_TYPE   = "MisAgent";
	const std::string MisAgentEntityData::ASSET_NAME = "AssetName";

    MisAgentEntityData::MisAgentEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {
    }

    MisAgentEntityData::~MisAgentEntityData()
    { // Nothing to clean up
    }

    IEntityData* MisAgentEntityData::clone(unsigned long key)
    {
        return new MisAgentEntityData(key);        
    }

    void MisAgentEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string MisAgentEntityData::getType()
    {
        return getStaticType();
    }
	
    std::string MisAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

    std::string MisAgentEntityData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
    }

}
