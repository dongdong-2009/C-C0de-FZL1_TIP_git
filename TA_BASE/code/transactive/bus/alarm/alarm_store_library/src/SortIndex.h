/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution in any form.
  *
  * Source:   $File: //depot/3001_TIP/TA_BASE/transactive/bus/alarm/alarm_store_library/src/SortIndex.h$
  * @author:  Noel R. Tajanlangit
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2011/10/03 10:07:40 $
  * Last modified by:  $Author: Noel R. Tajanlangit $
  *
  * This is the header for the sorting algorithm used to sort the in memory container for All Alarm Manager.
  */

#if !defined(__SORT_INDEX_INCLUDED__)
#define __SORT_INDEX_INCLUDED__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//Sort Functions //

//Sort By ID
/************************* Compare with char* *********************************/
template<class _Ty>
struct SortCharptrless : std::binary_function<_Ty, _Ty, bool> 
{
	bool operator()(const _Ty& _X, const _Ty& _Y) const
	{
		return (strcmp(_X, _Y) < 0); 
	}
};



//////////////////////////////////////////////////////////////////////////
// SORT_BY_DATETIME
// Note that this sort function is dependent on the overloaded comparison 
// method of TA_Base_Core::ItemMillisecondTime 
//////////////////////////////////////////////////////////////////////////
struct SortByDateTime
{
	bool operator ()(const TA_Base_Bus::AlarmDataItem* lhs, const TA_Base_Bus::AlarmDataItem* rhs) const
	{		
		if (lhs->transactiveTime == rhs->transactiveTime)
		{
			return ((DWORD)lhs < (DWORD)rhs);
		}
		return (lhs->transactiveTime < rhs->transactiveTime);
	}
};



#endif // !defined(__SORT_INDEX_INCLUDED__)
