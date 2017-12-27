/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/ArchiveManagerGUI.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Handles the application specifics for the items
  * handled by GenericGUI including user settings, online updates...
  * Inherits from TA_Base_Bus::AbstractDialogGUI.
  */

#include <sstream>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/ArchiveManagerGUI.h"
#include "app/data_management/archive_manager/src/resource.h"
#include "app/data_management/archive_manager/src/ArchiveManagerDlg.h"
#include "app/data_management/archive_manager/src/BasicHelper.h"
#include "app/data_management/archive_manager/src/ChineseInfo.h"

#include "bus/application_types/src/apptypes.h"
#include "bus/generic_gui/src/TransactiveMessage.h"
#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/user_settings/src/SettingsMgr.h"

#include "core/data_access_interface/entity_access/src/ArchiveManagerEntityData.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/exceptions/src/UserSettingsException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/GenericGuiException.h"
#include "core/exceptions/src/ProcessAlreadyRunningException.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

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
    
    ArchiveManagerGUI::ArchiveManagerGUI()
    :
    AbstractDialogGUI(IDD_ARCHIVEMANAGER_DIALOG)
    {   
        // Do nothing.
    }
    
    
    ArchiveManagerGUI::~ArchiveManagerGUI()
    {
        // Do nothing.
    }
    
    
    void ArchiveManagerGUI::createApplicationObject()
    {
        // The delete for this is handled by GenericGUI.
		CArchiveManagerDlg *pDlg = new CArchiveManagerDlg(*this);
        setApplicationWnd(*pDlg);
    }
    
    
    unsigned long ArchiveManagerGUI::getApplicationType()
    {
        return ARCHIVE_MANAGER_APPTYPE;
    }
    
    
    void ArchiveManagerGUI::checkEntity(TA_Base_Core::IEntityData* guiEntity)
    {
        // KT 3 Mar 04: PW #3136: Added to ensure only one instance of the Archive Manager is running at once.
        // Note: This only ensures that there is one instance connected to the schema specified by the
        // DbConnection runparam.  It cannot ensure that there is only one instance running system wide, if
        // multiple schemas are available.  However, this situation is unlikely.

        FUNCTION_ENTRY( "checkEntity" );

        TA_ASSERT( guiEntity != NULL, "The IEntityData pointer passed in was NULL" );
        
        setGuiEntity(*guiEntity);
        
        TA_Base_Core::ArchiveManagerEntityData* entityData = dynamic_cast< TA_Base_Core::ArchiveManagerEntityData* >( guiEntity );
        if ( 0 == entityData )
        {
            CString message;
            message.FormatMessage( UE050125_INVALID_ENTITY, TA_Base_Core::ArchiveManagerEntityData::getStaticType().c_str() );
            TA_THROW( TA_Base_Core::GenericGUIException( std::string( message ), TA_Base_Core::GenericGUIException::ENTITY_CONFIGURATION_INVALID) );
        }

        std::string workstationName( "" );
        if ( entityData->isArchiveManagerAlreadyRunning( workstationName ) )
        {
			CString applicationName;
			applicationName.LoadString( IDS_ARCHIVEMANAGER );
          /*  CString applicationName( TA_Base_Core::RunParams::getInstance().get( RPARAM_APPNAME ).c_str() );
            if ( applicationName.IsEmpty() )
            {
                applicationName.LoadString( IDS_ARCHIVEMANAGER );
            }*/

            CString reason;
            if ( workstationName.empty() )
            {
                reason.FormatMessage( UE050108_REASON_DB_FAILED, applicationName );
            }
            else
            {
                reason.FormatMessage( UE050108_REASON_RUNNING, applicationName, workstationName.c_str() );
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << applicationName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_050108,CAPTION_ARCHIVE_MANAGER);
            
			//::SendMessage(AfxGetMainWnd()->m_hWnd, WM_CLOSE, 0, 0);
			exit(0);
            //TA_THROW( TA_Base_Core::ProcessAlreadyRunningException( "The Archive Manager must be terminated as an instance of it is already connected to this schema." ) );
        }

        FUNCTION_EXIT;
    }
    
    
    void ArchiveManagerGUI::checkCommandLine()
    {  
        // Do nothing.
    }
    
    
    void ArchiveManagerGUI::entityChanged(const std::vector<std::string>& changes)
    {
        // None of the parameters for the Archive Manager entity are online-updatable,
        // so we don't need to do anything here.
    }
    
    
    void ArchiveManagerGUI::serverIsDown()
    {
        // This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.

        if ( getApplicationWnd() != 0 )
        {
            CArchiveManagerDlg* dlg = dynamic_cast< CArchiveManagerDlg* >( getApplicationWnd() );
            if ( dlg == 0 )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to update the server status because the dynamic cast failed." );
            }
            else
            {
                dlg->setServerStatus( false );
            }
        }
    }
    
    
    void ArchiveManagerGUI::serverIsUp()
    {
        // This method is used to ensure the GUI can handle when the server goes
        //up or down. The GUI should be updated to indicate the network status.
        //If the server has gone down it should enter some sort of read-only mode and
        //if the server comes back up it can restore to the correct operating mode.

        if ( getApplicationWnd() != 0 )
        {
            CArchiveManagerDlg* dlg = dynamic_cast< CArchiveManagerDlg* >( getApplicationWnd() );
            if ( dlg == 0 )
            {
                LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugError, 
                    "Failed to update the server status because the dynamic cast failed." );
            }
            else
            {
                dlg->setServerStatus( true );
            }
        }
    }
    
    
    void ArchiveManagerGUI::loadAllUserSettings()
    {
        FUNCTION_ENTRY( "loadAllUserSettings" );

        // First check if we should be loading the user settings.

        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
            setWindowPosition();
            return;
        }
        
        if (getUserSetting() == 0)
        {
            setUserSetting(*(new SettingsMgr(getSessionId(), getApplicationType() )));
        }
        
        loadPositionSettings();
    }
    
    
    void ArchiveManagerGUI::saveAllUserSettings()
    {
        FUNCTION_ENTRY( "saveAllUserSettings" );

        // First check if we should be saving the user settings.

        if ( RunParams::getInstance().get(RPARAM_USERPREFERENCES).empty() )
        {
            return;
        }
        
        if (getUserSetting() == 0)
        {
            setUserSetting(*( new SettingsMgr(getSessionId(), getApplicationType() )));
        }
        
        savePositionSettings();
    }
    
    
    void ArchiveManagerGUI::prepareForClose()
    {
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
        catch ( TA_Base_Core::UserSettingsException& e )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "UserSettingsException", e.what() );

            // Get the operator name from the session id.

            std::string operatorName( "" );
            try
            {
                std::auto_ptr< TA_Base_Core::IOperator > theOperator(
                    TA_Base_Core::OperatorAccessFactory::getInstance().getOperatorFromSession( 
                    TA_Base_Core::RunParams::getInstance().get( RPARAM_SESSIONID ), false ) );
                if ( 0 != theOperator.get() )
                {
                    operatorName = theOperator->getName();
                }
            }
            catch( TA_Base_Core::DatabaseException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", e.what() );
            }
            catch( TA_Base_Core::DataException& e )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException", e.what() );
            }

            if ( operatorName.empty() )
            {
                operatorName = "(unknown)";
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << operatorName << e.what();
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_050017,CAPTION_ARCHIVE_MANAGER);
        }

        // Clean up the rights library
        TA_Base_Bus::RightsLibrary* library = TA_ArchiveManager::getRightsLibrary();
        delete library;
        library = NULL;
    }


    void ArchiveManagerGUI::onInitGenericGUICompleted()
	{
		CWaitCursor cur;
		CWnd *pWnd = getApplicationWnd();
		pWnd->LockWindowUpdate();
        TA_Base_Bus::TransActiveDialog * pDlg = dynamic_cast<TA_Base_Bus::TransActiveDialog * >(pWnd);
		TA_ASSERT(pDlg != NULL, "the application window is NULL!");
		
		pDlg->init();
		pDlg->UnlockWindowUpdate();
		pDlg->UpdateWindow();
	}
} // TA_Base_App
