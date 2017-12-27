/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/IMasterECSAgentEntityData.h $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * IMasterECSAgentEntityData provides read-only access to MasterECSAgent entity data.
  */

#ifndef IMasterECSAgent_ENTITY_DATA_H
#define IMasterECSAgent_ENTITY_DATA_H

#include <vector>
#include "core/data_access_interface/entity_access/src/IEntityData.h"

namespace TA_Base_Core
{

    class IMasterECSAgentEntityData : public virtual IEntityData
    {
    public:

        virtual unsigned long       getWorkerThreadsSleepTimeInMSecs() = 0;
        virtual unsigned long       getMaxModeExecutionTimeInSecs() = 0;
        virtual unsigned long       getSelectionTimeoutInSecs() = 0;
        virtual unsigned long       getNormalMasterMode() = 0;
        virtual std::string         getSpareAtsAgentName() =0;

        virtual std::string         getMyZonesCongestionToken() = 0;
        virtual std::string         getMyZonesEmergencyToken() = 0;
        //virtual std::string         getMyZonesCongestionAppliedToken() = 0; // Not required
        virtual std::string         getMyZonesEmergencyAppliedToken() = 0;
        virtual std::string         getMyZonesEvacuationSelectedToken() = 0;
        virtual std::string         getMyZonesAirflowSelectedToken() = 0;
        virtual std::string         getMyZonesEvacuationAppliedToken() = 0;
        virtual std::string         getMyZonesAirflowAppliedToken() = 0;
        virtual std::string         getMyModeStatusToken() = 0;
        virtual std::string         getMyModeInProgressToken() = 0;

        virtual std::string         getMasterModeLocksSqlToken() = 0;
        virtual std::string         getSelectedMasterModesSqlToken() = 0;
        virtual std::string         getSelectedOverridingStationModesSqlToken() = 0;
        virtual std::string         getLastAppliedMasterModesSqlToken() = 0;
        virtual std::string         getLastAppliedStationModesSqlToken() = 0;
        virtual std::string         getLocalModesSqlToken() = 0;

		virtual std::string			getAssetName() = 0;

    };

} //close namespace TA_Base_Core

#endif

