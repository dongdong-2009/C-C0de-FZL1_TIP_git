#ifndef  __AFX_IALARM_DISPLAY_UPDATE_INCLUDED__
#define  __AFX_IALARM_DISPLAY_UPDATE_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
	
	class IAlarmDisplayUpdate
	{
	public:
		enum NOTIFY_MGS {NOTIFY_REFRESH = 0, NOTIFY_RESTART, NOTIFY_REMAP, NOTIFY_UNMAP};
		virtual void updateDisplay(NOTIFY_MGS msg) = 0;

	};
}


#endif  // __AFX_IDUTY_ALARM_UPDATE_INCLUDED__