/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/SystemControllerDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a system controller's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller/src/StdAfx.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDetailView.h"

#include "app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerListView.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/system_controller/src/IConfigSystemController.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int SystemControllerDetailView::NAME_FIELD_LENGTH          = 100;
    const int SystemControllerDetailView::DEBUG_LOG_DIR_FIELD_LENGTH = 1024;
    const int SystemControllerDetailView::BIN_DIR_FIELD_LENGTH = 1024;
    const int SystemControllerDetailView::AGENT_POLL_PERIOD_FIELD_LENGTH = 8;
    const int SystemControllerDetailView::HEARTBEAT_SEND_PERIOD_LENGTH = 8;
    const int SystemControllerDetailView::AGENT_RESPONSE_TIMEOUT_LENGTH = 8;

    const CString SystemControllerDetailView::CHANGE_GROUP_DIALOG_TITLE    = "Change Group";
    const CString SystemControllerDetailView::CHANGE_SERVER_DIALOG_TITLE    = "Change Server";

    const std::string SystemControllerDetailView::NO_GROUP = "<No Group>";

    IMPLEMENT_DYNCREATE(SystemControllerDetailView, CFormView)


    SystemControllerDetailView::SystemControllerDetailView() : 
        m_currentSystemController(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(SystemControllerDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(SystemControllerDetailView)
	//}}AFX_DATA_INIT
    }


    SystemControllerDetailView::~SystemControllerDetailView()
    {
    }


    void SystemControllerDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(SystemControllerDetailView)
	DDX_Control(pDX, IDC_ART_BITMAP, m_artBitmap);
	DDX_Control(pDX, IDC_EDIT_ART, m_agentResponseTimeout);
	DDX_Control(pDX, IDC_PROCESS_LIST, m_processList);
	DDX_Control(pDX, IDC_GROUP_LIST, m_groupList);
	DDX_Control(pDX, IDC_EDIT_DEBUGLOGDIR, m_debugLogDir);
	DDX_Control(pDX, IDC_EDIT_BINDIR, m_binDir);
	DDX_Control(pDX, IDC_EDIT_SERVER, m_server);
	DDX_Control(pDX, IDC_EDIT_HSP, m_heartbeatSendPeriod);
	DDX_Control(pDX, IDC_EDIT_GROUP, m_group);
	DDX_Control(pDX, IDC_EDIT_APP, m_agentPollPeriod);
	DDX_Control(pDX, IDC_SERVER_BITMAP, m_serverBitmap);
	DDX_Control(pDX, IDC_DEBUGLOGDIR_BITMAP, m_debugLogDirBitmap);
	DDX_Control(pDX, IDC_BINDIR_BITMAP, m_binDirBitmap);
	DDX_Control(pDX, IDC_HSP_BITMAP, m_hspBitmap);
	DDX_Control(pDX, IDC_APP_BITMAP, m_appBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(SystemControllerDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(SystemControllerDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_APP, onKillfocusEditAgentPollPeriod)
	ON_EN_KILLFOCUS(IDC_EDIT_DEBUGLOGDIR, onKillfocusEditLogDirectory)
	ON_EN_KILLFOCUS(IDC_EDIT_BINDIR, onKillfocusEditBinDirectory)
	ON_EN_KILLFOCUS(IDC_EDIT_HSP, onKillfocusEditHeartbeatSendPeriod)
	ON_BN_CLICKED(IDC_BUTTON_ADD, onButtonAddProcess)
	ON_BN_CLICKED(IDC_BUTTON_REMOVE, onButtonRemoveProcess)
	ON_BN_CLICKED(IDC_BUTTON_SERVER, onButtonServer)
	ON_BN_CLICKED(IDC_BUTTON_GROUP, onButtonGroup)
	ON_EN_KILLFOCUS(IDC_EDIT_ART, OnKillfocusEditAgentResponseTimeout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void SystemControllerDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_debugLogDir.SetLimitText(DEBUG_LOG_DIR_FIELD_LENGTH);
        m_binDir.SetLimitText(BIN_DIR_FIELD_LENGTH);
        m_heartbeatSendPeriod.SetLimitText(HEARTBEAT_SEND_PERIOD_LENGTH);
        m_agentPollPeriod.SetLimitText(AGENT_POLL_PERIOD_FIELD_LENGTH);
        m_agentResponseTimeout.SetLimitText(AGENT_RESPONSE_TIMEOUT_LENGTH);

        // Hide all the system controller details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem( IDC_STATIC_GROUP)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_GROUP)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_SERVER)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_SERVER)->ShowWindow(isShown);
        GetDlgItem( IDC_BUTTON_GROUP)->ShowWindow(isShown);
        GetDlgItem( IDC_BUTTON_SERVER)->ShowWindow(isShown);
        GetDlgItem( IDC_SERVER_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_APP)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_APP)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_ART)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_ART)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_HSP)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_HSP)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_DEBUGLOGDIR)->ShowWindow(isShown);
        GetDlgItem( IDC_EDIT_BINDIR)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_LOG_DIRECTORY)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_BIN_DIRECTORY)->ShowWindow(isShown);
        GetDlgItem( IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_APP_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_ART_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_HSP_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_DEBUGLOGDIR_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_BINDIR_BITMAP)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem( IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem( IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem( IDC_BUTTON_ADD)->ShowWindow(isShown);
        GetDlgItem( IDC_BUTTON_REMOVE)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_SECONDS1)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_SECONDS2)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_SECONDS3)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_PROCESSES)->ShowWindow(isShown);
        GetDlgItem( IDC_PROCESS_LIST)->ShowWindow(isShown);
        GetDlgItem( IDC_STATIC_GROUPS)->ShowWindow(isShown);
        GetDlgItem( IDC_GROUP_LIST)->ShowWindow(isShown);
    }


    void SystemControllerDetailView::refreshButtonAvailability()
    {
        if (m_currentSystemController == NULL)
        {
            return;
        }

        if (m_currentSystemController->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentSystemController->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }


    void SystemControllerDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<SystemControllerDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<SystemControllerListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper SystemControllerDetailView");
        }


        m_currentSystemController = dynamic_cast<TA_Base_Core::IConfigSystemController*>(currentItem);

        if (m_currentSystemController == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigSystemController* object

	        m_name.SetWindowText( m_currentSystemController->getName().c_str() );
            m_debugLogDir.SetWindowText( m_currentSystemController->getDebugLogDir().c_str() );
            m_binDir.SetWindowText( m_currentSystemController->getBinDir().c_str() );
            m_group.SetWindowText( m_currentSystemController->getGroupByName().c_str() );
            m_server.SetWindowText( m_currentSystemController->getServerByName().c_str() );

            CString agentPoll;
            agentPoll.Format("%lu", m_currentSystemController->getAgentPollPeriod());
            m_agentPollPeriod.SetWindowText( agentPoll );

            CString heartbeatSend;
            heartbeatSend.Format("%lu", m_currentSystemController->getHeartbeatSendPeriod());
            m_heartbeatSendPeriod.SetWindowText( heartbeatSend );

            CString agentResponseTimeout;
            agentResponseTimeout.Format("%lu", m_currentSystemController->getAgentResponseTimeout());
            m_agentResponseTimeout.SetWindowText( agentResponseTimeout );

            if( m_currentSystemController->isNew() )
            {
                // This is a new system controller or at least a copy of an system controller so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				// ++ TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentSystemController->getDateModified();
                if (modify == 0)
                {
                    m_modifyDate.SetWindowText(NO_VALUE);
                }
                else
                {
                    COleDateTime dateModified( modify );
                    // TD12474 ++
                    //m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
                    m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                    // ++ TD12474
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_currentSystemController->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474
            }

            m_groupList.populateControl( *m_currentSystemController );
            m_processList.populateControl( *m_currentSystemController );
            
            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve system controller details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve system controller details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigSystemController* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentSystemController != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentSystemController->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentSystemController->getUniqueIdentifier(), m_currentSystemController->getName().c_str() );
                }
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }   

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onKillfocusEditLogDirectory() 
    {
        FUNCTION_ENTRY("onKillfocusEditLogDirectory");

        if (m_currentSystemController != NULL )
        {
            try
            {
                CString logDirectory;
                m_debugLogDir.GetWindowText(logDirectory);
                m_currentSystemController->setDebugLogDir( std::string(logDirectory) );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }  
        
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onKillfocusEditBinDirectory() 
    {
        FUNCTION_ENTRY("onKillfocusEditBinDirectory");

        if (m_currentSystemController != NULL )
        {
            try
            {
                CString binDirectory;
                m_binDir.GetWindowText(binDirectory);
                m_currentSystemController->setBinDir( std::string(binDirectory) );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }  
        
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onKillfocusEditAgentPollPeriod() 
    {
        FUNCTION_ENTRY("onKillfocusEditAgentPollPeriod");

        if (m_currentSystemController != NULL )
        {
            // Retrieve the entered text
            CString enteredAgentPollPeriod;
            m_agentPollPeriod.GetWindowText(enteredAgentPollPeriod);

            try
            {
                // Now convert it into an unsigned long
                unsigned long agentPollPeriod;

                if (enteredAgentPollPeriod.IsEmpty())
                {
                    agentPollPeriod = 0;
                }
                else
                {
                    std::istringstream agentPollString;
                    agentPollString.str( std::string(enteredAgentPollPeriod) );

                    agentPollString >> agentPollPeriod;
                }

                m_currentSystemController->setAgentPollPeriod( agentPollPeriod );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onKillfocusEditHeartbeatSendPeriod() 
    {
        FUNCTION_ENTRY("onKillfocusEditAgentPollPeriod");

        if (m_currentSystemController != NULL )
        {
            // Retrieve the entered text
            CString enteredHeartbeatSendPeriod;
            m_heartbeatSendPeriod.GetWindowText(enteredHeartbeatSendPeriod);

            // Now convert it into an unsigned long
            unsigned long heartbeatSendPeriod;

            if (enteredHeartbeatSendPeriod.IsEmpty())
            {
                heartbeatSendPeriod = 0;
            }
            else
            {
                std::istringstream heartbeatSendPeriodString;
                heartbeatSendPeriodString.str( std::string(enteredHeartbeatSendPeriod) );

                heartbeatSendPeriodString >> heartbeatSendPeriod;
            }

            try
            {
                m_currentSystemController->setHeartbeatSendPeriod( heartbeatSendPeriod );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::OnKillfocusEditAgentResponseTimeout() 
    {
        FUNCTION_ENTRY("OnKillfocusEditAgentResponseTimeout");

        if (m_currentSystemController != NULL )
        {
            // Retrieve the entered text
            CString enteredAgentResponseTimeout;
            m_agentResponseTimeout.GetWindowText(enteredAgentResponseTimeout);

            // Now convert it into an unsigned long
            unsigned long agentResponseTimeout;

            if (enteredAgentResponseTimeout.IsEmpty())
            {
                agentResponseTimeout = 0;
            }
            else
            {
                std::istringstream agentResponseTimeoutString;
                agentResponseTimeoutString.str( std::string(enteredAgentResponseTimeout) );

                agentResponseTimeoutString >> agentResponseTimeout;
            }

            try
            {
                m_currentSystemController->setAgentResponseTimeout( agentResponseTimeout );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onButtonAddProcess() 
    {
	    m_processList.addNewItems();

        refreshButtonAvailability();
    }


    void SystemControllerDetailView::onButtonRemoveProcess() 
    {
	    m_processList.removeSelectedItem();

        refreshButtonAvailability();
    }


    void SystemControllerDetailView::onButtonServer() 
    {
	    FUNCTION_ENTRY("onButtonServer");
	 
        // Retrieve the current location this entity is using
        CString currentServer("");
        m_server.GetWindowText( currentServer );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllServers();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_SERVER_DIALOG_TITLE, currentServer, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new server");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new server which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentSystemController !=  NULL, "current system controller is NULL.");

            // If this is a different server then set it in our IConfigSystemController*.
            m_currentSystemController->setServerEntityKey( dlg.getSelectedKey() );

            // Now update the server the details view is showing
            m_server.SetWindowText( dlg.getSelectedName() );

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onButtonGroup() 
    {
	    FUNCTION_ENTRY("onButtonGroup");
	 
        // Retrieve the current group this system controller is using
        CString currentGroup("");
        m_group.GetWindowText( currentGroup);

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllGroups();
        /// Allow a blank entry as the user does not have to select a group
        items.insert( ChangeDialog::ItemsForSelection::value_type(0,NO_GROUP) );

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_GROUP_DIALOG_TITLE, currentGroup, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new group");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new group which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentSystemController !=  NULL, "current system controller is NULL.");

            // If this is a different group then set it in our IConfigSystemController*.
            m_currentSystemController->setGroupId( dlg.getSelectedKey() );

            // Now update the group the details view is showing
            if( 0 == dlg.getSelectedName().Compare(NO_GROUP.c_str()) )
            {
                m_group.SetWindowText( "" );
            }
            else
            {
                m_group.SetWindowText( dlg.getSelectedName() );
            }

            // Now tell the group list to re-populate so that the user cannot attempt to monitor the group
            // that this System Controller is now in.
            m_groupList.populateControl( *m_currentSystemController );

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentSystemController);

        FUNCTION_EXIT;
    }


    void SystemControllerDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentSystemController);

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // SystemControllerDetailView diagnostics

    #ifdef _DEBUG
    void SystemControllerDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void SystemControllerDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

