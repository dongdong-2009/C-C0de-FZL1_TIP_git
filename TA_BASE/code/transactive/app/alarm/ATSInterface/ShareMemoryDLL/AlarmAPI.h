/**
* The source code in this file is the property of 
* ST Electronics and is not for redistribution
* in any form.
*
* Source:    
* @author:  Luo HuiRong
* @version: $Revision: #1.1.0.0 $
*
* This file provides the interface definition for alarm interface between C955 and C952
*  
*/



#ifndef ALARM_API_INCLUDE_
#define ALARM_API_INCLUDE_
#include <string>
#include <list>

// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. 
// The DLL project will NOT define ALARM_API_IMPORTS symbol, which will expose the dllexport
// directive.
// All application that use the DLL must define the ALARM_API_IMPORTS symbol, which will
// expose the dllimport directive.
#ifdef ALARM_API_IMPORTS
#define ALARM_API __declspec(dllimport)
#else
#define ALARM_API __declspec(dllexport)
#endif


namespace I_ALARM_PROVIDER
{
	
	//Represents a date and time
	struct MillisecondTime
	{
		time_t time;           // object of type time_t, where the time value is stored.
		unsigned long millitm; // value in milliseconds
		MillisecondTime(const MillisecondTime &millisecondTime)
		{
			time = millisecondTime.time;
			millitm = millisecondTime.millitm;
		}
		MillisecondTime()
		{
			time=0;
			millitm=0;
		}
	};
    
    //The AlarmState enum describes the state type of the the ATS alarm
	enum AlarmState
	{
		AlarmClosed,          // alarm is in Closed State
		AlarmOpen,           // alarm is in Open State
		AlarmReturned,         //alarm value has returned back to a safe range
		AlarmAcknowledged     // alarm is in acknowledged state, if the alarm is in this state, the alarmAcknowledgeBy field of AlarmDetail must not be null.
	};
    
	//The MmsStateType enum describes the MMS state type of the ATS alarm
	enum MmsStateType
	{
		MMS_NONE,            // the alarm is not required to forward to MMS
		MMS_SEMI,            // The alarm is forwarded to MMS upon the operator command
		MMS_AUTO,            // The alarm is forwarded to the MMS automatically
		MMS_JR_SENT,         // A job request is successfully forward to MMS
		MMS_ALM_SENT,        // The alarm is successfully forward to MMS
		MMS_INHIBIT,         // the MMS alarm is inhibit 
		MMS_FAILED           // failed to send the alarm to MMS 
	};

    //The AlarmDetail structure contains all relevant information pertaining to a single ATS alarm
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
		std::string alarmType;    // the unique code  of alarm type, max length 30 characters
		std::string subsytemkey;  // the unique code  of subsystem, max length 30 characters
		std::string systemkey;    // the unique code  of system	, max length 30 characters	
        std::string alarmComments;  // comments of alarm, max 200 characters.
		unsigned long alarmSource; // External Source ID
		AlarmDetail():alarmID(""),assetName(""),alarmDescription(""),
			alarmAcknowledgeBy(""),locationId(""),parentAlarmID(""),avalancheHeadID(""),alarmValue(""),alarmType(""),subsytemkey(""),systemkey(""),alarmComments("")
		{
			
		}
		AlarmDetail(const AlarmDetail& alarmDetail)
		{
			sourceTime = alarmDetail.sourceTime;
			alarmID = alarmDetail.alarmID.c_str();
			ackTime = alarmDetail.ackTime;
			closeTime = alarmDetail.closeTime;
			assetName = alarmDetail.assetName.c_str();
			alarmSeverity = alarmDetail.alarmSeverity;
			alarmDescription = alarmDetail.alarmDescription.c_str();
			alarmAcknowledgeBy = alarmDetail.alarmAcknowledgeBy.c_str();
			state = alarmDetail.state;
			locationId = alarmDetail.locationId.c_str();
			parentAlarmID = alarmDetail.parentAlarmID.c_str();
			avalancheHeadID = alarmDetail.avalancheHeadID.c_str();
			isHeadOfAvalanche = alarmDetail.isHeadOfAvalanche;
			isChildOfAvalanche = alarmDetail.isChildOfAvalanche;
			mmsState = alarmDetail.mmsState;
			alarmValue = alarmDetail.alarmValue.c_str();
			omAlarm = alarmDetail.omAlarm;
			alarmType = alarmDetail.alarmType.c_str();
			subsytemkey = alarmDetail.subsytemkey.c_str();
			systemkey = alarmDetail.systemkey.c_str();
			alarmComments = alarmDetail.alarmComments.c_str();
			alarmSource = alarmDetail.alarmSource;
		}

	};
    
	//The ProfileInfo structure contains all relevant ISCS profile information
	struct ProfileInfo
	{
		ProfileInfo():updateTime(0),operatorId(""),profileId(""),loginLocation("")
		{
		}
		ProfileInfo(const ProfileInfo &profileInfo)
		{
			updateTime = profileInfo.updateTime;
			operatorId = profileInfo.operatorId.c_str();
			profileId = profileInfo.profileId.c_str();
			loginLocation = profileInfo.loginLocation.c_str();
			displayOnly = profileInfo.displayOnly;
		}

		time_t updateTime;           // the timestamp when ISCS trigger login/logout/change profile actions 
		std::string operatorId;      // login operator ID, max 10 characters.
		std::string profileId;       // login profile ID, max 10 characters.
		std::string loginLocation;   // refer to Section D.5 Station Name Code, max 4 characters
		bool  displayOnly;           // 'true' means that operator can only view without any control; 'false' means that operator can view and control.
	 
	};
    
	//The ATSAlarmQueueItem structure contains all relevant alarm information related to a user profile.
	struct ExAlarmQueueItem
	{
		AlarmDetail detail;         //alarm detail information
		ProfileInfo profileInfo;    //user profile information
	};

    //The AlarmAckUpdate structure is used to pass all relevant information to acknowledge alarm 
	struct AlarmAckUpdate
	{
		std::string alarmId; 		     // max 64 characters.
		std::string alarmAcknowledgeBy;  // operator ID, max 10 characters.
		unsigned long ackTime;
	};

	struct AlarmRequest
	{
		std::string alarmId; 		// max 38 characters.
		std::string operatorID;  // operatorId, max 10 characters.
		unsigned long requestTime;
		AlarmState  state;
	};

	//The AlarmAckUpdate structure is used to pass all relevant information to raise alarm MMS Job
	struct MmsJobRequest
	{
		std::string AssetName;      // the asset name(equipment ID), max 40 characters
		std::string AlarmId;        // max 64 characters alarm ID
		std::string OperatorId;     // operatorId 
		unsigned long targetDate;   // 0 if no target completion date.
		std::string problemReport;  // problem reported
	};
    
	/*
	 * The IATSAlarm class provides interface contract to 
	 *    retrieve operator login profile information from C955
	 *    submit ATS alarm to C955
	 *    provides ISCS to know whether the ATS restart or not
	 *    retrieve alarm acknowledge request from C955
	 *    retrieve alarm MMS job request from C955
	*/
	class ALARM_API IAlarmProvider
	{
	public:

		~IAlarmProvider(){};
	    /**
		* submitATSAlarm
		*
		* This function call to submit an ATS alarm. The submitted alarm can be a new alarm or an alarm update. 
		*
		* The passed in parameter is copied to the share memory,so it can be deleted after the function call. 
		* The API does not delete this parameter. 
		* This function may throw exception QUEUE_OVERFLOW once the Input Queue length is exceeded. ATS should be able to handle this exception	 
		*
		* @param	AlarmDetail&	the alarm information
		* @param    ProfileInfo&    the current user profile information
		*
		* @exception QUEUE_OVERFLOW Thrown for one of the following reasons:
		*            1) the Input Queue length is exceeded 
		*/
		virtual void submitAlarm( const AlarmDetail& details, const ProfileInfo& profileInfo ) = 0;


		/**
		* submitATSAlarmList
		*
		* This function call to submit a list of ATS alarms. The submitted alarm can be a new alarm or an alarm update. 
		*
		* The passed in parameter is copied to the share memory,so it can be deleted after the function call. 
		* The API does not delete this parameter. 
		* This function may throw exception QUEUE_OVERFLOW once the Input Queue length is exceeded. ATS should be able to handle this exception	 
		*
		* @param	std::list<AlarmDetail*>	 a list of alarm information
		* @param    ProfileInfo&             the current user profile information
		*
		* @exception QUEUE_OVERFLOW Thrown for one of the following reasons:
		*            1) the Input Queue length is exceeded 
		*/
		virtual void submitAlarmList( std::list<AlarmDetail*> details, const ProfileInfo& profileInfo ) = 0;

		/**
		* getProfileInfo
		*
		* This function provide a mechanism to ATS to retrieve the current user profile information. 
		*
		* ATS uses this function call to retrieve the current login Profile information so as to filter the submitted alarms and events. 
		* The detail of the filtering would refer to the access control documents. 
		* The return value indicates whether the ISCS is started. If the return value is false, it means the ISCS is not started yet.
		* If any field of ProfileInfo structure had been changed, ATS needs to re-send all alarms and events to ISCS based on changed profiles. 
		* If profileId & operatorId field are empty, ISCS has not login yet.
        *
		* When ATS MFT application starts up, it continuously calls getProfileInfo() to check current ISCS login state. 
		* If there is valid profileId & operatorId (not empty), then ATS builds up the current active alarm list and passes the list to the Input Queue through
        *
		* @param    ProfileInfo&             the current user profile information
		*
		* @return	bool to indicates whether the ISCS is started. If the return value is false,otherwise the value is true.
	    *
		*/
		virtual bool getProfileInfo( ProfileInfo& profileInfo ) = 0;


		/**
		* retrieveATSAlarmAckUpdate
		*
		* This function call provides ATS to retrieve the ATS alarm acknowledge request from the Output Queue. 
		* The return value is the total number of the alarm acknowledge request passed out in the parameter. 
		* The memory of each element of the parameter is allocated in the API, and it is a copy of the data within the share memory.
		* The API user must delete them after extracting necessary information to avoid memory leak. 
		* The alarm acknowledge request in the list is sequential, so the processing order must from the front to the back. 
		* The return list would strictly follow the order in the queue from the front to the back.
		*
		* @param	std::list<AlarmAckUpdate*>&	 it is a input and output parameter,the list which includes alarm acknowledge requests from C955.	 
		*
		* @return	unsigned long to indicates the total number of the alarm acknowledge request passed out in the parameter.
		*             
		*/
		virtual unsigned long retrieveAlarmAckUpdate( std::list<AlarmAckUpdate*>& updates ) = 0;

		/**
		* retrieveATSAlarmRequestUpdate
		*
		* This function call provides ATS to retrieve the ATS alarm request to acknowledge or close from the Output Queue. 
		* The return value is the total number of the alarm request passed out in the parameter. 
		* The memory of each element of the parameter is allocated in the API, and it is a copy of the data within the share memory.
		* The API user must delete them after extracting necessary information to avoid memory leak. 
		* The alarm acknowledge request in the list is sequential, so the processing order must from the front to the back. 
		* The return list would strictly follow the order in the queue from the front to the back.
		*
		* @param	std::list<AlarmRequest*>&	 it is a input and output parameter,the list which includes alarm acknowledge requests from C955.	 
		*
		* @return	unsigned long to indicates the total number of the alarm acknowledge request passed out in the parameter.
		*             
		*/
		virtual unsigned long retrieveAlarmRequestUpdate( std::list<AlarmRequest*>& updates ) = 0;

 
		/**
		* retrieveATSMmsJobRequest
		*
		* This function call provides ATS to retrieve the ATS alarm MMS job request from the Output Queue. 
		* The return value is the total number of the alarm MMS job request passed out in the parameter. 
		* The memory of each element of the parameter is allocated in the API, and it is a copy of the data within the share memory.
		* The API user must delete them after extracting necessary information to avoid memory leak. 
		* The alarm MMS job request in the list is sequential, so the processing order must from the front to the back. 
		* The return list would strictly follow the order in the queue from the front to the back.	 
		*
		* @param	std::list<MmsJobRequest*>&	 it is a input and output parameter,the list which includes alarm MMS job requests from C955.
		*
		* @return	unsigned long to indicates the total number of the alarm MMS job request passed out in the parameter.
		*             
		*/
		virtual unsigned long retrieveMmsJobRequest( std::list<MmsJobRequest*>& updates ) = 0;

		/**
		* setUpdateTime
		*
		* This function call provides ATS to set the alarm update time. 
		* This function call provides ISCS to know whether the ATS restart or not. 
		*
		* After ATS startup, ATS should call this function to notify ISCS of ATS startup, ISCS will clear all ATS alarms, 
		* after that, ATS will resubmit all ATS alarms to ISCS. 
		*
		* ATS will also clear all the command buffers during its application start up
		*
		* @param	time_t&	 the time when ATS startup.
		*             
		*/
		virtual void setUpdateTime( time_t updateTime ) = 0;
	};

	/*
	* The IISCSAlarm class provides interface contract to 
	*    set operator login profile information
	*    retrieve ATS alarm information
	*    submit alarm acknowledge request to ATS
	*    submit alarm MMS job request to ATS
	*	 
	* ATS is not required to call this functions.
	*/
	class ALARM_API IISCSAlarm
	{
	public:
		~IISCSAlarm(){};

		/**
		* retrieveATSAlarm
		*
		* This function call retrieves ATS alarms from input queue. 	 	 
		*
		* @param	std::list<ExAlarmQueueItem*>&	 a return list of ATS alarms.
		*             
		*/
		virtual unsigned long retrieveAlarm( std::list<ExAlarmQueueItem*>& details ) = 0;

		/**
		* setProfileInfo
		*
		* This function call sets operator profile information.   		 
		*
		* @param	ProfileInfo&	 operator profile.
		*             
		*/
		virtual void setProfileInfo( const ProfileInfo& profileInfo ) = 0;


		/**
		* submitATSAlarmAckUpdate
		*
		* This function call submits a ATS alarm acknowledge request.		 
		*
		* @param	AlarmAckUpdate&	 the ATS alarm acknowledge request
		*             
		*/
		virtual void submitAlarmAckUpdate( const AlarmAckUpdate& update ) = 0; //May catch 


		/**
		* submitAlarmRequest
		*
		* This function call submits a alarm request to acknowledge or close.		 
		*
		* @param	AlarmRequest&	 the alarm acknowledge/close request
		*             
		*/
		virtual void submitAlarmRequest( const AlarmRequest& update ) = 0; //May catch 

		/**
		* submitATSMmsJobRequest
		*
		* This function call submits a ATS alarm MMS job request.		 
		*
		* @param	MmsJobRequest&	 the ATS alarm MMS job request
		*             
		*/
		virtual void submitMmsJobRequest( const MmsJobRequest& update ) = 0;
	};
}


	/**
	* getAlarmProvider
	*
	* It is a factory method to return a singleton of IATSAlarm 
	*
	* @return	IATSAlarm*	 a singleton of IATSAlarm 
	*             
	*/
	ALARM_API I_ALARM_PROVIDER::IAlarmProvider* getAlarmProvider(unsigned long ulAlarmProviderID);


	/**
	* freeAlarmProvider
	*
	* It is a factory method to return a singleton of IISCSAlarm 
	*
	* @return	IISCSAlarm*	 a singleton of IISCSAlarm 
	*             
	*/
	ALARM_API void freeAlarmProvider(unsigned long ulAlarmProviderID);

	/**
	* getISCSInterface
	*
	* It is a factory method to return a singleton of IISCSAlarm 
	*
	* @return	IISCSAlarm*	 a singleton of IISCSAlarm 
	*             
	*/
	ALARM_API I_ALARM_PROVIDER::IISCSAlarm* getISCSInterface();

#endif