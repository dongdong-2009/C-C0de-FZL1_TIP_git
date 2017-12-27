#include "ExternalAlarmWrapper.h"


using namespace  I_ALARM_PROVIDER;
using namespace System::Runtime::InteropServices;
using namespace STEE::ISCS::Log;

namespace ISCSAlarmNetWrapper
{
	ExternalAlarmWrapper::ExternalAlarmWrapper(unsigned long sourceID)
	{
		m_AtsAlarm = getAlarmProvider(sourceID);
		m_SourceID = sourceID;

		CLASS_NAME = "ExternalAlarmWrapper";
		STEE::ISCS::Log::LogHelper::setLogFile( "../logs/ISCSAlarmNetWrapper.log");
		
	}
	ExternalAlarmWrapper::~ExternalAlarmWrapper()
	{
		delete m_AtsAlarm;
		m_AtsAlarm = NULL;
	}
	
	void ExternalAlarmWrapper::submitAlarmList(List<AlarmDetail^>^ details)
	{	
		System::String^ Function_Name = "submitAlarmList";
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Entered");
		
		std::list<I_ALARM_PROVIDER::AlarmDetail*> *_listAlarmDetail = new std::list<I_ALARM_PROVIDER::AlarmDetail*> () ;
		std::list<I_ALARM_PROVIDER::AlarmDetail*>::iterator iter;

		for(int i = 0; i < details->Count; i++)
		{
			I_ALARM_PROVIDER::AlarmDetail * _alarmDetail = new I_ALARM_PROVIDER::AlarmDetail();
			
			LogHelper::Trace(CLASS_NAME, Function_Name, System::String::Format("Alarm ID: {0},Value: {1}, State: {2}, Acknowledge: {3}, Description: {4}, Createtime: {5:d/M/yyyy HH:mm:ss}, Closetime: {6:d/M/yyyy HH:mm:ss}, Acktime: {7:d/M/yyyy HH:mm:ss}",
				details[i]->alarmID, details[i]->alarmValue, details[i]->state, "",details[i]->alarmDescription, details[i]->sourceTime, details[i]->closeTime, details[i]->ackTime ));


		    //set Alarm Source ID
			details[i]->alarmSource = m_SourceID;
			DataTransformHelper::ManagedAlarmDetailToAlarmDetail(details[i],_alarmDetail);
			_listAlarmDetail->push_back(_alarmDetail);
		}
		const ProfileInfo profileInfo;
		
		try
		{
			m_AtsAlarm->submitAlarmList(*_listAlarmDetail,profileInfo);
		}
		catch (char const* e)
		{
			String ^ _Exception = Marshal::PtrToStringAnsi((IntPtr) (char *) e);	

			//free memory
			for(iter = _listAlarmDetail->begin(); iter != _listAlarmDetail->end(); iter++)
			{
				delete (*iter);
				*iter = NULL;
			}
			delete(_listAlarmDetail);
			_listAlarmDetail = NULL;

			throw gcnew System::Exception(_Exception);
		}

		//free memory
		for(iter = _listAlarmDetail->begin(); iter != _listAlarmDetail->end(); iter++)
		{
			delete (*iter);
			*iter = NULL;
		}
		delete(_listAlarmDetail);
		_listAlarmDetail = NULL;
		
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Exited");
		return ;
	}

	unsigned long ExternalAlarmWrapper::retrieveAlarmAckCloseUpdate( List<AlarmRequest>^ updates )
	{
		System::String^ Function_Name = "retrieveAlarmAckCloseUpdate";
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Entered");

		std::list<I_ALARM_PROVIDER::AlarmRequest*> _listAlarmRequest ;
		
		unsigned long noAlarmAckRequest = m_AtsAlarm->retrieveAlarmRequestUpdate(_listAlarmRequest);
		

		std::list<I_ALARM_PROVIDER::AlarmRequest*>::iterator iter;
		for(iter = _listAlarmRequest.begin(); iter != _listAlarmRequest.end(); iter++)
		{
			AlarmRequest ^_managedAlarmRequest = gcnew AlarmRequest();
			DataTransformHelper::AlarmAlarmRequestToManagedAlarmRequest(*iter,_managedAlarmRequest);
			LogHelper::Trace(CLASS_NAME, Function_Name, System::String::Format("Alarm ID: {0},Operator Id: {1}, Request time: {2:d/M/yyyy HH:mm:ss}, AlarmState: {3}",
				_managedAlarmRequest->alarmId,_managedAlarmRequest->operatorID,_managedAlarmRequest->requestTime, _managedAlarmRequest->alarmStatue));

			updates->Add(*_managedAlarmRequest);

		}

		//free memory
		for(iter = _listAlarmRequest.begin(); iter != _listAlarmRequest.end(); iter++)
		{
			delete (*iter);
			*iter = NULL;
		}
		
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Exited");
		return noAlarmAckRequest;
	}

	bool ExternalAlarmWrapper::getISCSStartupTime( DateTime% startTime ) 
	{	
		System::String^ Function_Name = "getISCSStartupTime";
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Entered");
		ProfileInfo _profileInfo;
		bool IsISCSStarted = m_AtsAlarm->getProfileInfo(_profileInfo);
		
		I_ALARM_PROVIDER::MillisecondTime _ISCSStartUpTime;
		_ISCSStartUpTime.time = _profileInfo.updateTime;
		
		DataTransformHelper::MillisecondTimeToDateTime(_ISCSStartUpTime ,startTime );
		LogHelper::Trace(CLASS_NAME, Function_Name, System::String::Format("ISCS Startup Time {0:d/M/yyyy HH:mm:ss}",startTime ));
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Exited");
		return IsISCSStarted;
	}

	void ExternalAlarmWrapper::setExternalStartupTime( DateTime startTime )
	{	
		System::String^ Function_Name = "setExternalStartupTime";
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Entered");
		LogHelper::Trace(CLASS_NAME, Function_Name, System::String::Format("External StartUp Time {0:d/M/yyyy HH:mm:ss}",startTime ));

		I_ALARM_PROVIDER::MillisecondTime _milliSec;
		DataTransformHelper::DateTimeToMillisecondTime(startTime,_milliSec);
		m_AtsAlarm->setUpdateTime(_milliSec.time);
		
		LogHelper::Debug(CLASS_NAME, Function_Name, "Function Exited");
		return;
	}


}