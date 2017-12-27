// LogAlarmItem.h: interface for the LogAlarmItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LOGALARMITEM_H__8EA356C2_674C_44E8_A3EF_B07AEE7BBDAD__INCLUDED_)
#define AFX_LOGALARMITEM_H__8EA356C2_674C_44E8_A3EF_B07AEE7BBDAD__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/alarm/src/AlarmItem.h"

namespace TA_Base_Bus
{
	class LogAlarmItem  
	{
	public:
		LogAlarmItem();
		virtual ~LogAlarmItem();

		static void log(TA_Base_Core::AlarmItem* alarm);

	};
}

#endif // !defined(AFX_LOGALARMITEM_H__8EA356C2_674C_44E8_A3EF_B07AEE7BBDAD__INCLUDED_)
