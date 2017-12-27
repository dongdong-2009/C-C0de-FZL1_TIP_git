// SharedMemoryDLLWrapper.h

#pragma once

#include "IExternalAlarmWrapper.h";
#include "ExternalAlarmWrapper.h";
using namespace System;


namespace ISCSAlarmNetWrapper {

	public ref class AlarmWrapperFactory
	{
	public:
		/** 
		* It is a factory method to return a singleton IExternalAlarmWrapper
		* 
		* @return IExternalAlarmWrapper^ a singleton of IExternalAlarmWrapper
		*/
		static IExternalAlarmWrapper^ getExternalAlarmWrapper(unsigned long sourceID);

		/** 
		* Release singleton of IExternalAlarmWrapper
		*/
		static void releaseExternalWrapper(unsigned long sourceID);
	private:
		static IExternalAlarmWrapper ^ _IExternalAlarmWrapper;
		
	};
}
 