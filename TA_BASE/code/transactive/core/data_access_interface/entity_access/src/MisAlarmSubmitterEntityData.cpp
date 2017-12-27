/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MisAlarmSubmitterEntityData.cpp $
  * @author Gregg Kirkpatrick
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Implementataion of  MisAlarmSubmitterEntityData.
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/MisAlarmSubmitterEntityData.h"
#include "core/data_access_interface/src/IData.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::DebugUtil;

namespace TA_Base_Core
{

    const std::string MisAlarmSubmitterEntityData::ENTITY_TYPE  = "MisAlarmSubmitter";

    MisAlarmSubmitterEntityData::MisAlarmSubmitterEntityData(unsigned long key) 
        : EntityData(key,getStaticType())
    {

    }


     MisAlarmSubmitterEntityData::~MisAlarmSubmitterEntityData()
    {

    }


    IEntityData*  MisAlarmSubmitterEntityData::clone(unsigned long key)
    {
        return new  MisAlarmSubmitterEntityData(key);        
    }


    void  MisAlarmSubmitterEntityData::invalidate()
    {
		getHelper()->invalidate();
    }


    std::string  MisAlarmSubmitterEntityData::getType()
    {
        return getStaticType();
    }


    std::string  MisAlarmSubmitterEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
}
