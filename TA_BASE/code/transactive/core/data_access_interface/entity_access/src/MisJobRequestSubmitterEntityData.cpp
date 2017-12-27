/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisJobRequestSubmitterEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of   MisJobRequestSubmitterEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MisJobRequestSubmitterEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string MisJobRequestSubmitterEntityData::ENTITY_TYPE = "MisJobRequestSubmitter";

    MisJobRequestSubmitterEntityData::MisJobRequestSubmitterEntityData(unsigned long key)
        : EntityData(key,getStaticType())
    {
    }


    MisJobRequestSubmitterEntityData::~  MisJobRequestSubmitterEntityData()
    { // Nothing to clean up
    }


    IEntityData* MisJobRequestSubmitterEntityData::clone(unsigned long key)
    {
        return new   MisJobRequestSubmitterEntityData(key);        
    }


    void MisJobRequestSubmitterEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string MisJobRequestSubmitterEntityData::getType()
    {
        return getStaticType();
    }


    std::string MisJobRequestSubmitterEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}
