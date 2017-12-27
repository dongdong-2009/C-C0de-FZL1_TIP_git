/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/ControlStationRootDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is an extension of CDialog and is the root dialog and is responsible
  * for displaying the appropriate dialog boxes, messages and GraphWorX displays.
  */

#include "app/system_control/control_station/src/stdafx.h"
#include "app\system_control\control_station\src\ControlStationRootDialog.h"
#include "app\system_control\control_station\src\ControlStationConstants.h"
#include "app\system_control\control_station\src\EnumProcess.h"
#include "app\system_control\control_station\src\controlstation.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "app\system_control\control_station\src\ControlStation2.h"
#include "app\system_control\control_station\src\SchematicDisplay.h"
#include "app\system_control\control_station\src\TaskManagerDlg.h"
#include "app\system_control\control_station\src\DutyNotificationDialog.h"
#include "app\system_control\control_station\src\DutyRequestDialog.h"
#include "app/system_control/control_Station/src/ControlStationBar.h"
#include "app\system_control\control_station\src\IGraphWorxManager.h"
#include "app\system_control\control_station\src\WinkeysMgr.h"
#include "app\system_control\control_station\src\sinstall.h"

#include <sstream>
#include <algorithm>

#include "core/process_management/IDL/src/IManagedApplicationCorbaDef.h"
#include "bus\security\access_control\actions\src\AccessControlledActions.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus\generic_gui\src\AppLauncher.h"
#include "bus\application_types\src\apptypes.h"
#include "bus/sound/sound_client/src/SoundManagerProxy.h"
#include "core\exceptions\src\SessionException.h"
#include "core\exceptions\src\RightsException.h"
#include "core\exceptions\src\GraphWorxException.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\TA_String.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/SoundPlayerException.h"
#include "core/synchronisation/src/ReEntrantThreadLockable.h"
#include "core/threads/src/IWorkItem.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

ControlStationRootDialog* ControlStationRootDialog::s_instance = NULL;
TA_Base_Core::NonReEntrantThreadLockable ControlStationRootDialog::m_singletonLock;
TA_Base_Core::ReEntrantThreadLockable ControlStationRootDialog::m_initDlgLock;

const CString ControlStationRootDialog::MENU_RESUME_MESG = "MenuResume";
const CString ControlStationRootDialog::MENU_SUSPEND_MESG = "MenuSuspend";
const CString ControlStationRootDialog::SYSTEM_DEGRADED_MESG = "SystemDegraded";
const CString ControlStationRootDialog::SYSTEM_NORMAL_MESG = "SystemNormal";

const std::string ControlStationRootDialog::RPARAM_USE_SCREENSAVER = "UseScreenSaver";

using TA_Base_Core::DebugUtil;
using TA_Base_Core::GraphWorxException;
using TA_Base_Core::SessionException;
using TA_Base_Core::RightsException;
using TA_Base_Core::ThreadGuard;
using TA_Base_App::TA_ControlStation::APPLICATION_NAME;
using TA_Base_App::TA_ControlStation::EPermission;
using TA_Base_App::TA_ControlStation::MAX_PASSWORD_LENGTH;
using TA_Base_App::TA_ControlStation::MIN_PASSWORD_LENGTH;
using TA_Base_Bus::TransActiveMessage;

/////////////////////////////////////////////////////////////////////////////
// ControlStationRootDialog dialog

ControlStationRootDialog& ControlStationRootDialog::getInstance()
{
    // protect the instance check / creation
    ThreadGuard guard(m_singletonLock);

    if( s_instance == NULL )
    {
        s_instance = new ControlStationRootDialog();
    }
    return *s_instance;
}


ControlStationRootDialog::~ControlStationRootDialog()
{

	if( m_soundInterface != NULL )
	{
		delete m_soundInterface;
		m_soundInterface = NULL;
	}

    try
    {
        if (m_taskManager != NULL)
        {
            if (m_taskManager->m_hWnd != NULL)
            {
                m_taskManager->DestroyWindow();
            }
            else
            {
                delete m_taskManager;
                m_taskManager = NULL;
            }
        }

        if (m_launchBar != NULL)
        {
            if (m_launchBar->m_hWnd != NULL)
            {
                m_launchBar->DestroyWindow();
            }

            delete m_launchBar;
            m_launchBar = NULL;
        }
    }
    catch ( ... )
    {
        // Catch all exceptions so we don't get errors filtering out
        LOG( SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Caught in destructor of ControlStationRootDialog");
    }
	TA_Base_Core::ThreadPoolSingletonManager::removeInstance ( m_threadPool );
}


ControlStationRootDialog::ControlStationRootDialog()
    : m_taskManager(NULL), 
      m_comControlStation(NULL),
      m_comRippleControlStation(NULL),
      m_launchBar(NULL),
	  m_fHasInitDialog(false),
	  m_threadPool(NULL),
      CDialog(ControlStationRootDialog::IDD, NULL)
{
    //{{AFX_DATA_INIT(ControlStationRootDialog)
        // NOTE: the ClassWizard will add member initialization here
    //}}AFX_DATA_INIT
	m_threadPool = TA_Base_Core::ThreadPoolSingletonManager::getInstance();
}


void ControlStationRootDialog::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(ControlStationRootDialog)
        // NOTE: the ClassWizard will add DDX and DDV calls here
    //}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(ControlStationRootDialog, CDialog)
    //{{AFX_MSG_MAP(ControlStationRootDialog)
    ON_WM_WINDOWPOSCHANGING()
    ON_WM_CLOSE()
    //}}AFX_MSG_MAP
    ON_MESSAGE(WM_LAUNCH_DISPLAY,      onLaunchDisplay)
    ON_MESSAGE(WM_SERVER_STATE_CHANGE, onServerStateChange)
    ON_MESSAGE(WM_SYSTEMCONTROLLER_STATE_CHANGE, onSystemControllerStateChange)    
    ON_MESSAGE(WM_DUTY_CHANGE, onDutyChange)    
    ON_MESSAGE(WM_DUTY_REQUEST, onDutyRequest)    
    ON_MESSAGE(WM_DUTY_RESPONSE, onDutyResponse)    
    ON_MESSAGE(WM_SHOW_TASKMANAGER, onShowTaskManager)
    ON_MESSAGE(WM_HIDE_GRAPHWORX, onHideGraphWorxDisplays)
    ON_MESSAGE(WM_CREATE_DIALOGS, onCreateDialogs)
    ON_MESSAGE(WM_LOGIN_DETAILS_CHANGED, onLoginDetailsChanged)
    ON_MESSAGE(WM_DISPLAY_LOGIN_DIALOG, onDisplayLoginDialogue)
    ON_MESSAGE(WM_DISPLAY_OVERRIDE_DIALOG, onDisplayOperatorOverrideDialogue)
    ON_MESSAGE(WM_DISPLAY_CHANGE_PROFILE_DIALOG, onDisplayChangeProfileDialogue)
    ON_MESSAGE(WM_DISPLAY_CHANGE_OPERATOR_DIALOG, onDisplayChangeOperatorDialogue)
    ON_MESSAGE(WM_DISPLAY_CHANGE_PASSWORD_DIALOG, onDisplayChangePasswordDialogue)
    ON_MESSAGE(WM_LOGOUT, onLogout)
    ON_MESSAGE(WM_FORCE_LOGOUT, onForceLogout)
    ON_MESSAGE(WM_FORCE_END_OVERRIDE, onForceEndOverride)
    ON_MESSAGE(WM_CANCEL_CHILD, dereferenceDialog)
    ON_MESSAGE(WM_DISPLAY_LAUNCH_APPLICATION_ERROR, displayLaunchApplicationError)
    ON_MESSAGE(WM_TA_TO_GX_MESSAGE, onTAToGXMessage)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ControlStationRootDialog message handlers

BOOL ControlStationRootDialog::OnInitDialog() 
{
    CDialog::OnInitDialog();
	//TD17205,jianghp
	if (! SessionManager::getInstance().getDBUserPwdValid())
	{
		SessionManager::getInstance().cleanUp();
		::MessageBox(GetDesktopWindow()->m_hWnd, "please check the database username and password.", "Transactive", MB_OK|MB_ICONINFORMATION);
		exit(0);
	}
	//TD17205,jianghp
    // If we can't login automatically, then we'll need to display the login dialog
    if (!SessionManager::getInstance().canLoginAutomatically())
    {
        // Lets display the login dialog. The Sessionmanager will let us know when
        // it has finished initialising.
        PostMessage(WM_DISPLAY_LOGIN_DIALOG);
    }
    // Now kick off the rest of the SessionManager initialisation
    SessionManager::getInstance().start();

    // Make the Window always on top.
    SetWindowPos(&wndTopMost,0,0,0,0,SWP_NOSIZE|SWP_NOMOVE);

    ShowWindow(FALSE);

	// Call the sound player    
	try
	{
		m_soundInterface = new TA_Base_Bus::SoundManagerProxy();
	}
	catch(TA_Base_Core::SoundPlayerException& ex)
	{
		std::string error = "Unable to initialise SoundManager: ";
		error += ex.what();
		LOG_EXCEPTION_CATCH(SourceInfo, "SoundPlayerException", error.c_str());

        m_soundInterface = NULL;
	}

	TA_Base_Core::ThreadGuard guard( m_initDlgLock );

	m_fHasInitDialog = true;

    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


void ControlStationRootDialog::OnWindowPosChanging(WINDOWPOS FAR* lpwndpos) 
{
    // This will ensure that our dialogue is never ever shown on the screen
    // and always remains hidden.
    lpwndpos->flags &= ~SWP_SHOWWINDOW;
    CDialog::OnWindowPosChanging(lpwndpos);
}


void ControlStationRootDialog::initialisationComplete(bool wasSuccessful)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "initialisationComplete");

    // If we can't login automatically, then we'll need to display the login dialog
    if (SessionManager::getInstance().canLoginAutomatically())
    {
        if (!wasSuccessful)
        {
            TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_020070);
            exit();
        }
        return;
    }

    if (wasSuccessful)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugNorm, "Initialisation was successful");
        
        PostMessage(WM_CREATE_DIALOGS);

        // Now wait for the GraphWorX instances to be ready
        IGraphWorxManager* graphworxMgr = SessionManager::getInstance().getGraphWorxManager();
        if (graphworxMgr != NULL)
        {
            graphworxMgr->waitUntilReady();
        }

        m_loginDialog.activateWindow(); 
        m_loginDialog.SetForegroundWindow();
        m_loginDialog.BringWindowToTop();

        referenceDialog(m_passwordDialog);
        referenceDialog(m_loginDialog);

        LOG( SourceInfo, DebugUtil::FunctionExit, "initialisationComplete");
        return;
    }

    // Else we have a problem. Lets deal with it.
    LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,"Initialisation failed");

    // Make this non top most so that we could show the error message
    if (m_loginDialog.m_hWnd != NULL)
    {
        m_loginDialog.SetWindowPos(&CWnd::wndNoTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }

    TransActiveMessage userMsg;
    UINT selectedButton = userMsg.showMsgBox(IDS_UW_020010);

    // Make the login window top most again as operator has already dismissed message box
    if (m_loginDialog.m_hWnd != NULL)
    {
        m_loginDialog.SetWindowPos(&CWnd::wndTopMost, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
    }
    if (selectedButton == IDNO)
    {
        // Now we must exit
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "User does not want to retry initialisation so exiting");
        if (m_loginDialog.m_hWnd != NULL)
        {
            m_loginDialog.EndDialog(IDCANCEL);
        }
    }
    else
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Clean up and start initialisation again");
        // Clean up any mess left.
        SessionManager::cleanUp();
        // Kick off the SessionManager initialisation again.
        SessionManager::getInstance().start();
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "initialisationComplete");
}

void ControlStationRootDialog::fireDutyChanged()
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before ControlStationRootDialog::fireDutyChanged");
	this->m_comControlStation->Fire_dutyChanged();
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after ControlStationRootDialog::fireDutyChanged");
}
void ControlStationRootDialog::postDutyChanged()
{
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"before ControlStationRootDialog::postDutyChanged");
	TA_Base_Core::IWorkItemPtr workItem(new EventWorkItem(this));
	m_threadPool->queueWorkItem(workItem);
	LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"after ControlStationRootDialog::postDutyChanged");
}
/////////////////////////////////////////////////////////////////////////////
// Custom message handlers
/////////////////////////////////////////////////////////////////////////////


LRESULT ControlStationRootDialog::onDisplayLoginDialogue(WPARAM,LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDisplayLoginDialogue");

    //TD641 - for reason similar to TD653 see the comment in onLogout
    if (m_loginDialog.m_hWnd != NULL)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already have 1 instance of login dialog opened");
        return S_OK;
    }

    bool isSuccessfulLogin = false;
    m_loginDialog.setDialogType(TA_Base_App::TA_ControlStation::LOGIN_MODE);

    do
    {
        CString actionName;
        TransActiveMessage userMsg;
        try
        {
            // Call DoModal on the dialog box. This will display and wait for the run()
            // method to complete before allowing the user to do anything.
            int response = m_loginDialog.DoModal();

            if (response == IDCANCEL)
            {
                // If the control station couldn't start up normally, then just exit quietly
                if (m_loginDialog.getTerminateCode() == TA_Base_App::TA_ControlStation::SHUTDOWN)
                {
                    exit();
                    return S_OK;
                }

                BeginWaitCursor(); // display the hourglass cursor
                // see if we want to exit/restart so that we could do the appropriate actions and log messages
                bool isRestart = (m_loginDialog.getTerminateCode() == TA_Base_App::TA_ControlStation::RESTART);

                TA_VERIFY(actionName.LoadString(isRestart?IDS_RESTART:IDS_EXIT)!=0, "Unable to load IDS_EXIT or IDS_RESTART");
                EPermission permission = SessionManager::getInstance().isShutdownPermitted(m_loginDialog);
				//cl-19430
				if (m_loginDialog.getTerminateCode() == TA_Base_App::TA_ControlStation::EXIT)
                {
					std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
					TA_Base_Bus::AuthenticationLibrary authLibrary;
					authLibrary.endSession(sessionId);
                }
				//cl-19430
                EndWaitCursor(); // remove the hourglass cursor

                if (permission != TA_Base_App::TA_ControlStation::DENIED)
					 {
						 userMsg << actionName;
					 }

                switch (permission)
                {
                case TA_Base_App::TA_ControlStation::PERMITTED:
                {
                    UINT resp = userMsg.showMsgBox(IDS_UW_010003);
                    if (resp == IDYES)
                    {
                        // exit/restart is allowed and user wants to terminate
                        exit(isRestart);
                        return S_OK;
                    }
                    LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "User has chosen not to %s", actionName.GetBuffer(actionName.GetLength()));
                    break;
                }
                case TA_Base_App::TA_ControlStation::DENIED:
						 {
							 std::string userName;
							 std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
							 TA_Base_Bus::AuthenticationLibrary authLibrary;
							 TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
							 std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
							 userName = iOperator->getName();
							 userMsg << userName;
							 userMsg << actionName;
							 userMsg.showMsgBox(IDS_UE_010021);
							 break;
						 }
                case TA_Base_App::TA_ControlStation::UNKNOWN:
                    userMsg.showMsgBox(IDS_UE_020056);
                    break;
                case TA_Base_App::TA_ControlStation::GENERAL_ERROR:
                default:
                    userMsg.showMsgBox(IDS_UE_010018);
                    break;
                }
                m_loginDialog.useExistingDataOnNextDoModal();
            }

            else
            {
                BeginWaitCursor(); // display the hourglass cursor
                // response == IDOK
                TA_VERIFY(actionName.LoadString(IDS_LOGIN)!=0, "Unable to load IDS_LOGIN");
                SessionManager::getInstance().login(m_loginDialog);
                isSuccessfulLogin = true;

                if (m_launchBar != NULL)
                {
                    if (m_launchBar->m_hWnd != NULL)
                    {
                        m_launchBar->ShowWindow(SW_SHOW);
                    }
                }

                EndWaitCursor(); // remove the hourglass cursor
            }
        }
        catch ( SessionException& exception)
        {
            userMsg << actionName;
            unsigned int errorId;
            CString agentName;
            TA_VERIFY(agentName.LoadString(IDS_AUTHENTICATION_AGENT)!=0, "Unable to load IDS_AUTHENTICATION_AGENT");
            switch (exception.getFailType())
            {
            case SessionException::AUTHENTICATION_FAILED:
                errorId = IDS_UE_020046;
                break;
            case SessionException::AUTHENTICATION_AGENT_FAILED:
                userMsg << agentName;
                errorId = IDS_UE_030064;
                break;
            default:
                errorId = IDS_UE_010018;
                break;
            }
            userMsg.showMsgBox(errorId);
            m_loginDialog.useExistingDataOnNextDoModal();
        }
        catch ( GraphWorxException&)
        {
            userMsg.showMsgBox(IDS_UE_020049);
            m_loginDialog.useExistingDataOnNextDoModal();
        }

    } while( !isSuccessfulLogin );

    LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayLoginDialogue");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDisplayChangeProfileDialogue(WPARAM,LPARAM)
{
    // TD 8214 check if window is already active
    if (m_loginDialog.m_hWnd != NULL)
    {
        // already open
        return S_OK; 
    }

    // Check if current operator is allowed to change profile
    BeginWaitCursor(); // display the hourglass cursor
    EPermission permission = SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_CHANGE_PROFILE);
    EndWaitCursor(); // remove the hourglass cursor

    CString actionName;
    TA_VERIFY(actionName.LoadString(IDS_CHANGE_PROFILE)!=0, "Unable to load IDS_CHANGE_PROFILE");
    TransActiveMessage userMsg;

    if (permission == TA_Base_App::TA_ControlStation::DENIED)
    {
		 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			 "Operator tries to change profile without the right permission");
		 std::string userName;
		 std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		 TA_Base_Bus::AuthenticationLibrary authLibrary;
		 TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
		 std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
		 userName = iOperator->getName();
		 userMsg << userName;
		 userMsg << actionName;
		 userMsg.showMsgBox(IDS_UE_010021);
		 LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeProfileDialogue");
		 return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::UNKNOWN)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "The system could not determine the rights of the current operator to change profile");
		  userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_020056);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeProfileDialogue");
        return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::GENERAL_ERROR)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Some kind of error has occurred while checking for rights to change profile");
		  userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010018);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeProfileDialogue");
        return S_OK;
    }

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    // We got rights to do this, so continue

    bool isSuccessfulChange = false;
    m_loginDialog.setDialogType(TA_Base_App::TA_ControlStation::CHANGE_PROFILE_MODE);

    do
    {
        // Call DoModal on the dialog box. This will display and wait for the run()
        // method to complete before allowing the user to do anything.
        int response = m_loginDialog.DoModal();
        if (response == IDCANCEL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "User has cancelled the change profile");
            break;
        }
		if (!SessionManager::getInstance().getDutyManager().loseExclusiveDuty())
        {
            break;
        }
		try
		{
			BeginWaitCursor(); // display the hourglass cursor
			SessionManager::getInstance().changeProfile(m_loginDialog);
			loginDetailsChanged();
			isSuccessfulChange = true;
			EndWaitCursor(); // remove the hourglass cursor
		}
		catch ( const SessionException& exception)
		{
            userMsg << actionName;
			unsigned int errorId;
			CString agentName;
			TA_VERIFY(agentName.LoadString(IDS_AUTHENTICATION_AGENT)!=0, "Unable to load IDS_AUTHENTICATION_AGENT");
			switch (exception.getFailType())
			{
			case SessionException::AUTHENTICATION_FAILED:
				errorId = IDS_UE_020046;
				break;
			case SessionException::AUTHENTICATION_AGENT_FAILED:
				userMsg << agentName;
				errorId = IDS_UE_030064;
				break;
			default:
				errorId = IDS_UE_010018;
				break;
			}
			userMsg.showMsgBox(errorId);
			m_loginDialog.useExistingDataOnNextDoModal();
		}
    } while( !isSuccessfulChange );

	// fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
	if (m_comControlStation != NULL)
	{
		m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeProfileDialogue");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDisplayOperatorOverrideDialogue(WPARAM,LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDisplayOperatorOverrideDialogue");	

    bool static isShowingRemoveOverrideMsgBox = false;

    //TD641 - for reason similar to TD653 see the comment in onLogout
    if ((m_loginDialog.m_hWnd != NULL) || isShowingRemoveOverrideMsgBox)
    {
        ShowOwnedPopups(); // show the popup if it's already being displayed
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already have 1 instance of operator override dialog/remove override message box opened");
        return S_OK;
    }

    // Check if current operator is allowed to do operator override
    BeginWaitCursor(); // display the hourglass cursor
    EPermission permission = SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_OVERRIDE);
    EndWaitCursor(); // remove the hourglass cursor

    CString actionName;
    TA_VERIFY(actionName.LoadString(IDS_OPERATOR_OVERRIDE)!=0, "Unable to load IDS_OPERATOR_OVERRIDE");

    if (permission == TA_Base_App::TA_ControlStation::DENIED)
    {
		 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			 "Operator tries to perform operator override without the right permission");
		 TransActiveMessage userMsg;
		 std::string userName;
		 std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		 TA_Base_Bus::AuthenticationLibrary authLibrary;
		 TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
		 std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
		 userName = iOperator->getName();
		 userMsg << userName;
		 userMsg << actionName;
		 userMsg.showMsgBox(IDS_UE_010021);
		 LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayOperatorOverrideDialogue");
		 return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::UNKNOWN)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "The system could not determine the rights of the current operator to override");
		TransActiveMessage userMsg;
		userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_020056);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayOperatorOverrideDialogue");
        return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::GENERAL_ERROR)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Some kind of error has occurred while checking for rights to override");
		TransActiveMessage userMsg;
		userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010018);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayOperatorOverrideDialogue");
        return S_OK;
    }

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    // We got rights to do this, so continue

    // If we are already in operator override mode then remove it.
    if( SessionManager::getInstance().getSessionDetails().isOperatorOverridden() )
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Operator override already applied so we are removing it");

        isShowingRemoveOverrideMsgBox = true;
        TA_VERIFY(actionName.LoadString(IDS_REMOVE_OPERATOR_OVERRIDE)!=0, "Unable to load IDS_REMOVE_OPERATOR_OVERRIDE");
		TransActiveMessage userMsg;
        userMsg << actionName;

        int response = userMsg.showMsgBox(IDS_UW_010003);
        if ((response == IDYES) && SessionManager::getInstance().getDutyManager().loseExclusiveDuty())
        {
			BeginWaitCursor(); // display the hourglass cursor
			SessionManager::getInstance().removeOperatorOverride();
			loginDetailsChanged();
			EndWaitCursor(); // remove the hourglass cursor
		}
		// fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
		if (m_comControlStation != NULL)
		{
			m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
		}
        isShowingRemoveOverrideMsgBox = false;
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayOperatorOverrideDialogue");
		return S_OK;			
    }

    // Otherwise we want to add an operator override

    // Display operator override dialogue here.
    bool isSuccessfulOverride = false;
    m_loginDialog.setDialogType(TA_Base_App::TA_ControlStation::OPERATOR_OVERRIDE_MODE);

    do
    {
        // Call DoModal on the dialog box. This will display and wait for the run()
        // method to complete before allowing the user to do anything.
        int response = m_loginDialog.DoModal();
        if (response == IDCANCEL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "Operator cancelled the operator override");
            break;
        }

        try
        {
            BeginWaitCursor(); // display the hourglass cursor
            SessionManager::getInstance().operatorOverride(m_loginDialog);
            loginDetailsChanged();
            isSuccessfulOverride = true;
            EndWaitCursor(); // remove the hourglass cursor
        }
        catch ( SessionException& exception)
        {
            unsigned int errorId;
            CString agentName;
			TransActiveMessage userMsg;
            userMsg << actionName;
            TA_VERIFY(agentName.LoadString(IDS_AUTHENTICATION_AGENT)!=0, "Unable to load IDS_AUTHENTICATION_AGENT");
            switch (exception.getFailType())
            {
            case SessionException::AUTHENTICATION_FAILED:
                errorId = IDS_UE_020046;
                break;
            case SessionException::AUTHENTICATION_AGENT_FAILED:
                userMsg << agentName;
                errorId = IDS_UE_030064;
                break;
            default:
                errorId = IDS_UE_010018;
                break;
            }
            userMsg.showMsgBox(errorId);
            m_loginDialog.useExistingDataOnNextDoModal();
        }
    } while( !isSuccessfulOverride );

	// fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
	if (m_comControlStation != NULL)
	{
		m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
	}
    LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayOperatorOverrideDialogue");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDisplayChangeOperatorDialogue(WPARAM, LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDisplayChangeOperatorDialogue");

    //TD641 - for reason similar to TD653 see the comment in onLogout
    if (m_loginDialog.m_hWnd != NULL)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already have 1 instance of change operator dialog opened");
        return S_OK;
    }

    // Check if current operator is allowed to change operator
    BeginWaitCursor(); // display the hourglass cursor
    EPermission permission = SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_CHANGE_OPERATOR);
    EndWaitCursor(); // remove the hourglass cursor
    
    CString actionName;
    TA_VERIFY(actionName.LoadString(IDS_CHANGE_OPERATOR)!=0, "Unable to load IDS_CHANGE_OPERATOR");
    TransActiveMessage userMsg;
    

    if (permission == TA_Base_App::TA_ControlStation::DENIED)
    {
		 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			 "Operator tries to change operator without the right permission");
		 std::string userName;
		 std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		 TA_Base_Bus::AuthenticationLibrary authLibrary;
		 TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
		 std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
		 userName = iOperator->getName();
		 userMsg << userName;
		 userMsg << actionName;
		 userMsg.showMsgBox(IDS_UE_010021);
		 LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeOperatorDialogue");
		 return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::UNKNOWN)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "The system could not determine the rights of the current operator to change operator");
		  userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_020056);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeOperatorDialogue");
        return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::GENERAL_ERROR)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Some kind of error has occurred while checking for rights to change operator");
		  userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010018);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeOperatorDialogue");
        return S_OK;
    }

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    // We got rights to do this, so continue

    bool isSuccessfulChange = false;
    m_loginDialog.setDialogType(TA_Base_App::TA_ControlStation::CHANGE_OPERATOR_MODE);

    do
    {
        // Call DoModal on the dialog box. This will display and wait for the run()
        // method to complete before allowing the user to do anything.
        int response = m_loginDialog.DoModal();
        if (response == IDCANCEL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "User has cancelled the change operator");
            break;
        }

        try
        {
            BeginWaitCursor(); // display the hourglass cursor

            SessionManager::getInstance().changeOperator(m_loginDialog);
            loginDetailsChanged();
            isSuccessfulChange = true;
            EndWaitCursor(); // remove the hourglass cursor
        }
        catch ( const SessionException& exception)
        {
            unsigned int errorId;
            CString agentName;
            TA_VERIFY(agentName.LoadString(IDS_AUTHENTICATION_AGENT)!=0, "Unable to load IDS_AUTHENTICATION_AGENT");
            switch (exception.getFailType())
            {
            case SessionException::AUTHENTICATION_FAILED:
                errorId = IDS_UE_020046;
                break;
            case SessionException::AUTHENTICATION_AGENT_FAILED:
                userMsg << agentName;
                errorId = IDS_UE_030064;
                break;
            default:
                errorId = IDS_UE_010018;
                break;
            }
            userMsg.showMsgBox(errorId);
            m_loginDialog.useExistingDataOnNextDoModal();
        }
    } while( !isSuccessfulChange );

	// fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
	if (m_comControlStation != NULL)
	{
		m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
	}
    LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangeOperatorDialogue");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDisplayChangePasswordDialogue(WPARAM, LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDisplayChangePasswordDialogue");
    bool isSuccessfulChange = false;

    //TD641 - for reason similar to TD653 see the comment in onLogout
    if (m_passwordDialog.m_hWnd != NULL) 
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already have 1 instance of change password dialog opened");
        return S_OK;
    }

    // Check if current operator is allowed to change password
    BeginWaitCursor(); // display the hourglass cursor
    EPermission permission = SessionManager::getInstance().isControlPermitted(TA_Base_Bus::aca_CONTROL_STATION_CHANGE_PASSWORD);
    EndWaitCursor(); // remove the hourglass cursor

    CString actionName;
    TA_VERIFY(actionName.LoadString(IDS_CHANGE_PASSWORD)!=0, "Unable to load IDS_CHANGE_PASSWORD");
    

    if (permission == TA_Base_App::TA_ControlStation::DENIED)
    {
		 LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
			 "Operator tries to change password without the right permission");
		 std::string userName;
		 std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
		 TA_Base_Bus::AuthenticationLibrary authLibrary;
		 TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
		 std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
		 userName = iOperator->getName();
         TransActiveMessage userMsg;
		 userMsg << userName;
		 userMsg << actionName;
		 userMsg.showMsgBox(IDS_UE_010021);
		 LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangePasswordDialogue");
		 return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::UNKNOWN)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "The system could not determine the rights of the current operator to change password");
         TransActiveMessage userMsg;
		userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_020056);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangePasswordDialogue");
        return S_OK;
    }
    else if (permission == TA_Base_App::TA_ControlStation::GENERAL_ERROR)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Some kind of error has occurred while checking for rights to change password");
         TransActiveMessage userMsg;
		  userMsg << actionName;
        userMsg.showMsgBox(IDS_UE_010018);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangePasswordDialogue");
        return S_OK;
    }

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    // We got rights to do this, so continue

    do
    {
        // Call DoModal on the dialog box. This will display and wait for the run()
        // method to complete before allowing the user to do anything.
        int response = m_passwordDialog.DoModal();
        if (response == IDCANCEL)
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "User has cancelled the change password");
            break;
        }

        try
        {
            BeginWaitCursor(); // display the hourglass cursor
            SessionManager::getInstance().changePassword(m_passwordDialog);
            loginDetailsChanged();
            isSuccessfulChange = true;
            EndWaitCursor(); // remove the hourglass cursor
        }
        catch ( const SessionException& ex)
        {
            TransActiveMessage userMsg;
            unsigned int errorId;
            CString agentName, password;
            TA_VERIFY(agentName.LoadString(IDS_AUTHENTICATION_AGENT)!=0, "Unable to load IDS_AUTHENTICATION_AGENT");
            TA_VERIFY(password.LoadString(IDS_PASSWORD)!=0, "Unable to load IDS_PASSWORD");
            switch (ex.getFailType())
            {
            case SessionException::AUTHENTICATION_FAILED:
                userMsg << actionName;
                errorId = IDS_UE_020046;
                break;
            case SessionException::AUTHENTICATION_AGENT_FAILED:
                userMsg << actionName;
                userMsg << agentName;
                errorId = IDS_UE_030064;
                break;
            case SessionException::INVALID_PASSWORD_LENGTH:
                userMsg << password;
                userMsg << MIN_PASSWORD_LENGTH;
                errorId = IDS_UE_020055;
                m_passwordDialog.useExistingDataOnNextDoModal();
                break;
            case SessionException::PASSWORD_MISMATCH:
                errorId = IDS_UE_020045;
                m_passwordDialog.useExistingDataOnNextDoModal();
                break;
            default:
                userMsg << actionName;
                errorId = IDS_UE_010018;
                break;
            }
            userMsg.showMsgBox(errorId);
       }
    } while( !isSuccessfulChange );

	// fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
	if (m_comControlStation != NULL)
	{
		m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
	}

    LOG( SourceInfo, DebugUtil::FunctionExit, "onDisplayChangePasswordDialogue");
	return S_OK;
}


LRESULT ControlStationRootDialog::onLogout(WPARAM,LPARAM)
{
    // TD641, TD653 If this method get called from GraphWorx multiple times, multiple 
    // dialog boxes will be launched even if the dialog box is modal.
    // For some reason the same thread could run this method again without finishing
    // the one that is already running even if a normal thread guard is used here.
    // To get around this, we need to check to see if the dialog is already showing
    // before launching again.
    static bool isShowingConfirmationError = false;
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onLogout");

    if ((m_logoutDialog.m_hWnd != NULL) || isShowingConfirmationError)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already showing logout dialog/confirmation message box");
        return S_OK;
    }

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    int response = m_logoutDialog.DoModal();

    if (response == IDCANCEL)
    {
        // fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
        if (m_comControlStation != NULL)
        {
            m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
        }

        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "User has cancelled action");
        LOG( SourceInfo, DebugUtil::FunctionExit, "onLogout");
        return S_OK;
    }

    isShowingConfirmationError = true;

    CString actionName;
    unsigned long resourceId = 0;
    // otherwise, it's IDOK.  Determine if it is logout, exit or restart.
    switch(m_logoutDialog.getTerminateCode())
    {
    case TA_Base_App::TA_ControlStation::EXIT:
        resourceId = IDS_EXIT;
        break;
    case TA_Base_App::TA_ControlStation::RESTART:
        resourceId = IDS_RESTART;
        break;
    case TA_Base_App::TA_ControlStation::LOGOUT:
    default:
        resourceId = IDS_LOGOUT;
        break;
    }

    TA_VERIFY(actionName.LoadString(resourceId)!=0, "Unable to load IDS_EXIT, RESTART or LOGOUT");
    TransActiveMessage userMsg;
    userMsg << actionName;
    int resp = userMsg.showMsgBox(IDS_UW_010003);
    if (resp == IDNO)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "User chosen not to %s", actionName.GetBuffer(actionName.GetLength()));
	    // fire off MENU_RESUME_MESG message to GraphWorx to get it enable its menus again
	    if (m_comControlStation != NULL)
	    {
		    m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
	    }
    }
    else
    {
        // td8635 give user a chance to stop logout after finding out what 
        // exclusive duties they have
        if (SessionManager::getInstance().getDutyManager().loseExclusiveDuty())
        {
            switch(m_logoutDialog.getTerminateCode())
            {
            case TA_Base_App::TA_ControlStation::LOGOUT:
                logout();
                break;
            case TA_Base_App::TA_ControlStation::EXIT:
                 exit();
                break;
            case TA_Base_App::TA_ControlStation::RESTART:
                 exit(true);
                break;
            default:
                TA_ASSERT(false, "Some unhandled terminate type");
                break;
            }
        }
        else
        {
            m_comControlStation->Fire_shortMessage(MENU_RESUME_MESG.AllocSysString());
        }
    }
    isShowingConfirmationError = false;
    LOG( SourceInfo, DebugUtil::FunctionExit, "onLogout");
	return S_OK;
}


LRESULT ControlStationRootDialog::onForceLogout(WPARAM,LPARAM)
{
    static bool isShowing = false;
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onForceLogout");

    // if it's already showing, don't show another message box
    if (isShowing)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already showing a force logout message box");
        return S_OK;
    }

    isShowing = true;

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    CString sessionType;
    TA_VERIFY(sessionType.LoadString(IDS_LOGIN)!=0, "Unable to load IDS_LOGIN");
    TransActiveMessage userMsg;
    userMsg << sessionType;
    userMsg.showMsgBox(IDS_UE_020052);

    logout();
    isShowing = false;
    LOG( SourceInfo, DebugUtil::FunctionExit, "onForceLogout");
	return S_OK;
}


LRESULT ControlStationRootDialog::onForceEndOverride(WPARAM,LPARAM)
{
    static bool isShowing = false;
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onForceEndOverride");

    // if it's already showing, don't show another message box
    if (isShowing)
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Already showing a force end override message box");
        return S_OK;
    }

    isShowing = true;

    // fire off MENU_SUSPEND_MESG message to GraphWorx so that the menu won't be active
    // when the logout dialog is showing.  This prevents a selected menu from 
    // remaining visible when graphworx is asked to hide (i.e. user logout).
    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_shortMessage(MENU_SUSPEND_MESG.AllocSysString());
    }

    CString sessionType;
    TA_VERIFY(sessionType.LoadString(IDS_LOGIN)!=0, "Unable to load IDS_LOGIN");
    TransActiveMessage userMsg;
    userMsg << sessionType;
    userMsg.showMsgBox(IDS_UE_020052);

    SessionManager::getInstance().removeOperatorOverride();
    loginDetailsChanged();
    isShowing = false;
    LOG( SourceInfo, DebugUtil::FunctionExit, "onForceEndOverride");
	return S_OK;
}


LRESULT ControlStationRootDialog::onLaunchDisplay(WPARAM display, LPARAM )
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onLaunchDisplay");

    DisplayInformation* info = reinterpret_cast<DisplayInformation*>(display);

    if (info == NULL)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "Display passed was NULL. Cannot carry on and launch display");
        LOG( SourceInfo, DebugUtil::FunctionExit, "onLaunchDisplay");
        return S_FALSE;
    }

    if (m_comControlStation != NULL)
    {
        // Need to find out if there is a GraphWorX display located on the screen requested.
        // First need to find out what screen that is...
        unsigned int screenNumber = SessionManager::getInstance().getScreenPositioning().getWhichScreenCoordinateIsOn(info->position);

        // Now that we know the screen, we can determine if there is a graphworx display actually present on that
        // screen. If not, will need to "find" one.
        CRect screenArea = SessionManager::getInstance().getScreenPositioning().getRect(
            TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC,TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,screenNumber);

        if (screenArea.IsRectEmpty())
        {
            // The current screen does not have any graphworx areas on it, so need to search to see if we can find
            // one.
            CRect screenAreaLeft;
            if (screenNumber >= 2)
            {   
                screenAreaLeft = SessionManager::getInstance().getScreenPositioning().getRect(
                    TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC,TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,screenNumber - 1);
            }
            else
            {
                screenAreaLeft.SetRectEmpty();
            }

            CRect screenAreaRight;
            if (screenNumber < SessionManager::getInstance().getScreenPositioning().getNumberOfMonitors())
            {
                screenAreaRight = SessionManager::getInstance().getScreenPositioning().getRect(
                    TA_Base_Bus::TA_GenericGui::SCREEN_SPECIFIC,TA_Base_Bus::TA_GenericGui::AREA_SCHEMATIC,screenNumber + 1);
            }
            else
            {
                screenAreaRight.SetRectEmpty();
            }

            CRect screenAreaLaunchBar = SessionManager::getInstance().getScreenPositioning().getAppRectNextToLaunchBar();

            // Now out of the three rectangles, find one that works
            if (!screenAreaLeft.IsRectEmpty())
            {
                // If there was one available to the left, move the position one screen left.
                info->position = info->position - SessionManager::getInstance().getScreenPositioning().getEachScreenWidth();
            }
            else if (!screenAreaRight.IsRectEmpty())
            {
                // If the left wasn't available check the right
                info->position = info->position + SessionManager::getInstance().getScreenPositioning().getEachScreenWidth();
            }
            else if (!screenAreaLaunchBar.IsRectEmpty())
            {
                // Finally, if the right wasn't available, check the launch bar area
                info->position = screenAreaLaunchBar.left + 1;
            }
            else
            {
                // There aren't any graphworx displays, so log and exit
                LOG_GENERIC(SourceInfo,DebugUtil::DebugWarn,
                    "There were no active GraphWorX displays, so cannot launch display %s", info->display);

                return S_FALSE;
            }
        }
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, "launching display %s with asset %s on screen %li", info->display, info->asset, info->position);
        //TA_Base_Core::Thread::sleep(250); // Allow some time for the COM's launchDisplay to return before firing the events
        m_comControlStation->Fire_loadDisplayEx(info->display.AllocSysString(), info->asset.AllocSysString(), info->position);
        m_comControlStation->Fire_loadDisplay(info->display.AllocSysString(), info->position);
        delete reinterpret_cast<DisplayInformation*>(info);
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "onLaunchDisplay");
	return S_OK;
}


LRESULT ControlStationRootDialog::onServerStateChange(WPARAM isContactable, LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onServerStateChange");

    if (m_comControlStation != NULL)
    {
        m_comControlStation->Fire_networkStatusChanged(isContactable);
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "onServerStateChange");
	return S_OK;
}


LRESULT ControlStationRootDialog::onSystemControllerStateChange(WPARAM isOnline, LPARAM group)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onSystemControllerStateChange");

    // For any change in the system controller status, need to update the login dialog (if it is visible) to 
    // restrict the visible locations as necessary
    if (m_loginDialog.m_hWnd != NULL)
    {
        // Notify the login dialog only if it exists. Use the same message name as we used for this method.
        m_loginDialog.PostMessage(WM_SYSTEMCONTROLLER_STATE_CHANGE);
    }

    // TD 11910: Send messages to GraphWorx to change the colour of the operator/profile panel
    // depending on the state of the system.
    if(m_comControlStation != NULL)
    {
        if(isOnline)
        {
			if( m_soundInterface != NULL)
			{
				try
				{
					m_soundInterface->setPriority(TA_Base_App::TA_ControlStation::GROUP_STATUS_SOUND_CATEGORY,
					TA_Base_App::TA_ControlStation::GROUP_ONLINE_SOUND_CATEGORY);
				}
				// If there are any exceptions, just log and continue
				catch(TA_Base_Core::SoundPlayerException& ex)
				{
					std::string message("Caught while attempting to play sound for group online, with error: ");
					message += ex.what();
					LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to clear Duty Notificaiton sound.");
				}
			}
            m_comControlStation->Fire_shortMessage(SYSTEM_NORMAL_MESG.AllocSysString());
        }
        else
        {
			if( m_soundInterface != NULL)
			{
				try
				{
					m_soundInterface->setPriority(TA_Base_App::TA_ControlStation::GROUP_STATUS_SOUND_CATEGORY,
					TA_Base_App::TA_ControlStation::GROUP_OFFLINE_SOUND_CATEGORY);
				}
				// If there are any exceptions, just log and continue
				catch(TA_Base_Core::SoundPlayerException& ex)
				{
					std::string message("Caught while attempting to play sound for group offline, with error: ");
					message += ex.what();
					LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
				}
				catch(...)
				{
					LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to clear Duty Notificaiton sound.");
				}
			}
            m_comControlStation->Fire_shortMessage(SYSTEM_DEGRADED_MESG.AllocSysString());
        }
    }

    // Retrieve the group that has changed state
    // ++ AZ: TD6972 : check for NULL
    if (group != 0)
    {
        std::string* groupString = reinterpret_cast <std::string*> (group);
    
        CString status;
        TA_VERIFY(status.LoadString(isOnline?IDS_ONLINE:IDS_OFFLINE)!=0, "Unable to load IDS_ONLINE/OFFLINE");
    
        TransActiveMessage userMsg;
        userMsg << *groupString << status;
        userMsg.showMsgBox(IDS_UE_020058);

		if( m_soundInterface != NULL)
		{
			try
			{
				m_soundInterface->setPriority(TA_Base_App::TA_ControlStation::GROUP_STATUS_SOUND_CATEGORY, 0);
			}
			// If there are any exceptions, just log and continue
			catch(TA_Base_Core::SoundPlayerException& ex)
			{
				std::string message("Caught while attempting to clear sound for group status change, with error: ");
				message += ex.what();
				LOG_EXCEPTION_CATCH(SourceInfo,"SoundPlayerException",message);
			}
			catch(...)
			{
				LOG_EXCEPTION_CATCH(SourceInfo,"Unknown Exception","Caught an exception while attempting to clear Duty Notificaiton sound.");
			}
		}


        delete groupString;
    }
    // AZ: TD6972 ++

    LOG( SourceInfo, DebugUtil::FunctionExit, "onSystemControllerStateChange");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDutyChange(WPARAM dutyNotificationDetail, LPARAM nSeqFlag)
{
    // pop up message to see if user wants to launch the duty manager
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDutyChange");

    // if there's no active session, then don't display it
    if (!SessionManager::getInstance().getSessionDetails().isSessionValid())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Logging out, so not going to show dialog");
        return S_OK;
    }

	// TD19075 ++
	// here we create a new dlg
	if (1 == nSeqFlag)
	{
		DutyChangeNoteDlg* pNoteDlg = new DutyChangeNoteDlg();
		if (pNoteDlg->DoModal() == IDCANCEL)
		{
			delete pNoteDlg;

			//m_comControlStation->Fire_dutyChanged();
			this->postDutyChanged();
			
			LOG( SourceInfo, DebugUtil::FunctionExit, "onDutyChange");
			return S_OK;
		}
	}
	// ++ TD19075 
	

    // Allow multiple Duty Notification dialogs to appear
    // have to create this on the heap so that it remains valid when the method ends
    DutyNotificationDialog* dutyNotificationDialog = new DutyNotificationDialog(this, reinterpret_cast<DutyManager::DutyNotificationDetail*>(dutyNotificationDetail));
    if (dutyNotificationDialog->create())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
            "Unable to display the duty notification dialog, so just ignore");
    }
    referenceDialog(*dutyNotificationDialog);

    //m_comControlStation->Fire_dutyChanged();
	this->postDutyChanged();

    LOG( SourceInfo, DebugUtil::FunctionExit, "onDutyChange");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDutyRequest(WPARAM dutyRequestDetail, LPARAM)
{
    // pop up message to see if user wants to launch the duty manager
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDutyRequest");

    // if there's no active session, then don't display it
    if (!SessionManager::getInstance().getSessionDetails().isSessionValid())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugInfo, 
            "Logging out, so not going to show dialog");
        return S_OK;
    }

    // Allow multiple Duty Notification dialogs to appear
    // have to create this on the heap so that it remains valid when the method ends
    DutyManager::DutyRequestDetail* detail = reinterpret_cast<DutyManager::DutyRequestDetail*>(dutyRequestDetail);
    DutyRequestDialog* dutyRequestDialog = new DutyRequestDialog(this, detail);
    // if could not create the duty request dialog successfully, then just accept request
    if (!dutyRequestDialog->create())
    {
        LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
            "Unable to display the duty request dialog, so automatically accept the request");
        onDutyResponse(reinterpret_cast<WPARAM>(&detail->uniqueId), static_cast<LPARAM>(TA_Base_App::TA_ControlStation::ACCEPT));
    }
    referenceDialog(*dutyRequestDialog);
    LOG( SourceInfo, DebugUtil::FunctionExit, "onDutyRequest");
	return S_OK;
}


LRESULT ControlStationRootDialog::onDutyResponse(WPARAM uniqueId, LPARAM response)
{
    // pop up message to see if user wants to launch the duty manager
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onDutyResponse");

    std::string* id = reinterpret_cast<std::string*>(uniqueId);
    TA_Base_App::TA_ControlStation::EDutyResponse resp = static_cast<TA_Base_App::TA_ControlStation::EDutyResponse>(response);
    SessionManager::getInstance().getDutyManager().respond(*id, resp);
    delete id;

    LOG( SourceInfo, DebugUtil::FunctionExit, "onDutyResponse");
	return S_OK;
}


LRESULT ControlStationRootDialog::onShowTaskManager(WPARAM, LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onShowTaskManager");

    if ( (m_taskManager == NULL) || (m_taskManager->m_hWnd == NULL) )
    {
        CString taskManager;
        TA_VERIFY(taskManager.LoadString(IDS_TASK_MANAGER)!=0, "Unable to load IDS_TASK_MANAGER");
        TransActiveMessage userMsg;
        userMsg << taskManager;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_020050);
        LOG( SourceInfo, DebugUtil::FunctionExit, "onShowTaskManager");
        return S_OK;
    }

    m_taskManager->ShowWindow(SW_SHOW);

    LOG( SourceInfo, DebugUtil::FunctionExit, "onShowTaskManager");
	return S_OK;
}


LRESULT ControlStationRootDialog::onHideGraphWorxDisplays(WPARAM, LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onHideGraphWorxDisplays");
    if (m_comControlStation != NULL)
    {
        // Send an event to GraphWorx to tell it to hide all displays
        m_comControlStation->Fire_hideDisplays();
    }
    LOG( SourceInfo, DebugUtil::FunctionExit, "onHideGraphWorxDisplays");
	return S_OK;
}


LRESULT ControlStationRootDialog::onLoginDetailsChanged(WPARAM,LPARAM)
{
    loginDetailsChanged();
	return S_OK;
}


LRESULT ControlStationRootDialog::onCreateDialogs(WPARAM,LPARAM)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "onCreateDialogs");

    if (m_taskManager == NULL)
    {
        m_taskManager = new CTaskManagerDlg();
        m_taskManager->Create(IDD_TASK_MANAGER,this);
        m_taskManager->ShowWindow(SW_HIDE);
    }

    if (SessionManager::getInstance().getScreenPositioning().isLaunchBarDisplayed())
    {
        if (m_launchBar == NULL)
        {
            m_launchBar = new ControlStationBar();
        }

        m_launchBar->Create(IDD_LAUNCH_BAR,this);
        if (m_launchBar->m_hWnd != NULL)
        {
            m_launchBar->ShowWindow(SW_HIDE);
        }
    }

    LOG( SourceInfo, DebugUtil::FunctionExit, "onCreateDialogs");
	return S_OK;
}


LRESULT ControlStationRootDialog::dereferenceDialog(WPARAM dialog, LPARAM)
{
    CWnd* dlg = reinterpret_cast<CWnd*>(dialog);
    if (dlg != NULL)
    {
        m_childDialog.erase(std::remove(m_childDialog.begin(), m_childDialog.end(), dlg), m_childDialog.end());
    }
	return S_OK;
}


LRESULT ControlStationRootDialog::displayLaunchApplicationError(WPARAM wp, LPARAM lp)
{
    CString applicationName(reinterpret_cast<std::string*>(wp)->c_str());
	delete reinterpret_cast<std::string*>(wp);
   // TA_VERIFY(applicationName.LoadString(IDS_REQUESTED_APPLICATION)!=0, "Unable to load IDS_REQUESTED_APPLICATION");
    TransActiveMessage userMsg;
    userMsg << applicationName;
    userMsg.showMsgBox(IDS_UE_010002);
	return S_OK;
}


LRESULT ControlStationRootDialog::onTAToGXMessage(WPARAM pMsg, LPARAM)
{    
    FUNCTION_ENTRY("onTAToGXMessage");

    TAToGxMessageInformation *pGxMsg = reinterpret_cast<TAToGxMessageInformation*>(pMsg);

    if (pMsg == NULL)
    {
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "onTAToGXMessage passed was NULL. Ignore notification");
        return S_OK;
    }

    if (m_comControlStation != NULL)
    {        
        //LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Sending TAToGXMessage %s with asset %s on screen %li", info->display, info->asset, info->position);
        m_comControlStation->Fire_gxMessage(pGxMsg->targetSchematicID.AllocSysString(), pGxMsg->sourceEntityName.AllocSysString(), pGxMsg->message.AllocSysString());
        delete pGxMsg;
    }

    FUNCTION_EXIT;
	return S_OK;
}

//lizettejl++ (DP-changes) 
LRESULT ControlStationRootDialog::onGXToTAMessage(WPARAM pMsg, LPARAM)
{    
    FUNCTION_ENTRY("onGxToTAMessage");
	
	TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage* pGxMsg = 
		reinterpret_cast<TA_Base_Core::IManagedApplicationCorbaDef::GXToTAMessage*>(pMsg);

	if (pMsg == NULL)
	{
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, "onGxToTAMessage passed was NULL. Ignore notification");
        return S_OK;
    }    
	//UINT selectedButton = userMsg.showMsgBox(IDS_UE_020070);
	const char* msg = pGxMsg->message.in();
	std::string token;
 	std::vector<std::string> args;

	// parse pGxMsg->message 
	while (*msg != '\0')
	{
		if ((*msg == ':') && (*(msg+1) == ':'))
		{
			if (!token.empty())
			{
				args.push_back(token);
				token.erase();
			}
			msg+=2;
		}
		token += *msg;
		if (*(msg+1) == '\0')
		{
			args.push_back(token);
		}
		msg++;
	}		

	//message must be in this format:
	//TransActiveMessage::<Transactive message resource ID>[::<message param1>::<message paramN>]
	if (args.size() >= 2)
	{
		// we are expecting a transactive message to passed from graphworx
		TransActiveMessage userMsg;		
		// initialize message ID that we are expecting an invalid message format
		UINT messageId = IDS_UE_803011;
		if (strcmp(args[0].c_str(),"TransActiveMessage") == 0)
		{
			if (strcmp(args[1].c_str(),"IDS_UW_650005") == 0)
			{   //messageId for "Operator to proceed with Normal to DP (NDP) Operation"
				messageId = IDS_UW_650005; 
			}
			else if (strcmp(args[1].c_str(),"IDS_UW_650006") == 0)
			{	//messageId for ""
				messageId = IDS_UW_650006;
			}
		}
		userMsg.showMsgBox(messageId);
	}
	delete pGxMsg;
	pGxMsg = NULL;

	FUNCTION_EXIT;
	return S_OK;
}

/////////////////////////////////////////////////////////////////////////////
// Other methods
/////////////////////////////////////////////////////////////////////////////

void ControlStationRootDialog::loginDetailsChanged()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "loginDetailsChanged");

    CString profileName = SessionManager::getInstance().getSessionDetails().getProfileName().c_str();
    // Send an event to GraphWorX to alert it that the login details such as profile and operator have changed.
    if (m_comControlStation != NULL)
    {
        CString operatorName = SessionManager::getInstance().getSessionDetails().getOperatorName().c_str();
        m_comControlStation->Fire_loginDetailsChanged(operatorName.AllocSysString(),
                                                      profileName.AllocSysString());
    	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fired login details changed to ISCS Main Navigator with operator = %s, profile = %s", operatorName, profileName);
    }

    // Send an event to third party clients to alert them that the login details such as session Id and profile have changed.
    if (m_comRippleControlStation != NULL)
    {
        CString sessionId = SessionManager::getInstance().getSessionDetails().getSessionId().c_str();
        m_comRippleControlStation->Fire_detailsChanged(sessionId.AllocSysString(),
                                                       profileName.AllocSysString());
    	LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Fired login details changed to ATS displays with session ID = %s, profile = %s", sessionId, profileName);
    }


    if( (m_launchBar != NULL) && (m_launchBar->m_hWnd != NULL) )
    {
        m_launchBar->loginDetailsChanged();
    }
	
	//TD15156++
	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_USE_SCREENSAVER.c_str()))
	{
		//install the screen saver
		// construct the full path name of the screen saver
		ScreenSaverInstall install;
		install.InitInstance();
	}
	//++TD15156
	
    LOG( SourceInfo, DebugUtil::FunctionExit, "loginDetailsChanged");
}


void ControlStationRootDialog::OnClose() 
{
    CDialog::OnCancel();
}


void ControlStationRootDialog::exit(bool isRestart /*= false*/)
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "exit");
	//Haipeng Jiang added the codes to unlock the CAD keys
	WinkeysMgr::instance()->unLockCAD();
	
    if( (m_launchBar != NULL) && (m_launchBar->m_hWnd != NULL) )
    {
        // destroy the launch bar window and save any changes to shortcut
        m_launchBar->DestroyWindow();
    }
//TD17838,39 haipeng
	if( m_soundInterface != NULL )
	{
		try
		{
			delete m_soundInterface;
			m_soundInterface = NULL;
		}
		catch (...)
		{
			LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "Exception occurred while attempting to delete Sound Interface");
		}
	}    
	
	// Carry out notifications that the CS is exiting
    SessionManager::getInstance().exit(isRestart);
	SessionManager::getInstance().terminateAndWait();
	SessionManager::getInstance().cleanUp();
	
    cancelChildDialogs();

    // Close down all singletons
    EndDialog(IDCANCEL);
//TD17838,39 haipeng

    LOG( SourceInfo, DebugUtil::FunctionExit, "exit");
}


void ControlStationRootDialog::logout()
{
    LOG( SourceInfo, DebugUtil::FunctionEntry, "logout");

	//Haipeng Jiang added the codes to unlock the CAD keys
	WinkeysMgr::instance()->unLockCAD();
	
    if ( (m_taskManager != NULL) && (m_taskManager->m_hWnd != NULL) )
    {
        m_taskManager->ShowWindow(SW_HIDE);
    }
//TD17838,39 haipeng
	SessionManager::getInstance().terminateAndWait();
//TD17838,39 haipeng
    SessionManager::getInstance().logout();

    loginDetailsChanged();

    cancelChildDialogs();
	Sleep(1000);
    PostMessage(WM_DISPLAY_LOGIN_DIALOG, 0, 0);


    LOG( SourceInfo, DebugUtil::FunctionEntry, "logout");
}


void ControlStationRootDialog::referenceDialog(CWnd& dialog)
{
    m_childDialog.push_back(&dialog);
}


void ControlStationRootDialog::cancelChildDialogs()
{
    TransActiveMessage::closeAllMessages();

    // make all the child dialogs call OnCancel
    for (unsigned int i = 0; i < m_childDialog.size(); i++)
    {
        if (m_childDialog[i]->m_hWnd != NULL)
        {
            m_childDialog[i]->PostMessage(WM_FORCE_CANCEL);
        }
    }

    // TD834 - send a WM_CANCELMODE message to all top level windows to get rid of any popup menus.
    // TD1608 - changed from SendMessage to PostMessage to prevent control station from hanging 
    // on some systems
    ::PostMessage(HWND_BROADCAST, WM_CANCELMODE, 0, 0);
}

bool ControlStationRootDialog::hasInitDialog( )
{
	TA_Base_Core::ThreadGuard guard( m_initDlgLock );
	return m_fHasInitDialog;
}

EventWorkItem::EventWorkItem(ControlStationRootDialog* pDialog):
m_pDialog(pDialog)
{
}
EventWorkItem::~EventWorkItem()
{
	m_pDialog= NULL;

}

void EventWorkItem::executeWorkItem()
{
	m_pDialog->fireDutyChanged();
}
