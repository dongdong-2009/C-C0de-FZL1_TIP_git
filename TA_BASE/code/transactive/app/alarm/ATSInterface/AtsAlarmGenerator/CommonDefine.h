#pragma once
#include <vector>
#include <map>
#include <string>
#include <queue>
#include "..\ShareMemoryDLL\AlarmApi.h"

#define VA_SUBSYSTEM_ID		10001

#define HIGHEST_SEVERITY 3
#define UPDATE_AUTO_TIMER 1
#define UPDATE_TIME 1000

// windows messages
#define WMU_AUTO_SUBMIT_FINISH		WM_USER + 300
#define WMU_AUTO_ACKCLOSE_FINISH	WM_USER + 301

#define WMU_INSERT_ALARM		WM_USER + 302
#define WMU_UPDATE_ALARM		WM_USER + 303
#define WMU_REMOVE_ALARM		WM_USER + 304
#define WMU_CLEAR_ALARMS		WM_USER + 305

typedef std::vector<std::string> STRING_LIST;
typedef std::map<std::string, std::vector<std::string> > MAP_CONTAINER;
typedef struct std::pair<std::string, std::vector<std::string> > MK_PAIR;
typedef std::vector<I_ALARM_PROVIDER::AlarmDetail*> ALARMDETAIL_LIST;
typedef std::map<std::string, I_ALARM_PROVIDER::AlarmDetail*> ALARMDETAIL_MAP;

// CATSListCtrl
struct AlarmQueueItem
{
	time_t t_submit;
	I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail;

	AlarmQueueItem() : t_submit(0), pAlarmDetail(0){};
	~AlarmQueueItem(){};
};

typedef std::vector<AlarmQueueItem*> ALARMDETAIL_QUEUE;
