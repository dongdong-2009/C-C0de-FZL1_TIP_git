/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/StateConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for derived data point state configuration
  */

#pragma warning(disable : 4786 4284)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/StateConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredDerivedState.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    const int StateConfiguration::LABEL_FIELD_LENGTH            = 250;
    const int StateConfiguration::MESSAGE_FIELD_LENGTH          = 250;
    const int StateConfiguration::DELAY_FIELD_LENGTH            = 8;
    const int StateConfiguration::NUMBER_OF_STATES_FIELD_LENGTH = 3;

    // The title of the dialog that allows the alarm severity to be changed
    const CString StateConfiguration::CHANGE_ALARMSEVERITY_DIALOG_TITLE = "Change Alarm Severity";

    const std::string StateConfiguration::NO_SEVERITY = "<None>";


    IMPLEMENT_DYNCREATE(StateConfiguration, CPropertyPage)

    StateConfiguration::StateConfiguration()
      : m_currentEntity(NULL),
        m_databaseAccessor(NULL),
        m_currentState(NULL),
        m_parent(NULL),
        CPropertyPage(StateConfiguration::IDD,IDS_PROPPAGE_STATE)
    {
	    //{{AFX_DATA_INIT(StateConfiguration)
	m_isAlarmEnabled = FALSE;
	m_delay = _T("");
	m_label = _T("");
	m_message = _T("");
	m_severity = _T("");
	m_numberOfStates = _T("");
	//}}AFX_DATA_INIT
    }


    StateConfiguration::~StateConfiguration()
    {
    }


    void StateConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(StateConfiguration)
	DDX_Control(pDX, IDC_EDIT_STATES, m_numberOfStatesControl);
	DDX_Control(pDX, IDC_EDIT_DELAY, m_delayCtrl);
	DDX_Control(pDX, IDC_EDIT_MESSAGE, m_messageCtrl);
	DDX_Control(pDX, IDC_EDIT_LABEL, m_labelCtrl);
	DDX_Control(pDX, IDC_LABEL_BITMAP, m_labelBitmap);
	DDX_Control(pDX, IDC_STATE_LIST, m_stateList);
	DDX_Control(pDX, IDC_COMBO_MMS, m_mmsControl);
	DDX_Check(pDX, IDC_CHECK_ALARM_ENABLED, m_isAlarmEnabled);
	DDX_Text(pDX, IDC_EDIT_DELAY, m_delay);
	DDX_Text(pDX, IDC_EDIT_LABEL, m_label);
	DDX_Text(pDX, IDC_EDIT_MESSAGE, m_message);
	DDX_Text(pDX, IDC_EDIT_SEVERITY, m_severity);
	DDX_Text(pDX, IDC_EDIT_STATES, m_numberOfStates);
    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(StateConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(StateConfiguration)
	ON_BN_CLICKED(IDC_CHANGE_SEVERITY, onChangeSeverity)
	ON_BN_CLICKED(IDC_CHECK_ALARM_ENABLED, onCheckAlarmEnabled)
	ON_EN_KILLFOCUS(IDC_EDIT_DELAY, onKillfocusEditDelay)
	ON_EN_KILLFOCUS(IDC_EDIT_LABEL, onKillfocusEditLabel)
	ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE, onKillfocusEditMessage)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_STATE_LIST, onItemchangedStateList)
	ON_WM_SHOWWINDOW()
	ON_EN_KILLFOCUS(IDC_EDIT_STATES, onKillfocusEditStates)
    ON_CBN_SELCHANGE(IDC_COMBO_MMS, onSelChangedMms)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void StateConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                          ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;
        m_databaseAccessor = &databaseAccessor;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void StateConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");
        FUNCTION_EXIT;
    }


    void StateConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");

        TA_Base_Core::IConfigDerivedDataPoint& dp = m_databaseAccessor->getDerivedDataPointItem( m_currentEntity->getUniqueIdentifier() );
        
        typedef TA_Base_Core::IConfigDerivedDataPoint::StateMap StateMap;
        const StateMap& states = dp.getDerivedStates();

        // Step through each derived state and see that the Description (or Label) value has been set
        for (StateMap::const_iterator state = states.begin(); state != states.end(); ++state)
        {
            if (state->second->getStateDescription().empty())
            {
                CString dialogTitle;
                dialogTitle.LoadString(IDS_PROPPAGE_STATE);

                CString labelTitle;
                labelTitle.LoadString(IDS_LABEL_LABEL);

                std::ostringstream parameter;
                parameter << std::string(dialogTitle) << " - " << std::string(labelTitle);
                parameter << " (State " << state->second->getStateValue() << ")";

                params.push_back(parameter.str());
            }
        }

 
        FUNCTION_EXIT;
    }


    void StateConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // StateConfiguration message handlers

    BOOL StateConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();
	    
        CRect rect;
        m_stateList.SetExtendedStyle(m_stateList.GetExtendedStyle() | LVS_EX_FULLROWSELECT);
	    m_stateList.GetWindowRect( rect );
        m_stateList.InsertColumn(0,"State",LVCFMT_CENTER,rect.Width());
	    
        m_delayCtrl.SetLimitText(DELAY_FIELD_LENGTH);
        m_messageCtrl.SetLimitText(MESSAGE_FIELD_LENGTH);
        m_labelCtrl.SetLimitText(LABEL_FIELD_LENGTH);
        m_numberOfStatesControl.SetLimitText(NUMBER_OF_STATES_FIELD_LENGTH);

        // Populate available alarm MMS settings
        ScadaDetailView::setupMmsCombo(m_mmsControl);

        loadEntityIntoPage();

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void StateConfiguration::OnShowWindow(BOOL bShow, UINT nStatus) 
    {
	    CPropertyPage::OnShowWindow(bShow, nStatus);

        if (bShow ==FALSE)
        {
            //Hiding so don't care
            return;
        }

        if (m_hWnd == NULL)
        {
            return;
        }

        if (m_currentEntity == NULL)
        {
            return;
        }

        // States can change so we must update this page whenever it is shown
        m_stateList.DeleteAllItems();

        loadEntityIntoPage();
	    
    }


    void StateConfiguration::onKillfocusEditStates() 
    {
        UpdateData(TRUE);

        // Get the currently selected state and if one is selected then unselect it to blank
        // the details section
        int selected = -1;
        POSITION pos = m_stateList.GetFirstSelectedItemPosition();
        if (pos != NULL)
        {
            int index = m_stateList.GetNextSelectedItem(pos);
            selected = m_stateList.GetItemData(index);
            m_stateList.SetItemState(index,0,LVIS_SELECTED);
        }

        try
        {
            // Retrieve the datapoint
            TA_Base_Core::IConfigDerivedDataPoint& dp = m_databaseAccessor->getDerivedDataPointItem( m_currentEntity->getUniqueIdentifier() );
        
            // Reset the number of states it contains
            int numberOfStates = 0;

            if (!m_numberOfStates.IsEmpty())
            {
                std::istringstream stateString;
                stateString.str( std::string(m_numberOfStates) );
                stateString >> numberOfStates;
            }
            else
            {
                m_numberOfStates = "0";
            }

            dp.setNumberOfStates( numberOfStates );

            typedef TA_Base_Core::IConfigDerivedDataPoint::StateMap StateMap;
            const StateMap& states = dp.getDerivedStates();

            // Repopulate the list of states
            m_stateList.DeleteAllItems();
            int insertPos = 0;
            for (StateMap::const_iterator state = states.begin(); state != states.end(); ++state)
            {
                CString stateText;
                stateText.Format("%d",state->second->getStateValue());

                int position = m_stateList.InsertItem(insertPos++,stateText,-1);
                m_stateList.SetItemData(position, reinterpret_cast<unsigned long>(state->second) );
            }

            // Select either the first item or the item that was selected last time
            // Search all lParams stored with each list item for the selected state we have.
            LVFINDINFO info;
            info.flags = LVFI_PARAM;
            info.lParam = selected;

            int foundItemIndex = m_stateList.FindItem(&info);
            if (foundItemIndex == -1)
            {
                foundItemIndex = 0;
            }

            m_stateList.SetItemState(foundItemIndex, LVIS_SELECTED, LVIS_SELECTED);
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve derived data point state item details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve derived data point state item details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        UpdateData(FALSE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void StateConfiguration::onChangeSeverity() 
    {
        FUNCTION_ENTRY("onChangeSeverity");

        UpdateData(TRUE);
        
        if (m_currentState != NULL)
        {
	        changeSeverity(m_severity, m_currentState->getStateValue());
        }
            
        FUNCTION_EXIT;
    }


    void StateConfiguration::onCheckAlarmEnabled() 
    {
        FUNCTION_ENTRY("onCheckAlarmEnabled");

        UpdateData(TRUE);
	    
        if (m_currentState != NULL)
        {
            m_currentState->setIsAlarmEnabled( m_isAlarmEnabled==TRUE );
        }
        
        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        FUNCTION_EXIT;
    }

    
    void StateConfiguration::onKillfocusEditDelay() 
    {
        FUNCTION_ENTRY("onKillfocusEditDelay");

        UpdateData(TRUE);
	    
        if (m_currentState != NULL)
        {
            int alarmDelay = 0;
            if (!m_delay.IsEmpty())
            {
                // Convert the string into an integer
                std::istringstream delayString;
                delayString.str( std::string(m_delay) );
                delayString >> alarmDelay;
            }

            m_currentState->setAlarmDelay( alarmDelay );
        }

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void StateConfiguration::onKillfocusEditLabel() 
    {
        FUNCTION_ENTRY("onKillfocusEditLabel");

        UpdateData(TRUE);
	    
        if (m_currentState != NULL)
        {            
            m_currentState->setStateDescription( std::string(m_label) );
        }

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void StateConfiguration::onKillfocusEditMessage() 
    {
        FUNCTION_ENTRY("onKillfocusEditMessage");

        UpdateData(TRUE);
	    
        if (m_currentState != NULL)
        {
            m_currentState->setAlarmMessage( std::string(m_message) );
        }
	    
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void StateConfiguration::onItemchangedStateList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("onItemchangedStateList");

        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	    
        POSITION pos = m_stateList.GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            // Nothing selected so reset
            m_currentState = NULL;
            m_delay = "";
            m_label = "";
            m_message = "";
            m_severity = "";
            m_isAlarmEnabled = false;
            ScadaDetailView::setMmsStateComboSelection(m_mmsControl, "");

            UpdateData(FALSE);
        }
        else
        {
            int selected = m_stateList.GetNextSelectedItem(pos);

            using TA_Base_Core::ConfigConfiguredDerivedState;

            m_currentState = reinterpret_cast<ConfigConfiguredDerivedState*>(m_stateList.GetItemData(selected));

            m_delay.Format("%d",m_currentState->getAlarmDelay());
            m_label = m_currentState->getStateDescription().c_str();
            m_message = m_currentState->getAlarmMessage().c_str();
            m_severity = m_currentState->getAlarmSeverityName().c_str();
            m_isAlarmEnabled = m_currentState->getAlarmEnabled();

            std::string mmsState = m_currentState->getAlarmMMS();
            ScadaDetailView::setMmsStateComboSelection(m_mmsControl, mmsState);
        }

        UpdateData(FALSE);

        refresh();
	    
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        *pResult = 0;

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void StateConfiguration::loadEntityIntoPage()
    {
        FUNCTION_ENTRY("loadEntityIntoPage");

        if (m_hWnd == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Could not load the entity details yet as OnInitDialog has not been called");
            FUNCTION_EXIT;
            return;
        }

        if (m_currentEntity == NULL)
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Could not load the entity details yet as no current entity has been set yet");
            FUNCTION_EXIT;
            return;
        }


        try
        {
            TA_Base_Core::IConfigDerivedDataPoint& dp = m_databaseAccessor->getDerivedDataPointItem( m_currentEntity->getUniqueIdentifier() );
        
            //using TA_Base_Core::IConfigDerivedDataPoint::StateMap;
			typedef TA_Base_Core::IConfigDerivedDataPoint::StateMap StateMap;

            const StateMap& states = dp.getDerivedStates();
            int insertPosition =0;

            for (StateMap::const_iterator state = states.begin(); state != states.end(); ++state)
            {
                CString stateText;
                stateText.Format("%d",state->second->getStateValue());

                int position = m_stateList.InsertItem(insertPosition++,stateText,-1);
                m_stateList.SetItemData(position, reinterpret_cast<unsigned long>(state->second) );
            }

            // Select the first item so that something is selected
            m_stateList.SetItemState(0, LVIS_SELECTED, LVIS_SELECTED);

            // Set the number of states and determine if this field is enabled or disabled
            CString numberOfStates;
            numberOfStates.Format("%d",states.size());
            m_numberOfStates = numberOfStates;

            if(  m_currentEntity->getParameterValue(ScadaDetailView::DATAPOINT_TYPE) == ScadaDetailView::DERIVED_TYPE )
            {
                m_numberOfStatesControl.EnableWindow(FALSE);
            }
            else
            {
                // Output / enum types have user defined # of states
                m_numberOfStatesControl.EnableWindow(TRUE);
            }
            
            UpdateData(FALSE);

            refresh();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve derived data point state item details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve derived data point state item details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void StateConfiguration::changeSeverity(CString& currentSeverity, int state)
    {
        FUNCTION_ENTRY("changeSeverity");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        ScadaDatabaseAccessor db;
        ChangeDialog::ItemsForSelection items = db.getAllSeverities();
        items.insert(ChangeDialog::ItemsForSelection::value_type(0,NO_SEVERITY));

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_ALARMSEVERITY_DIALOG_TITLE, currentSeverity, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new severity");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new severity which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

            CString displayName("");
            // If this is a different severity then set it in our object.
            if (dlg.getSelectedName() != NO_SEVERITY.c_str())
            {
                displayName = dlg.getSelectedName();
            }
  
            if (m_currentState != NULL)
            {
                m_currentState->setAlarmSeverity(dlg.getSelectedKey());
            }

            // Now update the severity the details view is showing
            currentSeverity = displayName;
            UpdateData(FALSE);

        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve severity key so cannot update existing");
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve severity key so cannot update existing");
        }
        
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }

        
    void StateConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        UpdateData(TRUE);

        if (m_stateList.GetFirstSelectedItemPosition() != NULL)
        {
            GetDlgItem(IDC_EDIT_LABEL)->EnableWindow(true);
            GetDlgItem(IDC_CHECK_ALARM_ENABLED)->EnableWindow(true);
            
            // If checked then enable
            GetDlgItem(IDC_EDIT_SEVERITY)->EnableWindow(m_isAlarmEnabled);
            GetDlgItem(IDC_CHANGE_SEVERITY)->EnableWindow(m_isAlarmEnabled);
            GetDlgItem(IDC_EDIT_DELAY)->EnableWindow(m_isAlarmEnabled);
            GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(m_isAlarmEnabled);
            GetDlgItem(IDC_COMBO_MMS)->EnableWindow(m_isAlarmEnabled);
        }
        else
        {
            // Disable as no item is currently selected
            GetDlgItem(IDC_CHECK_ALARM_ENABLED)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_SEVERITY)->EnableWindow(false);
            GetDlgItem(IDC_CHANGE_SEVERITY)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_DELAY)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_MESSAGE)->EnableWindow(false);
            GetDlgItem(IDC_EDIT_LABEL)->EnableWindow(false);
            GetDlgItem(IDC_COMBO_MMS)->EnableWindow(false);
        }

        UpdateData(true);

        FUNCTION_EXIT;
    }

    
    void StateConfiguration::onSelChangedMms()
    {
        FUNCTION_ENTRY("onSelChangedMms");

        UpdateData(TRUE);
	    
        if (m_currentState != NULL)
        {
            std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_mmsControl);
            m_currentState->setAlarmMMS( mmsState );
        }
        
        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        FUNCTION_EXIT;
    }
}
