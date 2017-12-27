/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/MasterECSAgentEntityData.cpp $
  * @author HoaVu
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * MasterECSAgentEntityData implements the interface IMasterECSAgentEntityData to provide read-only access
  * to MasterECSAgent entity data.
  */

#if defined(_MSC_VER)
#pragma warning(disable: 4786 4250)
#endif // defined _MSC_VER

#include "core/data_access_interface/src/IData.h"
#include "core/utilities/src/TAAssert.h"
#include "core/data_access_interface/entity_access/src/MasterECSAgentEntityData.h"

namespace TA_Base_Core
{

    const std::string MasterECSAgentEntityData::ENTITY_TYPE = "MasterECSAgent";
    const std::string MasterECSAgentEntityData::WORKER_THREADS_SLEEP_TIME_IN_MSECS 
        = "WorkerThreadsSleepTimeInMSecs";
    const std::string MasterECSAgentEntityData::MAX_MODE_EXECUTION_TIME_IN_SECS 
        = "MaxModeExecutionTimeInSecs";
    const std::string MasterECSAgentEntityData::SELECTION_TIMEOUT_IN_SECS 
        = "SelectionTimeoutInSecs";
    const std::string MasterECSAgentEntityData::NORMAL_MASTER_MODE
        = "NormalMasterMode";
    const std::string MasterECSAgentEntityData::MY_ZONES_CONGESTION_TOKEN
        = "MyZonesCongestionToken";
    const std::string MasterECSAgentEntityData::MY_ZONES_EMERGENCY_TOKEN
        = "MyZonesEmergencyToken";
    //const std::string MasterECSAgentEntityData::MY_ZONES_CONGESTION_APPLIED_TOKEN 
    //    = "MyZonesCongestionAppliedToken";
    const std::string MasterECSAgentEntityData::MY_ZONES_EMERGENCY_APPLIED_TOKEN
        = "MyZonesEmergencyAppliedToken";
    const std::string MasterECSAgentEntityData::MY_ZONES_EVACUATION_SELECTED_TOKEN
        = "MyZonesEvacuationSelectedToken";
    const std::string MasterECSAgentEntityData::MY_ZONES_AIRFLOW_SELECTED_TOKEN
        = "MyZonesAirflowSelectedToken";
    const std::string MasterECSAgentEntityData::MY_ZONES_EVACUATION_APPLIED_TOKEN
        = "MyZonesEvacuationAppliedToken";
    const std::string MasterECSAgentEntityData::MY_ZONES_AIRFLOW_APPLIED_TOKEN
        = "MyZonesAirflowAppliedToken";
    const std::string MasterECSAgentEntityData::MY_MODE_STATUS_TOKEN
        = "MyModeStatusToken";
    const std::string MasterECSAgentEntityData::MY_MODE_IN_PROGRESS_TOKEN
        = "MyModeInProgressToken";
    const std::string MasterECSAgentEntityData::MASTER_MODE_LOCKS_SQL_TOKEN
        = "MasterModeLocksSqlToken";
    const std::string MasterECSAgentEntityData::SELECTED_MASTER_MODES_SQL_TOKEN
        = "SelectedMasterModesSqlToken";
    const std::string MasterECSAgentEntityData::SELECTED_OVERRIDING_STATION_MODES_TOKEN
        = "SelectedOverridingStationModesSqlToken";
    const std::string MasterECSAgentEntityData::LAST_APPLIED_MASTER_MODES_SQL_TOKEN
        = "LastAppliedMasterModesSqlToken";
    const std::string MasterECSAgentEntityData::LAST_APPLIED_STATION_MODES_SQL_TOKEN
        = "LastAppliedStationModesSqlToken";
    const std::string MasterECSAgentEntityData::LOCAL_MODES_SQL_TOKEN
        = "LocalModesSqlToken";
    const std::string MasterECSAgentEntityData::SPARE_ATS_AGENT_NAME
        = "SpareAtsAgentName";
	const std::string MasterECSAgentEntityData::ASSET_NAME = "AssetName";
    

	MasterECSAgentEntityData::MasterECSAgentEntityData()
    {
    }

	
    MasterECSAgentEntityData::MasterECSAgentEntityData ( unsigned long key )
	:
	EntityData( key, getStaticType() )
    {
    }


    MasterECSAgentEntityData::~MasterECSAgentEntityData()
    {
    }


	std::string MasterECSAgentEntityData::getType()
	{
		return getStaticType();
	}


    std::string MasterECSAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }


    IEntityData* MasterECSAgentEntityData::clone ( unsigned long key )
    {
        return new MasterECSAgentEntityData ( key );
    }

    void MasterECSAgentEntityData::invalidate()
    {
        getHelper()->invalidate();
    }

 
    unsigned long MasterECSAgentEntityData::getWorkerThreadsSleepTimeInMSecs()
    {
		std::string value = getHelper()->getParameter(WORKER_THREADS_SLEEP_TIME_IN_MSECS);
		unsigned long workerThreadsSleepTimeInMSecs = 0;
		if (!value.empty())
		{
			workerThreadsSleepTimeInMSecs = EntityHelper::getUnsignedLongData(value);
		}

        return workerThreadsSleepTimeInMSecs;
	}


    unsigned long MasterECSAgentEntityData::getMaxModeExecutionTimeInSecs()
    {
		std::string value = getHelper()->getParameter(MAX_MODE_EXECUTION_TIME_IN_SECS);
		unsigned long maxModeExecutionTimeInSecs = 0;
		if (!value.empty())
		{
			maxModeExecutionTimeInSecs = EntityHelper::getUnsignedLongData(value);
		}

        return maxModeExecutionTimeInSecs;
	}


    unsigned long MasterECSAgentEntityData::getSelectionTimeoutInSecs()
    {
		std::string value = getHelper()->getParameter(SELECTION_TIMEOUT_IN_SECS);
		unsigned long selectionTimeoutInSecs = 0;
		if (!value.empty())
		{
			selectionTimeoutInSecs = EntityHelper::getUnsignedLongData(value);
		}

        return selectionTimeoutInSecs;
	}


    unsigned long MasterECSAgentEntityData::getNormalMasterMode()
    {
		std::string value = getHelper()->getParameter(NORMAL_MASTER_MODE);
		unsigned long normalMasterMode = 0;
		if (!value.empty())
		{
			normalMasterMode = EntityHelper::getUnsignedLongData(value);
		}

        return normalMasterMode;
	}     


    std::string MasterECSAgentEntityData::getSpareAtsAgentName()
    {
		return getHelper()->getParameter(SPARE_ATS_AGENT_NAME);		
    }


    std::string MasterECSAgentEntityData::getMyZonesCongestionToken()
    {
		return getHelper()->getParameter(MY_ZONES_CONGESTION_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyZonesEmergencyToken()
    {
		return getHelper()->getParameter(MY_ZONES_EMERGENCY_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyZonesEmergencyAppliedToken()
    {
		return getHelper()->getParameter(MY_ZONES_EMERGENCY_APPLIED_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyZonesEvacuationSelectedToken()
    {
		return getHelper()->getParameter(MY_ZONES_EVACUATION_SELECTED_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyZonesAirflowSelectedToken()
    {
		return getHelper()->getParameter(MY_ZONES_AIRFLOW_SELECTED_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyZonesEvacuationAppliedToken()
    {
		return getHelper()->getParameter(MY_ZONES_EVACUATION_APPLIED_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyZonesAirflowAppliedToken()
    {
		return getHelper()->getParameter(MY_ZONES_AIRFLOW_APPLIED_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyModeStatusToken()
    {
		return getHelper()->getParameter(MY_MODE_STATUS_TOKEN);
	}


    std::string MasterECSAgentEntityData::getMyModeInProgressToken()
    {
		return getHelper()->getParameter(MY_MODE_IN_PROGRESS_TOKEN);
	}

	
    std::string MasterECSAgentEntityData::getMasterModeLocksSqlToken()
    {
		return getHelper()->getParameter(MASTER_MODE_LOCKS_SQL_TOKEN);
	}

	
    std::string MasterECSAgentEntityData::getSelectedMasterModesSqlToken()
    {
		return getHelper()->getParameter(SELECTED_MASTER_MODES_SQL_TOKEN);
	}


    std::string MasterECSAgentEntityData::getSelectedOverridingStationModesSqlToken()
    {
		return  getHelper()->getParameter(SELECTED_OVERRIDING_STATION_MODES_TOKEN);
	}


    std::string MasterECSAgentEntityData::getLastAppliedMasterModesSqlToken()
    {
		return getHelper()->getParameter(LAST_APPLIED_MASTER_MODES_SQL_TOKEN);
	}


    std::string MasterECSAgentEntityData::getLastAppliedStationModesSqlToken()
    {
		return getHelper()->getParameter(LAST_APPLIED_STATION_MODES_SQL_TOKEN);
	}


    std::string MasterECSAgentEntityData::getLocalModesSqlToken()
    {
		return getHelper()->getParameter(LOCAL_MODES_SQL_TOKEN);
	}


	std::string MasterECSAgentEntityData::getAssetName()
    {
		std::string assetName("Unknown");
		std::string value = getHelper()->getParameter(ASSET_NAME);
		if (!value.empty())
		{
			assetName = value;
		}
        return assetName;
	}

} //close namespace TA_Base_Core
