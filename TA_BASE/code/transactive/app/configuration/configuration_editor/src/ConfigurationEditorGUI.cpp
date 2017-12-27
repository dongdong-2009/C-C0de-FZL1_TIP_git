/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorGUI.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This is the ConfigurationEditor implementation of the GenericGUI IGUIApplication and IGUIAccess
  * interfaces.
  */
#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorGUI.h"
#include "app/configuration/configuration_editor/src/resource.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorDoc.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorView.h"
#include "app/configuration/configuration_editor/src/MainFrm.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorSelectionBar.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorSessionUpdater.h"
#include "bus/generic_gui/src/TransActiveFrame.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/application_types/src/apptypes.h"
#include "bus/user_settings/src/SettingsMgr.h"
#include "core/data_access_interface/entity_access/src/ConfigurationEditor.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"

#ifdef _DEBUG
    #undef THIS_FILE
    static char THIS_FILE[] = __FILE__;
    #define new DEBUG_NEW
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::SettingsMgr;

namespace TA_Base_App
{
    //TD10758 marc_bugfix
    UINT WorkerThreadConfigSessionUpdater(LPVOID Param)
    {
        ConfigurationEditorSessionUpdater cSessionUpdater;

        for (;;)
        {
            //For now, update the timestamp every 5 seconds
            if (!cSessionUpdater.updateConfigSessionTimeStamp())
            {
                break;
            }

            Sleep(5000);
        }

        AfxEndThread(0) ;

        return 0;
    }
    // This is the name of the user setting to load
    const std::string ConfigurationEditorGUI::ICON_SIZE_USER_SETTING = "IconSize";

    //////////////////////////////////////////////////////////////////////
    // Construction/Destruction
    //////////////////////////////////////////////////////////////////////

    ConfigurationEditorGUI::ConfigurationEditorGUI()
    {
    }

    ConfigurationEditorGUI::~ConfigurationEditorGUI()
    {
    }

    void ConfigurationEditorGUI::createApplicationObject()
    {
        FUNCTION_ENTRY("createApplicationObject");

        CSingleDocTemplate* pDocTemplate;
        pDocTemplate = new CSingleDocTemplate(
            IDR_MAINFRAME,
            RUNTIME_CLASS(CConfigurationEditorDoc),
            RUNTIME_CLASS(CMainFrame),       // main SDI frame window
            RUNTIME_CLASS(CConfigurationEditorView));
        AfxGetApp()->AddDocTemplate(pDocTemplate);

        // Parse command line for standard shell commands, DDE, file open
        CCommandLineInfo cmdInfo;
        AfxGetApp()->ParseCommandLine(cmdInfo);

        // Dispatch commands specified on the command line
        if (!AfxGetApp()->ProcessShellCommand(cmdInfo))
        {
            TA_THROW(TA_Base_Core::GenericGUIException(TA_Base_Core::GenericGUIException::INITIALISATION_ERRORS));
        }

        setApplicationWnd(*AfxGetMainWnd());

        FUNCTION_EXIT;
    }

    unsigned long ConfigurationEditorGUI::getApplicationType()
    {
        return CONFIG_EDITOR_GUI_APPTYPE;
    }

    void ConfigurationEditorGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        FUNCTION_ENTRY("checkEntity");

        TA_ASSERT(guiEntity != NULL, "The IEntityData pointer passed in was NULL");

        setGuiEntity(*guiEntity);

        TA_Base_Core::ConfigurationEditor* configEditor = dynamic_cast<TA_Base_Core::ConfigurationEditor*>(guiEntity);

        if (configEditor == NULL)
        {
            TA_THROW(TA_Base_Core::GenericGUIException("Configuration Editor entity passed in was not of the type 'ConfigurationEditor'",
                     TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID));
        }

        if (RunParams::getInstance().isSet(TA_Configuration::RPARAM_DO_NOT_CHECK_ALREADY_RUNNING.c_str()))
        {
            return;
        }

        std::string workstationName;

        if (configEditor->isConfigurationEditorAlreadyRunning(workstationName))
        {
            CString applicationName = TA_Base_Core::RunParams::getInstance().get(RPARAM_APPNAME).c_str();

            if (applicationName.IsEmpty())
            {
                applicationName.LoadString(IDS_APPNAME);
            }

            UINT selectedButton = showMsgBox(IDS_UE_030061, applicationName, workstationName);

            TA_THROW(TA_Base_Core::ProcessAlreadyRunningException("Configuration Editor must be terminated as an instance of it is already connected to this schema"));
        }

        //TD10758 marc_bugfix
        //Start worker thread that will update config editor session timestamp

        AfxBeginThread(WorkerThreadConfigSessionUpdater, NULL, THREAD_PRIORITY_NORMAL, 0, 0, NULL);

        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::checkCommandLine()
    {
        FUNCTION_ENTRY("checkCommandLine");
        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::entityChanged(const std::vector<std::string>& changes)
    {
        //TODO: This will be called by GenericGUI when it receives a callback
        //indicating that the GUI entity has been modified. GenericGUI will have
        //invalidated the entity database object and will tell the GUI which
        //items were modified. This method just has to retrieve the details out
        //of it and update accordingly. If the details in it are invalid then this
        //should print a warning message to the user and continue to use the old settings (ie do not update the GUI).
        FUNCTION_ENTRY("entityChanged");
        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::serverIsDown()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.
        FUNCTION_ENTRY("serverIsDown");
        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::serverIsUp()
    {
        // TODO: This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.
        FUNCTION_ENTRY("serverIsUp");
        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY("loadAllUserSettings");

        if (getUserSetting() == NULL)
        {
            TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

            if (userSettings == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
                FUNCTION_EXIT;
            }

            setUserSetting(*userSettings);
        }

        //
        // Load the icon size this user prefers
        //
        unsigned long iconSize = TA_Configuration::LARGE_ICON_SIZE; // Default size in case we cannot load one

        try
        {
            iconSize = getUserSetting()->getSettingAsLong(ICON_SIZE_USER_SETTING);
        }
        catch (const TA_Base_Core::UserSettingsException&)
        {
            LOG(SourceInfo, DebugUtil::ExceptionCatch, "UserSettingsException", "Cannot load the user preference for icon size");
        }

        // Now let the main frame know about the icon size
        CMainFrame* configurationEditorFrame = dynamic_cast<CMainFrame*>(getApplicationWnd());

        if (configurationEditorFrame != NULL)
        {
            ConfigurationEditorSelectionBar* selectionBar = configurationEditorFrame->getSelectionBar();

            if (selectionBar != NULL)
            {
                selectionBar->setIconSizeForSelectionBar(iconSize);
            }
        }

        //
        // Check if we should be loading the user position preferences.
        //
        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            setWindowPosition();
            LOG(SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings");
            return;
        }

        loadPositionSettings();

        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::saveAllUserSettings()
    {
        FUNCTION_ENTRY("saveAllUserSettings");

        if (getUserSetting() == NULL)
        {
            TA_Base_Bus::SettingsMgr* userSettings = new SettingsMgr(getSessionId(), getApplicationType());

            if (userSettings == NULL)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "new on SettingsMgr returned NULL. Cannot load user preferences");
                FUNCTION_EXIT;
            }

            setUserSetting(*userSettings);
        }

        //
        // Save the icon size this user prefers
        //

        // Get the icon size from the main frame
        CMainFrame* configurationEditorFrame = dynamic_cast<CMainFrame*>(getApplicationWnd());

        if (configurationEditorFrame != NULL)
        {
            ConfigurationEditorSelectionBar* selectionBar = configurationEditorFrame->getSelectionBar();

            if (selectionBar != NULL)
            {
                unsigned long iconSize = selectionBar->getIconSizeForSelectionBar();

                try
                {
                    getUserSetting()->saveSetting(ICON_SIZE_USER_SETTING, iconSize);
                }
                catch (const TA_Base_Core::UserSettingsException&)
                {
                    LOG(SourceInfo, DebugUtil::ExceptionCatch, "UserSettingsException", "Cannot save the user preference for icon size");
                }
            }
        }

        //
        // Check if we should be saving the user settings for position and size
        //
        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            return;
        }

        savePositionSettings();

        FUNCTION_EXIT;
    }

    void ConfigurationEditorGUI::prepareForClose()
    {
        FUNCTION_ENTRY("prepareForClose");

        //This provides a standard method for each GUI to use to clean itself
        //up ready to exit. This will be called whenever the application exits whether
        //it is closed by the Control Station or by the user. This should perform
        //tasks such as saving user settings, stopping any threads and asking the user
        //if they want to save their settings. When this method is completed the GUI
        //should be in a safe state to be closed.

        try
        {
            saveAllUserSettings();
        }
        catch (TA_Base_Core::UserSettingsException&)
        {
            // Handle here with appropriate errror message
        }

        // Perform any shutdown tasks here

        FUNCTION_EXIT;
    }

    //TD18095, jianghp
    void ConfigurationEditorGUI::onInitGenericGUICompleted()
    {
        CWnd* pWnd = getApplicationWnd();
        CMainFrame* pFrame =  dynamic_cast<CMainFrame* >(pWnd);
        TA_ASSERT(pFrame != NULL, "the app main frame is NULL!");
        pFrame->LockWindowUpdate();
        pFrame->init();
        pFrame->UnlockWindowUpdate();
    }
}
