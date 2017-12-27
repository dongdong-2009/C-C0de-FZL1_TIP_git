// ATSListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "AtsAlarmGenerator.h"
#include "ATSListCtrl.h"
#include "ThreadGuard.h"

// CATSListCtrl

IMPLEMENT_DYNAMIC(CATSListCtrl, CListCtrl)

CATSListCtrl::CATSListCtrl() 
	: m_pThreadAutoAckClose(0)
	, m_pUpdateCounters(0)
	, m_ulSumitted(0)
	, m_alarmCount(0)
{
}

CATSListCtrl::~CATSListCtrl()
{
}


BEGIN_MESSAGE_MAP(CATSListCtrl, CListCtrl)
//	ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, &CATSListCtrl::OnLvnEndlabeledit)
	ON_NOTIFY_REFLECT(LVN_DELETEITEM, &CATSListCtrl::OnLvnDeleteitem)
	//ON_MESSAGE(WM_)
	ON_MESSAGE(WMU_INSERT_ALARM, &CATSListCtrl::OnInsertAlarm)
	ON_MESSAGE(WMU_UPDATE_ALARM, &CATSListCtrl::OnUpdateAlarm)
	ON_MESSAGE(WMU_REMOVE_ALARM, &CATSListCtrl::OnRemoveAlarm)
	ON_MESSAGE(WMU_CLEAR_ALARMS, &CATSListCtrl::OnClearAlarms)
END_MESSAGE_MAP()

// CATSListCtrl message handlers
void CATSListCtrl::setupListControl(void)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	// SetUp Headers
	InsertColumn(0, "Date", LVCFMT_CENTER, 100);
	InsertColumn(1, "AlarmID", LVCFMT_CENTER, 150);
	InsertColumn(2, "State", LVCFMT_CENTER, 100);
	InsertColumn(3, "MMS State", LVCFMT_CENTER, 100);
	InsertColumn(4, "SubSystem", LVCFMT_CENTER, 100);
	InsertColumn(5, "Alarm Details", LVCFMT_LEFT, 300);

	SetExtendedStyle(LVS_EX_FULLROWSELECT);
}
// 
// bool CATSListCtrl::insertAlarm(time_t t_submit, I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetails)
// {
// 	TA_Base_Bus::ThreadGuard guard(m_threadLock);
// 	CString strDateTime = _formatTime(pAlarmDetails->sourceTime.time).c_str();
// 	CString alarmDetails = "";
// 	alarmDetails += CString(pAlarmDetails->locationId.c_str())+ ", ";
// 	alarmDetails += CString(pAlarmDetails->assetName.c_str())+ ", ";
// 	alarmDetails += CString(pAlarmDetails->alarmType.c_str()) + ", ";
// 	alarmDetails += CString(pAlarmDetails->alarmValue.c_str()) + ", ";
// 	alarmDetails += CString(pAlarmDetails->alarmDescription.c_str());
// 
// 	int nIndex = this->GetItemCount(); 
// 	InsertItem(nIndex,strDateTime);
// 	SetItemText(nIndex, 1, pAlarmDetails->alarmID.c_str());
// 	SetItemText(nIndex, 2, _formatState(pAlarmDetails->state).c_str());
// 	SetItemText(nIndex, 3, _formatMMSState(pAlarmDetails->mmsState).c_str());
// 	SetItemText(nIndex, 4, pAlarmDetails->subsytemkey.c_str());
// 	SetItemText(nIndex, 5, alarmDetails);
// 	SetItemData(nIndex, (DWORD)pAlarmDetails);
// 	m_ulSumitted++;
// 
// 	if (m_pThreadAutoAckClose)
// 	{
// 		AlarmQueueItem* pQueItem = new AlarmQueueItem();
// 		pQueItem->t_submit = t_submit;
// 		pQueItem->pAlarmDetail = pAlarmDetails;
// 		m_pThreadAutoAckClose->insert(pQueItem);
// 	}
// 
// 	m_pUpdateCounters->UpdateCounters(m_ulSumitted, GetItemCount());
// 	m_alarmCount++;
// 	return true;
// }

std::string CATSListCtrl::_formatTime(long ulTime)
{
	tm* ptm = gmtime(&ulTime);

	char szTime[256];
	strftime (szTime,256,"%H:%M:%S - %d-%b-%y", ptm);
	return std::string(szTime);
}

std::string CATSListCtrl::_formatState(I_ALARM_PROVIDER::AlarmState state)
{
	CString strState;
	switch (state)
	{
	case I_ALARM_PROVIDER::AlarmOpen:
		strState.Format("%s", "OPEN");
		break;
	case I_ALARM_PROVIDER::AlarmAcknowledged:
		strState.Format("%s", "ACKNOWLEDGE");
		break;
	case I_ALARM_PROVIDER::AlarmReturned:
		strState.Format("%s", "RETURNED");
		break;
	case I_ALARM_PROVIDER::AlarmClosed:
		strState.Format("%s", "CLOSED");
		break;
	default:
		strState.Format("%s", "unknown");
		break;

	}
	return strState.GetBuffer(0);
}

std::string CATSListCtrl::_formatMMSState(I_ALARM_PROVIDER::MmsStateType mmsStateType)
{
	STRING_LIST mmsStateList;
	CDataGenerator::getInstance().getStringList(MMSState, mmsStateList);
	
	return mmsStateList[(int)mmsStateType].c_str();
}

void CATSListCtrl::startAutoAckClose(int nAutoAckCloseInterval, HWND hWndDlg)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);

	if (NULL != m_pThreadAutoAckClose)
	{
		if (m_pThreadAutoAckClose->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
		{
			m_pThreadAutoAckClose->terminateAndWait();
		}
		delete m_pThreadAutoAckClose;
		m_pThreadAutoAckClose = NULL;
	}
	m_pThreadAutoAckClose = new CThreadAutoAckClose(nAutoAckCloseInterval, hWndDlg);
	m_pThreadAutoAckClose->start();
	m_ulSumitted = 0;
}

void CATSListCtrl::stopAutoAckClose()
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	if (NULL != m_pThreadAutoAckClose)
	{
		if (m_pThreadAutoAckClose->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
		{
			m_pThreadAutoAckClose->terminateAndWait();
		}
		delete m_pThreadAutoAckClose;
		m_pThreadAutoAckClose = NULL;
	}
}
void CATSListCtrl::finishAutoAck()
{
 	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	m_pThreadAutoAckClose->finishedAutoAck();
// 	if (NULL != m_pThreadAutoAckClose)
// 	{
// 		if (m_pThreadAutoAckClose->getCurrentState() == TA_Base_Core::Thread::THREAD_STATE_RUNNING)
// 		{
// 			m_pThreadAutoAckClose->finishedAutoAck();
// 			m_pThreadAutoAckClose->terminateAndWait();
// 		}
// 		delete m_pThreadAutoAckClose;
// 		m_pThreadAutoAckClose = NULL;
// 	}
}

// bool CATSListCtrl::updateAlarm(I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetail)
// {
// 	bool bResult = false;
// 	for (int nIdx = 0; nIdx < m_alarmCount; nIdx++ )
// 	{
// 		if (strcmpi(GetItemText(nIdx, 1), pAlarmDetail->alarmID.c_str()) == 0)
// 		{
// 			SetItemText(nIdx, 2, _formatState(pAlarmDetail->state).c_str());
// 			SetItemText(nIdx, 3, _formatMMSState(pAlarmDetail->mmsState).c_str());
// 			bResult = true;
// 			break;
// 		}
// 	}
// 	return bResult;
// }
// 
// bool CATSListCtrl::removeAlarm(std::string alarmID)
// {
// 	TA_Base_Bus::ThreadGuard guard(m_threadLock);
// 	bool bResult = false;
// 	for (int nIdx = 0; nIdx < this->GetItemCount(); nIdx++ )
// 	{
// 		if (strcmpi(GetItemText(nIdx, 1), alarmID.c_str()) == 0)
// 		{
// 			this->DeleteItem(nIdx);
// 			m_pUpdateCounters->UpdateCounters(m_ulSumitted, GetItemCount());
// 			bResult = true;
// 			break;
// 		}
// 	}
// 
// 	return bResult;
// }
// 
// bool CATSListCtrl::removeAllAlarms()
// {
// 	TA_Base_Bus::ThreadGuard guard(m_threadLock);
// 	DeleteAllItems();
// 
// 	return true;
// }
//void CATSListCtrl::OnLvnEndlabeledit(NMHDR *pNMHDR, LRESULT *pResult)
//{
//	NMLVDISPINFO *pDispInfo = reinterpret_cast<NMLVDISPINFO*>(pNMHDR);
//	// TODO: Add your control notification handler code here
//	*pResult = 0;
//}

void CATSListCtrl::OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// TODO: Add your control notification handler code here
	int nIndex = pNMLV->iItem;
	I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = (I_ALARM_PROVIDER::AlarmDetail*) GetItemData(nIndex);

	if (m_pThreadAutoAckClose)
	{
		m_pThreadAutoAckClose->remove(pAlarmDetail);
	}
	
	m_alarmDetailMap.erase(pAlarmDetail->alarmID);

	if (NULL != pAlarmDetail)
	{
		delete pAlarmDetail;
		pAlarmDetail = NULL;
	}
	*pResult = 0;
}

void CATSListCtrl::registerCountObserver(IUpdateCounters * pUpdateCounter)
{
	if (pUpdateCounter == NULL) return;
	
	m_pUpdateCounters = pUpdateCounter;
}

void CATSListCtrl::getSelectedAlarms(ALARMDETAIL_LIST& alarmList)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	alarmList.clear();
	
// 	if (GetSelectedCount() > 1)
// 	{
		// This means we want to unselect any items that are currently selected
		POSITION pos = GetFirstSelectedItemPosition();		
		while (pos != NULL)
		{
			int nIndex = GetNextSelectedItem(pos);
			I_ALARM_PROVIDER::AlarmDetail * pAlarmDetail = (I_ALARM_PROVIDER::AlarmDetail*)GetItemData(nIndex);
			if (pAlarmDetail != NULL)
			{
				alarmList.push_back(pAlarmDetail);
			}
		}
/*	}*/

}

I_ALARM_PROVIDER::AlarmDetail* CATSListCtrl::getAlarmDetails(std::string& strAlarmID)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = NULL;

	ALARMDETAIL_MAP::iterator itFind = m_alarmDetailMap.find(strAlarmID);
	if (itFind != m_alarmDetailMap.end())
	{
		pAlarmDetail = itFind->second;
	}

	return pAlarmDetail;
}

void CATSListCtrl::removeAlarmFromAutoClose(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetails)
{
	if (m_pThreadAutoAckClose != NULL)
	{
		m_pThreadAutoAckClose->remove(pAlarmDetails);
	}
}

LRESULT CATSListCtrl::OnInsertAlarm(WPARAM wParam, LPARAM lParam)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	I_ALARM_PROVIDER::AlarmDetail* pAlarmDetails = reinterpret_cast<I_ALARM_PROVIDER::AlarmDetail*>(lParam);
	time_t t_submit = static_cast<time_t>(wParam);

	CString strDateTime = _formatTime(pAlarmDetails->sourceTime.time).c_str();
	CString alarmDetails = "";
	alarmDetails += CString(pAlarmDetails->locationId.c_str())+ ", ";
	alarmDetails += CString(pAlarmDetails->assetName.c_str())+ ", ";
	alarmDetails += CString(pAlarmDetails->alarmType.c_str()) + ", ";
	alarmDetails += CString(pAlarmDetails->alarmValue.c_str()) + ", ";
	alarmDetails += CString(pAlarmDetails->alarmDescription.c_str());

	int nIndex = this->GetItemCount(); 
	InsertItem(nIndex,strDateTime);
	SetItemText(nIndex, 1, pAlarmDetails->alarmID.c_str());
	SetItemText(nIndex, 2, _formatState(pAlarmDetails->state).c_str());
	SetItemText(nIndex, 3, _formatMMSState(pAlarmDetails->mmsState).c_str());
	SetItemText(nIndex, 4, pAlarmDetails->subsytemkey.c_str());
	SetItemText(nIndex, 5, alarmDetails);
	SetItemData(nIndex, (DWORD)pAlarmDetails);
	m_alarmDetailMap.insert(ALARMDETAIL_MAP::value_type(pAlarmDetails->alarmID, pAlarmDetails));
	m_ulSumitted++;
	
	if (m_pThreadAutoAckClose)
	{
		AlarmQueueItem* pQueItem = new AlarmQueueItem();
		pQueItem->t_submit = t_submit;
		pQueItem->pAlarmDetail = pAlarmDetails;
		m_pThreadAutoAckClose->insert(pQueItem);
	}

	m_pUpdateCounters->UpdateCounters(m_ulSumitted, GetItemCount());
	return 0;
}

LRESULT CATSListCtrl::OnUpdateAlarm(WPARAM wParam, LPARAM lParam)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = reinterpret_cast<I_ALARM_PROVIDER::AlarmDetail*>(lParam);

	for (int nIdx = 0; nIdx < GetItemCount(); nIdx++ )
	{
		if (_strcmpi(GetItemText(nIdx, 1), pAlarmDetail->alarmID.c_str()) == 0)
		{
			SetItemText(nIdx, 2, _formatState(pAlarmDetail->state).c_str());
			SetItemText(nIdx, 3, _formatMMSState(pAlarmDetail->mmsState).c_str());
			//bResult = true;
			break;
		}
	}
	return 0;
}
LRESULT	CATSListCtrl::OnRemoveAlarm(WPARAM wParam, LPARAM lParam)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	I_ALARM_PROVIDER::AlarmDetail* pAlarmDetail = reinterpret_cast<I_ALARM_PROVIDER::AlarmDetail*>(lParam);

	for (int nIdx = 0; nIdx < this->GetItemCount(); nIdx++ )
	{
		if (_strcmpi(GetItemText(nIdx, 1), pAlarmDetail->alarmID.c_str()) == 0)
		{
			this->DeleteItem(nIdx);
			m_pUpdateCounters->UpdateCounters(m_ulSumitted, GetItemCount());
			/*bResult = true;*/
			break;
		}
	}

	return 0;
}

LRESULT CATSListCtrl::OnClearAlarms(WPARAM wParam, LPARAM lParam)
{
	TA_Base_Bus::ThreadGuard guard(m_threadLock);
	DeleteAllItems();
	return 0;
}
