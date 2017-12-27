/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDetailView.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an event plan association's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_plan_association/src/StdAfx.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDetailView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDatabaseAccessor.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocListView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocComponent.h"
#include "app/configuration/config_plugins/event_plan_association/src/ChangePlanDialog.h"
#include "app/configuration/configuration_editor/src/MainFrm.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/security/rights_library/src/RightsLibrary.h"
#include "bus/security/access_control/actions/src/AccessControlledActions.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/src/IResource.h"
#include "core/data_access_interface/plans/src/IConfigEventPlanAssocData.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataConfigurationException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"
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
    // The titles for the change dialog box
    const CString EventPlanAssocDetailView::CHANGE_ENTITYTYPE_DIALOG_TITLE    = "Change Entity Type";
    const CString EventPlanAssocDetailView::CHANGE_EVENTTYPE_DIALOG_TITLE  = "Change Event Type";

    const int EventPlanAssocDetailView::ENTITY_FIELD_LENGTH = 80;
    const int EventPlanAssocDetailView::ENTITYTYPE_FIELD_LENGTH = 80;
    const int EventPlanAssocDetailView::PLAN_FIELD_LENGTH = 256;
    const int EventPlanAssocDetailView::EVENTTYPE_FIELD_LENGTH = 256;

    
    IMPLEMENT_DYNCREATE(EventPlanAssocDetailView, CFormView)


    EventPlanAssocDetailView::EventPlanAssocDetailView() : 
        m_currentAssociation(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
		m_readOnly(false),
		m_rightsLibrary(NULL),
		m_resourceKey(0),
        AbstractDetailView(EventPlanAssocDetailView::IDD)
    {
    //{{AFX_DATA_INIT(EventPlanAssocDetailView)
    //}}AFX_DATA_INIT

		TA_Base_Bus::RightsLibraryFactory rlf;
        m_rightsLibrary = rlf.buildRightsLibrary();

		TA_Base_Core::IEntityData* guiEntity;
		TA_Base_Core::IResource* resource;

        std::string entityName = TA_Base_Core::RunParams::getInstance().get( RPARAM_ENTITYNAME );
        if ( !entityName.empty() )
        {
			try
			{
				guiEntity = TA_Base_Core::EntityAccessFactory::getInstance().getEntity( entityName );
				resource = TA_Base_Core::ResourceAccessFactory::getInstance().getResourceFromEntity( guiEntity->getKey() );
				m_resourceKey = resource->getKey();

				delete resource;
				resource = NULL;

				delete guiEntity;
				guiEntity = NULL;
            
			}
			catch(...)
			{
				if (resource != NULL)
				{
					delete resource;
					resource = NULL;
				}
				if (guiEntity != NULL)
				{
					delete guiEntity;
					guiEntity = NULL;
				}

				LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Error while trying to retrieve resource key");
			}
		}
	}


    EventPlanAssocDetailView::~EventPlanAssocDetailView()
    {
        if( m_rightsLibrary != NULL )
        {
            delete m_rightsLibrary;
            m_rightsLibrary = NULL;
        }
    }


    void EventPlanAssocDetailView::DoDataExchange(CDataExchange* pDX)
    {
        CFormView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(EventPlanAssocDetailView)
        DDX_Control(pDX, IDC_ENTITY_BITMAP, m_bitmapEntity);
        DDX_Control(pDX, IDC_EDIT_PLAN, m_plan);
        DDX_Control(pDX, IDC_EDIT_ENTITYTYPE, m_entityType);
        DDX_Control(pDX, IDC_EDIT_ENTITY, m_entity);
        DDX_Control(pDX, IDC_EDIT_EVENT, m_eventType);
        DDX_Control(pDX, IDC_PLAN_BITMAP, m_bitmapPlan);
        DDX_Control(pDX, IDC_EVENT_BITMAP, m_bitmapEventType);
        DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
        DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_RADIO_ENTITYTYPE, m_entityTypeRadio);
        DDX_Control(pDX, IDC_RADIO_ENTITY, m_entityRadio);
        DDX_Control(pDX, IDC_CHECK_ENABLED, m_enabled);
        //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(EventPlanAssocDetailView, AbstractDetailView)
    //{{AFX_MSG_MAP(EventPlanAssocDetailView)
    ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_EVENTTYPE, onButtonEventtype)
    ON_BN_CLICKED(IDC_BUTTON_ENTITYTYPE, onButtonEntitytype)
	ON_BN_CLICKED(IDC_BUTTON_ENTITY, onButtonEntity)
    ON_BN_CLICKED(IDC_BUTTON_PLAN, onButtonPlan)
    ON_BN_CLICKED(IDC_RADIO_ENTITY, onRadioEntity)
    ON_BN_CLICKED(IDC_RADIO_ENTITYTYPE, onRadioEntitytype)
    ON_BN_CLICKED(IDC_CHECK_ENABLED, onCheckEnabled)
    ON_EN_KILLFOCUS(IDC_EDIT_EVENT, onKillfocusEditEvent)
    ON_EN_KILLFOCUS(IDC_EDIT_ENTITY, onKillfocusEditEntity)
    ON_EN_KILLFOCUS(IDC_EDIT_ENTITYTYPE, onKillfocusEditEntitytype)
    ON_EN_KILLFOCUS(IDC_EDIT_PLAN, onKillfocusEditPlan)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void EventPlanAssocDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

        CFormView::OnInitialUpdate();

        // Set the limits on the text fields.
        m_entityType.SetLimitText(ENTITYTYPE_FIELD_LENGTH);
        m_entity.SetLimitText(ENTITY_FIELD_LENGTH);
        m_eventType.SetLimitText(EVENTTYPE_FIELD_LENGTH);
        m_plan.SetLimitText(PLAN_FIELD_LENGTH);

        // Hide all the event plan association details until we are told to populate.
        showItemDetails(false);

		// TD #12456
		// make the view read-only, with the exception of the Enabled checkbox
		// and the Apply/Reset buttons if the operator does not have modify rights
        std::string reason("");
        TA_Base_Bus::EDecisionModule decisionModule = TA_Base_Bus::UNDEFINED_DM;

		bool canModify = m_rightsLibrary->isActionPermittedOnResource(
							TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID),
							m_resourceKey,
							TA_Base_Bus::aca_CE_MODIFY_EVENT_PLAN_ASSOCIATIONS,
							reason,
							decisionModule);

		setReadOnly(!canModify);

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::setReadOnly(bool readOnly) 
    {
		m_readOnly = readOnly;

		GetDlgItem(IDC_BUTTON_EVENTTYPE)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_BUTTON_PLAN)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_RADIO_ENTITY)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_RADIO_ENTITYTYPE)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_EDIT_EVENT)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(!m_readOnly);
		GetDlgItem(IDC_EDIT_PLAN)->EnableWindow(!m_readOnly);
	}


    void EventPlanAssocDetailView::showItemDetails( bool isShown )
    {
        FUNCTION_ENTRY("showItemDetails");

        // TODO: When entity type is enabled uncomment controls below.

        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_EVENT)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_EVENT)->ShowWindow(isShown);
        //GetDlgItem(IDC_EDIT_ENTITYTYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PLAN)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PLAN)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ENTITY)->ShowWindow(isShown);
        GetDlgItem(IDC_EVENT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_PLAN_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_EVENTTYPE)->ShowWindow(isShown);
        //GetDlgItem(IDC_BUTTON_ENTITYTYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_ENTITY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_PLAN)->ShowWindow(isShown);
        //GetDlgItem(IDC_RADIO_ENTITYTYPE)->ShowWindow(isShown);
        //GetDlgItem(IDC_RADIO_ENTITY)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ENTITY_GROUP)->ShowWindow(isShown);
        GetDlgItem(IDC_ENTITY_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ENABLED)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_ENABLED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ENTITY)->ShowWindow(isShown);

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::refreshButtonAvailability()
    {
        if ( NULL == m_currentAssociation )
        {
            return;
        }

        if ( m_currentAssociation->isNew() )
        {
            // If it is new it is not possible to reset.
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        if( !m_entityType.isTextValid() || !m_entity.isTextValid() || !m_plan.isTextValid() || !m_eventType.isTextValid() )
        {
            // If any fields are invalid then enable the reset button but disable the apply button
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
        else if ( m_currentAssociation->hasChanged() )
        {
            // If all fields are valid and a change has been made enable both buttons.
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            // Otherwise disable both.
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }

    
    void EventPlanAssocDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        // Make sure the database accessor and list view pointers are valid.
        if ( ( NULL == m_databaseAccessor ) || ( NULL == m_listView ) )
        {
            m_databaseAccessor = dynamic_cast<EventPlanAssocDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<EventPlanAssocListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper EventPlanAssocDetailView");
        }

        // Convert the item to the correct type that can be used to get the event plan association
        // information as strings.
        m_currentAssociation = dynamic_cast<TA_Base_Core::IConfigEventPlanAssocData*>(currentItem);
        if ( NULL == m_currentAssociation )
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigEventPlanAssocData* object
            if ( m_currentAssociation->isEnabled() )
            {
                m_enabled.SetCheck(1);
            }
            else
            {
                m_enabled.SetCheck(0);
            }

            m_entityType.SetWindowText( m_currentAssociation->getEntityTypeName().c_str() );
            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_entity.SetWindowText( m_currentAssociation->getEntityName().c_str() );
            m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            //m_eventType.SetWindowText( m_currentAssociation->getEventTypeName().c_str() );
			m_eventType.SetWindowText( m_currentAssociation->getEventTypeDisplayName().c_str() );
            m_eventType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_plan.SetWindowText( m_currentAssociation->getPlanPath().c_str() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            // Set the state of the entity/entity type buttons and fields.
            refreshEntityAndEntityType();

            if( m_currentAssociation->isNew() )
            {
                // This is a new event plan association or at least a copy of an event plan association so we
                // indicate this in the type field and make sure the modify and create dates are blank.
                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
                m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);
				// ++ TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentAssociation->getDateModified();
                if ( 0 == modify )
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
                COleDateTime dateCreated ( m_currentAssociation->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474
            }

            // Show all of the fields.
            showItemDetails(true);

            // Set the initial state of the buttons
            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve event plan association details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve event plan association details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::refreshEntityAndEntityType()
    {
        if (( NULL == m_currentAssociation ) || ( m_readOnly ))
        {
            return;
        }

        // Refresh the state of the radio buttons and associated
        // text fields.
        if ( m_currentAssociation->isUsingEntity() )
        {
            GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(true);
            GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(false);
            m_entityRadio.SetCheck(1);
            m_entityTypeRadio.SetCheck(0);
        }
        else
        {
            GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(true);
            m_entityRadio.SetCheck(0);
            m_entityTypeRadio.SetCheck(1);
        }
    }


    void EventPlanAssocDetailView::onButtonEventtype() 
    {
        FUNCTION_ENTRY("onButtonEventtype");

        TA_ASSERT( m_databaseAccessor != NULL, "Database accessor is NULL" );

        if (m_currentAssociation == NULL)
        {
            return;
        }
        CWaitCursor wait;

        // Retrieve the current event type this association is using
        CString currentEventType("");
        m_eventType.GetWindowText( currentEventType );

        // Get the list of all event types.
        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllEventTypes();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_EVENTTYPE_DIALOG_TITLE, currentEventType, this);
        int response = dlg.DoModal();

        if ( IDOK != response )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new event type");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new event type which is %s", dlg.getSelectedName());

        try
        {
            // If this is a different entity type then set it in our database object.
            m_currentAssociation->setEventTypeKey( dlg.getSelectedKey() );

            // Now update the entity type the details view is showing
            m_eventType.SetWindowText( dlg.getSelectedName() );
            m_eventType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            // Update the list control to mark the association as modified if required.
            m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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


    void EventPlanAssocDetailView::onButtonEntitytype() 
    {
        FUNCTION_ENTRY("onButtonEntitytype");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if ( NULL == m_currentAssociation )
        {
            return;
        }
        CWaitCursor wait;

        // Retrieve the current entity type this association is using
        CString currentEntityType("");
        m_entityType.GetWindowText( currentEntityType );

        // Get the list of all entity types.
        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllEntityTypes();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_ENTITYTYPE_DIALOG_TITLE, currentEntityType, this);
        int response = dlg.DoModal();

        if ( IDOK != response )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new entity type");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new entity type which is %s", dlg.getSelectedName());

        try
        {
            // If this is a different entity type then set it in our database object.
            m_currentAssociation->setEntityTypeKey( dlg.getSelectedKey() );

            // Now update the entity type the details view is showing
            m_entityType.SetWindowText( dlg.getSelectedName() );
            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            // Update the list control to mark the association as modified if required.
            m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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


    void EventPlanAssocDetailView::onButtonEntity() 
    {
        FUNCTION_ENTRY("onButtonEntity");

        if (m_currentAssociation == NULL)
        {
            FUNCTION_EXIT;
            return;
        }
        CWaitCursor wait;

        // Retrieve the current entity this association is using
        CString currentEntity("");
        m_entity.GetWindowText( currentEntity );

        ChangeEntityDialog dlg( currentEntity, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new entity");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the entity %s", dlg.getSelectedName());

        try
        {
            // If this is a different entity then set it in our database object.
            m_currentAssociation->setEntityKey( dlg.getSelectedKey() );

            // Now update the entity name that the details view is showing
            m_entity.SetWindowText( dlg.getSelectedName() );
            m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            // Now update the row in the list view in the top pane of the splitter
    	    m_listView->itemDetailsChanged(m_currentAssociation->getUniqueIdentifier(),"");
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


    void EventPlanAssocDetailView::onButtonPlan() 
    {
        FUNCTION_ENTRY("onButtonPlan");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        std::string currentPlanPath;

        try
        {
            // Retrieve the current plan this association is using
            currentPlanPath = m_currentAssociation->getPlanPath();
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
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030001);
            FUNCTION_EXIT;
            return;
        }

        // Now construct the dialog and then wait for the user to finish with it. We must set the resource
        // handles up correctly so that the plan tree control can load icons correctly.
        HINSTANCE oldInstance = AfxGetResourceHandle();
        AfxSetResourceHandle( GetModuleHandle(EventPlanAssocComponent::DLL_NAME) );

        ChangePlanDialog dlg(currentPlanPath, this);
        int response = dlg.DoModal();

        AfxSetResourceHandle( oldInstance );

        if ( IDOK != response )
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new plan");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new plan which is %s", dlg.getSelectedPlanPath());

        try
        {
            // If this is a different plan then set it in our database object.
            m_currentAssociation->setPlanPath( std::string(dlg.getSelectedPlanPath()) );

            // Now update the entity type the details view is showing
            m_plan.SetWindowText( dlg.getSelectedPlanPath() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            // Update the list control to mark the association as modified if required.
            m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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


    void EventPlanAssocDetailView::onRadioEntity() 
    {
        FUNCTION_ENTRY("onRadioEntity");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        try
        {
            // We're using entity so blank the entity type key and fields
            m_currentAssociation->setEntityTypeKey(0);
            m_entityType.SetWindowText("");
            m_entityType.setTextType(TA_Base_Bus::TextColourEdit::VALID);

            // Set the state of the entity/entity type buttons and fields.
            refreshEntityAndEntityType();

            // Update the list control to mark the association as modified if required.
            m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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


    void EventPlanAssocDetailView::onRadioEntitytype() 
    {
        FUNCTION_ENTRY("onRadioEntitytype");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        try
        {
            // We're using entity type so blank the entity key and fields
            m_currentAssociation->setEntityKey(0);
            m_entity.SetWindowText("");
            m_entity.setTextType(TA_Base_Bus::TextColourEdit::VALID);

            // Set the state of the entity/entity type buttons and fields.
            refreshEntityAndEntityType();

            // Update the list control to mark the association as modified if required.
            m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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
    

    void EventPlanAssocDetailView::onKillfocusEditEvent() 
    {
        FUNCTION_ENTRY("onKillfocusEditEvent");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        // Get the current event type name.
        CString eventTypeString;
        m_eventType.GetWindowText(eventTypeString);

        try
        {
            // If the event type string has not changed then there is no point doing an update
            if ( eventTypeString == m_currentAssociation->getEventTypeDisplayName().c_str() )
            {
                m_eventType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAssociation->setEventTypeKeyFromName( std::string(eventTypeString) );
                m_eventType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Update the list control to mark the association as modified if required.
                m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            m_eventType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "The event type specified could not be found in the database");

            m_eventType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::onKillfocusEditEntity() 
    {
        FUNCTION_ENTRY("onKillfocusEditEntity");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        CString entityString;
        m_entity.GetWindowText(entityString);

        try
        {
            // If the event type string has not changed then there is no point doing an update
            if ( entityString == m_currentAssociation->getEntityName().c_str() )
            {
                m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAssociation->setEntityKeyFromName( std::string(entityString) );
                m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Update the list control to mark the association as modified if required.
                m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the entity could not be found in the database");

            m_entity.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::onKillfocusEditEntitytype() 
    {
        FUNCTION_ENTRY("onKillfocusEditEntitytype");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        CString entityTypeString;
        m_entityType.GetWindowText(entityTypeString);

        try
        {
            // If the event type string has not changed then there is no point doing an update
            if (entityTypeString == m_currentAssociation->getEntityTypeName().c_str())
            {
                m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAssociation->setEntityTypeKeyFromName( std::string(entityTypeString) );
                m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Update the list control to mark the association as modified if required.
                m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
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
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the entity type could not be found in the database");

            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }


        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::onKillfocusEditPlan() 
    {
        FUNCTION_ENTRY("onKillfocusEditPlan");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        CString planString;
        m_plan.GetWindowText(planString);

        try
        {
            // If the plan path string has not changed then there is no point doing an update
            if ( planString == m_currentAssociation->getPlanPath().c_str() )
            {
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                // Attempt to set the path as entered by the operator. The path will be
                // validated to ensure that it denotes an existing plan.
                m_currentAssociation->setPlanPath( std::string(planString) );

                // Misformatting of the plan path by users is allowed to a limited extent
                // (e.g. a missing '/' at the beginning is OK).
                // Now that the path is validated we can reset the plan edit box to display
                // the path in the correct format.
                m_plan.SetWindowText( m_currentAssociation->getPlanPath().c_str() );
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Update the list control to mark the association as modified if required.
                m_listView->itemDetailsChanged(m_currentAssociation->getUniqueIdentifier(),"");
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
        catch( const TA_Base_Core::DataException& ex)
        {
            CString errorMsg("Invalid path entered for the plan. ");
            errorMsg += ex.what();

            LOG_EXCEPTION_CATCH(SourceInfo, "Database", (LPCTSTR)errorMsg);

            m_plan.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }

    
    void EventPlanAssocDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        // Reset the state of the current association.
        resetButtonPressed( m_currentAssociation );

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::onButtonApply() 
    {
        FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        // Make sure all of the values are valid.
        if ( !m_entity.isTextValid()    || !m_entityType.isTextValid() || 
             !m_eventType.isTextValid() || !m_plan.isTextValid() )
        {
            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_INVALID_NAME);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName.GetBuffer(actionName.GetLength()) << reason.GetBuffer(reason.GetLength());
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }

        // Write the state of the current association to the database.
        applyButtonPressed( m_currentAssociation );

        // Force this call so our status column updates
        m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );

        FUNCTION_EXIT;
    }


    void EventPlanAssocDetailView::onCheckEnabled()
    {
        FUNCTION_ENTRY("onCheckEnabled");

        if ( NULL == m_currentAssociation )
        {
            return;
        }

        try
        {
            // Set the enabled state based off the state of the check box.
            m_currentAssociation->setEnabled( 0 != m_enabled.GetCheck() );

            // Update the list control to mark the association as modified if required.
            m_listView->itemDetailsChanged( m_currentAssociation->getUniqueIdentifier(), "" );
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // EventPlanAssocDetailView diagnostics

    #ifdef _DEBUG
    void EventPlanAssocDetailView::AssertValid() const
    {
        AbstractDetailView::AssertValid();
    }

    void EventPlanAssocDetailView::Dump(CDumpContext& dc) const
    {
        AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

