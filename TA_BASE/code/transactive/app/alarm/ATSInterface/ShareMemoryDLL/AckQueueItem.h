// AckQueueItem.h: interface for the AckQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSACKQUEUEITEM_H__1998F341_1C0D_4751_B58F_CC47A95F17CF__INCLUDED_)
#define AFX_ATSACKQUEUEITEM_H__1998F341_1C0D_4751_B58F_CC47A95F17CF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataStructInfo.h"

class AckQueueItem  
{
public:
	AckQueueItem();
	AckQueueItem(const char*_alarmId, const char* _alarmAcknowledgeBy, unsigned long _ackTime);
	
	AckQueueItem(const AckQueueItem& atsAckQueueItem);
	AckQueueItem& operator=(const AckQueueItem& atsAckQueueItem);

	~AckQueueItem();

public:
	char m_alarmId[ALARM_ID_SIZE]; 		 
	char m_alarmAcknowledgeBy[ALARM_ACKNOWLEDGE_BY_SIZE];  // operatorId 
	unsigned long m_ackTime;
};

#endif // !defined(AFX_ATSACKQUEUEITEM_H__1998F341_1C0D_4751_B58F_CC47A95F17CF__INCLUDED_)
