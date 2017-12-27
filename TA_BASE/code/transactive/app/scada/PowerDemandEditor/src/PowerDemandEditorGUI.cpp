/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/PowerDemandEditor/src/PowerDemandEditorGUI.cpp $
  * @author:  Andy Parker
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the managed process interface for the application.  This
  * handles the run parameters and the monitoring of changes passed from the control station.
  */


#include "app/scada/PowerDemandEditor/src/stdafx.h"

#include <sstream>
#include "tlhelp32.h"

#include "app/scada/PowerDemandEditor/src/PowerDemandEditorGUI.h"
#include "app/scada/PowerDemandEditor/src/resource.h"
#include "app/scada/PowerDemandEditor/src/PowerDemandEditorDlg.h"
#include "app/scada/PowerDemandEditor/src/PowerDemandEditor.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"



#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;
using namespace TA_Base_Bus;

namespace TA_Base_App
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    PowerDemandEditorGUI::PowerDemandEditorGUI()
    :
    AbstractDialogGUI(IDD_PWR_DMD_EDIT_DLG)
    {
    }


    PowerDemandEditorGUI::~PowerDemandEditorGUI()
    {
		FUNCTION_ENTRY ("~PowerDemandEditorGUI()");

		CWnd* wnd = getApplicationWnd();
		delete wnd;

        FUNCTION_EXIT;
    }


    void PowerDemandEditorGUI::createApplicationObject()
    {
        FUNCTION_ENTRY ("createApplicationObject()");

		// Create the dialog
		//TD18095, jianghp, to fix the performance of showing manager application
		CPowerDemandEditorDlg* dlg = new CPowerDemandEditorDlg(*this);
		setApplicationWnd(*dlg);

        FUNCTION_EXIT;
    }


   void PowerDemandEditorGUI::prepareForClose()
   {
		FUNCTION_ENTRY ("prepareForClose()");

		try
        {
            saveAllUserSettings();
        }
        catch ( TA_Base_Core::UserSettingsException& )
        {
            // Handle here with appropriate errror message
        }

        FUNCTION_EXIT;
    }

   void PowerDemandEditorGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY ("loadAllUserSettings()");

		// First check if we should be loading the user settings.
		if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
		   setWindowPosition();
           FUNCTION_EXIT;
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

        loadPositionSettings();

		FUNCTION_EXIT;
    }


    void PowerDemandEditorGUI::saveAllUserSettings()
    {
        FUNCTION_ENTRY ("saveAllUserSettings()");
    
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

        FUNCTION_EXIT;
    }


	unsigned long PowerDemandEditorGUI::getApplicationType()
    {
        FUNCTION_ENTRY ("getApplicationType()");
		FUNCTION_EXIT;

        return POWER_DEMAND_EDITOR_GUI_APPTYPE;
    }


	void PowerDemandEditorGUI::serverIsDown()
    {
        FUNCTION_ENTRY ("serverIsDown()");
		FUNCTION_EXIT;
    }


    void PowerDemandEditorGUI::serverIsUp()
    {
        FUNCTION_ENTRY ("serverIsUp()");
		FUNCTION_EXIT;
    }


	void PowerDemandEditorGUI::entityChanged(const std::vector<std::string>& changes)
	{
		FUNCTION_ENTRY ("entityChanged()");
		FUNCTION_EXIT;
	}


	void PowerDemandEditorGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		FUNCTION_ENTRY ("checkEntity()");
		FUNCTION_EXIT;
	}


	void PowerDemandEditorGUI::checkCommandLine()
	{
		FUNCTION_ENTRY ("checkCommandLine()");
		FUNCTION_EXIT;
	}
	//TD18095, jianghp
	void PowerDemandEditorGUI::onInitGenericGUICompleted()
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
}   // end namespace TA_Base_App
