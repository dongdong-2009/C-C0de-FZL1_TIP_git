// AlarmDataSync.h: interface for the AlarmDataSync class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMDATASYNC_H__11B95B15_E172_4811_B775_AF88F2EC75BA__INCLUDED_)
#define AFX_ALARMDATASYNC_H__11B95B15_E172_4811_B775_AF88F2EC75BA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "IAlarmDisplayUpdate.h"
#include "SimpleDispMultiIndex.h"
#include "bus/alarm/alarm_list_control/src/AlarmStore.h"

namespace TA_Base_Bus
{
	class AlarmDataSync  :  public IAlarmDisplayUpdate
	{
	public:
		AlarmDataSync();
		virtual ~AlarmDataSync();
	
		inline void setTarget(HWND target)
		{
			m_hwndTarget = target;		
		}

		void initialize()
		{
			m_pDisplayIndex->start();
		}

		// This used to filter the alarm items based on user configuration
//		void applyDutyIndexFilter(AlarmFilterSet* pFilterSet);
		void applyDutyIndexFilter(const Filter& filter, bool isDefault);
		
		// Use to dynamically change the sorting mechanism for the display items
		void setDisplaySort(SORT_COLUMN sortType, bool bSortAccending);
		AlarmVectorWrap* getDisplayData();
		AlarmSelectionWrap* getSelectionWrapper();
		
 		// This function will return the current container and data information
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo& getIndexInfo();
		
		// Methods for Select/UnSelect alarms
		// This function is used to check if alarm has been selected
		bool isAlarmItemSelected(std::string strAlarmID);
		
		// This function is used add alarms to the selected items
		void selectAlarmItem(unsigned long ulIndex);
		
		void selectRangeAlarms(unsigned long ulSelBegin, unsigned long ulSelEnd);

		// This function is used to unselect a particular alarm item.
		void unselectAlarmItem(unsigned long ulIndex);
		
		// Function to unselect all alarm items currently selected
		void unSelectAllAlarmItem();

		unsigned long getSelectedAlarmsCount();

		void updateDisplay(NOTIFY_MGS msg);

		bool isUpdating();

// 		// use to retrieve string attributes in AlarmDataItem
// 		std::string getItemString(std::string strAlarmID, STRING_ATTR nSubIndex);
// 		
// 		// use to retrieve number attributes in AlarmDataItem
// 		unsigned long getItemNumber(std::string strAlarmID, NUMBER_ATTR nSubIndex);

		inline int getClientPos(){return m_pDisplayIndex->getClientPos();};

		void setListDisplayInfo(ListDispInfo& listInfo);

		void setStoreService(AlarmStore * pAlarmStore);

		void setFreezeCacheRefresh(bool bFreezeCacheRefresh);

		std::string getAlarmID(unsigned long ulIndex);

	private:
		HWND m_hwndTarget;
		TA_Base_Core::IAlarmStoreCorbaDef::IndexInfo m_info;
		CSimpleDispMultiIndex* m_pDisplayIndex;

		//This is used in setting the filters  
		AlarmStore* m_pAlarmStore;
	};

}


#endif // !defined(AFX_ALARMDATASYNC_H__11B95B15_E172_4811_B775_AF88F2EC75BA__INCLUDED_)
