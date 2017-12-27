/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/StationECSAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * StationECSAgentEntityData implements the interface IStationECSAgentEntityData to provide read-only access
  * to StationECSAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include <algorithm>
#include <string>
#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/StationECSAgentEntityData.h"


static std::string toUpper(const std::string& in)
{
    std::string out(in);
    std::transform(out.begin(), out.end(), out.begin(), toupper);
    return out;
}

namespace TA_Base_Core
{

    const std::string StationECSAgentEntityData::ENTITY_TYPE = "StationECSAgent";
    const std::string StationECSAgentEntityData::WORKER_THREADS_SLEEP_TIME_IN_MSECS 
        = "WorkerThreadsSleepTimeInMSecs";
    const std::string StationECSAgentEntityData::MAX_MODE_EXECUTION_TIME_IN_SECS 
        = "MaxModeExecutionTimeInSecs";
    const std::string StationECSAgentEntityData::NORMAL_STATION_MODE 
        = "NormalStationMode";
    const std::string StationECSAgentEntityData::ISCS_ALIVE_POLL_TIME_IN_SECS
        = "IscsAlivePollTimeInSecs";
    const std::string StationECSAgentEntityData::MY_ZONES_CONGESTION_TOKEN
        = "MyZonesCongestionToken";
    const std::string StationECSAgentEntityData::MY_ZONES_EMERGENCY_TOKEN
        = "MyZonesEmergencyToken";
    const std::string StationECSAgentEntityData::MY_MASTER_MODE_LOCK_TOKEN
        = "MyMasterModeLockToken";
    const std::string StationECSAgentEntityData::MY_MASTER_MODE_LOCK_CONTROL_TOKEN
        = "MyMasterModeLockControlToken";
    const std::string StationECSAgentEntityData::MY_SELECTED_MASTER_MODE_TOKEN
        = "MySelectedMasterModeToken";
    const std::string StationECSAgentEntityData::MY_SELECTED_OVERRIDING_STATION_MODE_TOKEN
        = "MySelectedOverridingStationModeToken";
    const std::string StationECSAgentEntityData::MY_LAST_APPLIED_MASTER_MODE_TOKEN
        = "MyLastAppliedMasterModeToken";
    const std::string StationECSAgentEntityData::MY_LAST_APPLIED_STATION_MODE_TOKEN
        = "MyLastAppliedStationModeToken";
    const std::string StationECSAgentEntityData::MODE_STATUS_SQL_TOKEN
        = "ModeStatusSqlToken";
    const std::string StationECSAgentEntityData::MODE_CONTROL_SQL_TOKEN
        = "ModeControlSqlToken";
    const std::string StationECSAgentEntityData::MODE_IN_PROGRESS_SQL_TOKEN
        = "ModeInProgressSqlToken";
    const std::string StationECSAgentEntityData::LOCAL_MODE_SQL_TOKEN
        = "LocalModeSqlToken";
    const std::string StationECSAgentEntityData::ISCS_ALIVE_SQL_TOKEN
        = "IscsAliveSqlToken";
    const std::string StationECSAgentEntityData::AUTO_MODE_SQL_TOKEN
        = "AutoModeSqlToken";
    const std::string StationECSAgentEntityData::SPARE_ATS_AGENT_NAME
        = "SpareAtsAgentName";
	const std::string StationECSAgentEntityData::ASSET_NAME = "AssetName";


	StationECSAgentEntityData::StationECSAgentEntityData()
    {
    }

	
    StationECSAgentEntityData::StationECSAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    StationECSAgentEntityData::~StationECSAgentEntityData()
    {
    }


	std::string StationECSAgentEntityData::getType()
	{
		return getStaticType();
	}


    std::string StationECSAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }

          
    IEntityData* StationECSAgentEntityData::clone ( unsigned long key )
    {
        return new StationECSAgentEntityData ( key );
    }


    void StationECSAgentEntityData::invalidate()
    {
        FUNCTION_ENTRY("invalidate");
        getHelper()->invalidate();

        FUNCTION_EXIT;
    }


    unsigned long StationECSAgentEntityData::getWorkerThreadsSleepTimeInMSecs()
    {
		std::string value = getHelper()->getParameter(WORKER_THREADS_SLEEP_TIME_IN_MSECS);
		unsigned long workerThreadsSleepTimeInMSecs = 0;
		if (!value.empty())
		{
			workerThreadsSleepTimeInMSecs = EntityHelper::getUnsignedLongData(value);
		}

        return workerThreadsSleepTimeInMSecs;
	}


    unsigned long StationECSAgentEntityData::getMaxModeExecutionTimeInSecs()
    {
		std::string value = getHelper()->getParameter(MAX_MODE_EXECUTION_TIME_IN_SECS);
		unsigned long maxModeExecutionTimeInSecs = 0;
		if (!value.empty())
		{
			maxModeExecutionTimeInSecs = EntityHelper::getUnsignedLongData(value);
		}

        return maxModeExecutionTimeInSecs;
	}


    unsigned long StationECSAgentEntityData::getNormalStationMode()
    {
        FUNCTION_ENTRY("getNormalStationMode");

		std::string value = getHelper()->getParameter(NORMAL_STATION_MODE);
		unsigned long normalStationMode = 0;
		if (!value.empty())
		{ 
			normalStationMode = EntityHelper::getUnsignedLongData(value);
		}

        return normalStationMode;
	}


    unsigned long StationECSAgentEntityData::getIscsAlivePollTimeInSecs()
    {
        FUNCTION_ENTRY("getIscsAlivePollTimeInSecs");

		std::string value = getHelper()->getParameter(ISCS_ALIVE_POLL_TIME_IN_SECS);
		unsigned long iscsAlivePollTime = 0;
		if (!value.empty())
		{
			iscsAlivePollTime = EntityHelper::getUnsignedLongData(value);
		}

        return iscsAlivePollTime;
	}


    std::string StationECSAgentEntityData::getSpareAtsAgentName()
    {
		return getHelper()->getParameter(SPARE_ATS_AGENT_NAME);
    }


    std::string StationECSAgentEntityData::getMyZonesCongestionToken()
    {
		return getHelper()->getParameter(MY_ZONES_CONGESTION_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMyZonesEmergencyToken()
    {
		return getHelper()->getParameter(MY_ZONES_EMERGENCY_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMyMasterModeLockToken()
    {
		return getHelper()->getParameter(MY_MASTER_MODE_LOCK_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMyMasterModeLockControlToken()
    {
		return getHelper()->getParameter(MY_MASTER_MODE_LOCK_CONTROL_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMySelectedMasterModeToken()
    {
		return getHelper()->getParameter(MY_SELECTED_MASTER_MODE_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMySelectedOverridingStationModeToken()
    {
		return getHelper()->getParameter(MY_SELECTED_OVERRIDING_STATION_MODE_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMyLastAppliedMasterModeToken()
    {
		return getHelper()->getParameter(MY_LAST_APPLIED_MASTER_MODE_TOKEN);
	}  


    std::string StationECSAgentEntityData::getMyLastAppliedStationModeToken()
    {
		return getHelper()->getParameter(MY_LAST_APPLIED_STATION_MODE_TOKEN);
	}  


    std::string StationECSAgentEntityData::getModeStatusSqlToken()
    {
		return getHelper()->getParameter(MODE_STATUS_SQL_TOKEN);
	}  


    std::string StationECSAgentEntityData::getModeControlSqlToken()
    {
		return getHelper()->getParameter(MODE_CONTROL_SQL_TOKEN);
	}  


    std::string StationECSAgentEntityData::getModeInProgressSqlToken()
    {
		return getHelper()->getParameter(MODE_IN_PROGRESS_SQL_TOKEN);
	}  


    std::string StationECSAgentEntityData::getLocalModeSqlToken()
    {
		return getHelper()->getParameter(LOCAL_MODE_SQL_TOKEN);
	}  


    std::string StationECSAgentEntityData::getIscsAliveSqlToken()
    {
		return getHelper()->getParameter(ISCS_ALIVE_SQL_TOKEN);
	}  


    std::string StationECSAgentEntityData::getAutoModeSqlToken()
    {
		return getHelper()->getParameter(AUTO_MODE_SQL_TOKEN);
	}

	
    std::string StationECSAgentEntityData::getAssetName()
    {
		return getHelper()->getParameter(ASSET_NAME);
	}  

} //close namespace TA_Base_Core
