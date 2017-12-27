// LogAlarmItem.cpp: implementation of the LogAlarmItem class.
//
//////////////////////////////////////////////////////////////////////

#include "LogAlarmItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
#include "core/utilities/src/DebugUtil.h"


using TA_Base_Core::DebugUtil;

namespace TA_Base_Bus
{
	LogAlarmItem::LogAlarmItem()
	{

	}

	LogAlarmItem::~LogAlarmItem()
	{

	}
 
	void LogAlarmItem::log(TA_Base_Core::AlarmItem* alarm)
	{
		if (alarm ==NULL) return;
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "address=%ld",  alarm );   
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "alarmID=%s ackby=%s", alarm->alarmID, alarm->alarmAcknowledgedBy );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Comment=%s Description=%s", alarm->alarmComments, alarm->alarmDescription );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Severity=%ld value=%s", alarm->alarmSeverity, alarm->alarmValue );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "asset=%s entityKey=%ld", alarm->assetName, alarm->assocEntityKey );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "entityType=%ld avl=%s", alarm->assocEntityTypeKey, alarm->avalanche );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "avlID=%s dss=%s", alarm->avalancheHeadID, alarm->decisionSupportState );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "entityName=%s location=%ld", alarm->entityName, alarm->locationKey );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "messageTypeKey=%ld mmsState=%s", alarm->messageTypeKey, alarm->mmsState );
// 		LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "state=%ld subsystemKey=%ld", alarm->state, alarm->subsystemKey );
 	
		 	
	}
}