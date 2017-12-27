/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File$
 * @author:  Andy Siow
 * @version: $Revision$
 *
 * Last modification: $DateTime$
 * Last modified by:  $Author$
 * 
 * AlarmsGuiDlg.cpp : implementation file
 */

#include "stdafx.h"
#include <cmath>
#include <process.h>
#include <cstdlib>

#include "core\alarm\IDL\src\AlarmAgentCorbaDef.h"
#include "core\corba\src\CorbaUtil.h"
#include "core\utilities\src\DebugUtil.h" 
#include "core\utilities\src\RunParams.h"
#include "core\naming\src\Naming.h"
#include "core\data_access_interface\src\Operator.h"
#include "core\data_access_interface\src\OperatorAccessFactory.h"
#include "core\data_access_interface\entity_access\src\ConfigEntity.h"
#include "core\data_access_interface\entity_access\src\ConsoleAccessFactory.h"
#include "core\data_access_interface\entity_access\src\Console.h"
#include "core\data_access_interface\entity_access\src\EntityData.h"
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\src\ProfileAccessFactory.h"
#include "core\data_access_interface\src\IProfile.h"
#include "core\data_access_interface\src\AlarmTypeData.h"
#include "core\data_access_interface\src\AlarmTypeAccessFactory.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/data_access_interface/src/NamedFilter.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"
#include "core\exceptions\src\AssertException.h"
#include "core\exceptions\src\AuthenticationAgentException.h"
#include "core\exceptions\src\UserSettingsException.h"
#include "core\exceptions\src\ApplicationException.h"
#include "core\exceptions\src\ObjectResolutionException.h"
#include "core\synchronisation\src\ThreadGuard.h"
#include "core/utilities/src/TAAssert.h"
#include "core\data_access_interface\entity_access\src\AlarmGUI.h"
#include "core\utilities\src\Timer.h"
#include "core\message\src\MessageConfig.h"
#include "core/utilities/src/DateFormat.h" // TD12474 


#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/SystemControllerGroupUpdateSubscriber.h"
#include "bus\application_types\src\apptypes.h"
#include "bus\generic_gui\src\GenericGUI.h"
#include "bus/mfc_extensions/src/print_list_ctrl/ListCtrlPrint.h"
#include "bus\generic_gui\src\AppLauncher.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"

#include "bus/alarm/alarm_common/src/AlarmConstants.h"
#include "bus/alarm/alarm_list_control/src/AlarmActionFactory.h"
#include "bus/alarm/alarm_list_control/src/Filter.h"
#include "bus/alarm/alarm_list_control/src/Sort.h"
#include "bus/alarm/alarm_list_control/src/columns/ColumnIdentifiers.h"
#include "bus/alarm/alarm_list_control/src/IAlarmAction.h"
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

#include "app/alarm/alarm_manager/src/ExportAlarms.h"
#include "app/alarm/alarm_manager/src/Resource.h"
#include "app/alarm/alarm_manager/src/Globals.h"
#include "app/alarm/alarm_manager/src/SuppressionRulesDialog.h"
#include "app/alarm/alarm_manager/src/AlarmViewerGUI.h"
#include "app\alarm\alarm_manager\src\AlarmsGui.h"
#include "app\alarm\alarm_manager\src\AlarmsGuiDlg.h"

#include <algorithm>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Bus::TransActiveDialog;
using TA_Base_Bus::Sort;

//const CString DATE_TIME_FORMAT = "%d/%m/%Y %H:%M:%S"; //TD12474 
const CString EQUIPMENT_NAME = "Equipment";
const CString DATAPOINT_NAME = "DataPoint";
const CString REFRESH_PARAM = "RefreshRate";
const CString BLINK_FACTOR = "BlinkFactor";

const std::string CAlarmsGuiDlg::RPARAM_FILTERONASSET = "FilterOnAsset";
const std::string CAlarmsGuiDlg::RPARAM_FILTERONENTITY = "FilterOnEntity";
const std::string CAlarmsGuiDlg::RPARAM_XPOS = "AppXpos";
const std::string CAlarmsGuiDlg::RPARAM_YPOS = "AppYpos";
const std::string CAlarmsGuiDlg::RPARAM_HEIGHT = "AppHeight";
const std::string CAlarmsGuiDlg::RPARAM_WIDTH = "AppWidth";

//const std::string CAlarmsGuiDlg::NO_MATCHING_ALARMS_MESSAGE = "No alarms match the current filter";
const std::string CAlarmsGuiDlg::NO_MATCHING_ALARMS_MESSAGE = "没有匹配当前滤器的警报";
 
const int CAlarmsGuiDlg::ALARM_DATA_INITIALIZATION_TIMER = 1;  //TD17081

const int CAlarmsGuiDlg::UPDATE_TIMER = 2; 

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiDlg dialog

//CAlarmsGuiDlg::CAlarmsGuiDlg(CWnd* pParent /*=NULL*/)
CAlarmsGuiDlg::CAlarmsGuiDlg(TA_Base_Bus::IGUIAccess* controlClass)
    : m_controlClass(controlClass), 
	  m_alarmList(true),
      m_profileId(-1),
	  m_exceededMaxDisplayAlarm(false),
	  m_isLoadingAlarms(false),
      TransActiveDialog(*controlClass, CAlarmsGuiDlg::IDD, NULL),
	  m_ptrSettingsMgr(0),
	  m_isToolBarVisible(true),
	  m_hasAlarmListInit(false),
      m_groupUpdateSubscriber(NULL)
{
    //{{AFX_DATA_INIT(CAlarmsGuiDlg)
	m_totIntAlarms = 0;
	m_totUnacknowledged = 0;
	m_totAlarms = "0";
	//}}AFX_DATA_INIT
    // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
    m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    m_boldFont = NULL; 

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = 300;
    properties.minWidth = 500;
    setResizingProperties(properties);
/*//TD18095, jianghp
        setProfileId();
    
    	m_alarmFilterDlg = new TA_Base_App::AlarmFilterDlg(m_alarmList, this);
    
        m_groupUpdateSubscriber = new TA_Base_Bus::SystemControllerGroupUpdateSubscriber(*this);
    */
	// TD 13994 Reset the col width
	m_printoutColWidths = NULL;
}

// Destructor
CAlarmsGuiDlg::~CAlarmsGuiDlg()
{
	// Refer to OnDestroy. Most cleanup code is in there.

	// destroy font object
	m_font.DeleteObject();
	m_alarmfont.DeleteObject();
}

const int CAlarmsGuiDlg::ALARM_SOUND_TIMER = 1;

void CAlarmsGuiDlg::DoDataExchange(CDataExchange* pDX)
{
    TransActiveDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CAlarmsGuiDlg)
	DDX_Control(pDX, IDC_ALARM_LIST, m_alarmList);
	DDX_Control(pDX, IDC_MENU_LINE, m_MenuLine);
    DDX_Control(pDX, IDC_PROGRESS, m_progress);
    DDX_Control(pDX, IDC_STATUSBAR, m_statusBar); 
    DDX_Control(pDX, IDC_NAMED_FILTER, m_currentFilterListCtrl);
	DDX_Control(pDX, IDC_STATIC_CURRENT_FILTER, m_currentFilterText);
	DDX_Control(pDX, IDC_STATIC_TOTALARMS, m_totalAlarms);
	DDX_Control(pDX, IDC_STATIC_NOTACK, m_totalUnack);
	DDX_Control(pDX, IDC_TOTAL_ALARMS, m_totalAlarmsView);
	DDX_Control(pDX, IDC_NOT_ACKED, m_totalUnackEdit);
	DDX_Text(pDX, IDC_TOTAL_ALARMS, m_totAlarms);
	DDV_MinMaxUInt(pDX, m_totIntAlarms, 0, 4294967295);
	DDX_Text(pDX, IDC_NOT_ACKED, m_totUnacknowledged);
	DDV_MinMaxUInt(pDX, m_totUnacknowledged, 0, 4294967295);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CAlarmsGuiDlg, TransActiveDialog)
    //{{AFX_MSG_MAP(CAlarmsGuiDlg)
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_SIZE()
	ON_WM_MOVE() 
    ON_WM_CLOSE()
    ON_MESSAGE(WM_ON_CHANGE_MSG, OnChangeMsg)
	ON_WM_COMPAREITEM()
	ON_WM_DESTROY()
	ON_COMMAND(ID_FILE_PRINT, OnPrint)
	ON_COMMAND(IDC_PRINT, OnPrint)
	ON_COMMAND(ID_FILE_EXPORT, OnExport)
	ON_COMMAND(ID_FILE_EXIT, OnClose)
	ON_COMMAND(IDC_EXPORT, OnExport)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_VIEW_SHOWFILTERS, OnViewShowFilters)
	ON_COMMAND(IDC_SHOW_FILTERS, OnViewShowFilters)
	ON_COMMAND(ID_AM_ALARM_ACKNOWLEDGE, OnAlarmAcknowledge)
	ON_COMMAND(IDC_ACK, OnAlarmAcknowledge)
	ON_COMMAND(ID_AM_ALARM_VIEW_COMMENT, OnAlarmDisplayComment)
	ON_COMMAND(IDC_COMMENT, OnAlarmDisplayComment)
	ON_COMMAND(ID_ALARM_ALL_ACKNOWLEDGE, OnAlarmAllAcknowledge)
	ON_COMMAND(IDC_ACKALL, OnAlarmAllAcknowledge)
    ON_COMMAND(IDC_INHIBIT_ALARMS, OnViewInhibitAlarms)
    ON_COMMAND(IDC_SHOW_RULES, OnViewInhibitions)
	ON_COMMAND(ID_AM_ALARM_MANUAL_CLOSE, OnAlarmClose)
	ON_COMMAND(IDC_CLOSE_ALARM, OnAlarmClose)
	ON_COMMAND(ID_ALARM_ADDCOMMENT, OnAlarmDisplayComment)
	ON_COMMAND(IDC_COMMENT, OnAlarmDisplayComment)
	ON_COMMAND(ID_AM_ALARM_SCHEMATIC, OnAlarmSchematic)
	ON_COMMAND(ID_AM_ALARM_INSPECTOR, OnAlarmInspector)
	ON_COMMAND(ID_AM_ALARM_PLAN, OnAlarmPlan)
	//TD 13825 Postpone Dss function is not added to the menu
	ON_COMMAND(ID_AM_ALARM_PLAN_INSTANCE, OnAlarmPlanInstance)
	ON_COMMAND(ID_AM_CANCEL_ALARM_PLAN, OnAlarmCancelPlan)
	ON_COMMAND(ID_AM_ALARM_TREND, OnAlarmTrend)
	ON_COMMAND(ID_AM_ALARM_AVALANCHE_EVENTS, OnRelatedAvalancheEvents)
	ON_COMMAND(ID_AM_ALARM_JOB_REQUEST, OnAlarmJobRequest)
	ON_COMMAND(ID_AM_ALARM_EVENT_LOG, OnAlarmEventLog)
	ON_COMMAND(ID_AM_ALARM_EVENTS, OnAlarmEvents)
    ON_COMMAND(ID_AM_INHIBIT_BY_TYPE, OnViewInhibitAlarmByType)
    ON_COMMAND(ID_AM_INHIBIT_BY_DESCRIPTION, OnViewInhibitAlarmByDescription)
	//TD 11783 Event Viewer button is always enabled via toolbar, and will launch EventViewer
	ON_COMMAND(IDC_RELATED_EVENTS, OnEventsViewer)
	ON_COMMAND(ID_VIEW_STATUS_BAR, OnViewStatusBar)
	ON_COMMAND(ID_VIEW_TOOLBAR, OnViewToolbar)
    ON_COMMAND(ID_VIEW_SHOWINHIBITIONS, OnViewInhibitions)
	ON_COMMAND(IDC_DEFAULT_SORT, OnDefaultSort)
	ON_WM_COMPAREITEM()
	ON_CBN_SELCHANGE(IDC_NAMED_FILTER, OnSelchangeNamedFilter)
    ON_MESSAGE(WM_ON_FILTER_DLG_APPLY,OnFilterApply)
    ON_MESSAGE(WM_ON_FILTER_DLG_DELETE,OnFilterDelete)
    ON_MESSAGE(WM_ON_SYSTEM_DEGRADED,OnSystemInDegradedMode)
    ON_MESSAGE(WM_ON_SYSTEM_RECOVERED,OnSystemInNormalMode)
	ON_WM_TIMER()
	//TD18095,jianghp
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_WM_CLOSE()
	//TD18095,jianghp
	//[[add by wuzhongyi for popup comment dialog when double click alarmlist comment field
	ON_NOTIFY(NM_DBLCLK, IDC_ALARM_LIST, OnClickAlarmList)
	//]]
	//}}AFX_MSG_MAP
END_MESSAGE_MAP() 

/////////////////////////////////////////////////////////////////////////////
// CAlarmsGuiDlg message handlers


BOOL CAlarmsGuiDlg::OnInitDialog() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnInitDialog" );
    TransActiveDialog::OnInitDialog();

	// The following is required for common controls. An example of such a control is the pager control.
	INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PAGESCROLLER_CLASS|ICC_BAR_CLASSES;
    ::InitCommonControlsEx(&icex);

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);            // Set big icon
    SetIcon(AfxGetApp()->LoadIcon(IDR_MAINFRAME), FALSE);        // Set small icon

	//to disable the control for temparory loading...
	GetDlgItem(IDC_PAGER_CTRL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_CURRENT_FILTER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NAMED_FILTER)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_NOTACK)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_STATIC_TOTALARMS)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_NOT_ACKED)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_TOTAL_ALARMS)->ShowWindow(SW_HIDE);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnInitDialog" );
    return TRUE;  // return TRUE  unless you set the focus to a control
}

//TD17081
/* lsx
void CAlarmsGuiDlg::OnTimer(UINT nIDEvent)
{
	if (nIDEvent == ALARM_DATA_INITIALIZATION_TIMER && m_hasAlarmListInit == false)
	{	
		initializeAlarmList();
		KillTimer(ALARM_DATA_INITIALIZATION_TIMER);
	}
	else if (nIDEvent == ALARM_DATA_INITIALIZATION_TIMER)
	{
		KillTimer(ALARM_DATA_INITIALIZATION_TIMER);	
	}
}
*/
// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CAlarmsGuiDlg::OnPaint()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnPaint" );
    if (IsIconic())
    {
        CPaintDC dc(this); // device context for painting

        SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0); 

        // Center icon in client rectangle
        int cxIcon = GetSystemMetrics(SM_CXICON);
        int cyIcon = GetSystemMetrics(SM_CYICON);
        CRect rect;
        GetClientRect(&rect);
        int alarmItem = (rect.Width() - cxIcon + 1) / 2;
        int y = (rect.Height() - cyIcon + 1) / 2;

        // Draw the icon
        dc.DrawIcon(alarmItem, y, m_hIcon);
    }
    else
    {
        TransActiveDialog::OnPaint();
    }
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnPaint" );
}


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CAlarmsGuiDlg::OnQueryDragIcon()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnQueryDragIcon" );
    
	return (HCURSOR) m_hIcon;

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnQueryDragIcon" );
}


void CAlarmsGuiDlg::PopulateAlarmToolbar()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "PopulateAlarmToolbar" );

	GetDlgItem(IDC_PAGER_CTRL)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_CURRENT_FILTER)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_NAMED_FILTER)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_NOTACK)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_STATIC_TOTALARMS)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_NOT_ACKED)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_TOTAL_ALARMS)->ShowWindow(SW_SHOW);
	
	Invalidate();



    // Set Toolbar properties
    CWinApp*    pApp = AfxGetApp();
    UINT        tbarStyle;

    tbarStyle = WS_VISIBLE | WS_CHILD;
    tbarStyle = tbarStyle | CCS_NODIVIDER;
    tbarStyle = tbarStyle | CCS_NOPARENTALIGN;
    tbarStyle = tbarStyle | CCS_NORESIZE;
    tbarStyle = tbarStyle | TBSTYLE_TOOLTIPS;
    tbarStyle = tbarStyle | TBSTYLE_FLAT;
    tbarStyle = tbarStyle | TBSTYLE_TRANSPARENT;

    m_alarmsTbImageListEnabled.Create(16,16, ILC_MASK, 10, 20);
    m_alarmsTbImageListHot.Create(16,16, ILC_MASK, 10, 20);

	// We now have too many buttons for this Gui to display them all in the
	// toolbar at once, even when the window is maximised. So we create a
	// pager control for the toolbar to allow the user to scroll left or right
	// to allow access to all the buttons.
	CRect tmpRect;
	CWnd::GetDlgItem(IDC_PAGER_CTRL)->GetWindowRect(tmpRect);
	CWnd::ScreenToClient(tmpRect);

	CWnd::GetDlgItem(IDC_PAGER_CTRL)->DestroyWindow();

	// Create a Pager Control
	if (!m_Pager.Create(WC_PAGESCROLLER, _T(""),WS_CHILD | WS_VISIBLE | WS_TABSTOP, tmpRect, this, IDC_PAGER_CTRL))
	{
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040025);
	}

	if (!m_alarmsToolbar.Create(tbarStyle | TBSTYLE_FLAT, CRect(0,0,0,0), &m_Pager, IDC_ALARMS_TOOLBAR))
	{
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040026);
	}
    m_alarmsToolbar.SetFont(&m_font,FALSE);


	Pager_SetChild(m_Pager.m_hWnd, m_alarmsToolbar.m_hWnd);
	m_Pager.ModifyStyle(0,PGS_HORZ); // We want scrolling horizontally
	Pager_SetButtonSize(m_Pager.m_hWnd, 15);

	m_alarmsToolbar.SetButtonWidth(15, 85); // Was 75

	int buttonCount = m_alarmsToolbar.GetButtonCount();
	TBBUTTON		tbButton;

	for(int i(buttonCount - 1); i >= 0; --i)
	{
		m_alarmsToolbar.DeleteButton(i);
	}

    // Add Acknowledge button
    VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("确认")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_ACKNOWLEDGE));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_ACKNOWLEDGE_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;//| TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_ACK;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));
	
	//Add Default Sort button
	VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("默认排序")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_SORT));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_SORT_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_DEFAULT_SORT;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

	//Add Manual Close Button.
	VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("消除报警")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_MANUAL_CLOSE));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_MANUAL_CLOSE_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_CLOSE_ALARM;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

	//Add Filters button
    VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("过滤器")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_APPLY_FILTERS));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_APPLY_FILTERS_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON; //| TBSTYLE_CHECK// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_SHOW_FILTERS;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    //Add Suppress button
    VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("禁止")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_SUPPRESS));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_SUPPRESS_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; //| BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_INHIBIT_ALARMS;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    //Add Show Supp button
    VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("显示禁止项")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_SHOW_RULES));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_SHOW_RULES_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; //| BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_SHOW_RULES;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));
    
	//Add Ack All button
	VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("确认全部")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_DELETE));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_DELETE_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_ACKALL;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    // Add Separator
    tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_SEP;// | TBSTYLE_DROPDOWN; // | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = 0;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

	// Add Print button
	VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("导出")) != -1);
	tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_SAVE));
	m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_SAVE_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
    tbButton.dwData = 0;
	tbButton.idCommand = IDC_EXPORT;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    // Add Separator
   /* tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_SEP;// | TBSTYLE_DROPDOWN; // | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = 0;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));*/

	// Add Print button
	VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("打印")) != -1);
	tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_PRINT));
	m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_PRINT_HOT));
	tbButton.fsState = TBSTATE_ENABLED;
	tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE | BTNS_SHOWTEXT;// | TBSTYLE_DROPDOWN;
	tbButton.dwData = 0;
	tbButton.idCommand = IDC_PRINT;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    // Add Comment button
    VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("注释")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_COMMENT));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_COMMENT_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON;// | TBSTYLE_AUTOSIZE
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_COMMENT;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    // Add Separator
    /*tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_SEP;// | TBSTYLE_DROPDOWN; // | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = 0;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));*/

    // Add Event Viewer button
    VERIFY((tbButton.iString = m_alarmsToolbar.AddStrings("事件")) != -1);
    tbButton.iBitmap = m_alarmsTbImageListEnabled.Add(pApp->LoadIcon(IDI_EVENT_VIEWER_COLD));
    m_alarmsTbImageListHot.Add(pApp->LoadIcon(IDI_EVENT_VIEWER_HOT));
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE ;// | TBSTYLE_AUTOSIZE
    tbButton.dwData = 0;
    tbButton.idCommand = IDC_RELATED_EVENTS;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    // Add Separator
    tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_SEP;// | TBSTYLE_DROPDOWN; // | TBSTYLE_AUTOSIZE;
    tbButton.dwData = 0;
    tbButton.idCommand = 0;
    VERIFY(m_alarmsToolbar.AddButtons(1, &tbButton));

    m_alarmsToolbar.SetImageList(&m_alarmsTbImageListEnabled);
    m_alarmsToolbar.SetHotImageList(&m_alarmsTbImageListHot);
	
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "PopulateAlarmToolbar" );
}

BOOL CAlarmsGuiDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult)
{

    #define lpnm   ((LPNMHDR)lParam)
    #define lpnmTB ((LPNMTOOLBAR)lParam)
	LPNMPGCALCSIZE   pCalcSize = (LPNMPGCALCSIZE)lParam;

	CRect buttonRect;
	int buttonCount;

    if ( (lpnm->code == HDN_DIVIDERDBLCLICKA) ||
		      (lpnm->code == HDN_DIVIDERDBLCLICKW) )
    {
		TRACE("Double Click on Column Divider. Column = %d\n", lpnmTB->iItem);
		//ResizeColumn(lpnmTB->iItem);
		return FALSE;
	}
	// This needs to be here for the arrows of the pager control to appear
	else if(lpnm->code == PGN_CALCSIZE)
	{
		
		buttonCount = m_alarmsToolbar.GetButtonCount();

		// Determine size of child
		m_alarmsToolbar.GetItemRect(buttonCount - 1, buttonRect);

		switch(pCalcSize->dwFlag)
		{
		case PGF_CALCWIDTH:
			pCalcSize->iWidth = buttonRect.right;
			return 0;
			break;

		case PGF_CALCHEIGHT:
			pCalcSize->iHeight = buttonRect.bottom;
			return 0;
			break;			
		}

		return FALSE;
	}
	else if(lpnm->code == PGN_SCROLL)
	{
		LPNMPGSCROLL   pScroll = (LPNMPGSCROLL)lParam;

		switch(pScroll->iDir)
		{
		case PGF_SCROLLLEFT:
			pScroll->iScroll = 15;
			return FALSE;
			break;

		case PGF_SCROLLRIGHT:
			pScroll->iScroll = 15;
			return FALSE;
			break;

		case PGF_SCROLLUP:
			break;

		case PGF_SCROLLDOWN:
			break;
		}
	}

    return TransActiveDialog::OnNotify(wParam, lParam, pResult);

}


void CAlarmsGuiDlg::OnMove(int x, int y )
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnMove" );
	TransActiveDialog::OnMove(x, y);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnMove" );
}


// Note that this method sometimes causes the display to become slightly corrupted if the
// "show window contents while dragging" option is selected in the Windows display properties panel.
void CAlarmsGuiDlg::OnSize(UINT nType, int cx, int cy)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnSize" );

    // Call base class function
	TransActiveDialog::OnSize(nType, cx, cy);

    // And recalculate window placements
    calculateWindowPlacements(cx,cy);
	
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnSize" );
}

void CAlarmsGuiDlg::calculateWindowPlacements(int cx, int cy)
{
    FUNCTION_ENTRY("calculateWindowPlacements");

	// The following constant defines the width of the space reserved on the right of the toolbar line.
	// This space is used by the current filter and alarm totals controls
	const unsigned int FILTER_AND_TOTALS_WIDTH = 460;

    // Get the size of the status bar control (note that we just need the size, not the position)
	CRect statusBarRect;
	if (m_statusBar.m_hWnd != NULL)
    {
        m_statusBar.GetWindowRect(statusBarRect);
    }

    // Get the size of the pager bar control (note that we just need the size, not the position)
    CRect pagerRect;
	if (m_Pager.m_hWnd != NULL)
    {
        m_Pager.GetWindowRect(pagerRect);
    }
    else
    {
		GetDlgItem(IDC_PAGER_CTRL)->GetWindowRect(pagerRect);
    }

	// Move the Total Alarms controls if necessary
	if((m_totalAlarms.m_hWnd != NULL) && (m_totalAlarms.IsWindowVisible()))
	{
		// Move the Total Alarms static field and edit box
		CRect totAlarmStaticRect, totAlarmEditRect;
		m_totalAlarms.GetWindowRect(totAlarmStaticRect);
		m_totalAlarmsView.GetWindowRect(totAlarmEditRect);

		m_totalAlarmsView.MoveWindow(cx -(totAlarmEditRect.Width() + 2), totAlarmEditRect.Height()+2, totAlarmEditRect.Width(), totAlarmEditRect.Height(), true);
		m_totalAlarms.MoveWindow(cx -(totAlarmStaticRect.Width() + 70), totAlarmStaticRect.Height()+12, totAlarmStaticRect.Width(), totAlarmStaticRect.Height(), true); //TD 11920 expand display space for more char
	}

	// Move the Unack alarms controls if necessary
	if((m_totalUnack.m_hWnd != NULL) && (m_totalUnack.IsWindowVisible()))
	{
		// Move the Total Unack static field and edit box
	    CRect totUnackStaticRect, totUnackEditRect;
		m_totalUnack.GetWindowRect(totUnackStaticRect);
		m_totalUnackEdit.GetWindowRect(totUnackEditRect);

		m_totalUnackEdit.MoveWindow(cx -(totUnackEditRect.Width() + 2), 1, totUnackEditRect.Width(), totUnackEditRect.Height(), true);
		m_totalUnack.MoveWindow(cx -(totUnackStaticRect.Width() + 68), 6, totUnackStaticRect.Width(), totUnackStaticRect.Height(), true);  //TD 11920 expand display space for more char
	}

	// Move the current filter controls if necessary
	if((m_currentFilterText.m_hWnd != NULL) && (m_currentFilterText.IsWindowVisible()))
	{
		// Move the current filter static field and list control
		CRect currentFilterTextRect, currentFilterControlRect;
		m_currentFilterText.GetWindowRect(currentFilterTextRect);
		m_currentFilterListCtrl.GetWindowRect(currentFilterControlRect);

		m_currentFilterListCtrl.MoveWindow(cx -FILTER_AND_TOTALS_WIDTH + currentFilterTextRect.Width() + 10, 15,
				currentFilterControlRect.Width(), currentFilterControlRect.Height(), true);
		m_currentFilterText.MoveWindow(cx -FILTER_AND_TOTALS_WIDTH + 5, 10, currentFilterTextRect.Width(), currentFilterTextRect.Height(), true);
	}

	// Move menu line if necessary
    if( (m_MenuLine.m_hWnd != NULL) && m_MenuLine.IsWindowVisible() )
    {
		CRect menuLineRect;
		m_MenuLine.GetWindowRect(menuLineRect);
        ScreenToClient(menuLineRect);
        m_MenuLine.MoveWindow(0, 0, cx, menuLineRect.Height(), true);
    }

	// Move status bar and progress window
    m_statusBar.MoveWindow(0, cy - statusBarRect.Height(), cx, statusBarRect.Height(), true);
    m_progress.MoveWindow(0, cy - statusBarRect.Height(), cx, statusBarRect.Height(), true);

	// Move the pager (contains the toolbar) if necessary
	if((m_alarmsToolbar.m_hWnd != NULL) && IsWindow(m_alarmsToolbar.m_hWnd))
	{
		CRect tmpRect;
		m_Pager.GetWindowRect(tmpRect);
		ScreenToClient(tmpRect);
		tmpRect.right = cx - FILTER_AND_TOTALS_WIDTH;
		int left = tmpRect.left;
		m_Pager.MoveWindow(tmpRect);             
	}

	// Move the alarm list itself. To do this we need to calculate the height of the list control window
    int listControlWindowHeight = cy;
    if( (m_statusBar.m_hWnd != NULL) && m_statusBar.IsWindowVisible() )
    {
		listControlWindowHeight -= statusBarRect.Height() + 2;
    }
    if( (m_Pager.m_hWnd != NULL) && m_Pager.IsWindowVisible() )
    {
		listControlWindowHeight -= pagerRect.Height() + 5;
    }

	if(m_isToolBarVisible)
	{
		GetDlgItem(IDC_ALARM_LIST)->MoveWindow(0, (pagerRect.Height() + 10), cx, listControlWindowHeight, true);
	}
	else
	{
		GetDlgItem(IDC_ALARM_LIST)->MoveWindow(0, 3, cx, listControlWindowHeight, true);
	}
	if(RunParams::getInstance().isSet(RPARAM_XPOS.c_str())) //tdTD19064
	{
		int x = atoi(RunParams::getInstance().get(RPARAM_XPOS.c_str()).c_str());
		int y = atoi(RunParams::getInstance().get(RPARAM_YPOS.c_str()).c_str());
		int cy = atoi(RunParams::getInstance().get(RPARAM_HEIGHT.c_str()).c_str());
		int cx = atoi(RunParams::getInstance().get(RPARAM_WIDTH.c_str()).c_str());
		SetWindowPos(NULL,x,y,cx,cy,SWP_NOZORDER);
	}
    FUNCTION_EXIT;
}


LRESULT CAlarmsGuiDlg::OnChangeMsg(WPARAM wParam, LPARAM lParam)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnChangeMsg" );

	m_totIntAlarms = wParam;
	
	if (m_exceededMaxDisplayAlarm == true)
	{
		m_totAlarms.Format("%d+", m_totIntAlarms);
	}
	else
	{
		m_totAlarms.Format("%d", m_totIntAlarms);
	}
	m_totUnacknowledged = lParam; 
	
	UpdateData(false);
	return NULL;	// This line must be included. Everything above is application specific

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnChangeMsg" );
}

LRESULT CAlarmsGuiDlg::OnFilterApply(WPARAM wParam, LPARAM lParam)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnFilterApply" );

	std::string filterName;
	CString curName;

	//refresh the filter combo box with default
	populateNamedFilters(TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT);

	// Get the filter index type
	TA_Base_App::AlarmFilterDlg::FilterIndex fIndex = static_cast<TA_Base_App::AlarmFilterDlg::FilterIndex> (wParam);
	switch (fIndex)
	{
		case (TA_Base_App::AlarmFilterDlg::INDEX_DEFAULT):
			filterName = TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT;
			break;

		case (TA_Base_App::AlarmFilterDlg::INDEX_USER_DEFINED):
			filterName = TA_Base_Core::NamedFilter::UNNAMED;
			break;
		
		case (TA_Base_App::AlarmFilterDlg::INDEX_LIST):
			//Get the index of the Default Filter which is either first or last
			int defaultIndex = m_currentFilterListCtrl.FindString(-1, TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT.c_str() );

			int index = lParam;
// 			if(defaultIndex<=index)
// 			 index = index + 1;
            //++ Hu Wenguang
            // TD12728
			if(index>=0)
            // TD12728
            //Hu Wenguang ++ 
			m_currentFilterListCtrl.GetLBText(index, curName);
			filterName = curName;
			break;
	}
	
	// Populate the filter combo box with the current filter name		
	populateNamedFilters(filterName);

	// Reset the caption to show the filter param
    //std::string windowText(RunParams::getInstance().get(RPARAM_APPNAME));
	std::string windowText("ISCS警报管理器");
    std::string filterText(m_alarmFilterDlg->getCurrentFilterString());
    
    //if (0 < filterText.length() && filterName != "Default Filter")//TD15055
	if (0 < filterText.length() && filterName != "默认过滤器")
    {
        windowText += filterText;
        SetWindowText(CString(windowText.c_str()));
    }
	else // This is a case of empty filter string so set the text to just the application name
	{
        SetWindowText(CString(windowText.c_str()));
	}

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnFilterApply" );

	return NULL;	// This line must be included. Everything above is application specific
}


LRESULT CAlarmsGuiDlg::OnFilterDelete(WPARAM wParam, LPARAM lParam)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnFilterDelete" );

	// Confirm there is delete of the filter
	TA_Base_App::AlarmFilterDlg::FilterIndex fIndex = static_cast<TA_Base_App::AlarmFilterDlg::FilterIndex> (wParam);
	if ( fIndex == TA_Base_App::AlarmFilterDlg::INDEX_LIST )
	{
		// Refresh the filter combo box
		// Always go back to Default when a filter is deleted
		populateNamedFilters(TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT); 
	}
	
	// Otherwise do nothing
	return NULL;	// This line must be included. Everything above is application specific

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnFilterDelete" );
}


LRESULT	CAlarmsGuiDlg::OnFilterUpdate(WPARAM wParam, LPARAM lParam)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnFilterUpdate" );

	// Refresh the ComboBox
	CString strFilterName;
	GetDlgItemText(IDC_NAMED_FILTER, strFilterName);	

	populateNamedFilters(strFilterName.GetBuffer(0));

	//m_currentFilterListCtrl.SetCurSel()

	return 0;
}

LRESULT CAlarmsGuiDlg::OnSystemInDegradedMode(WPARAM wParam, LPARAM lParam)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnSystemInDegradedMode" );

    // Retrieve the locations related to the current profile. We still only want alarms
    // from these locations.
    TA_Base_Bus::Filter preFilter;
	std::vector<unsigned long> locations;
	initPreFiltering(preFilter, locations);

    // Clear pre-filter as we don't want rights to be checked before an alarm is displayed.
    preFilter.removeAll();
	m_alarmList.updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
    
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnSystemInDegradedMode" );
    return NULL;	// This line must be included. Everything above is application specific
}


LRESULT CAlarmsGuiDlg::OnSystemInNormalMode(WPARAM wParam, LPARAM lParam)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnSystemInNormalMode" );

	// Reset Pre-Filter
    TA_Base_Bus::Filter preFilter;
	std::vector<unsigned long> locations;
	initPreFiltering(preFilter, locations);
	m_alarmList.updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);
		
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnSystemInNormalMode" );
    return NULL;	// This line must be included. Everything above is application specific

}


// BJR 17/8/04: This function appears to apply to menus as well as buttons.
void CAlarmsGuiDlg::DisableButtons()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "DisableButtons" );

	ItemAvailability availability;
	availability[ACKNOWLEDGE_ALARM]			= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[VIEW_COMMENT]				= TA_Base_Bus::IAlarmListObserver::DISABLED;

	availability[RELATED_SCHEMATIC]			= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[RELATED_INSPECTOR]			= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[RELATED_EVENTS]			= TA_Base_Bus::IAlarmListObserver::DISABLED;

	availability[RELATED_DSS]				= TA_Base_Bus::IAlarmListObserver::DISABLED; 
	availability[POSTPONE_DSS]				= TA_Base_Bus::IAlarmListObserver::DISABLED; 

	availability[JOB_REQUEST]				= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[RELATED_TREND]				= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[RELATED_AVALANCHE_EVENTS]	= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[INCIDENT_LOG_SHEET]		= TA_Base_Bus::IAlarmListObserver::DISABLED;

	availability[CLOSE_AND_ACK_ALARM]		= TA_Base_Bus::IAlarmListObserver::DISABLED;

	availability[INHIBIT_ALARM]				= TA_Base_Bus::IAlarmListObserver::DISABLED;

	// The following are not in the main and right-click menus, but they are button actions
	availability[EDIT_COMMENT]				= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[ACKNOWLEDGE_ALL]			= TA_Base_Bus::IAlarmListObserver::DISABLED;
	availability[RELATED_PLAN_INSTANCE]		= TA_Base_Bus::IAlarmListObserver::DISABLED;

	updateAvailableAlarmActions(availability);

    setCurrentDescriptionText("");	// Clear the status bar text

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "DisableButtons" );
    return;
}


void CAlarmsGuiDlg::Print()
{
	FUNCTION_ENTRY( "Print" );

	std::string oper = "";
	std::string console = "";
	TA_Base_Core::IOperator* p_operator = NULL;
	TA_Base_Core::IConsole* p_console = NULL;
	try
	{
		p_operator = TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession(m_sessionId, false);
		oper = p_operator->getDescription();
		p_console = TA_Base_Core::ConsoleAccessFactory::getInstance().getConsoleFromSession(m_sessionId, false);
		console = p_console->getName();
	}
	catch(...)
	{
		if(p_operator != NULL)
		{
			delete p_operator;
			p_operator = 0;
		}
		if(p_console != NULL)
		{
			delete p_console;
			p_console = 0;
		}
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040027);
	}
	
	delete p_operator;
	delete p_console;

	// TD12474 ++
    /*CString header = "Alarms : " + CTime::GetCurrentTime().Format("%a %#d/%#m/%Y %#H:%M:%S")
        + " - " + oper.c_str() + " on console " + console.c_str();*/

	TA_Base_Core::DateFormat dateFormat;
	std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat3);	

	//TD14929
	CString header = "警报管理器: " + CTime::GetCurrentTime().Format(dateFormatStr.c_str())
		+ " - " + oper.c_str() + "  在控制台: " + console.c_str();
	// ++ TD12474 

	// Do not delete this structure. It will be deleted inside the thread
    CListCtrlPrint::t_ParamsToReceive* params = new CListCtrlPrint::t_ParamsToReceive;
	params->csPageHead = header;
	params->csPage = "页 %d / %d";
	params->list = &m_alarmList;

    // Now populate the colour map.
    CHeaderCtrl* pHeader = (CHeaderCtrl*)(m_alarmList.GetDlgItem(0));
    unsigned int numColumns = pHeader->GetItemCount();
    unsigned int numRows = m_alarmList.GetItemCount();
    params->colourMap = new TA_Base_Bus::CellColourMap( numColumns, numRows, RGB( 0, 0, 0 ), RGB( 255, 255, 255 ) );

    // Fill colour map with colours.
	try
	{
	
		for ( unsigned int i=0; i<numRows; i++ )
		{
			COLORREF foreground, background;
			m_alarmList.determinePrintColours( i, foreground, background );
			params->colourMap->setRowColour( i, foreground );
			params->colourMap->setRowBkColour( i, background );
		}
	}
	catch(...)
	{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "alarm store failed to find alarm by alarm id" );
	}
	// TD 13994 Determine the column width and adjust where appropriate
	if (m_printoutColWidths != NULL)
	{
		// create a new col width array for print job to process the width, if there are changes
		params->pColWidthArray = m_printoutColWidths;
	}


    CWinThread* printThread = AfxBeginThread(RUNTIME_CLASS(CListCtrlPrint));
    printThread->PostThreadMessage(CPrinterJob::WM_START_PRINT, 0, (LPARAM)params);

	FUNCTION_EXIT;
}


void CAlarmsGuiDlg::Save()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "Save" );
    
    // Get the GUI entity from which we can get the default export path.
    TA_Base_Core::AlarmGUI* alarmGUI = dynamic_cast<TA_Base_Core::AlarmGUI*>(m_controlClass->getGUIEntity());
	
    // Do not delete this structure. It will be deleted inside the thread
    CExportAlarms::t_ParamsToReceive* params;
    params = new CExportAlarms::t_ParamsToReceive;

    params->alarmList = &m_alarmList;
    try
    {
        //params->defaultExportPath = alarmGUI->getExportAlarmDirectory().c_str();
		//TD17888 marc_bugfix 
		params->defaultExportPath = alarmGUI->getExportDirectory().c_str();
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,"DatabaseError", "There was an error while attempting to retrieve the default export path. Using default.");
        params->defaultExportPath.Empty();
    }


    CWinThread* exportThread = AfxBeginThread(RUNTIME_CLASS(CExportAlarms));

    exportThread->PostThreadMessage(START_EXPORT, 0, (LPARAM)params);
	
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "Save" );
}


// DestroyWindow is called when the Control station is closed
// to terminate running Guis. The WM_DESTROY message is intercepted
// here in order to save user settings.
void CAlarmsGuiDlg::OnDestroy()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnDestroy" );

	if(m_alarmFilterDlg != NULL)
	{
		m_alarmFilterDlg->DestroyWindow();
		m_alarmFilterDlg = NULL;
	}

    if(m_groupUpdateSubscriber != NULL)
    {
        delete m_groupUpdateSubscriber;
        m_groupUpdateSubscriber = NULL;
    }

	if (m_ptrSettingsMgr)
	{
        OnClose();
    }

	// TD 13994 delete the col width array after use
	if (m_printoutColWidths != NULL)
	{
		delete m_printoutColWidths;
		m_printoutColWidths = NULL;
	}

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnDestroy" );
}



void CAlarmsGuiDlg::OnClose()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnClose" );
	m_controlClass->saveAllUserSettings();
	RunParams::getInstance().deregisterRunParamUser(this);
	m_alarmList.DestroyWindow();
    DestroyWindow();	
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnClose" );
}


BOOL CAlarmsGuiDlg::PreTranslateMessage(MSG* pMsg) 
{
	//LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "PreTranslateMessage" );

     if (WM_KEYFIRST <= pMsg->message && pMsg->message <= WM_KEYLAST)
     {
         HACCEL hAccel = m_hAccel;
         if (hAccel && ::TranslateAccelerator(AfxGetMainWnd()->m_hWnd, hAccel, pMsg))
             return TRUE;
     }

	 return TransActiveDialog::PreTranslateMessage(pMsg);

	//LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "PreTranslateMessage" );
}


// ---------------- Operations to handle menu items ------------------

void CAlarmsGuiDlg::OnPrint() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnPrint" );
	try
	{
		Print();
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "OnPrint failed" );
	}
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnPrint" );
}

void CAlarmsGuiDlg::OnExport() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnExport" );
	Save();
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnExport" );
}

void CAlarmsGuiDlg::OnAppAbout() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAppAbout" );
	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAppAbout" );
}


std::vector<TA_Base_Bus::IAlarmAction*> CAlarmsGuiDlg::initListControlActions()
{
	FUNCTION_ENTRY("initListControlActions");

    std::vector<TA_Base_Bus::IAlarmAction*> actions;

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
    //actions.push_back( new TA_Base_Bus::ActionIncidentLogSheet() );
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


void CAlarmsGuiDlg::initListControlColumns()
{
	FUNCTION_ENTRY("initListControlColumns");

	TA_Base_Bus::Sort descSort;
	TA_Base_Bus::Sort ascSort;
		
	//as per TD 12743/12744
	//we now use nuber of characters to specify initial width of coulmns
	//for Ack By and Asset columns
	CString widestCharacter = 'W';
	unsigned long widestCharWidth = m_alarmList.GetStringWidth (widestCharacter);


//	if( RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING))
//	{
		// KRTC required sorting behaviours
	/*	
		// Add disabled state and ack columns so that default sort will work.
		//TD 3354: This col is added to list but hidden from user to remove the black line affecting 1st col 
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnState(0, true));
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAck(0, false));

		// Source timestamp
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_TIME, Sort::SORT_ASCENDING);
		descSort = reverseSort(ascSort);
		unsigned long ulTimestampWidth = TA_Base_Bus::AlarmConstants::DEFAULT_TIMESTAMPCOLWIDTH;
		if (RunParams::getInstance().isSet(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()))
		{
			ulTimestampWidth = strtoul(RunParams::getInstance().get(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()).c_str(), NULL, 0);
		}
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true, &ascSort, &descSort, NULL));
	        		
		// Severity
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSeverity(40, true, &ascSort, &descSort, NULL)); //TD15005

		// Asset
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_ASSET_NAME,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);	
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAsset(210, true, &ascSort, &descSort, NULL)); //TD15005

		// Description
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_DESCRIPTION, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		//TD12744 / TD12743 / TD15005(4th) ++
		//will automatically be adjusted by expandColumnToFitListWidth
		//m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(504, true, &ascSort, &descSort, NULL)); 
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(1, true, &ascSort, &descSort, NULL)); 
		//++ TD12744 / TD12743 / TD15005(4th)

		// Value
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_VALUE, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,  Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnValue(140, true, &ascSort, &descSort, NULL));

		// TD #11287
		// Changed column order from DAM to MDA
		// MMS	
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_MMS, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnMms(24, true, &ascSort, &descSort, NULL));

		// DSS	
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_DSS, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnDecisionSupport(22, true, &ascSort, &descSort, NULL));

		// Avalanche	
		ascSort.removeAll();
		ascSort.addSort(Sort::SORT_AVALANCHE, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAvalanche(22, true, &ascSort, &descSort, NULL));

		// Acknowledged by
		ascSort.removeAll(); 
		ascSort.addSort(Sort::SORT_ACK_BY,		Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAckBy(10 * widestCharWidth, true, &ascSort, &descSort, NULL));

		// Comment	
		ascSort.removeAll();
		//TD #12719++
		ascSort.addSort(Sort::SORT_COMMENT,		Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		//TD ++#12719
		TA_Base_Bus::IAlarmAction* doubleClickAction = new TA_Base_Bus::ActionDisplayComment();
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnComment(24, true, &ascSort, &descSort, doubleClickAction));
*/
//	}
//	else
//	{
		// Add disabled state and ack columns so that default sort will work.
		//marvin++ wrong default sort
	/*	ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		*/
		//TD 3354: This col is added to list but hidden from user to remove the black line affecting 1st col 
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnState(0, true, SORT_BY_DEFAULT, NULL));
	//	m_alarmColumns.push_back(new TA_Base_Bus::ColumnAck(0, false, SORT_BY_ACK));

		// Source timestamp
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		descSort = reverseSort(ascSort);*/
		unsigned long ulTimestampWidth = TA_Base_Bus::AlarmConstants::DEFAULT_TIMESTAMPCOLWIDTH;
	/*	if (RunParams::getInstance().isSet(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()))
		{
			ulTimestampWidth = strtoul(RunParams::getInstance().get(TA_Base_Bus::AlarmConstants::RPARAM_TIMESTAMPCOLWIDTH.c_str()).c_str(), NULL, 0);
		}*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSourceTimestamp(ulTimestampWidth, true,SORT_BY_DATETIME, NULL));
	        		
		// Severity
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,	Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnSeverity(60, true, SORT_BY_SEVERITY, NULL)); //TD15005

		// Asset
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_ASSET_NAME,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);	*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAsset(210, true, SORT_BY_ASSET, NULL)); //TD15005

		// Description
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_DESCRIPTION, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		//TD12744 / TD12743 / TD15005(4th) ++
		//will automatically be adjusted by expandColumnToFitListWidth
		//m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(504, true, &ascSort, &descSort, NULL)); 
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnDescription(1, true, SORT_BY_DESCRIPTION, NULL)); 
		//++ TD12744 / TD12743 / TD15005(4th)

		// Value
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_VALUE, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnValue(140, true, SORT_BY_VALUE, NULL));

		// TD #11287
		// Changed column order from DAM to MDA
		// MMS	
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_MMS, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnMms(24, true, SORT_BY_MMS, NULL));

		// DSS	
	/*	ascSort.removeAll();
		ascSort.addSort(Sort::SORT_DSS, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnDecisionSupport(22, true, SORT_BY_DSS, NULL));

		// Avalanche	
		/*ascSort.removeAll();
		ascSort.addSort(Sort::SORT_AVALANCHE, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAvalanche(22, true, SORT_BY_AVALANCHE, NULL));

		// Acknowledged by
	/*	ascSort.removeAll(); 
		ascSort.addSort(Sort::SORT_ACK_BY,		Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);*/
		m_alarmColumns.push_back(new TA_Base_Bus::ColumnAckBy(10 * widestCharWidth, true, SORT_BY_ACK, NULL));

		// Comment	
	/*	ascSort.removeAll();
		//TD #12719++
		ascSort.addSort(Sort::SORT_COMMENT,		Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_OPEN_CLOSE,  Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_ACK_NOT_ACK, Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_SEVERITY,    Sort::SORT_ASCENDING);
		ascSort.addSort(Sort::SORT_TIME,        Sort::SORT_DESCENDING);
		descSort = reverseSort(ascSort);
		//TD ++#12719
		TA_Base_Bus::IAlarmAction* doubleClickAction = new TA_Base_Bus::ActionDisplayComment();*/
		//m_alarmColumns.push_back(new TA_Base_Bus::ColumnComment(24, true, SORT_BY_COMMENTS, NULL));
//	}

	FUNCTION_EXIT;
}


void CAlarmsGuiDlg::initPreFiltering(TA_Base_Bus::Filter& preFilter, std::vector<unsigned long>& locations)
{
	locations.clear();

	TA_Base_Bus::SessionInfo sessionInfo;

	try
	{
		// Get some info about the current session.
		TA_Base_Bus::AuthenticationLibrary authLibrary(true);
		sessionInfo = authLibrary.getSessionInfo(m_sessionId, m_sessionId);
	}
	catch(TA_Base_Core::AuthenticationAgentException&)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", "Could not retrieve session information");
		return;
	}
	catch(TA_Base_Core::ObjectResolutionException&)
	{
		LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", "Could not resolve Authentication Agent");
		return;
	}

	// The prefilter requires the session id.
	preFilter.addFilter(TA_Base_Bus::Filter::FILTER_PROFILE, 
						TA_Base_Bus::Filter::COMPARE_EQUAL, m_sessionId);

	bool isCentralProfile = false;
	TA_Base_Core::IProfile* profile;

	for (std::vector<unsigned long>::iterator iter = sessionInfo.ProfileId.begin();
		iter != sessionInfo.ProfileId.end(); iter++)
	{
		try
		{
			profile = TA_Base_Core::ProfileAccessFactory::getInstance().getProfile(*iter);

			if (profile->getType() == PROFILE_LOCAL)
			{
				locations.push_back(sessionInfo.LocationId);
			}
			else  // PROFILE_CENTRAL
			{
				isCentralProfile = true;
			}

			delete profile;
		}
		catch(TA_Base_Core::DataException&)
		{
			delete profile;
			LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not retrieve profile information");
		}
		catch(TA_Base_Core::DatabaseException&)
		{
			delete profile;
			LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not retrieve profile information");
		}
	}

	if (isCentralProfile)
	{
		// Central profiles require all locations.
		locations.clear();
	}
}


Sort CAlarmsGuiDlg::reverseSort(const Sort& sort)
{
	Sort reverseSort;
	TA_Base_Bus::Sort::SortDirection oppDirection;

	std::vector<TA_Base_Bus::Sort::SortDetails> sortDetails = sort.getSort();

	for (std::vector<TA_Base_Bus::Sort::SortDetails>::iterator iter = sortDetails.begin();
		iter != sortDetails.end(); iter++)
	{
		oppDirection = (iter->direction == Sort::SORT_ASCENDING ? 
							Sort::SORT_DESCENDING : Sort::SORT_ASCENDING);
		reverseSort.addSort(iter->type, oppDirection);
	}

	return reverseSort;
}


// Set a menu item check to specified value
void CAlarmsGuiDlg::SetMenuItemCheck(const UINT& itemID, bool bChecked)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "SetMenuItemCheck" );
	// Get the top memu
	CMenu* menu = GetMenu();
	if( menu == NULL )
	{
		return;
	}

	// Find the sub-menu specified by subMenuID
	menu->CheckMenuItem(itemID, MF_BYCOMMAND | (bChecked ? MF_CHECKED : MF_UNCHECKED));

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "SetMenuItemCheck" );
}

void CAlarmsGuiDlg::OnViewShowFilters() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnViewShowFilters" );
	
	m_alarmFilterDlg->m_updateNeeded = false;
	//TD15028++ wangyn
	m_alarmFilterDlg->setFromDateTime(m_alarmList);
	m_alarmFilterDlg->setToDateTime(m_alarmList);
	//++TD15028
	m_alarmFilterDlg->ShowWindow(SW_SHOW);
	m_alarmFilterDlg->CenterWindow(this); //TD15040
	
	// Get Current Filter ??	
	// Ensure that the name is a valid name and not empty
	// If the filter is User Defined, the default will be used
	CString curName;
	m_currentFilterListCtrl.GetWindowText(curName);
	if ( curName.IsEmpty() == false )
	{
		std::string filterName(curName);
		m_alarmFilterDlg->showFilter(filterName);

		// Reset the caption to show the filter param
		std::string windowText(RunParams::getInstance().get(RPARAM_APPNAME));
		std::string filterText(m_alarmFilterDlg->getCurrentFilterString());
		//TD15055----
		//Only when the 'Apply' or 'Ok' button is clicked,the text of header is changed.
		/*
		if (0 < filterText.length())
		{
			windowText += filterText;
			SetWindowText(CString(windowText.c_str()));
		}
		else // This is a case of empty filter string so set the text to just the application name
		{
			SetWindowText(CString(windowText.c_str()));
		}
		*/
		//TD15055----

	}

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnViewShowFilters" );
}


void CAlarmsGuiDlg::OnAlarmAcknowledge() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmAcknowledge" );
	m_alarmList.executeAction(ACKNOWLEDGE_ALARM);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmAcknowledge" );
}


void CAlarmsGuiDlg::OnAlarmAllAcknowledge() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmAllAcknowledge" );
	m_alarmList.executeAction(ACKNOWLEDGE_ALL);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmAllAcknowledge" );
} 


void CAlarmsGuiDlg::OnAlarmClose()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmClose" );
	m_alarmList.executeAction(CLOSE_AND_ACK_ALARM);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmClose" );
}

void CAlarmsGuiDlg::OnAlarmDisplayComment() 
{
    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmDisplayComment" );
	m_alarmList.executeAction(VIEW_COMMENT);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmDisplayComment" );
}


void CAlarmsGuiDlg::OnAlarmSchematic()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmSchematic" );
	m_alarmList.executeAction(RELATED_SCHEMATIC);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmSchematic" );
}


void CAlarmsGuiDlg::OnAlarmPlan()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmPlan" );
	m_alarmList.executeAction(RELATED_DSS);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmPlan" );
}

void CAlarmsGuiDlg::OnAlarmPlanInstance()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmPlanInstance" );
	m_alarmList.executeAction(RELATED_PLAN_INSTANCE);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmPlanInstance" );
}

void CAlarmsGuiDlg::OnAlarmInspector()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmInspector" );
	m_alarmList.executeAction(RELATED_INSPECTOR);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmInspector" );
}


void CAlarmsGuiDlg::OnAlarmTrend()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmTrend" );
	m_alarmList.executeAction(RELATED_TREND);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmTrend" );
}


void CAlarmsGuiDlg::OnRelatedAvalancheEvents()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnRelatedAvalancheEvents" );
	m_alarmList.executeAction(RELATED_AVALANCHE_EVENTS);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnRelatedAvalancheEvents" );
}


void CAlarmsGuiDlg::OnAlarmJobRequest()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmJobRequest" );
	m_alarmList.executeAction(JOB_REQUEST);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmJobRequest" );
}

// TD 13825 Postpone dss function not added to menu
void CAlarmsGuiDlg::OnAlarmCancelPlan()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmCancelPlan" );
	m_alarmList.executeAction(POSTPONE_DSS);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmCancelPlan" );
}

void CAlarmsGuiDlg::OnAlarmEventLog()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmEventLog" );
	m_alarmList.executeAction(INCIDENT_LOG_SHEET);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmEventLog" );
}


void CAlarmsGuiDlg::OnAlarmEvents()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnAlarmEvents" );
	m_alarmList.executeAction(RELATED_EVENTS);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnAlarmEvents" );
}

void CAlarmsGuiDlg::OnEventsViewer()
{
    FUNCTION_ENTRY("OnEventsViewer");

	m_alarmList.executeAction(EVENTS_VIEWER);

    FUNCTION_EXIT;
}

void CAlarmsGuiDlg::OnViewInhibitAlarms()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnViewInhibitAlarms" );

	m_alarmList.executeAction(INHIBIT_ALARM);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnViewInhibitAlarms" );
}

void CAlarmsGuiDlg::OnViewInhibitAlarmByType()
{
	FUNCTION_ENTRY("OnViewInhibitAlarmByType");

	m_alarmList.executeAction(INHIBIT_ALARM_BY_TYPE);

	FUNCTION_EXIT;
}

void CAlarmsGuiDlg::OnViewInhibitAlarmByDescription()
{
	FUNCTION_ENTRY("OnViewInhibitAlarmByDescription");

	m_alarmList.executeAction(INHIBIT_ALARM_BY_DESCRIPTION);

	FUNCTION_EXIT;
}

void CAlarmsGuiDlg::OnViewInhibitions()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnViewInhibitions()" );
    
    TA_Base_App::CSuppressionRulesDialog rulesDialog(m_profileId, this);
	rulesDialog.DoModal();
	
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnViewInhibitions()" );
}



void CAlarmsGuiDlg::OnViewStatusBar() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnViewStatusBar" );

	if( m_statusBar.IsWindowVisible() )
	{
		SetMenuItemCheck(ID_VIEW_STATUS_BAR, false);
		m_progress.ShowWindow( SW_HIDE );
		m_statusBar.ShowWindow( SW_HIDE );
	}
	else
	{
		SetMenuItemCheck(ID_VIEW_STATUS_BAR, true);
		m_statusBar.ShowWindow( SW_SHOW );
		//m_progress.ShowWindow( SW_SHOW );	
	}

	// Call OnSize as it has the code to resize the windows
    CRect client;
    GetClientRect(client);
	calculateWindowPlacements(client.right, client.bottom );

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnViewStatusBar" );
}


void CAlarmsGuiDlg::OnViewToolbar() 
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnViewToolbar" );  

	if( m_Pager.IsWindowVisible() )
	{
		SetMenuItemCheck(ID_VIEW_TOOLBAR, false);
		m_Pager.ShowWindow( SW_HIDE );
		m_MenuLine.ShowWindow( SW_HIDE );
		
		m_totalAlarms.ShowWindow(SW_HIDE);
		m_totalUnack.ShowWindow(SW_HIDE);
		m_totalAlarmsView.ShowWindow(SW_HIDE);
		m_totalUnackEdit.ShowWindow(SW_HIDE);
		m_isToolBarVisible = false;

	}
	else
	{
		SetMenuItemCheck(ID_VIEW_TOOLBAR, true);
		m_Pager.ShowWindow( SW_SHOW );
		m_MenuLine.ShowWindow( SW_SHOW );

		m_totalAlarms.ShowWindow(SW_SHOW);
		m_totalUnack.ShowWindow(SW_SHOW);
		m_totalAlarmsView.ShowWindow(SW_SHOW);
		m_totalUnackEdit.ShowWindow(SW_SHOW);
		m_isToolBarVisible = true;
	}

	// Call OnSize as it has the code to resize the windows
    CRect client;
    GetClientRect(client);
	calculateWindowPlacements(client.right, client.bottom );

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnViewToolbar" );
}


void CAlarmsGuiDlg::serverDown()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "serverDown" );

    DisableButtons();
	m_alarmList.DeleteAllItems();
	m_statusBar.SetWindowText("Alarm Manager: Server down.");

	TA_Base_Bus::TransActiveMessage userMsg;
	UINT selectedButton = userMsg.showMsgBox(IDS_UI_040028);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "serverDown" );	
}


void CAlarmsGuiDlg::serverUp()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "serverUp" );

	//TO DO: Add more fault tolerant mechanism here to retrieve alarms etc..
    m_statusBar.SetWindowText("Alarm Manager: Server connection re-established.");

	TA_Base_Bus::TransActiveMessage userMsg;
	UINT selectedButton = userMsg.showMsgBox(IDS_UI_040029);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "serverUp" );
}


void CAlarmsGuiDlg::onRunParamChange(const std::string& name, const std::string& value)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "onRunParamChange" );
	
	if(name.compare(RPARAM_SESSIONID) == 0)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParamChange received for session ID. New name is %s and sesion ID is %s", name.c_str(), value.c_str());

		// Store the new session id.
		m_sessionId = value;

		// Update the pre-filter and locations.
		TA_Base_Bus::Filter preFilter;
		std::vector<unsigned long> locations;
		initPreFiltering(preFilter, locations);
		m_alarmList.updatePreFilterLocationsAndSession(preFilter, locations, m_sessionId);

        // Reset the profile id
        setProfileId();

		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParamChange function ended for name: %s and session ID: %s", name.c_str(), m_sessionId.c_str());
	}
	//TD16109----
	//2)The current filter field should not be Default Filter.It should be showing the filter name of the filter based on selected alarms.
	// else if(name.compare(RPARAM_FILTERONASSET) == 0)
	if(name.compare(RPARAM_FILTERONASSET) == 0)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParamChange received for filter asset. New filter asset is %s", value.c_str());
		filterOnAsset(value);
		populateNamedFilters(RunParams::getInstance().get(RPARAM_FILTERONASSET.c_str()));
	}
	// else if(name.compare(RPARAM_FILTERONENTITY) == 0)
	if(name.compare(RPARAM_FILTERONENTITY) == 0)
	{
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "RunParamChange received for filter entity. New filter entity is %s", value.c_str());
		filterOnEntity(value);
		populateNamedFilters(RunParams::getInstance().get(RPARAM_FILTERONENTITY.c_str()));
	}
	//----TD16109
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "onRunParamChange" );
}
 

CProgressCtrl* CAlarmsGuiDlg::getProgressBar()
{
	m_progress.ShowWindow(SW_SHOW);
	return &m_progress;
}


void CAlarmsGuiDlg::finishedWithProgressBar()
{
	m_progress.ShowWindow(SW_HIDE);
}

// BJR: This updates availability of buttons and menu items
void CAlarmsGuiDlg::updateAvailableAlarmActions(const ItemAvailability& availability)
{
	CMenu* menu = GetMenu();
	if( menu == NULL )
	{
		return;
	}

	for (ItemAvailability::const_iterator iter = availability.begin(); 
		iter != availability.end(); iter++)
	{
		unsigned int menuState;
		bool isEnabled = (iter->second == TA_Base_Bus::IAlarmListObserver::ENABLED);
		if (isEnabled)
		{
			menuState = MF_BYCOMMAND | MF_ENABLED;
		}
		else
		{
			menuState = MF_BYCOMMAND | MF_DISABLED | MF_GRAYED;
		}

		switch (iter->first)
		{
			case ACKNOWLEDGE_ALARM:
				m_alarmsToolbar.EnableButton(IDC_ACK, isEnabled);
				menu->EnableMenuItem(ID_AM_ALARM_ACKNOWLEDGE, menuState);
				break;

			case VIEW_COMMENT:
				menu->EnableMenuItem(ID_AM_ALARM_VIEW_COMMENT, menuState);
				m_alarmsToolbar.EnableButton(IDC_COMMENT, isEnabled);
				break;

			case CLOSE_AND_ACK_ALARM:
				m_alarmsToolbar.EnableButton(IDC_CLOSE_ALARM, isEnabled);
				menu->EnableMenuItem(ID_AM_ALARM_MANUAL_CLOSE, menuState);
				break;

			case RELATED_INSPECTOR:
				menu->EnableMenuItem(ID_AM_ALARM_INSPECTOR, menuState);
				break;

			case RELATED_SCHEMATIC:
				menu->EnableMenuItem(ID_AM_ALARM_SCHEMATIC, menuState);
				break;

			case RELATED_TREND:
				menu->EnableMenuItem(ID_AM_ALARM_TREND, menuState);
				break;

			case RELATED_AVALANCHE_EVENTS:
				menu->EnableMenuItem(ID_AM_ALARM_AVALANCHE_EVENTS,menuState);
				break;

			case RELATED_EVENTS:
				menu->EnableMenuItem(ID_AM_ALARM_EVENTS, menuState);
				//TD 11783 Event Viewer button is always enabled via toolbar, and will launch EventViewer
				// m_alarmsToolbar.EnableButton(IDC_RELATED_EVENTS,isEnabled);
				break;
			case RELATED_PLAN_INSTANCE:
				menu->EnableMenuItem(ID_AM_ALARM_PLAN_INSTANCE, menuState);
				break;

			case JOB_REQUEST:
				menu->EnableMenuItem(ID_AM_ALARM_JOB_REQUEST, menuState);
				break;

			case RELATED_DSS: 
				menu->EnableMenuItem(ID_AM_ALARM_PLAN, menuState);
				break;

			case POSTPONE_DSS:
				menu->EnableMenuItem(ID_AM_CANCEL_ALARM_PLAN, menuState);
				break;

			case INCIDENT_LOG_SHEET: 
				menu->EnableMenuItem(ID_AM_ALARM_EVENT_LOG, menuState);
				break;

			case ACKNOWLEDGE_ALL:
				m_alarmsToolbar.EnableButton(IDC_ACKALL, isEnabled);
				break;

            case INHIBIT_ALARM:
				m_alarmsToolbar.EnableButton(IDC_INHIBIT_ALARMS, isEnabled);
				menu->EnableMenuItem(ID_AM_INHIBIT_BY_TYPE, menuState);
				menu->EnableMenuItem(ID_AM_INHIBIT_BY_DESCRIPTION, menuState);
				break;
		}
	}
}


void CAlarmsGuiDlg::notifyNoMatchingAlarms()
{
    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifyNoMatchingAlarms" );
	m_alarmList.Invalidate(); //17081
    m_statusBar.SetWindowText(NO_MATCHING_ALARMS_MESSAGE.c_str());
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifyNoMatchingAlarms" );
}

 void CAlarmsGuiDlg::notifyFilterReset()
 {
	 //refresh the filter combo box with default
	 populateNamedFilters(TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT);
	 
	 // Reset the caption to default
		std::string windowText("ISCS警报管理器");
	 SetWindowText(CString(windowText.c_str()));
 }

void CAlarmsGuiDlg::setCurrentDescriptionText(const CString& description)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "setCurrentDescriptionText" );
    m_statusBar.SetWindowText(description);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "setCurrentDescriptionText" );
}


void CAlarmsGuiDlg::updateAlarmCounts(unsigned int totAlarms, unsigned int totUnacknowledgedAlarms, bool exceededMaxDisplayAlarm, bool isLoadingAlarms)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "updateAlarmCounts" );
	m_exceededMaxDisplayAlarm = exceededMaxDisplayAlarm;
	if (isLoadingAlarms == true)
	{
		m_isLoadingAlarms = true;
		// Display wait ie. hour-glass cursor while loading
		BeginWaitCursor();
	}
	else
	{
		m_isLoadingAlarms = false;
		// Display normal ie. Arrow cursor after loading
		EndWaitCursor();
		SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
	}

	PostMessage(WM_ON_CHANGE_MSG, totAlarms, totUnacknowledgedAlarms);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "updateAlarmCounts" );
}


void CAlarmsGuiDlg::notifySystemStateDegraded()
{
    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifySystemStateDegraded" );
	PostMessage(WM_ON_SYSTEM_DEGRADED, NULL, NULL);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifySystemStateDegraded" );
}


void CAlarmsGuiDlg::notifySystemStateNormal()
{
    LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "notifySystemStateNormal" );
	PostMessage(WM_ON_SYSTEM_RECOVERED, NULL, NULL);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "notifySystemStateNormal" );
}

  
void CAlarmsGuiDlg::updateAlarmSound(unsigned long severity, bool wereAlarmsAdded)
{
	// Alarm manager plays no sound.
}


void CAlarmsGuiDlg::setWindowTitle(const std::string& windowText)
{
	SetWindowText(windowText.c_str());
}


void CAlarmsGuiDlg::OnDefaultSort()
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "OnDefaultSort" );
	m_alarmList.defaultSort();
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "OnDefaultSort" );
}


void CAlarmsGuiDlg::filterOnAsset(const std::string& asset)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "filterOnAsset");

	if( false == m_hasAlarmListInit)
	{
		initializeAlarmList();
	}

	TA_Base_Bus::Filter filter;
	filter.addFilter(TA_Base_Bus::Filter::FILTER_ASSET_NAME, 
		TA_Base_Bus::Filter::COMPARE_CONTAINS, asset);
	try
	{
		m_alarmList.setFilter(filter, false);
	}
	catch(TA_Base_Core::AlarmListCtrlException)
	{
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040022);
	}


	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "filterOnAsset");
}


void CAlarmsGuiDlg::filterOnEntity(const std::string& entity)
{
	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "filterOnEntity");

	unsigned long entityKey;

	if( false == m_hasAlarmListInit)
	{
		initializeAlarmList();
	}
	EntityNameToKeyMap::iterator itr = m_entityNameKeyMap.find(entity);

	if( m_entityNameKeyMap.end() == itr)
	{
		TA_Base_Core::IEntityData* entityData;
		try
		{
			entityData	= TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entity);
			entityKey	= entityData->getKey();
			delete entityData;
			entityData = NULL;
			m_entityNameKeyMap[entity] = entityKey;
		}
		catch(...)
		{
			if( entityData != NULL)
			{
				delete entityData;
				entityData = NULL;
			}
			entityKey = 0;
		}
		
	}
	else
	{
		entityKey = itr->second;
	}

	TA_Base_Bus::Filter filter;
	filter.addFilter(TA_Base_Bus::Filter::FILTER_ASSOCIATED_ENTITY_KEY, 
		TA_Base_Bus::Filter::COMPARE_EQUAL, entityKey);
	m_alarmList.setFilter(filter, false);

	LOG(SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "filterOnEntity");
}


void CAlarmsGuiDlg::setProfileId()
{
    // Get the current profile
    std::vector<unsigned long> profiles;
    std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
		"setProfileId function started with session ID obtained from RUNPARAM: %s, and member session ID: %s", sessionId.c_str(), m_sessionId.c_str());

    try
    {
        TA_Base_Bus::AuthenticationLibrary authLib(true);
        profiles = authLib.getProfiles(sessionId);
    }
    catch(const TA_Base_Core::AuthenticationAgentException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "AuthenticationAgentException", ex.what());
    }
    catch(const TA_Base_Core::ObjectResolutionException& ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "ObjectResolutionException", ex.what());
    }
    catch(...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown error occurred while retrieving current profile.");
    }

    unsigned long profileKey = -1;
    if(profiles.size() == 0)
    {
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "No profiles were retrieved.");
    }
    else
    {
        profileKey = profiles[profiles.size() - 1];
    }

    m_profileId = profileKey;

	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, 
		"setProfileId function ended with profile key: %ul obtained", m_profileId);
}

void CAlarmsGuiDlg::OnSelchangeNamedFilter() 
{
	FUNCTION_ENTRY("OnSelchangeNamedFilter");
	
	// Get the currently selected item.
	int selIdx = m_currentFilterListCtrl.GetCurSel();
	if ( 0 <= selIdx )
	{
		// Get the selected filter name
		CString filterName_c;
		m_currentFilterListCtrl.GetLBText( selIdx, filterName_c );
		std::string filterName( filterName_c );

		// Make sure the string is not empty
		if ( filterName !="" )
		{
			// Reset the filter via the AlarmFilterDlg object
			m_alarmFilterDlg->setNamedFilter( filterName );

			// Reset the filter combo box to selected name
			populateNamedFilters( filterName );

			// Reset the caption to show the filter param
			//std::string windowText(RunParams::getInstance().get(RPARAM_APPNAME));
			std::string windowText("ISCS警报管理器");
			std::string filterText(m_alarmFilterDlg->getCurrentFilterString());
			
			//if (0 < filterText.length() && filterName != "Default Filter")//TD15055
			if (0 < filterText.length() && filterName != "默认过滤器")
			{
				windowText += filterText;
				SetWindowText(CString(windowText.c_str()));
			}
			else // This is a case of empty filter string so set the text to just the application name
			{
				SetWindowText(CString(windowText.c_str()));
			}
		}
	}

	FUNCTION_EXIT;	
}

void CAlarmsGuiDlg::populateNamedFilters(std::string curName)
{

	// Make sure the combo box is empty.
	m_currentFilterListCtrl.ResetContent();

	try
	{
		// Always put the Default Filter name as first item
		m_currentFilterListCtrl.AddString( TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT.c_str() );
		
		// Get a list of the filters.
		TA_Base_Core::NamedFilterAccessFactory::FilterNameList filters = TA_Base_Core::NamedFilterAccessFactory::getInstance().getAllFilterNames();

		// Now add each to the combo box.
		TA_Base_Core::NamedFilterAccessFactory::FilterNameList::iterator namedFilterIt;
		for ( namedFilterIt=filters.begin(); namedFilterIt!=filters.end(); namedFilterIt++ )
		{
			int index = m_currentFilterListCtrl.AddString( namedFilterIt->first.c_str() );
		}

	}
	catch ( TA_Base_Core::DataException& )
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Could not get a list of named filters!");
	}

	// Check for present of curName for Default & User Defined
	if (curName != TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT && curName != TA_Base_Core::NamedFilter::UNNAMED )
	{
		//++TD16109
		if(RunParams::getInstance().get(RPARAM_FILTERONASSET.c_str()) != "")
		{
			m_currentFilterListCtrl.AddTempString(curName.c_str());
		}
		else if(RunParams::getInstance().get(RPARAM_FILTERONENTITY.c_str()) != "")
		{
			m_currentFilterListCtrl.AddTempString(curName.c_str());
		}
		//TD16109++

		//Set the curName as the name in the combo box
		int index = m_currentFilterListCtrl.SelectString ( -1, curName.c_str() );
		m_currentFilterListCtrl.SetCurSel(index);
	}
	// Set the name in the combox box as Default Filter
	else if (curName == TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT)
	{
		int index = m_currentFilterListCtrl.SelectString ( -1, TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT.c_str() );
		m_currentFilterListCtrl.SetCurSel(index);
	}
	// Otherwise this must be a User Defined as it is not in the Named Filters database
	else if (curName == TA_Base_Core::NamedFilter::UNNAMED)
	{
		m_currentFilterListCtrl.AddTempString(TA_Base_Core::NamedFilter::UNNAMED.c_str());
	}
}


void CAlarmsGuiDlg::updatePreFilterAccordingToSystemState(TA_Base_Bus::Filter& preFilter)
{
    // TD 6950: Check if the system is currently in degraded mode. If so, clear the pre-filter.
    std::string groupsOffline = RunParams::getInstance().get(RPARAM_GROUPSOFFLINE);
    if(!groupsOffline.empty())
    {
        std::string::size_type pos = 0;
        while(pos != std::string::npos)
        {
            pos = groupsOffline.find(",", 0);
            std::string group = groupsOffline.substr(0, pos);

            if(group == m_groupUpdateSubscriber->getCentralServerGroupName())
            {
                // System is degraded
                preFilter.removeAll();
                return;
            }

            if(pos != std::string::npos)
            {
                groupsOffline = groupsOffline.substr(pos+1, groupsOffline.size() - (pos + 1));
            }
        }
    }
}

void CAlarmsGuiDlg::init()
{
   m_thread_hd = CreateThread( NULL, NULL, (LPTHREAD_START_ROUTINE)ThreadFunction, this, 0, &m_thread_id );
        CloseHandle(m_thread_hd);     //reduce init time

	//lizhongyan add
	//CFont font;
	m_alarmfont.CreateFont(17, 0, 0, 0, FW_THIN, FALSE, FALSE, 0, GB2312_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
		DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS,"宋体");
	m_alarmList.SetFont(&m_alarmfont, TRUE);
	m_alarmList.GetHeaderCtrl()->SetFont(&m_alarmfont, TRUE);

	// setup font
	m_font.CreatePointFont(100,"宋体");
	SetFont(&m_font,FALSE);
	m_currentFilterListCtrl.SetFont(&m_font,FALSE);
	//m_alarmList.SetFont(&m_font,FALSE);
	m_statusBar.SetFont(&m_font,FALSE);
	
    //setProfileId();
	m_alarmFilterDlg = new TA_Base_App::AlarmFilterDlg(m_alarmList, this);
    m_groupUpdateSubscriber = new TA_Base_Bus::SystemControllerGroupUpdateSubscriber(*this);

	std::string strLevel = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
	if (!strLevel.empty())
	{
		TA_Base_Core::DebugUtil::EDebugLevel level = TA_Base_Core::DebugUtil::getInstance().getDebugLevelFromString(strLevel.c_str());
		TA_Base_Core::DebugUtil::getInstance().setLevel(level);
	}

	// Load the accelerator keys
	m_hAccel = ::LoadAccelerators(AfxGetResourceHandle(),MAKEINTRESOURCE(IDR_ACCELERATOR1));

	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "m_sessionId = atol(RunParams::GetInstance().Get(RPARAM_SESSIONID).c_str());");
    m_sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str();
	
	try
    {
        m_controlClass->loadAllUserSettings();
    }
    catch( TA_Base_Core::UserSettingsException& use )
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "Error loading User Settings: %s", use.what() );
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040023);
    }
    catch ( ... )
    {
        LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugWarn, "An error occurred while loading User Settings" );
		TA_Base_Bus::TransActiveMessage userMsg;
		UINT selectedButton = userMsg.showMsgBox(IDS_UI_040024);
    }
	
	// Register for the SessionId run param
	RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

	// Register for the FilterOnAsset run param
	RunParams::getInstance().registerRunParamUser(this, RPARAM_FILTERONASSET.c_str());

	// Register for the FilterOnEntity run param
	RunParams::getInstance().registerRunParamUser(this, RPARAM_FILTERONENTITY.c_str());
    // TD 6950: Start receiving message informing GUI when system is degraded/recovered
    m_groupUpdateSubscriber->startNotifications();

	//TD17081++ moved the alarm data retrieval to the timer thread		
	//// Initialise the Alarm List Control
	//TA_Base_Bus::Filter preFilter;
	//std::vector<unsigned long> locations;    
	//initPreFiltering(preFilter, locations);
    //// TD 6950: Check if the system is currently in degraded mode. If so, clear the pre-filter.
    //updatePreFilterAccordingToSystemState(preFilter);
    //TA_Base_Bus::AlarmActionFactory* factory = 
	//	new TA_Base_Bus::AlarmActionFactory(initListControlActions());		
	//++TD17081

	initListControlColumns();

	//TD 3354: To take control of the column resize by subclass the m_listHeader class to the alarm list ctrl
	m_listHeader.SubclassDlgItem(0, &m_alarmList);

	//TD17081++ create the alarm list control first before retrieving all alarms
	//TA_Base_Bus::Filter filter;
	// m_alarmList.startAlarmList(*this, *factory, m_alarmColumns, preFilter, locations, TA_Base_Bus::COL_STATE, true, filter);
	if(RunParams::getInstance().isSet(RPARAM_KRTCALARMSORTING))
	{
		m_alarmList.setupAlarmList(*this, m_alarmColumns, TA_Base_Bus::COL_SOURCETIMESTAMP, false);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "sort by COL_SOURCETIMESTAMP");
	}
	else
	{
		m_alarmList.setupAlarmList(*this, m_alarmColumns, TA_Base_Bus::COL_STATE, true);
		LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugInfo, "sort by COL_STATE");
	}
	//m_hasAlarmListInit = true;
	//++TD17081

	//TD12744 / TD12743 / TD15005(4th) ++
	//From the m_alarmColumns.push_back() calls, COL_DESCRIPTION is 5th (or 4th???) item
	m_listHeader.expandColumnToFitListWidth(m_alarmList, 4, true);
	LOG(SourceInfo, TA_Base_Core::DebugUtil::GenericLog, TA_Base_Core::DebugUtil::DebugDebug, "TD12744 / TD12743 / TD15005(4th): expandColumnToFitListWidth() 1" );
	//++ TD12744 / TD12743 / TD15005(4th)

	// TD 13994 since this is after init of alarm list
	// establish the default printout width for each columns
	// Note: get the width from header and not columns as the print list also obtains info from header
	m_printoutColWidths = new int [m_listHeader.GetItemCount() + 1];
	for (int i=0 ; i<m_listHeader.GetItemCount(); i++ )
	{
		HDITEM hiWid;
		hiWid.mask = HDI_WIDTH;
		if ( m_listHeader.GetItem(i, &hiWid) )
		{
			m_printoutColWidths[i+1] = hiWid.cxy;
		}
	}

	PopulateAlarmToolbar();

	//m_alarmFilterDlg->Create(IDD_ALARM_FILTER); // moved to the timer thread
	//TD17081++ start timer thread to retrieve alarm data in background
//lsx SetTimer(ALARM_DATA_INITIALIZATION_TIMER, 0, NULL); UPDATE_TIMER
	SetTimer(UPDATE_TIMER, 1500, NULL);
	initializeAlarmList();
	m_progress.SetRange(0, 100);
	m_progress.SetStep(1);

	DisableButtons();

    // Resize the window to account for the toolbar that was just added
    CRect viewerRect;
    GetClientRect(&viewerRect);
    calculateWindowPlacements(viewerRect.Width(),viewerRect.Height());

	//TD16109++
	// Apply a filter on asset, if the FilterOnAsset run param is set.
	if(RunParams::getInstance().get(RPARAM_FILTERONASSET.c_str()) != "")
    {
        filterOnAsset(RunParams::getInstance().get(RPARAM_FILTERONASSET.c_str()));
		populateNamedFilters(RunParams::getInstance().get(RPARAM_FILTERONASSET.c_str()));
    }
	// Apply a filter on entity, if the FilterOnEntity run param is set.
	else if(RunParams::getInstance().get(RPARAM_FILTERONENTITY.c_str()) != "")
    {
        filterOnEntity(RunParams::getInstance().get(RPARAM_FILTERONENTITY.c_str()));
		populateNamedFilters(RunParams::getInstance().get(RPARAM_FILTERONENTITY.c_str()));
    }
	else
	{
		// Initialise the Named Filter combo box with default when first started
		populateNamedFilters(TA_Base_App::AlarmFilterDlg::FILTER_DEFAULT);
	}
	//++TD16109
	
}
DWORD WINAPI CAlarmsGuiDlg::ThreadFunction(LPVOID lpThreadParameter)//reduce init time
{
    FUNCTION_ENTRY( "ThreadFunction" );

    CAlarmsGuiDlg* CAG = (CAlarmsGuiDlg*) lpThreadParameter;
    CAG->setProfileId();

    FUNCTION_EXIT;
    return TRUE;
}

void CAlarmsGuiDlg::OnAppExit() 
{
	m_controlClass->saveAllUserSettings();
	RunParams::getInstance().deregisterRunParamUser(this);
	m_alarmList.DestroyWindow();
    DestroyWindow();
}
	


void CAlarmsGuiDlg::initializeAlarmList()
{
	CWaitCursor waitCursor;

	setCurrentDescriptionText("Loading of alarms is in progress.");

	TA_Base_Bus::Filter preFilter;
	std::vector<unsigned long> locations;    
	initPreFiltering(preFilter, locations);

	updatePreFilterAccordingToSystemState(preFilter);

	TA_Base_Bus::AlarmActionFactory* factory = 
	new TA_Base_Bus::AlarmActionFactory(initListControlActions());

	TA_Base_Bus::Filter filter;
	m_alarmList.startAlarmList( *factory, preFilter, locations, filter);
	m_hasAlarmListInit = true;
	
	m_alarmFilterDlg->Create(IDD_ALARM_FILTER);
}

void CAlarmsGuiDlg::OnClickAlarmList(NMHDR* pNMHDR, LRESULT* pResult)
{
	FUNCTION_ENTRY("OnClickAlarmList");

	//get which column we double clicked
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	int subItem = pNMListView->iSubItem;

	//get whether the column is the comment column
	bool bIsCommentColumn = false;
	LVCOLUMN lvcol;
	char  str[256];
	lvcol.mask = LVCF_TEXT;
	lvcol.pszText = str;
	lvcol.cchTextMax = 256;
	m_alarmList.GetColumn(subItem, &lvcol);
	CString strColumnCaption;
	strColumnCaption.Format("%s",lvcol.pszText);	
	if(m_alarmList.isDeterminateColumn(TA_Base_Bus::COL_COMMENTS, strColumnCaption))
	{
		bIsCommentColumn = true;
	}
	
	//if we double click the comment column, will popup the comment dialog
	if( true == bIsCommentColumn )
	{
		OnAlarmDisplayComment();
	}
	*pResult = 0;
	FUNCTION_EXIT;
}