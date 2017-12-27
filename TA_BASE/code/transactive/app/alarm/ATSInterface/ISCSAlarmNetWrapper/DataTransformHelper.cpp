#include "DataTransformHelper.h"
#include "../ShareMemoryDLL/AlarmAPI.h"
#include "IExternalAlarmWrapper.h"
#include <time.h>   

using namespace I_ALARM_PROVIDER;
using namespace System::Runtime::InteropServices;

namespace ISCSAlarmNetWrapper
{	
	DataTransformHelper::DataTransformHelper()
	{
	}

	void DataTransformHelper::MarshalString ( String ^ s, std::string& os ) 
	{	
		if(String::IsNullOrEmpty(s))
		{
			os = "";
			return;
		}
		const char* chars = 
			(const char*)(Marshal::StringToHGlobalAnsi(s)).ToPointer();
		
		
		os = chars;
		Marshal::FreeHGlobal(IntPtr((void*)chars));
	}
	
	void DataTransformHelper::ManagedAlarmDetailToAlarmDetail ( AlarmDetail^ _ManagedAlarmDetail, I_ALARM_PROVIDER::AlarmDetail* _AlarmDetail)
	{
		DateTimeToMillisecondTime(_ManagedAlarmDetail->sourceTime,_AlarmDetail->sourceTime);
		MarshalString(_ManagedAlarmDetail->alarmID,_AlarmDetail->alarmID);		  
		DateTimeToTime(_ManagedAlarmDetail->ackTime,_AlarmDetail->ackTime);    
		DateTimeToTime(_ManagedAlarmDetail->closeTime,_AlarmDetail->closeTime);  
		MarshalString(_ManagedAlarmDetail->assetName,_AlarmDetail->assetName);		
		_AlarmDetail->alarmSeverity = _ManagedAlarmDetail->alarmSeverity; 
		MarshalString(_ManagedAlarmDetail->alarmDescription,_AlarmDetail->alarmDescription);		
		MarshalString(_ManagedAlarmDetail->alarmAcknowledgeBy,_AlarmDetail->alarmAcknowledgeBy );
		_AlarmDetail->state = static_cast<I_ALARM_PROVIDER::AlarmState>(_ManagedAlarmDetail->state);    
		MarshalString(_ManagedAlarmDetail->locationId,_AlarmDetail->locationId  );
		MarshalString(_ManagedAlarmDetail->parentAlarmID,_AlarmDetail->parentAlarmID );
		MarshalString(_ManagedAlarmDetail->avalancheHeadID,_AlarmDetail->avalancheHeadID  );		
		_AlarmDetail->isHeadOfAvalanche = _ManagedAlarmDetail->isHeadOfAvalanche;     
		_AlarmDetail->isChildOfAvalanche = _ManagedAlarmDetail->isChildOfAvalanche;
		ManagedMssStateToMssState(_ManagedAlarmDetail->mmsState,_AlarmDetail->mmsState); 
		MarshalString(_ManagedAlarmDetail->alarmValue,_AlarmDetail->alarmValue );	
		_AlarmDetail->omAlarm = _ManagedAlarmDetail->omAlarm;  
		MarshalString(_ManagedAlarmDetail->alarmType,_AlarmDetail->alarmType);	
		MarshalString(_ManagedAlarmDetail->subsytemkey,_AlarmDetail->subsytemkey);
		MarshalString(_ManagedAlarmDetail->systemkey,_AlarmDetail->systemkey);
		MarshalString(_ManagedAlarmDetail->alarmComments,_AlarmDetail->alarmComments );
		_AlarmDetail->alarmSource = _ManagedAlarmDetail->alarmSource; 
		
	}

	

	void DataTransformHelper::MillisecondTimeToDateTime(I_ALARM_PROVIDER::MillisecondTime millisecondTime, DateTime% dateTime) 
	{
		TimeToDateTime(millisecondTime.time, dateTime);
		dateTime.AddMilliseconds( millisecondTime.millitm);

	}
	void DataTransformHelper::DateTimeToMillisecondTime(DateTime dateTime, I_ALARM_PROVIDER::MillisecondTime& millisecondTime)
	{	
		unsigned long time;
		DateTimeToTime(dateTime, time );
		millisecondTime.time = time;
		millisecondTime.millitm = dateTime.Millisecond;

	}

	void DataTransformHelper::DateTimeToTime(DateTime dateTime, unsigned long& time)
	{
		TimeSpan diff = dateTime.ToUniversalTime() - DateTime(1970, 1, 1);	
		time = static_cast<time_t>(diff.TotalSeconds);
		
	}

	void DataTransformHelper::TimeToDateTime(unsigned long time, DateTime% dateTime)
	{
		unsigned long sec = time;
		dateTime =  DateTime(1970, 1, 1, 0, 0, 0, DateTimeKind::Utc).AddSeconds(sec).ToLocalTime();
	}
	

	void DataTransformHelper::AlarmAlarmRequestToManagedAlarmRequest(I_ALARM_PROVIDER::AlarmRequest* _AlarmRequest, AlarmRequest^ _ManagedAlarmRequest)
	{
		_ManagedAlarmRequest->alarmId =  gcnew String( _AlarmRequest->alarmId.c_str() );  
		_ManagedAlarmRequest->operatorID =  gcnew String( _AlarmRequest->operatorID.c_str() );
		TimeToDateTime(_AlarmRequest->requestTime,_ManagedAlarmRequest->requestTime); 
		_ManagedAlarmRequest->alarmStatue = static_cast<ISCSAlarmNetWrapper::AlarmState>(_AlarmRequest->state);
	}

	
	void DataTransformHelper::ManagedMssStateToMssState( MmsStateType _ManagedMmsStateType, I_ALARM_PROVIDER::MmsStateType &_MmsStateType)
	{
		switch(_ManagedMmsStateType)
		{
		case ISCSAlarmNetWrapper::MmsStateType::MMS_NONE:
			_MmsStateType = I_ALARM_PROVIDER::MmsStateType::MMS_NONE;
			break;
		case ISCSAlarmNetWrapper::MmsStateType::MMS_SEMI:
			_MmsStateType = I_ALARM_PROVIDER::MmsStateType::MMS_SEMI;
			break;
		case ISCSAlarmNetWrapper::MmsStateType::MMS_AUTO:
			_MmsStateType = I_ALARM_PROVIDER::MmsStateType::MMS_AUTO;
			break;
		case ISCSAlarmNetWrapper::MmsStateType::MMS_JR_SENT:
			_MmsStateType = I_ALARM_PROVIDER::MmsStateType::MMS_JR_SENT;
			break;
		case ISCSAlarmNetWrapper::MmsStateType::MMS_INHIBIT:
			_MmsStateType = I_ALARM_PROVIDER::MmsStateType::MMS_INHIBIT;
			break;

		}
	}
}