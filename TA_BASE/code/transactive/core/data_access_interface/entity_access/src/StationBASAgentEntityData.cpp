#include "core/data_access_interface/entity_access/src/StationBASAgentEntityData.h"
#include "core/utilities/src/DebugUtil.h"

namespace TA_Base_Core
{
	const std::string StationBASAgentEntityData::ENTITY_TYPE                    = "StationBASAgent";

	const std::string StationBASAgentEntityData::MY_MODE_CONTROL_SQL_TOKEN		= "ModeControlSqlToken";
	const std::string StationBASAgentEntityData::MY_MODE_VALUE_SQL_TOKEN		= "ModeValueSqlToken";
	const std::string StationBASAgentEntityData::MY_MODE_EXEC_STATUS_TOKEN		= "ModeExecStatusSqlToken";
	const std::string StationBASAgentEntityData::MY_MODE_RIGHT_STATUS_TOKEN		= "ModeRightStatusSqlToken";
	const std::string StationBASAgentEntityData::MY_MODE_RIGHT_CONTROL_TOKEN	= "ModeRightControlSqlToken";
    const std::string StationBASAgentEntityData::MY_FIRE_MODE_RECOVER_DP_TAG    = "FireModeRecoverDpTag";

	const std::string StationBASAgentEntityData::CURRENT_DAY_TT_PT_BGN			= "CurrentDayTTPtBGN";
	const std::string StationBASAgentEntityData::NEXT_DAY_TT_PT_BGN				= "NextDayTTPtBGN";
	const std::string StationBASAgentEntityData::TT_SCHEDULE_TIME				= "TTScheduleTime";
	const std::string StationBASAgentEntityData::STATION_FIRE_MONITOR_SECOND	= "StationFireMonitorSecond"; //GF-268

	const std::string StationBASAgentEntityData::SCANTIME                               = "ScanTimeMSecs";
	const std::string StationBASAgentEntityData::STATIONNAME                            = "StationName";
	const std::string StationBASAgentEntityData::ASSET_NAME                             = "AssetName";
	const std::string StationBASAgentEntityData::ANALOGUE_DP_UPDATE_INTERVAL            = "AnalogueDataPointUpdateInterval";
	const std::string StationBASAgentEntityData::RTU_SWITCHING_DISABLED                 = "RTUSwitchingDisabled";

	StationBASAgentEntityData::StationBASAgentEntityData()
	{
	}


	StationBASAgentEntityData::StationBASAgentEntityData ( unsigned long key )
		:EntityData( key, getStaticType() )
	{
	}


	StationBASAgentEntityData::~StationBASAgentEntityData()
	{
	}

	std::string StationBASAgentEntityData::getStaticType()
	{
		return ENTITY_TYPE;
	}

	std::string StationBASAgentEntityData::getType()
	{
		return getStaticType();
	}

	IEntityData* StationBASAgentEntityData::clone (unsigned long key)
	{
		return new StationBASAgentEntityData(key);
	}
	
	void StationBASAgentEntityData::invalidate()
	{
		FUNCTION_ENTRY("invalidate");

		getHelper()->invalidate();

		FUNCTION_EXIT;
	}

	std::string StationBASAgentEntityData::getMyModeControlToken()
	{
		return getHelper()->getParameter(MY_MODE_CONTROL_SQL_TOKEN);
	}

	std::string StationBASAgentEntityData::getMyModeValueToken()
	{
		return getHelper()->getParameter(MY_MODE_VALUE_SQL_TOKEN);
	}

	std::string StationBASAgentEntityData::getMyModeExecutionStatusToken()
	{
		return getHelper()->getParameter(MY_MODE_EXEC_STATUS_TOKEN);
	}

	std::string StationBASAgentEntityData::getMyModeRightStatusToken()
	{
		return getHelper()->getParameter(MY_MODE_RIGHT_STATUS_TOKEN);
	}

	std::string StationBASAgentEntityData::getMyModeRightsControlToken()
	{
		return getHelper()->getParameter(MY_MODE_RIGHT_CONTROL_TOKEN);
	}

	std::string StationBASAgentEntityData::getCurrentDayTTPtBGN()
	{
		return getHelper()->getParameter(CURRENT_DAY_TT_PT_BGN);
	}

	std::string StationBASAgentEntityData::getNextDayTTPtBGN()
	{
		return getHelper()->getParameter(NEXT_DAY_TT_PT_BGN);
	}

	std::string StationBASAgentEntityData::getTTScheduleTime()
	{
		return getHelper()->getParameter(TT_SCHEDULE_TIME);
	}
	
	std::string StationBASAgentEntityData::getMyFireModeRecoverDpTag()
    {
        return getHelper()->getParameter(MY_FIRE_MODE_RECOVER_DP_TAG);
    }

	//GF-268
	unsigned long StationBASAgentEntityData::getFireStationMonitorSecond()
	{
		const unsigned long DefaultMonitorSeconds = 20;

		unsigned long retValue = DefaultMonitorSeconds;
		std::string value = getHelper()->getParameter(STATION_FIRE_MONITOR_SECOND);
		if (!value.empty())
		{
			retValue = EntityHelper::getUnsignedLongData(value);
		}
		else
		{
			retValue = DefaultMonitorSeconds;
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Using the default station fire monitor time : %d seconds", DefaultMonitorSeconds);
		}
		return retValue;
	}
	/************************************************************************/
	/*               Implementation of IPMSAgentEntityData                  */
	/************************************************************************/
	
	int StationBASAgentEntityData::getScanTimeMSecs()
	{
		int scanTimeMSecs = 250;
		std::string value = getHelper()->getParameter(SCANTIME);
		if (!value.empty())
		{
			scanTimeMSecs = EntityHelper::getIntegerData(value);
		}
		return scanTimeMSecs;
	}

	const std::string StationBASAgentEntityData::getStationName()
	{
		return getHelper()->getParameter(STATIONNAME);
	}

	std::string StationBASAgentEntityData::getAssetName()
	{
		return getHelper()->getParameter(ASSET_NAME);
	}

	int StationBASAgentEntityData::getAnalogueDataPointUpdateInterval()
	{
		int analogueDataPointUpdateInterval = 0;
		std::string value = getHelper()->getParameter(ANALOGUE_DP_UPDATE_INTERVAL);
		if (!value.empty())
		{
			analogueDataPointUpdateInterval = EntityHelper::getIntegerData(value);
		}

		return analogueDataPointUpdateInterval;
	}

	bool StationBASAgentEntityData::getIsRTUSwitchingDisabled()
	{
		bool isRTUSwitchingDisabled = false;
		std::string value = getHelper()->getParameter(RTU_SWITCHING_DISABLED);
		if (!value.empty())
		{
			isRTUSwitchingDisabled = EntityHelper::getBooleanData(value);
		}
		return isRTUSwitchingDisabled;
	}
}
