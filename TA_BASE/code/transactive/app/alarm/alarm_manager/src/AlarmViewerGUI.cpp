/**
  * The source code in this file is the property of Ripple System
  * and is not for redistribution in any form. 
  *
  * Source: $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/AlarmViewerGUI.cpp $
  * @author: AndyS
  * @version:  $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by: $Author: builder $
  *
  * Implementation file for the Alarm Manager GUI application class.
  */


#pragma warning(disable:4541)
#pragma warning(disable:4786)

#include "stdafx.h"
#include "app\alarm\alarm_manager\src\Resource.h"

#include "app\alarm\alarm_manager\src\AlarmViewerGUI.h"
#include "app\alarm\alarm_manager\src\AlarmsGuiDlg.h"
#include "bus\alarm\alarm_list_control\src\Filter.h"

#include "core\data_access_interface\plans\src\AlarmPlanAssocAccessFactory.h"
#include "core\utilities\src\DebugUtil.h"
#include "core\utilities\src\RunParams.h"
#include "bus\user_settings\src\SettingsMgr.h"
#include "bus\application_types\src\apptypes.h"
#include "core\exceptions\src\UserSettingsException.h"

#include <sstream>

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

const std::string AM_SUBSYSTEM_FILTER = "AMSubsystemFilter";
const std::string AM_ALARMSTATE_FILTER = "AMAlarmStateFilter";
const std::string AM_LOCATION_FILTER = "AMLocationFilter";
const std::string AM_SEVERITY_FILTER = "AMSeverityFilter";
const std::string AM_ASSET_FILTER = "AMAssetFilter";
const std::string AM_ALARMTYPE_FILTER = "AMAlarmTypeFilter";
const std::string AM_OPERATOR_FILTER = "AMOperatorFilter";

namespace TA_Base_App
{
	AlarmViewerGUI::AlarmViewerGUI()
    :
    AbstractDialogGUI(IDD_ALARMSGUI_DIALOG)
	{

	}


	AlarmViewerGUI::~AlarmViewerGUI()
    {
            LOG (SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "~AlarmViewerGUI");

        try
        {
			CWnd* application = getApplicationWnd();
            if (application != NULL)
            {
                delete application;
            }
        }
        catch( ... )
        {

        }

            LOG (SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "~AlarmViewerGUI");
    }


    void AlarmViewerGUI::createApplicationObject()
    {
        CAlarmsGuiDlg *pDlg = new CAlarmsGuiDlg(this);
		setApplicationWnd(*pDlg);
    }


	unsigned long AlarmViewerGUI::getApplicationType()
	{
		return ALARM_GUI_APPTYPE;
	}


	void AlarmViewerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
	{
		setGuiEntity(*guiEntity);

		//TO DO: Check the config of the entity is valid. Throw the specified 
		// exception if an error is found
	}	


	void AlarmViewerGUI::checkCommandLine()
	{
		//TO DO: Check any command line arguments specific to this application are 
		//available and correct. Throw specified exception if they are not.
	}


	void AlarmViewerGUI::entityChanged(const std::vector<std::string>& changes)
	{
		//TO DO: check the chagnes passed and pass any relevant information on to the
		// dialog so it can update
	}


	void AlarmViewerGUI::serverIsDown()
	{
		//TO DO: get the GUI to update to indicate this
			LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "serverIsDown" );
        dynamic_cast <CAlarmsGuiDlg*> (getApplicationWnd())->serverDown();

            LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "serverIsDown" );
	}


	void AlarmViewerGUI::serverIsUp()
	{
		//TO DO: Get the GUI to update to indicate this
			LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionEntry, "serverIsUp" );
        dynamic_cast <CAlarmsGuiDlg*> (getApplicationWnd())->serverUp();
            LOG ( SourceInfo, TA_Base_Core::DebugUtil::FunctionExit, "serverIsUp" );

	}


	void AlarmViewerGUI::loadAllUserSettings()
	{
		// First check if we should be loading the user settings.
		if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
		   setWindowPosition();
		   LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
		   return;
		}

		if (getUserSetting() == NULL)
		{
			TA_Base_Bus::SettingsMgr* userSettings = 
				new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType());
			if (userSettings == NULL)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
				"new on SettingsMgr returned NULL. Cannot load user settings");
				return;
			}
			setUserSetting(*userSettings);
		}

		loadPositionSettings();
	}

	void AlarmViewerGUI::saveAllUserSettings()
	{
		// First check if we should be saving the user settings.
		if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
		{
			return;
		}

		if (getUserSetting() == NULL)
		{
			TA_Base_Bus::SettingsMgr* userSettings = 
				new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType());
			if (userSettings == NULL)
			{
				LOG( SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugWarn, 
				"new on SettingsMgr returned NULL. Cannot load user settings");
				return;
			}
			setUserSetting(*userSettings);
		}
    
		savePositionSettings();
	}


	void AlarmViewerGUI::prepareForClose()
	{
		try
		{
			saveAllUserSettings();
		}
		catch ( TA_Base_Core::UserSettingsException&)
		{
			//TO DO: Handle here with appropriate error message.
		}

        TA_Base_Core::AlarmPlanAssocAccessFactory::getInstance().cleanUp();
	}
	//TD18095, jianghp
	void AlarmViewerGUI::onInitGenericGUICompleted()
	{
		CWaitCursor cur;
		CWnd * pWnd = getApplicationWnd();
		CAlarmsGuiDlg * pDlg = dynamic_cast<CAlarmsGuiDlg * >(pWnd);
		pWnd->LockWindowUpdate();
		TA_ASSERT(pDlg != NULL, "the main Dlg is null!");
		pDlg->init();
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();
	};

}   // end namespace TA_Base_App
