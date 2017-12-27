/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemManagerDlg.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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
#include "DataCache.h"
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
#include "bus/security/access_control/actions/src/AccessControlledActions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/SystemControllerException.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/threads/src/ThreadPoolSingletonManager.h"
#include <boost/bind.hpp>

using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    //localization cuilike++
    //const std::string SystemManagerDlg::RUNNING_CONTROL = "Running Control";
    //const std::string SystemManagerDlg::RUNNING_MONITOR = "Running Monitor";
    //const std::string SystemManagerDlg::GOING_TO_CONTROL = "Changing to Control";
    //const std::string SystemManagerDlg::GOING_TO_MONITOR = "Changing to Monitor";
    //const std::string SystemManagerDlg::STARTUP = "Startup";
    //const std::string SystemManagerDlg::STOPPED = "Stopped";
    //const std::string SystemManagerDlg::NOT_RUNNING = "Not Running";
    //const std::string SystemManagerDlg::TERMINATING = "Terminating";
    //const std::string SystemManagerDlg::NOT_STARTED = "Not Started";

    //const std::string SystemManagerDlg::RUNNING = "Running";

    //const std::string SystemManagerDlg::NOT_APPLICABLE = "N/A";
    //const std::string SystemManagerDlg::CONTROL = "Control";
    //const std::string SystemManagerDlg::MONITOR = "Monitor";
    const std::string SystemManagerDlg::RUNNING_CONTROL = _T("控制状态");
    const std::string SystemManagerDlg::RUNNING_MONITOR = _T("监视状态");
    const std::string SystemManagerDlg::GOING_TO_CONTROL = _T("正在转为控制");
    const std::string SystemManagerDlg::GOING_TO_MONITOR = _T("正在转为监视");
    const std::string SystemManagerDlg::STARTUP = _T("启动");
    const std::string SystemManagerDlg::STOPPED = _T("停止");
    const std::string SystemManagerDlg::NOT_RUNNING = _T("未运行");
    const std::string SystemManagerDlg::TERMINATING = _T("正在停止");
    const std::string SystemManagerDlg::NOT_STARTED = _T("未启动");

    const std::string SystemManagerDlg::RUNNING = _T("运行中");

    const std::string SystemManagerDlg::NOT_APPLICABLE = "N/A";
    const std::string SystemManagerDlg::CONTROL = _T("控制");
    const std::string SystemManagerDlg::MONITOR = _T("监视");
    //localization ++cuilike

    SystemManagerDlg::SystemManagerDlg(TA_Base_Bus::IGUIAccess* gui)
        : TA_Base_Bus::TransActiveDialog(*gui, SystemManagerDlg::IDD, NULL), m_gui(gui)
    {
        //{{AFX_DATA_INIT(SystemManagerDlg)
        m_statusString = _T("");
        //}}AFX_DATA_INIT
        // Note that LoadIcon does not require a subsequent DestroyIcon in Win32
        m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
        m_currentChildWindow     = NULL;
        m_userLoggedIn           = false;
        m_sysControllerDialogBox = NULL;

        m_isStartProcessAllowed = false;
        m_isStopProcessAllowed = false;
        m_isSetProcessParamAllowed = false;
        m_isShutdownAllowed = false;
        m_isSetOperatingModeAllowed = false;
        m_isSetOptionsAllowed = false;
    }

    void SystemManagerDlg::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(SystemManagerDlg)
        DDX_Control(pDX, IDC_MENU_LINE, m_menuLine);
        DDX_Control(pDX, IDC_MAIN_PROCESSLIST, m_processList);
        DDX_Text(pDX, IDC_STATUS, m_statusString);
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
        ON_NOTIFY(LVN_ITEMCHANGED, IDC_MAIN_PROCESSLIST, onItemchangedMainProcesslist)
        ON_COMMAND(ID_FILE_EXIT, OnClose)
        ON_WM_ACTIVATE()
        ON_MESSAGE(WM_SHUTDOWN_SYSTEM_CONTROLLER, onShutdownSystemController)
        ON_MESSAGE(WM_NEW_PROCESS_DATA, onNewProcessData)
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

        MoveWindow(200, 200, 1200, 600);

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

        return TRUE;  // return TRUE  unless you set the focus to a control
    }

    void SystemManagerDlg::init()
    {
        //TA_Base_Bus::RightsLibraryFactory rlFactory;
        //m_rightsLibrary = boost::shared_ptr<TA_Base_Bus::RightsLibrary>(rlFactory.buildRightsLibrary());
        // m_rightsLibrary = boost::shared_ptr<TA_Base_Bus::RightsLibrary>(rlFactory.buildRightsLibrary(TA_Base_Bus::ALWAYS_TRUE_RL)); // Use for testing

        if (m_sysControllerDialogBox == NULL)
        {
            m_sysControllerDialogBox = new SystemControllerDlg(this);
        }
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
                TA_Base_Core::IOperatorPtr op(TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession(sessionId, false));
                operatorName = op->getName();
            }
        }
        catch (...)
        {
            // Error occurred - authentication agent is possibly down.
            // Can continue, but without sending audit messages.
            sessionId = "";
            operatorName = "";
        }

        // Login
        int res = AdminManager::getInstance().login(
                      RunParams::getInstance().get(RPARAM_HOSTNAME.c_str()),
                      RunParams::getInstance().get(RPARAM_PORTNUMBER.c_str()),
                      sessionId, operatorName);

        unsigned int errorId;

        switch (res)
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

        if (res != ERR_AUTH_NO_ERROR)
        {
            CString action, error, actionName;
            action.LoadString(IDS_LOGIN);
            error.LoadString(errorId);
            actionName = action + error;

            //To Display the Port Number
            if (errorId == IDS_NO_SYSTEM_CONTROLLER)
            {
                std::string portNumber = RunParams::getInstance().get(RPARAM_PORTNUMBER.c_str());
                CString portNumberString(portNumber.c_str());
                actionName += " " + portNumberString;
            }

            UINT selectedButton = showMsgBox(IDS_UE_020046, actionName);
            return;
        }

        // Log an event.

        const unsigned long entityKey = AdminManager::getInstance().getEntityKey();

        // Only send an audit message if in a valid session.
        if (AdminManager::getInstance().validSession())
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

        // Check the permissions for the currently logged in operator.
        //checkPermissions();
        checkPermissionsAsyn();

        // Register with Admin Manager as an observer
        registerObserver();

        setLoggedInMode(true);

        CMenu* pMenu = GetMenu();
        pMenu->EnableMenuItem(ID_CONTROL_STARTPROCESS, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_CHANGEPROCESSOPERATIONMODE, MF_GRAYED);
        pMenu->EnableMenuItem(ID_VIEW_OPTIONS, m_isSetOptionsAllowed ? MF_ENABLED : MF_GRAYED);

        // Register for the SessionId run param
        RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

        std::string hostName = AdminManager::getInstance().getHostName();
        std::string windowTitle = "System Manager - " + hostName;
        this->SetWindowText(windowTitle.c_str());

        //localization. cuilike++
        //m_statusString = "User ";
        //m_statusString += operatorName.c_str();
        //m_statusString += " logged in to [";
        //m_statusString += hostName.c_str();
        //m_statusString += "]";
        m_statusString = _T("用户 ");
        m_statusString += operatorName.c_str();
        m_statusString += _T("登录到[");
        m_statusString += hostName.c_str();
        m_statusString += _T("]");

        //localization. cuilike++
        UpdateData(FALSE);
    }

    void SystemManagerDlg::OnLogout()
    {
        CString actionName;
        actionName.LoadString(IDS_LOGOUT);
        UINT selectedButton = showMsgBox(IDS_UW_010003, actionName);

        if (selectedButton != IDYES)
        {
            return;
        }

        m_userLoggedIn = false;
        logout();
        this->SetWindowText("System Manager");
    }

    void SystemManagerDlg::setLoggedInMode(bool loggedIn)
    {
        m_userLoggedIn = loggedIn;

        setupMenu(loggedIn);
        setupToolbar(loggedIn);
    }

    void SystemManagerDlg::OnClose()
    {
        CString actionName;
        actionName.LoadString(IDS_EXIT);
        UINT selectedButton = showMsgBox(IDS_UW_010003, actionName);

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

        const unsigned long entityKey = AdminManager::getInstance().getEntityKey();
        std::string sessionId = AdminManager::getInstance().getSessionId();

        // Only send an audit message if in a valid session.
        if (AdminManager::getInstance().validSession())
        {
            TA_Base_Core::AuditMessageSender* auditSender;
            auditSender =
                TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender(TA_Base_Core::OperatorAudit::Context);

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
        AdminManager::getInstance().logout();

        //Close the open child window
        if (m_currentChildWindow)
        {
            if (m_currentChildWindow->m_hWnd)
            {
                m_currentChildWindow->EndDialog(IDCANCEL);
            }
        }

        m_statusString = "Logged out";
        UpdateData(FALSE);
        setLoggedInMode(false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START, false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, false);
        m_processList.DeleteAllItems();
    }

    void SystemManagerDlg::OnStartProcess()
    {
        std::vector<std::string> states;
        states.push_back(STOPPED);

        std::vector<std::string> nonRunningProcesses = getSelectedProcessesWithState(states);
        StartProcessDlg StartDlg(nonRunningProcesses, this);
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
        std::vector<std::string> runningProcesses = getSelectedProcessesWithState(states, false, true);
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
        std::vector<std::string> runningProcesses = getSelectedProcessesWithState(states, true, true);
        ChangeOperationModeDlg changeModeDlg(runningProcesses, this);
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

    void SystemManagerDlg::onNewData()
    {
        // Post a message so the SystemManagerDlg can retrieve the process
        // data in its thread.

        PostMessage(WM_NEW_PROCESS_DATA, 0, 0);
    }

    LRESULT SystemManagerDlg::onNewProcessData(WPARAM wparam, LPARAM lparam)
    {
        // Get new process list from the AdminManager and update display.
        try
        {
            m_processList.update(AdminManager::getInstance().getProcesses());

            // Update the menu / tool bar icons to match the new process data.
            onItemchangedMainProcesslist(NULL, NULL);
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            unregisterObserver();

            UINT selectedButton = showMsgBox(IDS_UE_020022);

            logout();
        }

        return 0;
    }

    void SystemManagerDlg::onRunParamChange(const std::string& name, const std::string& value)
    {
        if (name.compare(RPARAM_SESSIONID) == 0)
        {
            // yanrong: clear rights cache before check permission, force to call rights agent.
            m_rightsLibrary = DataCache::getInstance().getRightsLibrary();
            if (m_rightsLibrary)
            {
                m_rightsLibrary->resetCache();
            }

            // SessionID has changed. Update permissions.
            //checkPermissions();
            checkPermissionsAsyn();

            PostMessage(WM_NEW_PROCESS_DATA, 0, 0);
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
        ShutdownSystemControllerDlg shutdownDlg(AdminManager::getInstance().getHostName(), this);
        m_currentChildWindow = &shutdownDlg;
        shutdownDlg.DoModal();
        m_currentChildWindow = NULL;
    }

    void SystemManagerDlg::OnRefresh()
    {
        try
        {
            AdminManager::getInstance().refreshData();
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            unregisterObserver();

            UINT selectedButton = showMsgBox(IDS_UE_020022);

            logout();
        }
    }

    void SystemManagerDlg::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        if ((pWnd == &m_processList) && (m_userLoggedIn))
        {
            POSITION pos = m_processList.GetFirstSelectedItemPosition();

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

            m_processList.freeze();
            std::string currStatus;

            while (pos != NULL)
            {
                int itemNum = m_processList.GetNextSelectedItem(pos);
                currStatus = m_processList.getStatus(itemNum);

                if ((currStatus.compare(RUNNING_CONTROL) == 0) ||
                        (currStatus.compare(RUNNING_MONITOR) == 0) ||
                        (currStatus.compare(GOING_TO_CONTROL) == 0) ||
                        (currStatus.compare(GOING_TO_MONITOR) == 0) ||
                        (currStatus.compare(RUNNING) == 0))
                {
                    ContextMenu.EnableMenuItem(IDM_STOP_PROCESS, m_isStopProcessAllowed ? MF_ENABLED : MF_GRAYED);

                    std::string entityDescription = m_processList.GetItemText(itemNum, 0);
                    bool isManagedProcess = AdminManager::getInstance().isManagedProcessFromDescription(entityDescription);

                    // Can only change process operation mode on processes that are managed and are not services
                    if (isManagedProcess)
                    {
                        ContextMenu.EnableMenuItem(IDM_PROCESS_PARAM, m_isSetProcessParamAllowed ? MF_ENABLED : MF_GRAYED);
                    }
                }
                else if ((currStatus.compare(STARTUP) == 0) ||
                         (currStatus.compare(NOT_STARTED) == 0) ||
                         (currStatus.compare(NOT_RUNNING) == 0))
                {
                    ContextMenu.EnableMenuItem(IDM_STOP_PROCESS,
                                               m_isStopProcessAllowed ? MF_ENABLED : MF_GRAYED);
                }
                else if (currStatus.compare(STOPPED) == 0)
                {
                    ContextMenu.EnableMenuItem(IDM_START_PROCESS,
                                               m_isStartProcessAllowed ? MF_ENABLED : MF_GRAYED);
                    ContextMenu.EnableMenuItem(IDM_PROCESS_PARAM,
                                               m_isSetProcessParamAllowed ? MF_ENABLED : MF_GRAYED);
                }
            }

            m_processList.unfreeze();

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
            CDialog::OnContextMenu(pWnd, point);
        }
    }

    LRESULT SystemManagerDlg::onShutdownSystemController(WPARAM wparam, LPARAM lparam)
    {
        try
        {
            AdminManager::getInstance().shutdownSystemController();
        }
        catch (TA_Base_Core::SystemControllerException&)
        {
            // The System Controller will generally take longer than 20
            // seconds to shutdown - so do nothing if an exception was thrown.
        }

        logout();
        return 0;
    }

    void SystemManagerDlg::OnSize(UINT nType, int cx, int cy)
    {
        CDialog::OnSize(nType, cx, cy);

        // Resize the line under the menus.
        if (m_menuLine.m_hWnd != NULL)
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

        if (m_processList.m_hWnd)
        {
            m_processList.GetWindowRect(&ListWinRect);

            ListWinRect.top    = 70;
            ListWinRect.left   = 2;
            ListWinRect.bottom = std::max(ListWinRect.top + height - 93, ListWinRect.top);
            ListWinRect.right  = std::max(ListWinRect.left + width - 3, ListWinRect.left);
            m_processList.MoveWindow(&ListWinRect);
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
        pRect->bottom = std::max(pRect->bottom, pRect->top + 140);
        CDialog::OnSizing(fwSide, pRect);
    }

    void SystemManagerDlg::setupMenu(bool loggedIn)
    {
        CMenu* pMenu = GetMenu();

        if (loggedIn)
        {
            pMenu->EnableMenuItem(ID_FILE_LOGIN, MF_GRAYED);
            pMenu->EnableMenuItem(ID_FILE_LOGOUT, MF_ENABLED);
            pMenu->EnableMenuItem(1, MF_BYPOSITION | MF_ENABLED);
            pMenu->EnableMenuItem(2, MF_BYPOSITION | MF_ENABLED);

            pMenu->EnableMenuItem(ID_CONTROL_SHUTDOWNSYSTEMCONTROLLER, m_isShutdownAllowed ? MF_ENABLED : MF_GRAYED);
            pMenu->EnableMenuItem(ID_CONTROL_SETPARAM, MF_GRAYED);

            this->DrawMenuBar();
        }
        else
        {
            pMenu->EnableMenuItem(ID_FILE_LOGIN, MF_ENABLED);
            pMenu->EnableMenuItem(ID_FILE_LOGOUT, MF_GRAYED);
            pMenu->EnableMenuItem(1, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);
            pMenu->EnableMenuItem(2, MF_BYPOSITION | MF_GRAYED | MF_DISABLED);

            pMenu->EnableMenuItem(ID_CONTROL_SHUTDOWNSYSTEMCONTROLLER, MF_GRAYED);

            this->DrawMenuBar();
        }
    }
    void SystemManagerDlg::setupToolbar(bool loggedIn)
    {
        // Login button should be enabled, if not logged in and vice versa
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_LOGIN, !loggedIn);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_LOGOUT, loggedIn);

        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_REFRESH, loggedIn);
    }

    void SystemManagerDlg::initToolbar()
    {
        m_toolBar.GetToolBarCtrl().SetButtonSize(CSize(69, 55));

        //m_toolBar.SetButtonText(0,"Login");
        //m_toolBar.SetButtonText(1,"Logout");
        //m_toolBar.SetButtonText(2,"Start");
        //m_toolBar.SetButtonText(3,"Stop");
        //m_toolBar.SetButtonText(4,"Refresh");
        m_toolBar.SetButtonText(0, _T("登录")); //localization cuilike++
        m_toolBar.SetButtonText(1, _T("注销")); //localization cuilike++
        m_toolBar.SetButtonText(2, _T("开始")); //localization cuilike++
        m_toolBar.SetButtonText(3, _T("停止")); //localization cuilike++
        m_toolBar.SetButtonText(4, _T("刷新")); //localization cuilike++
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START, false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, false);
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
                     (sizeof(pTTTA->szText) / sizeof(pTTTA->szText[0])));
        else
            _mbstowcsz(pTTTW->szText, strTipText,
                       (sizeof(pTTTW->szText) / sizeof(pTTTW->szText[0])));

#else

        if (pNMHDR->code == TTN_NEEDTEXTA)
            _wcstombsz(pTTTA->szText, strTipText,
                       (sizeof(pTTTA->szText) / sizeof(pTTTA->szText[0])));
        else
            lstrcpyn(pTTTW->szText, strTipText,
                     (sizeof(pTTTW->szText) / sizeof(pTTTW->szText[0])));

#endif

        *pResult = 0;

        // Bring the tooltip window above other popup windows
        ::SetWindowPos(pNMHDR->hwndFrom, HWND_TOP, 0, 0, 0, 0,
                       SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOMOVE);
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
        CMenu* pMenu = GetMenu();

        // If the operator clicks inside the list client area, but does not select
        // a process, turn off all related buttons/menu items.
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START, false);
        m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, false);
        pMenu->EnableMenuItem(ID_CONTROL_STARTPROCESS, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_CHANGEPROCESSOPERATIONMODE, MF_GRAYED);
        pMenu->EnableMenuItem(ID_CONTROL_SETPARAM, MF_GRAYED);

        m_processList.freeze();
        std::string currStatus;
        POSITION pos = m_processList.GetFirstSelectedItemPosition();

        while (pos != NULL)
        {
            int itemNum = m_processList.GetNextSelectedItem(pos);
            currStatus = m_processList.getStatus(itemNum);

            if ((currStatus.compare(RUNNING_CONTROL) == 0) ||
                    (currStatus.compare(RUNNING_MONITOR) == 0) ||
                    (currStatus.compare(GOING_TO_CONTROL) == 0) ||
                    (currStatus.compare(GOING_TO_MONITOR) == 0) ||
                    (currStatus.compare(RUNNING) == 0))
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, m_isStopProcessAllowed);

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, m_isStopProcessAllowed ? MF_ENABLED : MF_GRAYED);

                std::string entityDescription = m_processList.GetItemText(itemNum, 0);
                bool isManagedProcess = AdminManager::getInstance().isManagedProcessFromDescription(entityDescription);
                bool isServiceProcess = AdminManager::getInstance().isServiceProcessFromDescription(entityDescription);
                bool isAutoFailBack = AdminManager::getInstance().isAutoFailBackFromDescription(entityDescription);

                // Can only set params on managed processes
                if (isManagedProcess)
                {
                    pMenu->EnableMenuItem(ID_CONTROL_SETPARAM, m_isSetProcessParamAllowed ? MF_ENABLED : MF_GRAYED);

                    // Can only change process operation mode on processes that are managed and are not services
                    if (!(isServiceProcess || isAutoFailBack))
                    {
                        // TD17104
                        if ((currStatus.compare(RUNNING_CONTROL) == 0) ||
                                (currStatus.compare(RUNNING_MONITOR) == 0) ||
                                (currStatus.compare(RUNNING) == 0))
                        {
                            pMenu->EnableMenuItem(ID_CONTROL_CHANGEPROCESSOPERATIONMODE, m_isSetOperatingModeAllowed ? MF_ENABLED : MF_GRAYED);
                        }
                    }
                }
            }
            else if (currStatus.compare(STARTUP) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, m_isStopProcessAllowed);

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, m_isStopProcessAllowed ? MF_ENABLED : MF_GRAYED);
            }
            else if (currStatus.compare(NOT_RUNNING) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, m_isStopProcessAllowed);

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, m_isStopProcessAllowed ? MF_ENABLED : MF_GRAYED);
            }
            else if (currStatus.compare(NOT_STARTED) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_STOP, m_isStopProcessAllowed);

                pMenu->EnableMenuItem(ID_CONTROL_STOPPROCESS, m_isStopProcessAllowed ? MF_ENABLED : MF_GRAYED);
            }
            else if (currStatus.compare(STOPPED) == 0)
            {
                m_toolBar.GetToolBarCtrl().EnableButton(ID_BUTTON_START, m_isStartProcessAllowed);

                pMenu->EnableMenuItem(ID_CONTROL_STARTPROCESS, m_isStartProcessAllowed ? MF_ENABLED : MF_GRAYED);
                pMenu->EnableMenuItem(ID_CONTROL_SETPARAM, m_isSetProcessParamAllowed ? MF_ENABLED : MF_GRAYED);
            }
        }

        m_processList.unfreeze();

        if (pResult != 0)
        {
            *pResult = 0;
        }
    }


    std::vector<std::string> SystemManagerDlg::getSelectedProcesses()
    {
        std::vector<std::string> processes;
        POSITION pos = m_processList.GetFirstSelectedItemPosition();

        while (pos != NULL)
        {
            int itemNum = m_processList.GetNextSelectedItem(pos);
            processes.push_back(m_processList.getProcessName(itemNum));
        }

        return processes;
    }

    std::vector<std::string> SystemManagerDlg::getSelectedProcessesWithState(const std::vector<std::string>& states,
            bool ignoreServices /*=false*/,
            bool ignoreNonManaged /*=false*/)
    {
        std::vector<std::string> processes;

        // Iterate through each selected process
        POSITION pos = m_processList.GetFirstSelectedItemPosition();

        while (pos != NULL)
        {
            int itemNum = m_processList.GetNextSelectedItem(pos);
            std::string processState = m_processList.getStatus(itemNum);

            std::string entityDescription = m_processList.GetItemText(itemNum, 0);
            bool isManagedProcess = AdminManager::getInstance().isManagedProcessFromDescription(entityDescription);
            bool isServiceProcess = AdminManager::getInstance().isServiceProcessFromDescription(entityDescription);
            bool isAutoFailBack = AdminManager::getInstance().isAutoFailBackFromDescription(entityDescription);

            if ((ignoreServices && isServiceProcess) || (ignoreServices && isAutoFailBack))
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

            while (statesToCheckFor != states.end())
            {
                if (processState == *statesToCheckFor)
                {
                    processes.push_back(m_processList.getProcessName(itemNum));
                    break;
                }

                ++statesToCheckFor;
            }
        }

        return processes;
    }


    void SystemManagerDlg::checkPermissionsAsyn()
    {
        TA_Base_Core::IWorkItemPtr item(new TA_Base_Core::SimpleWorkItem(boost::bind(&SystemManagerDlg::checkPermissions, this)));
        TA_Base_Core::ThreadPoolSingletonManager::getInstance()->queueWorkItem(item);
    }


    void SystemManagerDlg::checkPermissions()
    {
        FUNCTION_ENTRY("checkPermissions");
        CString functionList;
        std::vector<CString> vect_FunctionList;
        //std::iterator<CString> iter_FunctionList;
        bool errorOccurred = false;

        // Get the entity key of the System Controller.
        unsigned long entityKey = 0;

        try
        {
            TA_Base_Core::IEntityDataPtr entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity(TA_Base_Core::RunParams::getInstance().get(RPARAM_HOSTNAME.c_str())));
            entityKey = entityData->getKey();
        }
        catch (TA_Base_Core::DataException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException",
                "Errors occurred while trying to retrieve System Controller's entity key.");
            errorOccurred = true;
        }
        catch (TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException",
                "Errors occurred while trying to retrieve System Controller's entity key.");
            errorOccurred = true;
        }

        // Get the resource key from the entity key
        unsigned long resourceKey = 0;

        try
        {
            TA_Base_Core::IResourcePtr resource(TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity(entityKey));
            resourceKey = resource->getKey();
        }
        catch (TA_Base_Core::DataException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException",
                "Errors occurred while trying to retrieve a resource from the database.");
            errorOccurred = true;
        }
        catch (TA_Base_Core::DatabaseException&)
        {
            LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException",
                "Errors occurred while trying to retrieve the resource from the database.");
            errorOccurred = true;
        }

        if (TA_Base_Core::RunParams::getInstance().isSet("AllPermissions"))
        {
            m_isStartProcessAllowed = true;
            m_isStopProcessAllowed = true;
            m_isSetProcessParamAllowed = true;
            m_isShutdownAllowed = true;
            m_isSetOperatingModeAllowed = true;
            m_isSetOptionsAllowed = true;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "checkPermissions - run param AllPermissions is set, set all permissions to true");
        }
        else
        {
            m_rightsLibrary = DataCache::getInstance().getRightsLibrary();

            if (!m_rightsLibrary)
            {
                LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", "Errors occurred while trying to initialize rights library.");
                errorOccurred = true;
            }

            if (!errorOccurred)
            {
                // Now check the permissions for each rights-controlled function.
                try
                {
                    TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;
                    std::string reason;
                    std::string sessionId = m_gui->getSessionId();

                    // check rights library
                    if (!m_rightsLibrary.get())
                    {
                        init();
                    }

                    try
                    {
                        m_isStartProcessAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, TA_Base_Bus::aca_START_PROCESS, reason, decisionModule);
                    }
                    catch (TA_Base_Core::RightsException&)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_START_PROCESS);
                        vect_FunctionList.push_back(functionList);
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_START_PROCESS);
                        vect_FunctionList.push_back(functionList);
                    }

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "checkPermissions - resource key=%lu, action=%s, permission=%d", resourceKey, "aca_START_PROCESS", m_isStartProcessAllowed);

                    try
                    {
                        m_isStopProcessAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, TA_Base_Bus::aca_STOP_PROCESS, reason, decisionModule);
                    }
                    catch (TA_Base_Core::RightsException&)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_STOP_PROCESS);
                        vect_FunctionList.push_back(functionList);
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_STOP_PROCESS);
                        vect_FunctionList.push_back(functionList);
                    }

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "checkPermissions - resource key=%lu, action=%s, permission=%d", resourceKey, "aca_STOP_PROCESS", m_isStopProcessAllowed);

                    try
                    {
                        m_isSetProcessParamAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, TA_Base_Bus::aca_SET_PROCESS_RUNTIME_PARAMETER, reason, decisionModule);
                    }
                    catch (TA_Base_Core::RightsException&)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_PROCESS_PARAM_ALLOWED);
                        vect_FunctionList.push_back(functionList);
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_PROCESS_PARAM_ALLOWED);
                        vect_FunctionList.push_back(functionList);
                    }

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "checkPermissions - resource key=%lu, action=%s, permission=%d", resourceKey, "aca_SET_PROCESS_RUNTIME_PARAMETER", m_isSetProcessParamAllowed);

                    try
                    {
                        m_isShutdownAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, TA_Base_Bus::aca_SHUTDOWN_SYSTEM_CONTROLLER, reason, decisionModule);
                    }
                    catch (TA_Base_Core::RightsException&)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_SHUT_DOWN);
                        vect_FunctionList.push_back(functionList);
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_SHUT_DOWN);
                        vect_FunctionList.push_back(functionList);
                    }

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "checkPermissions - resource key=%lu, action=%s, permission=%d", resourceKey, "aca_SHUTDOWN_SYSTEM_CONTROLLER", m_isShutdownAllowed);

                    try
                    {
                        m_isSetOperatingModeAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, TA_Base_Bus::aca_SET_PROCESS_OPERATING_MODE, reason, decisionModule);
                    }
                    catch (TA_Base_Core::RightsException&)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_SET_OPERATING_MODE);
                        vect_FunctionList.push_back(functionList);
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_SET_OPERATING_MODE);
                        vect_FunctionList.push_back(functionList);
                    }

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "checkPermissions - resource key=%lu, action=%s, permission=%d", resourceKey, "aca_SET_PROCESS_OPERATING_MODE", m_isSetOperatingModeAllowed);

                    try
                    {
                        m_isSetOptionsAllowed = m_rightsLibrary->isActionPermittedOnResource(sessionId, resourceKey, TA_Base_Bus::aca_SYSTEM_MANAGER_OPTIONS, reason, decisionModule);
                    }
                    catch (TA_Base_Core::RightsException&)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_SET_OPTIONS);
                        vect_FunctionList.push_back(functionList);
                    }
                    catch (...)
                    {
                        LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                            "Errors occurred while trying to determine which actions are permitted.");
                        errorOccurred = true;
                        functionList.LoadString(IDS_SET_OPTIONS);
                        vect_FunctionList.push_back(functionList);
                    }

                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                                "checkPermissions - resource key=%lu, action=%s, permission=%d", resourceKey, "aca_SYSTEM_MANAGER_OPTIONS", m_isSetOptionsAllowed);
                }
                catch (TA_Base_Core::RightsException&)
                {
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "RightsException",
                        "Errors occurred while trying to determine which actions are permitted.");
                    errorOccurred = true;
                }
                catch (...)
                {
                    LOG(SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "Unknown",
                        "Errors occurred while trying to determine which actions are permitted.");
                    errorOccurred = true;
                }
            }
        }

        // If something has gone wrong, let the user know and disable everything.
        if (errorOccurred)
        {
            functionList.Empty();
            //functionList.LoadString(IDS_ALL_FUNCTIONALITY);
            TA_Base_Bus::TransActiveMessage userMsg;

            for (std::vector<CString>::iterator iter = vect_FunctionList.begin();
                    iter != vect_FunctionList.end(); ++iter)
            {
                functionList.Insert(functionList.GetLength(), "\r\n");
                functionList.Insert(functionList.GetLength(), *iter);
            }

            userMsg << functionList.GetBufferSetLength(functionList.GetLength());
            UINT selectedButton = userMsg.showMsgBox(IDS_UI_020002);

            m_isStartProcessAllowed = true;
            m_isStopProcessAllowed = true;
            m_isSetProcessParamAllowed = true;
            m_isShutdownAllowed = true;
            m_isSetOperatingModeAllowed = true;
            m_isSetOptionsAllowed = true;
        }

        // yanrong: privilege maybe changed, so tell main dialog to update display (e.g enable or not start/stop button).
        onNewData();

        FUNCTION_EXIT;
    }

    void SystemManagerDlg::OnActivate(UINT nState, CWnd* pWndOther, BOOL bMinimized)
    {
        TransActiveDialog::OnActivate(nState, pWndOther, bMinimized);

        if ((bMinimized == FALSE) &&
                ((nState == WA_ACTIVE) || (nState == WA_CLICKACTIVE)))
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
} // namespace TA_Base_App
