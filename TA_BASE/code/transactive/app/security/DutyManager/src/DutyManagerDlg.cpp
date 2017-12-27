/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/DutyManager/src/DutyManagerDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class represents the main Duty Manager dialog window.
  *
  */

#include "StdAfx.h"
#include "app/security/DutyManager/src/DutyManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const int DUTY_MGR_MINIMUM_WIDTH(620);
    const int DUTY_MGR_MINIMUM_HEIGHT(405);
	const int DUTY_MGR_INITIALIZE_TIMER = 1;
}

using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionKey;
using TA_Base_Bus::SessionId;
using TA_Base_Bus::SessionUpdate;
using TA_Base_Bus::ProfileKey;
using TA_Base_Bus::ProfileKeyList;
using TA_Base_Bus::TransActiveDialog;

using TA_Base_Core::DebugUtil;
using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// DutyManagerDlg dialog

bool DutyManagerDlg::m_isMartixDataInited = false;

extern bool g_populated;
// jinhua++
UINT  DutyManagerDlg::initialiseSubsystemDutyGridThreadFun( LPVOID pParam  )
{	

	DutyManagerDlg* pDlg = reinterpret_cast <DutyManagerDlg*>( pParam );
	ASSERT( pDlg != NULL );
    AfxGetApp()->DoWaitCursor(1);

	// Initialise the contents of the duty data store. Tell it to ignore all 
	// information relating to system operators and non-physical subsystems,
	// as we do not want to display these.		 
	LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin populating duty data");
	TA_Base_Bus::DutyDataStore::populate(true, true,true); 

	// Make sure pDlg is not NULL, in case that user close DutyManager before call PostMessage.
	if ( pDlg != NULL)
	{
		pDlg->PostMessage(WM_POPULATE_DATA, 0,0);
	}

	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyManagerDlg::initialiseSubsystemDutyGridThreadFun");

	return 0;
}
// ++jinhua

LRESULT DutyManagerDlg::OnPopulateData(WPARAM wParam, LPARAM lParam)
{
	try
	{
		CString statusMsg;

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Populating duty data finished");
		this->m_bridge->initialiseCurrentSession();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin building region page");
		this->m_regionDutyPropertyPage->initialiseRegionDutyGrid();

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "building region page finished");

		statusMsg.LoadString(IDS_SUBSYSTEM_STATUS);
		this->SendDlgItemMessage(IDC_STATUS_STATIC,WM_SETTEXT,0,(LPARAM)( (LPCSTR) statusMsg));


		// Register for subsystem duty add and remove messages
		TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
			TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage, this, NULL);

		this->getSubsytemDutyPropertyPage().initialiseSubsystemDutyGrid();


		this->SendDlgItemMessage(IDC_STATUS_STATIC,WM_SHOWWINDOW,SW_HIDE , 0);

		this->positionControls(false);

		AfxGetApp()->DoWaitCursor(-1);

		g_populated = true;
		m_isMartixDataInited = true;
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "DutyManagerDlg::OnPopulateData Exception");
	}
	return 0;
}
// ++jinhua

DutyManagerDlg::DutyManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback) :
    TransActiveDialog(genericGUICallback, DutyManagerDlg::IDD), 
		m_bridge(NULL),
		//m_dutyPropertySheet(NULL),
		m_regionDutyPropertyPage(NULL),
		m_subsystemDutyPropertyPage(NULL),
		m_shownStatus(true)		
{
    FUNCTION_ENTRY("DutyManagerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback)");

	//{{AFX_DATA_INIT(DutyManagerDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT

	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_DUTYMANAGER_ICON);

    TA_Base_Bus::ResizingProperties properties;
    properties.canMaximise = true;
    properties.maxWidth = -1;
    properties.maxHeight = -1;
    properties.minWidth = DUTY_MGR_MINIMUM_WIDTH;
    properties.minHeight = DUTY_MGR_MINIMUM_HEIGHT;

    setResizingProperties(properties);

	// jinhua++
	// Register for subsystem duty add and remove messages
	//TA_Base_Core::MessageSubscriptionManager::getInstance().subscribeToBroadcastCommsMessage(
    //    TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage, this, NULL);
	// ++jinhua

    FUNCTION_EXIT;
}


DutyManagerDlg::~DutyManagerDlg()
{
    FUNCTION_ENTRY("~DutyManagerDlg()");

	TA_Base_Core::MessageSubscriptionManager::getInstance().unsubscribeToMessages(this);

	delete m_bridge;
	m_bridge  = NULL;
    
	m_subsystemDutyPropertyPage->DestroyWindow();
	delete m_subsystemDutyPropertyPage;
	m_subsystemDutyPropertyPage = NULL;

	m_regionDutyPropertyPage->DestroyWindow();
	delete m_regionDutyPropertyPage;
	m_regionDutyPropertyPage = NULL;

/*
		m_dutyPropertySheet.DestroyWindow();
		delete m_dutyPropertySheet;
		m_dutyPropertySheet = NULL;
		*/
	
	FUNCTION_EXIT;
}

void DutyManagerDlg::receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)
{
    FUNCTION_ENTRY("receiveSpecialisedMessage(const TA_Base_Core::CommsMessageCorbaDef& message)");

    // Determine the type of duty update based on message type.
	if( 0 == TA_Base_Core::DutyAgentBroadcastComms::DutyAgentRecoverMessage.getTypeKey().compare(message.messageTypeKey.in()) )
	{
		TA_Base_Bus::DutyAgentTypeCorbaDef::Key locationKey = 0;
		if((message.messageState >>= locationKey) != 0)
		{
			LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, 
						 "Received Duty Agent switch over message for location %d. Will clean the duty cache", 
						 locationKey );	
			PostMessage(WM_USER_RECEIVE_DUTY_AGENT_RECOVER_UPDATE, 0, locationKey);
		}
	}


    FUNCTION_EXIT;
}

LRESULT DutyManagerDlg::OnReceiveDutyAgentRecover(WPARAM wParam, LPARAM lParam)
{
	//std::stringstream messageInfo;
	TA_Base_Bus::TransActiveMessage userMsg;
	std::string prefixStr = "";
	std::string locationNameStr = "";

	try
	{
		TA_Base_Core::ILocation* locattion = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(lParam);
		locationNameStr = locattion->getName();
		//messageInfo << "the DutyAgent at location "<< locattion->getName();
		//messageInfo << " is just recover from broken down. If you have some operation related to that location duty. Please close the Duty manager ane reopen it.";
	}
	catch(...)
	{
		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "can't get location info for location key %d", lParam);
		prefixStr = "有一个";
		//messageInfo << " One of the DutyAgent is just recover from broken down. If you have some operation related to the duty. Please close the Duty manager ane reopen it.";
	}

	//MessageBox::info(messageInfo.str());
	userMsg<<prefixStr;
	userMsg<<locationNameStr;
	userMsg.showMsgBox(IDS_UW_670008,"权限管理器");
	return 0;
}


void DutyManagerDlg::init()
{
	USES_CONVERSION;

	TA_ASSERT(this->m_hWnd != NULL, "the windows is not created yet!");
	// TD11122: Store the original title text.
    CString title;
    GetWindowText( title );

	CT2A szTitle(title);

	std::string strTitle((LPSTR)szTitle);
	
	if (this->m_bridge == NULL)
	{
		m_bridge =  new DutyManagerBridge(this, strTitle);
	}


	m_regionDutyPropertyPage =  new RegionDutyPropertyPage;
	m_subsystemDutyPropertyPage = new SubsystemDutyPropertyPage;
	//m_regionDutyPropertyPage->Create(IDD_REGIONS_DLG);
	//m_subsystemDutyPropertyPage->Create(IDD_SUBSYSTEMS_DLG);
	// Initialise the tabs.
    m_dutyPropertySheet.AddPage(&(*m_regionDutyPropertyPage));
    m_dutyPropertySheet.AddPage(&(*m_subsystemDutyPropertyPage));
	//m_dutyPropertySheet = new DutyManagerPropertySheet;
	m_dutyPropertySheet.Create(this, WS_CHILD | WS_VISIBLE | WS_CLIPCHILDREN);
	
    // Finally, position all the controls.
    //TD17082++
    positionControls(true);
    // positionControls();
	SetTimer(DUTY_MGR_INITIALIZE_TIMER,0, NULL);	
    //++TD17082
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"[CL-21875] end DutyManagerDlg::init, after Settimer");
}

void DutyManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	TransActiveDialog::DoDataExchange(pDX);

	//{{AFX_DATA_MAP(DutyManagerDlg)
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// DutyManagerDlg message handlers

BEGIN_MESSAGE_MAP(DutyManagerDlg, TransActiveDialog)
	//{{AFX_MSG_MAP(DutyManagerDlg)
    ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_WM_SIZE()
	ON_WM_TIMER()
	ON_COMMAND(ID_APP_EXIT, OnAppExit)
	ON_COMMAND(ID_HELP_DUTY_MGR,OnTransActiveHelp)
	ON_WM_CLOSE()
	ON_MESSAGE(WM_USER_RECEIVE_DUTY_AGENT_RECOVER_UPDATE, OnReceiveDutyAgentRecover)
	ON_MESSAGE(WM_POPULATE_DATA, OnPopulateData)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL DutyManagerDlg::OnInitDialog()
{
    FUNCTION_ENTRY("OnInitDialog()");

	TransActiveDialog::OnInitDialog();

	// Set the icon for this dialog.  The framework does this automatically
	// when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

    // Initialise current session details.
    //TD17082 - must be performed after onInitdialog to improve launch performance
    //initialiseCurrentSession();
	CWaitCursor waitCursor;	 
	CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));
	//statusCtrl->SetWindowText("checking the subsystem status...");	
	statusCtrl->SetWindowText(_T("检查子系统状态..."));	

	CRect windowRect;
	GetWindowRect(windowRect);
	ScreenToClient(windowRect);
	
	CRect statusBarRect;
	statusCtrl->GetWindowRect(statusBarRect);
	ScreenToClient(statusBarRect);
	statusBarRect.left = windowRect.left + 5;
	statusBarRect.right = windowRect.right - 9;
	statusBarRect.bottom = windowRect.bottom - 5;
	statusBarRect.top = statusBarRect.bottom - 20;
	statusCtrl->MoveWindow(statusBarRect);

	statusCtrl->ShowWindow(SW_SHOW);
	waitCursor.Restore();

    FUNCTION_EXIT;	
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void DutyManagerDlg::OnTransActiveHelp()
{
	TA_Base_Bus::HelpLauncher::getInstance().displayHelp(true);
}
//TD17082
void DutyManagerDlg::OnTimer(UINT nIDEvent)
{
	USES_CONVERSION;
	TA_ASSERT(m_bridge != NULL, "the duty brige object is NULL");
	if (nIDEvent == DUTY_MGR_INITIALIZE_TIMER)
	{
		KillTimer(DUTY_MGR_INITIALIZE_TIMER);
		CWaitCursor waitCursor;	 
		CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));

		//CString statusMsg;
		//statusMsg.LoadString(IDS_LOCATION_STATUS);
		//CW2T locationStatusMsg(statusMsg);

		statusCtrl->SetWindowText(_T("载入本地权限 ..."));
		

        // Initialise the contents of the duty data store. Tell it to ignore all 
        // information relating to system operators and non-physical subsystems,
        // as we do not want to display these.		 
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin populating duty data");
        TA_Base_Bus::DutyDataStore::populate(true, true); 
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Populating duty data finished");
		m_bridge->initialiseCurrentSession();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Begin building region page");
		m_regionDutyPropertyPage->initialiseRegionDutyGrid();
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "building region page finished");

		//statusMsg.LoadString(IDS_SUBSYSTEM_STATUS);
		//CW2T subsystemStatusMsg(statusMsg);

		statusCtrl->SetWindowText(_T("载入子系统权限 ..."));

		// jinhua++
		AfxBeginThread( initialiseSubsystemDutyGridThreadFun, this );
        //m_subsystemDutyPropertyPage->initialiseSubsystemDutyGrid();
		// ++jinhua

		statusCtrl->ShowWindow(SW_HIDE);
		positionControls(false);

  		KillTimer(DUTY_MGR_INITIALIZE_TIMER);	
		waitCursor.Restore();
	} 
}




void DutyManagerDlg::OnAppAbout()
{
    TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.
void DutyManagerDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

        // TD11803: A call to SendMessage was made when the window did not exist, for now just make sure
        // it won't generate an assertion and log the fact so it can be debugged further.
        if ( ::IsWindow(m_hWnd) )
        {
            SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);
        }
        else
        {
            LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Attempted to SendMessage when window does not exist." );
        }

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


// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR DutyManagerDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}


void DutyManagerDlg::OnSize(UINT nType, int cx, int cy) 
{
	TransActiveDialog::OnSize(nType, cx, cy);
	
    positionControls();
}


//TD17082
void DutyManagerDlg::positionControls(bool bStatusBarShown)
{
	if (::IsWindow(m_dutyPropertySheet.GetSafeHwnd()))
    {
        CRect windowRect;
        GetWindowRect(windowRect);
        ScreenToClient(windowRect);

        CRect propSheetRect;
        m_dutyPropertySheet.GetWindowRect(propSheetRect);
        ScreenToClient(propSheetRect);
        propSheetRect.left = windowRect.left + 3;
        //propSheetRect.top = windowRect.top + 35;
		 propSheetRect.top = windowRect.top + 45;
        propSheetRect.right = windowRect.right - 9;

        //TD17082 - check if status bar should be displayed or not
		if (bStatusBarShown)
		{	
			CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));
			CRect statusBarRect;
			statusCtrl->GetWindowRect(statusBarRect);
			ScreenToClient(statusBarRect);
			statusBarRect.left = windowRect.left + 5;
			statusBarRect.right = windowRect.right - 9;
			statusBarRect.bottom = windowRect.bottom - 5;
			statusBarRect.top = statusBarRect.bottom - 20;
			statusCtrl->MoveWindow(statusBarRect);
			propSheetRect.bottom = statusBarRect.top - 2;
		}
		else		
		{
			CStatic* statusCtrl = reinterpret_cast<CStatic*>(GetDlgItem(IDC_STATUS_STATIC));
			CRect statusBarRect;
			statusCtrl->GetWindowRect(statusBarRect);
			ScreenToClient(statusBarRect);

			statusBarRect.left = windowRect.left -10;
			statusBarRect.top = windowRect.top - 10;

			statusBarRect.right = statusBarRect.left + 5;
			statusBarRect.bottom = statusBarRect.top + 5;
			
			statusCtrl->MoveWindow(statusBarRect);

			propSheetRect.bottom = windowRect.bottom - 5;
		}
        //++TD17082

        m_dutyPropertySheet.MoveWindow(propSheetRect);  // The prop sheet will size the pages
    }
}

void DutyManagerDlg::OnAppExit() 
{
	delete m_bridge;
	m_bridge  = NULL;
	DestroyWindow();
}

void DutyManagerDlg::OnClose() 
{
	delete m_bridge;
	m_bridge  = NULL;
	DestroyWindow();	
}


// jinhua++
SubsystemDutyPropertyPage& DutyManagerDlg::getSubsytemDutyPropertyPage()
{
	FUNCTION_ENTRY( "SubsystemDutyPropertyPage" );

	FUNCTION_EXIT;
	return *m_subsystemDutyPropertyPage;
}
// ++jinhua

