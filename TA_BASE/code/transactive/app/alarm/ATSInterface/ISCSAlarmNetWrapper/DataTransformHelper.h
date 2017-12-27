#include "../ShareMemoryDLL/AlarmAPI.h"
#include "IExternalAlarmWrapper.h"

using namespace I_ALARM_PROVIDER;

namespace ISCSAlarmNetWrapper
{
     private ref class DataTransformHelper
	{
	public :
		/**
		* MarshalString
		*
		* Convert .Net String to C++ stirng
		*/
		static void MarshalString ( String ^ s, std::string& os );

		/**
		* ManagedAlarmDetailToAlarmDetail
		*
		* Convert managed AlarmDetail object to C++ I_ALARM_PROVIDER::AlarmDetail object
		*/
		static void ManagedAlarmDetailToAlarmDetail( AlarmDetail^ _ManagedAlarmDetail, I_ALARM_PROVIDER::AlarmDetail* _AlarmDetail);
		
		/**
		* MarshalString
		*
		* Convert I_ALARM_PROVIDER::AlarmRequest object to managed AlarmRequest object
		*/
		static void AlarmAlarmRequestToManagedAlarmRequest(I_ALARM_PROVIDER::AlarmRequest* _AlarmRequest, AlarmRequest^ _ManagedAlarmRequest);
		
		/**
		* MarshalString
		*
		* Convert managed MssState to _ALARM_PROVIDER::MmsStateType
		*/
		static void ManagedMssStateToMssState( MmsStateType _ManagedMmsStateType, I_ALARM_PROVIDER::MmsStateType &_MmsStateType);
		
		/**
		* MarshalString
		*
		* Convert .Net DateTime to I_ALARM_PROVIDER::MillisecondTime
		*/
		static void DateTimeToMillisecondTime(DateTime date, I_ALARM_PROVIDER::MillisecondTime & millisecondTime);
		
		/**
		* MarshalString
		*
		* Convert I_ALARM_PROVIDER::MillisecondTime String to .Net DateTime
		*/
		static void MillisecondTimeToDateTime(I_ALARM_PROVIDER::MillisecondTime millisecondTime, DateTime% dateTime);
		
		/**
		* MarshalString
		*
		* Convert .Net DateTime to unsigned long
		*/
		static void DateTimeToTime(DateTime dateTime, unsigned long & time);
		/**
		* MarshalString
		*
		* Convert unsigned long to .Net DateTime
		*/
		static void TimeToDateTime(unsigned long time, DateTime% dateTime);

	private:
		DataTransformHelper();
		static  DataTransformHelper^  m_instance = nullptr;
	};
}