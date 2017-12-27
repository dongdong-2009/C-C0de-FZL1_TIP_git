// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__DB41F486_9A03_4B66_9DBB_7DA080222CE8__INCLUDED_)
#define AFX_STDAFX_H__DB41F486_9A03_4B66_9DBB_7DA080222CE8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


// Insert your headers here
#define WIN32_LEAN_AND_MEAN		// Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <xmemory>
#include <algorithm>
#include <vector>
#include <list>
#include "core/alarm/IDL/src/AlarmMessageCorbaDef.h"
#include "core/alarm/IDL/src/AlarmStoreCorbaDef.h"
#include "core/alarm/src/AlarmConfig.h"
#include "core/alarm/src/AlarmItem.h"
#include "core/alarm/src/AlarmModifyItem.h"
#include "bus/alarm/shared_alarm/src/AlarmDataItem.h"

#include "bus/alarm/alarm_store_library/src/AlarmFilterSet.h"
#include "bus/alarm/alarm_store_library/src/AlarmStoreManager.h"


#define DUTYCACHELOCK    		"Global\\TA_DutyCacheLock"
#define SIGLOCKNAME				"Global\\TA_SignalLock"
#define ALLALARMLOCKNAME		"Global\\TA_AllAlarmLock"
#define DUTYINDEXLOCKNAME		"Global\\TA_DutyLock"
//#define FILTERINDEXLOCKNAME		"TA_FilterLock"
#define FASALARMLOCK			"Global\\TA_FASALARMLOCK"
#define SHARED_MEM_INIT			"Global\\TA_Shared_Mem_Init"

//#define STOREREADYNAME		"StoreReady"
#define REBUILDEVENTNAME		"TA_RebuildEvent"


#define SYN_EVENT HANDLE
#define SYN_MUTEX HANDLE

#define MAXREBUILDTIME 180
#define MAXBUILDCOUNT 500
#define MAXDUTYALARMCOUNT 30000
#define MAXALLALARMCOUNT 100000
/*************************** Index order **********************************/
typedef enum
{
	NOSORT_BY_HASH,
	NOSORT_BY_SUBSYSTEM,
	SORT_BY_DEFAULT,
	SORT_BY_DATETIME,
	SORT_BY_SEVERITY,
	SORT_BY_ASSET,
	SORT_BY_DESCRIPTION,
	SORT_BY_VALUE,	
	SORT_BY_MMS,
	SORT_BY_DSS,
	SORT_BY_AVALANCHE,
	SORT_BY_ACK,
	SORT_BY_COMMENTS,
	SORT_BY_ALARMOPERATORTYPE,
	SORT_BY_STRALARMTYPE,
	SORT_BY_SUBSYSTEMTYPE,
	SORT_BY_SYSTEMKEYTYPE,
	SORT_BY_LOCATION,
	SORT_BY_ALARMEXTFLAG,
	SORT_BY_SEVERITYUNACK
}SORT_COLUMN;


// Enumeration for string attributes
enum STRING_ATTR 
{
	ENTITY_NAME = 0,
	AVALANCHE,
	ASSET_NAME,
	ALARMID,
	AVALANCHE_HEAD_ID,
	ALARM_DESCRIPTION,
	ALARM_ACKNOWLEDGE_BY,
	ALARM_COMMENTS,
	DDS,
	MMS_STATE,
	ALARM_VALUE,
	ALARM_OPERATOR_MODE,
	MAX_STRING_ATTR
};

// Enumeration for number attributes
enum NUMBER_ATTR
{
	STATE = 0,
	ALARM_SEVERITY,
	SUBSYSTEM_KEY,
	ASSOC_ENTITY_KEY,
	ASSOC_ENTITY_TYPE_KEY,
	MESSAGE_TYPE_KEY,
	LOCATION_KEY,
	TRANSACTIVETIME_TIME,
	TRANSACTIVETIME_MILI,
	IS_EXT_ALARM,
	MAX_NUMBER_ATTR
};
/**************************************************************************/

/****************************** display Order ******************************/
typedef std::vector<TA_Base_Core::AlarmItem*> displayVec;
typedef std::list<TA_Base_Core::AlarmItem*> AlarmListT;
/**************************************************************************/


// TODO: reference additional headers your program requires here

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STDAFX_H__DB41F486_9A03_4B66_9DBB_7DA080222CE8__INCLUDED_)
