// AlarmSelectionWrap.h: interface for the AlarmSelectionWrap class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_AlarmSelectionWrap_H__2732015F_5F12_4C9E_BAE0_D9742103C695__INCLUDED_)
#define AFX_AlarmSelectionWrap_H__2732015F_5F12_4C9E_BAE0_D9742103C695__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/shared_alarm/src/CommonDef.h"
#include "core/alarm/src/AlarmModifyItem.h"
#include <set>

namespace TA_Base_Bus
{
	// forward declaration
	class CSimpleDispMultiIndex;

	class AlarmSelectionWrap  
	{
	public:
		AlarmSelectionWrap(CSimpleDispMultiIndex* pParent, TA_Base_Core::NonReEntrantThreadLockable& threadLock, std::set<std::string>& selectedAlarms);
		virtual ~AlarmSelectionWrap();

		// All items converted into AlarmModifyItems
		// The user is responsible for releasing the memory
		std::vector<TA_Base_Core::AlarmModifyItem*> getAllAlarmModifyItem();
		
		// The user is responsible for releasing the memory
		std::vector<TA_Base_Core::AlarmModifyItem*> getSelAlarmModifyItems();
		
		std::vector<std::string> getAllVisibleAlarms();
		
		std::vector<std::string> getSelectedAlarms();
		
		// The calling function is responsible for releasing the memory
		TA_Base_Core::AlarmModifyItem* getAlarmModifyItem(std::string strAlarmID);
		
		// use to retrieve string attributes in AlarmDataItem
		std::string getItemString(std::string strAlarmID, STRING_ATTR nSubIndex);
		
		// use to retrieve number attributes in AlarmDataItem
		unsigned long getItemNumber(std::string strAlarmID, NUMBER_ATTR nSubIndex);

		// The calling function is responsible for freeing the memory
		AlarmDataItem* getItemCopyID(std::string strAlarmID);

		// Retrieve all the related alarms from the head avalanche ID
		void getAvalanchedChildAlarms(std::string avalanchedHeadID, std::vector<std::string>& vecChildAlarms);

		bool isAlarmsVisible();

	private:
		TA_Base_Core::NonReEntrantThreadLockable& m_ThreadLock;
		std::set<std::string>& m_SelectedAlarms;
		CSimpleDispMultiIndex* m_pParentContainer;

		// Cache 1 alarm Item for faster retrieval
		AlarmDataItem* m_pCacheDataItem;
		std::string m_strLastAlarmIDAccess;
	};
}

#endif // !defined(AFX_AlarmSelectionWrap_H__2732015F_5F12_4C9E_BAE0_D9742103C695__INCLUDED_)
