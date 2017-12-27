/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/AlarmRulesDetailView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This view allows the user to configure an alarm rule's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesDetailView.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesDatabaseAccessor.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesListView.h"
#include "app/configuration/config_plugins/alarm_rules/src/AlarmRulesComponent.h"
#include "app/configuration/config_plugins/alarm_rules/src/ChangePlanDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/alarm_rule/src/IAlarmRuleData.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"
#include "core/data_access_interface/alarm_rule/src/AvalancheSuppressionRuleData.h"
#include "core/data_access_interface/alarm_rule/src/PlanExecutionRuleData.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 
#include "app/configuration/config_plugins/alarm_rules/src/ChineseSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::AvalancheSuppressionRuleData;
using TA_Base_Core::PlanExecutionRuleData;
using namespace TA_Base_App::TA_Configuration;
using namespace alarmrule_chinese;

namespace TA_Base_App
{
    // The titles for the change dialog box


    const int AlarmRulesDetailView::ENTITY_FIELD_LENGTH     = 80;
    const int AlarmRulesDetailView::ENTITYTYPE_FIELD_LENGTH = 80;
    const int AlarmRulesDetailView::ALARMTYPE_FIELD_LENGTH  = 256;
    const int AlarmRulesDetailView::TIMEOUT_FIELD_LENGTH    = 10;
    const int AlarmRulesDetailView::TIMEIN_FIELD_LENGTH     = 10;
    const int AlarmRulesDetailView::MINALARMS_FIELD_LENGTH  = 6;
    const int AlarmRulesDetailView::PLAN_FIELD_LENGTH       = 256;

    
    const bool AlarmRulesDetailView::SELECTED_RADIO_ENTITY       = true;
    const bool AlarmRulesDetailView::SELECTED_RADIO_ENTITY_TYPE  = ! SELECTED_RADIO_ENTITY;


    
    IMPLEMENT_DYNCREATE(AlarmRulesDetailView, CFormView)


    AlarmRulesDetailView::AlarmRulesDetailView() : 
        m_currentAlarmRule(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        m_selectedRadio(SELECTED_RADIO_ENTITY),
        AbstractDetailView(AlarmRulesDetailView::IDD)
    {
        //{{AFX_DATA_INIT(AlarmRulesDetailView)
        //}}AFX_DATA_INIT
    }


    AlarmRulesDetailView::~AlarmRulesDetailView()
    {
    }


    void AlarmRulesDetailView::DoDataExchange(CDataExchange* pDX)
    {
        CFormView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(AlarmRulesDetailView)
        DDX_Control(pDX, IDC_COMBO_TRIGGER, m_trigger);
        DDX_Control(pDX, IDC_EDIT_ENTITYTYPE, m_entityType);
        DDX_Control(pDX, IDC_EDIT_ENTITY, m_entity);
        DDX_Control(pDX, IDC_EDIT_ALARM, m_alarmType);
		DDX_Control(pDX, IDC_ENTITYTYPE_BITMAP, m_btimapEntityType); //TD18279 zhiqiang++
		DDX_Control(pDX, IDC_ENTITY_BITMAP, m_bitmapEntity); //TD18279 zhiqiang++
		DDX_Control(pDX, IDC_ALARMTYPE_BITMAP, m_bitmapAlarmType); //TD18279 zhiqiang++
        DDX_Control(pDX, IDC_TRIGGER_BITMAP, m_bitmapTrigger);
        DDX_Control(pDX, IDC_PLAN_BITMAP, m_bitmapPlan);
        DDX_Control(pDX, IDC_TIMEOUT_BITMAP, m_bitmapTimeout);
        DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
        DDX_Control(pDX, IDC_STATIC_DATE_MODIFIED, m_dateModifiedName);
        DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_STATIC_DATE_CREATED, m_dateCreatedName);
        DDX_Control(pDX, IDC_RADIO_ENTITYTYPE, m_entityTypeRadio);
        DDX_Control(pDX, IDC_RADIO_ENTITY, m_entityRadio);
        DDX_Control(pDX, IDC_BUTTON_APPLY, m_applyButton);
        DDX_Control(pDX, IDC_BUTTON_RESET, m_resetButton);
        DDX_Control(pDX, IDC_BUTTON_PLAN, m_planButton);
        DDX_Control(pDX, IDC_EDIT_PLAN, m_plan);
        DDX_Control(pDX, IDC_EDIT_TIMEOUT, m_timeout);
        DDX_Control(pDX, IDC_EDIT_TIMEIN, m_timeIn);
        DDX_Control(pDX, IDC_EDIT_MINALARMS, m_minAlarms);
        DDX_Control(pDX, IDC_LIST_AV_SUPP, m_avalancheSuppressionList);
		// TD 11796 "Add" button is taken out as all entities should be some form of data nodes/pts
        // DDX_Control(pDX, IDC_BUTTON_ADD, m_addChildButton);
        DDX_Control(pDX, IDC_BUTTON_REMOVE, m_removeChildButton);
        DDX_Control(pDX, IDC_BUTTON_ADD_DATA_NODE, m_addDataNodeButton);
        //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AlarmRulesDetailView, AbstractDetailView)
    //{{AFX_MSG_MAP(AlarmRulesDetailView)
    ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_BN_CLICKED(IDC_BUTTON_ALARMTYPE, onButtonAlarmtype)
    ON_BN_CLICKED(IDC_BUTTON_ENTITYTYPE, onButtonEntitytype)
    ON_BN_CLICKED(IDC_BUTTON_ENTITY, onButtonEntity)
    ON_BN_CLICKED(IDC_BUTTON_PLAN, onButtonPlan)
	// TD 11796 "Add" button is taken out as all entities should be some form of data nodes/pts
    // ON_BN_CLICKED(IDC_BUTTON_ADD, onButtonAddChild)
    ON_BN_CLICKED(IDC_BUTTON_REMOVE, onButtonRemoveChild)
    ON_BN_CLICKED(IDC_BUTTON_ADD_DATA_NODE, onButtonAddDataNode)
    ON_BN_CLICKED(IDC_RADIO_ENTITY, onRadioEntity)
    ON_BN_CLICKED(IDC_RADIO_ENTITYTYPE, onRadioEntitytype)
    ON_EN_KILLFOCUS(IDC_EDIT_ALARM, onKillfocusEditAlarm)
    ON_EN_KILLFOCUS(IDC_EDIT_ENTITY, onKillfocusEditEntity)
    ON_EN_KILLFOCUS(IDC_EDIT_ENTITYTYPE, onKillfocusEditEntitytype)
    ON_EN_KILLFOCUS(IDC_EDIT_TIMEOUT, onKillfocusEditTimeout)
    ON_EN_KILLFOCUS(IDC_EDIT_TIMEIN, onKillfocusEditTimeIn)
    ON_EN_KILLFOCUS(IDC_EDIT_MINALARMS, onKillfocusEditMinAlarms)
    ON_EN_KILLFOCUS(IDC_EDIT_PLAN, onKillfocusEditPlan)
    ON_CBN_SELCHANGE(IDC_COMBO_TRIGGER, onSelectionChangeComboTrigger)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AlarmRulesDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

        CFormView::OnInitialUpdate();
        
        m_entityType.SetLimitText(ENTITYTYPE_FIELD_LENGTH);
        m_entity.SetLimitText(ENTITY_FIELD_LENGTH);
        m_alarmType.SetLimitText(ALARMTYPE_FIELD_LENGTH);
        m_timeout.SetLimitText(TIMEOUT_FIELD_LENGTH);
        m_timeIn.SetLimitText(TIMEIN_FIELD_LENGTH);
        m_minAlarms.SetLimitText(MINALARMS_FIELD_LENGTH);
        m_plan.SetLimitText(PLAN_FIELD_LENGTH);

        
//         std::vector<std::string> triggers =
//             TA_Base_Core::AlarmRuleAccessFactory::getInstance().getAllRuleTriggers();
// 
// 
//         for(std::vector<std::string>::iterator it = triggers.begin();
//             it != triggers.end();
//             it ++)
//         {
//             m_trigger.AddString((*it).c_str());
//         }

		m_trigger.AddString(CString(TRIGGER_SUBMIT.c_str()));
		m_trigger.AddString(CString(TRIGGER_CLOSE.c_str()));
		m_trigger.AddString(CString(TRIGGER_ACKNOWLEDGE.c_str()));

        // Hide all the alarm rule details until we are told to populate.
        showItemDetails(false);

        // The position of the date fields and apply/reset buttons needs to be
        // saved the first time they are layed out. This indicates whether
        // their position has been saved since the page was last refreshed
        m_windowPositionsSaved = false;

        FUNCTION_EXIT;
    }



    void AlarmRulesDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_ALARM)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ALARM)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ENTITYTYPE)->ShowWindow(isShown);
		GetDlgItem(IDC_ENTITYTYPE_BITMAP)->ShowWindow(isShown); //TD18279 zhiqiang++
		GetDlgItem(IDC_ALARMTYPE_BITMAP)->ShowWindow(isShown); //TD18279 zhiqiang++
		GetDlgItem(IDC_ENTITY_BITMAP)->ShowWindow(isShown); //TD18279 zhiqiang++
        GetDlgItem(IDC_COMBO_TRIGGER)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TRIGGER)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ENTITY)->ShowWindow(isShown);
        GetDlgItem(IDC_TRIGGER_BITMAP)->ShowWindow(isShown);
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
        GetDlgItem(IDC_RADIO_ENTITYTYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_RADIO_ENTITY)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ENTITY_GROUP)->ShowWindow(isShown);

        // If the common item details are not being shown,
        // the other item details must not be shown either.
        if(!isShown)
        {
            showPlanItemDetails(isShown);
            showAvalancheItemDetails(isShown);
        }
    }


    void AlarmRulesDetailView::showPlanItemDetails(bool isShown)
    {
        GetDlgItem(IDC_STATIC_PLAN)->ShowWindow(isShown);
        GetDlgItem(IDC_PLAN_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PLAN)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_PLAN)->ShowWindow(isShown);
    }

    void AlarmRulesDetailView::showAvalancheItemDetails(bool isShown)
    {
        GetDlgItem(IDC_STATIC_TIMEOUT)->ShowWindow(isShown);
        GetDlgItem(IDC_TIMEOUT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_TIMEOUT)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TIMEOUT_SECONDS)->ShowWindow(isShown);
		/*  timeIn being canceled at C955
        GetDlgItem(IDC_STATIC_TIMEIN)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_TIMEIN)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TIMEIN_MSECONDS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MINALARMS)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_MINALARMS)->ShowWindow(isShown);
		*/
        GetDlgItem(IDC_LIST_AV_SUPP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_GROUP_AV_SUPP)->ShowWindow(isShown);
        // TD 11796 "Add" button is taken out as all entities should be some form of data nodes/pts
		// GetDlgItem(IDC_BUTTON_ADD)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_REMOVE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_ADD_DATA_NODE)->ShowWindow(isShown);
    }

    void AlarmRulesDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<AlarmRulesDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<AlarmRulesListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper AlarmRulesDetailView");
        }


        if(currentItem == NULL)
        {
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        m_currentAlarmRule = dynamic_cast<TA_Base_Core::IAlarmRuleData*>(currentItem);

        TA_ASSERT(m_currentAlarmRule != NULL,
                  "The alarm rule must be an IAlarmRuleData");


        // If this is the first time this method has been called since this
        // page was loaded, save the position of the date fields and
        // apply/reset buttons so they can be moved and returned to their
        // original location.
        // Note that this can't be done in onInitialUpdate as not everything
        // has been layed out yet.
        if(!m_windowPositionsSaved)
        {
            saveItemPositions();
            m_windowPositionsSaved = true;
        }


        try
        {
            showPlanItemDetails(false);
            showAvalancheItemDetails(false);

            std::string ruleType = m_currentAlarmRule->getRuleType();

            // The trigger field is only editable for plan execution rules.
            m_trigger.EnableWindow(false);


            if(ruleType == AvalancheSuppressionRuleData::RULE_TYPE)
			//if(ruleType == AVA_SUPPRE_RULE)
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"Populating avalanche suppression rule fields");

                // Avalanche Suppression rules have some extra fields
                layoutAvalancheSuppressionRuleDetails();

                AvalancheSuppressionRuleData* data =
                    dynamic_cast<AvalancheSuppressionRuleData*>(m_currentAlarmRule);

                TA_ASSERT(data != NULL,
                          "Avalanche Suppression Rules should be the only "
                          "rules with that rule type.");

                // Display the timeout value
                unsigned long timeout = data->getTimeout();
                std::stringstream stream;
                stream << timeout;
                m_timeout.SetWindowText(stream.str().c_str());
                m_timeout.setTextType(TA_Base_Bus::TextColourEdit::VALID);

                // Display the timein value
                unsigned long timein = data->getTimein();
                stream.str("");
                stream << timein;
                m_timeIn.SetWindowText(stream.str().c_str());
                m_timeIn.setTextType(TA_Base_Bus::TextColourEdit::VALID);

                // display the minAlarms value
                unsigned long minAlarms = data->getMinAlarms();
                stream.str("");
                stream << minAlarms;
                m_minAlarms.SetWindowText(stream.str().c_str());
                m_minAlarms.setTextType(TA_Base_Bus::TextColourEdit::VALID);

                // Populate the list of children of the avalanche
                // suppression rule
                m_avalancheSuppressionList.populateControl(
                    data, m_databaseAccessor);
            }
            else if(ruleType == PlanExecutionRuleData::RULE_TYPE)
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"Populating plan execution rule fields");

                // Plan Execution rules have some extra fields
                PlanExecutionRuleData* data =
                    dynamic_cast<PlanExecutionRuleData*>(m_currentAlarmRule);

                TA_ASSERT(data != NULL,
                          "Plan Execution Rules should be the only "
                          "rules with that rule type");

                m_plan.SetWindowText(data->getPlanPath().c_str());
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                layoutPlanExecutionDetails();

                // The trigger field is only editable for plan execution rules.
                m_trigger.EnableWindow(true);
            }
            else
            {
				LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,
					"Populating common details");

                layoutCommonDetails();
            }


            //  Now populate all the fields with the data retrieved from the IAlarmRuleData* object
            //m_alarmType.SetWindowText( m_currentAlarmRule->getAlarmTypeName().c_str() );
			 m_alarmType.SetWindowText( m_currentAlarmRule->getAlarmTypeDisplayName().c_str() );
            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            std::string ruleTrigger = m_currentAlarmRule->getRuleTrigger();
            if( ! ruleTrigger.empty() )
            {
                m_trigger.SelectString( -1, ruleTrigger.c_str() );
            }
			
			//check for entityTypeName not found
			std::string entityTypeName;
			try
			{

				entityTypeName = m_currentAlarmRule->getEntityTypeName();
			}
			catch ( const TA_Base_Core::DataException& )
			{
				
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
					"Entity with key %d does not exist. Setting value to empty.",
					m_currentAlarmRule->getEntityTypeKey());
				
				entityTypeName	= "";
				
			}


            m_entityType.SetWindowText( entityTypeName.c_str() );
            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );


			//check for entityName not found
			std::string entityName;
			try
			{
				 entityName = m_currentAlarmRule->getEntityName();
			}
			catch ( const TA_Base_Core::DataException& )
			{
				
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
					"Entity with key %d does not exist. Setting value to empty.",
					m_currentAlarmRule->getEntityKey());
				
				entityName	= "";
				
			}

			//m_entity.SetWindowText( m_currentAlarmRule->getEntityName().c_str() );
			m_entity.SetWindowText( entityName.c_str() );
			m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            refreshEntityAndEntityType();

            if( m_currentAlarmRule->isNew() )
            {
                // This is a new alarm rule or at least a copy of an alarm rule
                // so we indicate this in the type field and make sure the
                // modify and create dates are blank.
                refreshEntityAndEntityType();

                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
                m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

				// TD12474 ++
				//TA_Base_Core::DateFormat dateFormat; 
				//std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				std::string dateFormatStr = DATE_FORMAT_STR;
				// ++ TD12474

                // Retrieve the modify time and date.
                // If this is zero then one hasn't been set yet so leave the
                // modified field as blank
                time_t modify = m_currentAlarmRule->getDateModified();
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

                // The create time should always be set so we don't need to
                // perform the same checks as the modify date.
                COleDateTime dateCreated ( m_currentAlarmRule->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474 
            }


            // Show the relevant items, depending on what the alarm rule type is
            showItemDetails(true);
            
            if(ruleType == AvalancheSuppressionRuleData::RULE_TYPE)
            {
                showAvalancheItemDetails(true);
            }
            else if(ruleType == PlanExecutionRuleData::RULE_TYPE)
            {
                showPlanItemDetails(true);
            }

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve alarm rule details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm rule details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::refreshButtonAvailability()
    {
        if (m_currentAlarmRule == NULL)
        {
            return;
        }

        if (m_currentAlarmRule->isNew() )
        {
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            return;
        }
        
        // We have an old item
        if( !m_entityType.isTextValid() || !m_entity.isTextValid() || !m_plan.isTextValid() ||
            !m_alarmType.isTextValid()  || !m_timeout.isTextValid() || !m_timeIn.isTextValid() ||
            !m_minAlarms.isTextValid())
        {
            // If any fields are invalid then enable the reset button but disable the apply button
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
        else if (m_currentAlarmRule->hasChanged())
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
    
    
    void AlarmRulesDetailView::saveItemPositions()
    {
        m_dateCreatedName.GetWindowRect(&m_dateCreatedNameRect);
        m_createDate.GetWindowRect(&m_createDateRect);
        m_dateModifiedName.GetWindowRect(&m_dateModifiedNameRect);
        m_modifyDate.GetWindowRect(&m_modifyDateRect);
        m_applyButton.GetWindowRect(&m_applyButtonRect);
        m_resetButton.GetWindowRect(&m_resetButtonRect);
        

        AlarmRulesDetailView::ScreenToClient(&m_dateCreatedNameRect);
        AlarmRulesDetailView::ScreenToClient(&m_createDateRect);
        AlarmRulesDetailView::ScreenToClient(&m_dateModifiedNameRect);
        AlarmRulesDetailView::ScreenToClient(&m_modifyDateRect);
        AlarmRulesDetailView::ScreenToClient(&m_applyButtonRect);
        AlarmRulesDetailView::ScreenToClient(&m_resetButtonRect);

        
        // Save the y-coordinate of the bottom of the trigger combo
        m_originalTriggerBottom = getWindowBottom(m_trigger);

        // Find the y-coordinate of the bottom of the plan edit
        int planBottom = getWindowBottom(m_plan);
        
        // Find the y-coordinate of the bottom of the avalanche suppression
        // list view
        int avalancheGroupBottom = getWindowBottom(
                                       *GetDlgItem(IDC_STATIC_GROUP_AV_SUPP));

        // Find the amount to move the date fields and apply/reset buttons
        // down when the plan and avalanche suppression rule types are being
        // shown.
        m_planYDistance      = planBottom - m_originalTriggerBottom;
        m_avalancheYDistance = avalancheGroupBottom - m_originalTriggerBottom;
    }


    void AlarmRulesDetailView::layoutAvalancheSuppressionRuleDetails()
    {
        // Move the fields to their default position
        moveDateAndButtonsToDefaultPosition();

        // Move the fields to their new position
        moveWindowDown(m_dateCreatedName,  m_avalancheYDistance);
        moveWindowDown(m_createDate,       m_avalancheYDistance);
        moveWindowDown(m_dateModifiedName, m_avalancheYDistance);
        moveWindowDown(m_modifyDate,       m_avalancheYDistance);
        moveWindowDown(m_applyButton,      m_avalancheYDistance);
        moveWindowDown(m_resetButton,      m_avalancheYDistance);

        // Resize the border
        resizeBorder();
    }


    void AlarmRulesDetailView::layoutPlanExecutionDetails()
    {
        // Move the fields to their default position
        moveDateAndButtonsToDefaultPosition();

        // Move the fields to their new position
        moveWindowDown(m_dateCreatedName,  m_planYDistance);
        moveWindowDown(m_createDate,       m_planYDistance);
        moveWindowDown(m_dateModifiedName, m_planYDistance);
        moveWindowDown(m_modifyDate,       m_planYDistance);
        moveWindowDown(m_applyButton,      m_planYDistance);
        moveWindowDown(m_resetButton,      m_planYDistance);

        // Resize the border
        resizeBorder();
    }


    void AlarmRulesDetailView::layoutCommonDetails()
    {
        moveDateAndButtonsToDefaultPosition();
        
        // Resize the border
        resizeBorder();
    }

    void AlarmRulesDetailView::resizeBorder()
    {
        CRect rect;
        GetDlgItem(IDC_STATIC_TYPE)->GetWindowRect(&rect);
        AlarmRulesDetailView::ScreenToClient(&rect);
        
        // Get the y-coordinate of the bottom of the apply button
        int applyBottom = getWindowBottom(m_applyButton);

        // The bottom of the border should be 20 pixels below the apply button
        rect.bottom = applyBottom + 20;

        GetDlgItem(IDC_STATIC_TYPE)->MoveWindow(rect);
    }

    void AlarmRulesDetailView::moveDateAndButtonsToDefaultPosition()
    {
        m_dateCreatedName.MoveWindow(&m_dateCreatedNameRect);
        m_createDate.MoveWindow(&m_createDateRect);
        m_dateModifiedName.MoveWindow(&m_dateModifiedNameRect);
        m_modifyDate.MoveWindow(&m_modifyDateRect);
        m_applyButton.MoveWindow(&m_applyButtonRect);
        m_resetButton.MoveWindow(&m_resetButtonRect);


        // Get the y-coordinate of the bottom of the trigger combo
        int triggerBottom = getWindowBottom(m_trigger);

        // The difference between the original trigger position and the
        // position now is the amount to move the date fields and apply/reset
        // buttons down. (Note this value can be negative)
        int amountToMoveY = triggerBottom - m_originalTriggerBottom;

        moveWindowDown(m_dateCreatedName,  amountToMoveY);
        moveWindowDown(m_createDate,       amountToMoveY);
        moveWindowDown(m_dateModifiedName, amountToMoveY);
        moveWindowDown(m_modifyDate,       amountToMoveY);
        moveWindowDown(m_applyButton,      amountToMoveY);
        moveWindowDown(m_resetButton,      amountToMoveY);
    }


    void AlarmRulesDetailView::moveWindowDown(CWnd& window, int distancePixels)
    {
        CRect rect;
        window.GetWindowRect(&rect);
        AlarmRulesDetailView::ScreenToClient(&rect);
        
        rect.top += distancePixels;
        rect.bottom += distancePixels;

        window.MoveWindow(&rect);
    }



    int AlarmRulesDetailView::getWindowBottom(const CWnd& window)
    {
        // Find the y-coordinate of the bottom of the window
        RECT windowRect;
        window.GetWindowRect(&windowRect);
        AlarmRulesDetailView::ScreenToClient(&windowRect);

        return windowRect.bottom;
    }


    void AlarmRulesDetailView::refreshEntityAndEntityType()
    {
		FUNCTION_ENTRY("refreshEntityAndEntityType");

        if (m_currentAlarmRule == NULL)
        {
			FUNCTION_EXIT;
            return;
        }
        
        // Work out whether to enable entity or entity type
        bool radioToSelect = true;

        try
        {
            // If both entity and entity type are blank, go with
            // whatever is set in the m_selectedRadio variable.
            if(m_currentAlarmRule->getEntityKey() == 0 &&
               m_currentAlarmRule->getEntityTypeKey() == 0)
            {
                radioToSelect = m_selectedRadio;
            }
            // If only one of the two is blank, select the other
            else
            {
                if(m_currentAlarmRule->getEntityKey() == 0)
                {
                    radioToSelect = SELECTED_RADIO_ENTITY_TYPE;
                }
                else
                {
                    radioToSelect = SELECTED_RADIO_ENTITY;
                }
            }
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
       

        if (radioToSelect == SELECTED_RADIO_ENTITY)
        {
            GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(true);
            GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(false);
            m_entityRadio.SetCheck(1);
            m_entityTypeRadio.SetCheck(0);
        }
        // using entity type
        else
        {
            GetDlgItem(IDC_EDIT_ENTITY)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_ENTITYTYPE)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_ENTITY)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_ENTITYTYPE)->EnableWindow(true);
            m_entityRadio.SetCheck(0);
            m_entityTypeRadio.SetCheck(1);
        }

		FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onButtonAlarmtype() 
    {
        FUNCTION_ENTRY("onButtonAlarmtype");
        
        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the current alarm type this rule is using
        CString currentAlarmType("");
        m_alarmType.GetWindowText( currentAlarmType );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            CWaitCursor wait;
//             ChangeDialog::ItemsForSelection items =
//                 m_databaseAccessor->getAllAlarmTypes();
			ChangeDialog::ItemsForSelection items =
				m_databaseAccessor->getAllAlarmTypeDisplays();

            // Now construct the dialog and then wait for the user to
            // finish with it
            ChangeDialog dlg(
                items, CHANGE_ALARMTYPE_DIALOG_TITLE, currentAlarmType, this);
            int response = dlg.DoModal();

            if (response != IDOK)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                             "User cancelled the selection of a new alarm type");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "User selected a new alarm type which is %s",
                         dlg.getSelectedName());

            TA_ASSERT(m_currentAlarmRule !=  NULL, "Current rule is NULL.");

            // If this is a different alarm type then set it in our
            // database object.
            //std::ostringstream alarmTypeString;
            //alarmTypeString << dlg.getSelectedKey();
            m_currentAlarmRule->setAlarmType( dlg.getSelectedKey() );

            // Now update the alarm type the details view is showing
            m_alarmType.SetWindowText( dlg.getSelectedName() );
            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
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

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onButtonEntitytype() 
    {
        FUNCTION_ENTRY("onButtonEntitytype");
        
        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Retrieve the current entity type this rule is using
        CString currentEntityType("");
        m_entityType.GetWindowText( currentEntityType );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            CWaitCursor wait;
            ChangeDialog::ItemsForSelection items =
                m_databaseAccessor->getAllEntityTypes();

            // Now construct the dialog and then wait for the user
            // to finish with it
            ChangeDialog dlg(
                items, CHANGE_ENTITYTYPE_DIALOG_TITLE, currentEntityType, this);
            int response = dlg.DoModal();

            if (response != IDOK)
            {
                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo,
                            "User cancelled the selection of a new entity type");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "User selected a new entity type which is %s",
                         dlg.getSelectedName());

            TA_ASSERT(m_currentAlarmRule !=  NULL, "current rule is NULL.");

            // If this is a different entity type then set it in
            // our database object.
            m_currentAlarmRule->setEntityTypeKey( dlg.getSelectedKey() );

            // Now update the entity type the details view is showing
            m_entityType.SetWindowText( dlg.getSelectedName() );
            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
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

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onButtonEntity() 
    {
        FUNCTION_ENTRY("onButtonEntity");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }
        CWaitCursor wait;

        // Retrieve the current entity this rule is using
        CString currentEntity("");
        m_entity.GetWindowText( currentEntity );

        ChangeEntityDialog dlg( currentEntity, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new type");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the entity %s", dlg.getSelectedName());

        try
        {
            // If this is a different entity then set it in our database object.
            m_currentAlarmRule->setEntityKey( dlg.getSelectedKey() );

            // Now update the entity name that the details view is showing
            m_entity.SetWindowText( dlg.getSelectedName() );
            m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            // Now update the row in the list view in the top pane of the splitter
    	    m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
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


	// TD 11796 "Add" button is taken out as all entities should be some form of data nodes/pts
	/*void AlarmRulesDetailView::onButtonAddChild() 
    {
        FUNCTION_ENTRY("onButtonAddChild");
        
        try
        {
            m_avalancheSuppressionList.addChild();
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
        
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }*/

    void AlarmRulesDetailView::onButtonRemoveChild() 
    {
        FUNCTION_ENTRY("onButtonAddChild");

        try
        {
            m_avalancheSuppressionList.removeChild();
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

		//add code here to reset status on listview:
		m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onButtonAddDataNode() 
    {
        FUNCTION_ENTRY("onButtonAddDataNode");

        CWaitCursor wait;
        
        try
        {
            m_avalancheSuppressionList.addDataNode();
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


		//add code here to reset status on the listview:
		m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onButtonPlan()
    {
        FUNCTION_ENTRY("onButtonPlan");
        
        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        PlanExecutionRuleData* data =
            dynamic_cast<PlanExecutionRuleData*>(m_currentAlarmRule);

        TA_ASSERT(data != NULL,
                  "This method should only be called when a plan execution "
                  "rule is being displayed");

        try
        {
            // Retrieve the current plan this rule is using
            std::string currentPlanPath = data->getPlanPath();

            // Now construct the dialog and then wait for the user to finish
            // with it. We must set the resource handles up correctly so that
            // the plan tree control can load icons correctly.
            HINSTANCE oldInstance = AfxGetResourceHandle();
            AfxSetResourceHandle( GetModuleHandle(AlarmRulesComponent::DLL_NAME) );

            ChangePlanDialog dlg(currentPlanPath, this);
            int response = dlg.DoModal();

            AfxSetResourceHandle( oldInstance );

            if (response != IDOK)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                             "User cancelled the selection of a new plan");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "User selected a new plan which is %s",
                         dlg.getSelectedPlanPath());

            TA_ASSERT(data !=  NULL, "current rule is NULL.");

            // Set the plan key in our database object.
            data->setPlanPath( std::string(dlg.getSelectedPlanPath()) );

            // Now update the details view.
            m_plan.SetWindowText( dlg.getSelectedPlanPath() );
            m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_listView->itemDetailsChanged(data->getUniqueIdentifier(),"");
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch,
                "DataException",
                "Invalid plan path has been specified.");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm plan association details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onKillfocusEditPlan() 
    {
        FUNCTION_ENTRY("onKillfocusEditPlan");
	    
        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        PlanExecutionRuleData* planData =
            dynamic_cast<PlanExecutionRuleData*>(m_currentAlarmRule);

        TA_ASSERT(planData != NULL,
                 "This method should only be called when a Plan "
                 "Execution rule is being displayed");

        CString planString;
        m_plan.GetWindowText(planString);

        try
        {
            // If the plan string has not changed then there is no point doing an update
            if (planString == planData->getPlanPath().c_str())
            {
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                // Attempt to set the path as entered by the operator. The path will be
                // validated to ensure that it denotes an existing plan.
                planData->setPlanPath( std::string(planString) );

                // Misformatting of the plan path by users is allowed to a limited extent
                // (e.g. a missing '/' at the beginning is OK).
                // Now that the path is validated we can reset the plan edit box to display
                // the path in the correct format.
                m_plan.SetWindowText(planData->getPlanPath().c_str());
                m_plan.setTextType( TA_Base_Bus::TextColourEdit::VALID );
        
                m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve alarm plan association details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
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





    void AlarmRulesDetailView::onRadioEntity() 
    {
        FUNCTION_ENTRY("onRadioEntity");

        if(m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // We're using entity so blank the entity type key and fields
            m_currentAlarmRule->setEntityTypeKey(0);
        
            m_selectedRadio = SELECTED_RADIO_ENTITY;

            refreshEntityAndEntityType();

            m_entityType.SetWindowText("");
            m_entityType.setTextType(TA_Base_Bus::TextColourEdit::VALID);

            m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                                "An unknown exception was caught");
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onRadioEntitytype() 
    {
        FUNCTION_ENTRY("onRadioEntitytype");

        if(m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // We're using entity type so blank the entity key and fields
            m_currentAlarmRule->setEntityKey(0);
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                                "An unknown exception was caught");

            FUNCTION_EXIT;
            return;
        }


        m_selectedRadio = SELECTED_RADIO_ENTITY_TYPE;

        refreshEntityAndEntityType();

        m_entity.SetWindowText("");
        m_entity.setTextType(TA_Base_Bus::TextColourEdit::VALID);

        m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }
    

    void AlarmRulesDetailView::onKillfocusEditAlarm() 
    {
        FUNCTION_ENTRY("onKillfocusEditAlarm");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString alarmTypeString;
        m_alarmType.GetWindowText(alarmTypeString);

        try
        {
            // If the alarm type string has not changed then there is no point doing an update
            if (alarmTypeString == m_currentAlarmRule->getAlarmTypeDisplayName().c_str())
            {
                m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAlarmRule->setAlarmTypeByDisplayName( std::string(alarmTypeString) );

                m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the alarm type could not be found in the database");

            m_alarmType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onKillfocusEditTimeout()
    {
        FUNCTION_ENTRY("onKillfocusEditTimeout");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString timeoutString;
        m_timeout.GetWindowText(timeoutString);

        // Attempt to extract the timeout as a number
        unsigned long timeout = 0;
        std::stringstream stream;
        stream.str( std::string(timeoutString) );

        try
        {
            // If the number is invalid, give an error message
            if( !(stream >> timeout) || timeout == 0)
            {
                m_timeout.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

                refreshButtonAvailability();
                FUNCTION_EXIT;
                return;
            }
        
            AvalancheSuppressionRuleData* data =
                dynamic_cast<AvalancheSuppressionRuleData*>(m_currentAlarmRule);

            TA_ASSERT(data != NULL,
                      "This method should not be called when anything other than "
                      "a suppression rule is being configured.");

            // If the timeout field has not changed then there is no point doing
            // an update
            if (timeout == data->getTimeout())
            {
                refreshButtonAvailability();
                FUNCTION_EXIT;
                return;
            }

            data->setTimeout(timeout);
            m_timeout.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                                "An unknown exception was caught");

            m_timeout.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            refreshButtonAvailability();

            FUNCTION_EXIT;
            return;
        }

        m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AlarmRulesDetailView::onKillfocusEditTimeIn()
    {
        FUNCTION_ENTRY("onKillfocusEditTimeIn");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString timeInString;
        m_timeIn.GetWindowText(timeInString);

        // Attempt to extract the timeout as a number
        unsigned long timeIn = 0;
        std::stringstream stream;
        stream.str( std::string(timeInString) );

        try
        {
            // If the number is invalid, give an error message
            if( !(stream >> timeIn) || timeIn == 0)
            {
                m_timeIn.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

                refreshButtonAvailability();
                FUNCTION_EXIT;
                return;
            }
        
            AvalancheSuppressionRuleData* data =
                dynamic_cast<AvalancheSuppressionRuleData*>(m_currentAlarmRule);

            TA_ASSERT(data != NULL,
                      "This method should not be called when anything other than "
                      "a suppression rule is being configured.");

            // If the timeout field has not changed then there is no point doing
            // an update
            if (timeIn == data->getTimein())
            {
                refreshButtonAvailability();
                FUNCTION_EXIT;
                return;
            }

            data->setTimein(timeIn);
            m_timeIn.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                                "An unknown exception was caught");

            m_timeIn.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            refreshButtonAvailability();

            FUNCTION_EXIT;
            return;
        }

        m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AlarmRulesDetailView::onKillfocusEditMinAlarms()
    {
        FUNCTION_ENTRY("onKillfocusEditTimeIn");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString minAlarmsString;
        m_minAlarms.GetWindowText(minAlarmsString);

        // Attempt to extract the timeout as a number
        unsigned long minAlarms = 0;
        std::stringstream stream;
        stream.str( std::string(minAlarmsString) );

        try
        {
            // If the number is invalid, give an error message
            if( !(stream >> minAlarms))
            {
                m_minAlarms.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

                refreshButtonAvailability();
                FUNCTION_EXIT;
                return;
            }
        
            AvalancheSuppressionRuleData* data =
                dynamic_cast<AvalancheSuppressionRuleData*>(m_currentAlarmRule);

            TA_ASSERT(data != NULL,
                      "This method should not be called when anything other than "
                      "a suppression rule is being configured.");

            // If the timeout field has not changed then there is no point doing
            // an update
            if (minAlarms == data->getMinAlarms())
            {
                refreshButtonAvailability();
                FUNCTION_EXIT;
                return;
            }

            data->setMinAlarms(minAlarms);
            m_minAlarms.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                                "An unknown exception was caught");

            m_minAlarms.setTextType( TA_Base_Bus::TextColourEdit::INVALID );

            refreshButtonAvailability();

            FUNCTION_EXIT;
            return;
        }

        m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onKillfocusEditEntity() 
    {
        FUNCTION_ENTRY("onKillfocusEditEntity");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString entityString;
        m_entity.GetWindowText(entityString);

        try
        {
			std::string entityName;
			try
			{
				entityName = m_currentAlarmRule->getEntityName();
			}
			catch ( const TA_Base_Core::DataException& )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
					"Entity with key %d does not exist. Setting value to empty.", 
					m_currentAlarmRule->getEntityKey());
					entityName = "";
			}

            // If the entity type string has not changed then there is no point doing an update
            if (entityString == entityName.c_str())
            {
                m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAlarmRule->setEntity( std::string(entityString) );

                m_entity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the entity could not be found in the database");

            m_entity.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onKillfocusEditEntitytype() 
    {
        FUNCTION_ENTRY("onKillfocusEditEntitytype");
        
        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString entityTypeString;
        m_entityType.GetWindowText(entityTypeString);

        try
        {
            // If the entity type string has not changed then there is no point doing an update


			std::string entityTypeName;
			try
			{
				entityTypeName = m_currentAlarmRule->getEntityTypeName();
			}
			catch ( const TA_Base_Core::DataException& )
			{
				LOG_GENERIC( SourceInfo, DebugUtil::DebugError,
					"Entity with key %d does not exist. Setting value to empty.", 
					m_currentAlarmRule->getEntityTypeKey());
				entityTypeName = "";
			}

            if (entityTypeString == entityTypeName.c_str())
            {
                m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );
            }
            else
            {
                m_currentAlarmRule->setEntityType( std::string(entityTypeString) );

                m_entityType.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");
            }
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database","The name entered for the entity type could not be found in the database");

            m_entityType.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AlarmRulesDetailView::onSelectionChangeComboTrigger()
    {
        FUNCTION_ENTRY("onSelectionChangeComboTrigger");
        
        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CString triggerString;
        m_trigger.GetLBText(m_trigger.GetCurSel(), triggerString);

        // If the rule trigger string has not changed then
        // there is no point doing an update.
        if (triggerString == m_currentAlarmRule->getRuleTrigger().c_str())
        {
            return;
        }

        try
        {
            //use English version to set alarmRuleData
			std::string temp  = changeTriggerToEn(triggerString);
			m_currentAlarmRule->setRuleTrigger( temp );
        }
        catch( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Unknown",
                                "An unknown exception was caught");

            FUNCTION_EXIT;
            return;
        }


        m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(),"");

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    void AlarmRulesDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentAlarmRule);

        FUNCTION_EXIT;
    }


    void AlarmRulesDetailView::onButtonApply() 
    {
        FUNCTION_ENTRY("onButtonApply");

        if (m_currentAlarmRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        std::string ruleType = m_currentAlarmRule->getRuleType();

        // If an avalanche suppression rule is being configured, and
        // the timeout, timein or minAlarms fields are blank, give an error message
        
        if(ruleType == AvalancheSuppressionRuleData::RULE_TYPE)
        {
            CString timeoutText, timeinText, minAlarmsText;
            m_timeout.GetWindowText(timeoutText);
            m_timeIn.GetWindowText(timeinText);
            m_minAlarms.GetWindowText(minAlarmsText);

            UINT uiReason = 0;
            if (0 == timeoutText.GetLength())
                uiReason = IDS_TIMEOUT_NOT_SPECIFIED;
            else if (0 == timeinText.GetLength())
                uiReason = IDS_TIMEIN_NOT_SPECIFIED;
            else if (0 == minAlarmsText.GetLength())
                uiReason = IDS_MINALARMS_NOT_SPECIFIED;
                        
            // if any of the required fields were blank
            if (0 != uiReason)
            {
                CString actionName, reason;
                actionName.LoadString(IDS_APPLY);
                reason.LoadString(uiReason);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);

                return;
            }
        }

        // If a plan execution rule is being configured, and
        // the plan field is blank, give an error message
        CString planText;
        m_plan.GetWindowText(planText);

        if(ruleType == PlanExecutionRuleData::RULE_TYPE &&
           planText.GetLength() == 0)
        {
            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_PLAN_NOT_SPECIFIED);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }
        
        
        // If one of entity, entity type or alarm type is blank, give an error message.
        if (
             !m_entity.isTextValid()                           ||
             !m_entityType.isTextValid()                       ||
             !m_alarmType.isTextValid()                        ||
             (!m_plan.isTextValid() &&
                 ruleType == PlanExecutionRuleData::RULE_TYPE) ||
             (!m_timeout.isTextValid() &&
                 ruleType == AvalancheSuppressionRuleData::RULE_TYPE)
           )
        {
            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_INVALID_NAME);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }

        CString entityText;
        CString entityTypeText;
        CString alarmTypeText;

        m_entity.GetWindowText(entityText);
        m_entityType.GetWindowText(entityTypeText);
        m_alarmType.GetWindowText(alarmTypeText);

        // If one of entity, entity type or alarm type is blank, give an error message.
        if ( entityText.GetLength()     == 0    &&
             entityTypeText.GetLength() == 0    && 
             alarmTypeText.GetLength()  == 0       )
        {
            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_ENTITY_DETAIL_NOT_SPECIFIED);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }

        //check if the children is also a head
        if(ruleType == AvalancheSuppressionRuleData::RULE_TYPE)
        {
			TA_Base_Core::IAlarmRuleData* head=NULL;
	        std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair> Children=dynamic_cast<AvalancheSuppressionRuleData*>(m_currentAlarmRule)->getChildren();
			for(std::vector<AvalancheSuppressionRuleData::EntityKeyAlarmTypePair>::iterator itr=Children.begin();itr!=Children.end();itr++)
			{
				if(m_databaseAccessor->isChildAlsoAHead(*itr,&head))
				{
					//check if where is a circle
					if (m_databaseAccessor->isExistChild(head,m_currentAlarmRule))
					{
						CString actionName, reason;
						actionName.LoadString(IDS_APPLY);
						reason.LoadString(IDS_EXIST_CIRCLE);
						TA_Base_Bus::TransActiveMessage userMsg;
						userMsg << actionName << reason;
						UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
						return;
					}
				}
			}
    	}

        applyButtonPressed(m_currentAlarmRule);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_currentAlarmRule->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }

	CString AlarmRulesDetailView::changeTriggerToCh(std::string ruleTrigger){

		if(ruleTrigger==TRIGGER_SUBMIT_EN){
			return CString(TRIGGER_SUBMIT.c_str());
		}else if(ruleTrigger==TRIGGER_CLOSE_EN){
			return CString(TRIGGER_CLOSE.c_str());
		}else if(ruleTrigger==TRIGGER_ACKNOWLEDGE_EN){
			return CString(TRIGGER_ACKNOWLEDGE.c_str());
		}

		 return "";

	}

	std::string AlarmRulesDetailView::changeTriggerToEn(CString ruleTrigger){

		CT2CA  s1 (ruleTrigger);
		std::string temp(s1);
		if(temp==TRIGGER_SUBMIT){
			return TRIGGER_SUBMIT_EN;
		}else if(temp==TRIGGER_CLOSE){
			return TRIGGER_CLOSE_EN;
		}else if(temp==TRIGGER_ACKNOWLEDGE){
			return TRIGGER_ACKNOWLEDGE_EN;
		}

		return "";

	}




    /////////////////////////////////////////////////////////////////////////////
    // AlarmRulesDetailView diagnostics

    #ifdef _DEBUG
    void AlarmRulesDetailView::AssertValid() const
    {
        AbstractDetailView::AssertValid();
    }

    void AlarmRulesDetailView::Dump(CDumpContext& dc) const
    {
        AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG
}

