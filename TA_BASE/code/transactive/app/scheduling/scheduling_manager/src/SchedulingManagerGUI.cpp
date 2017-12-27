/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/src/SchedulingManagerGUI.cpp $
  * @author:  
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/08/03 17:02:56 $
  * Last modified by:  $Author: qi.huang $
  * 
  * This inherits from AbstractGUIApplication and implements all methods specific to documents.
  */

#include "app/scheduling/scheduling_manager/src/stdafx.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerGUI.h"
#include "app/scheduling/scheduling_manager/src/resource.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerDoc.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerView.h"
#include "app/scheduling/scheduling_manager/src/MainFrm.h"
#include "app/scheduling/scheduling_manager/src/SchedulingManagerConstants.h"
#include "bus/generic_gui/src/TransActiveFrame.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/UserSettingsException.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using namespace TA_Base_App::SchedulingManager;
using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;

namespace TA_Base_App
{

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    SchedulingManagerGUI::SchedulingManagerGUI()
    {   
    }


    SchedulingManagerGUI::~SchedulingManagerGUI()
    {
    }


    void SchedulingManagerGUI::createApplicationObject()
    {
        ::SetThreadLocale( MAKELANGID(LANG_CHINESE_SIMPLIFIED, SUBLANG_CHINESE_SIMPLIFIED) );

        CSingleDocTemplate* pDocTemplate;
        pDocTemplate = new CSingleDocTemplate(
		    IDR_MAINFRAME,
		    RUNTIME_CLASS(CSchedulingManagerDoc),
		    RUNTIME_CLASS(CMainFrame),       // main SDI frame window
		    RUNTIME_CLASS(CSchedulingManagerView));
        AfxGetApp()->AddDocTemplate(pDocTemplate);

        // Create the document.
        CCommandLineInfo cmdInfo;
        cmdInfo.m_nShellCommand = CCommandLineInfo::FileNew;

        if (!AfxGetApp()->ProcessShellCommand(cmdInfo))
        {
            // TODO: Throw exception
        }
        setApplicationWnd(*AfxGetMainWnd());
    }


    unsigned long SchedulingManagerGUI::getApplicationType()
    {
	    return SCHEDULING_MANAGER;
    }


    void SchedulingManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
	    setGuiEntity(*guiEntity);
    }


    void SchedulingManagerGUI::checkCommandLine()
    {  
    }


    void SchedulingManagerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        // The Scheduling Manager does not use any entity parameters, so don't need to do anything
    }


    void SchedulingManagerGUI::serverIsDown()
    {
        // when server is down, we want to disable all access related actions.  
        RunParams::getInstance().set(RPARAM_SERVERSTATUS.c_str(), RVALUE_SERVER_DOWN.c_str());
    }


    void SchedulingManagerGUI::serverIsUp()
    {
        // Tell the app that it could allow access related actions again.  
        RunParams::getInstance().set(RPARAM_SERVERSTATUS.c_str(), RVALUE_SERVER_UP.c_str());
    }


    void SchedulingManagerGUI::loadAllUserSettings()
    {
        // First check if we should be loading the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
           setWindowPosition();
           LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
           return;
        }

        try
        {
            if (getUserSetting() == NULL)
            {
                TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

                if (userSettings == NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
                }

                setUserSetting( *userSettings );            
            }

            loadPositionSettings();
        }
        catch (...)
        {
           setWindowPosition();
        }
    }


    void SchedulingManagerGUI::saveAllUserSettings()
    {
        // First check if we should be saving the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
            return;
        }

        if (getUserSetting() == NULL)
        {
            TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

            if (userSettings == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
            }

            setUserSetting( *userSettings );            
        }
    
        savePositionSettings();
    }


    void SchedulingManagerGUI::prepareForClose()
    {
        try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException&)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught some unknown exception when saving user settings");
        }

        try
        {
            // We want the scheduling manager to clean up any corba stuff before we terminate if it 
            // is terminated from control station.
            CWnd* mainFrm = getApplicationWnd();
            if (mainFrm != NULL)
            {
                mainFrm->PostMessage(WM_CLEAN_UP, 0, 0);
            }

        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown Exception", "Caught some unknown exception when cleaning up");
        }
    }


    void SchedulingManagerGUI::dutyChanged()
    {
        // don't have to do anything as all our controls are rights based, not 
        // duty based
    }
	//TD18095, jianghp
	void SchedulingManagerGUI::onInitGenericGUICompleted()
	{
		CWaitCursor cus;
		//do nothing, let the child class do something indeed.
		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();
		CMainFrame * pFrame = dynamic_cast<CMainFrame *>(pWnd);
		TA_ASSERT(pFrame != NULL, "the application main wnd is NULL!");
		pFrame->initJob();
		pWnd->UnlockWindowUpdate();
		cus.Restore();
		pWnd->UpdateWindow();
	};
};
