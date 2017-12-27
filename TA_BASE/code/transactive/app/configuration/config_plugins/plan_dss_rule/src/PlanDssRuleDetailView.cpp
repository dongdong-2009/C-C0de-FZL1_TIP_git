/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/20 12:26:46 $
  * Last modified by:  $Author: CM $
  * 
  * This view allows the user to configure an alarm plan association's details.
  */

#include "app/configuration/config_plugins/plan_dss_rule/src/StdAfx.h"

#include "ace/OS.h"
#include "ace/Time_Value.h"
#include "ace/Date_Time.h"

#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDetailView.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleDatabaseAccessor.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleListView.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleComponent.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/ChangePlanDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/plans/src/IConfigPlanDssDpRule.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DataConversion.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // The titles for the change dialog box
    const int ENTITY_FIELD_LENGTH = 80;
    const int LOCATION_FIELD_LENGTH = 80;
    const int PLAN_FIELD_LENGTH = 256;
    const int MINI_INTERVAL_FIELD_LENGTH = 30;
	const int ACTIVED_TOLERENT_TIME_LENGTH = 30;
	const int DENY_STRATEGY_LENGTH = 80;
	   
    IMPLEMENT_DYNCREATE(PlanDssRuleDetailView, CFormView)


    PlanDssRuleDetailView::PlanDssRuleDetailView() : 
    m_pCurPlanDssRule(NULL),
    m_listView(NULL),
    m_databaseAccessor(NULL),
    AbstractDetailView(PlanDssRuleDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(PlanDssRuleDetailView)
	//}}AFX_DATA_INIT
    }


    PlanDssRuleDetailView::~PlanDssRuleDetailView()
    {
    }


    void PlanDssRuleDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PlanDssRuleDetailView)
	    DDX_Control(pDX, IDC_ENTITY_BITMAP, m_bitmapEntity);
	    DDX_Control(pDX, IDC_EDIT_PLAN, m_edtActivePlan);
        DDX_Control(pDX, IDC_EDIT_LOCATION, m_edtLocation);
        DDX_Control(pDX, IDC_EDIT_DENY_STRATEGY, m_edtDenyStrategy);
	    DDX_Control(pDX, IDC_EDIT_ENTITY, m_edtTriggerEntity);
	    DDX_Control(pDX, IDC_EDIT_MINIMUM_INTERVAL, m_edtMinimumInterval);
		DDX_Control(pDX, IDC_EDIT_TOLERANT_TIME_FOR_ACTIVE, m_edtActivedTolerantTime);
		DDX_Control(pDX, IDC_RADIO_ACTIVE_TRUE,m_activateValueTrue );
		DDX_Control(pDX, IDC_RADIO_ACTIVE_FALSE,m_activateValueFalse );
		DDX_Control(pDX, IDC_CHECK_NEED_DEACTIVATE,m_needDeactivate );
		DDX_Control(pDX, IDC_CHECK_ALLOW_MULTIPLE,m_allowMultiple );
		DDX_Control(pDX, IDC_CHECK_ENABLE, m_checkEnable);
        DDX_Control(pDX, IDC_DTIME_START, m_validStartTimeInDay);
		DDX_Control(pDX, IDC_DTIME_END, m_validEndTimeInDay);
        DDX_Control(pDX, IDC_CHECK_SUN, m_validSunday );
		DDX_Control(pDX, IDC_CHECK_MON, m_validMonday );
		DDX_Control(pDX, IDC_CHECK_TUE,m_validTuesDay );
		DDX_Control(pDX, IDC_CHECK_WED,m_validWednesday );
		DDX_Control(pDX, IDC_CHECK_THU,m_validThursday );
		DDX_Control(pDX, IDC_CHECK_FRI,m_validFriday );
		DDX_Control(pDX, IDC_CHECK_SAT,m_validSaturday );
	    DDX_Control(pDX, IDC_PLAN_BITMAP, m_bitmapPlan);
	    DDX_Control(pDX, IDC_INTERVAL_BITMAP, m_bitmapInterval);
	    DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	    DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	    DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(PlanDssRuleDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(PlanDssRuleDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_ENTITY, onButtonTriggerEntity)
    ON_BN_CLICKED(IDC_BUTTON_PLAN, onButtonActivePlan)
    ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
    ON_BN_CLICKED(IDC_BUTTON_DENY_STRATEGY, onButtonDenyStrategy)
	ON_BN_CLICKED(IDC_RADIO_ACTIVE_TRUE, onButtonActivateTrue)
	ON_BN_CLICKED(IDC_RADIO_ACTIVE_FALSE, onButtonActivateFalse)
	ON_BN_CLICKED(IDC_CHECK_NEED_DEACTIVATE, onButtonNeedDeactivate)
	ON_BN_CLICKED(IDC_CHECK_ALLOW_MULTIPLE, onButtonAllowMultiple)
	ON_BN_CLICKED(IDC_CHECK_ENABLE, onButtonEnable)
	ON_BN_CLICKED(IDC_CHECK_SUN, onButtonSunday)
	ON_BN_CLICKED(IDC_CHECK_MON, onButtonMonday)
	ON_BN_CLICKED(IDC_CHECK_TUE, onButtonTuesday)
	ON_BN_CLICKED(IDC_CHECK_WED, onButtonWednesday)
	ON_BN_CLICKED(IDC_CHECK_THU, onButtonThursday)
	ON_BN_CLICKED(IDC_CHECK_FRI, onButtonFriday)
	ON_BN_CLICKED(IDC_CHECK_SAT, onButtonSaturday)
	//ON_NOTIFY_REFLECT(DTN_DATETIMECHANGE,IDC_DTIME_START, OnDatetimechangeStart)
	ON_NOTIFY(DTN_DATETIMECHANGE,IDC_DTIME_START, OnDatetimechangeStart)
	ON_NOTIFY(DTN_DATETIMECHANGE,IDC_DTIME_END, OnDatetimechangeEnd)
	ON_EN_KILLFOCUS(IDC_EDIT_MINIMUM_INTERVAL, onKillfocusEditMinimumInterval)
	ON_EN_KILLFOCUS(IDC_EDIT_TOLERANT_TIME_FOR_ACTIVE, onKillfocusEditActivedTolerantTime)

	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void PlanDssRuleDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        m_edtLocation.SetLimitText(LOCATION_FIELD_LENGTH);
        m_edtTriggerEntity.SetLimitText(ENTITY_FIELD_LENGTH);
        m_edtMinimumInterval.SetLimitText(MINI_INTERVAL_FIELD_LENGTH);
		m_edtActivedTolerantTime.SetLimitText(ACTIVED_TOLERENT_TIME_LENGTH);
        m_edtActivePlan.SetLimitText(PLAN_FIELD_LENGTH);
		m_edtDenyStrategy.SetLimitText(DENY_STRATEGY_LENGTH);

		//set CDataTimeCtrl mode
		m_validStartTimeInDay.SetFormat("HH:mm:ss");
		m_validEndTimeInDay.SetFormat("HH:mm:ss");

        // Hide all the alarm plan association details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void PlanDssRuleDetailView::showItemDetails(bool isShown)
    {
         // Show or hide all the controls from the view
         GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_LOCATION)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_MINIMUM_INTERVAL)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_PLAN)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_PLAN)->ShowWindow(isShown);
         GetDlgItem(IDC_EDIT_ENTITY)->ShowWindow(isShown);
         GetDlgItem(IDC_INTERVAL_BITMAP)->ShowWindow(isShown);
         GetDlgItem(IDC_PLAN_BITMAP)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_ACTIVATE_VALUE)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_DENY_STRATEGY)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_PLAN)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_ENTITY)->ShowWindow(isShown);
         GetDlgItem(IDC_BUTTON_LOCATION)->ShowWindow(isShown);
         GetDlgItem(IDC_RADIO_ACTIVE_TRUE)->ShowWindow(isShown);
         GetDlgItem(IDC_RADIO_ACTIVE_FALSE)->ShowWindow(isShown);
         GetDlgItem(IDC_STATIC_ENTITY_GROUP)->ShowWindow(isShown);
         GetDlgItem(IDC_ENTITY_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_TRIGGER_ENTITY)->ShowWindow(isShown);
		 GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_NEED_DEACTIVATE)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_ALLOW_MULTIPLE)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_ENABLE)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_DENY_STRATEGY)->ShowWindow(isShown);
		 GetDlgItem(IDC_EDIT_DENY_STRATEGY)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_INTERVAL)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_TOLERANT_TIME)->ShowWindow(isShown);
		 GetDlgItem(IDC_TOLERANT_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_EDIT_TOLERANT_TIME_FOR_ACTIVE)->ShowWindow(isShown);
		 GetDlgItem(IDC_VALIDTIME_BITMAP)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_START_TIME)->ShowWindow(isShown);
		 GetDlgItem(IDC_DTIME_START)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_END_TIME)->ShowWindow(isShown);
		 GetDlgItem(IDC_DTIME_END)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_SUN)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_MON)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_TUE)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_WED)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_THU)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_FRI)->ShowWindow(isShown);
		 GetDlgItem(IDC_CHECK_SAT)->ShowWindow(isShown);
		 GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
		 
    }


    void PlanDssRuleDetailView::refreshButtonAvailability()
    {
        if (m_pCurPlanDssRule == NULL)
        {
            return;
        }

		if(BST_CHECKED == m_allowMultiple.GetCheck())
		{
			m_edtDenyStrategy.EnableWindow(FALSE);
			GetDlgItem(IDC_BUTTON_DENY_STRATEGY)->EnableWindow(FALSE);

		}
		else
		{
			m_edtDenyStrategy.EnableWindow(TRUE);
			GetDlgItem(IDC_BUTTON_DENY_STRATEGY)->EnableWindow(TRUE);
		}

        if (m_pCurPlanDssRule->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if( !m_edtLocation.isTextValid() || !m_edtTriggerEntity.isTextValid() || !m_edtActivePlan.isTextValid() || !m_edtMinimumInterval.isTextValid() || !m_edtActivedTolerantTime.isTextValid() ||
			!m_edtDenyStrategy.isTextValid() )
        {
            // If any fields are invalid then enable the reset button but disable the apply button
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
        else if (m_pCurPlanDssRule->hasChanged())
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

    
    void PlanDssRuleDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<PlanDssRuleDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<PlanDssRuleListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper PlanDssRuleDetailView");
        }


        m_pCurPlanDssRule = dynamic_cast<TA_Base_Core::IConfigPlanDssDpRule*>(currentItem);

        if (m_pCurPlanDssRule == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            PlanDssRuleDatabaseAccessor::PlanDssDpRuleText item;
            m_databaseAccessor->convertItemToDisplay( *m_pCurPlanDssRule, item );
            //  Now populate all the fields with the data retrieved from the IConfigPlanDssDpRule* object
            m_edtMinimumInterval.SetWindowText( item.strMinimumInterval.c_str() );
            m_edtMinimumInterval.setTextType( TA_Base_Bus::TextColourEdit::VALID );

			m_edtActivedTolerantTime.SetWindowText( item.strActivedTolerantTime.c_str());
			m_edtActivedTolerantTime.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_edtActivePlan.SetWindowText( item.strActivePlan.c_str() );
            m_edtActivePlan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_edtLocation.SetWindowText( item.strLocation.c_str() );
            m_edtLocation.setTextType( TA_Base_Bus::TextColourEdit::VALID );

            m_edtTriggerEntity.SetWindowText( item.strTriggerEntity.c_str() );
            m_edtTriggerEntity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

			m_edtDenyStrategy.SetWindowText( item.strDenyStrategy.c_str());
			m_edtDenyStrategy.setTextType( TA_Base_Bus::TextColourEdit::VALID );
			
			if(m_pCurPlanDssRule->getActivateValue())
			{
				m_activateValueFalse.SetCheck(BST_UNCHECKED);
				m_activateValueTrue.SetCheck(BST_CHECKED);
			}
			else
			{
				m_activateValueFalse.SetCheck(BST_CHECKED);
				m_activateValueTrue.SetCheck(BST_UNCHECKED);				
			}
            
			m_needDeactivate.SetCheck(m_pCurPlanDssRule->getNeedDeactivate());

			m_allowMultiple.SetCheck(m_pCurPlanDssRule->getAllowMultiple());

			m_checkEnable.SetCheck(m_pCurPlanDssRule->getEnable());

			m_validSunday.SetCheck(m_pCurPlanDssRule->getValidSunday());
			m_validMonday.SetCheck(m_pCurPlanDssRule->getValidMonday());
			m_validTuesDay.SetCheck(m_pCurPlanDssRule->getValidTuesday());
			m_validWednesday.SetCheck(m_pCurPlanDssRule->getValidWednesday());
			m_validThursday.SetCheck(m_pCurPlanDssRule->getValidThursday());
			m_validFriday.SetCheck(m_pCurPlanDssRule->getValidFriday());
			m_validSaturday.SetCheck(m_pCurPlanDssRule->getValidSaturday());

			time_t currentTime = ACE_OS::gettimeofday().sec();
			time_t zeroTime = currentTime-((currentTime + SECONDS_INTERVAL) % SECONDS_IN_DAY);

			time_t startTime = m_pCurPlanDssRule->getValidStartTimeInDay();
			startTime = startTime + zeroTime;
			CTime cTimeStart(startTime);
			m_validStartTimeInDay.SetTime(&cTimeStart);
			
			time_t endTime = m_pCurPlanDssRule->getValidEndTimeInDay();
			endTime = endTime + zeroTime;
			CTime cTimeEnd(endTime);
			m_validEndTimeInDay.SetTime(&cTimeEnd);

            if( m_pCurPlanDssRule->isNew() )
            {

                // This is a new alarm plan association or at least a copy of an alarm plan association so we indicate this in the type
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
                time_t modify = m_pCurPlanDssRule->getDateModified();
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
                time_t created = m_pCurPlanDssRule->getDateCreated();
                COleDateTime dateCreated ( created );
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


    void PlanDssRuleDetailView::onButtonTriggerEntity() 
    {
        FUNCTION_ENTRY("onButtonTriggerEntity");

        if (m_pCurPlanDssRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CWaitCursor wait;

        // Retrieve the current entity this association is using
        CString currentEntity("");
        m_edtTriggerEntity.GetWindowText( currentEntity );

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
            const unsigned long ulNewEntity = dlg.getSelectedKey();

            if ( m_pCurPlanDssRule->getTriggerEntityKey() != ulNewEntity )
            {
                // If this is a different entity then set it in our database object.
                m_pCurPlanDssRule->setTriggerEntityKey( ulNewEntity );

                // Now update the entity name that the details view is showing
                m_edtTriggerEntity.SetWindowText( dlg.getSelectedName() );
                m_edtTriggerEntity.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Now update the row in the list view in the top pane of the splitter
                m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
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
 
        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void PlanDssRuleDetailView::onButtonActivePlan() 
    {
        FUNCTION_ENTRY("onButtonPlan");

        CWaitCursor wait;
        unsigned long ulPlanKey = 0u;

        try
        {
            // Retrieve the current plan this association is using
            ulPlanKey = m_pCurPlanDssRule->getActivePlanKey();
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
        AfxSetResourceHandle( GetModuleHandle(PlanDssRuleComponent::DLL_NAME) );

        ChangePlanDialog dlg(ulPlanKey, this);
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
            if (m_pCurPlanDssRule == NULL)
            {
                return;
            }

            const unsigned long ulNewPlanKey = dlg.getSelectedPlanId();

            if ( m_pCurPlanDssRule->getActivePlanKey() != ulNewPlanKey )
            {
                // If this is a different Plan then set it in our database object.
                m_pCurPlanDssRule->setActivePlanKey( ulNewPlanKey );

                // Now update the Plan the details view is showing
                m_edtActivePlan.SetWindowText( dlg.getSelectedPlanPath().c_str() );
                m_edtActivePlan.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
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

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }


    void PlanDssRuleDetailView::onKillfocusEditMinimumInterval() 
    {
        FUNCTION_ENTRY("onKillfocusEditMinimumInterval");

        if (m_pCurPlanDssRule == NULL)
        {
            return;
        }

        CString strMinimumInterval;
        m_edtMinimumInterval.GetWindowText(strMinimumInterval);

        // If the alarm type string has not changed then there is no point doing an update
        if (0 == strMinimumInterval.GetLength())
        {
            m_edtMinimumInterval.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
        }
        else
        {
            unsigned long ulInterval = TA_Base_Core::DataConversion::toUnsignedInteger( strMinimumInterval.GetBuffer() );
            strMinimumInterval.ReleaseBuffer();

            m_edtMinimumInterval.setTextType( ( ulInterval >= 0 ) ? TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID );

            if ( m_pCurPlanDssRule->getMinimumInterval() != ulInterval )
            {
                m_pCurPlanDssRule->setMinimumInterval( ulInterval );
                m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
            }
        }

        refreshButtonAvailability();
        
        FUNCTION_EXIT;
    }
    //lixiaoxia++
	void PlanDssRuleDetailView::onKillfocusEditActivedTolerantTime() 
	{
		FUNCTION_ENTRY("onKillfocusEditActivedTolerantTime");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		CString strActivedTolerantTime;
		m_edtActivedTolerantTime.GetWindowText(strActivedTolerantTime);

		// If the alarm type string has not changed then there is no point doing an update
		if (0 == strActivedTolerantTime.GetLength())
		{
			m_edtActivedTolerantTime.setTextType( TA_Base_Bus::TextColourEdit::INVALID );
		}
		else
		{
			unsigned long ulInterval = TA_Base_Core::DataConversion::toUnsignedInteger( strActivedTolerantTime.GetBuffer() );
			strActivedTolerantTime.ReleaseBuffer();

			m_edtActivedTolerantTime.setTextType( ( ulInterval >= 0 ) ? TA_Base_Bus::TextColourEdit::VALID : TA_Base_Bus::TextColourEdit::INVALID );

			if ( m_pCurPlanDssRule->getTolerantTimeForActive() != ulInterval )
			{
				m_pCurPlanDssRule->setTolerantTimeForActive( ulInterval );
				m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
			}
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonActivateTrue() 
	{
		FUNCTION_ENTRY("onButtonActivateTrue");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getActivateValue()) ^ (BST_CHECKED == m_activateValueTrue.GetCheck()) )
		{
			m_pCurPlanDssRule->setActivateValue(!(m_pCurPlanDssRule->getActivateValue()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonActivateFalse() 
	{
		FUNCTION_ENTRY("onButtonActivateFalse");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getActivateValue()) ^ (BST_UNCHECKED == m_activateValueFalse.GetCheck()) )
		{
			m_pCurPlanDssRule->setActivateValue(!(m_pCurPlanDssRule->getActivateValue()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonNeedDeactivate() 
	{
		FUNCTION_ENTRY("onButtonNeedDeactivate");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getNeedDeactivate()) ^ (BST_CHECKED == m_needDeactivate.GetCheck()) )
		{
			m_pCurPlanDssRule->setNeedDeactivate(!(m_pCurPlanDssRule->getNeedDeactivate()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonAllowMultiple() 
	{
		FUNCTION_ENTRY("onButtonAllowMultiple");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getAllowMultiple()) ^ (BST_CHECKED == m_allowMultiple.GetCheck()) )
		{
			m_pCurPlanDssRule->setAllowMultiple(!(m_pCurPlanDssRule->getAllowMultiple()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonEnable() 
	{
		FUNCTION_ENTRY("onButtonEnable");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getEnable()) ^ (BST_CHECKED == m_checkEnable.GetCheck()) )
		{
			m_pCurPlanDssRule->setEnable(!(m_pCurPlanDssRule->getEnable()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonMonday() 
	{
		FUNCTION_ENTRY("onButtonMonday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidMonday()) ^ (BST_CHECKED == m_validMonday.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidMonday(!(m_pCurPlanDssRule->getValidMonday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonSunday() 
	{
		FUNCTION_ENTRY("onButtonSunday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidSunday()) ^ (BST_CHECKED == m_validSunday.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidSunday(!(m_pCurPlanDssRule->getValidSunday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonTuesday() 
	{
		FUNCTION_ENTRY("onButtonTuesday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidTuesday()) ^ (BST_CHECKED == m_validTuesDay.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidTuesday(!(m_pCurPlanDssRule->getValidTuesday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonWednesday() 
	{
		FUNCTION_ENTRY("onButtonWednesday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidWednesday()) ^ (BST_CHECKED == m_validWednesday.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidWednesday(!(m_pCurPlanDssRule->getValidWednesday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonThursday() 
	{
		FUNCTION_ENTRY("onButtonThursday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidThursday()) ^ (BST_CHECKED == m_validThursday.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidThursday(!(m_pCurPlanDssRule->getValidThursday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonFriday() 
	{
		FUNCTION_ENTRY("onButtonFriday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidFriday()) ^ (BST_CHECKED == m_validFriday.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidFriday(!(m_pCurPlanDssRule->getValidFriday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::onButtonSaturday() 
	{
		FUNCTION_ENTRY("onButtonSaturday");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		if ((m_pCurPlanDssRule->getValidSaturday()) ^ (BST_CHECKED == m_validSaturday.GetCheck()) )
		{
			m_pCurPlanDssRule->setValidSaturday(!(m_pCurPlanDssRule->getValidSaturday()));
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::OnDatetimechangeStart(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnDatetimechangeStart");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		SYSTEMTIME time;
		m_validStartTimeInDay.GetTime(&time);
		unsigned long timeSecond = time.wHour * SECONDS_IN_HOUR + time.wMinute * SECONDS_IN_MINUTE + time.wSecond;
		if ( timeSecond != m_pCurPlanDssRule->getValidStartTimeInDay())
		{
			m_pCurPlanDssRule->setValidStartTimeInDay(timeSecond);
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		*pResult = 0;

		FUNCTION_EXIT;
	}

	void PlanDssRuleDetailView::OnDatetimechangeEnd(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		FUNCTION_ENTRY("OnDatetimechangeEnd");

		if (m_pCurPlanDssRule == NULL)
		{
			return;
		}

		SYSTEMTIME time;
		m_validEndTimeInDay.GetTime(&time);
		unsigned long timeSecond = time.wHour * 3600 + time.wMinute * 60 + time.wSecond;
		if ( timeSecond != m_pCurPlanDssRule->getValidEndTimeInDay())
		{
			m_pCurPlanDssRule->setValidEndTimeInDay(timeSecond);
			m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
		}

		refreshButtonAvailability();

		*pResult = 0;

		FUNCTION_EXIT;
	}
	//++lixiaoxia

    void PlanDssRuleDetailView::onButtonLocation()
    {
        FUNCTION_ENTRY("onButtonLocation");

        if (m_pCurPlanDssRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CWaitCursor wait;

        // Retrieve the current entity this association is using
        unsigned long ulCurLoc = m_pCurPlanDssRule->getLocationKey();

        ChangeDialog::ItemsForSelection mapLocs = m_databaseAccessor->getAllLocations();
        std::string strCurItem = "";
        std::map<unsigned long, std::string>::iterator itCurrent = mapLocs.find( ulCurLoc );
        if ( itCurrent != mapLocs.end() )
        {
            strCurItem = itCurrent->second;
        }

        ChangeDialog dlg( mapLocs, "车站", strCurItem.c_str(), this );
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
            const unsigned long ulNewLoc = dlg.getSelectedKey();

            if ( ulCurLoc != ulNewLoc )
            {
                // If this is a different entity then set it in our database object.
                m_pCurPlanDssRule->setLocationKey( ulNewLoc );

                // Now update the entity name that the details view is showing
                m_edtLocation.SetWindowText( dlg.getSelectedName() );
                m_edtLocation.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Now update the row in the list view in the top pane of the splitter
                m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
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

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void PlanDssRuleDetailView::onButtonDenyStrategy()
    {
        FUNCTION_ENTRY("onButtonLocation");

        if (m_pCurPlanDssRule == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CWaitCursor wait;

        // Retrieve the current entity this association is using
        int ulCurStrategy = m_pCurPlanDssRule->getDenyMultipleStrategy();

        ChangeDialog::ItemsForSelection mapStrategies = m_databaseAccessor->getAllStrategies();
        std::string strCurItem = "";
        std::map<unsigned long, std::string>::iterator itCurrent = mapStrategies.find( ulCurStrategy );
        if ( itCurrent != mapStrategies.end() )
        {
            strCurItem = itCurrent->second;
        }

        ChangeDialog dlg( mapStrategies, "拒绝多实例方式", strCurItem.c_str(), this );
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
            const unsigned long ulNewStrategy = dlg.getSelectedKey();

            if ( ulCurStrategy != ulNewStrategy )
            {
                // If this is a different entity then set it in our database object.
                m_pCurPlanDssRule->setDenyMultipleStrategy( ulNewStrategy );

                // Now update the entity name that the details view is showing
                m_edtDenyStrategy.SetWindowText( dlg.getSelectedName() );
                m_edtDenyStrategy.setTextType( TA_Base_Bus::TextColourEdit::VALID );

                // Now update the row in the list view in the top pane of the splitter
                m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(),"");
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

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    
    void PlanDssRuleDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_pCurPlanDssRule);

        FUNCTION_EXIT;
    }


    void PlanDssRuleDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        if ( !m_edtTriggerEntity.isTextValid()    || !m_edtLocation.isTextValid() || 
             !m_edtMinimumInterval.isTextValid() || !m_edtActivePlan.isTextValid() ||
			 !m_edtActivedTolerantTime.isTextValid() || !m_edtDenyStrategy.isTextValid())
        {
            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_FIELD_INVALID);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName.GetBuffer(actionName.GetLength()) << reason.GetBuffer(reason.GetLength());
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }
        applyButtonPressed(m_pCurPlanDssRule);

        // Force this call so our status column updates
        m_listView->itemDetailsChanged(m_pCurPlanDssRule->getUniqueIdentifier(), "");

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // PlanDssRuleDetailView diagnostics

}

