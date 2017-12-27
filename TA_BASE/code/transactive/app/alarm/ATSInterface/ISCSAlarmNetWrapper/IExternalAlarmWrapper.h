#pragma once
#include "../ShareMemoryDLL/AlarmAPI.h"
using namespace System;
using namespace System::Collections::Generic;

using namespace I_ALARM_PROVIDER;
namespace ISCSAlarmNetWrapper {

	public enum class MmsStateType
	{
		MMS_NONE,            // the alarm is not required to forward to MMS
		MMS_SEMI,            // The alarm is forwarded to MMS upon the operator command
		MMS_AUTO,            // The alarm is forwarded to the MMS automatically
		MMS_JR_SENT,         // A job request is successfully forward to MMS	 
		MMS_INHIBIT,         // the MMS alarm is inhibit 	 
	};
	
	public enum class AlarmState
	{
		AlarmClosed, // alarm is in Closed State
		AlarmOpen,  // alarm is in Open State
		AlarmReturned,  //alarm value has returned back to a safe range
		AlarmAcknowledged, //alarm is in acknowledged state, if the alarm is in this state, the alarmAcknowledgeBy field of AlarmDetail must not be null.

	};

	

	//The AlarmDetail structure contains all relevant information pertaining to a single ATS alarm
	public ref class AlarmDetail
	{
	public:
		DateTime sourceTime;  // The alarm generated time
		String^ alarmID;           // An unique alarm id to differentiate each alarm generated, max 38 characters.
		DateTime ackTime;       // the acknowledge time if any
		DateTime closeTime;     // the close time if any
		String^ assetName;       // the asset name(equipment ID), max 40 characters.
		unsigned long alarmSeverity;  // the severity of the alarm, 1(urgent),2(Critical) 3(Major),4(Minor)
		String^ alarmDescription;// the description of the alarm, max 200 characters.
		String^ alarmAcknowledgeBy; // the operator who acknowledge this alarm, max 10 characters
		AlarmState state;           // the alarm state
		String^ locationId;     //  // the location (ISCS will provide location and location name) that alarm is generated, max 6 characters.
		String^ parentAlarmID;  // the parent alarm ID, empty string if noapplicable.
		String^ avalancheHeadID;// related to the avalanche head if applicable, empty string if noapplicable.
		bool isHeadOfAvalanche;     // whether the alarm is the head of avalanche
		bool isChildOfAvalanche;    // whether the alarm is the child of avalanche
		MmsStateType mmsState;      // the MMS state
		String^ alarmValue;     // the value of the data point related to the alarm, max 16 characters.
		char omAlarm;                // ‘O’ for Operation Alarm, ‘M’ for maintenance alarm, or ‘B’ for both, or empty string
		String^ alarmType;      // the unique code  of alarm type, max length 30 characters
		String^ subsytemkey;  // the unique code  of subsystem, max length 30 characters
		String^ systemkey;    // the unique code  of system	, max length 30 characters	
		String^ alarmComments;  // comments of alarm, max 200 charactrs.
		unsigned long alarmSource; // External Source ID
		AlarmDetail():alarmID(""),assetName(""),alarmDescription(""),
			alarmAcknowledgeBy(""),locationId(""),parentAlarmID(""),avalancheHeadID(""),alarmValue(""),alarmType(""),subsytemkey(""),systemkey(""),alarmComments("")
		{

		}

		AlarmDetail(const AlarmDetail^ alarmDetail)
		{
			sourceTime = alarmDetail->sourceTime;
			alarmID = alarmDetail->alarmID;
			ackTime = alarmDetail->ackTime;
			closeTime = alarmDetail->closeTime;
			assetName = alarmDetail->assetName;
			alarmSeverity = alarmDetail->alarmSeverity;
			alarmDescription = alarmDetail->alarmDescription;
			alarmAcknowledgeBy = alarmDetail->alarmAcknowledgeBy;
			state = alarmDetail->state;
			locationId = alarmDetail->locationId;
			parentAlarmID = alarmDetail->parentAlarmID;
			avalancheHeadID = alarmDetail->avalancheHeadID;
			isHeadOfAvalanche = alarmDetail->isHeadOfAvalanche;
			isChildOfAvalanche = alarmDetail->isChildOfAvalanche;
			mmsState = alarmDetail->mmsState;
			alarmValue = alarmDetail->alarmValue;
			omAlarm = alarmDetail->omAlarm;
			alarmType = alarmDetail->alarmType;
			subsytemkey = alarmDetail->subsytemkey;
			systemkey = alarmDetail->systemkey;
			alarmComments = alarmDetail->alarmComments;
			alarmSource = alarmDetail->alarmSource;
		}		

	};

		//The AlarmRequest structure is used to pass all relevant information to acknowledge/close alarm 
		public value struct AlarmRequest
		{
			String^ alarmId; 		     // max 38 characters.
			String^ operatorID;  // operator ID, max 10 characters.
			DateTime requestTime; 
			AlarmState  alarmStatue;         //AlarmClosed or AlarmAcknowledged  
		};
		
	public interface class IExternalAlarmWrapper
	{
	public:
       void submitAlarmList( List<AlarmDetail^>^ details );   
	   unsigned long retrieveAlarmAckCloseUpdate( List<AlarmRequest>^ updates );
       bool getISCSStartupTime( DateTime% startTime ) ;
	   void setExternalStartupTime( DateTime startTime );

	
	};



}