// AlarmVectorWrap.h: interface for the CAlarmList class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ALARMLIST_H__F1D98609_90E5_4477_8EE8_E6735D819714__INCLUDED_)
#define AFX_ALARMLIST_H__F1D98609_90E5_4477_8EE8_E6735D819714__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/alarm/shared_alarm/src/CommonDef.h"
#include "core/alarm/src/AlarmModifyItem.h"

namespace TA_Base_Bus
{
	// forward declaration
	class CSimpleDispMultiIndex;

	class AlarmVectorWrap  
	{
	public:

		AlarmVectorWrap(CSimpleDispMultiIndex * pParentContainer, TA_Base_Core::NonReEntrantThreadLockable& threadLock, std::vector<AlarmDataItem*>& displayVec);
		virtual ~AlarmVectorWrap();
		
		// Use to retrieve the new instance of alarm Item at position nIndex.
		// The user is responsible for freeing object if this function is called.
		AlarmDataItem* getItemCopyAt(unsigned long nIndex);

		//AlarmVectorWrap* getSelectedAlarms();
		// The user is responsible for freeing object if this function is called.
		AlarmDataItem* getItemCopyID(std::string strAlarmID);

		// use to retrieve string attributes in AlarmDataItem
		std::string getItemString(unsigned long nIndex, STRING_ATTR nSubIndex);
		
		// use to retrieve number attributes in AlarmDataItem
		unsigned long getItemNumber(unsigned long nIndex, NUMBER_ATTR nSubIndex);

		// Use to retrieve the current total size of alarms
		unsigned long getSize();

// 		// Set the sorting order // by default its true
// 		void setSortAccending(bool bSortAccending);

		// Use to locate the current position of an alarm item. This is used in the new selection
		// mechanism to easily update the current position of the selected item.
		unsigned long findItemPos(std::string strAlarmID);

		void _clearCache();

	private: // variables
		TA_Base_Core::NonReEntrantThreadLockable& m_ThreadLock;
		std::vector<AlarmDataItem*>& m_displayVec;

		CSimpleDispMultiIndex * m_pParentContainer;

		// Cache AlarmDataItem
		TA_Base_Core::NonReEntrantThreadLockable m_CacheLock;
		AlarmDataItem* m_cacheDataItem;
		unsigned long m_ulLastIndex;
	};

}

#endif // !defined(AFX_ALARMLIST_H__F1D98609_90E5_4477_8EE8_E6735D819714__INCLUDED_)
