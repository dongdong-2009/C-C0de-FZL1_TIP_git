// AlarmStoreHarnessDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/alarm/AlarmStoreHarness/src/AlarmStoreHarness.h"
#include "app/alarm/AlarmStoreHarness/src/AlarmStoreHarnessDlg.h"

#include "bus/alarm/alarm_list_control/src/columns/ColumnAck.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnState.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSourceTimestamp.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnSeverity.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAsset.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnDescription.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnValue.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnDecisionSupport.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAvalanche.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnMms.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnComment.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnAckBy.h"

#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "bus/alarm/alarm_list_control/src/AlarmActionFactory.h"

#include "bus/alarm/alarm_list_control/src/actions/ActionDisplayComment.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSeparator.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedSchematic.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionCloseAndAckAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionIncidentLogSheet.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionJobRequest.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionPostponeDSS.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedDSS.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedPlanInstance.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedEvents.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedInspector.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedTrend.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionAcknowledgeAllAlarms.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionRelatedAvalancheEvents.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionSuppressAlarm.h"
#include "bus/alarm/alarm_list_control/src/actions/ActionEventsViewer.h"

#include "app/alarm/AlarmStoreHarness/src/configdlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


const int UPDATE_ALARM_THREAD			= 0;
const int REBUILD_ALARM_THREAD			= 1;
const int SET_SORT_THREAD				= 2;
const int SET_FILTER_THREAD				= 3;
const int START_AUTO_TEST_ACTION		= 4;
const int SET_FILTER_ACTION				= 5;
const int REBUILD_ALARM_ACTION			= 6;
const int CHECK_SHMEM_ACTION			= 7;

const int MAX_ACTION_INFO			= 65535;
const int MAX_ACTION_INFO_LINES		= 16;
const int MAX_ALARMSTORE_INFO_LINES		= 4;

#define TIMER_UPDATE_ALARM				1

#define ACTION_NEW_ALARM			0
#define ACTION_ACK_ALARM			1
#define ACTION_CLOSE_ALARM			2

/*
#define Item_ackAlarm				0
#define Item_closeAlarm				1
#define Item_updateComments			2
#define Item_updateMmsState			3
#define Item_updateAlarmValue		4
#define Item_updateAlarmValue		5
*/

using TA_Base_Core::ThreadGuard;

static const ULONG WM_UPDATE_ALARMS_MSG = WM_USER + 2;

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessDlg dialog

CAlarmStoreHarnessDlg::CAlarmStoreHarnessDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAlarmStoreHarnessDlg::IDD, pParent)
	, m_hasLoadedData(false)
	, m_testOnRunning(false)
	, m_alarmList(NULL)
	, m_totalAlarms(0)
	, m_notAckAlarms(0)
	, m_exceededMaxDisplayAlarm(false)
	, m_alarmInterval(1000)
	, m_sortInterval(5)
	, m_filterInterval(10)
	, m_rebuildInterval(5)
	, m_severityFilterValue(1)
	, m_locationFilterValue(1)
	, m_assetFilterValue("Asset")
	, m_descFilterValue("Harness Alarm")
	, m_sortThread(NULL)
	, m_filterThread(NULL)
	, m_rebuildThread(NULL)
{
	//{{AFX_DATA_INIT(CAlarmStoreHarnessDlg)
	m_cfgFileName = _T("");
	m_actionInfo = _T("");
	m_alarmStortInfo = _T("");
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	initAlarmThread();

	// init sort/filter/rebuild thread
	m_sortThread = new CAlarmStoreHarnessDlg::CWorkThread(*this,SET_SORT_THREAD, 5000);
	m_filterThread = new CAlarmStoreHarnessDlg::CWorkThread(*this,SET_FILTER_THREAD, 5000);
	m_rebuildThread = new CAlarmStoreHarnessDlg::CWorkThread(*this,REBUILD_ALARM_THREAD, 5000);
}


CAlarmStoreHarnessDlg::~CAlarmStoreHarnessDlg()
{
	if(m_sortThread != NULL)
	{
		if(m_sortThread->isRunning())
		{
			m_sortThread->terminateAndWait();
		}

		delete m_sortThread;
		m_sortThread = NULL;
	}

	if(m_filterThread != NULL)
	{
		if(m_filterThread->isRunning())
		{
			m_filterThread->terminateAndWait();
		}
		
		delete m_filterThread;
		m_filterThread = NULL;
	}

	if(m_rebuildThread != NULL)
	{
		if(m_rebuildThread->isRunning())
		{
			m_rebuildThread->terminateAndWait();
		}
		
		delete m_rebuildThread;
		m_rebuildThread = NULL;
	}

	removeAllAlarmthread();
}

void CAlarmStoreHarnessDlg::Initialize()
{
	if( m_alarmList == NULL)
	{
		m_alarmList = new AlarmListCtrlEx(false, false);
	}
}

void CAlarmStoreHarnessDlg::initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations)
{
	locations.clear();
}

void CAlarmStoreHarnessDlg::initListControlColumns()
{
	FUNCTION_ENTRY("initListControlColumns");

	//we now use nuber of characters to specify initial width of coulmns
	//for Ack By and Asset columns
	CString widestCharacter = 'W';
	unsigned long widestCharWidth = m_alarmList->GetStringWidth (widestCharacter);

	// State
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnState(80, true, NOSORT_BY_SUBSYSTEM, NULL));

	// Source timestamp
	unsigned long ulTimestampWidth = TA_Base_Bus::AlarmConstants::DEFAULT_TIMESTAMPCOLWIDTH;
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true,SORT_BY_DATETIME, NULL));
	        	
	// Severity
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnSeverity(40, true, SORT_BY_SEVERITY, NULL)); //TD15005

	// Asset
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnAsset(210, true, SORT_BY_ASSET, NULL)); //TD15005

	// Description
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(200, true, SORT_BY_DESCRIPTION, NULL)); 
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnValue(60, true, SORT_BY_VALUE, NULL));
	
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnAlarmOperatorType(24, true, SORT_BY_ALARMOPERATORTYPE, NULL));

	m_alarmColumns.push_back(new TA_Base_Bus::ColumnMms(24, true, SORT_BY_MMS, NULL));

	// DSS	
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnDecisionSupport(22, true, SORT_BY_DSS, NULL));
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnAvalanche(22, true, SORT_BY_AVALANCHE, NULL));

	m_alarmColumns.push_back(new TA_Base_Bus::ColumnAckBy(10 * widestCharWidth, true, SORT_BY_ACK, NULL));
	m_alarmColumns.push_back(new TA_Base_Bus::ColumnComment(24, true, SORT_BY_COMMENTS, NULL));

	FUNCTION_EXIT;
}

std::vector<TA_Base_Bus::IAlarmAction*> CAlarmStoreHarnessDlg::initListControlActions()
{
	FUNCTION_ENTRY("initListControlActions");
	
    std::vector<TA_Base_Bus::IAlarmAction*> actions;

	return actions; // return empty action list to make sure L/R click list control do nothing.
	
	// Add actions available from the context menu.
    actions.push_back( new TA_Base_Bus::ActionAcknowledgeAlarm() );
	actions.push_back( new TA_Base_Bus::ActionDisplayComment() );
    actions.push_back( new TA_Base_Bus::ActionSeparator() );
    actions.push_back( new TA_Base_Bus::ActionRelatedSchematic() );
    actions.push_back( new TA_Base_Bus::ActionRelatedInspector() );
    actions.push_back( new TA_Base_Bus::ActionRelatedEvents() );
    actions.push_back( new TA_Base_Bus::ActionSeparator() );
    actions.push_back( new TA_Base_Bus::ActionRelatedDSS() );
	actions.push_back( new TA_Base_Bus::ActionRelatedPlanInstance() ); 	
    actions.push_back( new TA_Base_Bus::ActionPostponeDSS() );
    actions.push_back( new TA_Base_Bus::ActionSeparator() );
    actions.push_back( new TA_Base_Bus::ActionJobRequest() );
    actions.push_back( new TA_Base_Bus::ActionRelatedTrend() );
    actions.push_back( new TA_Base_Bus::ActionRelatedAvalancheEvents() );
    actions.push_back( new TA_Base_Bus::ActionIncidentLogSheet() );
    actions.push_back( new TA_Base_Bus::ActionSeparator() );
    actions.push_back( new TA_Base_Bus::ActionCloseAndAckAlarm() );
	actions.push_back( new TA_Base_Bus::ActionSuppressAlarm() );
	
	// Add actions not available from the context menu.
	actions.push_back( new TA_Base_Bus::ActionAcknowledgeAllAlarms(this->GetSafeHwnd(),false) );
	// TD 11783 Add an Events Viewer Action to launch events viewer
	actions.push_back( new TA_Base_Bus::ActionEventsViewer(false) );
	
	FUNCTION_EXIT;
	return actions;
}

void CAlarmStoreHarnessDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAlarmStoreHarnessDlg)
	DDX_Control(pDX, IDC_COMBO_FILTER, m_comboFilter);
	DDX_Control(pDX, IDC_COMBO_SORT, m_comboSort);
	DDX_Control(pDX, IDC_COMBO_LOCATION, m_comboLocation);
	DDX_Control(pDX, IDC_EDIT_MESSAGING_INTERVAL, m_msgIntervalEdit);
	DDX_Control(pDX, IDC_EDIT_SORT_INTERVAL, m_sortIntervalEdit);
	DDX_Control(pDX, IDC_EDIT_FILTER_INTERVAL, m_filterIntervalEdit);
	DDX_Control(pDX, IDC_EDIT_REBUILD_INTERVAL, m_rebuildIntervalEdit);
	DDX_Control(pDX, IDC_BTN_START, m_testActionBtn);
	DDX_Control(pDX, IDC_ALARM_LIST, *m_alarmList);
	DDX_Text(pDX, IDC_TOTAL_ALARMS, m_totalAlarmsString);
	DDX_Text(pDX, IDC_NOT_ACK_ALARMS, m_notAckAlarmsString);
	DDX_Text(pDX, IDC_EDIT_CFGFILE, m_cfgFileName);
	DDX_Check(pDX, IDC_CHK_ALARM, m_checkAlarm);
	DDX_Check(pDX, IDC_CHK_FILTER, m_checkFilter);
	DDX_Check(pDX, IDC_CHK_SORT, m_checkSort);
	DDX_Check(pDX, IDC_CHK_REBUILD, m_checkRebuild);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAlarmStoreHarnessDlg, CDialog)
	//{{AFX_MSG_MAP(CAlarmStoreHarnessDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_CFG, OnBtnCfg)
	ON_BN_CLICKED(IDC_BTN_CFGFILE, OnBtnCfgfile)
	ON_BN_CLICKED(IDC_BTN_START, OnBtnStart)
	ON_BN_CLICKED(IDC_BTN_RELOAD, OnBtnReload)
	ON_BN_CLICKED(IDC_BTN_REBUILD_ALARM, OnBtnRebuildAlarm)
	ON_BN_CLICKED(IDC_BTN_CHECK_SM, OnBtnCheckShmem)
	ON_EN_CHANGE(IDC_EDIT_MESSAGING_INTERVAL, OnChangeEditMessagingInterval)
	ON_EN_CHANGE(IDC_EDIT_SORT_INTERVAL, OnChangeEditSortInterval)
	ON_EN_CHANGE(IDC_EDIT_FILTER_INTERVAL, OnChangeEditFilterInterval)
	ON_EN_CHANGE(IDC_EDIT_REBUILD_INTERVAL, OnChangeEditRebuildInterval)
	ON_BN_CLICKED(IDC_BTN_APPLY, OnBtnApply)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_CLOSEALL, OnBtnCloseall)
	ON_BN_CLICKED(IDC_BTN_OUTPUT, OnBtnOutput)
	ON_BN_CLICKED(IDC_BTN_OUTPUT2, OnBtnOutput2)
	ON_BN_CLICKED(IDC_BTN_SORT, OnBtnSort)
	ON_BN_CLICKED(IDC_BTN_FILTER, OnBtnFilter)
	ON_BN_CLICKED(IDC_BTN_REMOVE_LA, OnBtnRemove)
	ON_BN_CLICKED(IDC_BTN_CONFIG, OnBtnConfig)
	ON_BN_CLICKED(IDC_CHK_ALARM, OnCheckAlarm)
	ON_BN_CLICKED(IDC_CHK_FILTER, OnCheckFilter)
	ON_BN_CLICKED(IDC_CHK_SORT, OnCheckSort)
	ON_BN_CLICKED(IDC_CHK_REBUILD, OnCheckRebuild)
	//}}AFX_MSG_MAP
	ON_MESSAGE(WM_UPDATE_ALARMS_MSG, OnUpdateAlarms)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessDlg message handlers

BOOL CAlarmStoreHarnessDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon
	
	// TODO: Add extra initialization here
	try
	{
		m_alarmHarness.init();
	}
	catch (...)
	{
		MessageBox("Program launch failed. Please check if the AlarmStoreService is running");
		exit(1);
	}
	const char* cfgFileName = "./AlarmHarnessData.cfg";
	if( DataParser::getInstance().populateFromFile(cfgFileName) )
	{
		m_cfgFileName = _T(cfgFileName);
		UpdateData(FALSE);
		m_hasLoadedData = true;
	}

	SetNumToEdit(m_alarmInterval, m_msgIntervalEdit);
	SetNumToEdit(m_sortInterval, m_sortIntervalEdit);
	SetNumToEdit(m_filterInterval, m_filterIntervalEdit);
	SetNumToEdit(m_rebuildInterval, m_rebuildIntervalEdit);

	//
	m_checkAlarm = TRUE;
	enableComponent(FALSE, IDC_EDIT_SORT_INTERVAL);
	enableComponent(FALSE, IDC_EDIT_FILTER_INTERVAL);
	enableComponent(FALSE, IDC_EDIT_REBUILD_INTERVAL);
	UpdateData(FALSE);

/*	int index = 0;
	m_comboSort.AddString("NOSORT_BY_HASH");
	m_comboSort.InsertString(++index, "NOSORT_BY_SUBSYSTEM");
	*/
	m_comboSort.SetCurSel(0);
	m_comboFilter.SetCurSel(4);
	m_comboLocation.SetCurSel(1);

	// Set Session Id
    m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);

	// Initialise the Alarm List Control
	TA_Base_Bus::Filter preFilter;
	std::vector<unsigned long> locations; 
	initPreFiltering(preFilter, locations);

	TA_Base_Bus::Filter filter;

	TA_Base_Bus::AlarmActionFactory* factory = 
		    new TA_Base_Bus::AlarmActionFactory(initListControlActions());

	initListControlColumns();

	TA_ASSERT(m_alarmList != NULL, "m_alarmList hasn't been instantiated");
	m_alarmList->startAlarmList(*this, *factory, m_alarmColumns, preFilter, locations, TA_Base_Bus::COL_STATE, true, filter, m_sessionId, true);
	m_alarmHarness.setCallback(m_alarmList);
 
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAlarmStoreHarnessDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlarmStoreHarnessDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAlarmStoreHarnessDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CAlarmStoreHarnessDlg::OnBtnCfg() 
{
	// TODO: Add your control notification handler code here
// 	AlarmMessageDetail alarmDetail;
// 	AlarmStoreHarnessImpl harness;
// 	harness.setCallback(m_alarmList);
// 
// 	AlarmRaiseUpdate(alarmDetail, harness);
	AlarmRaiseUpdate();
}

void CAlarmStoreHarnessDlg::OnBtnCfgfile() 
{
	// TODO: Add your control notification handler code here
	const char* filters = "Cfg files (*.cfg)|*.cfg|All Files (*.*)|*.*||";
	CFileDialog fDlg( TRUE,".cfg",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filters );
	if( fDlg.DoModal() == IDOK )
	{
		m_cfgFileName = fDlg.GetPathName();
		DataParser::getInstance().populateFromFile( m_cfgFileName.GetBuffer( m_cfgFileName.GetLength() ) );
	}
	
	UpdateData(FALSE);	
}

void CAlarmStoreHarnessDlg::OnBtnRebuildAlarm()
{
	rebuildAlarm();
}

void CAlarmStoreHarnessDlg::OnBtnCheckShmem()
{
	checkShmem();
}

void CAlarmStoreHarnessDlg::OnCheckAlarm()
{
	UpdateData(TRUE);
}

void CAlarmStoreHarnessDlg::OnCheckFilter()
{
	UpdateData(TRUE);
	if(m_checkFilter == 1)
	{
		m_filterIntervalEdit.EnableWindow(TRUE);
	}
	else
	{
		m_filterIntervalEdit.EnableWindow(FALSE);
	}
}

void CAlarmStoreHarnessDlg::OnCheckSort()
{
	UpdateData(TRUE);
	if(m_checkSort == 1)
	{
		m_sortIntervalEdit.EnableWindow(TRUE);
	}
	else
	{
		m_sortIntervalEdit.EnableWindow(FALSE);
	}
}

void CAlarmStoreHarnessDlg::OnCheckRebuild()
{
	UpdateData(TRUE);
	if(m_checkRebuild == 1)
	{
		m_rebuildIntervalEdit.EnableWindow(TRUE);
	}
	else
	{
		m_rebuildIntervalEdit.EnableWindow(FALSE);
	}
}

void CAlarmStoreHarnessDlg::OnBtnConfig()
{
	CConfigDlg dlg(m_severityFilterValue, m_locationFilterValue, m_assetFilterValue.c_str(), m_descFilterValue.c_str());
	if( dlg.DoModal() == IDOK )
	{
		m_severityFilterValue = dlg.m_severityValue;
		m_locationFilterValue = dlg.m_locationValue;
		m_assetFilterValue = dlg.m_assetValue;
		m_descFilterValue = dlg.m_descValue;
	}
}


void CAlarmStoreHarnessDlg::OnTimer( UINT nIDEvent )
{
// 	AlarmMessageDetail alarmDetail;
// 	AlarmStoreHarnessImpl harness;
// 	harness.setCallback(m_alarmList);
// 
// 	AlarmRaiseUpdate(alarmDetail, harness);
}



int CAlarmStoreHarnessDlg::AlarmAction( int actionType )
{
	if( actionType < ACTION_NEW_ALARM || actionType > ACTION_CLOSE_ALARM )
		return -1;

	return 1;
}


void CAlarmStoreHarnessDlg::AlarmRaiseUpdate(/*AlarmMessageDetail& alarmDetail, AlarmStoreHarnessImpl& harnessImpl*/)
{
	ThreadGuard guard(m_alarmRaiseLock);

	AlarmMessageDetail alarmDetail;
	if( DataParser::getInstance().nextAlarmDetail( alarmDetail ) )
	{
		if( stricmp( alarmDetail.state.c_str(), "New" ) == 0 )
		{
			m_alarmHarness.newAlarm( alarmDetail );
			//harnessImpl.newAlarm(alarmDetail);
		}
		else if( stricmp( alarmDetail.state.c_str(), "Ack" ) == 0 )
		{
			if( !alarmDetail.alarmId.empty() && alarmDetail.alarmId.compare("empty") != 0)
			{
				m_alarmHarness.updateAlarm(Item_ackAlarm,alarmDetail);
				//harnessImpl.updateAlarm(Item_ackAlarm,alarmDetail);
			}
			else
			{
				TA_ASSERT(false, "Alarm ID Should not empty");
			}

		}
		else if( stricmp( alarmDetail.state.c_str(), "Close" ) == 0 )
		{
			if( !alarmDetail.alarmId.empty() && alarmDetail.alarmId.compare("empty") != 0)
			{
				m_alarmHarness.updateAlarm(Item_closeAlarm,alarmDetail );
				//harnessImpl.updateAlarm(Item_closeAlarm,alarmDetail );
			}
			else
			{
				TA_ASSERT(false, "Alarm ID Should not empty");
			}	
		}

		else if( stricmp( alarmDetail.state.c_str(), "Comments" ) == 0 )
		{
			if( !alarmDetail.alarmId.empty() && alarmDetail.alarmId.compare("empty") != 0)
			{
				m_alarmHarness.updateAlarm(Item_updateComments,alarmDetail );
				//harnessImpl.updateAlarm(Item_updateComments,alarmDetail );
			}
			else
			{
				TA_ASSERT(false, "Alarm ID Should not empty");
			}	
		}
		
		else if( stricmp( alarmDetail.state.c_str(), "MmsState" ) == 0 )
		{
			if( !alarmDetail.alarmId.empty() && alarmDetail.alarmId.compare("empty") != 0)
			{
				m_alarmHarness.updateAlarm(Item_updateMmsState,alarmDetail );
				//harnessImpl.updateAlarm(Item_updateMmsState,alarmDetail );
			}
			else
			{
				TA_ASSERT(false, "Alarm ID Should not empty");
			}	
		}

		else if( stricmp( alarmDetail.state.c_str(), "AlarmValue" ) == 0 )
		{
			if( !alarmDetail.alarmId.empty() && alarmDetail.alarmId.compare("empty") != 0)
			{
				m_alarmHarness.updateAlarm(Item_updateAlarmValue,alarmDetail );
				//harnessImpl.updateAlarm(Item_updateAlarmValue,alarmDetail );
			}
			else
			{
				TA_ASSERT(false, "Alarm ID Should not empty");
			}	
		}
		
	}
}


void CAlarmStoreHarnessDlg::SetSortThread()
{
	static int sortIndex = 0;
	sortIndex = sortIndex++ % 11;
	setSort(sortIndex);
}


void CAlarmStoreHarnessDlg::SetFilterThread()
{
	static int filterIndex = 0;
	filterIndex = filterIndex++ % 5;
	setFilter(filterIndex);
}


void CAlarmStoreHarnessDlg::RebuildAlarmIndexThread()
{
	rebuildAlarm();
}

void CAlarmStoreHarnessDlg::setSort(int index)
{
	if(m_alarmList != NULL)
	{
		SORT_COLUMN sort_col = getSortTypeFromCombIndex(index);
		try
		{
			m_alarmList->setSort(sort_col);
		}
		catch(...)
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "setSort caught unknown excepion");
		}
	}
}


void CAlarmStoreHarnessDlg::setFilter(int index)
{
	if(m_alarmList != NULL)
	{
		bool isDefault = false;
		TA_Base_Bus::Filter filter = buildFilterFromCombIndex(index, isDefault);
		try
		{
			m_alarmList->setFilter(filter, isDefault);
		}
		catch( TA_Base_Core::AlarmListCtrlException& ex )
		{
			LOG_EXCEPTION_CATCH(SourceInfo, "TA_Base_Core::AlarmListCtrlException", ex.what());
		}
	}
}

void CAlarmStoreHarnessDlg::rebuildAlarm()
{
	m_alarmHarness.rebuildAlarms();
}

void CAlarmStoreHarnessDlg::checkShmem()
{
	//m_alarmHarness.checkSharedMemory();
	//m_alarmHarness.removeLocationAlarms();
	long timeStamp = m_alarmList->getASTimeStamp();
}


std::string CAlarmStoreHarnessDlg::GetCurrentFomatedTime()
{
	time_t tt;
	time(&tt);
	struct tm* m = localtime(&tt);
	CString timeString;
	timeString.Format("[%02d/%02d/%04d-%02d:%02d:%02d] ",
		m->tm_mday,m->tm_mon,m->tm_year + 1900,
		m->tm_hour,m->tm_min,m->tm_sec);
	
	return timeString.GetBuffer( timeString.GetLength() );
}

void CAlarmStoreHarnessDlg::ShowActionInfo( const CString & info )
{
	//if( m_actionInfo.GetLength() >= MAX_ACTION_INFO )
	if( m_actionEdit.GetLineCount() > MAX_ACTION_INFO_LINES )
	{
		m_actionInfo.Empty();
	}

	CString newLine( GetCurrentFomatedTime().c_str() );
	newLine += info;

	//newLine += "\r\n";
	//m_actionInfo = newLine + m_actionInfo;
	m_actionInfo += newLine;
	m_actionInfo += "\r\n";

	UpdateData(FALSE);
	//m_actionEdit.SetSel( m_actionInfo.GetLength(), m_actionInfo.GetLength() );
}


void CAlarmStoreHarnessDlg::ShowAlarmStoreInfo( const CString & info )
{
	//if( m_alarmStortInfo.GetLength() >= MAX_ACTION_INFO )
	if( m_alarmStoreEdit.GetLineCount() > MAX_ALARMSTORE_INFO_LINES )
	{
		m_alarmStortInfo.Empty();
	}

	CString newLine( GetCurrentFomatedTime().c_str() );
	newLine += info;

	m_alarmStortInfo += newLine;
	m_alarmStortInfo += "\r\n";

	UpdateData(FALSE);
	//m_alarmStoreEdit.SetSel( m_alarmStortInfo.GetLength(), m_alarmStortInfo.GetLength() );
}


void CAlarmStoreHarnessDlg::OnShowMessageAction( int actionId, const std::string &alarmId )
{
	std::string act = "";
	if( actionId == 0 )
	{
		act = "[ New Alarm ]";
	}
	else if( actionId == 1 )
	{
		act = "[ Ack Alarm ]";
	}
	else if( actionId == 2 )
	{
		act = "[ Close Alarm ]";
	}
	else if( actionId == 3 )
	{
		act = "[ Update Comments ]";
	}
	else if( actionId == 4 )
	{
		act = "[ Update MssState ]";
	}
	else if( actionId == 5 )
	{
		act = "[ Update AlarmValue ]";
	}

	if( !act.empty() )
	{
		CString msg;
		msg.Format("%s  AlarmId=%s", act.c_str(), alarmId.c_str());
		ShowActionInfo( msg );
	}
}


LRESULT CAlarmStoreHarnessDlg::OnUpdateAlarms(WPARAM wParam, LPARAM lParam)
{
	m_totalAlarms = wParam;
	m_notAckAlarms = lParam;
	
	if (m_exceededMaxDisplayAlarm == true)
	{
		m_totalAlarmsString.Format("%d+", m_totalAlarms);
	}
	else
	{
		m_totalAlarmsString.Format("%d", m_totalAlarms);
	}
	
	m_notAckAlarmsString.Format("%d", m_notAckAlarms);
	UpdateData(false);
	return NULL;	// This line must be included. Everything above is application specific
}


//////////////////////////////////////
//
CAlarmStoreHarnessDlg::CWorkThread::CWorkThread( CAlarmStoreHarnessDlg& parent, int type , int interval /* =1000 */)
: m_parent(parent)
, m_type(type)
, m_terminated(true)
, m_terminateEvent(NULL)
, m_setIntervalEvent(NULL)
, m_interval(interval)
{
	m_terminateEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
	m_setIntervalEvent = CreateEvent( NULL, FALSE, FALSE, NULL );
}


CAlarmStoreHarnessDlg::CWorkThread::~CWorkThread()
{
	CloseHandle( m_terminateEvent );
	m_terminateEvent = NULL;

	CloseHandle( m_setIntervalEvent );
	m_setIntervalEvent = NULL;
}


void CAlarmStoreHarnessDlg::CWorkThread::run()
{
	m_terminated = false;
	bool needHandle = true;
	int evtCount = 2;
	HANDLE evts[2] = { m_setIntervalEvent, m_terminateEvent };
	DWORD waitResult;

	while( !m_terminated )
	{
		try
		{
			if( needHandle)
			{
				if( m_type == UPDATE_ALARM_THREAD )
				{
// 					AlarmMessageDetail alarmDetail;
// 					AlarmStoreHarnessImpl harness;
// 					harness.setCallback(m_parent.m_alarmList);
// 					m_parent.AlarmRaiseUpdate(alarmDetail, harness);
					m_parent.AlarmRaiseUpdate();
				}
				else if ( m_type == REBUILD_ALARM_THREAD )
				{
					m_parent.RebuildAlarmIndexThread();
				}
				else if ( m_type == SET_SORT_THREAD )
				{
					m_parent.SetSortThread();
				}
				else if ( m_type == SET_FILTER_THREAD )
				{
					m_parent.SetFilterThread();
				}
			}
			
			waitResult = WaitForMultipleObjects(evtCount, evts, FALSE, m_interval);
			if( waitResult == WAIT_OBJECT_0 )
			{
				needHandle = false;
			}
			else
			{
				needHandle = true;
			}
		}
		catch(...)
		{
			//TA_ASSERT(false, "Thread run method throw unknown exception.");
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError,"======== CAlarmStoreHarnessDlg::CWorkThread::run() caught an exception, [type=%d]=========", m_type);
		}
	}
}


void CAlarmStoreHarnessDlg::CWorkThread::setInterval(int interval)
{
	m_interval = interval;
	if(m_interval <= 50)
		m_interval = 50;
	SetEvent(m_setIntervalEvent);
}


void CAlarmStoreHarnessDlg::CWorkThread::terminate()
{
	m_terminated = true;
	SetEvent( m_terminateEvent );
}

////
/////////////////////////////////////////////////////////////////////
void CAlarmStoreHarnessDlg::OnBtnStart() 
{
	// TODO: Add your control notification handler code here
	CString btnText;
	m_testActionBtn.GetWindowText(btnText);

	if( btnText == "Start Test" )
	{
		if( StartTest() )
		{
			//enableMunualComponent(FALSE);
			enableAutoConfigComponent(FALSE);
			m_testActionBtn.SetWindowText( "Stop Test" );
		}
	}
	else{
		if( StopTest() )
		{
			//enableMunualComponent(TRUE);
			enableAutoConfigComponent(TRUE);
			m_testActionBtn.SetWindowText( "Start Test" );
		}
	}
}

void CAlarmStoreHarnessDlg::initAlarmThread()
{
	int alarmThreadCount = 32;
	ThreadGuard guard(m_threadLock);
// 	if(m_testOnRunning)
// 	{
// 		stopAllAlarmThread();
// 	}
// 
// 	removeAllAlarmthread();

	m_threadVect.reserve(alarmThreadCount);
	for(int i = 0; i < alarmThreadCount; ++i)
	{
		CAlarmStoreHarnessDlg::CWorkThread* alarmThread = new CAlarmStoreHarnessDlg::CWorkThread(*this,UPDATE_ALARM_THREAD);
		m_threadVect.push_back(alarmThread);
	}

// 	if(m_testOnRunning)
// 	{
// 		startAllAlarmThread(m_alarmInterval);
// 	}
}

void CAlarmStoreHarnessDlg::startAllAlarmThread(int interval)
{
	AlarmThreadVcetor::iterator iter = m_threadVect.begin();
	for(; iter != m_threadVect.end(); ++iter)
	{
		(*iter)->setInterval(interval);
		(*iter)->start();
	}
}

void CAlarmStoreHarnessDlg::stopAllAlarmThread()
{
	AlarmThreadVcetor::iterator iter = m_threadVect.begin();
	for(; iter != m_threadVect.end(); ++iter)
	{
		if((*iter)->isRunning())
		{
			(*iter)->terminateAndWait();
		}
	}
}

void CAlarmStoreHarnessDlg::removeAllAlarmthread()
{
	// stop it first
	stopAllAlarmThread();

	AlarmThreadVcetor::iterator iter = m_threadVect.begin();
	for(; iter != m_threadVect.end(); ++iter)
	{
		delete *iter;
	}
	m_threadVect.clear();
}

void CAlarmStoreHarnessDlg::setAllAlarmThreadInterval(int interval)
{
	AlarmThreadVcetor::iterator iter = m_threadVect.begin();
	for(; iter != m_threadVect.end(); ++iter)
	{
		(*iter)->setInterval(m_alarmInterval);
	}
}

bool CAlarmStoreHarnessDlg::StartTest()
{
	if( !DataParser::getInstance().hasData() )
	{
		MessageBox("No configuration data");
		return false;
	}

// 	SetTimer( TIMER_UPDATE_ALARM, m_alarmInterval, NULL );

	ThreadGuard guard(m_threadLock);
	// alarm update
	startAllAlarmThread(m_alarmInterval);

	// sort
	if(m_checkSort == 1)
	{
		TA_ASSERT(m_sortThread != NULL, "sort thread should be created");
		m_sortThread->setInterval(m_sortInterval * 1000);
		m_sortThread->start();
	}
	
	// filter
	if(m_checkFilter == 1)
	{
		TA_ASSERT(m_filterThread != NULL, "filter thread should be created");
		m_filterThread->setInterval(m_filterInterval * 1000);
		m_filterThread->start();
	}
	
	// rebuild alarm
	if(m_checkRebuild == 1)
	{
		TA_ASSERT(m_rebuildThread != NULL, "rebuild thread should be created");
		m_rebuildThread->setInterval(m_rebuildInterval * 1000);
		m_rebuildThread->start();
	}

	m_testOnRunning = true;

	return true;
}


bool CAlarmStoreHarnessDlg::StopTest()
{
	ThreadGuard guard(m_threadLock);
	// sort
	if(m_checkSort == 1)
	{
		TA_ASSERT(m_sortThread != NULL, "sort thread should be created");
		m_sortThread->terminateAndWait();
	}
	
	// filter
	if(m_checkFilter == 1)
	{
		TA_ASSERT(m_filterThread != NULL, "filter thread should be created");
		m_filterThread->terminateAndWait();
	}
	
	// rebuild alarm
	if(m_checkRebuild == 1)
	{
		TA_ASSERT(m_rebuildThread != NULL, "rebuild thread should be created");
		m_rebuildThread->terminateAndWait();
	}

	// alarm update
	stopAllAlarmThread();

	m_testOnRunning = false;
// 	KillTimer( TIMER_UPDATE_ALARM );

	return true;
}


void CAlarmStoreHarnessDlg::OnBtnReload() 
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if( m_cfgFileName.IsEmpty() || !DataParser::getInstance().populateFromFile( m_cfgFileName.GetBuffer( m_cfgFileName.GetLength() ) ) )
	{
		MessageBox("Please check the file name of configuaration data inputed by you");
	}
}


void CAlarmStoreHarnessDlg::SetNumToEdit( unsigned int num, CEdit & edit )
{
	CString text;
	text.Format( "%li", num );
	edit.SetWindowText( text );
}


unsigned int CAlarmStoreHarnessDlg::GetIntegerFromEdit(const CEdit& edit)
{
	CString iText;
	edit.GetWindowText( iText );
	
	unsigned int interval = 0;
	try
	{
		interval = atoi( iText.GetBuffer(iText.GetLength()) );
	}
	catch( ... )
	{
		MessageBox( "Please enter integer" );
	}
	
	return interval;
}

void CAlarmStoreHarnessDlg::OnChangeEditMessagingInterval() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	UpdateData();
	m_alarmInterval = GetIntegerFromEdit(m_msgIntervalEdit);
	UpdateData(FALSE);
}

void CAlarmStoreHarnessDlg::OnChangeEditSortInterval() 
{
	UpdateData();
	m_sortInterval = GetIntegerFromEdit(m_sortIntervalEdit);
	UpdateData(FALSE);
}

void CAlarmStoreHarnessDlg::OnChangeEditFilterInterval()
{
	UpdateData();
	m_filterInterval = GetIntegerFromEdit(m_filterIntervalEdit);
	UpdateData(FALSE);
}

void CAlarmStoreHarnessDlg::OnChangeEditRebuildInterval()
{
	UpdateData();
	m_rebuildInterval = GetIntegerFromEdit(m_rebuildIntervalEdit);
	UpdateData(FALSE);
}


void CAlarmStoreHarnessDlg::OnBtnApply() 
{
	// TODO: Add your control notification handler code here
	//m_messageThread->setInterval( m_sendMsgInterval );

	SetNumToEdit(m_alarmInterval, m_msgIntervalEdit);

	SetNumToEdit(m_sortInterval, m_sortIntervalEdit);
	SetNumToEdit(m_filterInterval, m_filterIntervalEdit);
	SetNumToEdit(m_rebuildInterval, m_rebuildIntervalEdit);

	if( m_testOnRunning )
	{
// 		KillTimer( TIMER_UPDATE_ALARM );
// 		SetTimer( TIMER_UPDATE_ALARM, m_alarmInterval, NULL );
		setAllAlarmThreadInterval(m_alarmInterval);

		// sort
		TA_ASSERT(m_sortThread != NULL, "sort thread should be created");
		m_sortThread->setInterval(m_sortInterval * 1000);

		// filter
		TA_ASSERT(m_filterThread != NULL, "filter thread should be created");
		m_filterThread->setInterval(m_filterInterval * 1000);

		// rebuild
		TA_ASSERT(m_rebuildThread != NULL, "rebuild thread should be created");
		m_rebuildThread->setInterval(m_rebuildInterval * 1000);
	}
}


void CAlarmStoreHarnessDlg::OnBtnCloseall() 
{
// 	std::map< std::string, AlarmMessageDetail >::iterator iter = m_openAlarms.begin();
// 	while( iter != m_openAlarms.end() )
// 	{
// 		/*
// 		m_alarmHarness.ackAlarm( iter->second );
// 		m_alarmHarness.closeAlarm( iter->second );
// 		*/
// 		m_alarmHarness.updateAlarm( Item_ackAlarm, iter->second );
// 		m_alarmHarness.updateAlarm( Item_closeAlarm, iter->second );
// 		++iter;
// 	}

	if(m_alarmList != NULL)
	{
		displayVec vec = m_alarmList->getAllVisibleAlarmsEx();

		for(int i = 0; i < (int)vec.size(); ++i)
		{
			AlarmMessageDetail alarmDetail;
			alarmDetail.alarmId = vec[i]->alarmID;

			//m_alarmHarness.updateAlarm(Item_ackAlarm, alarmDetail);
			m_alarmHarness.updateAlarm(Item_closeAlarm, alarmDetail);
		}
	}
}

void CAlarmStoreHarnessDlg::OnBtnOutput() 
{
// 	// TODO: Add your control notification handler code here
// 	//const char* filters = "txt files (*.txt)|*.txt|All Files (*.*)|*.*||";
// 	const char* filters = "html files (*.htm)|*.htm|All Files (*.*)|*.*||";
// 	CFileDialog fDlg( FALSE,".htm",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filters );
// 	if( fDlg.DoModal() == IDOK )
// 	{
// 		std::string fileName = fDlg.GetPathName();
// 		displayVec vec;
// 
// 		std::string tmp;
// 		int index = m_comboFilter.GetCurSel();
// 		//m_alarmHarness.setFilter(index,tmp);
// 		index = m_comboSort.GetCurSel();
// 		m_alarmHarness.getAlarmsWithDetail( &vec,index );
// 		DataOutputSaver::Instance().OutputResultHTML(fileName, &vec,false);
// 		//DataOutputSaver::Instance().OutputResultTxt(fileName, &vec);
// 	}


}

void CAlarmStoreHarnessDlg::OnBtnOutput2() 
{
// 	// TODO: Add your control notification handler code here
// 	// TODO: Add your control notification handler code here
// 	//const char* filters = "txt files (*.txt)|*.txt|All Files (*.*)|*.*||";
// 	const char* filters = "html files (*.htm)|*.htm|All Files (*.*)|*.*||";
// 	CFileDialog fDlg( FALSE,".htm",NULL,OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,filters );
// 	if( fDlg.DoModal() == IDOK )
// 	{
// 		std::string fileName = fDlg.GetPathName();
// 		displayVec vec;
// 
// 		std::string tmp;
// 		int index = m_comboFilter.GetCurSel();
// 		//m_alarmHarness.setFilter(index,tmp);
// 		index = m_comboSort.GetCurSel();
// 		m_alarmHarness.getAlarmsWithDetail( &vec,index );
// 		DataOutputSaver::Instance().OutputResultHTML(fileName, &vec,true);
// 		//DataOutputSaver::Instance().OutputResultTxt(fileName, &vec);
// 	}
}


SORT_COLUMN CAlarmStoreHarnessDlg::getSortTypeFromCombIndex(int index)
{
	SORT_COLUMN sort = SORT_BY_DEFAULT;
	switch( index )
	{
	case 0:
		sort = SORT_BY_DEFAULT;
		break;
		
	case 1:
		sort = SORT_BY_DATETIME;
		break;
		
	case 2:
		sort = SORT_BY_SEVERITY;
		break;
		
	case 3:
		sort = SORT_BY_ASSET;
		break;
		
	case 4:
		sort = SORT_BY_DESCRIPTION;
		break;
		
	case 5:
		sort = SORT_BY_VALUE;
		break;
		
	case 6:
		sort = SORT_BY_MMS;
		break;
		
	case 7:
		sort = SORT_BY_DSS;
		break;
		
	case 8:
		sort = SORT_BY_AVALANCHE;
		break;
		
	case 9:
		sort = SORT_BY_ACK;
		break;
		
	case 10:
		sort = SORT_BY_COMMENTS;
		break;
		
	default:
		sort = SORT_BY_DEFAULT;
		break;
	}

	return sort;
}


TA_Base_Bus::Filter CAlarmStoreHarnessDlg::buildFilterFromCombIndex(int index, bool& isDefault)
{
	isDefault = false;
	TA_Base_Bus::Filter filter;

	switch ( index )
	{
		// severity
	case 0:
		filter.addFilter( TA_Base_Bus::Filter::FilterType::FILTER_SEVERITY, 
			TA_Base_Bus::Filter::FilterComparison::COMPARE_EQUAL, m_severityFilterValue );
		break;
		
		// location
	case 1:
		filter.addFilter( TA_Base_Bus::Filter::FilterType::FILTER_LOCATION, 
			TA_Base_Bus::Filter::FilterComparison::COMPARE_EQUAL, m_locationFilterValue );
		break;
		
		// asset name
	case 2:
		filter.addFilter( TA_Base_Bus::Filter::FilterType::FILTER_ASSET_NAME, 
			TA_Base_Bus::Filter::FilterComparison::COMPARE_EQUAL, m_assetFilterValue );
		break;
		
		// description
	case 3:
		filter.addFilter( TA_Base_Bus::Filter::FilterType::FILTER_DESCRIPTION, 
			TA_Base_Bus::Filter::FilterComparison::COMPARE_EQUAL, m_descFilterValue );
		break;

		// clear/default
	case 4:
		isDefault = true;
	}

	return filter;
}


void CAlarmStoreHarnessDlg::OnBtnSort()
{
	setSort(m_comboSort.GetCurSel());
}


void CAlarmStoreHarnessDlg::OnBtnFilter()
{
	setFilter(m_comboFilter.GetCurSel());
}

void CAlarmStoreHarnessDlg::OnBtnRemove()
{
	m_alarmHarness.removeLocationAlarms(m_comboLocation.GetCurSel());
}

void CAlarmStoreHarnessDlg::enableComponent(BOOL enable, int nID)
{
	GetDlgItem(nID)->EnableWindow(enable);
}

void CAlarmStoreHarnessDlg::enableMunualComponent(BOOL enable)
{
	enableComponent(enable,IDC_COMBO_SORT);
	enableComponent(enable,IDC_COMBO_FILTER);
	enableComponent(enable,IDC_BTN_SORT);
	enableComponent(enable,IDC_BTN_FILTER);
	enableComponent(enable,IDC_BTN_REBUILD_ALARM);
	enableComponent(enable,IDC_BTN_CFG);
	enableComponent(enable,IDC_BTN_CLOSEALL);
	enableComponent(enable, IDC_BTN_CHECK_SM);
}

void CAlarmStoreHarnessDlg::enableAutoConfigComponent(BOOL enable)
{
	enableComponent(enable, IDC_CHK_FILTER);
	enableComponent(enable, IDC_CHK_SORT);
	enableComponent(enable, IDC_CHK_REBUILD);

	if(enable == 1)
	{
		if(m_checkAlarm == 1)
		{
			enableComponent(enable, IDC_EDIT_MESSAGING_INTERVAL);
		}

		if(m_checkSort == 1)
		{
			enableComponent(enable, IDC_EDIT_SORT_INTERVAL);
		}

		if(m_checkFilter == 1)
		{
			enableComponent(enable, IDC_EDIT_FILTER_INTERVAL);
		}
		
		if(m_checkRebuild == 1)
		{
			enableComponent(enable, IDC_EDIT_REBUILD_INTERVAL);
		}
	}
	else
	{
		enableComponent(enable, IDC_EDIT_MESSAGING_INTERVAL);
		enableComponent(enable, IDC_EDIT_SORT_INTERVAL);
		enableComponent(enable, IDC_EDIT_FILTER_INTERVAL);
		enableComponent(enable, IDC_EDIT_REBUILD_INTERVAL);
	}
}

////////////////////////////////////////////////////////////
// TA_Base_Bus::IAlarmListObserver interface implementationr

void CAlarmStoreHarnessDlg::updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms)
{
	//We must send a message to itself to update the total Alarms counter. 
	//Handling it here directly will cause an unnecessary assert to occur when
	//UpdateData(false) is called. This is an MFC bug, Microsoft Knowledge Base Article - 192853.
	m_exceededMaxDisplayAlarm = exceededMaxDisplayAlarm;
	
	if (isLoadingAlarms == true)
	{
		// Display wait ie. hour-glass cursor while loading
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_WAIT));
	}
	else
	{
		// Display normal ie. Arrow cursor after loading
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}
	
	PostMessage(WM_UPDATE_ALARMS_MSG, totAlarms, totUnacknowledgedAlarms);
}

void CAlarmStoreHarnessDlg::updateAlarmSound(unsigned long severity, bool wereAlarmsAdded)
{

}

void CAlarmStoreHarnessDlg::updateAvailableAlarmActions(const ItemAvailability& availability)
{

}

void CAlarmStoreHarnessDlg::setCurrentDescriptionText(const CString& description)
{

}

CProgressCtrl* CAlarmStoreHarnessDlg::getProgressBar()
{
	return NULL;
}

void CAlarmStoreHarnessDlg::finishedWithProgressBar()
{

}

void CAlarmStoreHarnessDlg::notifyNoMatchingAlarms()
{

}
//
////////////////////////////////////////////////////////////
