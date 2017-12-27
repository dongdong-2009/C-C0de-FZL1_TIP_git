/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Andy Parker
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class provides the managed process interface for the application.  This
  * handles the run parameters and the monitoring of changes passed from the control station.
  */


#include "app/scada/Inspector_Panel/src/stdafx.h"

#include <sstream>
#include "tlhelp32.h"

#include "app/scada/Inspector_Panel/src/InfoTabHelper.h"
#include "app/scada/Inspector_Panel/src/InspectorPanelGUI.h"
#include "app/scada/Inspector_Panel/src/resource.h"
#include "app/scada/Inspector_Panel/src/InspectorPanelDlg.h"
#include "app/scada/Inspector_Panel/src/InspectorPanel.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/entity_access/src/InspectorPanelEntityData.h"
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

namespace TA_Base_App
{
	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////

    InspectorPanelGUI::InspectorPanelGUI()
    :
    AbstractDialogGUI(IDD_INSPECTOR_PANEL_DIALOG),
	m_maxInstances(1)
    {
    }


    InspectorPanelGUI::~InspectorPanelGUI()
    {
		FUNCTION_ENTRY ("~InspectorPanelGUI()");

		CWnd* wnd = getApplicationWnd();

		//zn++ - IP will crash when closed
		if (wnd != NULL)
		{
			delete wnd;
		}
		//++zn
		exit(0);
        FUNCTION_EXIT;
    }


    void InspectorPanelGUI::createApplicationObject()
    {
        FUNCTION_ENTRY ("createApplicationObject()");

		// check if MaxInstances is set as a runtime parameter

		/*
		if ( 0 < m_maxInstances )
		{
			// get the number of InspectorPanel processes currently running,
			// including this process
			if ( m_maxInstances < findNumberOfInspectorPanelInstances() )
			{
				// Display message that the maximum number has been exceeded.
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << m_maxInstances;
				UINT selectedButton = userMsg.showMsgBox(IDS_UW_040007);

				// Kill the current instance without generating an error. The user has already been notified above.
				TA_THROW(TA_Base_Core::ProcessAlreadyRunningException("There are already a maximum instances of InspectorPanel application running within this Windows system.\n"));
			}
		
		}
		*/

		// Create the dialog
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "InspectorPanelGUI::createApplicationObject new CInspectorPanelDlg ");
		CInspectorPanelDlg* dlg = new CInspectorPanelDlg(*this);
		setApplicationWnd(*dlg);
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "InspectorPanelGUI::createApplicationObject after setApplicationWnd ");
        FUNCTION_EXIT;
    }


	//TD18095, jianghp
	void InspectorPanelGUI::onInitGenericGUICompleted()
	{
		//TD18310, wenching
		if ( 0 < m_maxInstances )
		{
			// get the number of InspectorPanel processes currently running,
			// including this process
			if ( m_maxInstances < findNumberOfInspectorPanelInstances() )
			{
				// Display message that the maximum number has been exceeded.
				TA_Base_Bus::TransActiveMessage userMsg;
				userMsg << m_maxInstances;
				UINT selectedButton = userMsg.showMsgBox(IDS_UW_040007, "¼à¿ØÃæ°å");

				CWnd* wnd = getApplicationWnd();
				TA_ASSERT( wnd != NULL, "main dialog should be created");
				delete wnd;
				//zn++ - do this avoid delete again when dtor is called
				wnd = NULL;
				setApplicationWnd(NULL);
				//++zn

				// Kill the current instance without generating an error. The user has already been notified above.
				TA_THROW(TA_Base_Core::ProcessAlreadyRunningException("There are already a maximum instances of InspectorPanel application running within this Windows system.\n"));
			}
		
		}

		CWaitCursor cur;
		CWnd *pWnd = getApplicationWnd();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "InspectorPanelGUI::onInitGenericGUICompleted before LockWindowUpdate ");
		pWnd->LockWindowUpdate();		
		CInspectorPanelDlg * pDlg = dynamic_cast<CInspectorPanelDlg * >(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");
		// TD18728, Since "ShowQuality" and "ShowTimestamp" was obtained after the list created, 
		// reset the InformatianList to shown the options correctly.
		pDlg->resetList();
		pDlg->init();
		pDlg->UnlockWindowUpdate();
		LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "InspectorPanelGUI::onInitGenericGUICompleted aft UnlockWindowUpdate ");
		//pDlg->UpdateWindow();	//above unlockwindowsupdate will automatically invalidate the window and cause the window repaint.
	}


	unsigned long InspectorPanelGUI::getApplicationType()
    {
        FUNCTION_ENTRY ("getApplicationType()");
		FUNCTION_EXIT;

        return INSPECTOR_PANEL_GUI_APPTYPE;
    }

	void InspectorPanelGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        FUNCTION_ENTRY ("checkEntity()");

		setGuiEntity(*guiEntity);

		// wenching++ (TD12997)
		TA_Base_Core::InspectorPanelEntityData* InspectorEntityAccess = dynamic_cast<TA_Base_Core::InspectorPanelEntityData*>(guiEntity);

		m_maxInstances = InspectorEntityAccess->getMaxInstances();
		InfoTabHelper::setShowQuality( InspectorEntityAccess->getShowQuality() );
		InfoTabHelper::setShowTimestamp( InspectorEntityAccess->getShowTimestamp() );
		// ++wenching (TD12997)

		FUNCTION_EXIT;
    }

	void InspectorPanelGUI::checkCommandLine()
    {
        FUNCTION_ENTRY ("checkCommandLine()");
		
        if (!TA_Base_Core::RunParams::getInstance().isSet(EQUIPMENT_RPARAM))
		{
			std::string  details = "Parameter missing - ";
			details +=  EQUIPMENT_RPARAM;				
			TA_THROW(TA_Base_Core::GenericGUIException(details.c_str(),TA_Base_Core::GenericGUIException::COMMAND_LINE_INVALID));
		}

        FUNCTION_EXIT;
    }


    void InspectorPanelGUI::entityChanged(const std::vector<std::string>& changes)
    {
        FUNCTION_ENTRY ("entityChanged()");
		FUNCTION_EXIT;
    }


    void InspectorPanelGUI::serverIsDown()
    {
        FUNCTION_ENTRY ("serverIsDown()");
		FUNCTION_EXIT;
    }


    void InspectorPanelGUI::serverIsUp()
    {
        FUNCTION_ENTRY ("serverIsUp()");
		FUNCTION_EXIT;
    }


    void InspectorPanelGUI::loadAllUserSettings()
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


    void InspectorPanelGUI::saveAllUserSettings()
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


   void InspectorPanelGUI::prepareForClose()
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
		catch (...)
		{
			// 2008-07-17 other exceptions?
			NULL;
		}

		
		CWnd* wnd = getApplicationWnd();

		try
		{
			CInspectorPanelDlg* dlg = dynamic_cast<CInspectorPanelDlg*>(wnd);

			if (dlg != NULL)
			{
				TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(dlg);

				// wenching++ (TD12597)
				//dlg->destroyEquipment();				
				// ++wenching (TD12597)
			}
		}
		catch (...)
		{
		}

        FUNCTION_EXIT;
    }


	bool InspectorPanelGUI::findInspectorPanelSubstringInExecutableName ( const std::string & execName ) const
	{
		bool substringFound = false;

		// find the sub-string "InspectorPanel"
		int pos = 0;
		pos = execName.find ( "InspectorPanel", 0 );

		// if found the sub-string
		if ( pos > -1 )
		{
			// set the flag
			substringFound = true;
		}
		// else do nothing

		return substringFound;
	}


	int InspectorPanelGUI::findNumberOfInspectorPanelInstances() const
	{
		int numberOfInspectorPanelInstances = 0;

		HANDLE handle;
		PROCESSENTRY32 process32Entry;
		process32Entry.dwSize = sizeof ( PROCESSENTRY32 );

		// if failed to take a snapshot of the running processes within the system
		if ( ! ( handle = CreateToolhelp32Snapshot ( TH32CS_SNAPPROCESS, 0 ) ) )
		{
			numberOfInspectorPanelInstances = 0;
		}
		else
		{
			// if sucessful to access the process info of the first process
			// in the snapshot table
			if ( Process32First ( handle, &process32Entry ) )
			{
				// if the name of the exe associated with the running process
				// has the "InspectorPanel" sub-string
				if ( true == findInspectorPanelSubstringInExecutableName ( process32Entry.szExeFile ) )
				{
					// increment the counter
					numberOfInspectorPanelInstances++;
				}

				// if successful to access the process info of the next
				// running process in the snapshot table
				while ( Process32Next ( handle, &process32Entry ) )
				{
					// if the name of the exe associated with the running process
					// has the "InspectorPanel" sub-string
					if ( true == findInspectorPanelSubstringInExecutableName ( process32Entry.szExeFile ) )
					{
						// increment the counter
						numberOfInspectorPanelInstances++;
					}
				}
			}

			// don't need the handle to the system snapshot anymore
			CloseHandle ( handle );
		}

		return numberOfInspectorPanelInstances;
	}
}   // end namespace TA_Base_App
