/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/core/data_access_interface/entity_access/src/AtsAgentEntityData.h $
  * @author Ross Tucker
  * @version $Revision: #10 $
  *
  * Last modification: $DateTime: 2017/09/07 14:59:07 $
  * Last modified by: $Author: jiayi.zhang $
  * 
  * Implement the IEntityData interface.
  */

#ifndef ATS_AGENT_ENTITY_DATA
#define ATS_AGENT_ENTITY_DATA

#include "core/data_access_interface/entity_access/src/EntityData.h"

namespace TA_Base_Core
{
    class AtsAgentEntityData : public EntityData
    {

    public:
        AtsAgentEntityData();
        AtsAgentEntityData( unsigned long key );
        virtual ~AtsAgentEntityData();
        virtual void invalidate();
        virtual std::string getType();
		static std::string getStaticType();
        static IEntityData* clone(unsigned long key);

		enum EAtsLocationType
		{
			Occ,
			Depot,
			MainStation,
			SecondaryStation1,
			SecondaryStation2,
			Unknown
		};
			
		virtual std::string getAtsServer1IpAddress();
		virtual std::string getAtsServer1ServicePort();
		virtual std::string getAtsServer2IpAddress();
		virtual std::string getAtsServer2ServicePort();
		virtual std::string getAssetName();
		virtual EAtsLocationType getAgentAtsLocationType();
		virtual unsigned long getAtsTablesBaseAddress();
		virtual unsigned long getIscsTablesBaseAddress();
		virtual unsigned long getMaxCommsRetries();
		virtual unsigned long getMaxWatchdogInterval();
		virtual unsigned long getTimetblChkInterval();

		virtual std::string getStopFlagAddress ();
		virtual std::string getTimeTableDirPath();
		virtual std::string getTrainPisDpAddress();
		//Train
		virtual std::string getTrainDirectionAddress ();
		virtual std::string getTrainServiceNumberAddress ();
		virtual std::string getTrainTimeOutDpAddress();
		virtual std::string getTrainDriveModeDpAddress();
		virtual std::string getTrainPosLostDpAddress();
		//Block
		virtual std::string getBlockDpAddress ();
		//Switch
		virtual std::string getSwPositionDpAddress();
		virtual std::string getSwFailedStatDpAddress();
		virtual std::string getSwResStatDpAddress();
		//Platform
		virtual std::string getPlatformTrainListDpAddress();
		virtual std::string getPlatformJumpDpAddress();
		virtual std::string getPlatformHoldDpAddress();
		//Emergency Stop Button
		virtual std::string getEsbStatusDpAddress ();
		//Flood Gate
		virtual std::string getFloodgatePermissionDpAddress ();
		virtual std::string getFloodgateReqDpAddress();
		//Signal
		virtual std::string getSigDisplayDpAddress();
		virtual std::string getSigHealthDpAddress();
		//PlatformDoor
		virtual std::string getPlatformDoorOverrideDpAddress();
		//PIDLocation
		virtual std::string getPIDLocationArrTimeDpAddress();
	
		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table	
		virtual long getOa1Timeout();
		virtual long getAtcTimeout();
		//++zhou yuan		

    protected:

    private:
        //
        // Copy constructor and assignment operator are not used
        //
        AtsAgentEntityData( const AtsAgentEntityData& theAtsPsdEntityData) {};            
		AtsAgentEntityData& operator=(const AtsAgentEntityData &) {};

        // 
        // The entity type is fixed for all instances of this class.
        // 
        static const std::string ENTITY_TYPE;
		static const std::string ATS1_IP_ADDRESS;
		static const std::string ATS1_SERVICE_PORT;
		static const std::string ATS2_IP_ADDRESS;
		static const std::string ATS2_SERVICE_PORT;
		static const std::string AGENT_ATS_LOCATION_TYPE;
		static const std::string ATS_TABLES_BASE_ADDRESS;
		static const std::string ISCS_TABLES_BASE_ADDRESS;
		static const std::string MAX_COMMS_RETRIES;
		static const std::string MAX_WATCHDOG_INTERVAL;
		static const std::string TIMETBL_CHK_INTERVAL;
		static const std::string TIMETBL_DIR_PATH;

		static const std::string ATS_LOC_TYPE_OCC;
		static const std::string ATS_LOC_TYPE_DEPOT;
		static const std::string ATS_LOC_TYPE_MAIN_STATION;		
		static const std::string ATS_LOC_TYPE_SECONDARY_STATION_1;
		static const std::string ATS_LOC_TYPE_SECONDARY_STATION_2;

		static const std::string OA1_TIMEOUT;
		static const std::string ATC_TIMEOUT;

		static const std::string ASSET_NAME;

		static const std::string DP_STOPFLAG;	
		static const std::string DP_TRAIN_PIS;

		//Train
		static const std::string DP_TRAIN_DIRECTION;
		static const std::string DP_TRAINSERVICENUM;
		static const std::string DP_TRAIN_DRIVE_MODE;
		static const std::string DP_TRAIN_POS_LOST;
		//Block
		static const std::string DP_BLK_STAT;
		//Switch
		static const std::string DP_SWITCH_POS;
		static const std::string DP_SWITCH_FAULTSTAT;
		static const std::string DP_SWITCH_RESSTAT;
		//Signal
		static const std::string DP_SIG_DISPLAYSTAT;
		static const std::string DP_SIG_HEALTHSTAT;
		//Platform
		static const std::string DP_SIG_PLATFORM_ARRTIME;
		static const std::string DP_SIG_PLATFORM_JUMPSTAT;
		static const std::string DP_SIG_PLATFORM_HOLDSTAT;		
		//Emergency Stop Button
		static const std::string DP_SIG_ESB_STAT;
		//Flood Gate
		static const std::string DP_SIG_FG_REQCLOSE;
		static const std::string DP_SIG_FG_PERMISSIONCLOSE;
		//Platform Door
		static const std::string DP_SIG_PD_OVERRIDED;

		bool m_isValidData;

		std::string m_atsServer1IpAddress;
        std::string m_atsServer1ServicePort;
		std::string m_atsServer2IpAddress;
		std::string m_atsServer2ServicePort;
		std::string m_assetName;
		EAtsLocationType m_agentAtsLocationType;
		unsigned long m_atsTablesBaseAddress;
		unsigned long m_iscsTablesBaseAddress;
		unsigned long m_maxCommsRetries;
		unsigned long m_maxWatchdogInterval;
		unsigned long m_timetblChkInterval;
		
		std::string m_strStopFlag;
		std::string m_timetableDirPath;
		std::string m_strTrainPisAddrss;

		//Gena Data Point Address
		//Train
		std::string m_strTrainServiceNumber;
		std::string m_strTrainDirection;
		std::string m_strTrainTimeOutAddrss;
		std::string m_strTrainDriveModeAddrss;
		std::string m_strTrainPosLostAddrss;
		//Block
		std::string m_strGenaBlkAddrss;
		//Switch
		std::string m_strSwitchPosAddr;
		std::string m_strSwitchFailAddr;
		std::string m_strSwitchResAddr;
		//Platform
		std::string m_strPisTrainListAddr;
		std::string m_strPisJumpAddr;
		std::string m_strPisHoldAddr;
		//Emergency Stop Button
		std::string m_strEsbStatAddress;
		//Flood gate
		std::string m_strFgPermAddress;
		std::string m_strFgReqAddrss;
		//Signal
		std::string m_strSigDisplayAddress;
		std::string m_strSigHealthAddrss;
		//PlatformDoor
		std::string m_strPdOverrideAddress;
		//PIDLocation
		std::string m_strPIDArrTimeAddress;

		//td 13947
		//zhou yuan++
		//start the AtsTrainDataTimer for ATC table	
		long		 m_oa1Timeout;
		long		 m_atcTimeout;
		//++zhou yuan
    };

    typedef boost::shared_ptr<AtsAgentEntityData> AtsAgentEntityDataPtr;
}

#endif // ATS_AGENT_ENTITY_DATA
