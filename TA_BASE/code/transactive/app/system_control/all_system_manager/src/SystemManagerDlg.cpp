/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemManagerDlg.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * SystemManagerDlg is the class associated with the application's main window.
  * This class inherits from AdminObserver. The main window registers itself 
  * as an observer with the AdminManager.
  *
  */

#pragma warning(disable:4786)

#include <afxpriv.h>   //For AfxLoadString
#include <algorithm>
#include <string>

#include "StdAfx.h"
#include "SystemManager.h"
#include "SystemManagerDlg.h"
#include "AdminManager.h"
#include "StopProcessDlg.h"
#include "ShutdownSystemControllerDlg.h"
#include "ProcessParamsDlg.h"
#include "ChangeOperationModeDlg.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/types/OperatorAudit_MessageTypes.h"
#include "core/message/src/AuditMessageSender.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/SystemControllerException.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    const std::string SystemManagerDlg::RUNNING_CONTROL = "Running Control";
    const std::string SystemManagerDlg::RUNNING_MONITOR = "Running Monitor";
    const std::string SystemManagerDlg::GOING_TO_CONTROL = "Changing to Control";
    const std::string SystemManagerDlg::GOING_TO_MONITOR = "Changing to Monitor";
    const std::string SystemManagerDlg::STARTUP = "Startup";
    const std::string SystemManagerDlg::STOPPED = "Stopped";
    const std::string SystemManagerDlg::NOT_RUNNING = "Not Running";
    const std::string SystemManagerDlg::TERMINATING = "Terminating";
    const std::string SystemManagerDlg::NOT_STARTED = "Not Started";

    const std::string SystemManagerDlg::RUNNING = "Running";

    const std::string SystemManagerDlg::NOT_APPLICABLE = "N/A";
    const std::string SystemManagerDlg::CONTROL = "Control";
    const std::string SystemManagerDlg::MONITOR = "Monitor";

    SystemManagerDlg::SystemManagerDlg(TA_Base_Bus::IGUIAccess* gui) : 
        TA_Base_Bus::TransActiveDialog(*gui, SystemManagerDlg::IDD, NULL), 
        m_tabHosts(),
        m_menuLine(),
        m_hIcon( AfxGetApp()->LoadIcon(IDR_MAINFRAME) ),
        m_gui(gui),
        m_userLoggedIn( false ),
        m_currentChildWindow( NULL ),
        m_sysControllerDialogBox( NULL ),
        m_rightsLibrary(),
        m_toolBar(),
        m_ulLoginEntityKey( 0 )
    {
        m_sysControllerDialogBox = new SystemControllerDlg(this);
        AdminManager::InitializeStaticMember( this );
    }

    void SystemManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(SystemManagerDlg)
	    DDX_Control(pDX, IDC_TAB_HOSTS, m_tabHosts);
	    DDX_Control(pDX, IDC_MENU_LINE, m_menuLine);
	    //}}AFX_DATA_MAP
    }

    BEGIN_MESSAGE_MAP(SystemManagerDlg, TransActiveDialog)
	    //{{AFX_MSG_MAP(SystemManagerDlg)
	    ON_WM_PAINT()
	    ON_WM_QUERYDRAGICON()
	    ON_BN_CLICKED(ID_FILE_LOGIN, OnLogin)
	    ON_BN_CLICKED(ID_FILE_LOGOUT, OnLogout)
	    ON_BN_CLICKED(ID_CONTROL_STARTPROCESS, OnStartProcess)
	    ON_BN_CLICKED(ID_CONTROL_SETPARAM, OnDebugLevel)
	    ON_BN_CLICKED(ID_CONTROL_CHANGEPROCESSOPERATIONMODE, OnChangeProcessMode)
	    ON_BN_CLICKED(ID_VIEW_OPTIONS, OnOptions)
	    ON_BN_CLICKED(ID_CONTROL_STOPPROCESS, OnStopProcess)
	    ON_BN_CLICKED(ID_CONTROL_SHUTDOWNSYSTEMCONTROLLER, OnShutdownMonitor)
	    ON_BN_CLICKED(ID_VIEW_REFRESH, OnRefresh)
	    ON_WM_CONTEXTMENU()
        ON_WM_SIZE()
        ON_WM_SIZING()
	    ON_BN_CLICKED(ID_HELP_ABOUT, OnAbout)
	    ON_COMMAND(ID_BUTTON_LOGIN, OnButtonLogin)
	    ON_COMMAND(ID_BUTTON_LOGOUT, OnButtonLogout)
	    ON_COMMAND(ID_BUTTON_START, OnButtonStartProcess)
	    ON_COMMAND(ID_BUTTON_STOP, OnButtonStopProcess)
        ON_COMMAND(ID_BUTTON_REFRESH, OnButtonRefresh)
		ON_COMMAND(ID_FILE_EXIT, OnClose)
		ON_WM_ACTIVATE()
        ON_MESSAGE( WM_SHUTDOWN_SYSTEM_CONTROLLER, onShutdownSystemController)
        ON_MESSAGE( WM_NEW_PROCESS_DATA, onNewProcessData)
        ON_MESSAGE( WM_TAB_SEL_CHANGED, onTabSelChanged)
        ON_MESSAGE( WM_PROCESS_LIST_SEL_CHANGED, onProcessListSelChanged)
        ON_MESSAGE( WM_TRACK_CONTEX_MENU, onListContexMenu)
        ON_MESSAGE( WM_LOGIN_DONE, onHostLoginDone)
	ON_COMMAND(ID_BUTTON_RETRYALL, OnButtonRetryall)
	//}}AFX_MSG_MAP
        ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
	    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    END_MESSAGE_MAP()
    
    BOOL SystemManagerDlg::OnInitDialog()
    {
		TA_Base_Bus::ResizingProperties properties;
		properties.canMaximise = true;
		properties.maxHeight = -1;
		properties.maxWidth = -1;
		properties.minHeight = 250;
		properties.minWidth = 500;
		setResizingProperties(properties);

		TransActiveDialog::OnInitDialog();

        // Set the icon for this dialog.  The framework does this automatically
        // when the application's main window is not a dialog
        SetIcon(m_hIcon, TRUE);         // Set big icon
        SetIcon(m_hIcon, FALSE);        // Set small icon

        m_toolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | 
             WS_VISIBLE | CBRS_TOP | CBRS_TOOLTIPS | 
             CBRS_FLYBY | CBRS_SIZE_DYNAMIC);
        m_toolBar.LoadToolBar(IDR_TOOLBAR1);

        // Position precisely the child windows
        RECT ClientRect;
        GetClientRect(&ClientRect);
        resizeChildWindows(ClientRect.right, ClientRect.bottom);
        initToolbar();

        // Draw the Toolbar
        RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);

        // Initialise menu and toolbar items
        // there is no user logged in yet, so setup the menu accordingly
        setLoggedInMode(false);
        setupLoginLogoutToolbarMenu(false);
        enableRetryAllButton( false );

        if ( NULL != m_sysControllerDialogBox )
        {
            m_tabHosts.SetFont( GetFont(), FALSE );
            m_tabHosts.initTabs( m_sysControllerDialogBox->getHostnames() );
        }

        SetDlgItemText( IDC_STATUS, "Please login first" );

        m_tabHosts.DeselectAll( FALSE );

        return TRUE;  // return TRUE  unless you set the focus to a control
    }

	void SystemManagerDlg::init()
	{
        TA_Base_Bus::RightsLibraryFactory rlFactory;
        m_rightsLibrary = std::auto_ptr<TA_Base_Bus::RightsLibrary>(rlFactory.buildRightsLibrary());
        AdminManager::setRightLibrary( m_rightsLibrary.get() );
	}
    // If you add a minimize button to your dialog, you will need the code below
    // to draw the icon.  For MFC applications using the document/view model,
    // this is automatically done for you by the framework.

    void SystemManagerDlg::OnPaint()
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
    // the minimized window.
    HCURSOR SystemManagerDlg::OnQueryDragIcon()
    {
        return (HCURSOR) m_hIcon;
    }

    void SystemManagerDlg::OnLogin()
    {
		if (m_sysControllerDialogBox == NULL)
		{
			m_sysControllerDialogBox = new SystemControllerDlg(this);
		}
        m_currentChildWindow = m_sysControllerDialogBox;
		
        int result = m_sysControllerDialogBox->DoModal();
        m_currentChildWindow = NULL;

        if (result == IDCANCEL)
        {
            return; // Not connected!
        }

        std::string sessionId = m_gui->getSessionId();
        std::string operatorName = "";

        // Get the operator's name
        try
        {
            if (sessionId != "")
            {
                TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::
                                            getInstance().getOperatorFromSession(sessionId, false);
                operatorName = op->getName();
                delete op;
            }
        }
        catch(...)
        {
            // Error occurred - authentication agent is possibly down.
            // Can continue, but without sending audit messages.
            sessionId = "";
            operatorName = "";
        }

        // Login
        int res = m_tabHosts.login( RunParams::getInstance().get(RPARAM_PORTNUMBER.c_str()), sessionId, operatorName);

        if (res == ERR_AUTH_SERVICE_ERROR)
        {
            CString csLoad = "";
            csLoad.LoadString( IDS_AUTHENTICATION_SERVICE_ERROR );
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << csLoad;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020046);
            return;
        }

        // Log an event.

        m_ulLoginEntityKey = m_tabHosts.getActiveAdminManager()->getHostEntityKey();
        const unsigned long entityKey = m_ulLoginEntityKey;
        
        // Only send an audit message if in a valid session.
        if (AdminManager::validSession())
        {
            TA_Base_Core::AuditMessageSender* auditSender;
            auditSender = 
                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::OperatorAudit::Context);
    
            TA_Base_Core::DescriptionParameters desc;
            TA_Base_Core::NameValuePair param1("hostname", 
                        RunParams::getInstance().get(RPARAM_HOSTNAME.c_str()));
            desc.push_back(&param1);
    
            auditSender->sendAuditMessage(
                TA_Base_Core::OperatorAudit::OperatorAdminLogIn, //Message Type
                entityKey, // Entity key
                desc,      // Description
                "",        // Additional details
                sessionId, // SessionID if applicable
                "",        // AlarmID if an alarm associated with event
                "0",       // IncidentKey if incident associated with event
                "");       // EventID of a parent event, used to link events

			delete auditSender;
        }

        // Register with Admin Manager as an observer
        registerObserver();

//         setLoggedInMode(true);
        setupLoginLogoutToolbarMenu( true );

        updateStatusMsg( res );
    }

    void SystemManagerDlg::OnLogout()
    {
        CString actionName;
        actionName.LoadString(IDS_LOGOUT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);
        if (selectedButton != IDYES)
        {
            return;
        }

        logout();
        SetWindowText("System Manager");
    }

    void SystemManagerDlg::setLoggedInMode(bool loggedIn)
    {
        setupMenu(loggedIn);
        setupToolbar(loggedIn); 

        if ( loggedIn )
        {
            CMenu* pMenu = GetMenu();
            pMenu->EnableMenuItem(ID_CONTROL_STARTPROCESS,MF_GRAYED);
            pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS,MF_GRAYED);
            pMenu->EnableMenuItem(ID_CONTROL_CHANGEPROCESSOPERATIONMODE,MF_GRAYED);
            pMenu->EnableMenuItem(ID_VIEW_OPTIONS, m_tabHosts.getActiveAdminManager()->isSetOptionsAllowed() ? MF_ENABLED : MF_GRAYED);

            // Register for the SessionId run param
            RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

            std::string hostName = m_tabHosts.getActiveAdminManager()->getHostName();
            std::string windowTitle("System Manager - ");
            windowTitle += hostName;
            SetWindowText(windowTitle.c_str());
        }
        else
        {
            m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START,false);
            m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP,false);

            SetWindowText( "System Manager" );
        }
    }

    void SystemManagerDlg::OnClose()
    {
        CString actionName;
        actionName.LoadString(IDS_EXIT);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);
        if (selectedButton != IDYES)
        {
            return;
        }
    
        logout();

		// Deregister for run param updates.
		RunParams::getInstance().deregisterRunParamUser(this);
		//TD18095, jianghp, to fix the performance of showing manager application
		TransActiveDialog::DestroyWindow();
    }

    void SystemManagerDlg::logout()
    {
        // Log an event.
        if ( !m_userLoggedIn )
        {
            return;
        }

        const unsigned long entityKey = m_ulLoginEntityKey;
        std::string sessionId = AdminManager::getSessionId();
        
        // Only send an audit message if in a valid session.
        if (AdminManager::validSession())
        {
            TA_Base_Core::AuditMessageSender* auditSender;
            auditSender = 
                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::OperatorAudit::Context );
    
            TA_Base_Core::DescriptionParameters desc;
            TA_Base_Core::NameValuePair param1("hostname", 
                        RunParams::getInstance().get(RPARAM_HOSTNAME.c_str()));
            desc.push_back(&param1);
   
            auditSender->sendAuditMessage(
                TA_Base_Core::OperatorAudit::OperatorAdminLogOut, // Message Type
                entityKey, // Entity key
                desc,        // Description
                "",          // Additional details
                sessionId,   // SessionID if applicable
                "",          // AlarmID if an alarm associated with event
                "",          // IncidentKey if incident associated with event
                "");         // EventID of a parent event, used to link events

			delete auditSender;
        }

        // Unregister with the AdminManager as an observer
        unregisterObserver();

	    //Ask the Admin Manager to terminate session
        m_tabHosts.logout();

        //Close the open child window
        if (m_currentChildWindow)
        {
            if (m_currentChildWindow->m_hWnd)
            {
                m_currentChildWindow->EndDialog(IDCANCEL);
            }
        }

        SetDlgItemText( IDC_STATUS, "Logged out" );

        setLoggedInMode(false);
        setupLoginLogoutToolbarMenu( false );
        enableRetryAllButton( false );

        m_tabHosts.clearProcessList();
    }

    void SystemManagerDlg::OnStartProcess()
    {
        std::vector<std::string> states;
        states.push_back(STOPPED);

        std::vector<std::string> nonRunningProcesses = getSelectedProcessesWithState(states);
        StartProcessDlg StartDlg(nonRunningProcesses,this);
        m_currentChildWindow = &StartDlg;
        StartDlg.DoModal();

        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::OnDebugLevel()
    {
        std::vector<std::string> states;
		states.push_back(STOPPED);
        states.push_back(RUNNING);
        states.push_back(RUNNING_CONTROL);
        states.push_back(RUNNING_MONITOR);
        states.push_back(GOING_TO_CONTROL);
        states.push_back(GOING_TO_MONITOR);
        // Ignore non-managed processes when selecting processes
        std::vector<std::string> runningProcesses = getSelectedProcessesWithState(states,false,true);
        ProcessParamsDlg ParamDlg(runningProcesses, this);
        m_currentChildWindow = &ParamDlg;
        ParamDlg.DoModal();

        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::OnChangeProcessMode()
    {
        std::vector<std::string> states;
        states.push_back(RUNNING_CONTROL);
        states.push_back(RUNNING_MONITOR);

        // When selecting processes ignore services and non-managed processes
        std::vector<std::string> runningProcesses = getSelectedProcessesWithState(states,true,true);
        ChangeOperationModeDlg changeModeDlg(runningProcesses,this);
        m_currentChildWindow = &changeModeDlg;
        changeModeDlg.DoModal();

        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::OnOptions()
    {
        OptionsDlg optionsDlg(this);
        m_currentChildWindow = &optionsDlg;
        optionsDlg.DoModal();
        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::onNewData( AdminManager* pAdminManager )
    {
        // Post a message so the SystemManagerDlg can retrieve the process
		// data in its thread.

		PostMessage(WM_NEW_PROCESS_DATA, reinterpret_cast<WPARAM>(pAdminManager), 0);
    }

	LRESULT SystemManagerDlg::onNewProcessData(WPARAM wparam, LPARAM lparam)
	{
        // Get new process list from the AdminManager and update display.
        AdminManager* pUpdatedAdminManager = reinterpret_cast<AdminManager*>(wparam);
        ProcessListCtrl* pTobeUpdateProcessList = NULL;
		try
        {
            m_tabHosts.highlightAdminBindTab( pUpdatedAdminManager, true );
            pTobeUpdateProcessList = m_tabHosts.getAdminBindProcessList( pUpdatedAdminManager );
            if ( NULL != pTobeUpdateProcessList )
            {
                pTobeUpdateProcessList->update(pUpdatedAdminManager->getProcesses());
            }
            if ( pUpdatedAdminManager == m_tabHosts.getActiveAdminManager() )
            {
                // Update the menu / tool bar icons to match the new process data.
	            onItemchangedMainProcesslist(NULL,NULL);
            }
		}
        catch(TA_Base_Core::SystemControllerException&)
        {
// 			unregisterObserver();

//             TA_Base_Bus::TransActiveMessage userMsg;
//             UINT selectedButton = userMsg.showMsgBox(IDS_UE_020022);

//             logout();

            if ( pUpdatedAdminManager == m_tabHosts.getActiveAdminManager() )
            {
                setLoggedInMode( false );
            }
            m_tabHosts.highlightAdminBindTab( pUpdatedAdminManager, false );
            if ( NULL != pTobeUpdateProcessList )
            {
                pTobeUpdateProcessList->DeleteAllItems();
            }
        }

        if ( pUpdatedAdminManager == m_tabHosts.getActiveAdminManager() )
        {
            int nLoginResult = pUpdatedAdminManager->getLoginResult();
            updateStatusMsg( nLoginResult );
        }

		return 0;
	}

    LRESULT SystemManagerDlg::onTabSelChanged(WPARAM wparam, LPARAM lparam)
    {
        AdminManager* pActiveAdminManager = reinterpret_cast<AdminManager*>(wparam);
        int nLoginResult = pActiveAdminManager->getLoginResult();
        bool bLoginSuccessful = ( ERR_AUTH_NO_ERROR == nLoginResult ) ? true : false;
        setLoggedInMode( bLoginSuccessful );
        if ( bLoginSuccessful )
        {
            onItemchangedMainProcesslist( NULL, NULL );
        }

        updateStatusMsg( nLoginResult );

        return 0;
    }

    LRESULT SystemManagerDlg::onListContexMenu(WPARAM wparam, LPARAM lparam)
    {
        CPoint newpoint = *(reinterpret_cast<CPoint*>(lparam));
        OnContextMenu( reinterpret_cast<CWnd*>(wparam), newpoint );
        return 0;
    }
    
    LRESULT SystemManagerDlg::onHostLoginDone(WPARAM wparam, LPARAM lparam)
    {
        AdminManager* pActiveAdminManager = reinterpret_cast<AdminManager*>(wparam);
        if ( pActiveAdminManager == m_tabHosts.getActiveAdminManager() )
        {
            onTabSelChanged( wparam, lparam );
        }

        int nLoginResult = pActiveAdminManager->getLoginResult();
        bool bLoginSuccessful = ( ERR_AUTH_NO_ERROR == nLoginResult ) ? true : false;
        m_tabHosts.highlightAdminBindTab( pActiveAdminManager, bLoginSuccessful );
        if ( !bLoginSuccessful )
        {
            enableRetryAllButton( true );
        }
        return 0;
    }
    
    LRESULT SystemManagerDlg::onProcessListSelChanged(WPARAM wparam, LPARAM lparam)
    {
        onItemchangedMainProcesslist( reinterpret_cast<NMHDR*>(wparam), reinterpret_cast<LRESULT*>(lparam) );
        return 0;
    }

    void SystemManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        if (name.compare(RPARAM_SESSIONID) == 0)
        {
			// yanrong: clear rights cache before check permission, force to call rights agent.
			m_rightsLibrary->resetCache();
            // SessionID has changed. Update permissions.
            checkPermissions();
        }
    }

    void SystemManagerDlg::OnStopProcess()
    {
        // Can stop processes with Running, Not Running, or Startup state.
        std::vector<std::string> states;
        states.push_back(RUNNING);
        states.push_back(RUNNING_CONTROL);
        states.push_back(RUNNING_MONITOR);
        states.push_back(GOING_TO_CONTROL);
        states.push_back(GOING_TO_MONITOR);
        states.push_back(STARTUP);
        states.push_back(NOT_RUNNING);
        states.push_back(NOT_STARTED);

        std::vector<std::string> stoppableProcesses = getSelectedProcessesWithState(states);

        StopProcessDlg stopDlg(stoppableProcesses, this);
        m_currentChildWindow = &stopDlg;
        stopDlg.DoModal();
        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::OnShutdownMonitor()
    {
        ShutdownSystemControllerDlg shutdownDlg(m_tabHosts.getActiveAdminManager()->getHostName(), this);
        m_currentChildWindow = &shutdownDlg;
        shutdownDlg.DoModal();
        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::OnRefresh()
    {
        try
        {
            m_tabHosts.getActiveAdminManager()->refreshData();
        }
        catch(TA_Base_Core::SystemControllerException&)
        {
// 			unregisterObserver();

            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020022);

// 			logout();
        }
    }

    void SystemManagerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        ProcessListCtrl* pActiveProcessList = m_tabHosts.getActiveProcessListCtrl();
        AdminManager* pAdminManager = m_tabHosts.getActiveAdminManager();
        if (( NULL == pActiveProcessList ) || ( NULL == pAdminManager ))
        {
            return;
        }
        if ((pWnd == pActiveProcessList) && (m_userLoggedIn))
        {
            POSITION pos = pActiveProcessList->GetFirstSelectedItemPosition();

			if (pos == NULL)
			{
				// No processes selected, so don't want to do anything.
				return;
			}

            CMenu ContextMenu;
            ContextMenu.LoadMenu(IDR_CONTEXT_MENU);

            // Initially disable all menu items.
            CMenu* pContextMenu = ContextMenu.GetSubMenu(0);
            pContextMenu->EnableMenuItem(0, MF_BYPOSITION | MF_GRAYED);
            pContextMenu->EnableMenuItem(1, MF_BYPOSITION | MF_GRAYED);
            pContextMenu->EnableMenuItem(2, MF_BYPOSITION | MF_GRAYED);   

            pActiveProcessList->freeze();
            std::string currStatus;

            while (pos != NULL)
            {
                int itemNum = pActiveProcessList->GetNextSelectedItem(pos);
                currStatus = pActiveProcessList->getStatus(itemNum);

                if( (currStatus.compare(RUNNING_CONTROL) == 0) ||
                    (currStatus.compare(RUNNING_MONITOR) == 0) ||
                    (currStatus.compare(GOING_TO_CONTROL) == 0) ||
                    (currStatus.compare(GOING_TO_MONITOR) == 0) ||
                    (currStatus.compare(RUNNING) == 0) )
                {
                    ContextMenu.EnableMenuItem(IDM_STOP_PROCESS, pAdminManager->isStopProcessAllowed() ? MF_ENABLED : MF_GRAYED);

                    std::string entityDescription = pActiveProcessList->GetItemText(itemNum,0);
                    bool isManagedProcess = pAdminManager->isManagedProcessFromDescription(entityDescription);

                    // Can only change process operation mode on processes that are managed and are not services
                    if( isManagedProcess )
                    {
                        ContextMenu.EnableMenuItem(IDM_PROCESS_PARAM,pAdminManager->isSetProcessParamAllowed() ? MF_ENABLED : MF_GRAYED);
                    }
                }
                else if( (currStatus.compare(STARTUP) == 0) ||
                         (currStatus.compare(NOT_STARTED) == 0) ||
                         (currStatus.compare(NOT_RUNNING) == 0) )
                {
                    ContextMenu.EnableMenuItem(IDM_STOP_PROCESS,
                        pAdminManager->isStopProcessAllowed() ? MF_ENABLED : MF_GRAYED);
                }
                else if (currStatus.compare(STOPPED) == 0)
                {
                    ContextMenu.EnableMenuItem(IDM_START_PROCESS,
                        pAdminManager->isStartProcessAllowed() ? MF_ENABLED : MF_GRAYED);
					ContextMenu.EnableMenuItem(IDM_PROCESS_PARAM,
						pAdminManager->isSetProcessParamAllowed() ? MF_ENABLED : MF_GRAYED);
                }
            }

            pActiveProcessList->unfreeze();

            int nCmd = (int) pContextMenu->TrackPopupMenu(
                TPM_LEFTALIGN | TPM_LEFTBUTTON | TPM_RETURNCMD,
                point.x, point.y,
                this);

            // Process the appropriate context menu command.
            switch (nCmd)
            {
            case IDM_START_PROCESS:
                OnStartProcess();
                break;
            case IDM_STOP_PROCESS:
                OnStopProcess();
                break;
            case IDM_PROCESS_PARAM:
                OnDebugLevel();
                break;
            default:
                break;
            }

            m_currentChildWindow = NULL;
        }
        else
        {
//             CDialog::OnContextMenu(pWnd, point);
        }
    }

    LRESULT SystemManagerDlg::onShutdownSystemController(WPARAM wparam, LPARAM lparam)
    {
        try
        {
            m_tabHosts.getActiveAdminManager()->shutdownSystemController();
        }
        catch(TA_Base_Core::SystemControllerException&)
        {
            // The System Controller will generally take longer than 20
            // seconds to shutdown - so do nothing if an exception was thrown.
        }

//         logout();
        return 0;
    }

    void SystemManagerDlg::OnSize(UINT nType, int cx, int cy)
    {
        CDialog::OnSize(nType, cx, cy);

		// Resize the line under the menus.
	    if(m_menuLine.m_hWnd != NULL)
	    {
			CRect menuLineRect;
			m_menuLine.GetWindowRect(menuLineRect);
			ScreenToClient(menuLineRect);
			m_menuLine.MoveWindow(0, 0, cx, menuLineRect.Height(), true);
		}

        resizeChildWindows(cx, cy);
    }

    void SystemManagerDlg::resizeChildWindows(int width, int height)
    {
        RECT  ListWinRect;
        if (m_tabHosts.m_hWnd)
        {
            m_tabHosts.GetWindowRect(&ListWinRect);

            ListWinRect.top    = 70;
            ListWinRect.left   = 2;
            ListWinRect.bottom = max(ListWinRect.top + height - 93, ListWinRect.top);
            ListWinRect.right  = max(ListWinRect.left + width - 3, ListWinRect.left);
            m_tabHosts.MoveWindow(&ListWinRect);
        }

        CWnd* pStatus = GetDlgItem(IDC_STATUS);
    
        if (pStatus)
        {
            if (pStatus->m_hWnd)
            {
                ListWinRect.top    = ListWinRect.bottom + 2;
                ListWinRect.bottom = ListWinRect.top + 20;
                pStatus->MoveWindow(&ListWinRect);
            }
        }
    }

    void SystemManagerDlg::OnSizing(UINT fwSide, LPRECT pRect)
    {
        pRect->bottom = max (pRect->bottom, pRect->top + 140);
        CDialog::OnSizing(fwSide, pRect);
    }

    void SystemManagerDlg::setupMenu(bool loggedIn)
    {
        CMenu* pMenu = GetMenu();
        if(loggedIn)
        {
            pMenu->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED);
		    pMenu->EnableMenuItem(2, MF_BYPOSITION | MF_ENABLED);

            pMenu->EnableMenuItem(ID_CONTROL_SHUTDOWNSYSTEMCONTROLLER,
                m_tabHosts.getActiveAdminManager()->isShutdownAllowed() ? MF_ENABLED : MF_GRAYED);        
            pMenu->EnableMenuItem(ID_CONTROL_SETPARAM,
                MF_GRAYED);
		    
            DrawMenuBar();
        }
        else
        {
            pMenu->EnableMenuItem(1, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
		    pMenu->EnableMenuItem(2, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);

            pMenu->EnableMenuItem(ID_CONTROL_SHUTDOWNSYSTEMCONTROLLER, MF_GRAYED);

		    DrawMenuBar();
        }
    }

    void SystemManagerDlg::setupToolbar(bool loggedIn)
    {
        // Login button should be enabled, if not logged in and vice versa
    
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_REFRESH,loggedIn);    
    }

    void SystemManagerDlg::initToolbar()
    {
        m_toolBar.GetToolBarCtrl().SetButtonSize(CSize(69,55));

        m_toolBar.SetButtonText(0,"Login");
        m_toolBar.SetButtonText(1,"Logout");
        m_toolBar.SetButtonText(2,"Start");
        m_toolBar.SetButtonText(3,"Stop");
        m_toolBar.SetButtonText(5,"Refresh");
        m_toolBar.SetButtonText(7,"RetryAll");
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START,false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP,false); 
    }

    // Handles TTN_NEEDTEXT message to display tooltips for the toolbar.
    // This code is based on CFrameWnd::OnToolTipText
    BOOL SystemManagerDlg::OnToolTipText(UINT, NMHDR* pNMHDR, LRESULT* pResult)
    {
	    ASSERT(pNMHDR->code == TTN_NEEDTEXTA || pNMHDR->code == TTN_NEEDTEXTW);
	    // allow top level routing frame to handle the message
	    if (GetRoutingFrame() != NULL)
		    return FALSE;
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
		    lstrcpyn(pTTTA->szText, strTipText,
			    (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	    else
		    _mbstowcsz(pTTTW->szText, strTipText,
			    (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
    #else
	    if (pNMHDR->code == TTN_NEEDTEXTA)
		    _wcstombsz(pTTTA->szText, strTipText,
			    (sizeof(pTTTA->szText)/sizeof(pTTTA->szText[0])));
	    else
		    lstrcpyn(pTTTW->szText, strTipText,
			    (sizeof(pTTTW->szText)/sizeof(pTTTW->szText[0])));
    #endif

	    *pResult = 0;

	    // Bring the tooltip window above other popup windows
	    ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
		    SWP_NOACTIVATE|SWP_NOSIZE|SWP_NOMOVE);
	    return TRUE;    // message was handled
    }

    void SystemManagerDlg::OnAbout()
    {
        TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();  
    }

    void SystemManagerDlg::OnButtonLogin() 
    {
	    OnLogin();	
    }
    void SystemManagerDlg::OnButtonLogout() 
    {
	    OnLogout();	
    }
    void SystemManagerDlg::OnButtonStartProcess() 
    {
	    OnStartProcess();	
    }

    void SystemManagerDlg::OnButtonStopProcess() 
    {
	    OnStopProcess();	
    }

    void SystemManagerDlg::OnButtonRefresh() 
    {
	    OnRefresh();
    }


    void SystemManagerDlg::onItemchangedMainProcesslist(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        if ( NULL != pNMListView )
        {
            if ( 0 == pNMListView->uNewState )
            {
                return;
            }
        }

        CMenu* pMenu = GetMenu();

        // If the operator clicks inside the list client area, but does not select
        // a process, turn off all related buttons/menu items.
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START, false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, false);
        pMenu->EnableMenuItem(ID_CONTROL_STARTPROCESS, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_CHANGEPROCESSOPERATIONMODE, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_SETPARAM, MF_GRAYED);

        ProcessListCtrl* pActiveProcessList = m_tabHosts.getActiveProcessListCtrl();
        AdminManager* pAdminManager = m_tabHosts.getActiveAdminManager();
        if (( NULL == pActiveProcessList ) || ( NULL == pAdminManager ))
        {
            return;
        }
        pActiveProcessList->freeze();
        std::string currStatus;
        POSITION pos = pActiveProcessList->GetFirstSelectedItemPosition();

        while (pos != NULL)
        {
            int itemNum = pActiveProcessList->GetNextSelectedItem(pos);
            currStatus = pActiveProcessList->getStatus(itemNum);

            if ( (currStatus.compare(RUNNING_CONTROL) == 0) ||
                 (currStatus.compare(RUNNING_MONITOR) == 0) ||
                 (currStatus.compare(GOING_TO_CONTROL) == 0) ||
                 (currStatus.compare(GOING_TO_MONITOR) == 0) ||
                 (currStatus.compare(RUNNING) == 0) )
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, pAdminManager->isStopProcessAllowed());

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, pAdminManager->isStopProcessAllowed() ? MF_ENABLED : MF_GRAYED);

                std::string entityDescription = pActiveProcessList->GetItemText(itemNum,0);
                bool isManagedProcess = m_tabHosts.getActiveAdminManager()->isManagedProcessFromDescription(entityDescription);
                bool isServiceProcess = m_tabHosts.getActiveAdminManager()->isServiceProcessFromDescription(entityDescription);
				bool isAutoFailBack = m_tabHosts.getActiveAdminManager()->isAutoFailBackFromDescription(entityDescription);

                // Can only set params on managed processes
                if( isManagedProcess )
                {
                    pMenu->EnableMenuItem(ID_CONTROL_SETPARAM, pAdminManager->isSetProcessParamAllowed() ? MF_ENABLED : MF_GRAYED);
                    
                    // Can only change process operation mode on processes that are managed and are not services
                    if (!(isServiceProcess||isAutoFailBack))
                    {
						// TD17104
						if ((currStatus.compare(RUNNING_CONTROL) == 0) ||
							(currStatus.compare(RUNNING_MONITOR) == 0) ||
							(currStatus.compare(RUNNING) == 0) )
						{
							pMenu->EnableMenuItem(ID_CONTROL_CHANGEPROCESSOPERATIONMODE, pAdminManager->isSetOperatingModeAllowed() ? MF_ENABLED : MF_GRAYED);
						}
                    }
                }
            }
            else if (currStatus.compare(STARTUP) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, pAdminManager->isStopProcessAllowed());

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, pAdminManager->isStopProcessAllowed() ? MF_ENABLED : MF_GRAYED);
            }
            else if (currStatus.compare(NOT_RUNNING) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, pAdminManager->isStopProcessAllowed());

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS,pAdminManager->isStopProcessAllowed() ? MF_ENABLED : MF_GRAYED);            
            }
            else if (currStatus.compare(NOT_STARTED) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, pAdminManager->isStopProcessAllowed());

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS,pAdminManager->isStopProcessAllowed() ? MF_ENABLED : MF_GRAYED);            
            }
            else if (currStatus.compare(STOPPED) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START, pAdminManager->isStartProcessAllowed());

                pMenu->EnableMenuItem(ID_CONTROL_STARTPROCESS, pAdminManager->isStartProcessAllowed() ? MF_ENABLED : MF_GRAYED);
            }
        }

        pActiveProcessList->unfreeze();
	    
		if (pResult != 0)
		{
			*pResult = 0;
		}
    }


    std::vector<std::string> SystemManagerDlg::getSelectedProcesses()
    {
        std::vector<std::string> processes;

        ProcessListCtrl* pActiveProcessList = m_tabHosts.getActiveProcessListCtrl();
        if ( NULL == pActiveProcessList )
        {
            return processes;
        }

        POSITION pos = pActiveProcessList->GetFirstSelectedItemPosition();
        while (pos != NULL)
        {
            int itemNum = pActiveProcessList->GetNextSelectedItem(pos);
            processes.push_back(pActiveProcessList->getProcessName(itemNum));
        }
        return processes;
    }

    std::vector<std::string> SystemManagerDlg::getSelectedProcessesWithState(const std::vector<std::string>& states,
                                                                             bool ignoreServices /*=false*/,
                                                                             bool ignoreNonManaged /*=false*/)
    {
        std::vector<std::string> processes;

        ProcessListCtrl* pActiveProcessList = m_tabHosts.getActiveProcessListCtrl();
        if ( NULL == pActiveProcessList )
        {
            return processes;
        }

        // Iterate through each selected process
        POSITION pos = pActiveProcessList->GetFirstSelectedItemPosition();
        while (pos != NULL)
        {
            int itemNum = pActiveProcessList->GetNextSelectedItem(pos);
            std::string processState = pActiveProcessList->getStatus(itemNum);

            std::string entityDescription = pActiveProcessList->GetItemText(itemNum,0);
            bool isManagedProcess = m_tabHosts.getActiveAdminManager()->isManagedProcessFromDescription(entityDescription);
            bool isServiceProcess = m_tabHosts.getActiveAdminManager()->isServiceProcessFromDescription(entityDescription);
			bool isAutoFailBack = m_tabHosts.getActiveAdminManager()->isAutoFailBackFromDescription(entityDescription);
            if ((ignoreServices && isServiceProcess)||(ignoreServices && isAutoFailBack))
            {
                // Ignore. No need to check states
                continue;
            }
            else if (ignoreNonManaged && !isManagedProcess)
            {
                //Ignore. No need to check states
                continue;
            }
			
            // Now check if the state matches any of those passed in
            std::vector<std::string>::const_iterator statesToCheckFor = states.begin();
            while(statesToCheckFor != states.end())
            {
                if (processState == *statesToCheckFor)
                {
                    processes.push_back(pActiveProcessList->getProcessName(itemNum));
                    break;
                }
                ++statesToCheckFor;
            }
        }
        return processes;
    }

    void SystemManagerDlg::checkPermissions()
    {
		FUNCTION_ENTRY("checkPermissions");

        m_tabHosts.checkPermissions();

        // yanrong: privilege maybe changed, so tell main dialog to update display (e.g enable or not start/stop button).
        onNewData( m_tabHosts.getActiveAdminManager() );

		FUNCTION_EXIT;
    }
	
    void SystemManagerDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized) 
    {
	    TransActiveDialog::OnActivate(nState, pWndOther, bMinimized);
	    
        if( (bMinimized == FALSE) && 
            ( (nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE) )) 
        {
            // This is when the window is restored (i.e no longer minimised)
            // Position precisely the child windows
            RECT ClientRect;
            GetClientRect(&ClientRect);
            resizeChildWindows(ClientRect.right, ClientRect.bottom);

            // Draw the Toolbar
            RepositionBars(AFX_IDW_CONTROLBAR_FIRST, AFX_IDW_CONTROLBAR_LAST, 0);
        }
    }
    
    AdminManager* SystemManagerDlg::getActiveAdminManagerPtr()
    {
        return m_tabHosts.getActiveAdminManager();
    }

    void SystemManagerDlg::updateStatusMsg( int nLoginResult )
    {
        unsigned int errorId;
        switch (nLoginResult)
        {
        case ERR_AUTH_NO_ERROR:
            break;
        case ERR_AUTH_SOCKET_ERROR:
            errorId = IDS_UNIDENTIFIED_CONSOLE;
            break;
        case ERR_AUTH_SERVICE_ERROR:
            errorId = IDS_AUTHENTICATION_SERVICE_ERROR;
            break;
        case ERR_GET_ADMIN_ERROR:
            errorId = IDS_CANNOT_ACCESS_SYSTEM_CONTROLLER;
            break;
        case ERR_NO_SYSTEM_CONTROLLER_ERROR:
            errorId = IDS_NO_SYSTEM_CONTROLLER;
            break;
        case ERR_RETRIEVING_PROCESS_DATA:
            errorId = IDS_CANNOT_RETRIEVE_INFO;
            break;
        default:
            errorId = IDS_LOGIN;
            break;
        }

        std::string strMsg = "";
        if ( ERR_AUTH_NO_ERROR < nLoginResult )
        {
            CString action, error;
            action.LoadString(IDS_LOGIN);
            error.LoadString(errorId);
            strMsg = action.GetBuffer( action.GetLength() );
            strMsg += " failed";
            strMsg += error.GetBuffer(error.GetLength() );
            action.ReleaseBuffer();
            error.ReleaseBuffer();
            //To Display the Port Number
            if(errorId == IDS_NO_SYSTEM_CONTROLLER)
            {
                std::string portNumber = RunParams::getInstance().get(RPARAM_PORTNUMBER.c_str());
                strMsg += ( " " + portNumber );
            }
        }
        else if ( ERR_LOGIN_NO_PROCESS == nLoginResult )
        {
            strMsg = "Login to "  + m_tabHosts.getActiveAdminManager()->getHostName() + " is not done";
        }
        else
        {
            strMsg = "Operator " + AdminManager::getOperatorName() + " logined";;
        }

        SetDlgItemText( IDC_STATUS, strMsg.c_str() );
    }

    void SystemManagerDlg::setupLoginLogoutToolbarMenu( bool bLogined )
    {
        m_tabHosts.EnableWindow( bLogined ? TRUE : FALSE );
        m_tabHosts.ShowWindow( bLogined ? SW_SHOW : SW_HIDE );
        m_userLoggedIn = bLogined;
        CMenu* pMenu = GetMenu();
        pMenu->EnableMenuItem(ID_FILE_LOGIN, bLogined ? MF_GRAYED : MF_ENABLED);
        pMenu->EnableMenuItem(ID_FILE_LOGOUT,bLogined ? MF_ENABLED : MF_GRAYED);
        DrawMenuBar();

        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_LOGIN,!bLogined);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_LOGOUT,bLogined);

        if ( !bLogined )
        {
            m_tabHosts.unHighlightAllTabs();
        }
    }

    void SystemManagerDlg::OnButtonRetryall() 
    {
        enableRetryAllButton( false );
        m_tabHosts.retryLoginAllFailed();
    }

    void SystemManagerDlg::enableRetryAllButton( bool bEnable /*= true */ )
    {
        m_toolBar.GetToolBarCtrl().EnableButton( ID_BUTTON_RETRYALL, bEnable );
    }
} // namespace TA_Base_App
