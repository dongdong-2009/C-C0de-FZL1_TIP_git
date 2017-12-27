#if !defined(AFX_HISTORYVIEWERDLG_CPP__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_)
#define AFX_HISTORYVIEWERDLG_CPP__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_

/**
* HistoryViewerDlg.cpp
*
* The source code in this file is the property of 
* Ripple Systems and is not for redistribution
* in any form.
*
* Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/HistoryViewerDlg.cpp $
* @author:  A. Lee
* @version: $Revision: #1 $
* Last modification: $DateTime: 2012/06/12 13:35:44 $
* Last modified by:  $Author: builder $
* 
* The main dialog class.
*/

#pragma warning(disable: 4355)  // 'this' : used in base member initializer list

#include <afxpriv.h>   //For AfxLoadString
#include <iomanip>

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/HistoryViewer.h"
//TD17529++
#include "app/trending/trend_viewer/src/HistoryViewerGUI.h"
#include "app/trending/trend_viewer/src/TrendsDialog.h" 
#include "app/trending/trend_viewer/src/TrendPropertiesDialog.h" 
//++TD17529
#include "app/trending/trend_viewer/src/HistoryViewerDlg.h"
#include "app/trending/trend_viewer/src/IGraph.h"
#include "app/trending/trend_viewer/src/PrintTitleDlg.h"
#include "app/trending/trend_viewer/src/GraphTypeFactory.h"
#include "app/trending/trend_viewer/src/TrendDataFactory.h"
#include "app/trending/trend_viewer/src/Globals.h"
#include "app/trending/trend_viewer/src/ColourDefs.h"
#include "app/trending/trend_viewer/src/Utilities.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/FunctionEntry.h"

#include "core/exceptions/src/TrendViewerException.h"
#include "core/data_access_interface/entity_access/src/HistoryViewerData.h" //TD17529
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/trends/src/ItemInfoDetails.h"
#include "core/data_access_interface/trends/src/globals.h"

#include "core/threads/src/Thread.h"

#include "bus/generic_gui/src/GenericGUI.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/rights_library/src/RightsLibrary.h"

#include "cots/teechart/TeeChartDefines.h"

// TeeChart includes
#include <panel.h>
#include <printer.h>
#include <titles.h>
#include <strings.h>
#include <export.h>
#include <toollist.h>
#include <cursortool.h>
#include <axisarrowtool.h>
#include <tools.h>
#include <pen.h>
#include <axes.h>
#include <axis.h>
#include <axislabels.h>
#include <axistitle.h>
#include <zoom.h>

#ifdef USE_OCCI
#include <occi.h>
#endif


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


int style = 0;


const CString LOAD_DATA = "Loading data...";
const int FONT_SIZE = 100;
const CString FONT = "Arial Bold";

const CString FILE_MENU_NAME = "&File";
const CString VIEW_MENU_NAME = "&View";


static UINT indicators[] =
{
    ID_SEPARATOR,           // status line indicator
		ID_LIVE_MODE
};


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::ItemInfo;
using TA_Base_Core::HistoryViewerData; //TD17529

namespace TA_Base_App
{
	
    TA_Base_Core::ReEntrantThreadLockable CHistoryViewerDlg::m_templateLock;    
	
	
    //
    // CHistoryViewerDlg
    //
    CHistoryViewerDlg::CHistoryViewerDlg(TA_Base_Bus::IGUIAccess* controlClass)
		: TransActiveDialog(*controlClass, CHistoryViewerDlg::IDD, NULL),
		m_nIDTracking( 0 ),            // For status messages
		m_nIDLastMessage( 0 ),
		m_nowCheck(false),
		m_itemStore(TrendDataFactory::getInstance().createCompositeStore(this)),
		//m_graph(NULL),
		m_archiveDataStore(TrendDataFactory::getInstance().createArchiveDataStore()),
		m_loadingComplete(false),
		m_serverDown(false),
		m_displayCursor(false),
		m_leftChartMargin(0),
		m_zoomed(false),
		m_useCmdLineParams(false),
		m_cursorToolIndex(0),
		m_axisScrolled(0),
		m_mouseDownX(0),
		m_mouseDownY(0),
		m_mouseUpX(0),
		m_mouseUpY(0),
		m_arrowToolIndex(0),
		m_singleItemDisplayed(false),
		m_resetGraph(false),
		m_lastDisplayTime(0),
		m_preloadPropertyItems(false),
		m_trendsDlg(NULL), // TD12625 azenitha,
		m_controlClass(controlClass)
    {
        FUNCTION_ENTRY("CHistoryViewerDlg");
		
        //{{AFX_DATA_INIT(CHistoryViewerDlg)
		//}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        
        m_sessionId = RunParams::getInstance().get(RPARAM_SESSIONID);
        RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);        
		
        setTransActiveResizingProperties();
		
		
		
        FUNCTION_EXIT;
    }
	
	void CHistoryViewerDlg::init()
	{
		//TD17529++ initialize the maxTrends to be displayed from the gui entity configuration
		TA_Base_Core::HistoryViewerData* guiEntity = dynamic_cast<TA_Base_Core::HistoryViewerData*>(m_controlClass->getGUIEntity());
		if ((NULL != guiEntity) && (guiEntity->getType() == HistoryViewerData::getStaticType()))
		{
			m_maxTrendsDisplayed = guiEntity->getMaxTrendsDisplayed();
			//TD17718++ - use the configured max points in DB 
			Utilities::setMaxPointsForBarGraph(guiEntity->getMaxPointsForBarGraph());
			Utilities::setMaxPointsForOtherGraphs(guiEntity->getMaxPointsForOtherGraphs());
			//++TD17718			
			Utilities::setGraphRefreshInterval(guiEntity->getGraphRefreshInterval()); //TD17529
		}
		else
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Entity configuration is not of type Trend Viewer.");						
		}
		//++TD17529
		
        //
        // Set a default graph. This will be a LINE graph - can be changed later.
        //
		
        m_currentTrendProperties.name = "";
        m_currentTrendProperties.description = "";
        m_currentTrendProperties.graphType = LINE;
        m_currentTrendProperties.samplePeriod = DEFAULT_SAMPLE_PERIOD; //TD17529
		//default time range to be displayed is 1 HOUR
        m_currentTrendProperties.startDateTime = CTime::GetCurrentTime().GetTime() - 3600;
        m_currentTrendProperties.endDateTime = m_currentTrendProperties.startDateTime + 3600;
        m_currentTrendProperties.live = false;
        m_currentTrendProperties.extraInfo = "";
        m_currentTrendProperties.items.clear();
        m_currentTrendProperties.modificationsUnsaved = true;
        m_currentTrendProperties.isNew = true;
		
        m_graph = GraphTypeFactory::getInstance().createLineGraph(
            m_chartCtrl, 
            m_currentTrendProperties.name,
            m_currentTrendProperties.description,
            m_currentTrendProperties.startDateTime,  // Set start time to an hour prior to current time
            m_currentTrendProperties.endDateTime,
            m_currentTrendProperties.live,
            m_currentTrendProperties.samplePeriod,
            m_itemStore.get(),
            m_archiveDataStore.get(),
            this);     
        
        TA_Base_Bus::RightsLibraryFactory rightsLibraryFactory;
		
		m_rightsLibrary = rightsLibraryFactory.buildRightsLibrary();
		
		
        // TD 11203: Launch the Trends dialog over the top of the main Trend Viewer window
        PostMessage(WM_LAUNCH_TRENDS_DLG, 0, 0);
        
        // TD 3094: The sql query that loads the datapoints takes about 25 seconds to execute. 
        // Post a message here to load the items so that OnInitDlg will return straight away and 
        // the Trend Viewer can be displayed quickly. Meanwhile, the items will be loading in the
        // background. Still won't actually be able to configure trends while the items are loading
        // but at the least the Trend Viewer will be displayed.
        PostMessage(WM_LOAD_ITEMS, 0, 0); 
        
        
        // TD 2924: Send a "Size" message to resize the manager. Forcing a resize will 
        // force the toolbar to display.
        CRect viewerRect;
        GetWindowRect(&viewerRect);
        ScreenToClient(&viewerRect);
        PostMessage(WM_SIZE,viewerRect.Height()-1, viewerRect.Width()-1 );
		
	}
	
	//TD17529++ - removed the destructor's definition from being inline
	CHistoryViewerDlg::~CHistoryViewerDlg()
	{
		try
		{
			if (m_rightsLibrary != NULL)
			{
				delete m_rightsLibrary;
				m_rightsLibrary = NULL;
			}
			
			// TD12625 azenitha++
			if (m_trendsDlg != NULL)
			{
				m_trendsDlg = NULL;
			}
			// TD12625 ++azenitha			
			
		}
		catch ( ... )
		{
		}
	}           
	
    //
    // DoDataExchange
    //
    void CHistoryViewerDlg::DoDataExchange(CDataExchange* pDX)
    {
        TransActiveDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(CHistoryViewerDlg)
		DDX_Control(pDX, IDC_DISPLAY_AREA, m_displayArea);
		DDX_Control(pDX, IDC_END_STATIC, m_endTimeCtrl);
		DDX_Control(pDX, IDC_START_STATIC, m_startTimeCtrl);
        DDX_Control(pDX, IDC_TCHART1, m_chartCtrl);
		//}}AFX_DATA_MAP
    }
	
    BEGIN_MESSAGE_MAP(CHistoryViewerDlg, TransActiveDialog)
        //{{AFX_MSG_MAP(CHistoryViewerDlg)
        ON_WM_PAINT()
        ON_WM_QUERYDRAGICON()
        ON_WM_SIZE()
        ON_WM_CLOSE()
        ON_WM_CREATE()
        ON_COMMAND(ID_NEW, OnNew)
        ON_COMMAND(ID_REFRESH, OnRefresh)
        ON_COMMAND(ID_NOW, OnNowCheck)
        ON_COMMAND(ID_TRENDS, OnTrends)
        ON_COMMAND(ID_PROPERTIES, OnProperties)
        ON_COMMAND(ID_CURSOR, OnCursor)
        ON_COMMAND(ID_VIEW_NOW, OnToggleNowCheck)
        ON_COMMAND(ID_PRINT_GRAPH, OnPrintGraph)
        ON_COMMAND(ID_EXPORT, OnExport)
        ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
        ON_COMMAND(ID_HELP, OnHelp)
        ON_MESSAGE(WM_SETMESSAGESTRING, OnSetMessageString)
        ON_MESSAGE(WM_POPMESSAGESTRING, OnPopMessageString)
        ON_WM_ENTERIDLE()
        ON_WM_INITMENUPOPUP()
        ON_WM_MENUSELECT()
        ON_WM_DESTROY()
        ON_WM_SYSCOMMAND()
        //}}AFX_MSG_MAP
        // Tool tip text
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
        // Standard View menu options
        ON_COMMAND(ID_VIEW_STATUS_BAR, OnStatusBarCheck)
        ON_COMMAND(ID_VIEW_TOOLBAR, OnToolBarCheck)
        ON_UPDATE_COMMAND_UI(ID_VIEW_STATUS_BAR, OnUpdateStatusBarMenu)
        ON_UPDATE_COMMAND_UI(ID_VIEW_TOOLBAR, OnUpdateToolBarMenu)
        ON_MESSAGE(WM_PROCESS_ERROR_MSG, OnProcessErrorMessage)
        ON_MESSAGE(WM_LOAD_ITEMS, OnLoadItems)
        ON_MESSAGE(WM_LOADING_COMPLETE, OnLoadingComplete)
        ON_MESSAGE(WM_LAUNCH_TRENDS_DLG, OnLaunchTrendsDialog)
		END_MESSAGE_MAP()
		
		
		BEGIN_EVENTSINK_MAP(CHistoryViewerDlg, TransActiveDialog)
        //{{AFX_EVENTSINK_MAP(CHistoryViewerDlg)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 27 /* OnZoom */, OnZoomGraph, VTS_NONE)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 26 /* OnUndoZoom */, OnUndoZoomGraph, VTS_NONE)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 24 /* OnScroll */, OnScrollGraph, VTS_NONE)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 2 /* OnAllowScroll */, OnAllowScroll, VTS_I4 VTS_PR8 VTS_PR8 VTS_PBOOL)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 40 /* OnCursorToolChange */, OnCursorToolChange, VTS_I4 VTS_I4 VTS_I4 VTS_R8 VTS_R8 VTS_I4 VTS_I4)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 7 /* OnClickSeries */, OnClickSeries, VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4 VTS_I4)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 5 /* OnClickBackground */, OnClickBackground, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 6 /* OnClickLegend */, OnClickLegend, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 19 /* OnMouseDown */, OnMouseDown, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
        ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 21 /* OnMouseUp */, OnMouseUp, VTS_I4 VTS_I4 VTS_I4 VTS_I4)
		ON_EVENT(CHistoryViewerDlg, IDC_TCHART1, 14 /* OnGetAxisLabel */, OnGetAxisLabel, VTS_I4 VTS_I4 VTS_I4 VTS_PBSTR)
		//}}AFX_EVENTSINK_MAP
		END_EVENTSINK_MAP()
		
		
		
		//
		// Need to implement this to avoid application shutting down whenever
		// the user hits enter.
		//
		void CHistoryViewerDlg::OnOK()
    {
        //
        // Make the enter key apply
        //
        //OnApply();
    }
	
	
    //
    // OnInitDialog
    //
    BOOL CHistoryViewerDlg::OnInitDialog()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnInitDialog");
		
        TransActiveDialog::OnInitDialog();
        short    shBtnColor = 30;
		
        //
        // Set the icon for this dialog.  The framework does this automatically
        // when the application's main window is not a dialog
        //
        SetIcon(m_hIcon, TRUE);            // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon
		
        if(teeChart6IsInstalled() == false)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << COTS_COMPONENT_NAME;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120025);
			
            std::string errorMsg(COTS_COMPONENT_NAME);
            errorMsg += " is not installed";
            TA_THROW(TA_Base_Core::TrendViewerException(TA_Base_Core::TrendViewerException::TEECHART_NOT_INSTALLED, 
                errorMsg.c_str()));
        }
		
        m_leftChartMargin = m_chartCtrl.GetPanel().GetMarginLeft();
        
        // Increase the right margin slightly so labels will fit nicely
        m_chartCtrl.GetPanel().SetMarginRight(m_chartCtrl.GetPanel().GetMarginRight() + 2);
		
        initialiseMenuItems();
        mapDisplayAreaResources();
        setUpDisplayArea();
        parseParameters();
        setUpGraph();
        setAccessControlledButtons();
		
		
		
        LOG(SourceInfo, DebugUtil::FunctionExit, "OnInitDialog");
		
        return TRUE;
    }
	
	
    //
    // onRunParamChange
    //
    void CHistoryViewerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        FUNCTION_ENTRY("onRunParamChange");
		
        if(name == RPARAM_SESSIONID)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Session ID changed");
            m_sessionId = value;
            setAccessControlledButtons();
        }
		
        if(name == RPARAM_ENTITYNAME)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Entity name changed");
            setAccessControlledButtons();
        }
		
        if(name == "Items")
        {
            // Reset everything
            m_currentTrendProperties.name = "";
            m_currentTrendProperties.description = "";
            m_currentTrendProperties.graphType = LINE;
            m_currentTrendProperties.samplePeriod = DEFAULT_SAMPLE_PERIOD; //TD17529
            m_currentTrendProperties.startDateTime = CTime::GetCurrentTime().GetTime() - 3600;
            m_currentTrendProperties.endDateTime = m_currentTrendProperties.startDateTime + 3600;
            m_currentTrendProperties.live = false;
            m_currentTrendProperties.extraInfo = "";
            m_currentTrendProperties.items.clear();
            m_currentTrendProperties.modificationsUnsaved = true;
            m_currentTrendProperties.isNew = true;
			
            // Add new item
            ItemInfo itemInfo;
            itemInfo.index = 1; 
            itemInfo.colour = Utilities::createHexStringFromColourRef(DEFAULT_COLOURS[0]);
            itemInfo.displayName = value.substr(0, MAX_DISPLAY_NAME_LENGTH);
            m_currentTrendProperties.items[value] = itemInfo;
			
            // Reload the graph
            loadTrendTemplate(m_currentTrendProperties);
        }
		
        FUNCTION_EXIT;
    }
	
    
    //
    // If you add a minimize button to your dialog, you will need the code below
    // to draw the icon.  For MFC applications using the document/view model,
    // this is automatically done for you by the framework.
    //
    void CHistoryViewerDlg::OnPaint() 
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
            TransActiveDialog::OnPaint();
        }
    }
	
	
    //
    // The system calls this to obtain the cursor to display while the user drags
    // the minimized window.
    //
    HCURSOR CHistoryViewerDlg::OnQueryDragIcon()
    {
        return (HCURSOR) m_hIcon;
    }
	
	
    // 
    // OnNew
    //
    void CHistoryViewerDlg::OnNew() 
    {
        FUNCTION_ENTRY("OnNew");
		
        CTrendPropertiesDialog trendPropertiesDlg(*m_itemStore.get(), *m_archiveDataStore.get());
		trendPropertiesDlg.setMaxTrendsToDisplay(m_maxTrendsDisplayed); //TD17529 
		
        try
        {
			
            if(trendPropertiesDlg.DoModal() == IDOK)
            {
                // Retrieve the Trend Properties and load the graph
                loadTrendTemplate(trendPropertiesDlg.getTrendProperties());
            }						
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
			
            std::string reason("");
            if(ex.getType() == TA_Base_Core::TrendViewerException::DIALOG_INIT_ERROR)
            {
				reason = "Error launching dialog: ";
                reason += ex.what();
            }
            else
            {
				reason = "Error launching dialog: ";
                reason += "Unknown error occured.";
            }
			
			reason += " Cannot launch TrendProperties dialog.";
			
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120026);
        }        
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // OnRefresh
    //
    void CHistoryViewerDlg::OnRefresh() 
    {
        FUNCTION_ENTRY("OnRefresh");
		
        // If refresh is pressed before the items have finished loading, it won't refresh the
        // graph until the load is complete. Since the items already added to the graph would
        // have been preloaded (either via command line or trend template), can set the
        // m_preloadPropertyItems parameter to true here so it won't wait in the addItems()
        // method until all the items have been loaded.
        m_preloadPropertyItems = true;
		
        // Retrieve the Trend Properties and load the graph
        loadTrendTemplate(m_currentTrendProperties, true);
		
        m_preloadPropertyItems = false;
		
        FUNCTION_EXIT;
    }
	
    
    //
    // OnNowCheck
    //
    void CHistoryViewerDlg::OnNowCheck() 
    {
        // liborm 04/01/2005 - fix for TD#5050 - following line moved to setLiveMode
        //m_toolBar.setNowBitmap();
		
        m_nowCheck = !m_nowCheck;
        // liborm 04/01/2005 - fix for TD#5050 - following code moved to setLiveMode
        /*
        if(m_nowCheck == true)
        {
		m_wndStatusBar.GetStatusBarCtrl().SetText("LIVE", m_wndStatusBar.CommandToIndex(ID_LIVE_MODE), 0);
        }
        else
        {
		m_wndStatusBar.GetStatusBarCtrl().SetText("", m_wndStatusBar.CommandToIndex(ID_LIVE_MODE), 0);
	}*/
		
        if(m_nowCheck)
        {
            long currentRange = m_currentTrendProperties.endDateTime = m_currentTrendProperties.startDateTime;
            m_currentTrendProperties.endDateTime = CTime::GetCurrentTime().GetTime();
            m_currentTrendProperties.startDateTime = m_currentTrendProperties.endDateTime - currentRange;
        }
		
        m_currentTrendProperties.live = m_nowCheck;
        setLiveMode(m_nowCheck);
    }
	
	
    //
    // Toggles the now button
    //
    void CHistoryViewerDlg::OnToggleNowCheck() 
    {
        m_toolBar.GetToolBarCtrl().CheckButton( ID_NOW, !m_nowCheck );
		
        OnNowCheck();
    }
	
	
    //
    // OnTrends
    //
    void CHistoryViewerDlg::OnTrends()
    {
        FUNCTION_ENTRY("OnTrends");
		
        CTrendsDialog trendsDlg(*m_itemStore.get(), *m_archiveDataStore.get(), m_loadingComplete);
		trendsDlg.setMaxTrendsToDisplay(m_maxTrendsDisplayed); //TD17529
		// TD12625 azenitha++
		// Store instance of TrendsDialog
		if (m_trendsDlg != NULL)
		{
			m_trendsDlg = NULL;
		}
		m_trendsDlg = &trendsDlg;
		// TD12625 ++azenitha
		
        if(IDOK == trendsDlg.DoModal())
        {
			m_trendsDlg = NULL; //TD17812
            LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "Loading trend template");
            loadTrendTemplate(trendsDlg.getTrendProperties());
        }
		
		// TD12625 azenitha++
		// IMPORTANT: Set instance of TrendsDialog to NULL
		if (m_trendsDlg != NULL)
		{
			m_trendsDlg = NULL;
		}
		// TD12625 ++azenitha
		
        FUNCTION_EXIT;
    }
	
	
    //
    // OnProperties
    //
    void CHistoryViewerDlg::OnProperties()
    {
        FUNCTION_ENTRY("OnProperties");
		
        CTrendPropertiesDialog trendPropertiesDlg(*m_itemStore.get(), *m_archiveDataStore.get());
        trendPropertiesDlg.setTrendProperties(m_currentTrendProperties);
		trendPropertiesDlg.setMaxTrendsToDisplay(m_maxTrendsDisplayed); //TD17529
		
        try
        {
            if(trendPropertiesDlg.DoModal() == IDOK)
            {
                loadTrendTemplate(trendPropertiesDlg.getTrendProperties());
            }
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
			
            std::string reason("");
            if(ex.getType() == TA_Base_Core::TrendViewerException::DIALOG_INIT_ERROR)
            {
				reason = "Error launching dialog: ";
                reason += ex.what();
            }
            else
            {
				reason = "Error launching dialog: ";
                reason += "Unknown error occured.";
            }
			
			reason += " Cannot launch TrendProperties dialog.";
			
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120026);
        }
		
        FUNCTION_EXIT;
    }
	
	
    //
    // OnCursor
    //
    void CHistoryViewerDlg::OnCursor()
    {
        FUNCTION_ENTRY("OnCursor");
		
        m_displayCursor = !m_displayCursor;
		
        m_toolBar.GetToolBarCtrl().CheckButton(ID_CURSOR, m_displayCursor);
		
        if(m_graph.get() != NULL)
        {
            m_graph->displayCursor(m_displayCursor, m_cursorToolIndex);
        }
		
        setMenuItemCheck(VIEW_MENU_NAME, ID_CURSOR, m_displayCursor);
		
        //
        // Enable/Disable values in display area
        //
		
        int show = SW_SHOW;
        if(m_displayCursor == false)
        {
            show = SW_HIDE;
			
            if(m_currentTrendProperties.graphType == COMPARISON)
            {
                setComparisonSettings(true);
            }
        }
		
        int numItems = m_displayItems.size();
        for(int i = 1; i < numItems + 1; ++i)
        {
            int resourceId = m_itemValueResourceIds[i];
            GetDlgItem(resourceId)->ShowWindow(show);
        }
		
        GetDlgItem(IDC_CURSOR_TIME)->ShowWindow(show);
		
        FUNCTION_EXIT;
    }
	
	
    LRESULT CHistoryViewerDlg::OnLaunchTrendsDialog(WPARAM wParam, LPARAM lParam)
    {
        // Launch the Trend dialog
        m_preloadPropertyItems = true;
        OnTrends();
        m_preloadPropertyItems = false;
        return 0;
    }
	
	
    //
    // loadItems
    //
    void CHistoryViewerDlg::loadItems()
    {
        FUNCTION_ENTRY("loadItems");
		
        try
        {
            BeginWaitCursor();
            m_itemStore->loadItems();
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
			
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120027);
        }
		
        // Disable Trends button until loading has finished
        m_toolBar.GetToolBarCtrl().EnableButton(ID_TRENDS, false);
		
        // Disable Trend item in menu
        enableMenuItem(FILE_MENU_NAME, ID_OPEN_TREND, false);
        
        // Set status string to indicate loading is taking place
        OnSetMessageString(ID_LOADING_STATUS_BAR);
		
        FUNCTION_EXIT;
    }
	
	
    //
    // parseParameters
    //
    // Get the parameters and use them to set the plot parameters
    // --trendTemplate=<template name>
    // or
    // --items=<name>,<name>,... --graph-type=<LINE, BAR, COMPARISON, XY> --sample-period=<in seconds> [--offset]
    //
    // If --trendtemplate is given, all other paremters will be ignored.
    //
    void CHistoryViewerDlg::parseParameters()
    {
        FUNCTION_ENTRY("ParseParameters");
		
        // 
        // TrendTemplate
        //
        std::string templateName = RunParams::getInstance().get("TrendTemplate");
        if(templateName.size() > 0)
        {
            try
            {
                TrendPropertiesStore trendPropertiesStore;
                TrendProperties trendProperties = trendPropertiesStore.getTrendProperties(templateName);
				
                m_currentTrendProperties.name = trendProperties.name;
                m_currentTrendProperties.description = trendProperties.description;
                m_currentTrendProperties.graphType = trendProperties.graphType;
                m_currentTrendProperties.samplePeriod = trendProperties.samplePeriod;
                m_currentTrendProperties.startDateTime = trendProperties.startDateTime;
                m_currentTrendProperties.endDateTime = trendProperties.endDateTime;
                m_currentTrendProperties.live = trendProperties.live;
                m_currentTrendProperties.extraInfo = trendProperties.extraInfo;
                m_currentTrendProperties.items = trendProperties.items;
                m_currentTrendProperties.modificationsUnsaved = trendProperties.modificationsUnsaved;
                m_currentTrendProperties.isNew = trendProperties.isNew;
				
                m_useCmdLineParams = true;
				
                // If the TrendTemplate parameter has been specified, ignore the other parameters
                FUNCTION_EXIT;
                return;
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
				
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << templateName.c_str();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120028);
				
                // Don't return here, let it attempt to load the other command line parameters if
                // they were specified.
            }
        }
		
        //
        // Items
        //
        CString items = RunParams::getInstance().get("Items").c_str();
        if(items.GetLength() > 0)
        {
            std::string name;
            int commaPosition = 0; 
            int count = 0;
            bool reachedMax = false;
            while( (commaPosition = items.Find( ',' )) >= 0 )
            {
                if(count == MAX_ITEMS_DEFAULT)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Maximum allowed items reached.");
                    reachedMax = true;
                    break;
                }
				
                name = items.Left( commaPosition ).GetBuffer(0);
                items = items.Right( items.GetLength() - (commaPosition+1) );
				
                // Only add the item if it hasn't already been added
                std::map<std::string, TA_Base_Core::ItemInfo>::iterator it = (m_currentTrendProperties.items).find(name);
                if(it == (m_currentTrendProperties.items).end())
                {
                    ++count;
					
                    ItemInfo itemInfo;
                    itemInfo.index = count;
                    itemInfo.colour = Utilities::createHexStringFromColourRef(DEFAULT_COLOURS[count - 1]);
                    itemInfo.displayName = name.substr(0, MAX_DISPLAY_NAME_LENGTH);
					
                    m_currentTrendProperties.items[name] = itemInfo;
                }
            }
			
            if(reachedMax == false)
            {
                // Get the last one
                name = items.GetBuffer(0);
				
                std::map<std::string, ItemInfo>::iterator it = m_currentTrendProperties.items.find(name);
                if(it == (m_currentTrendProperties.items).end())
                {
                    ++count;
					
                    ItemInfo itemInfo;
                    itemInfo.index = count;
                    itemInfo.colour = Utilities::createHexStringFromColourRef(DEFAULT_COLOURS[count - 1]);
                    itemInfo.displayName = name.substr(0, MAX_DISPLAY_NAME_LENGTH);
					
                    m_currentTrendProperties.items[name] = itemInfo;
                }
            }
        }
        else
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Command line parameter did not include item names.");
            FUNCTION_EXIT;
            return;
        }
		
        //
        // Graph Type
        //
        const CString GRAPH_TYPE_PARAM("GraphType");
        CString graphType = RunParams::getInstance().get(GRAPH_TYPE_PARAM).c_str();
        if(graphType.IsEmpty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Command line parameter did not include graph type. Setting to default.");
            graphType = GRAPH_TYPES[TA_Base_App::LINE].c_str();
        }
		
        try
        {
            m_currentTrendProperties.graphType = TrendPropertiesStore::getGraphType((LPCTSTR) graphType);
        }
        catch(const TA_Base_Core::TrendViewerException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
			
            if(ex.getType() == TA_Base_Core::TrendViewerException::GRAPH_TYPE_INVALID)
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << ex.what();
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120030);
                FUNCTION_EXIT;
                return;
            }
			
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << GRAPH_TYPE_PARAM;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120030);
            FUNCTION_EXIT;
            return;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
			
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << GRAPH_TYPE_PARAM;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120030);
            FUNCTION_EXIT;
            return;
        }
        
        //
        // SamplePeriod
        //
        CString samplePeriod = RunParams::getInstance().get("SamplePeriod").c_str();
        if(samplePeriod.IsEmpty())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Command line parameter did not include sample period. Setting to default.");
			//TD17529
            //samplePeriod = "60";
			m_currentTrendProperties.samplePeriod = DEFAULT_SAMPLE_PERIOD; 
        }
		
		//TD17529 - initialization is set above
        //m_currentTrendProperties.samplePeriod = strtoul(samplePeriod.GetBuffer(0), NULL, 10);
        
		
        //
        // Offset
        //
        CString offset = RunParams::getInstance().get("Offset").c_str();
        if(offset.GetLength() > 0)
        {
            m_currentTrendProperties.extraInfo = offset.GetBuffer(0);
        }
		
        m_useCmdLineParams = true;
		
        FUNCTION_EXIT;
    }
	
	
    //
    // setUpGraph
    //
    void CHistoryViewerDlg::setUpGraph()
    {
        FUNCTION_ENTRY("setUpGraph");
		
        //
        // Remove the labels from the left and right ends of the y axis
        //
        m_chartCtrl.GetAxis().GetBottom().GetLabels().SetOnAxis(false);
		
        //
        // Clicking the right arrow will now increase the time and left
        // arrow will decrease the time
        //
        CToolList toolList = m_chartCtrl.GetTools();
        long toolCount = toolList.GetCount();
		
        for(long i = 0; i < toolCount; ++i)
        {
            if(toolList.GetItems(i).GetToolType() == tcAxisArrow)
            {
                m_arrowToolIndex = i;
                CAxisArrowTool arrowTool = toolList.GetItems(m_arrowToolIndex).GetAsAxisArrow();
                arrowTool.SetScrollPercent(SCROLL_PERCENTAGE);
                break;
            }
        }
        
        //
        // Add the cursor tool to the chart to enable display of cursor
        //
        m_cursorToolIndex = m_chartCtrl.GetTools().Add(tcCursor);
        m_chartCtrl.GetTools().GetItems(m_cursorToolIndex).SetActive(false);
        CCursorTool cursorTool = m_chartCtrl.GetTools().GetItems(m_cursorToolIndex).GetAsTeeCursor();
        cursorTool.SetStyle(cssVertical);
        cursorTool.SetFollowMouse(false);
        cursorTool.SetSnap(true);
        cursorTool.GetPen().SetColor(CURSOR_COLOUR);
		
        if(m_useCmdLineParams == true)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loading graph using command line parameters");
			
            loadTrendTemplate(m_currentTrendProperties);
        }
        else
        {
            //
            // Launch the Trends dialog
            //
            /*CTrendsDialog trendsDlg(*m_itemStore.get(), *m_archiveDataStore.get(), m_loadingComplete);
            if(IDOK == trendsDlg.DoModal())
            {
			LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loading selected graph");
			
			  m_preloadPropertyItems = true;
			  m_currentTrendProperties = trendsDlg.getTrendProperties();
			  loadTrendTemplate(m_currentTrendProperties);
			  m_preloadPropertyItems = false;
			  }
            else*/
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Loading empty graph");
				
                // Just load an empty, default graph
                if(m_graph.get() != NULL)
                {
                    m_graph->setUpGraph();
                    m_graph->adjustXAxisFormat();
                    m_graph->adjustXAxisTicks();
                }
            }
        }
		
        FUNCTION_EXIT;
    }
	
	
    //
    // OnSize
    //
    void CHistoryViewerDlg::OnSize(UINT nType, int cx, int cy) 
    {
        if( m_chartCtrl) // && nType != SIZE_MAXHIDE && nType != SIZE_MINIMIZED  )
        {
            TransActiveDialog::OnSize(nType, cx, cy);
            resizeGraph(cx, cy);
        }
    }
	
	
    //
    // loadingComplete
    //
    void CHistoryViewerDlg::loadingComplete()
    {
        FUNCTION_ENTRY("loadingComplete");
		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugNorm, "loading Complete");
		
        m_loadingComplete = true;
        PostMessage(WM_LOADING_COMPLETE, NULL, NULL);
		
        FUNCTION_EXIT;
    }
	
    
    //
    // processError
    //
    void CHistoryViewerDlg::processError(const std::string& error)
    {
        FUNCTION_ENTRY("processError");
		
        std::string* errorPtr = new std::string(error);
        WPARAM wParam = reinterpret_cast <WPARAM> (errorPtr);
		
        PostMessage(WM_PROCESS_ERROR_MSG, wParam, NULL);
        
        FUNCTION_EXIT;
    }
	
	
    //
    // OnProcesErrorMessage
    //
    LRESULT CHistoryViewerDlg::OnProcessErrorMessage(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnProcesErrorMessage");
		
        std::string error = *(reinterpret_cast <std::string*> (wParam));
        std::string action("load all datapoints");
		
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << action << error;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);
        
        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "%s", error.c_str());
		
        FUNCTION_EXIT;
        return 0;
    }
	
	
    //
    // OnLoadItems
    //
    LRESULT CHistoryViewerDlg::OnLoadItems(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnLoadItems");
		
        loadItems();
		
        FUNCTION_EXIT;
        return 0;
    }
	
	
    //
    // OnLoadingComplete
    //
    LRESULT CHistoryViewerDlg::OnLoadingComplete(WPARAM wParam, LPARAM lParam)
    {
        FUNCTION_ENTRY("OnLoadingComplete");
		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "All items have been loaded. Setting buttons to enabled");
		
        EndWaitCursor();
		
        m_toolBar.GetToolBarCtrl().EnableButton(ID_PROPERTIES, true);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_TRENDS, true);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_NEW, true);
		
        enableMenuItem(VIEW_MENU_NAME, ID_PROPERTIES, true);
        enableMenuItem(FILE_MENU_NAME, ID_TRENDS, true);
        enableMenuItem(FILE_MENU_NAME, ID_NEW, true);
		
        OnSetMessageString(AFX_IDS_IDLEMESSAGE);
		
		// TD12625 azenitha++
		// Enable New button in TrendsDialog
		if (m_trendsDlg != NULL)
		{
			m_trendsDlg->enableNewButton();
		}		
		// TD12625 ++azenitha
		
        FUNCTION_EXIT;
        return 0;
    }
	
	
    //
    // addItem
    //
    void CHistoryViewerDlg::addItem(int index, const std::string& name, double lowRange, double highRange, const std::string& unitLabel, int precision)
    {
        FUNCTION_ENTRY("addItem");
		
        std::string convertedLow = Utilities::convertValueWithPrecision(lowRange, precision);
        std::string convertedHigh = Utilities::convertValueWithPrecision(highRange, precision);
		
        std::ostringstream str;
        str << "[" << convertedLow << ", " << convertedHigh << "]" << " " << unitLabel;
		
        DisplayItems displayItems;
        displayItems.itemIndex = index;
        displayItems.name = name;
        displayItems.range = str.str();
        
        m_displayItems.push_back(displayItems);
		
        reloadDisplayArea();
		
        FUNCTION_EXIT;
    }
	
    
    //
    // removeItem
    //
    void CHistoryViewerDlg::removeItem(const std::string& name)
    {
        FUNCTION_ENTRY("removeItem");
		
        std::vector<DisplayItems>::iterator it = m_displayItems.begin();
        for(; it != m_displayItems.end(); ++it)
        {
            if(name == it->name)
            {
                m_displayItems.erase(it);
                break;
            }
        }
		
        reloadDisplayArea();
		
        FUNCTION_EXIT;
    }
	
	
    //
    // setStartEndTimeDisplay
    //
    void CHistoryViewerDlg::setStartEndTime(time_t startTime, time_t endTime)
    {
        CTime start(startTime);
		
		//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec1c); // TD12474
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4); //TD17174
		
		// TD12474 ++
		//CString startTimeStr = start.Format(DATE_TIME_FORMAT_WITHOUT_SECONDS);
		CString startTimeStr = start.Format(dateFormatStr.c_str());
		// ++ TD12474
		
        m_startTimeCtrl.SetWindowText(startTimeStr);
        m_startTimeCtrl.ShowWindow(SW_SHOW);
		
        // Resize the start time static so the whole string fits
        CDC* pDCStart = m_startTimeCtrl.GetDC();
        int startWidth = pDCStart->GetTextExtent(startTimeStr.GetBuffer(0)).cx; 
        
        CRect startRect;
        m_startTimeCtrl.GetWindowRect(startRect);
        ScreenToClient(startRect);
        m_startTimeCtrl.MoveWindow(startRect.left, startRect.top, startWidth, startRect.Height());
		
        CTime end(endTime);
		
		// TD12474 ++
		//CString endTimeStr = end.Format(DATE_TIME_FORMAT_WITHOUT_SECONDS);
		CString endTimeStr = end.Format(dateFormatStr.c_str());
		// ++ TD12474
		
        m_endTimeCtrl.SetWindowText(endTimeStr);
        m_endTimeCtrl.ShowWindow(SW_SHOW);
		
        // Resize the end time static so the whole string fits
        CDC* pDCEnd = m_endTimeCtrl.GetDC();
        int endWidth = pDCEnd->GetTextExtent(endTimeStr.GetBuffer(0)).cx; 
        
        CRect endRect;
        m_endTimeCtrl.GetWindowRect(endRect);
        ScreenToClient(endRect);
		
        CRect chartRect;
        m_chartCtrl.GetWindowRect(chartRect);
        ScreenToClient(chartRect);
		
        // Move the time so that the right hand side is in line with the right side of graph
        int x = chartRect.right - endWidth;
        int y = endRect.top;
        m_endTimeCtrl.MoveWindow(x, y, endWidth, endRect.Height());
    }
	
    
    //
    // setCursorTime
    //
    void CHistoryViewerDlg::setCursorTime(time_t cursorTime)
    {
        COleDateTime cursorDateTime(cursorTime);
		
		// TD12474 ++
		//CString time = cursorDateTime.Format(DATE_TIME_FORMAT_WITHOUT_SECONDS);
		//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec1c);
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4); //TD17174
		CString time = cursorDateTime.Format(dateFormatStr.c_str());
		// ++ TD12474
		
        CString timeStr("Time: ");
		
        // If the time is 0 then the cursor is not intersecting with a series.
        // Leave the time display blank.
        if(cursorTime != 0)
        {
            timeStr += time;
        }
        
        GetDlgItem(IDC_CURSOR_TIME)->SetWindowText(timeStr.GetBuffer(0));
        
        // Resize the static so the whole time string fits
        CDC* pDCValue = GetDlgItem(IDC_CURSOR_TIME)->GetDC();
        int timeWidth = pDCValue->GetTextExtent(timeStr.GetBuffer(0)).cx; 
		
        CRect timeRect;
        GetDlgItem(IDC_CURSOR_TIME)->GetWindowRect(timeRect);
        ScreenToClient(timeRect);
        GetDlgItem(IDC_CURSOR_TIME)->MoveWindow(timeRect.left, timeRect.top, timeWidth, timeRect.Height());
    }
	
	
    //
    // setOffsetTime
    //
    void CHistoryViewerDlg::setOffsetTime(time_t offsetTime)
    {
        COleDateTime cursorDateTime(offsetTime);
		
		// TD12474 ++
		//CString time = cursorDateTime.Format(DATE_TIME_FORMAT_WITHOUT_SECONDS);
		// TD17174++
		//std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormatNoSec1c);
		std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat4);
		// ++TD17174
		CString time = cursorDateTime.Format(dateFormatStr.c_str());
		// ++ TD12474
		
        CString offStr("Offset: ");
        offStr += time;
		
        CDC* pDCOffset = GetDlgItem(IDC_OFFSET_TIME)->GetDC();
        int offsetWidth = pDCOffset->GetTextExtent(offStr.GetBuffer(0)).cx; 
        CRect offsetRect;
        GetDlgItem(IDC_OFFSET_TIME)->GetWindowRect(offsetRect);
        ScreenToClient(offsetRect);
        GetDlgItem(IDC_OFFSET_TIME)->MoveWindow(offsetRect.left, offsetRect.top, offsetWidth, offsetRect.Height());
		
        GetDlgItem(IDC_OFFSET_TIME)->SetWindowText(offStr.GetBuffer(0));
    }
	
	
    //
    // setCursorValue
    //
    void CHistoryViewerDlg::setCursorValue(int index, double value, int precision, const std::string& unitLabel)
    {
        std::string convertedValue = Utilities::convertValueWithPrecision(value, precision);
		
        std::ostringstream str;
        str << "Value: ";
		
        if(!convertedValue.empty())
        {
            str << convertedValue;
            str << " ";
            str << unitLabel;
        }
        
        std::string valueStr(str.str());
		
        // Find the display item with the given index
        int displayIndex = 0;
        std::vector<DisplayItems>::iterator iter = m_displayItems.begin();
        for(; iter != m_displayItems.end(); ++iter)
        {
            if(iter->itemIndex == index)
            {
                displayIndex = iter->displayIndex;
            }
        }
		
        if(displayIndex == 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot set cursor value for item with invalid index: %d", index);
            return;
        }
        
        int valueResourceId = m_itemValueResourceIds[displayIndex];
        
        // Resize the static so the whole value string fits
        CDC* pDCValue = GetDlgItem(valueResourceId)->GetDC();
        int valueWidth = pDCValue->GetTextExtent(valueStr.c_str()).cx + 1; 
        CRect valueRect;
        GetDlgItem(valueResourceId)->GetWindowRect(valueRect);
        ScreenToClient(valueRect);
        GetDlgItem(valueResourceId)->MoveWindow(valueRect.left, valueRect.top, valueWidth, valueRect.Height());
		
        GetDlgItem(valueResourceId)->SetWindowText(valueStr.c_str());
    }
	
	
    //
    // setOffsetValue
    //
    void CHistoryViewerDlg::setOffsetValue(int index, double value, double offsetValue, int precision, const std::string& unitLabel)
    {
        // Find the display item with the given index
        int displayIndex = 0;
        std::vector<DisplayItems>::iterator iter = m_displayItems.begin();
        for(; iter != m_displayItems.end(); ++iter)
        {
            if(iter->itemIndex == index)
            {
                displayIndex = iter->displayIndex;
            }
        }
		
        if(displayIndex == 0)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Cannot set cursor value for item with invalid index: %d", index);
            return;
        }
		
        int valueResourceId = m_itemValueResourceIds[displayIndex];
        
        std::string convertedValue = Utilities::convertValueWithPrecision(value, precision);
		
        std::ostringstream str;
        str << "Value: ";
        
        if(!convertedValue.empty())
        {
            
            // Append the value onto the end of the cursor value
            str << convertedValue;
        }
        else
        {
            str << " ";
        }
		
        str << ",  ";
		
        std::string convertedOffsetValue = Utilities::convertValueWithPrecision(offsetValue, precision);
        if(!convertedOffsetValue.empty())
        {
            // Append the offset value onto the end of the cursor value
            str << convertedOffsetValue;
        }
        else
        {
            str << " ";
        }
		
        // Append engineering unit label
        str << unitLabel;
        
        std::string valueStr = str.str();
        
        // Resize the static so the whole value string fits
        CDC* pDCValue = GetDlgItem(valueResourceId)->GetDC();
        int valueWidth = pDCValue->GetTextExtent(valueStr.c_str()).cx; 
        CRect valueRect;
        GetDlgItem(valueResourceId)->GetWindowRect(valueRect);
        ScreenToClient(valueRect);
        GetDlgItem(valueResourceId)->MoveWindow(valueRect.left, valueRect.top, valueWidth, valueRect.Height());
		
        GetDlgItem(valueResourceId)->SetWindowText(valueStr.c_str());
    }
	
	
    //
    // resetDefaultAxis
    //
    void CHistoryViewerDlg::resetDefaultAxis()
    {
		// Turn on the default axis
        m_chartCtrl.GetAxis().GetLeft().GetLabels().SetVisible(true);
        m_chartCtrl.GetAxis().GetLeft().GetTitle().SetVisible(true);
        m_chartCtrl.GetAxis().GetLeft().SetVisible(true);
        m_chartCtrl.GetAxis().GetLeft().GetLabels().SetStyle(talAuto);
		
        m_chartCtrl.GetAxis().GetBottom().GetLabels().SetVisible(true);
        m_chartCtrl.GetAxis().GetBottom().GetTitle().SetVisible(true);
        m_chartCtrl.GetAxis().GetBottom().SetVisible(true);
        m_chartCtrl.GetAxis().GetBottom().GetLabels().SetStyle(talAuto);
		
        m_chartCtrl.GetPanel().SetMarginLeft(m_leftChartMargin);
    }
	
	
    // 
    // OnClickLegend
    //
    void CHistoryViewerDlg::OnClickLegend(long Button, long Shift, long X, long Y) 
    {
        std::vector<long> nonActiveSeriesNumbers;
        for(int i = 0; i < m_chartCtrl.GetSeriesCount(); ++i)
        {
            CSeries series = m_chartCtrl.Series(i);
            
            if(series.GetActive() == false)
            {
                nonActiveSeriesNumbers.push_back(i);
            }
        }
		
        m_graph->onClickLegend(nonActiveSeriesNumbers);
    }
	
	
    // 
    // OnClickSeries
    //
    void CHistoryViewerDlg::OnClickSeries(long SeriesIndex, long ValueIndex, long Button, long Shift, long X, long Y) 
    {
        // Turn off the default axis
        m_chartCtrl.GetAxis().GetLeft().GetLabels().SetVisible(false);
        m_chartCtrl.GetAxis().GetLeft().GetTitle().SetVisible(false);
        
        // Turing off the default axis and title will shift the chart to the left.
        // Reset the left margin of the chart to the original position so the custom
        // axis labels are visible.
        m_chartCtrl.GetPanel().SetMarginLeft(m_leftChartMargin + CUSTOM_AXIS_CHART_OFFSET);
		
        m_graph->onClickSeries(SeriesIndex);
    }
	
	
    // 
    // OnClickBackground
    //
    void CHistoryViewerDlg::OnClickBackground(long Button, long Shift, long X, long Y) 
    {
        if(Button != mbLeft)
        {
            return;
        }
		
        // Don't change anything if only 1 item is displayed, because then we always want
        // the engineering unit axis to show.
        if(m_singleItemDisplayed)
        {
            return;
        }
        
        resetDefaultAxis();
        
        m_graph->onClickBackground();
    }
	
	
    // 
    // OnCursorToolChange
    //
    void CHistoryViewerDlg::OnCursorToolChange(long Tool, long X, long Y, double XVal, double YVal, long Series, long ValueIndex) 
    {
        m_graph->onCursorToolChange(Tool, X, Y, XVal, YVal, Series);
    }
	
	
    // 
    // OnZoomGraph
    //
    void CHistoryViewerDlg::OnZoomGraph() 
    {
        FUNCTION_ENTRY("OnZoomGraph");
		
        time_t startDateTime = 0;
        long rangeSeconds = 0;
        m_graph->onZoom(startDateTime, rangeSeconds, m_mouseDownX, m_mouseDownY, m_mouseUpX, m_mouseUpY, false);
		
        m_zoomed = true;
		
        m_chartCtrl.Invalidate();
		
        FUNCTION_EXIT;
    }
	
	
    //
    // OnUndoZoomGraph
    //
    void CHistoryViewerDlg::OnUndoZoomGraph() 
    {
        FUNCTION_ENTRY("OnUndoZoomGraph");
		
        time_t startDateTime = 0;
        long rangeSeconds = 0;
		
        if(m_graph.get() != NULL)
        {
            m_graph->onZoom(startDateTime, rangeSeconds, m_mouseDownX, m_mouseDownY, m_mouseUpX, m_mouseUpY, true);
        }
		
        m_zoomed = false;
		
        FUNCTION_EXIT;
    }
	
	
    //
    // OnMouseDown
    //
    void CHistoryViewerDlg::OnMouseDown(long Button, long Shift, long X, long Y) 
    {
        m_mouseDownX = X;
        m_mouseDownY = Y;
    }
	
	
    //
    // OnMouseUp
    //
    void CHistoryViewerDlg::OnMouseUp(long Button, long Shift, long X, long Y) 
    {
        m_mouseUpX = X;
        m_mouseUpY = Y;
    }
	
	
    // 
    // OnScrollGraph
    //
    void CHistoryViewerDlg::OnScrollGraph() 
    {
        FUNCTION_ENTRY("OnScrollGraph");
		
        if(m_nowCheck == true && m_currentTrendProperties.graphType != XY)
        {
            // Uncheck the button if it is in live mode
            TA_ASSERT(m_toolBar.GetToolBarCtrl().IsButtonChecked(ID_NOW), "Live Button in incorrect state");
            m_toolBar.GetToolBarCtrl().CheckButton(ID_NOW, false);
            OnNowCheck();
        }
		
        time_t startDateTime = 0;
        long rangeSeconds = 0;
        m_graph->onScroll(startDateTime, rangeSeconds, m_axisScrolled);
		
        if(m_zoomed == false)
        {
            m_currentTrendProperties.startDateTime = startDateTime;
            m_currentTrendProperties.endDateTime = startDateTime + rangeSeconds;
            // liborm - 06/01/2005 - fix for TD#7900 - see checkArchivedDataNeeded method
            checkArchivedDataNeeded(startDateTime, rangeSeconds);            
        }
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // OnAllowScroll
    //
    void CHistoryViewerDlg::OnAllowScroll(long Axis, double FAR* AMin, double FAR* AMax, BOOL FAR* AllowScroll) 
    {
        FUNCTION_ENTRY("OnAllowScroll");
		
        bool allowScroll = true;
		
        std::vector<long> scrollableAxes = m_graph->getScrollableAxesIndexes();
        std::vector<long>::iterator it = scrollableAxes.begin();
		
        for(; it != scrollableAxes.end(); ++it)
        {
            if(Axis == *it)
            {
                m_axisScrolled = Axis;
				
                // If it's the left scroll arrow clicked then always allow to scroll
                double min = m_chartCtrl.GetAxis().GetBottom().GetMinimum();            
                if(min > *AMin)
                {
                    allowScroll = true;
                }
                else
                {
                    COleDateTime endTime = *AMax;
					
                    // Add an extra 30 seconds else CTime::GetCurrentTime() will not ever
                    // be greater than maxTime
                    CTime maxTime(endTime.GetYear(), endTime.GetMonth(), endTime.GetDay(),
                        endTime.GetHour(), endTime.GetMinute(), endTime.GetSecond() + 30);
                    
                    if(maxTime > CTime::GetCurrentTime())
                    {
                        //allowScroll = false;
                        time_t currentTime = CTime::GetCurrentTime().GetTime();
                        *AMax = DATE(COleDateTime(currentTime));
						
                        time_t startTime = currentTime - (m_currentTrendProperties.endDateTime - m_currentTrendProperties.startDateTime);
                        *AMin = DATE(COleDateTime(startTime));
						
                        // Have scrolled past the current time. Don't allow the scroll, but manually adjust
                        // the X-axis so it looks at though the scroll has occurred, but only to the current
                        // date/time.
                        m_chartCtrl.GetAxis().GetBottom().SetMinMax(*AMin, *AMax);
                        m_axisScrolled = 0;
                        OnScrollGraph();
						
                        // Set this flag to false so TeeChart won't actually move the graph
                        allowScroll = false;
                    }
                    else
                    {
                        allowScroll = true;
                    }
                }
            }
        }
		
        *AllowScroll = allowScroll;
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // OnGetAxisLabel
    //
    void CHistoryViewerDlg::OnGetAxisLabel(long Axis, long SeriesIndex, long ValueIndex, BSTR FAR* LabelText) 
    {
        // Don't do anything if the graph is reloading
        if(m_resetGraph)
        {
            return;
        }
		
        if(SeriesIndex != -1)
        {
            std::string label = m_graph->getAxisLabel(Axis, SeriesIndex, ValueIndex);
			
            if(label.empty())
            {
                return;
            }
			
            VARIANT item;
            VariantInit(&item);
            item.vt = VT_BSTR;
            OLECHAR oleChar[100];
			
            int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, label.c_str(), -1, oleChar, 100);
            BSTR bstrMsg = SysAllocString(oleChar);
            item.bstrVal = bstrMsg;
			
            *LabelText = bstrMsg;
        }
    }
	
    
    // 
    // OnPrintGraph
    //
    void CHistoryViewerDlg::OnPrintGraph() 
    {
        FUNCTION_ENTRY("OnPrintGraph");
		
        if(m_chartCtrl.GetPrinter().GetPrinterCount() <= 0)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120019);
            return;
        }
		
        //
        // Display a dialog to give the operator the option
        // of printing a title with the graph
        //
        CPrintTitleDlg titleDlg;
        if(titleDlg.DoModal() == IDOK)
        {
            std::vector<CString> titles = titleDlg.getTitles();
			
            m_chartCtrl.GetHeader().GetText().Clear();
			
			//TD17674++ - simplified implementation
			COleVariant var;
			for(int i = 0; i < titles.size(); ++i)
			{
                if(titles[i] == "")
                {
                    continue;
                }
				var = titles[i];
				m_chartCtrl.GetHeader().GetText().Add(*(LPCVARIANT)var);
				
			}
			/*
            for(int i = 0; i < titles.size(); ++i)
            {
			if(titles[i] == "")
			{
			continue;
			}
			
			  VARIANT item;
			  VariantInit(&item);
			  item.vt = VT_BSTR;
			  
                // Add an extra 4 bytes in just in case there are return characters. If 2 lines
                // were used for the title an extra 2 characters would have been allowed (see comment
                // in PrintTitleDlg::OnChangeTitle()).
                OLECHAR oleChar[104];
                int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, titles[i], -1, oleChar, 104);
				
				  try
				  {
				  BSTR bstrMsg = SysAllocString(oleChar);
				  item.bstrVal = bstrMsg;
				  m_chartCtrl.GetHeader().GetText().Add(item);
				  
                    SysFreeString(oleChar);
					}
					catch(...)
					{
                    // For some reason SysFreeString occassionally throws an exception, even though oleChar
                    // is valid. This doesn't affect the print title appearing however. So this catch is 
                    // here just in case this happens so the Trend Viewer doesn't shut down.
					}
					}
			*/            
			//++TD17674 (lizettejl)
			
            m_chartCtrl.GetHeader().SetColor(DEFAULT_PEN_COLOUR);
            m_chartCtrl.GetHeader().SetVisible(TRUE);
            
        }
        else
        {
            return;
        }
		
        //
        // Check if the date and time needs to be printed
        //
        if(titleDlg.doPrintDateTime() == true)
        {
            // Get the date and time
            CTime timeNow = CTime::GetCurrentTime();
			
            // TD12474 ++
            //CString dateTime = timeNow.Format(DATE_TIME_FORMAT);
			std::string dateFormatStr = m_DateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);
            CString dateTime = timeNow.Format(dateFormatStr.c_str());
            // ++ TD12474
            
            VARIANT item;
            VariantInit(&item);
            item.vt = VT_BSTR;
            OLECHAR oleChar[100];
			
            try
            {
                int x = MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, dateTime, -1, oleChar, 100);
                BSTR bstrMsg = SysAllocString(oleChar);
                item.bstrVal = bstrMsg;
                m_chartCtrl.GetHeader().GetText().Add(item);
				
                SysFreeString(oleChar);
            }
            catch(...)
            {
                // For some reason SysFreeString occassionally throws an exception, even though oleChar
                // is valid. This doesn't affect the print title appearing however. So this catch is 
                // here just in case this happens so the Trend Viewer doesn't shut down.
            }
        }
		
        //
        // Check if the pen needs to be changed
        //
        if(titleDlg.doChangePen() == true)
        {
            try
            {
                m_graph->updatePenStyle(false);
            }
            catch(...)
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
				
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_120031);
            }
        }
		
        //
        // Set the margins to 5% each
        //
        m_chartCtrl.GetPrinter().SetMarginBottom( DEFAULT_PRINTER_BOTTOM_MARGIN );
        m_chartCtrl.GetPrinter().SetMarginTop( DEFAULT_PRINTER_TOP_MARGIN );
        m_chartCtrl.GetPrinter().SetMarginLeft( DEFAULT_PRINTER_LEFT_MARGIN );
        m_chartCtrl.GetPrinter().SetMarginRight( DEFAULT_PRINTER_RIGHT_MARGIN );
		// TD13711 - set the margins accdg to specs
        //m_chartCtrl.GetPrinter().SetPrintProportional(true); llizettejl
        
        //
        // Show the preview and they can print from there.
        //
        m_chartCtrl.GetPrinter().ShowPreview();
        
        //
        // Remove the header and change the pen styles back to the original
        //
		//TD17749++ - must redisplay trend property name after print dialog is closed
        //m_chartCtrl.GetHeader().SetVisible(FALSE);
		m_chartCtrl.GetHeader().GetText().Clear();		
		COleVariant var;
		var = m_currentTrendProperties.name.c_str();
		m_chartCtrl.GetHeader().GetText().Add(*(LPCVARIANT)var);
		//++TD17749
        if(titleDlg.doChangePen() == true)
        {
            m_graph->updatePenStyle(true);
        }
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // OnExport
    //
    void CHistoryViewerDlg::OnExport() 
    {
        FUNCTION_ENTRY("OnExport");
		
        m_chartCtrl.GetExport().ShowExport();
		
        FUNCTION_EXIT;
    }
	
	
    //
    // DestroyWindow
    // 
    BOOL CHistoryViewerDlg::DestroyWindow()
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "DestroyWindow");
		
        RunParams::getInstance().deregisterRunParamUser(this);
		
        // The graph object needs to be explicitly deleted here. This is because calls
        // are made on the chart object when the graph is deleted and if we wait for the 
        // graph object to be automatically cleaned up, the chart object has already been 
        // deleted and the application will assert.
        m_graph.reset();
		
        IItemStore* itemStore = m_itemStore.release();
        if(itemStore != NULL)
        {
            delete itemStore;
            itemStore = NULL;
        }
		
        IArchiveDataStore* dataStore = m_archiveDataStore.release();
        if(dataStore != NULL)
        {
            delete dataStore;
            dataStore = NULL;
        }
		
        LOG(SourceInfo, DebugUtil::FunctionExit, "DestroyWindow");
		
        return TransActiveDialog::DestroyWindow();
    }
	
	
    //
    // OnClose
    //
    void CHistoryViewerDlg::OnClose() 
    {
        LOG(SourceInfo, DebugUtil::FunctionEntry, "OnClose");
		
        LOG(SourceInfo, DebugUtil::FunctionExit, "OnClose");		
		//TD18095, jianghp, to fix the performance of showing manager application
		DestroyWindow();
   }
	
	
	//
    // OnAppAbout
    //
    void CHistoryViewerDlg::OnAppAbout() 
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
    }
	
	
    //
    // OnHelp
    //
    void CHistoryViewerDlg::OnHelp() 
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
    }
	
	
    //
    // OnCreate
    //
    int CHistoryViewerDlg::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        FUNCTION_ENTRY("OnCreate");
		
        if (TransActiveDialog::OnCreate(lpCreateStruct) == -1)
        {
            return -1;
        }
        
        if (!m_wndStatusBar.CreateEx( this, SBT_TOOLTIPS | SBARS_SIZEGRIP,
            WS_CHILD | WS_VISIBLE | CBRS_BOTTOM, AFX_IDW_TOOLBAR) ||
            !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)) )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to create status bar\n");
        }
		
        CFont font;
        font.CreatePointFont(FONT_SIZE,FONT,NULL);
        m_wndStatusBar.SetFont(&font,FALSE);
        m_wndStatusBar.GetStatusBarCtrl().SetTipText( 0, "Ready" );
        m_wndStatusBar.SetPaneInfo(0, m_wndStatusBar.GetItemID(0),    SBPS_STRETCH, NULL );
        OnSetMessageString(AFX_IDS_IDLEMESSAGE);
		
        createToolbar();
        
        FUNCTION_EXIT;
        return 0;
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnSetMessageString
    //      OnSetMessageString updates the status bar text.
    //
    //      This code is based on CFrameWnd::OnSetMessageString. We assume
    //      a string ID is always passed in wParam.
    LRESULT CHistoryViewerDlg::OnSetMessageString(WPARAM wParam, LPARAM lParam)
    {
        UINT    nIDMsg = (UINT)wParam;
        CString strMsg;
		
        if (nIDMsg)
        {
            if (strMsg.LoadString(nIDMsg) != 0)
            {
                //
                // Strip anything after the newline (newline breaks up the status bar
                // help from the toop tip text).
                //
                CString strStatusText;
                AfxExtractSubString(strStatusText, strMsg, 0, '\n');
                if(m_serverDown == true)
                {
                    m_wndStatusBar.SetWindowText("Disconnected from server...");
                }
                else if(m_loadingComplete == false)
                {
                    m_wndStatusBar.SetWindowText("Loading...");
                }
                else
                {
                    m_wndStatusBar.SetWindowText(strStatusText);
                }
            }
            else
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Warning: no message line prompt for ID %x%04X\n", nIDMsg);
            }
        }
		
        UINT nIDLast     = m_nIDLastMessage;
        m_nIDLastMessage = nIDMsg;
        m_nIDTracking    = nIDMsg;
        return nIDLast;
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CModelessMain::OnPopMessageString
    //      Resets status bar message string. This code is based on
    //      CFrameWnd::OnPopMessageString
    LRESULT CHistoryViewerDlg::OnPopMessageString(WPARAM wParam, LPARAM lParam)
    {
        if (m_nFlags & WF_NOPOPMSG)
        {
            return 0;
        }
		
        return SendMessage(WM_SETMESSAGESTRING, wParam, lParam);
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnEnterIdle
    //      OnEnterIdle updates the status bar when there's nothing better to do.
    //      This code is based on CFrameWnd::OnEnterIdle.
    void CHistoryViewerDlg::OnEnterIdle(UINT nWhy, CWnd* pWho)
    {
        if(m_serverDown == true)
        {
            OnSetMessageString(ID_SERVER_DOWN_STATUS_BAR);
            return;
        }
		
        if (nWhy != MSGF_MENU || m_nIDTracking == m_nIDLastMessage)
        {
            return;
        }
		
        OnSetMessageString(m_nIDTracking);
        TA_ASSERT(m_nIDTracking == m_nIDLastMessage, "Message Ids don't match");
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnMenuSelect
    //      OnMenuSelect updates the status bar message, based on the state
    //      of the dialog menu.
    //
    //      This code is based on CFrameWnd::OnMenuSelect.  We assume the
    //      application does not support context sensitive help.
    void CHistoryViewerDlg::OnMenuSelect(UINT nItemID, UINT nFlags, HMENU hSysMenu)
    {
        // set the tracking state
        if (nFlags == 0xFFFF)
        {
            // cancel menu operation (go back to idle now)
            m_nIDTracking = AFX_IDS_IDLEMESSAGE;
            OnSetMessageString(m_nIDTracking);  // set string now
            TA_ASSERT(m_nIDTracking == m_nIDLastMessage, "Message Ids don't match");
        }
        else if (nItemID == 0 || nFlags & (MF_SEPARATOR|MF_POPUP|MF_MENUBREAK|MF_MENUBARBREAK))
        {
            // nothing should be displayed
            m_nIDTracking = 0;
        }
        else if (nItemID >= 0xF000 && nItemID < 0xF1F0)
        {
            // special string table entries for system commands
            m_nIDTracking = ID_COMMAND_FROM_SC(nItemID);
            TA_ASSERT(m_nIDTracking >= AFX_IDS_SCFIRST && m_nIDTracking < AFX_IDS_SCFIRST + 31, "Unexpected Ids");
        }
        else
        {
            // track on idle
            m_nIDTracking = nItemID;
        }
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnInitMenuPopup
    //      OnInitMenuPopup updates the state of items on a popup menu.
    //
    //      This code is based on CFrameWnd::OnInitMenuPopup.  We assume the
    //      application does not support context sensitive help.
    void CHistoryViewerDlg::OnInitMenuPopup(CMenu* pPopupMenu, UINT nIndex, BOOL bSysMenu)
    {
        if (!bSysMenu)
        {
            TA_ASSERT(pPopupMenu != NULL, "Menu is NULL");
			
            // check the enabled state of various menu items
            CCmdUI state;
            state.m_pMenu = pPopupMenu;
            TA_ASSERT(state.m_pOther == NULL, "State is NULL");
			
            state.m_nIndexMax = pPopupMenu->GetMenuItemCount();
            for (state.m_nIndex = 0; state.m_nIndex < state.m_nIndexMax;
			state.m_nIndex++)
            {
                state.m_nID = pPopupMenu->GetMenuItemID(state.m_nIndex);
                if (state.m_nID == 0)
                {
                    continue; // menu separator or invalid cmd - ignore it
                }
				
                TA_ASSERT(state.m_pOther == NULL, "State is NULL");
                TA_ASSERT(state.m_pMenu != NULL, "State is NULL");
                if (state.m_nID == (UINT)-1)
                {
                    // possibly a popup menu, route to first item of that popup
                    state.m_pSubMenu = pPopupMenu->GetSubMenu(state.m_nIndex);
                    if (state.m_pSubMenu == NULL ||
                        (state.m_nID = state.m_pSubMenu->GetMenuItemID(0)) == 0 ||
                        state.m_nID == (UINT)-1)
                    {
                        continue; // first item of popup can't be routed to
                    }
                    state.DoUpdate(this, FALSE);  // popups are never auto disabled
                }
                else
                {
                    // normal menu item
                    // Auto enable/disable if command is _not_ a system command
                    state.m_pSubMenu = NULL;
                    UINT itemState = pPopupMenu->GetMenuState(state.m_nID, MF_BYCOMMAND);
                    if( !(itemState & 0x03) )
                    {
                        state.DoUpdate(this, state.m_nID < 0xF000);
                    }
                }
            }
        }
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnStatusBarCheck
    //      OnStatusBarCheck toggles the status of the status bar when the
    //      corresponding View option is selected from the menu.
    void CHistoryViewerDlg::OnStatusBarCheck()
    {
        m_wndStatusBar.ShowWindow(m_wndStatusBar.IsWindowVisible() ? SW_HIDE : SW_SHOWNA);
        resizeGraph();
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnUpdateStatusBarMenu
    //      OnUpdateStatusBarMenu checks or unchecks the View option,
    //      depending on whether or not the status bar is visible.
    void CHistoryViewerDlg::OnUpdateStatusBarMenu(CCmdUI* pCmdUI)
    {
        TA_ASSERT(pCmdUI->m_nID == ID_VIEW_STATUS_BAR, "Id is not Status Bar");
        pCmdUI->SetCheck(m_wndStatusBar.IsWindowVisible());
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnToolBarCheck
    //      OnToolBarCheck toggles the status of the toolbar when the
    //      corresponding View option is selected from the menu.
    void CHistoryViewerDlg::OnToolBarCheck()
    {
        int showWindow = SW_SHOWNA;
        if(m_toolBar.IsWindowVisible())
        {
            showWindow = SW_HIDE;
        }
		
        m_toolBar.ShowWindow(showWindow);
        resizeGraph();
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CHistoryViewerDlg::OnUpdateToolBarMenu
    //      OnUpdateToolBarMenu checks or unchecks the View option,
    //      depending on whether or not the toolbar is visible.
    void CHistoryViewerDlg::OnUpdateToolBarMenu(CCmdUI* pCmdUI)
    {
        TA_ASSERT(pCmdUI->m_nID == ID_VIEW_TOOLBAR, "Id is not Tool Bar");
        pCmdUI->SetCheck(m_toolBar.IsWindowVisible());
    }
	
	
    //
    // serverDown
    //
    void CHistoryViewerDlg::serverDown()
    {
        m_serverDown = true;
    }
	
	
    //
    // serverUp
    //
    void CHistoryViewerDlg::serverUp()
    {
        m_serverDown = false;
    }
	
	
    //
    // resizeGraph
    //
    void CHistoryViewerDlg::resizeGraph(int cx, int cy)
    {
        FUNCTION_ENTRY("resizeGraph");
		
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
		
        if( m_chartCtrl )
        {
            //
            // Resize the graph
            //
			
            static bool firstTime = true;
            static int timeGraphOffset = 0;
            static int displayAreaOffset = 0;
            
            CRect client;
            GetClientRect(&client);     
            
            int windowHeight = client.Height();
            int windowWidth = client.Width();
			
            CRect graphRect;
            m_chartCtrl.GetWindowRect(&graphRect);
            ScreenToClient(&graphRect);
            
            CRect displayAreaRect;
            m_displayArea.GetWindowRect(&displayAreaRect);
            ScreenToClient(&displayAreaRect);
			
            CRect startTimeRect;
            m_startTimeCtrl.GetWindowRect(&startTimeRect);
            ScreenToClient(&startTimeRect);
			
            //
            // Adjust horizontally
            //
            if(firstTime == true)
            {
                //
                // Find the horizontal offset between the chart and the display area
                //
                displayAreaOffset = displayAreaRect.left - graphRect.right;
            }
			
            //
            // Adjust size for the display area
            //
            windowWidth -= displayAreaRect.Width();
            windowWidth -= displayAreaOffset*2;
            
            int topAdjust = 0;
            int bottomAdjust = 0;
			
            //
            // Adjust Vertically
            //
            
            //
            // Get toolbar coordinates (if visible)
            //
            if(m_toolBar.IsWindowVisible())
            {
                CRect toolBarRect;
                m_toolBar.GetWindowRect(&toolBarRect);
                ScreenToClient(&toolBarRect);
                topAdjust += toolBarRect.Height();
            }
            else
            {
                topAdjust = 45;
            }
			
            // Ajust height for toolbar
            windowHeight -= topAdjust;
			
            int tempY = 0;
			
            //
            // Get Status bar coodinates (if visible)
            //
            if(m_wndStatusBar.IsWindowVisible())
            {
                CRect statusBarRect;
                m_wndStatusBar.GetWindowRect(&statusBarRect);
                ScreenToClient(&statusBarRect);
				
                // Ajudst height for status bar
                bottomAdjust += statusBarRect.Height();
                
                tempY = statusBarRect.top;
            }
            else
            {
                bottomAdjust += 20;
                tempY = client.bottom;
            }
			
            // Adjust height for status bar
            windowHeight -= bottomAdjust;
			
            // Adjust height for time placeholders
            windowHeight -= startTimeRect.Height();
			
            if(firstTime)
            {
                //
                // Get the offset between the graph and the start/end time placeholders
                //
                timeGraphOffset = startTimeRect.top - graphRect.bottom;
            }
			
            // Adjust height for the offset
            windowHeight -= (timeGraphOffset*2);
			
            //
            // Resize the chart
            //
            m_chartCtrl.MoveWindow(0, topAdjust, windowWidth, windowHeight, true);
            
            //
            // Resize the display area
            //
            int displayAreaX = windowWidth+displayAreaOffset;
            int displayAreaY = topAdjust;
            int displayAreaWidth = displayAreaRect.Width();
            int displayAreaHeight = windowHeight;
            m_displayArea.MoveWindow(displayAreaX, displayAreaY, displayAreaWidth, displayAreaHeight);
			
            //
            // Move the time displays
            //
            CString startTime("");
            m_startTimeCtrl.GetWindowText(startTime);
            //int yTime = topAdjust + windowHeight + timeGraphOffset;
            int yTime = client.bottom - bottomAdjust - timeGraphOffset - startTimeRect.Height();
            CDC * pDCstart = m_startTimeCtrl.GetDC();
            int startTimeWidth = pDCstart->GetTextExtent(startTime.GetBuffer(0)).cx;   
            m_startTimeCtrl.MoveWindow(30, yTime, startTimeWidth, startTimeRect.Height());
			
            CString endTime("");
            m_endTimeCtrl.GetWindowText(endTime);
            CDC * pDCend = m_endTimeCtrl.GetDC();
            int endTimeWidth = pDCend->GetTextExtent(endTime.GetBuffer(0)).cx;   
            int xTime = windowWidth - endTimeWidth;
            m_endTimeCtrl.MoveWindow(xTime, yTime, endTimeWidth, startTimeRect.Height());
			
            //
            // Move the item information
            //
            CRect newDisplayArea(displayAreaX, displayAreaY, displayAreaX + displayAreaWidth, displayAreaY + displayAreaHeight);
            resizeDisplayArea(displayAreaRect, newDisplayArea, firstTime);
			
            firstTime = false;
        }
		
        FUNCTION_EXIT;
    }
	
	
    //
    // resizeDisplayArea
    //
    void CHistoryViewerDlg::resizeDisplayArea(const CRect& oldDisplayAreaRect, const CRect& newDisplayAreaRect, bool firstTime)
    {
        static int topOffset = 0;
        static int nameRangeOffset = 0;
        static int rangeValueOffset = 0;
        static int rangeNameOffset = 0;
        static int leftOffset = 0;
        static int cursorOffset = 0;
        static int offsetOffset = 0;
        
        CRect itemNameRect1;
        GetDlgItem(IDC_STATIC1)->GetWindowRect(itemNameRect1);
        ScreenToClient(itemNameRect1);
		
        CRect itemNameRect2;
        GetDlgItem(IDC_STATIC2)->GetWindowRect(itemNameRect2);
        ScreenToClient(itemNameRect2);
		
        CRect itemRangeRect;
        GetDlgItem(IDC_RANGE1)->GetWindowRect(itemRangeRect);
        ScreenToClient(itemRangeRect);
		
        CRect valueRangeRect;
        GetDlgItem(IDC_VALUE1)->GetWindowRect(valueRangeRect);
        ScreenToClient(valueRangeRect);
		
        CRect cursorTimeRect;
        GetDlgItem(IDC_CURSOR_TIME)->GetWindowRect(cursorTimeRect);
        ScreenToClient(cursorTimeRect);
		
        CRect offsetTimeRect;
        GetDlgItem(IDC_OFFSET_TIME)->GetWindowRect(offsetTimeRect);
        ScreenToClient(offsetTimeRect);
		
        if(firstTime == true)
        {
            topOffset = itemNameRect1.top - oldDisplayAreaRect.top;
            nameRangeOffset = itemRangeRect.top - itemNameRect1.bottom;
            rangeValueOffset = valueRangeRect.top - itemRangeRect.bottom;
            rangeNameOffset = itemNameRect2.top - valueRangeRect.bottom;
            leftOffset = itemNameRect1.left - oldDisplayAreaRect.left;
            cursorOffset = offsetTimeRect.top - cursorTimeRect.bottom;
            offsetOffset = oldDisplayAreaRect.bottom - offsetTimeRect.bottom;
        }
		
        int x = newDisplayAreaRect.left + leftOffset;
        int y = newDisplayAreaRect.top + topOffset;
		
        std::map<int, int>::iterator itName = m_itemNameResourceIds.begin();
        std::map<int, int>::iterator itRange = m_itemRangeResourceIds.begin();
        std::map<int, int>::iterator itValue = m_itemValueResourceIds.begin();
		
        //
        // Resize all the static placeholders
        //
        for(; itName != m_itemNameResourceIds.end(); ++itName, ++itRange, ++itValue)
        {
            CRect itemNameRect;
            GetDlgItem(itName->second)->GetWindowRect(itemNameRect);
            ScreenToClient(itemNameRect);
            GetDlgItem(itName->second)->MoveWindow(x, y, itemNameRect.Width(), itemNameRect.Height());
			
            y += itemNameRect1.Height() + nameRangeOffset;
			
            CRect rangeRect;
            GetDlgItem(itRange->second)->GetWindowRect(rangeRect);
            ScreenToClient(rangeRect);
            GetDlgItem(itRange->second)->MoveWindow(x, y, rangeRect.Width(), rangeRect.Height());
			
            y += itemRangeRect.Height() + rangeValueOffset;
            
            CRect valueRect;
            GetDlgItem(itValue->second)->GetWindowRect(valueRect);
            ScreenToClient(valueRect);
            GetDlgItem(itValue->second)->MoveWindow(x, y, valueRect.Width(), valueRect.Height());
            
            y += valueRangeRect.Height() + rangeNameOffset;
        }
		
        y = newDisplayAreaRect.bottom - offsetOffset - offsetTimeRect.Height();
        GetDlgItem(IDC_OFFSET_TIME)->MoveWindow(x, y, offsetTimeRect.Width(), offsetTimeRect.Height());
		
        GetDlgItem(IDC_CURSOR_TIME)->MoveWindow(x, y - cursorOffset - cursorTimeRect.Height(), cursorTimeRect.Width(), cursorTimeRect.Height());
    }
	
	
    //
    // resizeGraph
    //
    void CHistoryViewerDlg::resizeGraph()
    {
        CRect client;
        GetClientRect(client);
        resizeGraph(client.Width(), client.Height());
    }
	
	
    /////////////////////////////////////////////////////////////////////////////
    // CModelessMain::OnToolTipText
    //      Handles TTN_NEEDTEXT message to display tooltips for the toolbar.
    //      This code is based on CFrameWnd::OnToolTipText
    BOOL CHistoryViewerDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
    {
        TA_ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW, "Unexpected codes");
		
        // allow top level routing frame to handle the message
        if (GetRoutingFrame() != NULL)
        {
            return FALSE;
        }
		
        // need to handle both ANSI and UNICODE versions of the message
        TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
        TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
        TCHAR szFullText[256];
        CString strTipText;
        UINT nID = pNMHDR->idFrom;
        if (pNMHDR->code == TTN_NEEDTEXTA && (pTTTA->uFlags & TTF_IDISHWND) ||
            pNMHDR->code == TTN_NEEDTEXTW && (pTTTW->uFlags & TTF_IDISHWND))
        {
            // idFrom is actually the HWND of the tool
            nID = ((UINT)(WORD)::GetDlgCtrlID((HWND)nID));
        }
		
        if (nID != 0) // will be zero on a separator
        {
            AfxLoadString(nID, szFullText);
			// this is the command id, not the button index
            AfxExtractSubString(strTipText, szFullText, 1, '\n');
        }
#ifndef _UNICODE
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            lstrcpyn(pTTTA->szText, strTipText, (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
        }
        else
        {
            _mbstowcsz(pTTTW->szText, strTipText, (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
        }
#else
        if (pNMHDR->code == TTN_NEEDTEXTA)
        {
            _wcstombsz(pTTTA->szText, strTipText, (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
        }
        else
        {
            lstrcpyn(pTTTW->szText, strTipText, (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
        }
#endif
        *pResult = 0;
		
        // bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0, SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
		
        return TRUE;    // message was handled
    }
	
	
    //
    // createToolbar
    //
    void CHistoryViewerDlg::createToolbar()
    {
        FUNCTION_ENTRY("createToolbar");
		
        //
        // Create toolbar at the top of the dialog window
        //
        if (!m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS 
            | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) || !m_toolBar.LoadToolBar(IDR_MAINTOOLBAR))
        {
            m_toolBar.LoadToolBar(IDR_MAINTOOLBAR);
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Failed to create toolbar\n");
        }
		
        //
        // Reset the font
        //
        CFont font;
        font.CreatePointFont(FONT_SIZE,FONT,NULL);
        m_toolBar.SetFont(&font,TRUE);
		
        //
        // Add text to the buttons
        // Any exception from these functions will fall through and be caught by
        // the calling member
        //
        addTextToButton( ID_NEW, "New" );
        addTextToButton( ID_TRENDS, "Trends" );
        addTextToButton( ID_PROPERTIES, "Properties" );
        addTextToButton( ID_EXPORT, "Export" );
        addTextToButton( ID_PRINT_GRAPH, "Print" );
        addTextToButton( ID_CURSOR, "Cursor" );
        addTextToButton( ID_NOW, "Live" );
        addTextToButton( ID_REFRESH, "Refresh" );
        addTextToButton( ID_HELP, "Help" );
        m_toolBar.SetButtonStyle( m_toolBar.CommandToIndex(ID_NOW ), TBBS_CHECKBOX);
        m_toolBar.SetButtonStyle( m_toolBar.CommandToIndex(ID_CURSOR ), TBBS_CHECKBOX);
        
		// TD13728 - lizettejl - resized to view the complete text for "Properties"
        // m_toolBar.GetToolBarCtrl().SetButtonSize(CSize(59, 40));
		m_toolBar.GetToolBarCtrl().SetButtonSize(CSize(80, 40));
		
        m_toolBar.addNowBitmaps();
		
        //
        // Disable Live button, Properties button and Trends button - 
        // Properties button will be enabled when there is a templated loaded into the graph,
        // Trend button will be enabled when all the items have been loaded from database.
        //
        m_toolBar.GetToolBarCtrl().EnableButton(ID_PROPERTIES, false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_TRENDS, false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_NEW, false);
        m_toolBar.setNowBitmap();
		
        FUNCTION_EXIT;
    }
	
	
    //
    // addTextToButton
    //
    void CHistoryViewerDlg::addTextToButton(UINT buttonId, const CString& buttonLabel)
    {
        int index = m_toolBar.CommandToIndex( buttonId );
        if( index >=0 )
        {
            m_toolBar.SetButtonText( index, buttonLabel );
        }
        else
        {
            LOG(SourceInfo, DebugUtil::GenericLog,  DebugUtil::DebugError, "Unable to find %s button", buttonLabel);
            
            TA_ASSERT(false, "Button has not been added");
        }
    }
	
	
    //
    // saveUserSettings
    //
    void CHistoryViewerDlg::saveUserSettings(std::map<std::string, std::string>& userSettings)
    {
        FUNCTION_ENTRY("saveUserSettings");
		
        // This method is left in here in case it is needed in the future
		
        FUNCTION_EXIT;
    }
	
	
    //
    // loadUserSettings
    //
    void CHistoryViewerDlg::loadUserSettings(TA_Base_Bus::SettingsMgr& settingMgr)
    {
        FUNCTION_ENTRY("loadUserSettings");
		
        // This method is left in here in case it is needed in the future
		
        FUNCTION_EXIT;
    }
	
	
    //
    // restoreUserSettings
    //
    void CHistoryViewerDlg::restoreUserSettings()
    {
        FUNCTION_ENTRY("restoreUserSettings");
		
        // This method is left in here in case it is needed in the future
		
        FUNCTION_EXIT;
    }
    
	
    // 
    // findMenuItem
    //
    int CHistoryViewerDlg::findMenuItem(const CString& menuName)
    {
        // findMenuItem() will find a menu item from the specified menu and returns
        // its position (0-based) in the specified menu.  It returns -1 if no menu
        // item is found.
		
        CMenu* topMenu = GetMenu();
        TA_ASSERT(topMenu != NULL, "Menu does not exist");
        TA_ASSERT(::IsMenu(topMenu->GetSafeHmenu()), "Menu does not exist");
		
        int count = topMenu->GetMenuItemCount();
        CString str;
        for (int i = 0; i < count; i++)
        {
            if( topMenu->GetMenuString(i, str, MF_BYPOSITION) && str == menuName )
            {
                return i;
            }
        }
		
        // If execution reaches this point, an programatic error has been made.
        // Assert false.
        CString errorMsg("Missing menu item");
        errorMsg += ": ";
        errorMsg += menuName;
        TA_ASSERT(false, errorMsg.GetBuffer(0));
		
        return 0;
    }
	
    
    // 
    // enableMenuItem
    //
    void CHistoryViewerDlg::enableMenuItem(const CString& menuName, const UINT& menuItemID, bool enable)
    {
        CMenu* topMenu = GetMenu();
        TA_ASSERT(topMenu != NULL, "Menu does not exist");
		
        CMenu* subMenu = topMenu->GetSubMenu(findMenuItem(menuName));
		
        if(enable == false)
        {
            subMenu->EnableMenuItem(menuItemID, MF_BYCOMMAND | MF_DISABLED | MF_GRAYED);
        }
        else
        {
            subMenu->EnableMenuItem(menuItemID, MF_BYCOMMAND | MF_ENABLED);
        }
    }
	
	
    // 
    // setMenuItemCheck
    //
    void CHistoryViewerDlg::setMenuItemCheck(const CString& menuString, const UINT& menuItemID, bool bChecked)
    {
        //
        // Get the top memu
        //
        CMenu* topMenu = GetMenu();
        TA_ASSERT(topMenu != NULL, "Menu does not exist");
		
        //
        // Find the sub-menu specified by subMenuID
        //
        CMenu* subMenu = topMenu->GetSubMenu(findMenuItem(menuString));
        subMenu->CheckMenuItem(menuItemID, MF_BYCOMMAND | (bChecked ? MF_CHECKED : MF_UNCHECKED));
    }
	
	
    // 
    // loadTrendTemplate
    //
    void CHistoryViewerDlg::loadTrendTemplate(const TrendProperties& trendProperties, bool refresh /*=false*/)
    {
        FUNCTION_ENTRY("loadTrendTemplate");
		
        TA_Base_Core::ThreadGuard guard(m_templateLock);
		
        m_resetGraph = true;
		
        m_graph.reset();
		
        try
        {
            if(!refresh)
            {
                // Reset the left margin if it had been shifted by a different graph type
                m_chartCtrl.GetTools().GetItems(m_arrowToolIndex).SetActive(true);
				
                // Reset the default axes in case they had been changed by a different graph type
                resetDefaultAxis();
            }
			
            // The cursor needs to be reset before setUpGraph() is called for Comparison Graph
            resetCursor(); 
			
            // If is live graph, reset the start date/time according to current time and range
            time_t startDateTime = trendProperties.startDateTime;
            time_t rangeInSeconds = trendProperties.endDateTime - trendProperties.startDateTime;
			
            if(trendProperties.live == true)
            {
                CTime nowTime = CTime::GetCurrentTime();
                time_t startTime = nowTime.GetTime() - rangeInSeconds;
                startDateTime = startTime;
            }
			
            time_t endTime = startDateTime + rangeInSeconds;
            
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Creating new graph...");
			
            // Create the new graph
            switch(trendProperties.graphType)
            {
			case LINE:          m_graph = GraphTypeFactory::getInstance().createLineGraph(
									m_chartCtrl, 
									trendProperties.name,
									trendProperties.description,
									startDateTime,
									endTime,
									trendProperties.live,
									trendProperties.samplePeriod,
									m_itemStore.get(),
									m_archiveDataStore.get(),
									this);
				break;
				
			case BAR:           m_graph = GraphTypeFactory::getInstance().createBarGraph(
									m_chartCtrl, 
									trendProperties.name,
									trendProperties.description,
									startDateTime,
									endTime,
									trendProperties.live,
									trendProperties.samplePeriod,
									m_itemStore.get(),
									m_archiveDataStore.get(),
									this,
									trendProperties.extraInfo == "Live");
				break;
				
			case COMPARISON:    m_graph = GraphTypeFactory::getInstance().createComparisonGraph(
									m_chartCtrl, 
									trendProperties.name,
									trendProperties.description,
									startDateTime,
									endTime,
									trendProperties.live,
									trendProperties.samplePeriod,
									m_itemStore.get(),
									m_archiveDataStore.get(),
									this,
									Utilities::convertOffsetStringToSeconds(trendProperties.extraInfo.c_str(), trendProperties.startDateTime),
									m_leftChartMargin,
									m_cursorToolIndex,
									m_arrowToolIndex);
				
				break;
				
			case XY:            m_graph = GraphTypeFactory::getInstance().createXYGraph(
									m_chartCtrl, 
									trendProperties.name,
									trendProperties.description,
									startDateTime,
									endTime,
									trendProperties.live,
									trendProperties.samplePeriod,
									m_itemStore.get(),
									m_archiveDataStore.get(),
									this,
									m_arrowToolIndex,
									m_leftChartMargin);
				break;
				
			default:            m_graph = GraphTypeFactory::getInstance().createLineGraph(
									m_chartCtrl, 
									trendProperties.name,
									trendProperties.description,
									startDateTime,
									endTime,
									trendProperties.live,
									trendProperties.samplePeriod,
									m_itemStore.get(),
									m_archiveDataStore.get(),
									this);
				
				LOG_GENERIC(SourceInfo, DebugUtil::DebugError, 
					"An invalid graph type was specified. Setting to Line type.");
            }
			
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Setting up graph...");
			
            m_graph->setUpGraph();
            m_graph->adjustXAxisFormat();
            m_graph->adjustXAxisTicks();
			
            // Specific XY settings need to be set before adding items to graph
            setXYSettings(trendProperties.graphType != m_currentTrendProperties.graphType);
			
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Adding items to graph...");
			
            // Add the items to the graph
            addItems(trendProperties.items);
			
            setLiveMode(trendProperties.live, true); //TD17935
			
            m_currentTrendProperties = trendProperties;
            m_currentTrendProperties.startDateTime = startDateTime;
            m_currentTrendProperties.endDateTime = endTime;
			
            // Specific Comparison settings need to be set after trendProperties has been assigned to m_currentTrendProperties
            setComparisonSettings(trendProperties.graphType == COMPARISON);
			
            // Set the new times in the time display
            setStartEndTime(m_currentTrendProperties.startDateTime, endTime);
			
            m_resetGraph = false;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
			
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << trendProperties.name.c_str();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_120028);
        }
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // setResizingProperties
    //
    void CHistoryViewerDlg::setTransActiveResizingProperties()
    {
        FUNCTION_ENTRY("setResizingProperties");
		
        TA_Base_Bus::ResizingProperties properties;
        properties.canMaximise = true;
        properties.maxHeight = -1;
        properties.maxWidth = -1;
        properties.minHeight = 672;
        properties.minWidth = 850;
        setResizingProperties(properties);
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // teeChart6IsInstalled
    //
    bool CHistoryViewerDlg::teeChart6IsInstalled()
    {
        FUNCTION_ENTRY("teeChart6IsInstalled");
        
        HKEY key;
        LONG rc = ::RegOpenKeyEx(HKEY_CLASSES_ROOT, REG_SUBKEY_STRING, 0, KEY_QUERY_VALUE, &key);
        if( ERROR_SUCCESS == rc )
        {
            ::RegCloseKey(key);
        }
		
        FUNCTION_EXIT;
        return(ERROR_SUCCESS==rc);
    }
	
	
    // 
    // setUpDisplayArea
    //
    void CHistoryViewerDlg::setUpDisplayArea()
    {
        FUNCTION_ENTRY("setUpDisplayArea");
		
        //
        // Disable all items in display area
        //
        std::map<int, int>::iterator it1 = m_itemNameResourceIds.begin();
        for(; it1 != m_itemNameResourceIds.end(); ++it1)
        {
            GetDlgItem(it1->second)->ShowWindow(SW_HIDE);
        }
		
        std::map<int, int>::iterator it2 = m_itemRangeResourceIds.begin();
        for(; it2 != m_itemRangeResourceIds.end(); ++it2)
        {
            GetDlgItem(it2->second)->ShowWindow(SW_HIDE);
        }
		
        std::map<int, int>::iterator it3 = m_itemValueResourceIds.begin();
        for(; it3 != m_itemValueResourceIds.end(); ++it3)
        {
			GetDlgItem(it3->second)->SetWindowText("");  //TD17940
            GetDlgItem(it3->second)->ShowWindow(SW_HIDE);
        }
		
        GetDlgItem(IDC_OFFSET_TIME)->ShowWindow(SW_HIDE);
        GetDlgItem(IDC_CURSOR_TIME)->ShowWindow(SW_HIDE);
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // setAccessControlledButtons
    //
    void CHistoryViewerDlg::setAccessControlledButtons()
    {
        FUNCTION_ENTRY("setAccessControlledButtons");
		
		//      boost::scoped_ptr<TA_Base_Bus::RightsLibrary> rl(NULL);
        bool allowExport = false;
        bool allowPrint = false;
		
        try
        {
			//          TA_Base_Bus::RightsLibraryFactory rlFactory;
			//          rl.reset(rlFactory.buildRightsLibrary());
            TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
            
            std::auto_ptr<TA_Base_Core::IEntityData> iData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(RunParams::getInstance().get(RPARAM_ENTITYNAME).c_str()));
            unsigned long anEntityKey = iData->getKey();
            
            std::auto_ptr<TA_Base_Core::IResource> r(TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( anEntityKey ));
            unsigned long resourceID = r->getKey();
            
            std::string reason;
            
            allowExport = m_rightsLibrary->isActionPermittedOnResource(
                m_sessionId, 
                resourceID, 
                TA_Base_Bus::aca_TREND_EXPORT, 
                reason, 
                decisionModule);
			
            allowPrint = m_rightsLibrary->isActionPermittedOnResource(
                m_sessionId, 
                resourceID, 
                TA_Base_Bus::aca_TREND_PRINT, 
                reason, 
                decisionModule); 
        }
        catch(TA_Base_Core::RightsException& ex)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "RightsException", ex.what());
			
            std::string msg("Privileges for Print and Export cannot be determined: ");
            msg += ex.what();
			
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, msg.c_str());
			
            allowExport = false;
            allowPrint = false;
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
			
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Privileges for Print and Export cannot be determined.");
			
            allowExport = false;
            allowPrint = false;
        }
        
        // Set Buttons
        m_toolBar.GetToolBarCtrl().EnableButton(ID_EXPORT, allowExport);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_PRINT_GRAPH, allowPrint);
		
        // Set Menu Items
        enableMenuItem(FILE_MENU_NAME, ID_EXPORT, allowExport);
        enableMenuItem(FILE_MENU_NAME, ID_PRINT_GRAPH, allowPrint);
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // mapDisplayAreaResources
    //
    void CHistoryViewerDlg::mapDisplayAreaResources()
    {
        FUNCTION_ENTRY("mapDisplayAreaResources");
		
        m_itemNameResourceIds.clear();
        m_itemNameResourceIds[1] = IDC_STATIC1;
        m_itemNameResourceIds[2] = IDC_STATIC2;
        m_itemNameResourceIds[3] = IDC_STATIC3;
        m_itemNameResourceIds[4] = IDC_STATIC4;
        m_itemNameResourceIds[5] = IDC_STATIC5;
        m_itemNameResourceIds[6] = IDC_STATIC6;
        m_itemNameResourceIds[7] = IDC_STATIC7;
        m_itemNameResourceIds[8] = IDC_STATIC8;
		
        m_itemRangeResourceIds.clear();
        m_itemRangeResourceIds[1] = IDC_RANGE1;
        m_itemRangeResourceIds[2] = IDC_RANGE2;
        m_itemRangeResourceIds[3] = IDC_RANGE3;
        m_itemRangeResourceIds[4] = IDC_RANGE4;
        m_itemRangeResourceIds[5] = IDC_RANGE5;
        m_itemRangeResourceIds[6] = IDC_RANGE6;
        m_itemRangeResourceIds[7] = IDC_RANGE7;
        m_itemRangeResourceIds[8] = IDC_RANGE8;
		
        m_itemValueResourceIds.clear();
        m_itemValueResourceIds[1] = IDC_VALUE1;
        m_itemValueResourceIds[2] = IDC_VALUE2;
        m_itemValueResourceIds[3] = IDC_VALUE3;
        m_itemValueResourceIds[4] = IDC_VALUE4;
        m_itemValueResourceIds[5] = IDC_VALUE5;
        m_itemValueResourceIds[6] = IDC_VALUE6;
        m_itemValueResourceIds[7] = IDC_VALUE7;
        m_itemValueResourceIds[8] = IDC_VALUE8;
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // reloadDisplayArea
    //
    void CHistoryViewerDlg::reloadDisplayArea()
    {
        FUNCTION_ENTRY("reloadDisplayArea");
		
        // Hide all items
        setUpDisplayArea();
		
        int count = 0;
        std::vector<DisplayItems>::iterator it = m_displayItems.begin();
        for(; it != m_displayItems.end(); ++it)
        {
            ++count;
            int nameResourceId = m_itemNameResourceIds[count];
            int rangeResourceId = m_itemRangeResourceIds[count];
			
            // Reset the display item index
            it->displayIndex = count;
            
            GetDlgItem(nameResourceId)->SetWindowText(it->name.c_str());
            GetDlgItem(rangeResourceId)->SetWindowText(it->range.c_str());
            
            // Resize the width of the place holders so the whole text is visible
            CDC* pDCName = GetDlgItem(nameResourceId)->GetDC();
            int nameWidth = pDCName->GetTextExtent(it->name.c_str()).cx + 1;  // Plus 1 to ensure the width is big enough
            CRect nameRect;
            GetDlgItem(nameResourceId)->GetWindowRect(nameRect);
            ScreenToClient(nameRect);
            GetDlgItem(nameResourceId)->MoveWindow(nameRect.left, nameRect.top, nameWidth, nameRect.Height());
            
            CDC* pDCRange = GetDlgItem(rangeResourceId)->GetDC();
            int rangeWidth = pDCRange->GetTextExtent(it->range.c_str()).cx; 
            CRect rangeRect;
            GetDlgItem(rangeResourceId)->GetWindowRect(rangeRect);
            ScreenToClient(rangeRect);
            GetDlgItem(rangeResourceId)->MoveWindow(rangeRect.left, rangeRect.top, rangeWidth, rangeRect.Height());
			
            // Redisplay the item
            GetDlgItem(nameResourceId)->ShowWindow(SW_SHOW);
            GetDlgItem(rangeResourceId)->ShowWindow(SW_SHOW);
        }
		
        // Send a "Size" message to resize the manager. Forcing a resize will 
        // resize the items in the display area so they fit properly.
        PostMessage(WM_SIZE, 0, 0);
		
        FUNCTION_EXIT;
    }
	
	
    // 
    // resetCursor
    //
    void CHistoryViewerDlg::resetCursor()
    {
        // Remove the old cursor
        m_chartCtrl.GetTools().Delete(m_cursorToolIndex);
		
        //
        // Add a new cursor tool to the chart to enable display of cursor. A new cursor
        // is added as opposed to using the old one because the cursor tool change event
        // is called when a new cursor is added. This will ensure that if the graph type
        // changes, the cursor values will be retrieved and displayed correctly.
        //
        m_cursorToolIndex = m_chartCtrl.GetTools().Add(tcCursor);
        m_chartCtrl.GetTools().GetItems(m_cursorToolIndex).SetActive(false);
        CCursorTool cursorTool = m_chartCtrl.GetTools().GetItems(m_cursorToolIndex).GetAsTeeCursor();
        cursorTool.SetStyle(cssVertical);
        cursorTool.SetFollowMouse(false);
        cursorTool.SetSnap(true);
        cursorTool.GetPen().SetColor(CURSOR_COLOUR);
		
        // If the cursor was on, turn it off
        if(m_displayCursor == true)
        {
            OnCursor();
        }
    }
	
	
    // 
    // resetZoom
    //
    void CHistoryViewerDlg::resetZoom()
    {
        if(m_chartCtrl.GetZoom().GetZoomed())
        {
            m_chartCtrl.GetZoom().Undo();
        }
    }
	
	
    // 
    // setLiveMode
    //
    void CHistoryViewerDlg::setLiveMode(bool isLive, bool onFirstLoad /*= false*/) //TD17935
    {
        m_graph->setLiveMode(isLive, m_currentTrendProperties.startDateTime, onFirstLoad); //TD17935
        m_nowCheck = isLive;
        m_toolBar.GetToolBarCtrl().CheckButton(ID_NOW, isLive);
        m_toolBar.setNowBitmap();
        m_currentTrendProperties.live = isLive;
		
        setMenuItemCheck(VIEW_MENU_NAME, ID_VIEW_NOW, m_nowCheck);
		
        // liborm 04/01/2005 - fix for TD#5050 - set status bar from here instead of in onNowCheck()
        if(m_nowCheck == true)
        {
            m_wndStatusBar.GetStatusBarCtrl().SetText("LIVE", m_wndStatusBar.CommandToIndex(ID_LIVE_MODE), 0);
        }
        else
        {
            m_wndStatusBar.GetStatusBarCtrl().SetText("", m_wndStatusBar.CommandToIndex(ID_LIVE_MODE), 0);
        }
        // TD#5050
    }
	
    
    // 
    // setOffsetDisplay
    //
    void CHistoryViewerDlg::setComparisonSettings(bool display)
    {
        if(display == true)
        {
            std::string displayString("Offset: ");
            displayString += m_currentTrendProperties.extraInfo;
			
            CDC* pDCOffset = GetDlgItem(IDC_OFFSET_TIME)->GetDC();
            int offsetWidth = pDCOffset->GetTextExtent(displayString.c_str()).cx; 
            CRect offsetRect;
            GetDlgItem(IDC_OFFSET_TIME)->GetWindowRect(offsetRect);
            ScreenToClient(offsetRect);
            GetDlgItem(IDC_OFFSET_TIME)->MoveWindow(offsetRect.left, offsetRect.top, offsetWidth, offsetRect.Height());
			
            GetDlgItem(IDC_OFFSET_TIME)->SetWindowText(displayString.c_str());
            GetDlgItem(IDC_OFFSET_TIME)->ShowWindow(SW_SHOW);
        }
        else
        {
            GetDlgItem(IDC_OFFSET_TIME)->ShowWindow(SW_HIDE);
        }
    }
	
    
    // 
    // setXYSettings
    //
    void CHistoryViewerDlg::setXYSettings(bool graphTypeChanged)
    {
        // If the previous graph was an XY and the new one is not, then reset the zoom here because
        // the axis types are different and the graph gets confused when trying to reset the X-axis 
        // back to a date/time format when it is no longer in that format.
        if(m_zoomed == true && graphTypeChanged == true)
        {
            resetZoom();
        }
    }
	
	
    // 
    // setSpecialSettings
    //
    void CHistoryViewerDlg::setSpecialSettings(EGraphType graphType)
    {
        switch(graphType)
        {
		case XY:            setXYSettings(graphType != m_currentTrendProperties.graphType);
			break;
			
		case COMPARISON:    setComparisonSettings(true);
			break;
			
		default:            return;
        }
    }
	
	
    // 
    // addItems
    //
    void CHistoryViewerDlg::addItems(const std::map<std::string, TA_Base_Core::ItemInfo>& items)
    {
        FUNCTION_ENTRY("addItems");
		
        // Setting up a new group of items. Clear the last ones.
        m_displayItems.clear();
		
        // Need to add the items in order of ascending indexes so they will
        // be displayed correctly in the display area
		
        std::map<std::string, TA_Base_Core::ItemInfo> trendItems = items;
		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Waiting for load to complete...");
		
        // Wait if loading hasn't completed
        while(!m_loadingComplete)
        {
			
            if(m_useCmdLineParams || m_preloadPropertyItems)
            {
                // Using command line parameters which means the Trend Viewer was launched from
                // another application. Here, we try to speed up the display of the Trend Viewer
                // by loading the added items first.
                std::vector<std::string> itemNames;
                std::map<std::string, TA_Base_Core::ItemInfo>::const_iterator iter = items.begin();
				//TD17529++
                //for(; iter != items.end(); ++iter)
				for (int itemCtr=0; iter != items.end(); ++iter, ++itemCtr)
                {
					//TD17529++
					TA_Base_Core::ItemInfo itemInfo = iter->second;
					//TD17884
					LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Current Item[%d]: %s",itemInfo.index, itemInfo.displayName.c_str());
					if (itemInfo.index <= m_maxTrendsDisplayed)					
						itemNames.push_back(iter->first);
					//++TD17529
					
                }
				
                try
                {
                    m_itemStore->loadItems(itemNames);
                }
                catch(const TA_Base_Core::TrendViewerException& ex)
                {
                    std::string action("display all datapoints");
					
                    TA_Base_Bus::TransActiveMessage userMsg;
                    userMsg << action << ex.what();
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_120023);
                }
				
                // Remove the items that had problems while loading
                trendItems.clear();
                std::vector<std::string>::iterator it = itemNames.begin();
                for(; it != itemNames.end(); ++it)
                {
                    std::map<std::string, TA_Base_Core::ItemInfo>::const_iterator it2 = items.find(*it);
                    if(it2 != items.end())
                    {
                        trendItems[it2->first] = it2->second;
                    }
                }
				
                break;
            }
			
            const int SLEEP_IN_MILLISECS = 100;
            TA_Base_Core::Thread::sleep(SLEEP_IN_MILLISECS);
        }
		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "addItems - Loading Complete.");
		
        m_singleItemDisplayed = false;
        int itemCount = 0;
        for(int i = 0; i < MAX_ITEMS_DEFAULT; ++i)
        {
            std::map<std::string, ItemInfo>::iterator it = trendItems.begin();
			
            for(; it != trendItems.end(); ++it)
            {
                int index = it->second.index;
				
				//TD17529++
                //if(index == i + 1) 
				if((index == i + 1) && (index <= m_maxTrendsDisplayed))
					//++TD17529
                {
                    std::string itemName = it->first;
                    std::string displayName = it->second.displayName;
					
                    // Ignore if the item is "<empty>"
                    if(itemName == EMPTY_STRING)
                    {
                        trendItems.erase(it);
                        break;
                    }
					
                    COLORREF colour = Utilities::convertHexStringToColorRef(it->second.colour);
					
                    try
                    {
                        m_graph->addToGraph(index, itemName, colour, displayName);
                        ++itemCount;
                    }
                    catch(TA_Base_Core::TrendViewerException& ex)
                    {
                        LOG_EXCEPTION_CATCH(SourceInfo, "TrendViewerException", ex.what());
						
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << itemName.c_str() << ex.what();
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120032);
                    }
                    catch(...)
                    {
                        const CString REASON("Unknown");
                        LOG_EXCEPTION_CATCH(SourceInfo, LPCSTR(REASON), LPCSTR(REASON));
						
                        TA_Base_Bus::TransActiveMessage userMsg;
                        userMsg << itemName.c_str() << REASON;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_120032);
                    }
					
                    trendItems.erase(it);
                    break;
                }
            }                
        }
		
        LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Items added to graph");
		
        // TD 8624: Display engineering axis if there is only one item in the graph.
        if(itemCount == 1)
        {
            // Only the first parameter is used, so the others are just set at arbitrary values
            OnClickSeries(1, 0, 0, 0, 0, 0); 
            m_singleItemDisplayed = true;
        }
		
        FUNCTION_EXIT;
    }
	
	
    void CHistoryViewerDlg::initialiseMenuItems()
    {
        enableMenuItem(FILE_MENU_NAME, ID_NEW, false);          // Don't enable this until items have been loaded
        enableMenuItem(FILE_MENU_NAME, ID_TRENDS, false);       // Don't enable this until items have been loaded
        enableMenuItem(FILE_MENU_NAME, ID_EXPORT, true);
        enableMenuItem(FILE_MENU_NAME, ID_PRINT_GRAPH, true);
		
        enableMenuItem(VIEW_MENU_NAME, ID_REFRESH, true);
        enableMenuItem(VIEW_MENU_NAME, ID_CURSOR, true);
        enableMenuItem(VIEW_MENU_NAME, ID_PROPERTIES, false);   // Don't enable this until items have been loaded
        
        setMenuItemCheck(VIEW_MENU_NAME, ID_VIEW_NOW, m_nowCheck);
        setMenuItemCheck(VIEW_MENU_NAME, ID_CURSOR, m_displayCursor);
		
    }
	
    //
    // checkArchivedDataNeeded
    // liborm 06/01/2005 - fix for TD#7900 - when needed, displays a message informing 
    // the user to restore archived data for the specified date when dragging the graph.
    //
    // this method is copied from TrendPropertiesDialog, TODO: put this method
    // into a common class such as Utilities.cpp, make it return true/false
    // and seperate GUI code. A problem with this is that the ArchiveDataStore
    // object is needed.
    //
    void CHistoryViewerDlg::checkArchivedDataNeeded(time_t startTime, long rangeSeconds)
    {
        FUNCTION_ENTRY("checkArchivedDataNeeded");
		
        CTime nowTime = CTime::GetCurrentTime();
		
        // WP 4619M50121007 TD 9735        
        // If the 'archive data required' message box has been shown in the last
        // 2 seconds, don't show it again. This stops the message box constantly
        // coming up when dragging the graph with the right mouse button. This
        // happens because dragging causes the OnScrollGraph event handler to be
        // constantly envoked as the graph is dragged.
		if( ( GetTickCount() - m_lastDisplayTime ) < 2000 )
		{
            FUNCTION_EXIT;
			return;	
		}
		
        time_t earliestDate = nowTime.GetTime() - ((long) m_archiveDataStore->getDatabaseStorageDays() + 1) * SECONDS_IN_DAY;
        
        if(startTime < earliestDate)
        {
            time_t endTime = startTime + rangeSeconds;
            if(!m_archiveDataStore->isAllDataRestored(startTime, endTime))
            {
                // Need to restore archived data before all points can be displayed
                std::string unrestoredDates = m_archiveDataStore->getUnrestoredDates(startTime, endTime);
				
                std::string warning("Archive data needs to be restored before all points can be displayed. The following dates need to be restored: ");
                warning += unrestoredDates;
				
                // TD14164 azenitha++
				/*AfxMessageBox(warning.c_str());*/
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << unrestoredDates.c_str();
				UINT selectedButton = userMsg.showMsgBox(IDS_UE_120034);
				// TD14164 ++azenitha
            }
        }
        m_lastDisplayTime = GetTickCount();
		
        FUNCTION_EXIT;
    }
	
} // TA_App


#endif // AFX_HISTORYVIEWERDLG_CPP__D0BB3C09_825C_11D6_B243_0050BAB25370__INCLUDED_




