/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemManager.cpp $
  * @author:  Justin Ebedes
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the main class for the System Manager.
  * It does not inherit from TransactiveWinApp, as
  * the System Manager must be able to run without
  * a Control Station. Instead it inherits directly
  * from CWinApp and re-implements some of the
  * TransactiveWinApp code.
  *
  */

#include "StdAfx.h"
#include "SystemManager.h"
#include "SystemManagerDlg.h"
#include "DataCache.h"
#include "bus/generic_gui/src/GenericGUI.h"
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/generic_gui/src/TransActiveWinApp.h"
#include "bus/generic_gui/src/IGUIApplication.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/exceptions/src/CannotUseSysControllerException.h"
#include "core/threads/src/Thread.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/src/NameValuePair.h"
#include "core/message/types/ApplicationStatusAudit_MessageTypes.h"
#include "core/data_access_interface/entity_access/src/ConsoleAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IConsole.h"

#include "ace/ACE.h"

#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "bus\security\authentication_library\src\AuthenticationLibrary.h"

using TA_Base_Core::GenericGUIException;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
    BEGIN_MESSAGE_MAP(SystemManager, TransActiveWinApp)
        //{{AFX_MSG_MAP(SystemManager)
        // NOTE - the ClassWizard will add and remove mapping macros here.
        //    DO NOT EDIT what you see in these blocks of generated code!
        //}}AFX_MSG
        ON_COMMAND(ID_HELP, CWinApp::OnHelp)
        ON_COMMAND(ID_HELP_INDEX, CWinApp::OnHelpIndex)
    END_MESSAGE_MAP()


    SystemManager::SystemManager()
        : m_genericGUI(NULL),
          m_sessionHandler(NULL),
          m_applicationName("System Manager"),
          TransActiveWinApp(&m_gui, "System Manager")
    {
    }

    /////////////////////////////////////////////////////////////////////////////
    // The one and only SystemManager object

    SystemManager theApp;

    /////////////////////////////////////////////////////////////////////////////
    // SystemManager initialization

    BOOL SystemManager::InitInstance()
    {
        FUNCTION_ENTRY("InitInstance");

        ACE::init();

        AfxEnableControlContainer();

#ifdef _AFXDLL
        //Enable3dControls();			// Call this when using MFC in a shared DLL
#else
        Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

        bool shouldStartMessagePump = false;

        try
        {
            m_genericGUI = new TA_Base_Bus::GenericGUI(m_gui, m_lpCmdLine);
            DataCache::getInstance().run();

            shouldStartMessagePump = m_gui.displayApplication();

            //m_consoleName = retrieveConsoleName();
            std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);

            // See if the application name is already set in RunParams
            if (appName.empty())
            {
                // not yet set, so just set it as the one that was passed in as the
                // argument
                RunParams::getInstance().set(RPARAM_APPNAME, m_applicationName);
                LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Setting the RunParam RPARAM_PROGNAME as %s", m_applicationName);
            }
            else
            {
                // use the one that is already in RunParams
                m_applicationName = appName.c_str();
            }

            //submitAppStartedAuditMessage();
            m_genericGUI->run();
        }
        catch (const GenericGUIException& ex)
        {
            DataCache::getInstance().run();
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "GenericGUIException", "Now calculating which error it was so an appropriate message can be displayed to the user");

            TA_Base_Bus::TransActiveMessage userMsg;

            if (ex.getFailType() != GenericGUIException::ACCESS_DENIED)
            {
                userMsg << m_applicationName;
            }

            unsigned long errorId;

            switch (ex.getFailType())
            {
            case (GenericGUIException::COMMUNICATION_ERROR_WITH_CONTROL_STATION):
            case (GenericGUIException::NO_ACTIVE_SESSION):
                // These errors are ok because we will try other means
                shouldStartMessagePump = runWithoutControlStation();
                FUNCTION_EXIT
                return shouldStartMessagePump;

            case (GenericGUIException::ENTITY_CONFIGURATION_INVALID):
                errorId = IDS_UE_010011;
                break;

            case (GenericGUIException::ENTITY_DOES_NOT_EXIST):
            case (GenericGUIException::ENTITY_NOT_UNIQUE):
                errorId = IDS_UE_010013;
                break;

            case (GenericGUIException::COMMAND_LINE_INVALID):
                errorId = IDS_UE_010008;
                break;

            case (GenericGUIException::NO_DATABASE_CONNECTION):
                errorId = IDS_UE_010001;
                break;

            case (GenericGUIException::ACCESS_DENIED):
            {
                errorId = IDS_UE_010021;
                std::string userName;
                std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
                TA_Base_Bus::AuthenticationLibrary authLibrary;
                TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
                std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
                userName = iOperator->getName();
                std::string actionName = "start this application";
                userMsg << userName;
                userMsg << actionName;
                break;
            }

            case (GenericGUIException::UNKNOWN_ACCESS_RIGHTS):
                errorId = IDS_UE_010019;
                break;

            case (GenericGUIException::INITIALISATION_ERRORS):
            default:
                errorId = IDS_UE_010012;
                std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
                userMsg << appName;
                break;
            }

            userMsg.showMsgBox(errorId);
        }
        catch (const TA_Base_Core::ProcessAlreadyRunningException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "ProcessAlreadyRunningException", "Shutting down quietly as this application is already running and must only be running once.");
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Because this has been caught at the top level we must now shutdown.");

            if (m_genericGUI != NULL)
            {
                m_genericGUI->fatalErrorOccurred();
            }

            std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
            showMsgBox(IDS_UE_010010, appName);
        }

        FUNCTION_EXIT;

        return shouldStartMessagePump;
    }

    void SystemManager::WinHelp(DWORD dwData, UINT nCmd)
    {
        if (HELP_CONTEXT == nCmd)
        {
            // display help for the specified topic
            TA_Base_Bus::HelpLauncher::getInstance().displayContextSpecificHelp(LOWORD(dwData));
        }
        else
        {
            // display the main help contents
            TA_Base_Bus::HelpLauncher::getInstance().displayHelp();
        }
    }

    int SystemManager::ExitInstance()
    {
        FUNCTION_ENTRY("ExitInstance");

        unsigned long entityKey = 0;

        if (m_genericGUI != NULL)
        {
            entityKey = m_genericGUI->getEntityKey();
            m_genericGUI->shutdown();
            m_genericGUI->deactivateAndDeleteServant();
            // Don't need to delete m_genericGUI as the call above does this.
        }

        if (m_sessionHandler != NULL)
        {
            entityKey = m_sessionHandler->getEntityKey();
            delete m_sessionHandler;
            m_sessionHandler = NULL;
        }

        /*
        // TD11480: If the location key is not set, we cannot send audit messages. But even if it
        // is set, we still need to make sure it's not set to 0 (which is possible - eg if the db used
        // by the System Controller is missing the MFT's console). NotifyHosts also needs to be set.
        if ( RunParams::getInstance().isSet(RPARAM_LOCATIONKEY)
            && RunParams::getInstance().isSet(RPARAM_NOTIFYHOSTS)
            && RunParams::getInstance().get(RPARAM_LOCATIONKEY) != "0" )
        {
            // Get the audit message sender with which to send the message
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
                getAuditMessageSender(TA_Base_Core::ApplicationStatusAudit::Context);

            TA_Base_Core::DescriptionParameters params;
            TA_Base_Core::NameValuePair param1(TransActiveWinApp::APPLICATION_NAME, std::string(m_applicationName));
            TA_Base_Core::NameValuePair param2(TransActiveWinApp::CONSOLE_NAME, m_consoleName);
            params.push_back(&param1);
            params.push_back(&param2);

            sender->sendAuditMessage( TA_Base_Core::ApplicationStatusAudit::ApplicationExit,
                                     entityKey,
                                     params,
                                     "N/A",
                                     TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ),
                                     "",
                                     "",
                                     "" );

            // Need to sleep to give the event time to submit itself.
            TA_Base_Core::Thread::sleep(100);

            delete sender;
            sender = NULL;
        }
        else
        {
            LOG_GENERIC(SourceInfo,DebugUtil::DebugFatal,
                "Location key not available on applicaiton exit. No shutdown audit message can be sent.");
        }
        */

        try
        {
            TA_Base_Core::CorbaUtil::getInstance().cleanup();
        }
        catch (...)
        {
        }

        ACE::fini();

        return CWinApp::ExitInstance();
        FUNCTION_EXIT;
    }

    bool SystemManager::runWithoutControlStation()
    {
        FUNCTION_ENTRY("runWithoutControlStation");

        bool shouldStartMessagePump = false;

        // First we need to set the application name to be the hard-coded one since we can't talk to
        // a Control Station
        RunParams::getInstance().set(RPARAM_APPNAME, m_applicationName);
        LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugDebug, "Setting the RunParam RPARAM_PROGNAME as %s", m_applicationName);

        try
        {
            m_sessionHandler = new SessionHandler(m_gui);

            if (m_sessionHandler != NULL)
            {
                shouldStartMessagePump = m_sessionHandler->run();
            }

            //submitAppStartedAuditMessage();
        }
        catch (const GenericGUIException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "GenericGUIException", "Now calculating which error it was so an appropriate message can be displayed to the user");

            TA_Base_Bus::TransActiveMessage userMsg;
            unsigned long errorId;

            switch (ex.getFailType())
            {
            case (GenericGUIException::ENTITY_CONFIGURATION_INVALID):
                errorId = IDS_UE_010011;
                break;

            case (GenericGUIException::ENTITY_DOES_NOT_EXIST):
            case (GenericGUIException::ENTITY_NOT_UNIQUE):
                errorId = IDS_UE_010013;
                break;

            case (GenericGUIException::COMMAND_LINE_INVALID):
                errorId = IDS_UE_010008;
                break;

            case (GenericGUIException::NO_DATABASE_CONNECTION):
                errorId = IDS_UE_010001;
                break;

            case (GenericGUIException::ACCESS_DENIED):
            {
                errorId = IDS_UE_010021;
                std::string userName;
                std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
                TA_Base_Bus::AuthenticationLibrary authLibrary;
                TA_Base_Bus::SessionInfo sessionInfo = authLibrary.getSessionInfo(sessionId, sessionId);
                std::auto_ptr<TA_Base_Core::IOperator> iOperator(TA_Base_Core::OperatorAccessFactory::getInstance().getOperator(sessionInfo.UserId, false));
                userName = iOperator->getName();
                std::string actionName = "start this application";
                userMsg << userName;
                userMsg << actionName;
                break;
            }

            case (GenericGUIException::UNKNOWN_ACCESS_RIGHTS):
                errorId = IDS_UE_010019;
                break;

            case (GenericGUIException::INITIALISATION_ERRORS):
            default:
                errorId = IDS_UE_010012;
                std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
                userMsg << appName;
                break;
            }

            userMsg.showMsgBox(errorId);
        }
        catch (TA_Base_Core::CannotUseSysControllerException& ex)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "CannotUseSysControllerException", "Because this has been caught at the top level we must now shutdown.");
            CString errorMessage;
            errorMessage.Format(ex.what(), m_applicationName);
            UINT selectedButton = showMsgBox(IDS_UE_020071, errorMessage);
        }
        catch (...)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "Unknown", "Because this has been caught at the top level we must now shutdown.");
            std::string appName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME);
            showMsgBox(IDS_UE_010010, appName);
        }

        FUNCTION_EXIT;

        return shouldStartMessagePump;
    }


    void SystemManager::submitAppStartedAuditMessage()
    {
        TA_ASSERT((m_genericGUI != NULL) || (m_sessionHandler != NULL),
                  "Must have successfully initialise app before it could send an audit message");

        m_consoleName = retrieveConsoleName();

        // Need to get the console name of this session (if we can)
        try
        {
            unsigned long entityKey = (m_sessionHandler == NULL) ? m_genericGUI->getEntityKey() : m_sessionHandler->getEntityKey();

            // Get the audit message sender with which to send the message
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().
                    getAuditMessageSender(TA_Base_Core::ApplicationStatusAudit::Context);

            TA_Base_Core::DescriptionParameters params;
            TA_Base_Core::NameValuePair param1(TransActiveWinApp::APPLICATION_NAME, std::string(m_applicationName));
            TA_Base_Core::NameValuePair param2(TransActiveWinApp::CONSOLE_NAME, m_consoleName);
            params.push_back(&param1);
            params.push_back(&param2);

            sender->sendAuditMessage(TA_Base_Core::ApplicationStatusAudit::ApplicationStart,
                                     entityKey,
                                     params,
                                     "N/A",
                                     TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
                                     "",
                                     "",
                                     "");

            delete sender;
            sender = NULL;
        }
        catch (...)
        {
            // If there are any problems, then just log and move on
            LOG_EXCEPTION_CATCH(SourceInfo, "General Exception",
                                "An exception was caught while attempting to send an audit message");
        }
    }


    std::string SystemManager::retrieveConsoleName()
    {
        std::string defaultName = (LPCTSTR)getConsoleName();
        return DataCache::getInstance().getConsoleName(defaultName);
    }
} // namespace TA_Base_App
