/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AtsAgentEntityData.cpp $
  * @author:  Darren Sampson
  * @version: $Revision: #13 $
  *
  * Last modification: $DateTime: 2017/09/08 13:17:19 $
  * Last modified by:  $Author: jiayi.zhang $
  * 
  * AtsAgentEntityData provide read-only access to AtsAgent entity data.
  */


#if defined(_MSC_VER)
#pragma warning(disable:4786)
#endif // defined _MSC_VER

#include "core/data_access_interface/entity_access/src/AtsAgentEntityData.h"
#include "core/utilities/src/TAAssert.h"

namespace TA_Base_Core
{
	
    const std::string AtsAgentEntityData::ENTITY_TYPE = "AtsAgent";
    const std::string AtsAgentEntityData::ATS1_IP_ADDRESS = "AtsServer1IpAddress";
	const std::string AtsAgentEntityData::ATS1_SERVICE_PORT = "AtsServer1ServicePort";
	const std::string AtsAgentEntityData::ATS2_IP_ADDRESS = "AtsServer2IpAddress";
	const std::string AtsAgentEntityData::ATS2_SERVICE_PORT = "AtsServer2ServicePort";
	const std::string AtsAgentEntityData::AGENT_ATS_LOCATION_TYPE = "AgentAtsLocationType";
	const std::string AtsAgentEntityData::ATS_TABLES_BASE_ADDRESS = "AtsTablesBaseAddress";
	const std::string AtsAgentEntityData::ISCS_TABLES_BASE_ADDRESS = "IscsTablesBaseAddress";
	const std::string AtsAgentEntityData::MAX_COMMS_RETRIES = "MaxCommsRetries";
	const std::string AtsAgentEntityData::MAX_WATCHDOG_INTERVAL = "MaxWatchdogInterval";
	const std::string AtsAgentEntityData::TIMETBL_CHK_INTERVAL = "TimetblCheckIntervalMins";
	const std::string AtsAgentEntityData::TIMETBL_DIR_PATH = "TimetblDirPath";

	const std::string AtsAgentEntityData::ATS_LOC_TYPE_OCC = "OCC";
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_DEPOT = "DPT";
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_MAIN_STATION = "MSS";		
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_SECONDARY_STATION_1 = "SSS1";
	const std::string AtsAgentEntityData::ATS_LOC_TYPE_SECONDARY_STATION_2 = "SSS2";

	const std::string AtsAgentEntityData::OA1_TIMEOUT = "OA1TableTimeout";
	const std::string AtsAgentEntityData::ATC_TIMEOUT = "ATCDataTimeout";

	const std::string AtsAgentEntityData::ASSET_NAME = "AssetName";

	
	const std::string AtsAgentEntityData::DP_TRAIN_PIS = "TrainPisDPAddress";

	//Train
	const std::string AtsAgentEntityData::DP_TRAIN_DIRECTION = "Train Direction";       //cannot be changed due to it is used by FZL1,GZL9,GZL13
	const std::string AtsAgentEntityData::DP_TRAINSERVICENUM = "Train Service Number";  //cannot be changed due to it is used by FZL1,GZL9,GZL13
	const std::string AtsAgentEntityData::DP_STOPFLAG = "Stop Flag";   //Travel Time Execeeded, cannot be changed due to it is used by FZL1,GZL9,GZL13
	const std::string AtsAgentEntityData::DP_TRAIN_DRIVE_MODE = "SIG_Drive_Mode";
	const std::string AtsAgentEntityData::DP_TRAIN_POS_LOST = "SIG_Train_LostFlag";
	
	//const std::string AtsAgentEntityData::DP_GENA_TRAIN_TIME_OUT = "diiSIG-TraRunTim";	
	
	//Block
	const std::string AtsAgentEntityData::DP_BLK_STAT= "SIG_Block_Status";
 	
	//Switch
	const std::string AtsAgentEntityData::DP_SWITCH_POS= "SIG_Switch_Position";
	const std::string AtsAgentEntityData::DP_SWITCH_FAULTSTAT= "SIG_Switch_FailStat";
	const std::string AtsAgentEntityData::DP_SWITCH_RESSTAT= "SIG_Switch_ResStat";
	
	//Signal
	const std::string AtsAgentEntityData::DP_SIG_DISPLAYSTAT = "SIG_SIG_DisplayStat";
	const std::string AtsAgentEntityData::DP_SIG_HEALTHSTAT = "SIG_SIG_HealthStat";
	
	//Platform
	const std::string AtsAgentEntityData::DP_SIG_PLATFORM_ARRTIME = "SIG_Platform_ArrTime";
	const std::string AtsAgentEntityData::DP_SIG_PLATFORM_JUMPSTAT = "SIG_Platform_JumpStat";
	const std::string AtsAgentEntityData::DP_SIG_PLATFORM_HOLDSTAT = "SIG_Platform_HoldStat";

	
	//Emergency Stop Button
	const std::string AtsAgentEntityData::DP_SIG_ESB_STAT = "SIG_ESB_Stat";
	
	//Flood Gate
	const std::string AtsAgentEntityData::DP_SIG_FG_REQCLOSE = "SIG_FG_ReqClose";
	const std::string AtsAgentEntityData::DP_SIG_FG_PERMISSIONCLOSE = "SIG_FG_PermClose";
	
	//Platform Door
	const std::string AtsAgentEntityData::DP_SIG_PD_OVERRIDED = "SIG_PlatformDoor_Override";

    AtsAgentEntityData::AtsAgentEntityData() :
        m_isValidData(false),  m_atsServer1IpAddress (""), m_atsServer1ServicePort (0),
		m_atsServer2IpAddress (""), m_atsServer2ServicePort (0), m_agentAtsLocationType (Unknown), 
		m_atsTablesBaseAddress (0), m_iscsTablesBaseAddress (0), 
		m_oa1Timeout(0),m_atcTimeout(0), m_assetName("")
    {}

    AtsAgentEntityData::AtsAgentEntityData( unsigned long key ) :
		EntityData( key, getStaticType() ),
        m_isValidData(false)
    {
    }

    void AtsAgentEntityData::invalidate()
    {
        getHelper()->invalidate();

        m_isValidData = false;
    }

    AtsAgentEntityData::~AtsAgentEntityData()
    {
    }

    std::string AtsAgentEntityData::getType()
    {
        return getStaticType();
    }

    std::string AtsAgentEntityData::getStaticType()
    {
        return ENTITY_TYPE;
    }
    
	std::string AtsAgentEntityData::getAtsServer1IpAddress()
    {
		m_atsServer1IpAddress = getHelper()->getParameter(ATS1_IP_ADDRESS);

        return m_atsServer1IpAddress;
    }

	std::string AtsAgentEntityData::getAtsServer1ServicePort()
    {
		m_atsServer1ServicePort = getHelper()->getParameter(ATS1_SERVICE_PORT);

        return m_atsServer1ServicePort;
    }

	std::string AtsAgentEntityData::getAtsServer2IpAddress()
    {
		m_atsServer2IpAddress = getHelper()->getParameter(ATS2_IP_ADDRESS);

        return m_atsServer2IpAddress;
    }

	std::string AtsAgentEntityData::getAtsServer2ServicePort()
    {
		m_atsServer2ServicePort = getHelper()->getParameter(ATS2_SERVICE_PORT);

        return m_atsServer2ServicePort;
    }

	std::string AtsAgentEntityData::getAssetName()
    {
		m_assetName = getHelper()->getParameter(ASSET_NAME);

        return m_assetName;
    }

	AtsAgentEntityData::EAtsLocationType AtsAgentEntityData::getAgentAtsLocationType()
	{
		std::string value = getHelper()->getParameter(AGENT_ATS_LOCATION_TYPE);
		if (ATS_LOC_TYPE_OCC == value)
		{
			m_agentAtsLocationType = Occ;
		}
		else if (ATS_LOC_TYPE_DEPOT == value)
		{
			m_agentAtsLocationType = Depot;
		}
		else if (ATS_LOC_TYPE_MAIN_STATION == value)
		{
			m_agentAtsLocationType = MainStation;
		}
		else if (ATS_LOC_TYPE_SECONDARY_STATION_1 == value)
		{
			m_agentAtsLocationType = SecondaryStation1;
		}
		else if (ATS_LOC_TYPE_SECONDARY_STATION_2 == value)
		{
			m_agentAtsLocationType = SecondaryStation2;
		}
		else
		{
			m_agentAtsLocationType = Unknown;
		}

        return m_agentAtsLocationType;
	}

	unsigned long AtsAgentEntityData::getAtsTablesBaseAddress()
    {
		std::string value = getHelper()->getParameter(ATS_TABLES_BASE_ADDRESS);
		m_atsTablesBaseAddress = EntityHelper::getUnsignedLongData(value);

        return m_atsTablesBaseAddress;
    }

	unsigned long AtsAgentEntityData::getIscsTablesBaseAddress()
    {
		std::string value = getHelper()->getParameter(ISCS_TABLES_BASE_ADDRESS);
		m_iscsTablesBaseAddress = EntityHelper::getUnsignedLongData(value);

        return m_iscsTablesBaseAddress;
    }

	unsigned long AtsAgentEntityData::getMaxCommsRetries()
    {
		std::string value = getHelper()->getParameter(MAX_COMMS_RETRIES);
		m_maxCommsRetries = EntityHelper::getUnsignedLongData(value);

        return m_maxCommsRetries;
    }

	unsigned long AtsAgentEntityData::getMaxWatchdogInterval()
    {
		std::string value = getHelper()->getParameter(MAX_WATCHDOG_INTERVAL);
		m_maxWatchdogInterval = EntityHelper::getUnsignedLongData(value);

        return m_maxWatchdogInterval;
    }
	
	//td 13947
	//zhou yuan++
	//start the AtsTrainDataTimer for ATC table
	long AtsAgentEntityData::getAtcTimeout()
	{
		std::string value = getHelper()->getParameter(ATC_TIMEOUT);
		m_atcTimeout = EntityHelper::getIntegerData(value);

		return m_atcTimeout;
	}

	long AtsAgentEntityData::getOa1Timeout()
	{
		std::string value = getHelper()->getParameter(OA1_TIMEOUT);
		m_oa1Timeout = EntityHelper::getIntegerData(value);

		return m_oa1Timeout;		
	}
	//++zhou yuan

    TA_Base_Core::IEntityData* AtsAgentEntityData::clone(unsigned long key)
    {
        return( new AtsAgentEntityData(key) );
    }
	


	std::string AtsAgentEntityData::getTrainDirectionAddress ()
	{
		m_strTrainDirection = getHelper()->getParameter(DP_TRAIN_DIRECTION);

		return m_strTrainDirection;
	}

	std::string AtsAgentEntityData::getStopFlagAddress ()
	{
		m_strStopFlag = getHelper()->getParameter(DP_STOPFLAG);

		return m_strStopFlag;
	}

	std::string AtsAgentEntityData::getTrainServiceNumberAddress ()
	{
		m_strTrainServiceNumber = getHelper()->getParameter(DP_TRAINSERVICENUM);

		return m_strTrainServiceNumber;
	}

	unsigned long AtsAgentEntityData::getTimetblChkInterval()
	{
		std::string value= getHelper()->getParameter(TIMETBL_CHK_INTERVAL);
		m_timetblChkInterval = EntityHelper::getUnsignedLongData(value);

		return m_timetblChkInterval;
	}

	std::string AtsAgentEntityData::getTimeTableDirPath()
	{
		m_timetableDirPath = getHelper()->getParameter(TIMETBL_DIR_PATH);

		return m_timetableDirPath;
	}

	std::string AtsAgentEntityData::getTrainPisDpAddress()
	{
		m_strTrainPisAddrss = getHelper()->getParameter(DP_TRAIN_PIS);

		return m_strTrainPisAddrss;
	}

	std::string AtsAgentEntityData::getTrainTimeOutDpAddress()
	{
		m_strTrainTimeOutAddrss = getHelper()->getParameter(DP_STOPFLAG);

		return m_strTrainTimeOutAddrss;
	}

	std::string AtsAgentEntityData::getTrainDriveModeDpAddress()
	{
		m_strTrainDriveModeAddrss = getHelper()->getParameter(DP_TRAIN_DRIVE_MODE);

		return m_strTrainDriveModeAddrss;
	}

	std::string AtsAgentEntityData::getTrainPosLostDpAddress()
	{
		m_strTrainPosLostAddrss = getHelper()->getParameter(DP_TRAIN_POS_LOST);

		return m_strTrainPosLostAddrss;
	}
	 
	std::string AtsAgentEntityData::getBlockDpAddress()
	{
		m_strGenaBlkAddrss = getHelper()->getParameter(DP_BLK_STAT);

		return m_strGenaBlkAddrss;
	}

	std::string AtsAgentEntityData::getSwPositionDpAddress()
	{
		m_strSwitchPosAddr = getHelper()->getParameter(DP_SWITCH_POS);

		return m_strSwitchPosAddr;
	}

	std::string AtsAgentEntityData::getSwFailedStatDpAddress()
	{
		m_strSwitchFailAddr = getHelper()->getParameter(DP_SWITCH_FAULTSTAT);

		return m_strSwitchFailAddr;
	}

	std::string AtsAgentEntityData::getSwResStatDpAddress()
	{
		m_strSwitchResAddr = getHelper()->getParameter(DP_SWITCH_RESSTAT);

		return m_strSwitchResAddr;
	}

	std::string AtsAgentEntityData::getPlatformTrainListDpAddress()
	{
		m_strPisTrainListAddr = getHelper()->getParameter(DP_SIG_PLATFORM_ARRTIME);

		return m_strPisTrainListAddr;
	}

	std::string AtsAgentEntityData::getPlatformJumpDpAddress()
	{
		m_strPisJumpAddr = getHelper()->getParameter(DP_SIG_PLATFORM_JUMPSTAT);

		return m_strPisJumpAddr;
	}

	std::string AtsAgentEntityData::getPlatformHoldDpAddress()
	{
		m_strPisHoldAddr = getHelper()->getParameter(DP_SIG_PLATFORM_HOLDSTAT);

		return m_strPisHoldAddr;
	}

	std::string AtsAgentEntityData::getEsbStatusDpAddress()
	{
		m_strEsbStatAddress = getHelper()->getParameter(DP_SIG_ESB_STAT);

		return m_strEsbStatAddress;
	}

	std::string AtsAgentEntityData::getFloodgatePermissionDpAddress()
	{
		m_strFgPermAddress = getHelper()->getParameter(DP_SIG_FG_PERMISSIONCLOSE);

		return m_strFgPermAddress;
	}

	std::string AtsAgentEntityData::getFloodgateReqDpAddress()
	{
		m_strFgReqAddrss = getHelper()->getParameter(DP_SIG_FG_REQCLOSE);

		return m_strFgReqAddrss;
	}

	std::string AtsAgentEntityData::getSigDisplayDpAddress()
	{
		m_strSigDisplayAddress = getHelper()->getParameter(DP_SIG_DISPLAYSTAT);

		return m_strSigDisplayAddress;
	}

	std::string AtsAgentEntityData::getSigHealthDpAddress()
	{
		m_strSigHealthAddrss = getHelper()->getParameter(DP_SIG_HEALTHSTAT);

		return m_strSigHealthAddrss;
	}

	std::string AtsAgentEntityData::getPlatformDoorOverrideDpAddress()
	{
		m_strPdOverrideAddress = getHelper()->getParameter(DP_SIG_PD_OVERRIDED);

		return m_strPdOverrideAddress;
	}

	std::string AtsAgentEntityData::getPIDLocationArrTimeDpAddress()
	{
		m_strPIDArrTimeAddress = getHelper()->getParameter(DP_SIG_PLATFORM_ARRTIME);

		return m_strPIDArrTimeAddress;
	}

}