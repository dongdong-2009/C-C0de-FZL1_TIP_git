/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentDetailView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Agent's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/AgentDetailView.h"

#include "app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeView.h"
#include "app/configuration/config_plugins/agent/src/ConfigAgent.h"
#include "app/configuration/config_plugins/agent/src/PasswordDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

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
using TA_Base_Core::EntityAccessFactory;
using TA_Base_Core::IConfigEntity;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // The titles for the change dialog box
    const CString AgentDetailView::CHANGE_LOCATION_DIALOG_TITLE  = "Change Location";

    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int AgentDetailView::NAME_FIELD_LENGTH        = 64;  // Must leave enough space for 'MonitoredProcess' to be appended
    const int AgentDetailView::DESCRIPTION_FIELD_LENGTH = 2000;
    const int AgentDetailView::EXECUTABLE_FIELD_LENGTH  = 100;
    const int AgentDetailView::ADDRESS_FIELD_LENGTH     = 100;
    const int AgentDetailView::USERNAME_FIELD_LENGTH    = 256;

    // These are the names of the entity parameters that we have fixed fields for
    const std::string AgentDetailView::USERNAME          = "UserName";
	const std::string AgentDetailView::PASSWORD          = "Passwd";
    const std::string AgentDetailView::IS_MANAGED        = "ManagedProcess";
    const std::string AgentDetailView::IS_SERVICE        = "ServiceProcess";
    const std::string AgentDetailView::AUTO_FAILBACK     = "AutoFailback";
    const std::string AgentDetailView::TERMINATE_TIMEOUT = "TerminateTimeout";
    const std::string AgentDetailView::KILL_TIMEOUT      = "KillTimeout";
    const std::string AgentDetailView::STARTUP_TIMEOUT   = "StartTimeout";
    const std::string AgentDetailView::CONTROL_TIMEOUT   = "ControlTimeout";
    const std::string AgentDetailView::MONITOR_TIMEOUT   = "MonitorTimeout";
    const std::string AgentDetailView::START_LAG         = "StartLag";
	const std::string AgentDetailView::PRIORITY          = "Priority";
          
    const std::string AgentDetailView::TA_TRUE  = "Yes";
    const std::string AgentDetailView::TA_FALSE = "No";

	const std::string AgentDetailView::RPARAM_HIDEPASSWD = "HidePasswd";


    IMPLEMENT_DYNCREATE(AgentDetailView, CFormView)


    AgentDetailView::AgentDetailView() : 
        m_currentAgent(NULL),
        m_treeView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AgentDetailView::IDD)
    {
        //{{AFX_DATA_INIT(AgentDetailView)
	//}}AFX_DATA_INIT
    }


    AgentDetailView::~AgentDetailView()
    {
    }


    void AgentDetailView::DoDataExchange(CDataExchange* pDX)
    {
        CFormView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(AgentDetailView)
	DDX_Control(pDX, IDC_EDIT_PASSWORD, m_password);
	DDX_Control(pDX, IDC_EDIT_PRIORITY, m_priority);
	DDX_Control(pDX, IDC_EDIT_CONTROL, m_controlTimeout);
	DDX_Control(pDX, IDC_EDIT_MONITOR, m_monitorTimeout);
	DDX_Control(pDX, IDC_EDIT_STARTUP, m_startupTimeout);
	DDX_Control(pDX, IDC_ADDRESS_BITMAP, m_addressBitmap);
        DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
        DDX_Control(pDX, IDC_DESCRIPTION_BITMAP, m_descriptionBitmap);
        DDX_Control(pDX, IDC_EXECUTABLE_BITMAP, m_executableBitmap);
        DDX_Control(pDX, IDC_LOCATION_BITMAP, m_locationBitmap);
        DDX_Control(pDX, IDC_USERNAME_BITMAP, m_usernameBitmap);
		DDX_Control(pDX, IDC_PASSWORD_BITMAP, m_passwordBitmap);
        DDX_Control(pDX, IDC_LAG_BITMAP, m_lagBitmap);
		DDX_Control(pDX, IDC_EDIT_SUBSYSTEM, m_subsystem);
        DDX_Control(pDX, IDC_EDIT_NAME, m_name);
        DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
        DDX_Control(pDX, IDC_EDIT_EXECUTABLE, m_executable);
        DDX_Control(pDX, IDC_EDIT_ADDRESS, m_address);
        DDX_Control(pDX, IDC_EDIT_LOCATION, m_location);
        DDX_Control(pDX, IDC_CHECK_MANAGED, m_checkManaged);
        DDX_Control(pDX, IDC_CHECK_SERVICE, m_checkService);
        DDX_Control(pDX, IDC_CHECK_FAILBACK, m_checkFailback);
        DDX_Control(pDX, IDC_EDIT_TERMINATE, m_terminateTimeout);
        DDX_Control(pDX, IDC_EDIT_KILL, m_killTimeout);
        DDX_Control(pDX, IDC_EDIT_LAG, m_startLag);
        DDX_Control(pDX, IDC_EDIT_USERNAME, m_username);
        DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_attributeList);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
        DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_STATIC_STATUS, m_status);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AgentDetailView, AbstractDetailView)
    //{{AFX_MSG_MAP(AgentDetailView)
    ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
    ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
    ON_EN_KILLFOCUS(IDC_EDIT_EXECUTABLE, onKillfocusEditExecutable)
    ON_EN_KILLFOCUS(IDC_EDIT_ADDRESS, onKillfocusEditAddress)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
    ON_EN_KILLFOCUS(IDC_EDIT_USERNAME, onKillfocusEditUsername)
	ON_EN_KILLFOCUS(IDC_EDIT_PASSWORD, onKillfocusEditPassword)
    ON_BN_CLICKED(IDC_CHECK_MANAGED, onCheckManaged)
    ON_BN_CLICKED(IDC_CHECK_SERVICE, onCheckService)
    ON_BN_CLICKED(IDC_CHECK_FAILBACK, onCheckFailback)
    ON_EN_KILLFOCUS(IDC_EDIT_TERMINATE, onKillfocusEditTerminateTimeout)
    ON_EN_KILLFOCUS(IDC_EDIT_KILL, onKillfocusEditKillTimeout)
    ON_EN_KILLFOCUS(IDC_EDIT_LAG, onKillfocusEditStartLag)
    ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_CHANGE(IDC_EDIT_NAME, onChangeEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_MONITOR, onKillfocusEditMonitorTimeout)
	ON_EN_KILLFOCUS(IDC_EDIT_CONTROL, onKillfocusEditControlTimeout)
	ON_EN_KILLFOCUS(IDC_EDIT_STARTUP, onKillfocusEditStartupTimeout)
	ON_EN_KILLFOCUS(IDC_EDIT_PRIORITY, onKillfocusEditPriority)
	ON_BN_CLICKED(IDC_BUTTON_PASSWORD, OnButtonPassword)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AgentDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

        CFormView::OnInitialUpdate();
        
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText( NAME_FIELD_LENGTH );
        m_description.SetLimitText( DESCRIPTION_FIELD_LENGTH );
        m_executable.SetLimitText( EXECUTABLE_FIELD_LENGTH );
        m_address.SetLimitText( ADDRESS_FIELD_LENGTH );
        m_username.SetLimitText( USERNAME_FIELD_LENGTH );
        
        // Hide all the Agent details until we are told to populate.
        showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AgentDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view

        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_EXECUTABLE)->ShowWindow(isShown);
        GetDlgItem(IDC_EXECUTABLE_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_EXECUTABLE)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_ADDRESS_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ADDRESS)->ShowWindow(isShown);
        
        GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_LOCATION)->ShowWindow(isShown);
        
        GetDlgItem(IDC_STATIC_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SUBSYSTEM)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_USERNAME)->ShowWindow(isShown);
        GetDlgItem(IDC_USERNAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_USERNAME)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_MANAGED)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_MANAGED)->ShowWindow(isShown);

		//TD18202 marc_bugfix
		//hide password-related controls for C830
		if (RunParams::getInstance().isSet(RPARAM_HIDEPASSWD.c_str()))
		{
			GetDlgItem(IDC_STATIC_PASSWORD)->ShowWindow(false);
		    GetDlgItem(IDC_PASSWORD_BITMAP)->ShowWindow(false);
			GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(false);
			GetDlgItem(IDC_BUTTON_PASSWORD)->ShowWindow(false);
		}
		else
		{
			GetDlgItem(IDC_STATIC_PASSWORD)->ShowWindow(isShown);
		    GetDlgItem(IDC_PASSWORD_BITMAP)->ShowWindow(isShown);
			GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(isShown);
			GetDlgItem(IDC_BUTTON_PASSWORD)->ShowWindow(isShown);
		}
        
        GetDlgItem(IDC_STATIC_SERVICE)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_SERVICE)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_FAILBACK)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_FAILBACK)->ShowWindow(isShown);
        
        GetDlgItem(IDC_STATIC_TERMINATE)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_TERMINATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SECONDS1)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_KILL)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_KILL)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SECONDS2)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_STARTUP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_STARTUP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SECONDS4)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_CONTROL)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_CONTROL)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SECONDS5)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_MONITOR)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_MONITOR)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SECONDS6)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_LAG)->ShowWindow(isShown);
        GetDlgItem(IDC_LAG_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_LAG)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SECONDS3)->ShowWindow(isShown);

		GetDlgItem(IDC_STATIC_PRIORITY)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PRIORITY)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_ATTRIBUTES)->ShowWindow(isShown);
        GetDlgItem(IDC_LIST_ATTRIBUTES)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);

        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);

        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        
        GetDlgItem(IDC_STATIC_STATUS)->ShowWindow(isShown);
    }


    void AgentDetailView::enableChildEntityDetails(bool isEnabled)
    {
        m_address.EnableWindow(isEnabled);
        m_attributeList.EnableWindow(isEnabled);
        m_checkManaged.EnableWindow(isEnabled);
    }


    void AgentDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_treeView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AgentDatabaseAccessor*>(getDatabaseAccessor());
            m_treeView = dynamic_cast<AgentTreeView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_treeView != NULL, "The list view has not been set yet or is not a proper AgentDetailView");
        }


        m_currentAgent = dynamic_cast<ConfigAgent*>(currentItem);

        if (m_currentAgent == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // Now populate all the fields with the data retrieved from the
            // IConfigAgent* object

		    m_name.SetWindowText( m_currentAgent->getAgentName().c_str() );
            m_description.SetWindowText( m_currentAgent->getDescription().c_str());
            m_location.SetWindowText( m_currentAgent->getLocationName().c_str());
            m_subsystem.SetWindowText( m_currentAgent->getSubsystemName().c_str());

            m_executable.SetWindowText( m_currentAgent->getExecutable().c_str() );
            m_username.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(USERNAME).c_str() );
			m_password.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(PASSWORD).c_str() );
            m_terminateTimeout.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(TERMINATE_TIMEOUT).c_str() );
            m_startLag.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(START_LAG).c_str() );
			m_priority.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(PRIORITY).c_str() );
            m_killTimeout.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(KILL_TIMEOUT).c_str() );
            m_startupTimeout.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(STARTUP_TIMEOUT).c_str() );
            m_controlTimeout.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(CONTROL_TIMEOUT).c_str() );
            m_monitorTimeout.SetWindowText( m_currentAgent->getMonitoredProcessEntity().getParameterValue(MONITOR_TIMEOUT).c_str() );


            std::string value = m_currentAgent->getMonitoredProcessEntity().getParameterValue(IS_MANAGED);

            setIsManaged(value == TA_TRUE);
            m_checkManaged.SetCheck( value == TA_TRUE );

            value = m_currentAgent->getMonitoredProcessEntity().getParameterValue(IS_SERVICE);
            setIsService(value == TA_TRUE);
            m_checkService.SetCheck( value == TA_TRUE );


            value = m_currentAgent->getMonitoredProcessEntity().getParameterValue(AUTO_FAILBACK);
            m_checkFailback.SetCheck( value == TA_TRUE );


            // Populate the details of the child entity
            displayRelatedEntityDetails();

            if( m_currentAgent->isNew() )
            {
                // This is a new Agent or at least a copy of an Agent so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                IConfigEntity* agent = m_currentAgent->getAgentEntity();
                CString statusLabel("");
                if (agent != NULL)
                {
                    statusLabel += agent->getType().c_str();
                }
                statusLabel += NEW_LABEL;
                m_status.SetWindowText(statusLabel);

                m_modifyDate.SetWindowText(NO_VALUE);
                m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                IConfigEntity* agent = m_currentAgent->getAgentEntity();
                CString statusLabel(NO_VALUE);
                if (agent != NULL)
                {
                    statusLabel = agent->getType().c_str();
                }
                m_status.SetWindowText(statusLabel);

				TA_Base_Core::DateFormat dateFormat; // TD12474
                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentAgent->getMonitoredProcessEntity().getDateModified();
                if (modify == 0)
                {
                    m_modifyDate.SetWindowText(NO_VALUE);
                }
                else
                {
                    COleDateTime dateModified( modify );
                    // TD12474 ++
                    //m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
					std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
                    m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                    // ++ TD12474 
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_currentAgent->getMonitoredProcessEntity().getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474 
            }
            
            refreshButtonAvailability();

            showItemDetails(true);

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Agent details to display");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Agent details to display");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }

    
    void AgentDetailView::refreshButtonAvailability()
    {
        if (m_currentAgent == NULL)
        {
            return;
        }

        if (m_currentAgent->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentAgent->hasChanged())
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

    
    void AgentDetailView::displayRelatedEntityDetails()
    {
        FUNCTION_ENTRY("displayRelatedEntityDetails");

        if (m_currentAgent == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // If the related entity doesn't exist, blank out and disable all
        // its fields and alert the user.
        if( ! m_currentAgent->doesAgentEntityExist())
        {
            m_address.SetWindowText("");
            m_attributeList.populateControl( NULL );

            // If there is no child entity then there is no point setting this to a managed process.
            // So disable the check
            m_checkManaged.SetCheck(0);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(TA_FALSE,IS_MANAGED);
            setIsManaged(false);

            enableChildEntityDetails(false);

            FUNCTION_EXIT;
            return;
        }

        m_address.SetWindowText( m_currentAgent->getAddress().c_str());
        m_attributeList.populateControl( m_currentAgent->getAgentEntity() );

        enableChildEntityDetails(true);
    }


    void AgentDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentAgent);

        FUNCTION_EXIT;
    }


    void AgentDetailView::onButtonApply() 
    {
        FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        try
        {
	        if (m_currentAgent != NULL)
            {
                AfxGetApp()->DoWaitCursor(1);

                // Set up the details for the audit message
                MessageSender::UpdateDetails auditUpdateDetails;
                auditUpdateDetails.isNewItem = m_currentAgent->isNew();
                auditUpdateDetails.isDelete = false;
                auditUpdateDetails.configUpdateMessage = MessageSender::ENTITY;
                auditUpdateDetails.nameOfChangedItem = m_currentAgent->getName();
                if (!m_currentAgent->isNew())
                {
                    auditUpdateDetails.changes = m_currentAgent->getAllItemChanges();
                }

                // Set up the details for the online update message for the monitored process entity
                MessageSender::UpdateDetails mpOnlineUpdateDetails;
                mpOnlineUpdateDetails.isNewItem = m_currentAgent->isNew();
                mpOnlineUpdateDetails.isDelete = false;
                mpOnlineUpdateDetails.configUpdateMessage = MessageSender::ENTITY;
                mpOnlineUpdateDetails.nameOfChangedItem = m_currentAgent->getMonitoredProcessEntity().getName();
                if (!m_currentAgent->isNew())
                {
                    mpOnlineUpdateDetails.changes = m_currentAgent->getMonitoredProcessEntity().getAllItemChanges();
                }
                
                // Set up the details for the online update message for the monitored process entity
                MessageSender::UpdateDetails agentOnlineUpdateDetails;
                if (m_currentAgent->doesAgentEntityExist())
                {
                    agentOnlineUpdateDetails.isNewItem = m_currentAgent->isNew();
                    agentOnlineUpdateDetails.isDelete = false;
                    agentOnlineUpdateDetails.configUpdateMessage = MessageSender::ENTITY;
                    agentOnlineUpdateDetails.nameOfChangedItem = m_currentAgent->getAgentEntity()->getName();
                    if (!m_currentAgent->isNew())
                    {
                        agentOnlineUpdateDetails.changes = m_currentAgent->getAgentEntity()->getAllItemChanges();
                    }
                }

                // Apply the changes to the database
                m_currentAgent->applyChanges();

                // Update the view
                populateItemDetails(m_currentAgent);

                TA_ASSERT( getMessageSender() != NULL, "The MessageSender is NULL");

                // Retrieve the key of the item after we have applied in case this is a new item.
                // If it is a new item it doesn't get given a key until Apply is called.
                // Now send all the messages that are required

                auditUpdateDetails.keyOfChangedItem = m_currentAgent->getKey();
                getMessageSender()->sendAuditMessagesForTheChange(auditUpdateDetails);

                mpOnlineUpdateDetails.keyOfChangedItem = m_currentAgent->getMonitoredProcessEntity().getKey();
                getMessageSender()->sendOnlineUpdateMessagesForTheChange(mpOnlineUpdateDetails);

                if (m_currentAgent->doesAgentEntityExist())
                {
                    agentOnlineUpdateDetails.keyOfChangedItem = m_currentAgent->getAgentEntity()->getKey();
                    getMessageSender()->sendOnlineUpdateMessagesForTheChange( agentOnlineUpdateDetails );
                }
            }
        }
        catch ( const TA_Base_Core::DataConfigurationException& exception )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataConfigurationException","");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_FIELD_MISSING);

            std::vector<std::string> missing = exception.getMissingFields();
            for (std::vector<std::string>::iterator iter = missing.begin(); iter != missing.end(); ++iter)
            {
                reason += iter->c_str();
                reason += _T("\n");
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not apply the item changes");
            TA_Base_Bus::TransActiveMessage userMsg;
            CString action, reason;
            action.LoadString(IDS_APPLY);
            reason.LoadString(IDS_DATABASE_ERROR);
            userMsg << action << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DataException& exception)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not apply the item changes");

            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::WRONG_TYPE):
                {
                    errorId = IDS_FIELD_INVALID;
                    break;
                }
            
                case (TA_Base_Core::DataException::MISSING_MANDATORY):
                {
                    errorId = IDS_FIELD_MISSING;
                    break;
                }

                default:
                {
                    errorId = IDS_UNKNOWN_ERROR;
                    break;
                }
            }

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }

        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void AgentDetailView::onCheckManaged()
    {
        FUNCTION_ENTRY("onCheckManaged");

        if (m_currentAgent != NULL )
        {
            int checked = m_checkManaged.GetCheck();
            
            try
            {
                std::string isChecked(TA_TRUE);
                if (checked == 0)
                {
                    isChecked = TA_FALSE;
                    setIsManaged(false);
                }
                else
                {
                    setIsManaged(true);
                }

                m_currentAgent->getMonitoredProcessEntity().setParameterValue(isChecked,IS_MANAGED);
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


    void AgentDetailView::onCheckService()
    {
        FUNCTION_ENTRY("onCheckService");

        if (m_currentAgent != NULL )
        {
            int checked = m_checkService.GetCheck();
            
            std::string isChecked(TA_TRUE);
            if (checked == 0)
            {
                isChecked = TA_FALSE;
            }

            setIsService(checked != 0);

            try
            {
                m_currentAgent->getMonitoredProcessEntity().setParameterValue(isChecked,IS_SERVICE);
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


    void AgentDetailView::onCheckFailback()
    {
        FUNCTION_ENTRY("onCheckFailback");

        if (m_currentAgent != NULL )
        {
            int checked = m_checkFailback.GetCheck();
            
            std::string isChecked(TA_TRUE);
            if (checked == 0)
            {
                isChecked = TA_FALSE;
            }

            try
            {
                m_currentAgent->getMonitoredProcessEntity().setParameterValue(isChecked,AUTO_FAILBACK);
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

    
    void AgentDetailView::onKillfocusEditExecutable()
    {
        FUNCTION_ENTRY("onKillfocusEditExecutable");

        // When the user changes the executable we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }


        try
        {
            CString executable;
            m_executable.GetWindowText(executable);
            m_currentAgent->setExecutable( std::string(executable) );
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


    void AgentDetailView::onKillfocusEditAddress() 
    {
        FUNCTION_ENTRY("onKillfocusEditAddress");

        // When the user changes the address we must update the ConfigAgent* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentAgent == NULL || !m_currentAgent->doesAgentEntityExist())
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString address;
            m_address.GetWindowText(address);
            m_currentAgent->setAddress( std::string(address) );
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

    
    void AgentDetailView::onKillfocusEditUsername()
    {
        FUNCTION_ENTRY("onKillfocusEditUsername");

        // When the user changes the username we must update the
        // ConfigAgent* object so that it has a record of all
        // changes when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString username;
            m_username.GetWindowText(username);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(username),USERNAME);
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

	void AgentDetailView::onKillfocusEditPassword()
    {
        FUNCTION_ENTRY("onKillfocusEditPassword");

        // When the user changes the username we must update the
        // ConfigAgent* object so that it has a record of all
        // changes when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString password;
            m_password.GetWindowText(password);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(password),PASSWORD);
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


    void AgentDetailView::onKillfocusEditTerminateTimeout()
    {
        FUNCTION_ENTRY("onKillfocusEditTerminateTimeout");

        // When the user changes the terminate timeout we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString timeout;
            m_terminateTimeout.GetWindowText(timeout);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(timeout),TERMINATE_TIMEOUT);
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


    void AgentDetailView::onKillfocusEditKillTimeout()
    {
        FUNCTION_ENTRY("onKillfocusEditKillTimeout");

        // When the user changes the kill timeout we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString timeout;
            m_killTimeout.GetWindowText(timeout);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(timeout),KILL_TIMEOUT);
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


    void AgentDetailView::onKillfocusEditMonitorTimeout() 
    {
        FUNCTION_ENTRY("onKillfocusEditMonitorTimeout");

        // When the user changes the monitor timeout we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString timeout;
            m_monitorTimeout.GetWindowText(timeout);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(timeout),MONITOR_TIMEOUT);
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


    void AgentDetailView::onKillfocusEditControlTimeout() 
    {
        FUNCTION_ENTRY("onKillfocusEditControlTimeout");

        // When the user changes the monitor timeout we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString timeout;
            m_controlTimeout.GetWindowText(timeout);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(timeout),CONTROL_TIMEOUT);
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


    void AgentDetailView::onKillfocusEditStartupTimeout() 
    {
        FUNCTION_ENTRY("onKillfocusEditStartupTimeout");

        // When the user changes the monitor timeout we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString timeout;
            m_startupTimeout.GetWindowText(timeout);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(timeout),STARTUP_TIMEOUT);
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


    void AgentDetailView::onKillfocusEditStartLag()
    {
        FUNCTION_ENTRY("onKillfocusEditStartLag");

        // When the user changes the start lag we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString startLag;
            m_startLag.GetWindowText(startLag);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(startLag),START_LAG);
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

	void AgentDetailView::onKillfocusEditPriority() 
	{
		FUNCTION_ENTRY("onKillfocusEditPriority");

        // When the user changes the priority we must update the
        // ConfigAgent* object so that it has a record of all changes
        // when the user chooses to apply.
		        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString priority;
            m_priority.GetWindowText(priority);
			m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(priority),PRIORITY);
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


    void AgentDetailView::onKillfocusEditDescription() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // When the user changes the description we must update the ConfigAgent* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString description;
            m_description.GetWindowText(description);
            m_currentAgent->setDescription( std::string(description) );
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

        m_treeView->itemDetailsChanged(m_currentAgent->getUniqueIdentifier(), AgentTreeView::CHANGE_OF_NAME_IN_TREE);

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AgentDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        if (m_currentAgent == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            CString name;
            m_name.GetWindowText(name);

            m_currentAgent->setAgentName( std::string(name) );
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
 

    void AgentDetailView::onChangeEditName() 
    {
        CString name;
        m_name.GetWindowText(name);
    
        if (name.Find(' ') != -1)
        {
            // Get where the cursor currently is
            int startChar;
            int endChar;
            m_name.GetSel(startChar,endChar);
            
            // Remove the spaces
            name.Remove(' ');

            // Update the text and reset the user's cursor
            m_name.SetWindowText(name);
            m_name.SetSel(startChar,endChar);
        }
    }


    void AgentDetailView::onButtonLocation() 
    {
        FUNCTION_ENTRY("onButtonLocation");
        
        if (m_currentAgent == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the current location this entity is using
        CString currentLocation("");
        m_location.GetWindowText( currentLocation );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllLocations();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_LOCATION_DIALOG_TITLE, currentLocation, this);
        dlg.setSortItems( false );
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new location");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new location which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentAgent !=  NULL, "current agent is NULL.");

            // If this is a different location then set it in our ConfigAgent*.
            m_currentAgent->setLocation( dlg.getSelectedKey() );

            // Now update the location the details view is showing
            m_location.SetWindowText( dlg.getSelectedName() );

            m_treeView->itemDetailsChanged(m_currentAgent->getUniqueIdentifier(), AgentTreeView::CHANGE_OF_PARENT_IN_TREE);

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


    void AgentDetailView::setIsManaged(bool isManaged)
    {
        if (m_currentAgent == NULL)
        {
            return;
        }

        if (isManaged)
        {
            m_checkService.EnableWindow(TRUE);
            m_checkFailback.EnableWindow(TRUE);
            m_terminateTimeout.EnableWindow(TRUE);
            m_startupTimeout.EnableWindow(TRUE);
            m_controlTimeout.EnableWindow(TRUE);
            m_monitorTimeout.EnableWindow(TRUE);
        }
        else
        {
            m_checkService.SetCheck(0);
            m_checkService.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(TA_FALSE,IS_SERVICE);

            m_checkFailback.SetCheck(0);
            m_checkFailback.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(TA_FALSE,AUTO_FAILBACK);

            m_terminateTimeout.SetWindowText("");
            m_terminateTimeout.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue("",TERMINATE_TIMEOUT);

            m_startupTimeout.SetWindowText("");
            m_startupTimeout.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue("",STARTUP_TIMEOUT);

            m_controlTimeout.SetWindowText("");
            m_controlTimeout.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue("",CONTROL_TIMEOUT);

            m_monitorTimeout.SetWindowText("");
            m_monitorTimeout.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue("",MONITOR_TIMEOUT);
        }
    }


    void AgentDetailView::setIsService(bool isService)
    {
        if (m_currentAgent == NULL)
        {
            return;
        }

        if (isService)
        {
            m_controlTimeout.SetWindowText("");
            m_controlTimeout.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue("",CONTROL_TIMEOUT);

            m_monitorTimeout.SetWindowText("");
            m_monitorTimeout.EnableWindow(FALSE);
            m_currentAgent->getMonitoredProcessEntity().setParameterValue("",MONITOR_TIMEOUT);
        }
        else
        {
            m_monitorTimeout.EnableWindow(TRUE);
            m_controlTimeout.EnableWindow(TRUE);
        }
    }



    /////////////////////////////////////////////////////////////////////////////
    // AgentDetailView diagnostics

    #ifdef _DEBUG
    void AgentDetailView::AssertValid() const
    {
        AbstractDetailView::AssertValid();
    }

    void AgentDetailView::Dump(CDumpContext& dc) const
    {
        AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

	void AgentDetailView::OnButtonPassword() 
	{
		// TODO: Add your control notification handler code here
        FUNCTION_ENTRY("onButtonPassword");
	    
        // Now construct the dialog and then wait for the user to finish with it
        PasswordDialog dlg;
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the entering of a new password");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User entered a new password");

        try
        {
			CString newPassword = dlg.getPassword();
			
            TA_ASSERT(m_currentAgent !=  NULL, "current agent is NULL.");

            

            // If this is a different location then set it in our IConfigEntity*.
            m_currentAgent->getMonitoredProcessEntity().setParameterValue(std::string(newPassword), PASSWORD);
			
            // Now update the location the details view is showing
            m_password.SetWindowText( newPassword );

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


}
