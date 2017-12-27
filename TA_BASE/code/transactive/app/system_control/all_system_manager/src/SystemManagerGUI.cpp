/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemManagerGUI.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  */
#include "StdAfx.h"
#include "SystemManagerGUI.h"
#include "SystemManagerDlg.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/UserSettingsException.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Bus::SettingsMgr;
using namespace TA_Base_Bus;

namespace TA_Base_App
{

    SystemManagerGUI::SystemManagerGUI()
    :
    AbstractDialogGUI(IDD_SYSTEMMANAGER_DIALOG)
    {
    }

    SystemManagerGUI::~SystemManagerGUI()
    {
    }

    void SystemManagerGUI::createApplicationObject()
    {
        // Construct the dialog
        SystemManagerDlg* dialog = new SystemManagerDlg(this);

        // And now set the dialog object to the stored applicaiton object
        setApplicationWnd(*dialog);
    }

    unsigned long SystemManagerGUI::getApplicationType()
    {
	    return ADMIN_GUI_APPTYPE;
    }

    void SystemManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        setGuiEntity(*guiEntity);
    }

    void SystemManagerGUI::checkCommandLine()
    {
    }

    void SystemManagerGUI::entityChanged(const std::vector<std::string>& changes)
    {
    }

    void SystemManagerGUI::serverIsDown()
    {
    }

    void SystemManagerGUI::serverIsUp()
    {
    }

    void SystemManagerGUI::prepareForClose()
    {
        try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException& )
        {
            // Handle here with appropriate log message
        }
    }

    void SystemManagerGUI::loadAllUserSettings()
    {
        // First check if we should be loading the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).
                  empty() )
        {
           setWindowPosition();
           LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
           return;
        }

        if (getUserSetting() == NULL)
        {
            setUserSetting(*(new SettingsMgr(getSessionId(), getApplicationType())));
        }

        loadPositionSettings();
    }

    void SystemManagerGUI::saveAllUserSettings()
    {
        // First check if we should be saving the user settings.
        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
            return;
        }

        if (getUserSetting() == NULL)
        {
            setUserSetting(*new SettingsMgr(getSessionId(), getApplicationType()));
        }
    
        savePositionSettings();
        // TODO: Save any other settings here
    }


	//TD18095, jianghp
	void SystemManagerGUI::onInitGenericGUICompleted()
	{
		CWaitCursor cur;
		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();
		TransActiveDialog * pDlg = dynamic_cast<TransActiveDialog * >(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");
		pDlg->init();
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();
	}
    
    void SystemManagerGUI::setRunWithoutControlStation()
    {
        CWnd *pWnd = getApplicationWnd();
        TransActiveDialog * pDlg = dynamic_cast<TransActiveDialog * >(pWnd);
		if ( NULL != pDlg )
        {
//             pDlg->setRunWithoutControlStation();
		}
    }
} // namespace TA_Base_App
