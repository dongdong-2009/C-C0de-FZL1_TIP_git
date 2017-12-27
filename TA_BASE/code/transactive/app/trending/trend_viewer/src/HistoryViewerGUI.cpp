#ifndef HistoryViewerGUI_CPP
#define HistoryViewerGUI_CPP

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/HistoryViewerGUI.cpp $
  * @author:  A. Lee
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the main control class.
  *
  */

#include <string>

#include "app/trending/trend_viewer/src/stdafx.h"
#include "app/trending/trend_viewer/src/HistoryViewerGUI.h"
#include "app/trending/trend_viewer/src/HistoryViewerDlg.h"
#include "app/trending/trend_viewer/src/globals.h"

#include "bus/application_types/src/AppTypes.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/exceptions/src/GenericGUIException.h"
#include "core/exceptions/src/DataException.h"
#include "core/data_access_interface/entity_access/src/HistoryViewerData.h"


using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using TA_Base_Core::UserSettingsException;
using TA_Base_Bus::SettingsMgr;
using TA_Base_Core::GenericGUIException;
using TA_Base_Core::HistoryViewerData;
using TA_Base_Core::DataException;
using namespace TA_Base_Bus;

namespace TA_Base_App
{

    //
    // HistoryViewerGUI
    //
    HistoryViewerGUI::HistoryViewerGUI()
    :
    AbstractDialogGUI(IDD_HISTORYVIEWER_DIALOG)
    {
    }


    //
    // ~HistoryViewerGUI
    //
    HistoryViewerGUI::~HistoryViewerGUI()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "~HistoryViewerGUI" );

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
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError, 
                "An error occurred upon shutdown");
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "~HistoryViewerGUI" );
    }


    //
    // createApplicationObject
    //
    void HistoryViewerGUI::createApplicationObject()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "createApplicationObject" );

		//TD18095, jianghp, to fix the performance of showing manager application
		CHistoryViewerDlg *pDlg = new CHistoryViewerDlg(this);
		//TD18095, jianghp, to fix the performance of showing manager application
        setApplicationWnd(*pDlg);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "createApplicationObject" );
    }


    //
    // loadAllUserSettings
    //
    void HistoryViewerGUI::loadAllUserSettings()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "loadAllUserSettings" );

        TA_Base_Bus::SettingsMgr* userSettings = getUserSetting();
        if (userSettings == NULL)
        {
            setUserSetting(*(new TA_Base_Bus::SettingsMgr(getSessionId(), getApplicationType())));
        }

        // Restore any other settings here

        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            setWindowPosition();
            LOG( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );            
            return;
        }
         
        loadPositionSettings();

        LOG ( SourceInfo, DebugUtil::FunctionExit, "loadAllUserSettings" );
    }


    //
    // saveAllUserSettings
    //
    void HistoryViewerGUI::saveAllUserSettings()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "saveAllUserSettings" );
 
        TA_Base_Bus::SettingsMgr* userSettings = getUserSetting();
        if (userSettings == NULL)
        {
            setUserSetting(*(new SettingsMgr(getSessionId(), getApplicationType())));
        }

        // Save any other settings here
        std::map<std::string, std::string> userSettingsMap;
        CWnd* application = getApplicationWnd();

        std::map<std::string, std::string>::iterator it = userSettingsMap.begin();
        for(; it != userSettingsMap.end(); ++it)
        {
            userSettings->saveSetting(it->first, it->second);
        }

        if (RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty())
        {
            return;
        }
        
        savePositionSettings();

        LOG( SourceInfo, DebugUtil::FunctionExit, "saveAllUserSettings" );
    }


    //
    // getApplicationType
    //
    unsigned long HistoryViewerGUI::getApplicationType()
    {
        LOG( SourceInfo, DebugUtil::FunctionEntry, "getApplicationType" );

        LOG( SourceInfo, DebugUtil::FunctionExit, "getApplicationType" );

        return TREND_VIEWER_GUI_APPTYPE;
    }

    
    //
    // checkEntity
    //
    void HistoryViewerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "checkEntity" );
        
        setGuiEntity(*guiEntity);

        LOG ( SourceInfo, DebugUtil::FunctionExit, "checkEntity" );
    }


    //
    // checkCommandLine
    //
    void HistoryViewerGUI::checkCommandLine()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "checkCommandLine" );

        //
        // GraphType - optional
        //
        std::string graphType = RunParams::getInstance().get("GraphType");
        if(graphType.empty() == false)
        {
            if(graphType != "BAR" && graphType != "LINE" && graphType != "COMPARISON" && graphType != "XY")
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: GraphType", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }

        //
        // SamplePeriod - optional
        //
        std::string samplePeriod = RunParams::getInstance().get("SamplePeriod");
        if(samplePeriod.empty() == false)
        {
            if(samplePeriod.find_first_not_of("0123456789,") != std::string::npos)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: SamplePeriod", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }


        //
        // Range - optional
        //
        std::string range = RunParams::getInstance().get("Range");
        if(range.empty() == false)
        {
            if(range.find_first_not_of("0123456789,:") != std::string::npos)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: Range", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }

            int comma = range.find_first_of(",");
            if(comma == -1)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: Range", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }

            int colon = range.find_first_of(":");
            if(colon == -1)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: Range", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }

        //
        // Offset - optional
        //
        std::string offset = RunParams::getInstance().get("Offset");
        if(offset.empty() == false)
        {
            if(range.find_first_not_of("0123456789") != std::string::npos)
            {
                TA_THROW( GenericGUIException("Invalid commandline parameter: Offset", 
                    GenericGUIException::COMMAND_LINE_INVALID ));
            }
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "checkCommandLine" );
    }


    //
    // entityChanged
    //
    void HistoryViewerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "entityChanged" );

        LOG ( SourceInfo, DebugUtil::FunctionExit, "entityChanged" );
    }


    //
    // serverIsDown
    //
    void HistoryViewerGUI::serverIsDown()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "serverIsDown" );

        // 
        // Get the GUI to update to indicate this.
        //
        try
        {
            CHistoryViewerDlg* theDlg = dynamic_cast <CHistoryViewerDlg*> (getApplicationWnd());
            if(theDlg != NULL)
            {
                theDlg->serverDown();
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error occurred while attempting dynamic cast");
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "serverIsDown" );
    }


    //
    // serverIsUp
    //
    void HistoryViewerGUI::serverIsUp()
    {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "serverIsUp" );

        // 
        // Get the GUI to update to indicate this.
        //
        try
        {
            dynamic_cast <CHistoryViewerDlg*> (getApplicationWnd())->serverUp();
            CHistoryViewerDlg* theDlg = dynamic_cast <CHistoryViewerDlg*> (getApplicationWnd());
            if(theDlg != NULL)
            {
                theDlg->serverUp();
            }
        }
        catch(...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown", "Unknown");
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Error occurred while attempting dynamic cast");
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "serverIsUp" );
    }


    //
    // prepareForClose
    //
   void HistoryViewerGUI::prepareForClose()
   {
        LOG ( SourceInfo, DebugUtil::FunctionEntry, "prepareForClose" );

        try
        {
            saveAllUserSettings();
        }
        catch ( UserSettingsException& )
        {
            LOG(SourceInfo, DebugUtil::GenericLog, DebugUtil::DebugError,
                "Could not save user settings.");
        }

        LOG ( SourceInfo, DebugUtil::FunctionExit, "prepareForClose" );
    }

   //TD18095, jianghp
   void HistoryViewerGUI::onInitGenericGUICompleted()
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

#endif // HistoryViewerGUI_CPP
