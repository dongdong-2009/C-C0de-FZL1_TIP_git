// This is the main DLL file.

#include "AlarmWrapperFactory.h"

namespace ISCSAlarmNetWrapper
{

	IExternalAlarmWrapper^ AlarmWrapperFactory::getExternalAlarmWrapper(unsigned long sourceID)
	{
		if(_IExternalAlarmWrapper == nullptr)
		{
			_IExternalAlarmWrapper = gcnew ExternalAlarmWrapper(sourceID);
		}
		return _IExternalAlarmWrapper;
		
	}
    void AlarmWrapperFactory::releaseExternalWrapper(unsigned long sourceID)
	{
		if(_IExternalAlarmWrapper != nullptr)
		{
			delete(_IExternalAlarmWrapper);
			_IExternalAlarmWrapper = nullptr;
		}
	}
		
}