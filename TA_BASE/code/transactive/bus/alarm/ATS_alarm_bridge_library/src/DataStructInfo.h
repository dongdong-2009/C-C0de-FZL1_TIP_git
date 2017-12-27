#ifndef __DATA_STRUCT_INFO_H_
#define __DATA_STRUCT_INFO_H_
 
 namespace I_ALARM_PROVIDER
{
	struct MillisecondTime
	{
		unsigned long time;
		unsigned long millitm;
	};
	
	enum AlarmState
	{
		AlarmClosed,          // alarm is in Closed State
		AlarmOpen,           // alarm is in Open State
		AlarmReturned,         //alarm value has returned back to a safe range
		AlarmAcknowledged     // alarm is in acknowledged state, if the alarm is in this state, the alarmAcknowledgeBy field of AlarmDetail must not be null.
	};	
	
	enum AlarmCommandType
	{
		ACKCommand,
		MMSJobCommand
	};
	
	enum MmsStateType
	{
		MMS_NONE,
		MMS_SEMI,
		MMS_AUTO,
		MMS_JR_SENT,
		MMS_ALM_SENT,
		MMS_INHIBIT,
		MMS_FAILED
	};
	
	struct AlarmDetail
	{
		MillisecondTime sourceTime;  // The alarm generated time
		std::string alarmID;         // An unique alarm id to differentiate each alarm generated, max 64 characters.
		unsigned long ackTime;       // the acknowledge time if any
		unsigned long closeTime;     // the close time if any
		std::string assetName;       // the asset name(equipment ID), max 40 characters.
		unsigned long alarmSeverity; // the severity of the alarm
		std::string alarmDescription;// the description of the alarm, max 200 characters.
		std::string alarmAcknowledgeBy; // the operator who acknowledge this alarm, max 10 characters
		AlarmState state;           // the alarm state
		std::string locationId;     // the location (Sect D.5 Station Code Name) that alarm is generated, max 6 characters.
		std::string parentAlarmID;  // the parent alarm ID
		std::string avalancheHeadID;// related to the avalanche head if applicable
		bool isHeadOfAvalanche;     // whether the alarm is the head of avalanche
		bool isChildOfAvalanche;    // whether the alarm is the child of avalanche
		MmsStateType mmsState;      // the MMS state
		std::string alarmValue;     // the value of the data point related to the alarm, max 16 characters.
		char omAlarm;               // ¡®O¡¯ for Operation Alarm, ¡®M¡¯ for maintenance alarm, or ¡®B¡¯ for both.
		unsigned long alarmType;    // the PKey of alarm type
		unsigned long subsytemkey;  // the PKey of subsystem
		unsigned long systemkey;    // the PKey of system		
		std::string alarmComments;  // comments of alarm, max 200 charactrs.
		std::string strAlarmType;   // the PKey of alarm type
		std::string subsytemType;	// the PKey of subsystem
		std::string systemkeyType;  // the PKey of system	
		unsigned long locationKey;  // location key // Added by Noel 
		unsigned long alarmSource;  // to identify the alarm supplier = 
		AlarmDetail():alarmID(""),assetName(""),alarmDescription(""),
			alarmAcknowledgeBy(""),locationId(""),parentAlarmID(""),avalancheHeadID(""),alarmValue(""),alarmComments(""),strAlarmType(""),subsytemType(""),systemkeyType("")
		{
			
		}
		
	};
    
	
	struct ProfileInfo
	{
		ProfileInfo():updateTime(0),operatorId(""),profileId(""),loginLocation("")
		{
		}
		
		time_t updateTime;           // the timestamp when ISCS trigger login/logout/change profile actions 
		std::string operatorId;      // login operator ID, max 10 characters.
		std::string profileId;       // login profile ID, max 10 characters.
		std::string loginLocation;   // refer to Section D.5 Station Name Code, max 4 characters
		bool  displayOnly;           // 'true' means that operator can only view without any control; 'false' means that operator can view and control.
		
	};
	
	struct ExAlarmQueueItem
	{
		AlarmDetail detail;
		ProfileInfo profileInfo;
	};


	struct AlarmACKCommand
	{
		std::string alarmId; 		// max 38 characters.
	    std::string alarmAcknowledgeBy;  // operatorId, max 10 characters.
	    unsigned long ackTime;
		bool close;
	};

	struct AlarmRequest
	{
		std::string alarmId; 		// max 38 characters.
		std::string operatorID;  // operatorId, max 10 characters.
		unsigned long requestTime;
	    AlarmState  state;
	};

	struct AlarmMmsJobCommand
	{
		std::string assetName;
		std::string alarmId;
		std::string operatorId; // ¡°Requestor¡±
		unsigned long targetDate; // 0 if no target completion date.
		std::string problemReport;
	};


	//for ISCS notify  profile of ISCS to ATS.
	struct ExtProfileInfo
	{
		ExtProfileInfo():updateTime(0),operatorId(""),profileId(""),loginLocation(""),ID(1),displayOnly(false)
		{
		}
		unsigned long ID;
		time_t updateTime; // the timestamp when ISCS trigger login/logout/change profile actions 
		std::string operatorId; // login operatorId max 10 characters.
		std::string profileId; // login operatorId max 10 characters.
		std::string loginLocation; // Refer to Section D.5 Station Name Code, max 4 characters
		bool displayOnly;
	};

	//for ATS notify updateTime to ISCS
	struct ISCSProfileInfo
	{
		ISCSProfileInfo():updateTime(0),operatorId(""),profileId(""),loginLocation(""),ID(1),displayOnly(false)
		{
		}
		unsigned long ID;
		time_t updateTime; // the timestamp when ISCS trigger login/logout/change profile actions 
		std::string operatorId; // login operatorId max 10 characters.
		std::string profileId; // login operatorId max 10 characters.
		std::string loginLocation; // Refer to Section D.5 Station Name Code, max 6 characters
		bool displayOnly;
	};

	const int ALARM_ID_SIZE = 64;
	const int ASSERT_NAME_SIZE = 40;
	const int ALARM_DESCRIPTION_SIZE = 200;
	const int ALARM_ACKNOWLEDGE_BY_SIZE = 10;
	const int LOCATION_ID_SIZE = 6;
	const int ALARM_VALUE_SIZE = 16;
	const int OPERATOR_ID_SIZE = 10;
	const int PROFILE_ID_SIZE = 10;
	const int LOGIN_LOCATION_SIZE = 6;
	const int OPERATION_MODE_SIZE=1;
	const int PROBLEM_REPORT_SIZE=200;
	const int ALARM_TYPE_SIZE=100;
	const int SUBSYSTEM_KEY_SIZE=100;
	const int SYSTEM_KEY_SIZE=100;
}



// const char *SHARE_MEM_MUTEX = "share mem mutex";
// const char *ISCS_QUEUE_MUTEX = "iscs queue mutex";
// const char *ATS_MMS_QUEUE_MUTEX = "ats mms queue mutex";
// const char *ATS_ACK_QUEUE_MUTEX = "ats ack queue mutex";

// #define SHARE_MEM_MUTEX "share mem mutex"
// #define ISCS_QUEUE_MUTEX "iscs queue mutex"
// #define ATS_MMS_QUEUE_MUTEX "ats mms queue mutex"
// #define ATS_ACK_QUEUE_MUTEX "ats ack queue mutex"



#endif //__DATA_STRUCT_INFO_H_