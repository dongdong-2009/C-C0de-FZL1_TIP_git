#pragma once
#include "CommandHandler.h"
#include "DataGenerator.h"
#include "NonReEntrantThreadLockable.h"
#include "ThreadAutoAckClose.h"
#include "IAlarmUpdate.h"
#include "IUpdateCounters.h"
#include <set>

class CATSListCtrl : public CListCtrl//, public IAlarmUpdate
{
	DECLARE_DYNAMIC(CATSListCtrl)

public:
	CATSListCtrl();
	virtual ~CATSListCtrl();
	afx_msg void OnLvnDeleteitem(NMHDR *pNMHDR, LRESULT *pResult);
	void setupListControl(void);
	void startAutoAckClose(int nAutoAckCloseInterval, HWND hWndDlg);
	void stopAutoAckClose();
	void finishAutoAck();
	void registerCountObserver(IUpdateCounters * pUpdateCounter);
	void removeAlarmFromAutoClose(I_ALARM_PROVIDER::AlarmDetail* pAlarmDetails);
	
// 	virtual bool insertAlarm(time_t t_submit, I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetails);
// 	virtual bool updateAlarm(I_ATS_ISCS_ALARM::AlarmDetail* pAlarmDetail);
// 	virtual bool removeAlarm(std::string alarmID);
// 	virtual bool removeAllAlarms();

	void getSelectedAlarms(ALARMDETAIL_LIST & alarmList);
	I_ALARM_PROVIDER::AlarmDetail* getAlarmDetails(std::string& strAlarmID);

	inline void resetSubmitCounter(){ m_ulSumitted = 0; };


protected:
	afx_msg LRESULT  OnInsertAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnUpdateAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	 OnRemoveAlarm(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT  OnClearAlarms(WPARAM wParam, LPARAM lParam);
	
	DECLARE_MESSAGE_MAP()
	
private: // functions
	std::string _formatTime(long ulTime);
	std::string _formatState(I_ALARM_PROVIDER::AlarmState state);
	std::string _formatMMSState(I_ALARM_PROVIDER::MmsStateType mmsStateType);

private: // variables
	ALARMDETAIL_MAP m_alarmDetailMap;
	CThreadAutoAckClose * m_pThreadAutoAckClose;
	TA_Base_Bus::NonReEntrantThreadLockable m_threadLock;
	IUpdateCounters* m_pUpdateCounters;
	ULONG m_ulSumitted;
	ULONG m_alarmCount;

};


