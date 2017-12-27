// ATSMmsQueueItem.cpp: implementation of the ATSMmsQueueItem class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "MmsQueueItem.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

MmsQueueItem::MmsQueueItem(const char* _assetName, const char* _alarmId, const char* _operatorId, 
								 unsigned long targetDate, const char* _problemReport)
{
	memset(m_assetName, 0, ASSERT_NAME_SIZE);
	strcpy(m_assetName, _assetName);
	
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	strcpy(m_alarmId, _alarmId);

	memset(m_operatorId, 0, OPERATOR_ID_SIZE);
	strcpy(m_operatorId, _operatorId);

	m_targetDate = targetDate;
	
	memset(m_problemReport, 0, ALARM_DESCRIPTION_SIZE);
	strcpy(m_problemReport, _problemReport);
}

MmsQueueItem::MmsQueueItem() : m_targetDate(0)
{
	memset(m_assetName, 0, ASSERT_NAME_SIZE);
	memset(m_alarmId, 0, ALARM_ID_SIZE);
	memset(m_operatorId, 0, OPERATOR_ID_SIZE);
	memset(m_problemReport, 0, ALARM_DESCRIPTION_SIZE);
}

MmsQueueItem::~MmsQueueItem()
{
}

MmsQueueItem::MmsQueueItem(const MmsQueueItem& atsMmsQueueItem)
{
	memcpy(m_assetName, atsMmsQueueItem.m_assetName, ASSERT_NAME_SIZE);
	memcpy(m_alarmId, atsMmsQueueItem.m_alarmId, ALARM_ID_SIZE);
	memcpy(m_operatorId, atsMmsQueueItem.m_operatorId, OPERATOR_ID_SIZE);
	memcpy(m_problemReport, atsMmsQueueItem.m_problemReport, ALARM_DESCRIPTION_SIZE);
	m_targetDate = atsMmsQueueItem.m_targetDate;
}

MmsQueueItem& MmsQueueItem::operator=(const MmsQueueItem& atsMmsQueueItem)
{
	if (this == &atsMmsQueueItem)
		return *this;

	memcpy(m_assetName, atsMmsQueueItem.m_assetName, ASSERT_NAME_SIZE);
	memcpy(m_alarmId, atsMmsQueueItem.m_alarmId, ALARM_ID_SIZE);
	memcpy(m_operatorId, atsMmsQueueItem.m_operatorId, OPERATOR_ID_SIZE);
	memcpy(m_problemReport, atsMmsQueueItem.m_problemReport, ALARM_DESCRIPTION_SIZE);
	m_targetDate = atsMmsQueueItem.m_targetDate;

	return *this;
}