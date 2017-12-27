#include "IExternalAlarmWrapper.h"
#include "../ShareMemoryDLL/AlarmAPI.h"
#include "DataTransformHelper.h"
#using <STEE.ISCS.Log.dll>

using namespace System;
using namespace  I_ALARM_PROVIDER;


namespace ISCSAlarmNetWrapper
{
	private ref class ExternalAlarmWrapper: IExternalAlarmWrapper
	{
	public:
		ExternalAlarmWrapper(unsigned long sourceID);
		~ExternalAlarmWrapper();
		virtual void submitAlarmList( List<AlarmDetail^>^ details);
		virtual unsigned long retrieveAlarmAckCloseUpdate( List<AlarmRequest>^ updates );
		virtual bool getISCSStartupTime( DateTime% startTime ) ;
		virtual void setExternalStartupTime( DateTime startTime );
	private:
		IAlarmProvider* m_AtsAlarm;
		long m_SourceID;
		System::String^ CLASS_NAME ;
		
	};
}
