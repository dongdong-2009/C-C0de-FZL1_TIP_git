/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IStationECSAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IStationECSAgentEntityData provides read-only access to StationECSAgent entity data.
  */

#ifndef IStationECSAgent_ENTITY_DATA_H
#define IStationECSAgent_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IStationECSAgentEntityData : public virtual IEntityData
    {
    public:

        virtual unsigned long       getWorkerThreadsSleepTimeInMSecs() = 0;
        virtual unsigned long       getMaxModeExecutionTimeInSecs() = 0;
        virtual unsigned long       getNormalStationMode() = 0;
        virtual unsigned long       getIscsAlivePollTimeInSecs() = 0;
        virtual std::string         getSpareAtsAgentName() =0;
        
        virtual std::string         getMyZonesCongestionToken()  = 0;
        virtual std::string         getMyZonesEmergencyToken()  = 0;
        virtual std::string         getMyMasterModeLockToken()  = 0;
        virtual std::string         getMyMasterModeLockControlToken()  = 0;
        virtual std::string         getMySelectedMasterModeToken()  = 0;
        virtual std::string         getMySelectedOverridingStationModeToken()  = 0;
        virtual std::string         getMyLastAppliedMasterModeToken()  = 0;
        virtual std::string         getMyLastAppliedStationModeToken()  = 0;

        virtual std::string         getModeStatusSqlToken()  = 0;
        virtual std::string         getModeControlSqlToken()  = 0;
        virtual std::string         getModeInProgressSqlToken()  = 0;
        virtual std::string         getLocalModeSqlToken()  = 0;
        virtual std::string         getIscsAliveSqlToken() = 0;      
        virtual std::string         getAutoModeSqlToken() = 0;

		virtual std::string			getAssetName() = 0;
        
    };

} //close namespace TA_Base_Core

#endif

