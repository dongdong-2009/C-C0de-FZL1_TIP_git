// MmsQueueItem.h: interface for the MmsQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_ATSMMSQUEUEITEM_H__5C47BD12_6CBA_48DB_8B86_DFEFB2D28E36__INCLUDED_)
#define AFX_ATSMMSQUEUEITEM_H__5C47BD12_6CBA_48DB_8B86_DFEFB2D28E36__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "DataStructInfo.h"

class MmsQueueItem  
{
public:
	MmsQueueItem(const char* _assetName, const char* _alarmId, const char* _operatorId, unsigned long targetDate, const char* _problemReport);
	MmsQueueItem();
	~MmsQueueItem();

	MmsQueueItem(const MmsQueueItem& atsMmsQueueItem);
	MmsQueueItem& operator=(const MmsQueueItem& atsMmsQueueItem);

public:
	char m_assetName[ASSERT_NAME_SIZE];
	char m_alarmId[ALARM_ID_SIZE];
	char m_operatorId[OPERATOR_ID_SIZE]; // ��Requestor��
	unsigned long m_targetDate; // 0 if no target completion date. // zhiqiang Ŀǰ�в��������ֶ���ʲô��˼.
	char m_problemReport[ALARM_DESCRIPTION_SIZE]; // zhiqiang Ŀǰ�в��������ֶ���ʲô��˼,����һ�����ܵ�ֵ.
};

#endif // !defined(AFX_ATSMMSQUEUEITEM_H__5C47BD12_6CBA_48DB_8B86_DFEFB2D28E36__INCLUDED_)
