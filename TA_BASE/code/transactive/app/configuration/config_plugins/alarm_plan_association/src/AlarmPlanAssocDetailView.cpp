/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an alarm plan association's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_plan_association/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDetailView.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocDatabaseAccessor.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocListView.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocComponent.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/ChangePlanDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/plans/src/IConfigAlarmPlanAssoc.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 
#include "app/configuration/config_plugins/alarm_plan_association/src/ChineseSupport.h"
using namespace alarmPlanAsso_chinese;

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
   
    const int AlarmPlanAssocDetailView::ENTITY_FIELD_LENGTH = 80;
    const int AlarmPlanAssocDetailView::ENTITYTYPE_FIELD_LENGTH = 80;
    const int AlarmPlanAssocDetailView::PLAN_FIELD_LENGTH = 256;
    const int AlarmPlanAssocDetailView::ALARMTYPE_FIELD_LENGTH = 256;

    
    IMPLEMENT_DYNCREATE(AlarmPlanAssocDetailView, CFormView)


    AlarmPlanAssocDetailView::AlarmPlanAssocDetailView() : 
        m_currentAssociation(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(AlarmPlanAssocDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(AlarmPlanAssocDetailView)
	//}}AFX_DATA_INIT
    }


    AlarmPlanAssocDetailView::~AlarmPlanAssocDetailView()
    {
    }


    void AlarmPlanAssocDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AlarmPlanAssocDetailView)
	DDX_Control(pDX, IDC_ENTITY_BITMAP, m_bitmapEntity);
	DDX_Control(pDX, IDC_EDIT_PLAN, m_plan);
	DDX_Control(pDX, IDC_EDIT_ENTITYTYPE, m_entityType);
	DDX_Control(pDX, IDC_EDIT_ENTITY, m_entity);
	DDX_Control(pDX, IDC_EDIT_ALARM, m_alarmType);
	DDX_Control(pDX, IDC_PLAN_BITMAP, m_bitmapPlan);
	DDX_Control(pDX, IDC_ALARM_BITMAP, m_bitmapAlarmType);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_RADIO_ENTITYTYPE, m_entityTypeRadio);
	DDX_Control(pDX, IDC_RADIO_ENTITY, m_entityRadio);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AlarmPlanAssocDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(AlarmPlanAssocDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_ALARMTYPE, onButtonAlarmtype)
	ON_BN_CLICKED(IDC_BUTTON_ENTITYTYPE, onButtonEntitytype)
	ON_BN_CLICKED(IDC_BUTTON_ENTITY, onButtonEntity)
	ON_BN_CLICKED(IDC_BUTTON_PLAN, onButtonPlan)
	ON_BN_CLICKED(IDC_RADIO_ENTITY, onRadioEntity)
	ON_BN_CLICKED(IDC_RADIO_ENTITYTYPE, onRadioEntitytype)
	ON_EN_KILLFOCUS(IDC_EDIT_ALARM, onKillfocusEditAlarm)
	ON_EN_KILLFOCUS(IDC_EDIT_ENTITY, onKillfocusEditEntity)
	ON_EN_KILLFOCUS(IDC_EDIT_ENTITYTYPE, onKillfocusEditEntitytype)
	ON_EN_KILLFOCUS(IDC_EDIT_PLAN, onKillfocusEditPlan)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AlarmPlanAssocDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_entityType.SetLimitText(ENTITYTYPE_FIELD_LENGTH);
        m_entity.SetLimitText(ENTITY_FIELD_LENGTH);
        m_alarmType.SetLimitText(ALARMTYPE_FIELD_LENGTH);
        m_plan.SetLimitText(PLAN_FIELD_LENGTH);

        // Hide all the alarm plan association details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocDetailView::showItemDetails(bool isShown)
    {
         // Show or hide all the controls from the view
         GetDlgItem(IDC_STATIC_ALARM)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_ALARM)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_ENTITYTYPE)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_PLAN)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_PLAN)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_ENTITY)->ShowWindow(isShown);
         GetDlgItem(IDC_ALARM_BITMAP)->ShowWindow(isShown);
         GetDlgItem(IDC_PLAN_BITMAP)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_ALARMTYPE)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_ENTITYTYPE)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_ENTITY)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_PLAN)->ShowWindow(isShown);
         GetDlgItem(IDC_RADIO_ENTITYTYPE)->ShowWindow(isShown);
         GetDlgItem(IDC_RADIO_ENTITY)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_ENTITY_GROUP)->ShowWindow(isShown);
         GetDlgItem(IDC_ENTITY_BITMAP)->ShowWindow(isShown);
    }


    void AlarmPlanAssocDetailView::refreshButtonAvailability()
    {
        if (m_currentAssociation == NULL)
        {
            return;
        }

        if (m_currentAssociation->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if( !m_entityType.isTextValid() || !m_entity.isTextValid() || !m_plan.isTextValid() || !m_alarmType.isTextValid() )
        {
            // If any fields are invalid then enable the reset button but disable the apply button
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
        else if (m_currentAssociation->hasChanged())
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

    
    void AlarmPlanAssocDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AlarmPlanAssocDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AlarmPlanAssocListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AlarmPlanAssocDetailView");
        }


        m_currentAssociation = dynamic_cast<TA_Base_Core::IConfigAlarmPlanAssoc*>(currentItem);

        if (m_currentAssociation == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigAlarmPlanAssoc* object
            m_alarmType.SetWindowText( m_currentAssociation->getAlarmTypeDisplayName().c_str() );
            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_plan.SetWindowText( m_currentAssociation->getPlanPath().c_str() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_entityType.SetWindowText( m_currentAssociation->getEntityTypeName().c_str() );
            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_entity.SetWindowText( m_currentAssociation->getEntityName().c_str() );
            m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            refreshEntityAndEntityType();

            if( m_currentAssociation->isNew() )
            {

                // This is a new alarm plan association or at least a copy of an alarm plan association so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                refreshEntityAndEntityType();

                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {

                m_type.SetWindowText( NO_VALUE );

				// TD12474 ++
// 				TA_Base_Core::DateFormat dateFormat; 
// 				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				// ++ TD12474
				std::string dateFormatStr = DATE_FORMAT_STR;

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentAssociation->getDateModified();
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
                COleDateTime dateCreated ( m_currentAssociation->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474
            }
            
            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm plan association details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm plan association details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocDetailView::refreshEntityAndEntityType()
    {        
        if (m_currentAssociation == NULL)
        {
            return;
        }

        if (m_currentAssociation->isUsingEntity())
        {
            GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(true);
            GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(false);
            m_entityRadio.SetCheck(1);
            m_entityTypeRadio.SetCheck(0);
        }
        else // using entity type
        {
            GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(true);
            m_entityRadio.SetCheck(0);
            m_entityTypeRadio.SetCheck(1);
        }
    }


    void AlarmPlanAssocDetailView::onButtonAlarmtype() 
    {
        FUNCTION_ENTRY("onButtonAlarmtype");
        CWaitCursor wait;
	    
        // Retrieve the current alarm type this association is using
        CString currentAlarmType("");
        m_alarmType.GetWindowText( currentAlarmType );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllAlarmTypeDisplays();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_ALARMTYPE_DIALOG_TITLE, currentAlarmType, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new alarm type");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new alarm type which is %s", dlg.getSelectedName());

        try
        {
            
            if (m_currentAssociation == NULL)
            {
                return;
            }

            // If this is a different entity type then set it in our database object.
            //std::ostringstream alarmTypeString;
            //alarmTypeString << dlg.getSelectedKey();
            m_currentAssociation->setAlarmTypeKey( dlg.getSelectedKey());

            // Now update the entity type the details view is showing
            m_alarmType.SetWindowText( dlg.getSelectedName() );
            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

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


    void AlarmPlanAssocDetailView::onButtonEntitytype() 
    {
        FUNCTION_ENTRY("onButtonEntitytype");

        if (m_currentAssociation == NULL)
        {
            return;
        }
        CWaitCursor wait;
	    
        // Retrieve the current entity type this association is using
        CString currentEntityType("");
        m_entityType.GetWindowText( currentEntityType );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllEntityTypes();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_ENTITYTYPE_DIALOG_TITLE, currentEntityType, this);
        int response = dlg.DoModal();

        if (response != IDOK)
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


    void AlarmPlanAssocDetailView::onButtonEntity() 
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


    void AlarmPlanAssocDetailView::onButtonPlan() 
    {
        FUNCTION_ENTRY("onButtonPlan");

        CWaitCursor wait;
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
        AfxSetResourceHandle( GetModuleHandle(AlarmPlanAssocComponent::DLL_NAME) );

        ChangePlanDialog dlg(currentPlanPath, this);
        int response = dlg.DoModal();

        AfxSetResourceHandle( oldInstance );

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new plan");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new plan which is %s", dlg.getSelectedPlanPath());

        try
        {
            if (m_currentAssociation == NULL)
            {
                return;
            }

            // If this is a different Plan then set it in our database object.
            m_currentAssociation->setPlanPath( std::string(dlg.getSelectedPlanPath()) );

            // Now update the Plan the details view is showing
            m_plan.SetWindowText( dlg.getSelectedPlanPath() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

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


    void AlarmPlanAssocDetailView::onRadioEntity() 
    {
        FUNCTION_ENTRY("onRadioEntity");

        if (m_currentAssociation == NULL)
        {
            return;
        }

        try
        {
            // We're using entity so blank the entity type key and fields
            m_currentAssociation->setEntityTypeKey(0);

            refreshEntityAndEntityType();

            m_entityType.SetWindowText("");
            m_entityType.setTextType(TA_Base_Bus::TextColourEdit::VALID);

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


    void AlarmPlanAssocDetailView::onRadioEntitytype() 
    {
        FUNCTION_ENTRY("onRadioEntitytype");

        if (m_currentAssociation == NULL)
        {
            return;
        }

        try
        {
            // We're using entity type so blank the entity key and fields
            m_currentAssociation->setEntityKey(0);

            refreshEntityAndEntityType();

            m_entity.SetWindowText("");
            m_entity.setTextType(TA_Base_Bus::TextColourEdit::VALID);

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
    

    void AlarmPlanAssocDetailView::onKillfocusEditAlarm() 
    {
        FUNCTION_ENTRY("onKillfocusEditAlarm");

        if (m_currentAssociation == NULL)
        {
            return;
        }

        CString alarmTypeString;
        m_alarmType.GetWindowText(alarmTypeString);

        try
        {
            // If the alarm type string has not changed then there is no point doing an update
            if (alarmTypeString == m_currentAssociation->getAlarmTypeDisplayName().c_str())
            {
                m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAssociation->setAlarmTypeByDisplayName( std::string(alarmTypeString) );

                m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                m_listView->itemDetailsChanged(m_currentAssociation->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "The alarm type specified could not be found in the database");

            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void AlarmPlanAssocDetailView::onKillfocusEditEntity() 
    {
        FUNCTION_ENTRY("onKillfocusEditEntity");

        if (m_currentAssociation == NULL)
        {
            return;
        }

        CString entityString;
        m_entity.GetWindowText(entityString);

        try
        {
            // If the alarm type string has not changed then there is no point doing an update
            if (entityString == m_currentAssociation->getEntityName().c_str())
            {
                m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAssociation->setEntity( std::string(entityString) );

                m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                
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
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the entity could not be found in the database");

            m_entity.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void AlarmPlanAssocDetailView::onKillfocusEditEntitytype() 
    {
        FUNCTION_ENTRY("onKillfocusEditEntitytype");
	    
        if (m_currentAssociation == NULL)
        {
            return;
        }

        CString entityTypeString;
        m_entityType.GetWindowText(entityTypeString);

        try
        {
            // If the alarm type string has not changed then there is no point doing an update
            if (entityTypeString == m_currentAssociation->getEntityTypeName().c_str())
            {
                m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAssociation->setEntityType( std::string(entityTypeString) );

                m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
	    
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
        catch( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the entity type could not be found in the database");

            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }


        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void AlarmPlanAssocDetailView::onKillfocusEditPlan() 
    {
        FUNCTION_ENTRY("onKillfocusEditPlan");
	    
        if (m_currentAssociation == NULL)
        {
            return;
        }

        CString planString;
        m_plan.GetWindowText(planString);

        try
        {
            // If the plan path string has not changed then there is no point doing an update
            if (planString == m_currentAssociation->getPlanPath().c_str())
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
                m_plan.SetWindowText(m_currentAssociation->getPlanPath().c_str());
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );
                
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

    
    void AlarmPlanAssocDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentAssociation);

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        if ( !m_entity.isTextValid()    || !m_entityType.isTextValid() || 
             !m_alarmType.isTextValid() || !m_plan.isTextValid() )
        {
            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_INVALID_NAME);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName.GetBuffer(actionName.GetLength()) << reason.GetBuffer(reason.GetLength());
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }
        applyButtonPressed(m_currentAssociation);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_currentAssociation->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AlarmPlanAssocDetailView diagnostics

    #ifdef _DEBUG
    void AlarmPlanAssocDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void AlarmPlanAssocDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

