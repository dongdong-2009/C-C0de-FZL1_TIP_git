#if !defined(__AFX_ALARM_MODIFY_ITEM_INCLUDED__)
#define __AFX_ALARM_MODIFY_ITEM_INCLUDED__

#include <string>

namespace TA_Base_Core
{
	class AlarmModifyItem
	{
	public:
		inline AlarmModifyItem(
			std::string _strAlarmID, 
			std::string _strAcknowledgeBy, 
			unsigned long _ulLocationKey) :
		alarmID(_strAlarmID),
		alarmAcknowledgedBy(_strAcknowledgeBy),
		locationKey(_ulLocationKey)
		{};

		inline ~AlarmModifyItem(){};
		
		std::string alarmID;
		unsigned long locationKey;
		std::string alarmAcknowledgedBy;
		
	private:
		AlarmModifyItem();
	};
}

#endif //__AFX_ALARM_MODIFY_ITEM_INCLUDED__