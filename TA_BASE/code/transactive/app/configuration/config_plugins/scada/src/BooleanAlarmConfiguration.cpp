/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/BooleanAlarmConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for boolean alarms.
  */

#pragma warning(disable:4786 4284)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/BooleanAlarmConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
using TA_Base_Core::DebugUtil;


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    // The names of the entity parameters we will use
    const std::string BooleanAlarmConfiguration::TRUE_ALARM_ENABLED   = "TrueAlarmEnable";
    const std::string BooleanAlarmConfiguration::TRUE_ALARM_DELAY     = "TrueAlarmDelay";
    const std::string BooleanAlarmConfiguration::TRUE_ALARM_MESSAGE   = "TrueAlarmMessage";
    const std::string BooleanAlarmConfiguration::TRUE_ALARM_SEVERITY  = "TrueAlarmSeverity";
    const std::string BooleanAlarmConfiguration::FALSE_ALARM_ENABLED  = "FalseAlarmEnable";
    const std::string BooleanAlarmConfiguration::FALSE_ALARM_DELAY    = "FalseAlarmDelay";
    const std::string BooleanAlarmConfiguration::FALSE_ALARM_MESSAGE  = "FalseAlarmMessage";
    const std::string BooleanAlarmConfiguration::FALSE_ALARM_SEVERITY = "FalseAlarmSeverity";
    const std::string BooleanAlarmConfiguration::TRUE_ALARM_MMS       = "TrueAlarmMMS";
    const std::string BooleanAlarmConfiguration::FALSE_ALARM_MMS      = "FalseAlarmMMS";

    // The title of the dialog that allows the alarm severity to be changed
    const CString BooleanAlarmConfiguration::CHANGE_ALARMSEVERITY_DIALOG_TITLE = "Change Alarm Severity";

    // The limit in length for the text fields.
    const int BooleanAlarmConfiguration::MESSAGE_FIELD_LENGTH = 128;
    const int BooleanAlarmConfiguration::DELAY_FIELD_LENGTH   = 9;

    const std::string BooleanAlarmConfiguration::NO_SEVERITY = "<None>";


    IMPLEMENT_DYNCREATE(BooleanAlarmConfiguration, CPropertyPage)

    BooleanAlarmConfiguration::BooleanAlarmConfiguration()
    : m_currentEntity(NULL), 
	m_dataPointEntityData(NULL), //TD15446
	m_parent(NULL), 
    m_databaseAccessor(NULL),
	CPropertyPage(BooleanAlarmConfiguration::IDD, IDS_PROPPAGE_BOOLEAN_ALARMS)
    {
	    //{{AFX_DATA_INIT(BooleanAlarmConfiguration)
	m_falseCheck = FALSE;
	m_trueCheck = FALSE;
	m_falseDelay = _T("");
	m_trueDelay = _T("");
	m_falseMessage = _T("");
	m_trueMessage = _T("");
	m_falseSeverity = _T("");
	m_trueSeverity = _T("");
	//}}AFX_DATA_INIT
    }


    BooleanAlarmConfiguration::~BooleanAlarmConfiguration()
    {
		//TD15446
		if (m_dataPointEntityData != NULL)
		{
			delete m_dataPointEntityData;
		}
    }


    void BooleanAlarmConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(BooleanAlarmConfiguration)
	DDX_Control(pDX, IDC_EDIT_DELAY_BOOLTRUE, m_trueDelayControl);
	DDX_Control(pDX, IDC_EDIT_DELAY_BOOLFALSE, m_falseDelayControl);
	DDX_Control(pDX, IDC_EDIT_MESSAGE_BOOLTRUE, m_trueMessageControl);
	DDX_Control(pDX, IDC_EDIT_MESSAGE_BOOLFALSE, m_falseMessageControl);
    DDX_Control(pDX, IDC_COMBO_MMS_TRUE, m_trueMmsCombo);
    DDX_Control(pDX, IDC_COMBO_MMS_FALSE, m_falseMmsCombo);
	DDX_Check(pDX, IDC_CHECK_BOOL_FALSE_ALARM, m_falseCheck);
	DDX_Check(pDX, IDC_CHECK_BOOL_TRUE_ALARM, m_trueCheck);
	DDX_Text(pDX, IDC_EDIT_DELAY_BOOLFALSE, m_falseDelay);
	DDX_Text(pDX, IDC_EDIT_DELAY_BOOLTRUE, m_trueDelay);
	DDX_Text(pDX, IDC_EDIT_MESSAGE_BOOLFALSE, m_falseMessage);
	DDX_Text(pDX, IDC_EDIT_MESSAGE_BOOLTRUE, m_trueMessage);
	DDX_Text(pDX, IDC_EDIT_SEVERITY_BOOLFALSE, m_falseSeverity);
	DDX_Text(pDX, IDC_EDIT_SEVERITY_BOOLTRUE, m_trueSeverity);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(BooleanAlarmConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(BooleanAlarmConfiguration)
	ON_BN_CLICKED(IDC_CHECK_BOOL_FALSE_ALARM, onCheckBoolFalseAlarm)
	ON_BN_CLICKED(IDC_CHECK_BOOL_TRUE_ALARM, onCheckBoolTrueAlarm)
	ON_BN_CLICKED(IDC_CHANGE_SEVERITY_FALSE, onChangeSeverityFalse)
	ON_BN_CLICKED(IDC_CHANGE_SEVERITY_TRUE, onChangeSeverityTrue)
	ON_EN_KILLFOCUS(IDC_EDIT_DELAY_BOOLFALSE, onKillfocusEditDelayBoolFalse)
	ON_EN_KILLFOCUS(IDC_EDIT_DELAY_BOOLTRUE, onKillfocusEditDelayBoolTrue)
	ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_BOOLFALSE, onKillfocusEditMessageBoolFalse)
	ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_BOOLTRUE, onKillfocusEditMessageBoolTrue)
    ON_CBN_SELCHANGE(IDC_COMBO_MMS_TRUE, onSelChangedTrueAlarm)
    ON_CBN_SELCHANGE(IDC_COMBO_MMS_FALSE, onSelChangedFalseAlarm)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void BooleanAlarmConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                 ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;
        m_databaseAccessor = &databaseAccessor;

		//TD15446, for load and save alarm enable
		if (m_dataPointEntityData != NULL && m_dataPointEntityData->getKey() != m_currentEntity->getKey())
		{ 
			delete m_dataPointEntityData;
			m_dataPointEntityData = NULL;
		}
		
		if (m_dataPointEntityData == NULL)
		{
			m_dataPointEntityData = new TA_Base_Core::DataPointEntityData(m_currentEntity->getKey());
		}
		
        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void BooleanAlarmConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");

        parametersToIgnore.insert(TRUE_ALARM_ENABLED);
        parametersToIgnore.insert(TRUE_ALARM_DELAY);
        parametersToIgnore.insert(TRUE_ALARM_MESSAGE);
        parametersToIgnore.insert(TRUE_ALARM_SEVERITY);
        parametersToIgnore.insert(FALSE_ALARM_ENABLED);
        parametersToIgnore.insert(FALSE_ALARM_DELAY);
        parametersToIgnore.insert(FALSE_ALARM_MESSAGE);
        parametersToIgnore.insert(FALSE_ALARM_SEVERITY);
        parametersToIgnore.insert(TRUE_ALARM_MMS);
        parametersToIgnore.insert(FALSE_ALARM_MMS);
/**/
        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");
        FUNCTION_EXIT;
    }

    
    void BooleanAlarmConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // BooleanAlarmConfiguration message handlers


    BOOL BooleanAlarmConfiguration::OnInitDialog() 
    {
        FUNCTION_ENTRY("OnInitDialog");

	    CPropertyPage::OnInitDialog();

        m_trueMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
        m_falseMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
	    m_trueDelayControl.SetLimitText(DELAY_FIELD_LENGTH);
	    m_falseDelayControl.SetLimitText(DELAY_FIELD_LENGTH);

        // Populate available alarm MMS settings
        ScadaDetailView::setupMmsCombo(m_trueMmsCombo);
        ScadaDetailView::setupMmsCombo(m_falseMmsCombo);

        loadEntityIntoPage();

        FUNCTION_EXIT;
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void BooleanAlarmConfiguration::onCheckBoolFalseAlarm() 
    {
        FUNCTION_ENTRY("onCheckBoolFalseAlarm");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::onSelChangedTrueAlarm()
    {
        FUNCTION_ENTRY("onSelChangedTrueAlarm");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);
        
        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_trueMmsCombo);
        m_currentEntity->setParameterValue(mmsState, TRUE_ALARM_MMS);

        refresh();
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::onSelChangedFalseAlarm()
    {
        FUNCTION_ENTRY("onSelChangedFalseAlarm");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);
        
        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_falseMmsCombo);
        m_currentEntity->setParameterValue(mmsState, FALSE_ALARM_MMS);

        refresh();
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::onCheckBoolTrueAlarm() 
    {
        FUNCTION_ENTRY("onCheckBoolTrueAlarm");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::onChangeSeverityFalse() 
    {
        FUNCTION_ENTRY("onChangeSeverityFalse");

        changeSeverity(m_falseSeverity, FALSE_ALARM_SEVERITY);

        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::onChangeSeverityTrue() 
    {
        FUNCTION_ENTRY("onChangeSeverityTrue");

        changeSeverity(m_trueSeverity, TRUE_ALARM_SEVERITY);

        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::onKillfocusEditDelayBoolFalse() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_falseDelay),FALSE_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void BooleanAlarmConfiguration::onKillfocusEditDelayBoolTrue() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_trueDelay),TRUE_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void BooleanAlarmConfiguration::onKillfocusEditMessageBoolFalse() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_falseMessage),FALSE_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void BooleanAlarmConfiguration::onKillfocusEditMessageBoolTrue() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_trueMessage),TRUE_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void BooleanAlarmConfiguration::loadEntityIntoPage()
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
			//TD15446
			m_trueCheck = m_databaseAccessor->getDpStateHelper().getTrueAlarmEnabled(m_currentEntity->getKey());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the true alarm was enabled or not");
        }

        try
        {
			m_falseCheck = m_databaseAccessor->getDpStateHelper().getFalseAlarmEnabled(m_currentEntity->getKey());
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the false alarm was enabled or not");
        }

        try
        {
	        m_trueMessage = m_currentEntity->getParameterValue(TRUE_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the true alarm message");
        }

        try
        {
	        m_falseMessage = m_currentEntity->getParameterValue(FALSE_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the false alarm message");
        }

        try
        {
	        m_trueDelay = m_currentEntity->getParameterValue(TRUE_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the true alarm delay");
        }

        try
        {
	        m_falseDelay = m_currentEntity->getParameterValue(FALSE_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the false alarm delay");
        }

        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(TRUE_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_trueSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the true alarm severity");
        }

        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(FALSE_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_falseSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the false alarm severity");
        }

        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(TRUE_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_trueMmsCombo, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the True Alarm MMS");
        }

        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(FALSE_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_falseMmsCombo, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the False Alarm MMS");
        }

        UpdateData(FALSE);

        refresh();

        // First GetParent() gives us the property sheet. The next one gives us the view
        m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void BooleanAlarmConfiguration::changeSeverity(CString& currentSeverity, const std::string& entityParameterName)
    {
        FUNCTION_ENTRY("changeSeverity");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllSeverities();
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

            CString keyString("");
            CString displayName("");
            // If this is a different severity then set it in our IConfigAlarmType*.
            if (dlg.getSelectedName() != NO_SEVERITY.c_str())
            {
                keyString.Format("%lu",dlg.getSelectedKey());
                displayName = dlg.getSelectedName();
            }
                
            m_currentEntity->setParameterValue( std::string(keyString), entityParameterName );

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

        
    void BooleanAlarmConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_TRUE)->EnableWindow(m_trueCheck);
        GetDlgItem(IDC_EDIT_SEVERITY_BOOLTRUE)->EnableWindow(m_trueCheck);
        GetDlgItem(IDC_EDIT_MESSAGE_BOOLTRUE)->EnableWindow(m_trueCheck);
        GetDlgItem(IDC_EDIT_DELAY_BOOLTRUE)->EnableWindow(m_trueCheck);
        GetDlgItem(IDC_COMBO_MMS_TRUE)->EnableWindow(m_trueCheck);

        GetDlgItem(IDC_CHANGE_SEVERITY_FALSE)->EnableWindow(m_falseCheck);
        GetDlgItem(IDC_EDIT_SEVERITY_BOOLFALSE)->EnableWindow(m_falseCheck);
        GetDlgItem(IDC_EDIT_MESSAGE_BOOLFALSE)->EnableWindow(m_falseCheck);
        GetDlgItem(IDC_EDIT_DELAY_BOOLFALSE)->EnableWindow(m_falseCheck);
        GetDlgItem(IDC_COMBO_MMS_FALSE)->EnableWindow(m_falseCheck);

        UpdateData(true);

        FUNCTION_EXIT;
    }

	//TD15446
	bool BooleanAlarmConfiguration::getIsAlarmStatesChanged()
	{
		if (m_hWnd == NULL || m_currentEntity == NULL ||  m_dataPointEntityData == NULL)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "The dislog is not ready yet.");
			return false;
		}

		UpdateData(TRUE);

		return(	(m_trueCheck == TRUE) != m_databaseAccessor->getDpStateHelper().getTrueAlarmEnabled(m_currentEntity->getKey()) ||
				(m_falseCheck == TRUE) != m_databaseAccessor->getDpStateHelper().getFalseAlarmEnabled(m_currentEntity->getKey()) );
	}

	//TD15446
	void BooleanAlarmConfiguration::saveAlarmStates()
	{
		if (m_hWnd == NULL || m_currentEntity == NULL ||  m_dataPointEntityData == NULL)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "The dislog is not ready yet.");
			return;
		}
		
		UpdateData(TRUE);

		bool alarmEnabled = false;

		alarmEnabled = m_trueCheck == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getTrueAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setTrueAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}

		alarmEnabled = m_falseCheck == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getFalseAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setFalseAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}
	
	}

	//TD15446
	TA_Base_Core::ItemChanges BooleanAlarmConfiguration::getAllItemChanges()
	{
		TA_Base_Core::ItemChanges changes;

		if (m_hWnd == NULL || m_currentEntity == NULL ||  m_dataPointEntityData == NULL)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "The dislog is not ready yet.");
			return changes;
		}
		
		UpdateData(TRUE);
		
		TA_Base_Core::Values values;
		
		values.newValue = m_trueCheck == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getTrueAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( TRUE_ALARM_ENABLED, values));
		}

		values.newValue = m_falseCheck == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getFalseAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( FALSE_ALARM_ENABLED, values));
		}
		
		return changes;
		
	}
}

