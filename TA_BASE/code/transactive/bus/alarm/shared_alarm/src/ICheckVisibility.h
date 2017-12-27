// Author: Noel R. Tajanlangit


#ifndef __ICHECK_VISIBILITY_H__
#define __ICHECK_VISIBILITY_H__

#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"

namespace TA_Base_Bus
{
	class ICheckVisibility
	{
	public:
		virtual ~ICheckVisibility(){};

		virtual bool isAlarmVisible(AlarmDataItem* pAlarm) = 0;
	};
}


#endif //__ICHECK_VISIBILITY_H__