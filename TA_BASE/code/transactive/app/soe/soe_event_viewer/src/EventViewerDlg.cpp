/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/EventViewerDlg.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #6 $
 *
 * Last modification: $DateTime: 2009/12/03 17:15:17 $
 * Last modified by:  $Author: miao.yu $
 * 
 * EventViewer is the TransactiveWinApp derived main application class. This is the class that bootstraps the application.
 */

#pragma warning(disable:4800)       // 'long': forcing value to bool - this warning is generated for legitimate static casts

#include <afxpriv.h>    // Included for the common controls
#include <math.h>       // Included for log10
#include <sstream>
#include <sys/timeb.h>
#include <algorithm>
#include "boost/lexical_cast.hpp"

#include "app/soe/soe_event_viewer/src/stdafx.h"
#include "app/soe/soe_event_viewer/src/EventViewer.h"
#include "app/soe/soe_event_viewer/src/EventViewerDlg.h"
#include "app/soe/soe_event_viewer/src/EventViewerGUI.h"
#include "app/soe/soe_event_viewer/src/FilterDlg.h"
#include "app/soe/soe_event_viewer/src/CombinedEventFilterDlg.h"
#include "app/soe/soe_event_viewer/src/CombinedEventFilter.h"
#include "app/soe/soe_event_viewer/src/WaitNotificationBox.h"
#include "app/soe/soe_event_viewer/src/DatabaseAccess.h"
//*zhiqiang++begin
#include "app/soe/soe_event_viewer/src/PopulateEventLevelsThread.h"
#include "app/soe/soe_event_viewer/src/ApplyRightsThread.h"
//*/ //zhiqiang++end

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/AppLauncher.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "core/data_access_interface/entity_access/src/ConfigurationEditor.h"
#include "core/data_access_interface/soe/src/SOEEvent.h"
#include "core/data_access_interface/src/NamedFilterAccessFactory.h"
#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/entity_access/src/SOEEventViewer.h"
#include "core/database/src/SimpleDb.h"
#include "core/exceptions/src/ApplicationException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DbConnectionFailed.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

#include "core/threads/src/Thread.h"

#include "core/data_access_interface/entity_access/src/EventViewer.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

// Use boost tokenizer for splitting up command line arguments.
#include "boost/tokenizer.hpp"

const int EventViewerDlg::STANDARD_MIN_HEIGHT = 205;
const int EventViewerDlg::STANDARD_MIN_WIDTH = 500;
const int EventViewerDlg::OFFSET_NO_STATUSBAR = 5;
const int EventViewerDlg::OFFSET_NO_TOOLBAR = 50;
const CString EventViewerDlg::IDLE_TEXT = "准备就绪";
const int EventViewerDlg::STRETCH_PANE = 0;
const CString EventViewerDlg::VIEW_MENU = "&View";
const CString EventViewerDlg::COLUMNS_MENU = "Columns";
// GF-23 : change font size
//const CString EventViewerDlg::FONT_NAME = "Arial Bold";
const CString EventViewerDlg::FONT_NAME = "宋体";
const int EventViewerDlg::FONT_SIZE = 100;

const CString EventViewerDlg::NAMED_FILTER_LABEL_TEXT = "当前  过滤 ";
const long    EventViewerDlg::NAMED_FILTER_LABEL_SEPARATOR_ID = 99995;
const int     EventViewerDlg::NAMED_FILTER_LABEL_SEPARATOR_WIDTH = 50; //change 60 to ...
const int     EventViewerDlg::NAMED_FILTER_LABEL_TOP = 6;
const int     EventViewerDlg::NAMED_FILTER_LABEL_OFFSET = 0;
const long    EventViewerDlg::NAMED_FILTER_COMBO_SEPARATOR_ID = 99996;
const int     EventViewerDlg::NAMED_FILTER_COMBO_SEPARATOR_WIDTH = 200;
const int     EventViewerDlg::NAMED_FILTER_COMBO_TOP = 10;
const int     EventViewerDlg::NAMED_FILTER_COMBO_OFFSET = 15;
const int     EventViewerDlg::NAMED_FILTER_COMBO_DROP_HEIGHT = 251;

const int     EventViewerDlg::EVENT_LEVEL_SEPARATOR_ID = 99997;
const CString EventViewerDlg::EVENT_LEVEL_LABEL_TEXT = "事件  级别 ";
const long    EventViewerDlg::EVENT_LEVEL_LABEL_SEPARATOR_ID = 99998;
const int     EventViewerDlg::EVENT_LEVEL_LABEL_SEPARATOR_WIDTH = 50; //change 50 to ...
const int     EventViewerDlg::EVENT_LEVEL_LABEL_TOP = 6;
const int     EventViewerDlg::EVENT_LEVEL_LABEL_OFFSET = 0;
const long    EventViewerDlg::EVENT_LEVEL_COMBO_SEPARATOR_ID = 99999;
const int     EventViewerDlg::EVENT_LEVEL_COMBO_SEPARATOR_WIDTH = 200;
const int     EventViewerDlg::EVENT_LEVEL_COMBO_TOP = 10;
const int     EventViewerDlg::EVENT_LEVEL_COMBO_OFFSET = 15;
const int     EventViewerDlg::EVENT_LEVEL_COMBO_DROP_HEIGHT = 251;

using TA_Base_Core::NamedFilter;
using TA_Base_Core::NamedFilterAccessFactory;
using TA_Base_Core::SOEEvent;
using TA_Base_Core::DataException;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_Core;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    // Number of seconds in 24 hours.
    const unsigned int TWENTY_FOUR_HOURS( 24*60*60 );

    // RunParam constants.
    const std::string RPARAM_FILTER_SUBSYSTEM( "FilterSubsystem" );
    const std::string RPARAM_FILTER_LOCATION( "FilterLocation" );
    const std::string RPARAM_FILTER_OPERATOR( "FilterOperator" );
    const std::string RPARAM_FILTER_EVENTTYPE( "FilterEventtype" );
    const std::string RPARAM_FILTER_ASSET( "FilterAsset" );
    const std::string RPARAM_FILTER_DESCRIPTION( "FilterDescription" );
    const std::string RPARAM_FILTER_FROM( "FilterFrom" );
    const std::string RPARAM_FILTER_TO( "FilterTo" );
    const std::string RPARAM_FILTER_SEVERITY( "FilterSeverity" );
    const std::string RPARAM_FILTER_ALARMTYPE( "FilterAlarmtype" );
    const std::string RPARAM_FILTER_ALARMID( "AlarmId" );
	const std::string RPARAM_FILTER_ALARMCHILD( "AlarmChild");
    const std::string RPARAM_FILTER_DEFAULT( "FilterDefault" );

    const std::string ERROR_LOAD( "delete" );
    const std::string ERROR_LOADED( "deleted" );
}

/////////////////////////////////////////////////////////////////////////////
// EventViewerDlg dialog
EventViewerDlg::EventViewerDlg(TA_Base_Bus::IGUIAccess& controlClass)
    : m_controlClass(controlClass), 
      TransActiveDialog(controlClass, EventViewerDlg::IDD, NULL)
      ,m_listWidthOffset(0),m_listTopOffset(0),m_listBottomOffset(0),
      m_filter(NULL),m_filterDlg(NULL), m_progressBarExists(false), m_refreshTime(0),
	  m_pPopulateEventLevelsThread(NULL), m_pApplyRightsThread(NULL) //zhiqiang++
{
	//{{AFX_DATA_INIT(EventViewerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);


    // Set the resizing properties
    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxHeight = -1;
    properties.maxWidth = -1;
    properties.minHeight = STANDARD_MIN_HEIGHT;
    properties.minWidth = STANDARD_MIN_WIDTH;
    setResizingProperties(properties);

}

EventViewerDlg::~EventViewerDlg()
{
    if (m_pApplyRightsThread != NULL)
	{
		delete m_pApplyRightsThread;
		m_pApplyRightsThread = NULL;
	}
	if (m_pPopulateEventLevelsThread != NULL)
	{
		delete m_pPopulateEventLevelsThread;
		m_pPopulateEventLevelsThread = NULL;
	}
}
void EventViewerDlg::init()
{
	FUNCTION_ENTRY("EventViewerDlg::init");//zhiqiang++

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventViewerDlg::init 1. ");
	// setup font object
	m_font.CreatePointFont( FONT_SIZE, FONT_NAME );
	m_listfont.CreatePointFont(110, FONT_NAME );
	
    // Construct the rights library.
    TA_Base_Bus::RightsLibraryFactory rlFactory;
    m_rightsLibrary = std::auto_ptr<TA_Base_Bus::RightsLibrary>( rlFactory.buildRightsLibrary(TA_Base_Bus::DEFAULT_RL, TA_Base_Bus::RightsLibraryFactory::MANAGER) ); 
	
    // Get the current connection string
    m_currentDbString = getCurrentDatabaseString();

	// Set the colour coding behaviour
	//li zhongyan 
	unsigned long entitykey = m_controlClass.getGUIEntity()->getKey();
	
	//TA_Base_Core::SOEEventViewer* guiEntity = new TA_Base_Core::SOEEventViewer(entitykey);
    TA_Base_Core::SOEEventViewer* guiEntity = dynamic_cast<TA_Base_Core::SOEEventViewer*>(m_controlClass.getGUIEntity());


	

    bool colourCodingEnabled(false);
    unsigned long maxEvents = 0;
	unsigned long defaultEventLevel = 0;
    m_refreshTime = 2000;
    try
    {
        colourCodingEnabled = guiEntity->areAlarmEventsColourCoded();
        maxEvents = guiEntity->getEventLimit();
        m_refreshTime = guiEntity->getEventRefreshTime()*1000;
		defaultEventLevel = guiEntity->getDefaultEventVisibilityLevel();
    }
    catch(...)
    {
        // If something goes wrong keep what was successful! Everything else
        // will have the defaults values set above.
    }  

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventViewerDlg::init 2. ");
	
    // Set initial window caption.
   
	
	// set events list box font
	m_eventList.SetFont(&m_listfont,FALSE);

    // Call this to set up the common controls - in this case Pager and CreateDialog
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC = ICC_PAGESCROLLER_CLASS|ICC_BAR_CLASSES;
    ::InitCommonControlsEx(&icex);

	GetDlgItem(IDC_PAGER_CTRL)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PAGE_UP)->ShowWindow(SW_SHOW);
	GetDlgItem(IDC_PAGE_DOWN)->ShowWindow(SW_SHOW);
	
    // Set up the GUI components.
    setUpStatusBar();
    setUpToolBar(); 
    setUpMenu();

    // Add styles to the list view
    m_eventList.SetExtendedStyle(LVS_EX_FULLROWSELECT       // Enable selecting a whole row when clicking anywhere in it
                                | LVS_EX_HEADERDRAGDROP     // Enable moving the order of columns
                                | LVS_EX_INFOTIP            // Enable tool tips when column text is too wide
                                | LVS_EX_GRIDLINES|0x00010000 );       // Show gridlines.
    
	//TD17143 - do not call showWindow here as it will be called right after the dialog is initialized
    // ShowWindow(SW_SHOW); 

    // Send a "Size" message to repaint the event list (invalidate doesn't work here for some reason)
    //CRect viewerRect;
    //GetWindowRect(&viewerRect);
    //ScreenToClient(&viewerRect);
    //this->PostMessage(WM_SIZE,viewerRect.Height()-1, viewerRect.Width()-1 );
	//++TD17143
	m_filter = new CombinedEventFilter();
	m_filter->setFilterID(IDD_COMBINED_EVENT_FILTER);


	// Populate the filter based on command line parameters.
	setFilterFromCommandLine();


	// Set the max events parameter in the filter
	m_filter->setMaxEvents(maxEvents);

	// Set the default event level.
	m_filter->setEventLevel( defaultEventLevel );

	// Set the filter in the list component
	m_eventList.setFilter(m_filter);

    // Set the default refresh period (in milliseconds).
    m_eventList.setRefreshPeriod(m_refreshTime);

    // Set the colour coding behaviour
    m_eventList.setColourCoding(colourCodingEnabled);
	setWindowCaption();

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventViewerDlg::init 3. ");

    // And finish off by initiating the default event load with the appropriate data
    //loadEvents(true);

    // Start listening for RunParam changes.
    RunParams& runParams = RunParams::getInstance();
    runParams.registerRunParamUser( this, RPARAM_SESSIONID );
    runParams.registerRunParamUser( this, RPARAM_FILTER_SUBSYSTEM.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_LOCATION.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_OPERATOR.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_EVENTTYPE.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_ASSET.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_DESCRIPTION.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_FROM.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_TO.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_SEVERITY.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_ALARMTYPE.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_ALARMID.c_str() );
	runParams.registerRunParamUser( this, RPARAM_FILTER_ALARMCHILD.c_str() );
    runParams.registerRunParamUser( this, RPARAM_FILTER_DEFAULT.c_str() );

    // Load the db connection strings
    std::string dbConnectionFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
    TA_Base_Core::DbConnectionStrings dbConnectionStrings(dbConnectionFile);

	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "EventViewerDlg::init 4. ");

    try
    {
        std::vector<TA_Base_Core::DataConnection> dataConnections = dbConnectionStrings.getConnectionList(TA_Base_Core::Event_Ad, TA_Base_Core::Read);
        std::vector<TA_Base_Core::DataConnection>::iterator iter = dataConnections.begin();
        for(; iter != dataConnections.end(); ++iter)
        {
            std::string tempString = (*iter).first;
            
            std::ostringstream str;
            str << RPARAM_DBPREFIX << tempString;

            runParams.registerRunParamUser( this, str.str().c_str() );
        }
    }
    catch(const TA_Base_Core::DbConnectionFailed& ex)
    {
        // If there was a failure getting the connection strings, this isn't catastrophic, but it 
        // means that we won't be able to be notified when a database goes down or up.
        LOG_EXCEPTION_CATCH(SourceInfo, "DbConnectionFailed", ex.what());
        LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Cannot listen for database status changes");
    }

	//TD17143 - post update here once everything else is initialized already
    PostMessage(WM_USER_UPDATE_EVENTFILTERS_RIGHTS);

	//delete guiEntity;

    FUNCTION_EXIT; //zhiqiang++

}

void EventViewerDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActiveDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EventViewerDlg)
	DDX_Control(pDX, IDC_PAGE_DOWN, m_pageDown);
	DDX_Control(pDX, IDC_PAGE_UP, m_pageUp);
	DDX_Control(pDX, IDC_EVENT_LIST, m_eventList);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EventViewerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(EventViewerDlg)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_CLOSE()
	ON_COMMAND(ID_FILE_EXIT, OnFileExit)
	ON_COMMAND(IDC_VIEW_FILTERS, OnViewFilters)
	ON_COMMAND(IDC_STATUS_BAR, OnStatusBar)
	ON_COMMAND(IDC_TOOL_BAR, OnToolBar)
    ON_COMMAND(IDC_SHOW_ALL, OnShowAll)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
    ON_COMMAND(ID_HELP, OnHelp)
	ON_WM_MENUSELECT()
	ON_BN_CLICKED(IDC_PAGE_UP, OnPageUp)
	ON_BN_CLICKED(IDC_PAGE_DOWN, OnPageDown)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
	//zhiqiang++begin
	ON_MESSAGE(WM_USER_APPLY_RIGHTS, OnApplyRights) 
    //zhiqiang--end
    ON_MESSAGE(WM_USER_LIST_UPDATE,OnListUpdate)
	ON_MESSAGE(WM_USER_LISTVIEW_UPDATE,OnListViewUpdate)
    ON_MESSAGE(WM_USER_LOAD_COUNT,OnLoadListCount)
    ON_MESSAGE(WM_USER_LOAD_UPDATE,OnLoadListUpdate)
    ON_MESSAGE(WM_USER_FILTER_DLG_CLOSED,OnFilterDlgClose)
    ON_MESSAGE(WM_USER_FILTER_DLG_APPLY,OnFilterApply)
    ON_MESSAGE(WM_USER_FILTER_DLG_CANCEL,OnFilterDlgCancel)
    ON_MESSAGE(WM_USER_FILTER_DLG_NAMED_FILTER_CHANGE,OnNamedFilterChange)
    ON_MESSAGE(WM_USER_CLOSE_MAIN_WINDOW,OnCloseMainWindow)
    ON_MESSAGE(WM_USER_LIST_SELECTION_UPDATE,OnListItemSelect)
    ON_COMMAND(IDC_NEXT, OnNext)
    ON_COMMAND(IDC_PREVIOUS, OnPrevious)
    ON_COMMAND(IDC_PRINT, OnPrint)
    ON_COMMAND(IDC_EXPORT, OnExport)
    ON_COMMAND(IDC_ALARM_MANAGER, OnAlarmManager)
    ON_COMMAND(IDC_VIEW_INCIDENTS, OnIncidentReport)
	ON_CBN_SELCHANGE(ID_NAMED_FILTER_COMBO, OnSelchangeComboNamedFilter)
	ON_MESSAGE(WM_USER_UPDATE_EVENTFILTERS_RIGHTS, OnSetupEventFiltersAndRights) //TD17143
END_MESSAGE_MAP()


// This array is used by the status bar to set up the two panes it contains.
static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_SIZE
};

// This array is used by the status bar to set up the two panes it contains.
static UINT threeIndicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_CAP,
    ID_INDICATOR_SIZE
};

/////////////////////////////////////////////////////////////////////////////
// EventViewerDlg message handlers

BOOL EventViewerDlg::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog");
	TransActiveDialog::OnInitDialog();

    // Set the icon for this dialog.  The framework does this automatically
    //  when the application's main window is not a dialog
    SetIcon(m_hIcon, TRUE);			// Set big icon
    SetIcon(m_hIcon, FALSE);		// Set small icon

	GetDlgItem(IDC_PAGER_CTRL)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PAGE_UP)->ShowWindow(SW_HIDE);
	GetDlgItem(IDC_PAGE_DOWN)->ShowWindow(SW_HIDE);	

    FUNCTION_EXIT;
    return TRUE;
}

//TD17143 
void EventViewerDlg::OnShowWindow(BOOL bShow, UINT nStatus)
{
	// call invalidate so that the list will be repainted
	CRect viewerRect;
    GetWindowRect(&viewerRect);
    ScreenToClient(&viewerRect);
	PostMessage(WM_SIZE,viewerRect.Height()-1, viewerRect.Width()-1 );
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void EventViewerDlg::OnPaint()
{
    FUNCTION_ENTRY("OnPaint");
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
        TransActiveDialog::OnPaint();
    }
    FUNCTION_EXIT;
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR EventViewerDlg::OnQueryDragIcon()
{
    FUNCTION_ENTRY("OnQueryDragIcon");
    FUNCTION_EXIT;
    return (HCURSOR) m_hIcon;
}

void EventViewerDlg::initialiseWindowOffsets()
{
    // TD2038: The code in this method was taken out of OnSize() because it
    // needs to be called prior OnSize() being called from a Control Station call.

    // Get the main window rectangle - it will be used when resizing most of the components
    CRect window;        
    GetWindowRect(&window);
    ScreenToClient(&window);

    CWnd* listCtrl = GetDlgItem(IDC_EVENT_LIST);
    if (NULL != listCtrl)
    {
        // Get the rectangle for the list control
        CRect list;
        listCtrl->GetWindowRect(&list);
        ScreenToClient(&list);

        if (0 == m_listWidthOffset && 0 == m_listTopOffset && 0 == m_listBottomOffset)
        {
            // if all three variables are 0, then the list control has not yet been adjusted

            // need to work out the offsets so they can be maintained, no matter how
            // the dialog is resized
            m_listWidthOffset = list.left - window.left;
            m_listTopOffset = list.top - window.top;
            m_listBottomOffset = window.bottom - list.bottom;
        }
    }
}

// Called when the viewer is re-sized
void EventViewerDlg::OnSize(UINT nType, int cx, int cy) 
{
    FUNCTION_ENTRY("OnSize");
    // Let the CDialog base class do any re-sizing processing it needs
	TransActiveDialog::OnSize(nType, cx, cy);

    // Get the main window rectangle - it will be used when resizing most of the components
    CRect window;        
    GetWindowRect(&window);
    ScreenToClient(&window);


    // Start by resizing the menu line
    CWnd* menuLine = GetDlgItem(IDC_MENU_LINE);
    if (NULL != menuLine)
    {
        // Get the rectangle for the menu line
        CRect line;
        menuLine->GetWindowRect(&line);
        ScreenToClient(&line);

        // Don't care about it's height - it's just a line. Set the line width bounds to the same as the main applicaiton window
        line.right = window.right;
        line.left = window.left;
        menuLine->MoveWindow(&line);
    }

    // Resize the event list
    CWnd* listCtrl = GetDlgItem(IDC_EVENT_LIST);
    if (NULL != listCtrl)
    {
        // Get the rectangle for the list control
        CRect list;
        listCtrl->GetWindowRect(&list);
        ScreenToClient(&list);

        // If the status bar is not visible, want to reduce the bottom offset
        unsigned int bottomOffset = m_listBottomOffset;
        if (NULL != m_statusBar.GetSafeHwnd())        
        {
            // Fix for TD2038: Check if the status bar is NULL before calling IsWindowVisible()
            if(!m_statusBar.IsWindowVisible())
            {
                bottomOffset = OFFSET_NO_STATUSBAR;
            }
            else
            {
                CRect status;
                m_statusBar.GetWindowRect(&status);
                ScreenToClient(&status);

                bottomOffset = m_listBottomOffset + status.Height();
            }
        }
        
        // If the tool bar is not visible, want to reduce the top offset
        unsigned int topOffset = m_listTopOffset;
        if (NULL != m_eventsToolbar.GetSafeHwnd() && !m_eventsToolbar.IsWindowVisible())            
        {
            topOffset = OFFSET_NO_TOOLBAR;
        }

        // And now resize the list to the offsets
        list.left = window.left + m_listWidthOffset;
        list.right = window.right - m_listWidthOffset;
        list.bottom = window.bottom - bottomOffset;
        list.top = window.top + topOffset;

        listCtrl->MoveWindow(list);
    }


    // Resize the pager control (with the toolbar in it)

    if(IsWindow(m_eventsToolbar.GetSafeHwnd()))
	{
        // The value provided by cx is not current as the left value seems
        // to be negative... manually calculate it instead.
        int realx = (( window.left < 0 )?-window.left:window.left) + window.right;

        // Get the rectangle fo the pager control
	    CRect pagerRect;
	    m_Pager.GetWindowRect(pagerRect);
	    ScreenToClient(pagerRect);

        // Reposition the page up and page down buttons.
        CRect pageUpRect;
        m_pageUp.GetWindowRect(pageUpRect);
        ScreenToClient(pageUpRect);

        pageUpRect.right = realx - 23;
        pageUpRect.left = pageUpRect.right - 26;
	    m_pageUp.MoveWindow(pageUpRect);
        
        CRect pageDownRect;
        m_pageDown.GetWindowRect(pageDownRect);
        ScreenToClient(pageDownRect);

        pageDownRect.right = realx - 23;
        pageDownRect.left = pageDownRect.right - 26;
        m_pageDown.MoveWindow(pageDownRect); 
        
        // Set the right limit of the pager (the left will always remain the same)
	    pagerRect.right = realx - 50;	    
	    m_Pager.MoveWindow(pagerRect);
    }
    
	// Resizing the status bar is automatically handled by the RepositionBars command
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

    FUNCTION_EXIT;
}


// OnNotify is called when when a windows common control wants to send a message to the controling application
BOOL EventViewerDlg::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
    // This #define allows access to the header part of the lParam
    #define lpnm   ((LPNMHDR)lParam)


    if(lpnm->code == PGN_CALCSIZE)  // This message is requesting the size of the window (toolbar) contained in the pager
	{
        // cast the lParam to a pointer of the MPGCALCSIZE object
	    LPNMPGCALCSIZE   pCalcSize = (LPNMPGCALCSIZE)lParam;	

        // Determine the number of buttons contained in the toolbar
	    int buttonCount;		
		buttonCount = m_eventsToolbar.GetButtonCount();

		// Get the bonding rectangle for the far right toolbar button
	    CRect buttonRect;
		m_eventsToolbar.GetItemRect(buttonCount - 1, buttonRect);

        // Determine if the control is requesting hieght of width information
		switch(pCalcSize->dwFlag)
		{
		case PGF_CALCWIDTH:                                 // Width
			pCalcSize->iWidth = buttonRect.right;           // Set the width to the right hand side of the far-right button
			return 0;
			break;

		case PGF_CALCHEIGHT:                                // Height
			pCalcSize->iHeight = buttonRect.bottom;         // Set the height to the bottom of the button
			return 0;
			break;			
		}

		return FALSE;                                       // Unknown message request, so return FALSE (i.e. not valid data)
	}
    else if(lpnm->code == PGN_SCROLL)   // This message comes from the pager, and allows the application to set how far to scroll
    {
        LPNMPGSCROLL   pScroll = (LPNMPGSCROLL)lParam;

        switch(pScroll->iDir)
        {
        case PGF_SCROLLLEFT:
        case PGF_SCROLLRIGHT:
        case PGF_SCROLLUP:
        case PGF_SCROLLDOWN:
            pScroll->iScroll = 15;     // For any scroll direction, scroll 15 pixels at a time.
            break;
        }
    }
    else if (lpnm->code == TTN_GETDISPINFO) // This message comes from the toolbar when the mouse has "floated" over a button
    {                                       // for longer than the timeout period, and it wants to diaplay a tooltip.
        // Return the text associated with the currently "hot" button
        LPNMTTDISPINFO pDispInfo = (LPNMTTDISPINFO)lParam;

        // The NMTTDISPINFO struct has two items that need to be set - lpszText, which is the tooltip text to display, and
        // hinst which is the application instance that contains the toolbar requesting the tooltip.
        pDispInfo->lpszText = (LPTSTR) lpnm->idFrom;    // idFrom is the applicaiton ID of the control that sent the message -
                                                        // in this case, the button command ID iteslf, which is useful, as these
                                                        // have strings stored against them in the string table, so just cast
                                                        // it to a LPTSTR and the entry for the currently "hot" button in the
                                                        // string table will be displayed.
        pDispInfo->hinst = AfxGetInstanceHandle();
    }
    else if (lpnm->code == TBN_HOTITEMCHANGE) // This message comes from the toolbar when the mouse moves into (or out of) a 
    {                                         // toolbar button. It is used here to put a message in the status bar about the 
                                              // button
        LPNMTBHOTITEM pHotItem = (LPNMTBHOTITEM)lParam;

        // This is the default message - if we can't get one, then display this.
        CString strMsg = IDLE_TEXT;

        // Bitwise OR - only want to load the string if the mouse is NOT leaving the toolbar (i.e. it is entering, or already
        // in). So bitwise AND the item flags with NOT(~) HICF_LEAVING, the flag indicating the mouse has left the button.
        if (pHotItem->dwFlags & ~HICF_LEAVING)
        {
            // Load the description from the string table - similar to the LTPSTR cast from above, but safer.
            strMsg.LoadString(pHotItem->idNew);
        }
        
        // If the "hot item" is a disabled button, don't show the text ('cause we won't be notified when leaving
        // it, meaning the text can't be erased).
        if(!m_eventsToolbar.IsButtonEnabled(pHotItem->idNew))
        {
            // Set it back to the idle text
            strMsg = IDLE_TEXT;
        }

        // If the string was not filled in (i.e. the string table is empty, or the ID was not found)
        if (strMsg.GetLength() == 0)
        {
            // Set it back to the idle text
            strMsg = IDLE_TEXT;
        }

        // And set the status bar to display the message
        //m_statusBar.SetWindowText(strMsg);
    }

	return TransActiveDialog::OnNotify(wParam, lParam, pResult);
}

void EventViewerDlg::OnClose() 
{  
    FUNCTION_ENTRY("OnClose");
    // Need to handle closing of this dialog carefully. If the list is currently loading events, there is no way 
    // to stop it, and deleting a thread that is currently active gives undefined results, so just have to wait for it to 
    // finish.

    // In order that the user realises their "close me" request has been registered, pop up a dialog box to tell them to wait
    // while it closes down.

    // Use the Windows Platform SDK call CreateDialog here instead of creating a CDialog dialog, as getting a CDialog to
    // attach itself to the desktop is difficult. The advantage of attaching this window to the desktop is that even when this
    // method goes out of scope, it remains visible, up until the applications GUI resources are cleaned up, which means that
    // it will be visible essentially until the application has removed itself from memory, which with the number of database
    // threads and objects that are in the Event Viewer can be a number of seconds after the main dialog disapears from view.
    //
    // AL : 3/2/04 : Commented out for TD 1182
    //
    //HWND dlg = CreateDialog(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDD_WAIT_FOR_CLOSE),GetDesktopWindow()->GetSafeHwnd(),NULL);
    //::ShowWindow(dlg,SW_SHOW);

    // Attach the dialog to a CWnd object - that way we can centre it easily.
    //CWnd* dlgWindow = new CWnd();
    //dlgWindow->Attach(dlg);
    //dlgWindow->CenterWindow();

    // Disable the main applicaiton window - stops users from clicking close twice, or doing any other action once close is clicked
    EnableWindow(FALSE);

    // Shutdown and clear the list. Shutting down the list is the action that can take quite some time (it has to wait for any
    // load to finish first).
    m_eventList.shutdownList(); 
    m_eventList.clearList();
	
    // Delete the filter object (if it exists)
    if (m_filter != NULL)
    {	
   		m_filter->terminateThread();
		delete m_filter;
        m_filter = NULL;
    }

    // And close the dialog, and the applicaiton.
	//TD18095, jianghp, to fix the performance of showing manager application
	TransActiveDialog::DestroyWindow();

    FUNCTION_EXIT;
}

LRESULT EventViewerDlg::OnCloseMainWindow(WPARAM,LPARAM)
{
    FUNCTION_ENTRY("OnCloseMainWindow");
    // OnCloseMainWindow is called from the message map when the control station attempts to close the application. It is done 
    // this way, because the incoming CORBA thread from the control station can't be used to create dialog boxes, which is 
    // precisely what we want to do - so a two-stage process is necessitated.

    // As in OnClose, create a dialog box to notify the user the application is closing. See the comment in OnClose for why
    // the Windows SDK CreateDialog method was used.
    //
    // AL : 3/2/04 : Commented out for TD 1182
    //
    //HWND dlg = CreateDialog(AfxGetInstanceHandle(),MAKEINTRESOURCE(IDD_WAIT_FOR_CLOSE),GetDesktopWindow()->GetSafeHwnd(),NULL);
    //::ShowWindow(dlg,SW_SHOW);
    //CWnd* dlgWindow = new CWnd();
    //dlgWindow->Attach(dlg);
    //dlgWindow->CenterWindow();

    EnableWindow(FALSE);

    m_eventList.shutdownList(); 

    m_eventList.clearList();
	
    if (m_filter != NULL)
    {
		m_filter->terminateThread();
	    delete m_filter;
        m_filter = NULL;
    }
    
    // The only difference here is that we don't want to call OnClose() - as the dialog will be destroyed by the GenericGUI
    // framework, not us.

    FUNCTION_EXIT;
    return 0;
}

bool EventViewerDlg::isReadyToClose()
{
    FUNCTION_ENTRY("isReadyToClose");
    // This is the second method in the 2-stage control-station initiated shutdown procedure. It lets the control station know
    // when it is safe to actually destroy the application.

    // It is safe to destroy the applicaiton ONLY when thre are no events in the event list, and the event list is not
    // currently in the process of loading more.
    FUNCTION_EXIT;
    return (m_eventList.GetItemCount() == 0 && !m_eventList.isActive());
}

void EventViewerDlg::OnFileExit() 
{
    FUNCTION_ENTRY("OnFileExit");
    // File->Exit has been selected. Treat that the same as clicking the "X" in the top-right corner.
	//TD18095, jianghp, to fix the performance of showing manager application
	TransActiveDialog::DestroyWindow();
    FUNCTION_EXIT;
}

void EventViewerDlg::OnMenuSelect(UINT nItemID,UINT nFlags, HMENU hSysMenu)
{
    FUNCTION_ENTRY("OnMenuSelect");
    // Want to display a helper message in the status bar for each menu item. Have to do that manually for 
    // a dialog, so here it is.

    // If nFlags == 0xFFFF and hSysMenu == 0, then the menu has been closed - i.e., set it back to "Ready"
    if (nFlags == 0xFFFF && hSysMenu == 0)
    {
        //m_statusBar.SetWindowText(IDLE_TEXT);
    }
    // Or, if we're over a separator, or a in a popup, or any of these things, then want to keep displaying
    // what is currently displayed
	else if (nItemID == 0 ||
			 nFlags & (MF_SEPARATOR|MF_POPUP|MF_MENUBREAK|MF_MENUBARBREAK))
	{
		// Don't change what is currently displayed
	}
    // Or if the system menu is selected
	else if (nItemID >= 0xF000 && nItemID < 0xF1F0)
	{
        CString strMsg;
		// special string table entries for system commands
        if (strMsg.LoadString(ID_COMMAND_FROM_SC(nItemID)) != 0)
        {
			m_statusBar.SetWindowText(strMsg);
        }
	}
	else
	{
		// set the message string
        CString strMsg;
		if (strMsg.LoadString(nItemID) != 0)    // Load the string from the string table (a return of 0 is a failure)
        {
			m_statusBar.SetWindowText(strMsg);
        }
	}
    FUNCTION_EXIT;
}

void EventViewerDlg::PreSubclassWindow() 
{
    FUNCTION_ENTRY("PreSubclassWindow");
    // Called before OnInitDialog

    // Create a new filter object - this might be overridden later
	TransActiveDialog::PreSubclassWindow();
    FUNCTION_EXIT;
}

LRESULT  EventViewerDlg::OnListUpdate(WPARAM size,LPARAM loadFlags)
{
    FUNCTION_ENTRY("OnListUpdate");

    // Called when the event list notifies the main window the list has changed in size. The event list will not 
    // automatically update itself - this gives the application the ability to control when and how view updates occur

    // If the progress bar is currently active update its value.
    if ( m_progressBarExists )
    {
        m_statusBar.setProgressBarPos( size );
    }


    // Then set the text showing the event count
    std::ostringstream sizeStr;
    //sizeStr << "Event Count: " << size;
	sizeStr << "事件条数: " << size;
    if ( size >= m_filter->getMaxEvents() )
    {
        sizeStr << "+";
    }
    m_statusBar.SetPaneText( m_statusBar.CommandToIndex(ID_INDICATOR_SIZE), sizeStr.str().c_str() );

    // And set the STRETCH property on the left-most pane - that will make the left pane take up all the status bar
    // not used by any other panes.
	m_statusBar.SetPaneInfo( STRETCH_PANE, m_statusBar.GetItemID(STRETCH_PANE), SBPS_STRETCH, NULL );

    // Resize the "list-size" status-bar pane to enable it to display the event count correctly
    m_statusBar.SetPaneInfo( m_statusBar.CommandToIndex(ID_INDICATOR_SIZE), // The index of the pane in the status bar
                             ID_INDICATOR_SIZE,  // The ID to assign to this (same as it is currently, but method requires it)
                             SBPS_NORMAL,        // Sytle of the pane - again, not change, but this method requires it
							 90 + ( log10( (double)size + 1.1 ) + 1 ) *9 ); // The size of the pane - 9 times the size of the number plus a constant, > 0

    // Force progress bar to resize.
    m_statusBar.resizeProgressBar();

	 
    FUNCTION_EXIT;
    return 0;
}

LRESULT  EventViewerDlg::OnListViewUpdate(WPARAM size,LPARAM loadFlags)
{
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnListViewUpdate begin ");
    FUNCTION_ENTRY("OnListViewUpdate");
	// Then set the text showing the event count
    std::ostringstream sizeStr;
    //sizeStr << "Event Count: " << size;
	sizeStr << "事件条数: " << size;
    if ( size >= m_filter->getMaxEvents() )
    {
        sizeStr << "+";
    }
    m_statusBar.SetPaneText( m_statusBar.CommandToIndex(ID_INDICATOR_SIZE), sizeStr.str().c_str() );

    // And set the STRETCH property on the left-most pane - that will make the left pane take up all the status bar
    // not used by any other panes.
	m_statusBar.SetPaneInfo( STRETCH_PANE, m_statusBar.GetItemID(STRETCH_PANE), SBPS_STRETCH, NULL );

    // Resize the "list-size" status-bar pane to enable it to display the event count correctly
    m_statusBar.SetPaneInfo( m_statusBar.CommandToIndex(ID_INDICATOR_SIZE), // The index of the pane in the status bar
                             ID_INDICATOR_SIZE,  // The ID to assign to this (same as it is currently, but method requires it)
                             SBPS_NORMAL,        // Sytle of the pane - again, not change, but this method requires it
                            90 + ( log10( (double)size + 1.1 ) + 1 )*9 ); // The size of the pane - 9 times the size of the number plus a constant

    // Force progress bar to resize.
    m_statusBar.resizeProgressBar();
    unsigned long flags = LVSICF_NOSCROLL|LVSICF_NOINVALIDATEALL;
    flags &= ~LVSICF_NOINVALIDATEALL;

	// update event lists
	if( loadFlags & Filter::MASK_UPDATE_LIST )
	{
		m_eventList.SetItemCountEx( size );
		m_eventList.Invalidate();
		m_eventList.UpdateWindow();
	}


	if( DisplayItem::s_sortColumn == CombinedEventFilter::COLUMN_SOURCE_DATETIME )
	{
		// page up/down is only available when sorting DateTime
		if ( loadFlags & Filter::MASK_PAGEUP )
		{
			m_pageUp.EnableWindow(TRUE);
		}
		else
		{
			m_pageUp.EnableWindow(FALSE);
		}
		if ( loadFlags & Filter::MASK_PAGEDOWN )
		{
			m_pageDown.EnableWindow(TRUE);
		}
		else
		{
			m_pageDown.EnableWindow(FALSE);
		}
	}
	else
	{
        m_pageUp.EnableWindow(FALSE);
        m_pageDown.EnableWindow(FALSE);
	}




	// loading data
	if( loadFlags & Filter::MASK_LOADING )
	{
		if( loadFlags & Filter::MASK_SORTING )
		{
			m_statusBar.SetWindowText( "载入完成, 排序中 ..." );
		}
		else
		{
			m_statusBar.SetWindowText( "载入数据中 ..." );
		}
	}
	else if( loadFlags & Filter::MASK_REFRESH )
	{
		if( loadFlags & Filter::MASK_SORTING )
		{
			m_statusBar.SetWindowText( "刷新完成, 排序中 ..." );
		}
		else
		{
			m_statusBar.SetWindowText( "获取新数据中 ..." );
		}
	}
	else
	{
		m_statusBar.SetWindowText( "载入数据完毕" );
	}


	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "OnListViewUpdate end ");

	FUNCTION_EXIT;
	return 0;
}
LRESULT  EventViewerDlg::OnLoadListCount(WPARAM count,LPARAM text)
{
    FUNCTION_ENTRY("OnLoadListCount");
    // This is called when the Event List notifies the main application that a load has begun, and approximately how many
    // events it is expecting to load.
    if(m_progressBarExists)
    {
        m_statusBar.removeProgressBar();
        m_progressBarExists = false;
    }

    if ( 0 < count )
    {
        // Start the progress bar in the SEPARATOR pane
        m_statusBar.startProgressBar(ID_SEPARATOR);
        m_progressBarExists = true;

        // Set its range from 0 to count
        m_statusBar.setProgressRange(0,count);
    }
    else
    {
        if ( NULL == text )
        {
            m_statusBar.SetWindowText(IDLE_TEXT);
        }
        else
        {
            m_statusBar.SetWindowText( reinterpret_cast<char*>(text) );
        }
    }

    FUNCTION_EXIT;
    return 0;
}

LRESULT  EventViewerDlg::OnLoadListUpdate(WPARAM count,LPARAM)
{
    // BACTODO: Remove this method entirely.
    FUNCTION_ENTRY("OnLoadListUpdate");
    // This is called when the Event List notifies the main application that "count" events have been loaded since the last
    // call to this method. So increment the progress bar accordingly.
    m_statusBar.incrementProgressBar(count);

    FUNCTION_EXIT;
    return 0;
}

void EventViewerDlg::OnViewFilters() 
{
    FUNCTION_ENTRY("OnViewFilters");	
    // Enbale the wait cursor - loading the information from the database could take a perceptable amount of time.
    CWaitCursor waitCursor;

    // The show filters is a toggle button.
    // If it has been toggled on, then need to create the filter dialog based on what filter we currently have selected.
    // If the button has just been toggled on, the filter dialog member variable will be null, so test on that.
    if (NULL == m_filterDlg)
    {
        // Pause the loading if it is currently loading events
        m_filter->pauseCurrentListDataLoad();
        m_filter->pauseRefresh();//changxing
        
        // If the current filter is for audit events, then create the dialog for that type.
        if (IDD_COMBINED_EVENT_FILTER == m_filter->getFilterID())
        {
            // Construct it as a CombinedEventFilterDlg, allowing access to the dialog creation methods
            CombinedEventFilterDlg* filterDlg = new CombinedEventFilterDlg(m_filter);
            
            // Create and show the window
            filterDlg->Create(IDD_COMBINED_EVENT_FILTER);
            filterDlg->ShowWindow(SW_SHOW);
            filterDlg->CenterWindow(this);

            // Assign the filter dialog to the member variable. Need to cast it down to the base type.
            m_filterDlg = dynamic_cast<FilterDlg*> (filterDlg);
        }

        if (NULL != m_filterDlg)
        {
            // If the filter box was created successfully, check the menu and toolbar as necessary
	        GetMenu()->CheckMenuItem(IDC_VIEW_FILTERS,MF_CHECKED);
	        m_eventsToolbar.CheckButton(IDC_VIEW_FILTERS, true);
        }
        else
        {
            // If not, make sure they stay unchecked
	        GetMenu()->CheckMenuItem(IDC_VIEW_FILTERS,MF_UNCHECKED);
	        m_eventsToolbar.CheckButton(IDC_VIEW_FILTERS, false);
        }
		m_filter->resumeRefresh();//changxing
    }
    else
    {
        // The button has been toggled off, so need to destroy the filter window. 
        // FilterDlg does not acutally subclass from CDialog, so need to cast the filter back to a CDialog
        // This requires a reinterpret cast, but it should be safe, as the assigning code is in this method as well.
        CDialog* filterDlg = reinterpret_cast<CDialog*> (m_filterDlg);
		//Begin-- TD7268 LiDan 29.5.2007
		//Pause the loading
		m_filter->pauseCurrentListDataLoad();
		m_filter->pauseRefresh();

        // TD11568: Make sure the dialog is not destroyed if it is disabled.
        if ( filterDlg->IsWindowEnabled() )
        {
            filterDlg->ShowWindow(SW_HIDE);

            delete m_filterDlg;
            m_filterDlg = NULL;

	        GetMenu()->CheckMenuItem(IDC_VIEW_FILTERS,MF_UNCHECKED);
	        m_eventsToolbar.CheckButton(IDC_VIEW_FILTERS, false);

            // And enable or disable the updating button and combo box, and the "Refresh" button as necessary
            if ( Filter::CURRENT_TIME == m_filter->getToTime() )
            {
                m_eventsToolbar.EnableButton(IDC_NEXT,false);
                m_eventList.setRefreshPeriod( m_refreshTime );
            }
            else
            {
                m_eventsToolbar.EnableButton(IDC_NEXT);
                m_eventList.setRefreshPeriod(0);
            }
        }
        else
        {
            // Window is disabled so was not destroyed, back sure the button state is consistent.
            GetMenu()->CheckMenuItem(IDC_VIEW_FILTERS,MF_CHECKED);
            m_eventsToolbar.CheckButton(IDC_VIEW_FILTERS,true);
        }
		//Begin-- TD7268 LiDan 29.5.2007
		//Resume the loading
		m_filter->resumeCurrentListDataLoad();
		m_filter->resumeRefresh();
    }

    setWindowCaption();

    FUNCTION_EXIT;
}


LRESULT  EventViewerDlg::OnFilterDlgClose(WPARAM,LPARAM)
{
    FUNCTION_ENTRY("OnFilterDlgClose");
    // When the filter dialog closes, clear the event list, because we are loading a new filter
    m_eventList.clearList();

    // Disable the filter menu checkmark, and uncheck the button.
    OnViewFilters();

    // Update the named filter combo box.
    populateNamedFilters();

    // Reload the event view
    loadEvents(false);

    FUNCTION_EXIT;
    return 0;
}


LRESULT  EventViewerDlg::OnFilterApply(WPARAM,LPARAM)
{
    FUNCTION_ENTRY("OnFilterApply");
    // When filters are applied, clear the event list, because we are loading a new filter.
    m_eventList.clearList();

    // Enable or disable the updating button and combo box, and the "Refresh" button as necessary
    if (0 == m_filter->getToTime())
    {
        m_eventsToolbar.EnableButton(IDC_NEXT,false);
    }
    else
    {
        m_eventsToolbar.EnableButton(IDC_NEXT);
        m_eventList.setRefreshPeriod(0);
    }

    // Set the title bar to show what the current filters are
	// yanrong++ CL-15055
//    std::string windowText(RunParams::getInstance().get(RPARAM_APPNAME));
//    std::string filterText(m_filter->getCurrentFilterString());
//
//    if (0 < filterText.length())
//    {
//        windowText += filterText;
//        SetWindowText(CString(windowText.c_str()));
//    }
	setWindowCaption();
	// ++yanrong CL-15055

    // Update the named filter combo box.
    populateNamedFilters();

    // Finally, trigger a reload to use the new filter.
    loadEvents(false);

    FUNCTION_EXIT;
    return 0;
}

LRESULT  EventViewerDlg::OnFilterDlgCancel(WPARAM,LPARAM)
{
    FUNCTION_ENTRY("OnFilterDlgCancel");

    // Close the fitler dialog, disable the menu checkmark, and uncheck the toolbar button
    OnViewFilters();

    FUNCTION_EXIT;
    return 0;
}

LRESULT EventViewerDlg::OnNamedFilterChange(WPARAM,LPARAM)
{
    FUNCTION_ENTRY("OnNamedFilterChange");

    // A named filter has been saved/deleted so we need to refresh the contents.
    populateNamedFilters();

    FUNCTION_EXIT;
    return 0;
}

LRESULT EventViewerDlg::OnListItemSelect(WPARAM wParam,LPARAM lParam)
{
    return 0;
}

void EventViewerDlg::OnNext()
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"OnNext come in");
	bool enableRefresh=false;
    // The "Next" button has been clicked
    time_t toTime = m_filter->getToTime();
    if ( Filter::CURRENT_TIME == toTime )
    {
        // The "current" flag is set. This method should not have been called, so put up a message box and return
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UI_040021);
        return;
    }
    time_t fromTime = m_filter->getFromTime();

    time_t difference = toTime - fromTime;

    // Set up the new toTime, which is the current time + the difference
    toTime += difference;

    const time_t currentTime = CTime::GetCurrentTime().GetTime();
    if (currentTime < toTime)
    {
        // The new toTime is in the "future" so set the filter back into "current" mode
        toTime = 0;
        fromTime = currentTime - difference;

        // In "current" mode, can't use next, so disable it, but can use "updating" so enable that.
        m_eventsToolbar.EnableButton(IDC_NEXT,false);
        m_eventList.setRefreshPeriod( m_refreshTime );
		enableRefresh=true;
    }
    else
    {
        fromTime += difference;
    }

    m_filter->setToTime(toTime);
    m_filter->setFromTime(fromTime);

    // Disable the previous button if there are no more events earlier than the from date
    if(m_filter->getFromTime() > m_filter->getEarliestTime())
    {
        m_eventsToolbar.EnableButton(IDC_PREVIOUS,true);
    }

    if (m_filterDlg != NULL)
    {
        // If there is a currently active filter dialog, notify it so it can update its to and from times
        m_filterDlg->updateFilter();
    }

    // This change forced the named filter to an unnamed state, so the combo box must be updated.
    populateNamedFilters();

	// refresh window caption
	setWindowCaption();


    // And now load the events matching the new filter
    loadEvents(enableRefresh);
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"OnNext come out");

}


void EventViewerDlg::OnPrevious()
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"OnPrevious come in");
    // The "previous" button has been clicked

    // Check if the "to" time is current.
    time_t toTime = m_filter->getToTime();
    if ( Filter::CURRENT_TIME == toTime )
    {
        // Previous can leagally be called when the "to" flag is set, so need to get the
        // current time
        toTime = CTime::GetCurrentTime().GetTime();

        // And disable updating.
        //m_eventList.setRefreshPeriod(0);
    }

    // Check if the "from" time is 24 hours before "to" time.
    time_t fromTime = m_filter->getFromTime();
    if ( Filter::PREVIOUS_DAY == fromTime )
    {
        // Previous can legally be called when the "from" flag is set, so need to get the
        // time 24 hours before "to" time.
        fromTime = toTime - TWENTY_FOUR_HOURS;
    }

    // Now determine the new "to" and "from" times.
    time_t difference = toTime - fromTime;
    m_filter->setToTime(toTime - difference);
    m_filter->setFromTime(fromTime - difference);

    // Disable the previous button if there are no more events earlier than the from date
    if(m_filter->getFromTime() <= m_filter->getEarliestTime())
    {
        m_eventsToolbar.EnableButton(IDC_PREVIOUS,false);
    }
    
    m_eventsToolbar.EnableButton(IDC_NEXT,true);

    if (m_filterDlg != NULL)
    {
        // If there is a currently active filter dialog, notify it so it can update its to and from times
        m_filterDlg->updateFilter();
    }

    // This change forced the named filter to an unnamed state, so the combo box must be updated.
    populateNamedFilters();
	
	// refresh window caption
	setWindowCaption();
	
	// clear list view
	m_eventList.SetItemCountEx( 0 );
	
    // And now load the events matching the new filter
    loadEvents(false);

    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,"OnPrevious come out");
}


void EventViewerDlg::OnPrint()
{
    FUNCTION_ENTRY("OnPrint");
    // Notify the event list printing was requested
    m_eventList.print();

    FUNCTION_EXIT;
}

void EventViewerDlg::OnExport()
{
    FUNCTION_ENTRY("OnExport");
	//li zhongyan change here
    //TA_Base_Core::SOEEventViewer* guiEntity = dynamic_cast<TA_Base_Core::SOEEventViewer*>(m_controlClass.getGUIEntity());
	unsigned long entitykey = m_controlClass.getGUIEntity()->getKey();
	TA_Base_Core::SOEEventViewer* guiEntity = new TA_Base_Core::SOEEventViewer(entitykey);

    CString exportLocation("");
    try
    {

        //exportLocation = guiEntity->getDefaultExportLocation().c_str();
		//TD17888  marc_bugfix
		exportLocation = guiEntity->getExportDirectory().c_str();

        // ++ AZ : TD 13826, no err message for invalid path
        exportLocation.TrimRight();
        if (exportLocation != "")
        {
            // check if the path is valid
/*
            SHFILEINFO sInfo;
            DWORD ret = SHGetFileInfo(exportLocation.GetBuffer(0), 0, &sInfo, sizeof(sInfo), SHGFI_TYPENAME);

            if (ret == 0)
            {
                CString errMessage;
                errMessage = "Invalid default path ";
                errMessage += exportLocation;
                MessageBox(errMessage, "Cannot use default directory");

                exportLocation.Empty();
            }
*/
			CString fileName = "Untitled";
			CFile outFile;

			if ( 0 == outFile.Open(exportLocation + fileName, CFile::modeCreate | CFile::modeReadWrite) )
			{

				TCHAR szMaxpath[MAX_PATH];

  				//TD15037++ - show error dialog just like alarm manager              
				//CString errMessage;
                //errMessage = "Invalid default path ";
                //errMessage += exportLocation;
                //MessageBox(errMessage, "Cannot use default directory");
				
				std::string msg = exportLocation + fileName + ".\n";
				msg += "Set and save to Local Directory";
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << std::string( msg );
				UINT selectedButton = userMsg.showMsgBox(IDS_UI_040034);
				//++TD15037

                exportLocation.Empty();
				if (SHGetSpecialFolderPath(NULL, szMaxpath, CSIDL_PERSONAL, FALSE))
				{
					 exportLocation = szMaxpath;

				}
			}
			else
			{
				// remove the file if it is created
				if (outFile != NULL) 
				{	
					outFile.Close();
					CFile::Remove(exportLocation + fileName);
				}
			}

        }
		else
		{
			//TD17888 marc_bugfix
			TCHAR szMaxpath[MAX_PATH];

		    if (SHGetSpecialFolderPath(NULL, szMaxpath, CSIDL_PERSONAL, FALSE))
			{
				exportLocation = szMaxpath;
				
			}
		}
        // AZ ++ TD 13826
    }
    catch(...)
    {
        exportLocation.Empty();
    }

    // TD11124: Disable current load and refreshing while exporting is taking place.
    m_eventList.setRefreshPeriod(0);
    m_filter->pauseCurrentListDataLoad();

    // Notify the event list exporting was requested
    m_eventList.export( exportLocation );

    // TD11124: Enable current load and refreshing now that exporting is complete.
    m_filter->resumeCurrentListDataLoad();
    m_eventList.setRefreshPeriod(m_refreshTime);

	delete guiEntity;

    FUNCTION_EXIT;
}

void EventViewerDlg::OnAlarmManager()
{
    FUNCTION_ENTRY("OnAlarmManager");

    try
    {
        using namespace TA_Base_Bus::TA_GenericGui;

        // Determine the screen that will have the Alarm Manager. 
        CPoint pt( GetMessagePos() );
        RECT position = TA_Base_Bus::AppLauncher::getInstance().getRect( SCREEN_CURRENT,
                                                                         AREA_SCHEMATIC,
                                                                         pt.x );
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Launching Event Viewer at xPos: %d yPos: %d and it should resize to fit on a single screen.", position.left, position.top);

        // Launch the alarm manager.
        TA_Base_Bus::AppLauncher::getInstance().launchApplication(ALARM_GUI_APPTYPE,
                                                                  "",
                                                                  POS_BOUNDED|POS_RESIZABLE,
                                                                  ALIGN_FIT|ALIGN_LEFT|ALIGN_TOP,
                                                                  NULL,
                                                                  &position);
    }
    catch(TA_Base_Core::ApplicationException&)
    {
        LOG_EXCEPTION_CATCH(SourceInfo, "ApplicationException", "Could not launch event viewer");

        // Construct the error message to display.
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << std::string("Alarm Manager");
        userMsg.showMsgBox(IDS_UE_010002);
    }

    FUNCTION_EXIT;
}

void EventViewerDlg::OnIncidentReport()
{
    FUNCTION_ENTRY("OnIncidentReport");

    // Generate an incident report on any selected alarm events.
    m_eventList.executeAction( ACTION_CREATE_INCIDENT_REPORT );

    FUNCTION_EXIT;
}

void EventViewerDlg::OnSelchangeComboNamedFilter()
{
    FUNCTION_ENTRY("OnSelchangeComboNamedFilter");

    // Get the currently selected item.
    int selIdx = m_namedFiltersComboBox.GetCurSel();
    if ( 0 <= selIdx )
    {
        // Get the name of the filter.
        CString filterName_c;
        m_namedFiltersComboBox.GetLBText( selIdx, filterName_c );
        std::string filterName( filterName_c );

        // Make sure the same filter wasn't selected.
        if ( filterName != m_filter->getNamedFilter()->getName() )
        {
            std::auto_ptr<NamedFilter> loadedFilter;

            try
            {
                // Load the filter.
                loadedFilter = NamedFilterAccessFactory::getInstance().getNamedFilter( filterName );
            }
            catch ( DataException& )
            {
                // Display the load error message.
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << ERROR_LOAD;
                userMsg << ERROR_LOADED;
                userMsg.showMsgBox(IDS_UE_040031);

                // Reselect the old named filter.
                populateNamedFilters();
                return;
            }

            // Make a copy of it.
            m_filter->setNamedFilter( loadedFilter );

			if (m_filter->getToTime()==0 && m_filter->getFromTime()==0)
			{
				loadEvents(true);
			}
			else
			{
				loadEvents(false);
			}
            // Load the events matching the filter.
            

            // Update the window title.
            setWindowCaption();

            // TD11343: Update any button states that depend on the current filter.
            m_eventsToolbar.EnableButton( IDC_NEXT, Filter::CURRENT_TIME!=m_filter->getToTime() );

            // Update the filter dialog if required.
            if ( NULL != m_filterDlg )
            {
                m_filterDlg->updateFilter();
            }
        }
    }    

    FUNCTION_EXIT;
}

/*zhiqiang--begin
//TD17143
LRESULT EventViewerDlg::OnSetupEventFiltersAndRights(WPARAM,LPARAM)
{
	//zhiqiang-- FUNCTION_ENTRY("OnRefreshRights");
	FUNCTION_ENTRY("EventViewerDlg::OnSetupEventFiltersAndRights"); //zhiqiang++
	
    // Populate it.
    populateNamedFilters();
 
    // Populate event levels
    populateEventLevels();

	applyRights();
	FUNCTION_EXIT;
	return 0;
}
//*/ //zhiqiang--end

//zhiqiang++begin
LRESULT EventViewerDlg::OnSetupEventFiltersAndRights(WPARAM, LPARAM)
{
	FUNCTION_ENTRY("EventViewerDlg::OnSetupEventFiltersAndRights");
 	
	populateNamedFilters();

	m_pPopulateEventLevelsThread = new CPopulateEventLevelsThread();
	m_pPopulateEventLevelsThread->start();

	m_pApplyRightsThread = new CApplyRightsThread(&m_controlClass);
	m_pApplyRightsThread->start();
    // Populate it.populateNamedFilters();
	
    // Populate event levels populateEventLevels();
	
	// applyRights();
	FUNCTION_EXIT;
	return 0;
}

void EventViewerDlg::enableDisableColumn(const std::string& columnName,const unsigned long menuId)
{
    FUNCTION_ENTRY("enableDisableColumn");
    // Get the header control (need this to figure out which column ID coresponds to the supplied column name).
    CHeaderCtrl* header = m_eventList.GetHeaderCtrl();

    // In order to get a column name from a header control, need to get header items, so set one up.
    HDITEM hdi;
    TCHAR  stringBuffer[256];   // To get the name in a header control, need a string buffer - which is what we have here

    hdi.mask = HDI_TEXT;        // Set the mask to HDI_TEXT, telling the header we want to retrieve the name
    hdi.pszText = stringBuffer; // Set a string pointer for the header control to write the text to (the string buffer)
    hdi.cchTextMax = 256;       // Tell the header control how long the buffer is (max size)

    // Loop for the number of items in the header
    int count = header->GetItemCount();
    for (int i=0;i < count;i++)
    {
       // Get the item, and put the name in a string
       header->GetItem(i, &hdi);
       std::string headerName(stringBuffer);
   
       if (0 == headerName.compare(columnName))
       {
           // If this is the column we're looking for...
           if (m_eventList.GetColumnWidth(i) == 0)
           {
               // ... and it is currently not visible, make it visible
               m_eventList.SetColumnWidth(i,150);

               // And check its menu item
               GetMenu()->CheckMenuItem(menuId,MF_CHECKED);
           }
           else
           {
               // ... and it is currently visible, hide it
               m_eventList.SetColumnWidth(i,0);

               // And uncheck its menu item
               GetMenu()->CheckMenuItem(menuId,MF_UNCHECKED);
           }
           return;
       }
    }

    FUNCTION_EXIT;
}

void EventViewerDlg::OnStatusBar() 
{ 
    FUNCTION_ENTRY("OnStatusBar");   
    // Toggle the visibility of the status bar
    if (m_statusBar.IsWindowVisible())
    {
        // If it's currently visible, then hide it
        m_statusBar.ShowWindow(SW_HIDE);
        // Uncheck the item in the menu
        GetMenu()->CheckMenuItem(IDC_STATUS_BAR,MF_UNCHECKED);
    }
    else
    {
        // If it's currently not visible, then show it
        m_statusBar.ShowWindow(SW_SHOW);
        // Check the item in the menu
        GetMenu()->CheckMenuItem(IDC_STATUS_BAR,MF_CHECKED);
    }

    // Reposition the status bar
	CRect rcClientNow;
    RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST,
				   0, reposQuery, rcClientNow);    

    // Send a "Size" message to repaint the main window (invalidate doesn't work here for some reason)
    CRect viewerRect;
    GetWindowRect(&viewerRect);
    ScreenToClient(&viewerRect);
    this->PostMessage(WM_SIZE,viewerRect.Height(),viewerRect.Width());

    FUNCTION_EXIT;
}

void EventViewerDlg::OnToolBar() 
{
    FUNCTION_ENTRY("OnToolBar");
    // Toggle the visibility of the tool bar
    if (m_eventsToolbar.IsWindowVisible())
    {
        // If it's currently visible, hide it (and the pager it's in)
        m_eventsToolbar.ShowWindow(SW_HIDE);
        CWnd* pager = GetDlgItem(IDC_PAGER_CTRL);
        pager->ShowWindow(SW_HIDE);
        m_pageUp.ShowWindow(SW_HIDE);
        m_pageDown.ShowWindow(SW_HIDE);
        // Check the item in the menu
        GetMenu()->CheckMenuItem(IDC_TOOL_BAR,MF_UNCHECKED);
    }
    else
    {
        // If it's currently not visible, show it (and the pager it's in)
        m_eventsToolbar.ShowWindow(SW_SHOW);
        CWnd* pager = GetDlgItem(IDC_PAGER_CTRL);
        pager->ShowWindow(SW_SHOW);
        m_pageUp.ShowWindow(SW_SHOW);
        m_pageDown.ShowWindow(SW_SHOW);
        // Check the item in the menu
        GetMenu()->CheckMenuItem(IDC_TOOL_BAR,MF_CHECKED);
    }  

    // Send a "Size" message to repaint the toolbar (invalidate doesn't work here for some reason)
    CRect viewerRect;
    GetWindowRect(&viewerRect);
    ScreenToClient(&viewerRect);
    this->SendMessage(WM_SIZE,viewerRect.Height(),viewerRect.Width());	

    // Invalidate the event list to cause it to redraw
    m_eventList.Invalidate();
    FUNCTION_EXIT;
}


void EventViewerDlg::OnShowAll()
{
    FUNCTION_ENTRY("OnShowAll");

    // Get the current state.
    MENUITEMINFO info;
    info.cbSize = sizeof (MENUITEMINFO); // must fill up this field
    info.fMask = MIIM_STATE;             // get the state of the menu item
    GetMenu()->GetMenuItemInfo( IDC_SHOW_ALL, &info );

    // Change to the new state.
    if ( info.fState & MF_CHECKED )
    {
        // Uncheck the menu item.
        GetMenu()->CheckMenuItem( IDC_SHOW_ALL, MF_UNCHECKED|MF_BYCOMMAND );
        m_filter->setDutyEnabled( true );
    }
    else
    {
        // Check the menu item.
        GetMenu()->CheckMenuItem( IDC_SHOW_ALL, MF_CHECKED|MF_BYCOMMAND );
        m_filter->setDutyEnabled( false );
    }

    // Reload the current filter.
    reloadEvents();

    FUNCTION_EXIT;
}


void EventViewerDlg::OnAppAbout() 
{
    FUNCTION_ENTRY("OnAppAbout");
    // Display the about box
	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    FUNCTION_EXIT;
}


void EventViewerDlg::OnHelp() 
{
    FUNCTION_ENTRY("OnHelp");
    // Display the about box
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp();	
    FUNCTION_EXIT;
}


int EventViewerDlg::findMenuItem(CMenu* Menu, LPCTSTR MenuString)
{
    FUNCTION_ENTRY("findMenuItem");
    // Ensure the menu is valid (and is actually a menu).
   ASSERT(Menu);
   ASSERT(::IsMenu(Menu->GetSafeHmenu()));

   // Loop for each item in the menu
   int count = Menu->GetMenuItemCount();
   for (int i = 0; i < count; i++)
   {
      CString str;
      // If the menu name matches the search string, return it
      if (Menu->GetMenuString(i, str, MF_BYPOSITION) &&
         (strcmp(str, MenuString) == 0))
         return i;
   }

   // If not found, return -1
   FUNCTION_EXIT;
   return -1;
}

void EventViewerDlg::setUpStatusBar()
{
    FUNCTION_ENTRY("setUpStatusBar");
    // Create the status bar
	if (m_statusBar.Create(this)) 
    {
        // If successful, set the indicators.
	    m_statusBar.SetIndicators(indicators,sizeof(indicators)/sizeof(UINT));


        // Set the default text display
        m_statusBar.SetWindowText("加载中...");
        
        // And set the STRETCH property on the left-most pane - that will make the left pane take up all the status bar
        // not used by any other panes.
		m_statusBar.SetPaneInfo(STRETCH_PANE, m_statusBar.GetItemID(STRETCH_PANE),
			SBPS_STRETCH, NULL );

        // Set the size of the event count pane
        int pane = m_statusBar.CommandToIndex(ID_INDICATOR_SIZE);
        m_statusBar.SetPaneInfo(pane, m_statusBar.GetItemID(pane),
		    SBPS_NORMAL, 130 );

    }   

    // Ensure the statusbar is displayed in the 
    //CFont font;
    //font.CreatePointFont(FONT_SIZE,FONT_NAME,NULL); // 10pt Arial Bold
    m_statusBar.SetFont(&m_font,FALSE);

    
	// Get the size of the application window
	CRect clientRectStart;
	CRect clientRectCurrent;
	GetClientRect(clientRectStart);

    // And reposition the status bar across the bottom
	RepositionBars(AFX_IDW_CONTROLBAR_FIRST, // These two specify the range in which the default windows 
                   AFX_IDW_CONTROLBAR_LAST,  // control bars reside, which includes the status bar
                   0, // The item ID to resize to fill the space not occupied by the status bar - set to 0, because resizing is manual
                   reposQuery, // Tell the method not to re-size the main window
                   clientRectCurrent); // A rectangle object which will be set to the size of the client area after the method has completed

	// Now move all the controls so they are in the same relative
	// position within the remaining client area as they would be
	// with no control bars.

    // So first get the offset by which everything needs to be moved
	CPoint offset(clientRectCurrent.left - clientRectStart.left,
					clientRectCurrent.top - clientRectStart.top);

    // Get the first child of the main applicaiton window
    CWnd* childWindow = GetWindow(GW_CHILD);
	CRect  childRect;
	while (NULL != childWindow)
	{
        // Get the current client rectangle for the window
		childWindow->GetWindowRect(childRect);
		ScreenToClient(childRect);

        // Adjust is by the offest
		childRect.OffsetRect(offset);

        // And move the window accordingly
		childWindow->MoveWindow(childRect, FALSE);

        // Then get the next child window
		childWindow = childWindow->GetNextWindow();
	}

	// Adjust the dialog window dimensions

    // Get the current main applicaiton window
	CRect windowRect;
	GetWindowRect(windowRect);

    // And resize it by the offset from adding the control bars
	windowRect.right += clientRectStart.Width() - clientRectCurrent.Width();
	windowRect.bottom += clientRectStart.Height() - clientRectCurrent.Height();
	MoveWindow(windowRect, FALSE);
    FUNCTION_EXIT;
}


void EventViewerDlg::setUpToolBar()
{
    FUNCTION_ENTRY("setUpToolBar");
    // Get the CWinApp object of this application (need it for the image lists)
    CWinApp*	pApp = AfxGetApp();
    
    // Set the tool bar child
    UINT		tbarStyle;	
	tbarStyle = WS_VISIBLE | WS_CHILD;          // Standard windows sytles
	tbarStyle = tbarStyle | CCS_NODIVIDER;      // Don't draw a divider line at the top of the toolbar (we have a different line for that)
	tbarStyle = tbarStyle | CCS_NOPARENTALIGN;  // Don't align the toolbar with the top of the window (i.e. manual placement)
	tbarStyle = tbarStyle | CCS_NORESIZE;       // Don't resize the toolbar automatically (we'll do it ourselves)
	tbarStyle = tbarStyle | TBSTYLE_TOOLTIPS;   // Enable tooltip notification when floating
	tbarStyle = tbarStyle | TBSTYLE_FLAT;       // Enable the "flat" style (buttons with text under them, and hot tracking enabled)	
    tbarStyle = tbarStyle | TBSTYLE_TRANSPARENT;


    // Create the standard ("enabled") image list, and the highlighted ("hot") image list
    m_eventsTbImageListEnabled.Create(16,16, ILC_MASK, 10, 2);
	m_eventsTbImageListHot.Create(16,16, ILC_MASK, 10, 2);

    // Get the bounds of the pager static
	CRect pagerRect;
	GetDlgItem(IDC_PAGER_CTRL)->GetWindowRect(pagerRect);
	ScreenToClient(pagerRect);
    pagerRect.right -= 50; // This is needed to correctly draw pager the first time.

    // And then delete it (we'll replace it with the actual pager next)
	GetDlgItem(IDC_PAGER_CTRL)->DestroyWindow();

	// Create a Pager Control in the bounds of the old pager static, and give it the same ID
	m_Pager.Create(WC_PAGESCROLLER, _T(""),WS_CHILD | WS_VISIBLE | WS_TABSTOP , pagerRect, this, IDC_PAGER_CTRL);

    // Create the toolbar, with the pager set as its parent window
	m_eventsToolbar.Create(tbarStyle, pagerRect, &m_Pager, IDC_EVENTS_TBAR );

    // Tell the pager that the toolbar is its child
	Pager_SetChild(m_Pager.m_hWnd, m_eventsToolbar.m_hWnd);
	m_Pager.ModifyStyle(0,PGS_HORZ); //  See horizontal scrolling
	Pager_SetButtonSize(m_Pager.m_hWnd, 20);     // And set the default button size

	m_eventsToolbar.SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);  // Notify the event toolbar that drop-down buttons can be used (if needed)
    m_eventsToolbar.SetButtonWidth(20, 200); // Set the min and max button widths

    // Ensure the toolbar is displayed in the correct font
    //CFont font;
    //font.CreatePointFont(FONT_SIZE,FONT_NAME,NULL); // 10pt Arial Bold
    m_eventsToolbar.SetFont(&m_font,FALSE);

    // The TBBUTTON struct is used to define the buttons to be added to the toolbar.
	TBBUTTON		tbButton;

	// Add Print button

    // Add a string to the toolbar's string list. AddStrings() returns the index at which it was added, and that is set in the
    // TBBUTTON struct iString member (tells the toolbar which index to use for this button)
	VERIFY((tbButton.iString = m_eventsToolbar.AddStrings("打印  ")) != -1);

    // Set the image for this button. As with the string, the image is actually added to an image list (and loaded from the 
    // IDI_REFRESH resource), and the TBBUTTON struct is set to the index in the image list to look up.
	tbButton.iBitmap = m_eventsTbImageListEnabled.Add(pApp->LoadIcon(IDI_PRINT));

    // Same for the "hot" image. By toolbar constraints, the two indexes need to be the same (that's why this doesn't
    // set another index value)
	m_eventsTbImageListHot.Add(pApp->LoadIcon(IDI_PRINT_HOT));

    // Set the button style - enabled, button (not divider, etc), and autosizing.
	tbButton.fsState = TBSTATE_ENABLED;
	tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE;
	//tbButton.fsStyle = TBSTYLE_BUTTON;
    // Set the data associated with the button - nothing
	tbButton.dwData = 0;

    // Set the command that will be sent to the app when the button is clicked
	tbButton.idCommand = IDC_PRINT;

    // and finally add the button to the toolbar (the 1 tells the toolbar how many buttons are being added in this command - 
    // if you want to be advanced you can define many toolbar buttons, and add them all to the list at once, but that isn't as
    // clear)
	VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));

	// Add Export button

	VERIFY((tbButton.iString = m_eventsToolbar.AddStrings("导出  ")) != -1);
	tbButton.iBitmap = m_eventsTbImageListEnabled.Add(pApp->LoadIcon(IDI_SAVE));
	m_eventsTbImageListHot.Add(pApp->LoadIcon(IDI_SAVE_HOT));
	tbButton.fsState = TBSTATE_ENABLED;
	tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; //smallkey change
	//tbButton.fsStyle = TBSTYLE_BUTTON;
	tbButton.dwData = 0;
	tbButton.idCommand = IDC_EXPORT;
	VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));    

    // Add a separator

    tbButton.iBitmap = NULL;
    tbButton.fsState = TBSTATE_ENABLED;
    tbButton.fsStyle = TBSTYLE_SEP; // This style sets the button as a separator
    tbButton.dwData = 0;
    tbButton.idCommand = 0;
    VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));


	// Add Filter button - a check button

	VERIFY((tbButton.iString = m_eventsToolbar.AddStrings("过滤器")) != -1);
	tbButton.iBitmap = m_eventsTbImageListEnabled.Add(pApp->LoadIcon(IDI_FILTERS));
	m_eventsTbImageListHot.Add(pApp->LoadIcon(IDI_FILTERS_HOT));
	tbButton.fsState = TBSTATE_ENABLED; // The CHECK style (below) means this is a toggle button
	tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECK | TBSTYLE_AUTOSIZE; //smallkey change
	//tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_CHECK;
	tbButton.dwData = 0;
	tbButton.idCommand = IDC_VIEW_FILTERS;
	VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));

	// Add Previous button

	VERIFY((tbButton.iString = m_eventsToolbar.AddStrings("向前")) != -1);
	tbButton.iBitmap = m_eventsTbImageListEnabled.Add(pApp->LoadIcon(IDI_PREVIOUS));
	m_eventsTbImageListHot.Add(pApp->LoadIcon(IDI_PREVIOUS_HOT));
	tbButton.fsState = TBSTATE_ENABLED;
	tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; //smallkey change
	//tbButton.fsStyle = TBSTYLE_BUTTON;
	tbButton.dwData = 0;
	tbButton.idCommand = IDC_PREVIOUS;
	VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));

	// Add Next button

	VERIFY((tbButton.iString = m_eventsToolbar.AddStrings("向后")) != -1);
	tbButton.iBitmap = m_eventsTbImageListEnabled.Add(pApp->LoadIcon(IDI_NEXT));
	m_eventsTbImageListHot.Add(pApp->LoadIcon(IDI_NEXT_HOT));
	tbButton.fsState = TBSTATE_ENABLED;
	tbButton.fsStyle = TBSTYLE_BUTTON | TBSTYLE_AUTOSIZE; //smallkey change
	//tbButton.fsStyle = TBSTYLE_BUTTON;
	tbButton.dwData = 0;
	tbButton.idCommand = IDC_NEXT;
	VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));
    


    ////////////////////////////
    // Named Filter Combo Box //
    ////////////////////////////
    {
        // Add Space

        tbButton.iBitmap = NULL;
        tbButton.fsState = TBSTATE_ENABLED;
        tbButton.fsStyle = TBSTYLE_SEP;
        tbButton.dwData = 0;
        tbButton.idCommand = 0;
        VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));

        // Add Combo box label placeholder. You can't actually add separate label to a toolbar, so we're just adding 
        // a wide "separator" button on top of which we can place the label (in a bit)

	    tbButton.iBitmap = NAMED_FILTER_LABEL_SEPARATOR_WIDTH;
        tbButton.fsState = TBSTATE_ENABLED;
        tbButton.fsStyle = TBSTYLE_SEP;
        tbButton.dwData = 0;
	    tbButton.idCommand = NAMED_FILTER_LABEL_SEPARATOR_ID;
	    VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));

        // Add Combo box placeholder. As with the label, can't directly add a combo box to a toolbar, so again, make a large
        // spearator button, and cover it with the combo box later.

	    tbButton.iBitmap = NAMED_FILTER_COMBO_SEPARATOR_WIDTH;
        tbButton.fsState = TBSTATE_ENABLED;
        tbButton.fsStyle = TBSTYLE_SEP;
        tbButton.dwData = 0;
	    tbButton.idCommand = NAMED_FILTER_COMBO_SEPARATOR_ID;
	    VERIFY(m_eventsToolbar.AddButtons(1, &tbButton));

        // Use two different rects when positioning the label and combo box.
        // rect will be the 'const' version, while rectCopy will contain a copy
        // of rect. This will save excessive calls to GetItemRect.
	    CRect rect, rectCopy;
	    int nIndex = m_eventsToolbar.CommandToIndex(NAMED_FILTER_LABEL_SEPARATOR_ID);
        m_eventsToolbar.GetItemRect( nIndex, &rect );

        // Determine the size and position for the static that hides label separator.
        rectCopy = rect;
        rectCopy.left += NAMED_FILTER_LABEL_OFFSET;
        CStatic* hideSeparator = new CStatic();
        hideSeparator->Create( "", WS_VISIBLE, rectCopy, &m_eventsToolbar );

        // Determine size and position for label.
        rectCopy = rect;
        rectCopy.left += NAMED_FILTER_LABEL_OFFSET; // move it right a bit to make way for the previous box
        rectCopy.top = NAMED_FILTER_LABEL_TOP;

        // Insert the label.
        CStatic* namedFilter = new CStatic();
        namedFilter->Create( NAMED_FILTER_LABEL_TEXT, WS_VISIBLE|SS_RIGHT, rectCopy, &m_eventsToolbar );
		namedFilter->SetFont(&m_font,FALSE);

        nIndex = m_eventsToolbar.CommandToIndex(NAMED_FILTER_COMBO_SEPARATOR_ID);
        m_eventsToolbar.GetItemRect(nIndex, &rect);

        // Determine size and position for the static that hides combo separator.
        rectCopy = rect;
        rectCopy.left += NAMED_FILTER_COMBO_OFFSET; // move it right a bit to make way for the previous label
        hideSeparator = new CStatic();
        hideSeparator->Create( "", WS_VISIBLE, rectCopy, &m_eventsToolbar );

        // Determine size and position for combo box.
        rectCopy = rect;
        rectCopy.left += NAMED_FILTER_COMBO_OFFSET;
        rectCopy.top = NAMED_FILTER_COMBO_TOP;
	    rectCopy.bottom = NAMED_FILTER_COMBO_DROP_HEIGHT;

        // Insert the combo box.
	    m_namedFiltersComboBox.Create(CBS_DROPDOWNLIST | WS_VISIBLE |
		    WS_TABSTOP | WS_VSCROLL | CBS_SORT, rectCopy, &m_eventsToolbar, ID_NAMED_FILTER_COMBO);
		m_namedFiltersComboBox.SetFont(&m_font);
	    
		//TD17143 - must be called onrefreshrights
        // Populate it.
        //populateNamedFilters();
    }


    // Set the image lists in the view
   	m_eventsToolbar.SetImageList(&m_eventsTbImageListEnabled);
	m_eventsToolbar.SetHotImageList(&m_eventsTbImageListHot);

    // Disable the buttons that are only enabled when one event is selected
    m_eventsToolbar.EnableButton(IDC_VIEW_INCIDENTS, false);
    
    // Disable the tool-bar buttons that are not yet implemented
    m_eventsToolbar.EnableButton(IDC_NEXT,false);

    // Prepare the page navigation buttons that arent actually part of the toolbar.
    HICON buttonIcon;
    buttonIcon = AfxGetApp()->LoadIcon(IDI_PAGE_UP);
    m_pageUp.SetIcon(buttonIcon);
    m_pageUp.EnableWindow(FALSE);
    buttonIcon = AfxGetApp()->LoadIcon(IDI_PAGE_DOWN);
    m_pageDown.SetIcon(buttonIcon);
    m_pageDown.EnableWindow(FALSE);

    // Re-draw the tool bar (just to make sure everything shows up fine).
    m_eventsToolbar.RedrawWindow();

    // Now make sure rights are applied.
	//TD17143++ - call is moved to the windows message handler
    //applyRights();
	//++TD17143	
    FUNCTION_EXIT;
}

void EventViewerDlg::setUpMenu()
{
    FUNCTION_ENTRY("setUpMenu");
    // Get the main menu
    CMenu* theMenu = GetMenu();

    // Determine if the close button and file->exit are to be disabled
	//li zhongyan change here
    //TA_Base_Core::SOEEventViewer* guiEntity = dynamic_cast<TA_Base_Core::SOEEventViewer*>(m_controlClass.getGUIEntity());
	unsigned long entitykey = m_controlClass.getGUIEntity()->getKey();
	TA_Base_Core::SOEEventViewer* guiEntity = new TA_Base_Core::SOEEventViewer(entitykey);

    bool disableClose(false);
    try
    {
        disableClose = guiEntity->isCloseDisabled();
    }
    catch(...)
    {
        disableClose = false;
    }   

    if(disableClose)
    {
        // Disable the close button
        HMENU hmenu = ::GetSystemMenu(AfxGetMainWnd()->m_hWnd, FALSE);
        ::EnableMenuItem(hmenu, SC_CLOSE, MF_GRAYED | MF_BYCOMMAND);

        // Now disable the exit button
        theMenu->EnableMenuItem(ID_FILE_EXIT,MF_GRAYED);
    }
	delete guiEntity;
    FUNCTION_EXIT;
}

void EventViewerDlg::loadEvents(bool enableRefresh)
{
    FUNCTION_ENTRY("loadEvents");
    // Need to load events from scratch based on the current filter.

    // This can take some time, so enable a wait cursor
    CWaitCursor waitCursor;

    // Set the load flag - this is a load and not a refresh
    m_updateType = UPDATE_LOAD;

    // Notify the event list to load new events
    m_eventList.loadEvents(enableRefresh);

    FUNCTION_EXIT;
}

void EventViewerDlg::reloadEvents()
{
	FUNCTION_ENTRY("loadEvents");
	// Need to load events from scratch based on the current filter.

	// This can take some time, so enable a wait cursor
	CWaitCursor waitCursor;

	// Set the load flag - this is a load and not a refresh
	m_updateType = UPDATE_LOAD;

	// Notify the event list to load new events
	m_eventList.reloadEvents();

	FUNCTION_EXIT;
}

void EventViewerDlg::setFilter(Filter* filter)
{
    FUNCTION_ENTRY("setFilter");
    m_filter = filter;
    FUNCTION_EXIT;
}

void EventViewerDlg::setWindowCaption()
{
    FUNCTION_ENTRY("setWindowCaption");
    // Set the title bar to show what the current filters are
    //std::string windowText(RunParams::getInstance().get(RPARAM_APPNAME));
	std::string windowText("SOE查看器");
    std::string filterText(m_filter->getCurrentFilterString());

    if (0 < filterText.length())
    {
        //windowText += " - Filtered on: " + filterText;
		windowText += " - 过滤: " + filterText;
        SetWindowText(CString(windowText.c_str()));
    }
    else
    {
        SetWindowText(CString(windowText.c_str()));
    }
    FUNCTION_EXIT;
}

void EventViewerDlg::OnPageUp() 
{	
    // Trigger the page change.
    m_eventList.previousPage();
}

void EventViewerDlg::OnPageDown() 
{
    // Trigger the page change.
    m_eventList.nextPage();
}

void EventViewerDlg::setFilterFromCommandLine()
{
    RunParams& runParams = RunParams::getInstance();

    // Get a pointer to the current named filter.
    NamedFilter* namedFilter = m_filter->getNamedFilter();

    // Create the vector used to store the keys.
    std::vector<unsigned long> keyList;

	// Check default
    std::string defaultParam( runParams.get( RPARAM_FILTER_DEFAULT.c_str() ) );
    if ( 0 < defaultParam.size() )
    {
		try
		{
			// Trigger the filter load.
			std::auto_ptr<NamedFilter> loadedFilter = NamedFilterAccessFactory::getInstance().getNamedFilter( CombinedEventFilter::NAMEDFILTER_DEFAULT );
			m_filter->setNamedFilter( loadedFilter );
		}
		catch ( DataException& ex )
		{
			// Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

			// Display the load error message.
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ERROR_LOAD;
			userMsg << ERROR_LOADED;
			userMsg.showMsgBox(IDS_UE_040031);
		}

		return;
	}

    // Check locations
    std::string locationParam( runParams.get( RPARAM_FILTER_LOCATION.c_str() ) );
    if ( 0 < locationParam.size() )
    {
        tokenizeKeyList( locationParam, keyList );
		namedFilter->setNumericFilter( SOEEventFilter::FILTER_LOCATION_KEY, keyList );
    }

    // Check asset name
    std::string assetParam( runParams.get( RPARAM_FILTER_ASSET.c_str() ) );
    if ( 0 < assetParam.size() )
    {
		namedFilter->addStringFilter( SOEEventFilter::FILTER_ASSET_NAME, assetParam );
    }

    std::string fromParam( runParams.get( RPARAM_FILTER_FROM.c_str() ) );
    if ( 0 < fromParam.size() )
    {
        // Covert value to time_t.
        std::stringstream fromStream( fromParam );
        time_t fromTime;
        if ( ! ( fromStream >> fromTime ).fail() )
        {
            // 0 is a special value.
            if ( 0 == fromTime )
            {
                m_filter->setFromTime( m_filter->getEarliestTime() );
            }
            else
            {
                m_filter->setFromTime( fromTime );
            }
        }
    }

    std::string toParam( runParams.get( RPARAM_FILTER_TO.c_str() ) );
    if ( 0 < toParam.size() )
    {
        // Covert value to time_t.
        std::stringstream toStream( toParam );
        time_t toTime;
        if ( ! ( toStream >> toTime ).fail() )
        {
            m_filter->setToTime( toTime );
        }
    }

    // Check severities
//     std::string severityParam( runParams.get( RPARAM_FILTER_SEVERITY.c_str() ) );
//     if ( 0 < severityParam.size() )
//     {
//         tokenizeKeyList( severityParam, keyList );
//         namedFilter->setNumericFilter( CombinedEventFilter::FILTER_SEVERITY, keyList );
//     }

  
}

void EventViewerDlg::tokenizeKeyList( const std::string& param, std::vector<unsigned long>& keys )
{
    typedef boost::tokenizer< boost::char_separator<char> > tokenizer;

    // Variables required for string->integer conversion.
    std::stringstream tempStream;
    unsigned long key;

    // Prepare the tokenizer and iterator.
    boost::char_separator<char> lineSep( "," );
    boost::tokenizer< boost::char_separator<char> > keyTokens( param, lineSep );
    boost::tokenizer< boost::char_separator<char> >::iterator it = keyTokens.begin();

    // Make sure the vector is empty.
    keys.clear();

    // For each key...
    for ( ; it != keyTokens.end(); it++ )
    {
        // Prepare the string stream for conversion.

		// td11982
		tempStream.clear();
        tempStream.str( *it );

        // Convert the key to an integer and add it to the filter if conversion succeeded.
        if ( ! ( tempStream >> key ).fail() )
        {
            keys.push_back( key );
        }
    }

}

void EventViewerDlg::onRunParamChange(const std::string& name, const std::string& value)
{
    LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Received new RunParam (%s = %s)", name.c_str(), value.c_str());//TD14337 yezh++

    // We need to reload the list when either:
    // 1) The current database has gone down
    // 2) The primary database has come back up and we are currently using a secondary

    std::ostringstream str;
    str << RPARAM_DBPREFIX << m_currentDbString;
    CString currentStr(str.str().c_str());
    if ( currentStr.CompareNoCase(name.c_str()) == 0 )
    {
        // Want to reload the list when the database being used has gone offline
        if ( value == RPARAM_DBOFFLINE )
        {
            reloadEvents();

            m_currentDbString = getCurrentDatabaseString();
        }
    }

    if( value == RPARAM_DBONLINE )
    {
        // Get the current connection
        std::string currentConnection = getCurrentDatabaseString();
        
        if(m_currentDbString != currentConnection)
        {
            // There is now a new database connection. Reload the list.
            reloadEvents();

            m_currentDbString = currentConnection;
        }

    }

    if ( RPARAM_SESSIONID == name )
    {
        // If the session changed reapply any right dependant settings
        // and return.
        applyRights();
		//m_filter->sessionChanged();
        // liuyu++ TD12737
        reloadEvents();
        // ++liuyu TD12737
        return;
    }

	if ( RPARAM_FILTER_DEFAULT == name ) // set to default filter and return
	{
		try
		{
			// Trigger the filter load.
			std::auto_ptr<NamedFilter> loadedFilter = NamedFilterAccessFactory::getInstance().getNamedFilter( CombinedEventFilter::NAMEDFILTER_DEFAULT );
			m_filter->setNamedFilter( loadedFilter );
		}
		catch ( DataException& ex )
		{
			// Log the exception.
			LOG_EXCEPTION_CATCH( SourceInfo, "DataException", ex.getWhichData() );

			// Display the load error message.
			TA_Base_Bus::TransActiveMessage userMsg;
			userMsg << ERROR_LOAD;
			userMsg << ERROR_LOADED;
			userMsg.showMsgBox(IDS_UE_040031);
			return;
		}
	}
	else
	{
		// Get a pointer to the current named filter.
		NamedFilter* namedFilter = m_filter->getNamedFilter();

		// Create the vector used to store the keys.
		std::vector<unsigned long> keyList;

		// Change filter depending on what changed.
// 		if ( RPARAM_FILTER_SUBSYSTEM == name )
// 		{
// 			DatabaseAccess dbAccess;
// 			std::vector<unsigned long> appSubsystems;
// 			std::vector<unsigned long> physSubsystems;
// 
// 			// Set the subsystem filters to the specified values.
// 			tokenizeKeyList( value, keyList );
// 			dbAccess.separateSubsystems( keyList, appSubsystems, physSubsystems );
// 			namedFilter->setNumericFilter( CombinedEventFilter::FILTER_APPLICATION_SUBSYSTEM, appSubsystems );
// 			namedFilter->setNumericFilter( CombinedEventFilter::FILTER_PHYSICAL_SUBSYSTEM, physSubsystems );
// 		}
// 		else if ( RPARAM_FILTER_LOCATION == name )
// 		{
// 			// Set the location filter to the specified values.
// 			tokenizeKeyList( value, keyList );
// 			 namedFilter->setNumericFilter( CombinedEventFilter::FILTER_LOCATION, keyList );
// 		}
// 		else if ( RPARAM_FILTER_OPERATOR == name )
// 		{
// 			// Set the operator filter to the specified values.
// 			tokenizeKeyList( value, keyList );
// 			 namedFilter->setNumericFilter( CombinedEventFilter::FILTER_OPERATOR, keyList );
// 		}
// 		else if ( RPARAM_FILTER_EVENTTYPE == name )
// 		{
// 			// Set the event type filter to the specified values.
// 			tokenizeKeyList( value, keyList );
// 			 namedFilter->setNumericFilter( CombinedEventFilter::FILTER_EVENTTYPE, keyList );
// 		}
		if ( RPARAM_FILTER_ASSET == name )
		{
			// Clear current asset filter and add new.
			namedFilter->removeStringFilter( CombinedEventFilter::FILTER_ASSET );
			namedFilter->addStringFilter( CombinedEventFilter::FILTER_ASSET, value );
		}
		else if ( RPARAM_FILTER_DESCRIPTION == name )
		{
			// Clear current description filter and add new.
			namedFilter->removeStringFilter( CombinedEventFilter::FILTER_DESCRIPTION );
			namedFilter->addStringFilter( CombinedEventFilter::FILTER_DESCRIPTION, value );
		}
		else if ( RPARAM_FILTER_FROM == name )
		{
			// Covert value to time_t.
			std::stringstream fromStream( value );
			time_t fromTime;
			if ( ! ( fromStream >> fromTime ).fail() )
			{
				// 0 is a special value.
				if ( 0 == fromTime )
				{
					m_filter->setFromTime( m_filter->getEarliestTime() );
				}
				else
				{
					m_filter->setFromTime( fromTime );
				}
			}
		}
		else if ( RPARAM_FILTER_TO == name )
		{
			// Covert value to time_t.
			std::stringstream toStream( value );
			time_t toTime;
			if ( ! ( toStream >> toTime ).fail() )
			{
				m_filter->setToTime( toTime );
			}
		}
// 		else if ( RPARAM_FILTER_SEVERITY == name )
// 		{
// 			// Set the severity filter to the specified values.
// 			tokenizeKeyList( value, keyList );
// 			namedFilter->setNumericFilter( CombinedEventFilter::FILTER_SEVERITY, keyList );
// 		}
		 
		 
		
	}

    // Update the filter dialog if it exists.
    if ( NULL != m_filterDlg )
    {
        m_filterDlg->updateFilter();
    }
	
	// set the window title to the changed filter
	setWindowCaption();

    // Trigger a database load.
    reloadEvents();
}


LRESULT EventViewerDlg::OnApplyRights(WPARAM wParam, LPARAM lParam)
{
	FUNCTION_ENTRY("OnApplyRights"); 
    // Flags to set visibility - default to visible.
    bool eventLevelVisible = true;
	bool showAllEventsEnabled = false;
	SrtActionPermittedInfo *pSrtActionPermittedInfo = reinterpret_cast<SrtActionPermittedInfo *>(wParam);
	
	try
	{
        std::string reason;
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
		
		eventLevelVisible = m_rightsLibrary->isActionPermittedOnResourceAndLocSub( pSrtActionPermittedInfo->sessionId, pSrtActionPermittedInfo->resourceId,
			pSrtActionPermittedInfo->locationKey, pSrtActionPermittedInfo->subsystemKey, TA_Base_Bus::aca_CHANGE_EVENT_LEVEL, reason, decisionModule );
		
		showAllEventsEnabled = m_rightsLibrary->isActionPermittedOnResourceAndLocSub( pSrtActionPermittedInfo->sessionId, pSrtActionPermittedInfo->resourceId,
            pSrtActionPermittedInfo->locationKey, pSrtActionPermittedInfo->subsystemKey,  TA_Base_Bus::aca_SHOW_ALL_EVENTS, reason, decisionModule );
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Errors occurred while trying to get rights.");
	}
	
	
	// Get the current state.
	MENUITEMINFO info;
	info.cbSize = sizeof (MENUITEMINFO); // must fill up this field
	info.fMask = MIIM_STATE;             // get the state of the menu item
	GetMenu()->GetMenuItemInfo( IDC_SHOW_ALL, &info );
	// Enable the menu command for show all events
	if ( showAllEventsEnabled )
	{
		if ( info.fState == MFS_GRAYED )
		{
			// enable the menu item.
			GetMenu()->EnableMenuItem( IDC_SHOW_ALL, MF_ENABLED|MF_BYCOMMAND );
		}
	}
	else
	{
		if ( info.fState == MFS_ENABLED )
		{
			// gray the menu item.
			GetMenu()->EnableMenuItem( IDC_SHOW_ALL, MF_GRAYED|MF_BYCOMMAND );
		}
	}
	
	//release memory space
	delete pSrtActionPermittedInfo; 

	return 0;
	
	FUNCTION_EXIT;
}
//zhiqiang++end
void EventViewerDlg::populateNamedFilters()
{
	FUNCTION_ENTRY("populateNamedFilters");
    // Make sure the combo box is empty.
    m_namedFiltersComboBox.ResetContent();

    try
    {
        // Get a list of the filters.
        NamedFilterAccessFactory::FilterNameList filters = NamedFilterAccessFactory::getInstance().getAllFilterNames();

        // Now add each to the combo box.
        NamedFilterAccessFactory::FilterNameList::iterator namedFilterIt;
        for ( namedFilterIt=filters.begin(); namedFilterIt!=filters.end(); namedFilterIt++ )
        {
            int index = m_namedFiltersComboBox.AddString( namedFilterIt->first.c_str() );
            if ( 0 <= index )
            {
                // If the data was inserted correctly store whether it is read only.
                m_namedFiltersComboBox.SetItemData( index, static_cast<int>(namedFilterIt->second) );

                // If it is the current filter then select it.
                if ( namedFilterIt->first == m_filter->getNamedFilter()->getName() )
                {
                    m_namedFiltersComboBox.SetCurSel( index );
                }
            }
        }
    }
    catch ( DataException& )
    {
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Could not get a list of named filters!");
    }

    // TD11626: If nothing is currently selected than it is a temporary name.
    if ( CB_ERR == m_namedFiltersComboBox.GetCurSel() )
    {
		m_namedFiltersComboBox.AddTempString( m_filter->getNamedFilter()->getName().c_str() );
    }
	
	//make filter combBox refresh 
	m_namedFiltersComboBox.Invalidate();
	FUNCTION_EXIT;//zhiqiang++
}


void EventViewerDlg::applyRights()
{
	FUNCTION_ENTRY("applyRights"); //zhiqiang++
    // Flags to set visibility - default to visible.
    bool eventLevelVisible = true;
	bool showAllEventsEnabled = false;

	try
	{
        std::string reason;
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

		// Determine the resource (subsystem) key.
        std::auto_ptr<TA_Base_Core::IResource> resource(
            TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(m_controlClass.getGUIEntity()->getKey()) );

		//TD17143++ - change action permission request to something that works faster
		TA_Base_Core::IEntityData* entityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( resource->getKey()); 
		unsigned long locationKey = entityData->getLocation();
		unsigned long subsystemKey = entityData->getSubsystem();
		delete entityData;
		std::string sessionId = m_controlClass.getSessionId();
		unsigned long resourceId = resource->getKey();
		
        // Now determine the rights.
        // eventLevelVisible = m_rightsLibrary->isActionPermittedOnResource( m_controlClass.getSessionId(), resource->getKey(),
        //    TA_Base_Bus::aca_CHANGE_EVENT_LEVEL, reason, decisionModule );
		eventLevelVisible = m_rightsLibrary->isActionPermittedOnResourceAndLocSub( sessionId, resourceId,
            locationKey, subsystemKey, TA_Base_Bus::aca_CHANGE_EVENT_LEVEL, reason, decisionModule );

		// showAllEventsEnabled = m_rightsLibrary->isActionPermittedOnResource( m_controlClass.getSessionId(), resource->getKey(),
        //    TA_Base_Bus::aca_SHOW_ALL_EVENTS, reason, decisionModule );
		showAllEventsEnabled = m_rightsLibrary->isActionPermittedOnResourceAndLocSub( sessionId, resourceId,
            locationKey, subsystemKey,  TA_Base_Bus::aca_SHOW_ALL_EVENTS, reason, decisionModule );
		//++TD17143
	}
	catch( ... )
	{
		LOG_EXCEPTION_CATCH( SourceInfo, "DataException", "Errors occurred while trying to get rights.");
	}

	// Get the current state.
	MENUITEMINFO info;
	info.cbSize = sizeof (MENUITEMINFO); // must fill up this field
	info.fMask = MIIM_STATE;             // get the state of the menu item
	GetMenu()->GetMenuItemInfo( IDC_SHOW_ALL, &info );
	// Enable the menu command for show all events
	if ( showAllEventsEnabled )
	{
		if ( info.fState == MFS_GRAYED )
		{
			// enable the menu item.
			GetMenu()->EnableMenuItem( IDC_SHOW_ALL, MF_ENABLED|MF_BYCOMMAND );
		}
	}
	else
	{
		if ( info.fState == MFS_ENABLED )
		{
			// gray the menu item.
			GetMenu()->EnableMenuItem( IDC_SHOW_ALL, MF_GRAYED|MF_BYCOMMAND );
		}
	}
	FUNCTION_EXIT;//zhiqiang++
}

std::string EventViewerDlg::getCurrentDatabaseString()
{
    std::string connectStr;
    TA_Base_Core::DbConnection::getInstance().getConnectionString(TA_Base_Core::Event_Ad, TA_Base_Core::Read, connectStr);
    std::vector<std::string> components;
    SimpleDb::ParseConnectStr(connectStr, components);
    return components[0];
}
