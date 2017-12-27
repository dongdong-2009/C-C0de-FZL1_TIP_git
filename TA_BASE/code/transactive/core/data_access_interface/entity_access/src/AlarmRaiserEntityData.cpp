/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AlarmRaiserEntityData.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class implements the AlarmRaiserEntityData class.
  *
  */

#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/AlarmRaiserEntityData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
    const std::string AlarmRaiserEntityData::ENTITY_TYPE = "AlarmRaiser";
    // 
    // The entity parameters for this entity type.
    //
    const std::string AlarmRaiserEntityData::USER_ID = "UserId";
    const std::string AlarmRaiserEntityData::PROFILE_ID = "ProfileId";
    const std::string AlarmRaiserEntityData::WORK_STATION_ID = "WorkStationId";
    const std::string AlarmRaiserEntityData::PASSWORD = "Password";

    //
    // Constructor AlarmRaiserEntityData
    //
    AlarmRaiserEntityData::AlarmRaiserEntityData() :
	 EntityData()
    {
        // Nothing else
    }
    
    //
    // Constructor AlarmRaiserEntityData
    //
    AlarmRaiserEntityData::AlarmRaiserEntityData( unsigned long key ) :
    EntityData( key, getStaticType() )
    {
        // Nothing else
    }
    
    //
    //invalidate
    //
    void AlarmRaiserEntityData::invalidate()
    {
        TA_ASSERT(getHelper() != NULL, "The EntityHelper pointer is null");
        getHelper()->invalidate();
    }
    
    //
    // Destructor
    //
    AlarmRaiserEntityData::~AlarmRaiserEntityData()
    {
        // Nothing else
    }
    
    //
    //getType
    //
    std::string AlarmRaiserEntityData::getType()
    {
        return getStaticType();
    }

    std::string AlarmRaiserEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
    
    //
    //getUserId
    //
    unsigned long AlarmRaiserEntityData::getUserId()
    {
		unsigned long userId = 0;
		std::string value = getHelper()->getParameter(USER_ID);
		userId = EntityHelper::getUnsignedLongData(value);

        return userId;
    }
    
    //
    //getProfileId
    //
    unsigned long AlarmRaiserEntityData::getProfileId()
    {
		unsigned long profileId;
		std::string value = getHelper()->getParameter(PROFILE_ID);
		profileId = EntityHelper::getUnsignedLongData(value);

        return profileId;
    }
    
    //
    //getWorkStationId
    //
    unsigned long AlarmRaiserEntityData::getWorkStationId()
    {
		unsigned long workStationId = 0;
		std::string value = getHelper()->getParameter(WORK_STATION_ID);
		workStationId = EntityHelper::getUnsignedLongData(value);

        return workStationId;
    }
    
    //
    //getPassword
    //
    std::string AlarmRaiserEntityData::getPassword()
    {
        return getHelper()->getParameter(PASSWORD);
    }
    
    //
    //clone
    //
    TA_Base_Core::IEntityData* AlarmRaiserEntityData::clone(unsigned long key)
    {
        return( new AlarmRaiserEntityData(key) );
    }
    
    
}
