// AlarmStoreHarnessDlg.h : header file
//

#if !defined(AFX_ALARMSTOREHARNESSDLG_H__5B702FF7_E9AD_43BC_9B55_10770958A1D0__INCLUDED_)
#define AFX_ALARMSTOREHARNESSDLG_H__5B702FF7_E9AD_43BC_9B55_10770958A1D0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SimAlarmAgentImpl.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/RunParams.h"
#include <list>
#include <map>

#include "bus/alarm/alarm_list_control/src/AlarmListCtrl.h"
#include "bus/alarm/alarm_list_control/src/IAlarmListObserver.h"

class AlarmListCtrlEx : virtual public AlarmStoreAction
					  , virtual public TA_Base_Bus::AlarmListCtrl
{
public:
	AlarmListCtrlEx(bool canColumnsBeSorted /*=true*/, bool canColumnsBeReordered /*=true*/)
		: AlarmListCtrl(canColumnsBeReordered, canColumnsBeReordered)
	{
		
	}
	
	virtual ~AlarmListCtrlEx()
	{
		
	}
	
	std::vector<TA_Base_Core::AlarmItem*> getAllVisibleAlarmsEx() const
	{
		return getAllVisibleAlarms();
	}

	virtual void NewAlarm( TA_Base_Core::AlarmDetailCorbaDef& detail )
	{
		newAlarm(detail);
	};

	virtual void UpdateAlarm( TA_Base_Core::AlarmUpdateCorbaDef& update )
	{
		updateAlarm(update);
	}

	virtual void RebuildAlarms()
	{
		rebuildAlarmIndex();
	}
	
};


/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessDlg dialog

class CAlarmStoreHarnessDlg : public CDialog
							, public TA_Base_Bus::IAlarmListObserver
{
// Construction
public:
	CAlarmStoreHarnessDlg(CWnd* pParent = NULL);	// standard constructor
	virtual ~CAlarmStoreHarnessDlg();

// Dialog Data
	//{{AFX_DATA(CAlarmStoreHarnessDlg)
	enum { IDD = IDD_ALARMSTOREHARNESS_DIALOG };
	CComboBox	m_comboFilter;
	CComboBox	m_comboSort;
	CComboBox	m_comboLocation;
	CEdit	m_alarmStoreEdit;
	CEdit	m_corbaIntervalEdit;
	CEdit	m_msgIntervalEdit;
	CEdit	m_sortIntervalEdit;
	CEdit	m_filterIntervalEdit;
	CEdit	m_rebuildIntervalEdit;
	CButton	m_testActionBtn;
	CEdit	m_actionEdit;
	CString	m_cfgFileName;
	CString	m_actionInfo;
	CString	m_alarmStortInfo;
	CString m_totalAlarmsString;
	CString m_notAckAlarmsString;
	BOOL m_checkAlarm;
	BOOL m_checkFilter;
	BOOL m_checkSort;
	BOOL m_checkRebuild;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAlarmStoreHarnessDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
public:
	void Initialize();

	///////////////////////////////////////////////////////////////////
	// TA_Base_Bus::IAlarmListObserver interface implementation
   /**
    * updateAlarmCounts
    * 
    * The virtual method that updates the container's total acknowledge and unacknowledge values.
    *
    * @param The number of alarms in the system.
    *
    * @param The number of unacknowledged alarms in the system.
    *
    * @param The indicator whether the store has more than the alarms displayed
    *
    * @param The indicator whether store is still busy loading all alarms
	*/
	virtual void updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms);
	
	
	/**
	* updateAlarmSound
	* 
	* The virtual method that updates the container's alarm severity tones.
	*
	* @param The severity tone to update as an unsigned long.
	* @param True if new alarms were inserted, and false otherwise.
	*/
	virtual void updateAlarmSound(unsigned long severity, bool wereAlarmsAdded);
	
	
	/**
	* updateAvailableAlarmActions
	*
	* This will pass in the current availability of each alarm action. This will be called whenever
	* the alarm selection changes or the current alarms change.
	*
	* @param ItemAvailability - This should pass the availability of every possible alarm action currently
	*                           being used.
	*/
	virtual void updateAvailableAlarmActions(const ItemAvailability& availability);
	
	
	/**
	* setCurrentDescriptionText
	*
	* This will pass some appropriate text to be shown by the observer in a status bar or the like.
	* This string will hold the description of the selected alarm. If multiple alarms are selected it
	* will indicate how many alarms are selected
	*
	* @param CString - The text to be displayed
	*/
	virtual void setCurrentDescriptionText(const CString& description);
	
	
	/**
	* getProgressBar
	* 
	* The virtual method that retrieves the progress bar (if any) and then updates it. Once
	* the progress bar is finished with the method finishWithProgressBar() is called.
	*
	* @return 
	*/
	virtual CProgressCtrl* getProgressBar();
	
	
	   /**
	   * finishedWithProgressBar
	   * 
	   * This will be called after a progress bar has finished updating. This will only be called if
	   * the call to getProgressBar does not return NULL.
	*/
	virtual void finishedWithProgressBar();
	
	/**
	* notifyNoMatchingAlarms
	* 
	* This will be called if no alarms match the current filter. 
	*/
	virtual void notifyNoMatchingAlarms();
	//
	///////////////////////////////////////////////////////////////////////////

protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CAlarmStoreHarnessDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnBtnCfg();
	afx_msg void OnBtnCfgfile();
	afx_msg void OnBtnRebuildAlarm();
	afx_msg void OnBtnCheckShmem();
	afx_msg void OnBtnConfig();
	afx_msg void OnBtnStart();
	afx_msg void OnBtnReload();
	afx_msg void OnChangeEditMessagingInterval();
	afx_msg void OnChangeEditSortInterval();
	afx_msg void OnChangeEditFilterInterval();
	afx_msg void OnChangeEditRebuildInterval();
	afx_msg void OnBtnApply();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnBtnCloseall();
	afx_msg void OnBtnOutput();
	afx_msg void OnBtnOutput2();
	afx_msg void OnBtnSort();
	afx_msg void OnBtnFilter();
	afx_msg void OnBtnRemove();
	afx_msg void OnCheckAlarm();
	afx_msg void OnCheckFilter();
	afx_msg void OnCheckSort();
	afx_msg void OnCheckRebuild();
	afx_msg LRESULT OnUpdateAlarms(WPARAM, LPARAM);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

private:
	bool StartTest();
	bool StopTest();
	void AlarmRaiseUpdate(/*AlarmMessageDetail& alarmDetail, AlarmStoreHarnessImpl& harnessImpl*/);
	int AlarmAction( int actionType );
	void OnShowMessageAction( int actionId, const std::string &alarmId );
	void ShowActionInfo( const CString & info );
	void ShowAlarmStoreInfo( const CString & info );

	void SetNumToEdit( unsigned int num, CEdit & edit );
	unsigned int GetIntegerFromEdit(const CEdit& edit);
	std::string GetCurrentFomatedTime();

	// for alarm list control
	void initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations);
	void initListControlColumns();
	std::vector<TA_Base_Bus::IAlarmAction*> initListControlActions();

	// alarm update thread control
	void initAlarmThread();
	void startAllAlarmThread(int interval);
	void stopAllAlarmThread();
	void removeAllAlarmthread();
	void setAllAlarmThreadInterval(int interval);

	// sort/filter/rebuild thread control
// 	void initOtherThread();
// 	void startOtherThread();
// 	void stopOtherThread();
// 	void removeOtherThread();

	// constructor Sort and filter
	SORT_COLUMN getSortTypeFromCombIndex(int index);
	TA_Base_Bus::Filter buildFilterFromCombIndex(int index, bool& isDefault);
	void setSort(int index);
	void setFilter(int index);

	// rebuild alarm
	void rebuildAlarm();

	// check shared memory
	void checkShmem();
	
	// sort/filter/rebuildAlarm thread method
	void SetSortThread();
	void SetFilterThread();
	void RebuildAlarmIndexThread();

	// enable/disable component
	void enableComponent(BOOL enable, int nID);
	void enableMunualComponent(BOOL enable);
	void enableAutoConfigComponent(BOOL enable);

private:
	AlarmStoreHarnessImpl m_alarmHarness;
	bool m_hasLoadedData;
	bool m_testOnRunning;

	class CWorkThread : public TA_Base_Core::Thread
	{
	public :
		CWorkThread( CAlarmStoreHarnessDlg& parent, int type, int interval = 1000 );
		virtual ~CWorkThread();

		void run();
		void terminate();
		void setInterval(int interval);
		bool isRunning() const
		{
			return !m_terminated;
		}

	private:
		int		m_type;
		CAlarmStoreHarnessDlg& m_parent;
		bool	m_terminated;
		HANDLE	m_terminateEvent;
		HANDLE	m_setIntervalEvent;
		int		m_interval;
	};

	friend class CWorkThread;

	// thread lock
	TA_Base_Core::ReEntrantThreadLockable m_threadLock;

	// alarm raise lock
	TA_Base_Core::ReEntrantThreadLockable m_alarmRaiseLock;

	// alarm thread
	typedef std::vector<CWorkThread*> AlarmThreadVcetor;
	AlarmThreadVcetor m_threadVect;

	// sort/filter/rebuild thread
	CWorkThread* m_sortThread;
	CWorkThread* m_filterThread;
	CWorkThread* m_rebuildThread;

	std::string m_cureentAlarmId;
	
	//
	AlarmListCtrlEx* m_alarmList;
	std::vector<TA_Base_Bus::IAlarmColumn*> m_alarmColumns;
	std::string m_sessionId;
	int m_totalAlarms;
	int m_notAckAlarms;
	bool m_exceededMaxDisplayAlarm;

	// test interval
	unsigned int m_alarmInterval;
	unsigned int m_sortInterval;
	unsigned int m_filterInterval;
	unsigned int m_rebuildInterval;

	// filter value
	int m_severityFilterValue;
	int m_locationFilterValue;
	std::string m_assetFilterValue;
	std::string m_descFilterValue;

};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALARMSTOREHARNESSDLG_H__5B702FF7_E9AD_43BC_9B55_10770958A1D0__INCLUDED_)
