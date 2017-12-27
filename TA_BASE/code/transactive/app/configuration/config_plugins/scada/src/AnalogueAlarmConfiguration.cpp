/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AnalogueAlarmConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for analogue alarms.
  */

#pragma warning(disable:4786 4284)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/AnalogueAlarmConfiguration.h"
#include "app/configuration/config_plugins/scada/src/AnalogueEngUnitsConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

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
	const int MAX_SCROLL_BAR_SIZE = 240;

    // The names of the entity parameters we will use
	const std::string AnalogueAlarmConfiguration::ALARM_THRESHOLD     = "AlarmThreshold";
    const std::string AnalogueAlarmConfiguration::LOLO_ALARM_ENABLED  = "LoLoAlarmEnabled";
    const std::string AnalogueAlarmConfiguration::LO_ALARM_ENABLED    = "LoAlarmEnabled";
    const std::string AnalogueAlarmConfiguration::HIHI_ALARM_ENABLED  = "HiHiAlarmEnabled";
    const std::string AnalogueAlarmConfiguration::HI_ALARM_ENABLED    = "HiAlarmEnabled";
    const std::string AnalogueAlarmConfiguration::LOLO_ALARM_DELAY    = "LoLoAlarmDelay";
    const std::string AnalogueAlarmConfiguration::LO_ALARM_DELAY      = "LoAlarmDelay";
    const std::string AnalogueAlarmConfiguration::HIHI_ALARM_DELAY    = "HiHiAlarmDelay";
    const std::string AnalogueAlarmConfiguration::HI_ALARM_DELAY      = "HiAlarmDelay";
    const std::string AnalogueAlarmConfiguration::LOLO_ALARM_MESSAGE  = "LoLoAlarmMessage";
    const std::string AnalogueAlarmConfiguration::LO_ALARM_MESSAGE    = "LoAlarmMessage";
    const std::string AnalogueAlarmConfiguration::HIHI_ALARM_MESSAGE  = "HiHiAlarmMessage";
    const std::string AnalogueAlarmConfiguration::HI_ALARM_MESSAGE    = "HiAlarmMessage";
    const std::string AnalogueAlarmConfiguration::LOLO_ALARM_SEVERITY = "LoLoSeverity";
    const std::string AnalogueAlarmConfiguration::LO_ALARM_SEVERITY   = "LoSeverity";
    const std::string AnalogueAlarmConfiguration::HIHI_ALARM_SEVERITY = "HiHiSeverity";
    const std::string AnalogueAlarmConfiguration::HI_ALARM_SEVERITY   = "HiSeverity";
    const std::string AnalogueAlarmConfiguration::LOLO_ALARM_LIMIT    = "LoLoAlarmValue";
    const std::string AnalogueAlarmConfiguration::LO_ALARM_LIMIT      = "LoAlarmValue";
    const std::string AnalogueAlarmConfiguration::HIHI_ALARM_LIMIT    = "HiHiAlarmValue";
    const std::string AnalogueAlarmConfiguration::HI_ALARM_LIMIT      = "HiAlarmValue";
    const std::string AnalogueAlarmConfiguration::HI_ALARM_MMS        = "HiAlarmMMS";
    const std::string AnalogueAlarmConfiguration::HIHI_ALARM_MMS      = "HiHiAlarmMMS";
    const std::string AnalogueAlarmConfiguration::LO_ALARM_MMS        = "LoAlarmMMS";
    const std::string AnalogueAlarmConfiguration::LOLO_ALARM_MMS      = "LoLoAlarmMMS";

	const std::string AnalogueAlarmConfiguration::OH_ALARM_ENABLED = "OverScaleHiAlarmEnabled";
	const std::string AnalogueAlarmConfiguration::OH_ALARM_DELAY = "OverScaleHiAlarmDelay";
	const std::string AnalogueAlarmConfiguration::OH_ALARM_MESSAGE  = "OverScaleHiAlarmMessage";
	const std::string AnalogueAlarmConfiguration::OH_ALARM_SEVERITY = "OverScaleHiSeverity";
	const std::string AnalogueAlarmConfiguration::OH_ALARM_LIMIT = "OverScaleHiAlarmValue";
	const std::string AnalogueAlarmConfiguration::OH_ALARM_MMS = "OverScaleHiAlarmMMS";

	const std::string AnalogueAlarmConfiguration::OL_ALARM_ENABLED = "OverScaleLoAlarmEnabled";
	const std::string AnalogueAlarmConfiguration::OL_ALARM_DELAY = "OverScaleLoAlarmDelay";    
	const std::string AnalogueAlarmConfiguration::OL_ALARM_MESSAGE = "OverScaleLoAlarmMessage";      
	const std::string AnalogueAlarmConfiguration::OL_ALARM_SEVERITY = "OverScaleLoSeverity";
	const std::string AnalogueAlarmConfiguration::OL_ALARM_LIMIT = "OverScaleLoAlarmValue";    
	const std::string AnalogueAlarmConfiguration::OL_ALARM_MMS = "OverScaleLoAlarmMMS";

	const std::string AnalogueAlarmConfiguration::ALARM_LIMIT_PRECISION = "Precision"; //TD15446

    // The title of the dialog that allows the alarm severity to be changed
    const CString AnalogueAlarmConfiguration::CHANGE_ALARMSEVERITY_DIALOG_TITLE = "Change Alarm Severity";

    // The limit in length for the text fields.
    const int AnalogueAlarmConfiguration::MESSAGE_FIELD_LENGTH = 128;
    const int AnalogueAlarmConfiguration::DELAY_FIELD_LENGTH   = 9;
    const int AnalogueAlarmConfiguration::LIMIT_FIELD_LENGTH   = 9;

    const std::string AnalogueAlarmConfiguration::NO_SEVERITY = "<None>";

    
    IMPLEMENT_DYNCREATE(AnalogueAlarmConfiguration, CPropertyPage)

    AnalogueAlarmConfiguration::AnalogueAlarmConfiguration()
	:
	m_currentEntity(NULL),
	m_dataPointEntityData(NULL), //TD15446
	m_parent(NULL),
    m_databaseAccessor(NULL),
	m_previousScrollBoxPos ( 0 ),
	CPropertyPage ( AnalogueAlarmConfiguration::IDD, IDS_PROPPAGE_ANALOGUE_ALARMS )
    {
	    //{{AFX_DATA_INIT(AnalogueAlarmConfiguration)
	    m_highCheck = FALSE;
	    m_highHighCheck = FALSE;
	    m_lowCheck = FALSE;
	    m_lowLowCheck = FALSE;
		m_overScaleHiAlarmChecked = FALSE;
		m_overScaleLoAlarmChecked = FALSE;
		m_alarmThreshold = _T("0");
	    m_highDelay = _T("");
	    m_highHighDelay = _T("");
	    m_lowDelay = _T("");
	    m_lowLowDelay = _T("");
	    m_highMessage = _T("");
	    m_highHighMessage = _T("");
	    m_lowMessage = _T("");
	    m_lowLowMessage = _T("");
	    m_highSeverity = _T("");
	    m_highHighSeverity = _T("");
	    m_lowSeverity = _T("");
	    m_lowLowSeverity = _T("");
	    m_highLimit = _T("");
	    m_highHighLimit = _T("");
	    m_lowLimit = _T("");
	    m_lowLowLimit = _T("");
		m_overScaleHiAlarmDelay = _T("");
		m_overScaleLoAlarmDelay = _T("");
		m_overScaleHiAlarmLimit = _T("");
		m_overScaleLoAlarmLimit = _T("");
		m_overScaleHiAlarmMessage = _T("");
		m_overScaleLoAlarmMessage = _T("");
		m_overScaleHiAlarmSeverity = _T("");
		m_overScaleLoAlarmSeverity = _T("");
	//}}AFX_DATA_INIT
    }


    AnalogueAlarmConfiguration::~AnalogueAlarmConfiguration()
    {
		//TD15446
		if (m_dataPointEntityData != NULL)
		{
			delete m_dataPointEntityData;
		}
    }


    void AnalogueAlarmConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AnalogueAlarmConfiguration)
		DDX_Control(pDX, IDC_EDIT_LIMIT_LOLO, m_loloLimitControl);
		DDX_Control(pDX, IDC_EDIT_LIMIT_LO, m_loLimitControl);
		DDX_Control(pDX, IDC_EDIT_LIMIT_HIHI, m_hihiLimitControl);
		DDX_Control(pDX, IDC_EDIT_LIMIT_HI, m_hiLimitControl);
		DDX_Control(pDX, IDC_EDIT_DELAY_LOLO, m_loloDelayControl);
		DDX_Control(pDX, IDC_EDIT_DELAY_LO, m_loDelayControl);
		DDX_Control(pDX, IDC_EDIT_ALARM_THRESHOLD, m_alarmThresholdControl);
		DDX_Control(pDX, IDC_EDIT_DELAY_HIHI, m_hihiDelayControl);
		DDX_Control(pDX, IDC_EDIT_DELAY_HI, m_hiDelayControl);
		DDX_Control(pDX, IDC_EDIT_MESSAGE_LOLO, m_loloMessageControl);
		DDX_Control(pDX, IDC_EDIT_MESSAGE_LO, m_loMessageControl);
		DDX_Control(pDX, IDC_EDIT_MESSAGE_HI, m_hiMessageControl);
		DDX_Control(pDX, IDC_EDIT_MESSAGE_HIHI, m_hihiMessageControl);
		DDX_Control(pDX, IDC_COMBO_MMS_HI, m_hiMmsControl);
		DDX_Control(pDX, IDC_COMBO_MMS_HIHI, m_hihiMmsControl);
		DDX_Control(pDX, IDC_COMBO_MMS_LO, m_loMmsControl);
		DDX_Control(pDX, IDC_COMBO_MMS_LOLO, m_loloMmsControl);
		DDX_Control(pDX, IDC_EDIT_SEVERITY_OL, m_overScaleLoAlarmSeverityControl);
		DDX_Control(pDX, IDC_EDIT_SEVERITY_OH, m_overScaleHiAlarmSeverityControl);
		DDX_Control(pDX, IDC_EDIT_MESSAGE_OL, m_overScaleLoAlarmMessageControl);
		DDX_Control(pDX, IDC_EDIT_MESSAGE_OH, m_overScaleHiAlarmMessageControl);
		DDX_Control(pDX, IDC_EDIT_LIMIT_OL, m_overScaleLoAlarmLimitControl);
		DDX_Control(pDX, IDC_EDIT_LIMIT_OH, m_overScaleHiAlarmLimitControl);
		DDX_Control(pDX, IDC_EDIT_DELAY_OL, m_overScaleLoAlarmDelayControl);
		DDX_Control(pDX, IDC_EDIT_DELAY_OH, m_overScaleHiAlarmDelayControl);
		DDX_Control(pDX, IDC_COMBO_MMS_OL, m_overScaleLoAlarmMmsControl);
		DDX_Control(pDX, IDC_COMBO_MMS_OH, m_overScaleHiAlarmMmsControl);
  	    DDX_Check(pDX, IDC_CHECK_ANALOGUE_HI_ALARM, m_highCheck);
	    DDX_Check(pDX, IDC_CHECK_ANALOGUE_HIHI_ALARM, m_highHighCheck);
	    DDX_Check(pDX, IDC_CHECK_ANALOGUE_LO_ALARM, m_lowCheck);
	    DDX_Check(pDX, IDC_CHECK_ANALOGUE_LOLO_ALARM, m_lowLowCheck);
		DDX_Text(pDX, IDC_EDIT_ALARM_THRESHOLD, m_alarmThreshold);
	    DDX_Text(pDX, IDC_EDIT_DELAY_HI, m_highDelay);
	    DDX_Text(pDX, IDC_EDIT_DELAY_HIHI, m_highHighDelay);
	    DDX_Text(pDX, IDC_EDIT_DELAY_LO, m_lowDelay);
	    DDX_Text(pDX, IDC_EDIT_DELAY_LOLO, m_lowLowDelay);
	    DDX_Text(pDX, IDC_EDIT_MESSAGE_HI, m_highMessage);
	    DDX_Text(pDX, IDC_EDIT_MESSAGE_HIHI, m_highHighMessage);
	    DDX_Text(pDX, IDC_EDIT_MESSAGE_LO, m_lowMessage);
	    DDX_Text(pDX, IDC_EDIT_MESSAGE_LOLO, m_lowLowMessage);
	    DDX_Text(pDX, IDC_EDIT_SEVERITY_HI, m_highSeverity);
	    DDX_Text(pDX, IDC_EDIT_SEVERITY_HIHI, m_highHighSeverity);
	    DDX_Text(pDX, IDC_EDIT_SEVERITY_LO, m_lowSeverity);
	    DDX_Text(pDX, IDC_EDIT_SEVERITY_LOLO, m_lowLowSeverity);
	    DDX_Text(pDX, IDC_EDIT_LIMIT_HI, m_highLimit);
	    DDX_Text(pDX, IDC_EDIT_LIMIT_HIHI, m_highHighLimit);
	    DDX_Text(pDX, IDC_EDIT_LIMIT_LO, m_lowLimit);
	    DDX_Text(pDX, IDC_EDIT_LIMIT_LOLO, m_lowLowLimit);
		DDX_Check(pDX, IDC_CHECK_ANALOGUE_OH_ALARM, m_overScaleHiAlarmChecked);
		DDX_Check(pDX, IDC_CHECK_ANALOGUE_OL_ALARM, m_overScaleLoAlarmChecked);
		DDX_Text(pDX, IDC_EDIT_DELAY_OH, m_overScaleHiAlarmDelay);
		DDX_Text(pDX, IDC_EDIT_DELAY_OL, m_overScaleLoAlarmDelay);
		DDX_Text(pDX, IDC_EDIT_LIMIT_OH, m_overScaleHiAlarmLimit);
		DDX_Text(pDX, IDC_EDIT_LIMIT_OL, m_overScaleLoAlarmLimit);
		DDX_Text(pDX, IDC_EDIT_MESSAGE_OH, m_overScaleHiAlarmMessage);
		DDX_Text(pDX, IDC_EDIT_MESSAGE_OL, m_overScaleLoAlarmMessage);
		DDX_Text(pDX, IDC_EDIT_SEVERITY_OH, m_overScaleHiAlarmSeverity);
		DDX_Text(pDX, IDC_EDIT_SEVERITY_OL, m_overScaleLoAlarmSeverity);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AnalogueAlarmConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(AnalogueAlarmConfiguration)
		ON_BN_CLICKED(IDC_CHANGE_SEVERITY_HI, onChangeSeverityHigh)		
		ON_BN_CLICKED(IDC_CHANGE_SEVERITY_HIHI, onChangeSeverityHighHigh)
		ON_BN_CLICKED(IDC_CHANGE_SEVERITY_LOLO, onChangeSeverityLowLow)
		ON_BN_CLICKED(IDC_CHANGE_SEVERITY_LO, onChangeSeverityLow)
		ON_BN_CLICKED(IDC_CHANGE_SEVERITY_OH, onChangeSeverityOverScaleHi)
		ON_BN_CLICKED(IDC_CHANGE_SEVERITY_OL, onChangeSeverityOverScaleLo)

		ON_BN_CLICKED(IDC_CHECK_ANALOGUE_HI_ALARM, onCheckAnalogueHighAlarm)
		ON_BN_CLICKED(IDC_CHECK_ANALOGUE_HIHI_ALARM, onCheckAnalogueHighHighAlarm)
		ON_BN_CLICKED(IDC_CHECK_ANALOGUE_LO_ALARM, onCheckAnalogueLowAlarm)
		ON_BN_CLICKED(IDC_CHECK_ANALOGUE_LOLO_ALARM, onCheckAnalogueLowLowAlarm)
		ON_BN_CLICKED(IDC_CHECK_ANALOGUE_OH_ALARM, onCheckAnalogueOverScaleHiAlarm)
		ON_BN_CLICKED(IDC_CHECK_ANALOGUE_OL_ALARM, onCheckAnalogueOverScaleLoAlarm)

		ON_EN_KILLFOCUS(IDC_EDIT_ALARM_THRESHOLD, onKillfocusEditAlarmThreshold)

		ON_EN_KILLFOCUS(IDC_EDIT_DELAY_HI, onKillfocusEditDelayHigh)
		ON_EN_KILLFOCUS(IDC_EDIT_DELAY_HIHI, onKillfocusEditDelayHighHigh)
		ON_EN_KILLFOCUS(IDC_EDIT_DELAY_LO, onKillfocusEditDelayLowLow)
		ON_EN_KILLFOCUS(IDC_EDIT_DELAY_LOLO, onKillfocusEditDelayLow)
		ON_EN_KILLFOCUS(IDC_EDIT_DELAY_OH, onKillfocusEditDelayOverScaleHi)
		ON_EN_KILLFOCUS(IDC_EDIT_DELAY_OL, onKillfocusEditDelayOverScaleLo)

		ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_HI, onKillfocusEditMessageHigh)
		ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_HIHI, onKillfocusEditMessageHighHigh)
		ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_LO, onKillfocusEditMessageLow)
		ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_LOLO, onKillfocusEditMessageLowLow)
		ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_OH, onKillfocusEditMessageOverScaleHi)
		ON_EN_KILLFOCUS(IDC_EDIT_MESSAGE_OL, onKillfocusEditMessageOverScaleLo)

		ON_EN_KILLFOCUS(IDC_EDIT_LIMIT_HI, onKillfocusEditLimitHigh)
		ON_EN_KILLFOCUS(IDC_EDIT_LIMIT_HIHI, onKillfocusEditLimitHighHigh)
		ON_EN_KILLFOCUS(IDC_EDIT_LIMIT_LO, onKillfocusEditLimitLow)
		ON_EN_KILLFOCUS(IDC_EDIT_LIMIT_LOLO, onKillfocusEditLimitLowLow)
		ON_EN_KILLFOCUS(IDC_EDIT_LIMIT_OH, onKillfocusEditLimitOverScaleHi)
		ON_EN_KILLFOCUS(IDC_EDIT_LIMIT_OL, onKillfocusEditLimitOverScaleLo)

		ON_CBN_SELCHANGE(IDC_COMBO_MMS_HI, onSelChangeHiMms)
		ON_CBN_SELCHANGE(IDC_COMBO_MMS_HIHI, onSelChangeHiHiMms)
		ON_CBN_SELCHANGE(IDC_COMBO_MMS_LO, onSelChangeLowMms)
		ON_CBN_SELCHANGE(IDC_COMBO_MMS_LOLO, onSelChangeLowLowMms)	
		ON_CBN_SELCHANGE(IDC_COMBO_MMS_OH, onSelchangeComboMmsOverScaleHi)
		ON_CBN_SELCHANGE(IDC_COMBO_MMS_OL, onSelchangeComboMmsOverScaleLo)

		ON_WM_VSCROLL()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    void AnalogueAlarmConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                  ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;
        m_databaseAccessor = &databaseAccessor;

		//TD15446, for load and save alarm enable/alarm threshold value
		//if the page is changed to display other datapoint, need to recreate entitydata
		if (m_dataPointEntityData != NULL && m_dataPointEntityData->getKey() != m_currentEntity->getKey())
		{ 
			delete m_dataPointEntityData;
			m_dataPointEntityData = NULL;
		}

		if (m_dataPointEntityData == NULL)
		{
			m_dataPointEntityData = new TA_Base_Core::DataPointEntityData(m_currentEntity->getKey());
		}

		//get the precision of analogue datapoint for formatting the alarm threshold value
		std::string precision = m_currentEntity->getParameterValue(ALARM_LIMIT_PRECISION);
		if (precision.empty() != true)
		{
			m_alarmLimitFormatString = "%." + precision + "f";
		}
		else
		{
			m_alarmLimitFormatString = "%f"; //use the default precision.
		}

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void AnalogueAlarmConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");

		parametersToIgnore.insert(ALARM_THRESHOLD);
        //parametersToIgnore.insert(LOLO_ALARM_ENABLED);
        //parametersToIgnore.insert(LO_ALARM_ENABLED);
        //parametersToIgnore.insert(HIHI_ALARM_ENABLED);
        //parametersToIgnore.insert(HI_ALARM_ENABLED);
        parametersToIgnore.insert(LOLO_ALARM_DELAY);
        parametersToIgnore.insert(LO_ALARM_DELAY);
        parametersToIgnore.insert(HIHI_ALARM_DELAY);
        parametersToIgnore.insert(HI_ALARM_DELAY);
        parametersToIgnore.insert(LOLO_ALARM_MESSAGE);
        parametersToIgnore.insert(LO_ALARM_MESSAGE);
        parametersToIgnore.insert(HIHI_ALARM_MESSAGE);
        parametersToIgnore.insert(HI_ALARM_MESSAGE);
        parametersToIgnore.insert(LOLO_ALARM_SEVERITY);
        parametersToIgnore.insert(LO_ALARM_SEVERITY);
        parametersToIgnore.insert(HIHI_ALARM_SEVERITY);
        parametersToIgnore.insert(HI_ALARM_SEVERITY);
        //parametersToIgnore.insert(LOLO_ALARM_LIMIT);
        //parametersToIgnore.insert(LO_ALARM_LIMIT);
        //parametersToIgnore.insert(HIHI_ALARM_LIMIT);
        //parametersToIgnore.insert(HI_ALARM_LIMIT);
        parametersToIgnore.insert(HI_ALARM_MMS);
        parametersToIgnore.insert(HIHI_ALARM_MMS);
        parametersToIgnore.insert(LO_ALARM_MMS);
        parametersToIgnore.insert(LOLO_ALARM_MMS);
        //parametersToIgnore.insert(OH_ALARM_ENABLED);
        parametersToIgnore.insert(OH_ALARM_DELAY);
		parametersToIgnore.insert(OH_ALARM_MESSAGE);
		parametersToIgnore.insert(OH_ALARM_SEVERITY);
		//parametersToIgnore.insert(OH_ALARM_LIMIT);
		parametersToIgnore.insert(OH_ALARM_MMS);
		//parametersToIgnore.insert(OL_ALARM_ENABLED);
        parametersToIgnore.insert(OL_ALARM_DELAY);        
        parametersToIgnore.insert(OL_ALARM_MESSAGE);        
        parametersToIgnore.insert(OL_ALARM_SEVERITY);
        //parametersToIgnore.insert(OL_ALARM_LIMIT);     
        parametersToIgnore.insert(OL_ALARM_MMS);
        parametersToIgnore.insert(ALARM_LIMIT_PRECISION); //TD15446
		


/**/
        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");
        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");

        FieldValidityChecker checker;

        checkValidity(HIHI_ALARM_LIMIT, IDS_LABEL_HIHI_LIMIT, checker, params);
        checkValidity(HI_ALARM_LIMIT, IDS_LABEL_HI_LIMIT, checker, params);
        checkValidity(LO_ALARM_LIMIT, IDS_LABEL_LO_LIMIT, checker, params);
        checkValidity(LOLO_ALARM_LIMIT, IDS_LABEL_LOLO_LIMIT, checker, params);
		
		//TD15446
        checkValidity(OH_ALARM_LIMIT, IDS_LABEL_OH_LIMIT, checker, params);
        checkValidity(OL_ALARM_LIMIT, IDS_LABEL_OL_LIMIT, checker, params);
		
        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // AnalogueAlarmConfiguration message handlers

    BOOL AnalogueAlarmConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();

        m_hihiMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
        m_hiMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
        m_loloMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
        m_loMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
		m_overScaleLoAlarmMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);
		m_overScaleHiAlarmMessageControl.SetLimitText(MESSAGE_FIELD_LENGTH);

		m_alarmThresholdControl.SetLimitText(DELAY_FIELD_LENGTH);
	    m_hihiDelayControl.SetLimitText(DELAY_FIELD_LENGTH);
	    m_hiDelayControl.SetLimitText(DELAY_FIELD_LENGTH);
	    m_loloDelayControl.SetLimitText(DELAY_FIELD_LENGTH);
	    m_loDelayControl.SetLimitText(DELAY_FIELD_LENGTH);
		m_overScaleLoAlarmDelayControl.SetLimitText(DELAY_FIELD_LENGTH);
		m_overScaleHiAlarmDelayControl.SetLimitText(DELAY_FIELD_LENGTH);

        m_hihiLimitControl.SetLimitText(LIMIT_FIELD_LENGTH);
	    m_hiLimitControl.SetLimitText(LIMIT_FIELD_LENGTH);
	    m_loloLimitControl.SetLimitText(LIMIT_FIELD_LENGTH);
	    m_loLimitControl.SetLimitText(LIMIT_FIELD_LENGTH);
		m_overScaleLoAlarmLimitControl.SetLimitText(LIMIT_FIELD_LENGTH);
		m_overScaleHiAlarmLimitControl.SetLimitText(LIMIT_FIELD_LENGTH);

        // Populate available alarm MMS settings
        ScadaDetailView::setupMmsCombo(m_hiMmsControl);
        ScadaDetailView::setupMmsCombo(m_hihiMmsControl);
        ScadaDetailView::setupMmsCombo(m_loMmsControl);
        ScadaDetailView::setupMmsCombo(m_loloMmsControl);
		ScadaDetailView::setupMmsCombo(m_overScaleLoAlarmMmsControl);
		ScadaDetailView::setupMmsCombo(m_overScaleHiAlarmMmsControl);

		//
		// setup scroll bar
		//

		EnableScrollBar( SB_VERT );

		int nmin = 0;
		int nmax = MAX_SCROLL_BAR_SIZE;
		UINT npage = 1;
		int npos = 0;

		SCROLLINFO si = { sizeof(SCROLLINFO), SIF_ALL, nmin, nmax, npage, npos };
		SetScrollInfo ( SB_VERT, &si );

		//
		// load the page
		//

        loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void AnalogueAlarmConfiguration::onChangeSeverityHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");

        changeSeverity(m_highSeverity, HI_ALARM_SEVERITY);
    }


    void AnalogueAlarmConfiguration::onChangeSeverityHighHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        changeSeverity(m_highHighSeverity, HIHI_ALARM_SEVERITY);
    }


    void AnalogueAlarmConfiguration::onChangeSeverityLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        changeSeverity(m_lowSeverity, LO_ALARM_SEVERITY);
    }

    
    void AnalogueAlarmConfiguration::onChangeSeverityLowLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        changeSeverity(m_lowLowSeverity, LOLO_ALARM_SEVERITY);
    }


    void AnalogueAlarmConfiguration::onCheckAnalogueHighAlarm() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onCheckAnalogueHighHighAlarm() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onCheckAnalogueLowAlarm() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onCheckAnalogueLowLowAlarm() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }



	void AnalogueAlarmConfiguration::onKillfocusEditAlarmThreshold() 
	{
		TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
		
		UpdateData(TRUE);
		m_currentEntity->setParameterValue(std::string(m_alarmThreshold),ALARM_THRESHOLD);

		if (m_parent != NULL)
		{
			m_parent->refreshButtonAvailability();
		}
	}

    
	void AnalogueAlarmConfiguration::onKillfocusEditDelayHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_highDelay),HI_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditDelayHighHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_highHighDelay),HIHI_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditDelayLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_lowDelay),LO_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }

    
    void AnalogueAlarmConfiguration::onKillfocusEditDelayLowLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_lowLowDelay),LOLO_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditMessageHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_highMessage),HI_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditMessageHighHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_highHighMessage),HIHI_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditMessageLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_lowMessage),LO_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditMessageLowLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_lowLowMessage),LOLO_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditLimitHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditLimitHighHigh() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditLimitLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueAlarmConfiguration::onKillfocusEditLimitLowLow() 
    {
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void AnalogueAlarmConfiguration::loadEntityIntoPage()
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

		// alarm threshold data
		m_alarmThreshold = "0";
        try
        {
	        m_alarmThreshold = m_currentEntity->getParameterValue(ALARM_THRESHOLD).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the Alarm Threshold");
        }

        try
        {
            loadHiHiData();
            loadHiData();
            loadLoData();
            loadLoLoData();
		    loadOverScaleHiData();
		    loadOverScaleLoData();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not load entity data");
        }

		// reset the scroll box position
		m_previousScrollBoxPos = 0;
		SetScrollPos ( SB_VERT, m_previousScrollBoxPos );
        
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


    void AnalogueAlarmConfiguration::loadHiHiData()
    {
        FUNCTION_ENTRY("loadHiHiData");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        try
        {
            //TD15446
			m_highHighCheck = m_databaseAccessor->getDpStateHelper().getHiHiAlarmEnabled( m_currentEntity->getKey() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the High High alarm was enabled or not");
        }

        try
        {
	        m_highHighMessage = m_currentEntity->getParameterValue(HIHI_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the High High alarm message");
        }

        try
        {
	        m_highHighDelay = m_currentEntity->getParameterValue(HIHI_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the High High alarm delay");
        }

        try
        {
			m_highHighLimit.Format( m_alarmLimitFormatString.c_str(),
                                    m_databaseAccessor->getDpStateHelper().getHiHiValue( m_currentEntity->getKey() ) );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the High High alarm limit/value");
        }

        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(HIHI_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_highHighSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the high alarm severity");
        }

        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(HIHI_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_hihiMmsControl, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the HiHi Alarm MMS");
        }

        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::loadHiData()
    {
        FUNCTION_ENTRY("loadHiData");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        try
        {
            //TD15446
            m_highCheck = m_databaseAccessor->getDpStateHelper().getHiAlarmEnabled( m_currentEntity->getKey() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the High alarm was enabled or not");
        }

        try
        {
	        m_highMessage = m_currentEntity->getParameterValue(HI_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the High alarm message");
        }

        try
        {
	        m_highDelay = m_currentEntity->getParameterValue(HI_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the High alarm delay");
        }

        try
        {
			m_highLimit.Format( m_alarmLimitFormatString.c_str(),
                                m_databaseAccessor->getDpStateHelper().getHiValue( m_currentEntity->getKey() ) );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the High alarm limit/value");
        }

        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(HI_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_highSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the high alarm severity");
        }

        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(HI_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_hiMmsControl, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the Hi Alarm MMS");
        }

        FUNCTION_EXIT;
    }

    
    void AnalogueAlarmConfiguration::loadLoData()
    {
        FUNCTION_ENTRY("loadLoData");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        try
        {
            //TD15446
            m_lowCheck = m_databaseAccessor->getDpStateHelper().getLoAlarmEnabled( m_currentEntity->getKey() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the low alarm was enabled or not");
        }

        try
        {
	        m_lowMessage = m_currentEntity->getParameterValue(LO_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low alarm message");
        }

        try
        {
	        m_lowDelay = m_currentEntity->getParameterValue(LO_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low alarm delay");
        }

        try
        {
			m_lowLimit.Format( m_alarmLimitFormatString.c_str(),
                               m_databaseAccessor->getDpStateHelper().getLoValue( m_currentEntity->getKey() ) );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low alarm limit/value");
        }

        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(LO_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_lowSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low alarm severity");
        }

        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(LO_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_loMmsControl, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the Lo Alarm MMS");
        }

        FUNCTION_EXIT;
    }
    
    
    void AnalogueAlarmConfiguration::loadLoLoData()
    {
        FUNCTION_ENTRY("loadLoLoData");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        try
        {
            //TD15446
            m_lowLowCheck = m_databaseAccessor->getDpStateHelper().getLoLoAlarmEnabled( m_currentEntity->getKey() );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the low low alarm was enabled or not");
        }

        try
        {
	        m_lowLowMessage = m_currentEntity->getParameterValue(LOLO_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low low alarm message");
        }

        try
        {
	        m_lowLowDelay = m_currentEntity->getParameterValue(LOLO_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low low alarm delay");
        }

        try
        {
			m_lowLowLimit.Format( m_alarmLimitFormatString.c_str(), 
                                  m_databaseAccessor->getDpStateHelper().getLoLoValue( m_currentEntity->getKey() ) );
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low low alarm limit/value");
        }

        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(LOLO_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_lowLowSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the low low alarm severity");
        }

        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(LOLO_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_loloMmsControl, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the LoLo Alarm MMS");
        }

        FUNCTION_EXIT;
    }


	void AnalogueAlarmConfiguration::loadOverScaleHiData()
	{
        FUNCTION_ENTRY("loadOverScaleHiData");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

		// load m_overScaleHiAlarmChecked
		try
		{
            //TD15446
            m_overScaleHiAlarmChecked = m_databaseAccessor->getDpStateHelper().getOverScaleHiAlarmEnabled( m_currentEntity->getKey() );
		}
		catch ( ... )
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale high was enabled or not.");
		}

		// load m_overScaleHiAlarmMessage
        try
        {
	        m_overScaleHiAlarmMessage = m_currentEntity->getParameterValue(OH_ALARM_MESSAGE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale high alarm message");
        }

		// load m_overScaleHiAlarmDelay
        try
        {
	        m_overScaleHiAlarmDelay = m_currentEntity->getParameterValue(OH_ALARM_DELAY).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale high alarm delay");
        }

		// load m_overScaleHiAlarmLimit
        try
        {
			m_overScaleHiAlarmLimit.Format( m_alarmLimitFormatString.c_str(),
                                            m_databaseAccessor->getDpStateHelper().getOverScaleHiValue(m_currentEntity->getKey()));
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale high alarm limit");
        }

		// load m_overScaleHiAlarmSeverity
        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue(OH_ALARM_SEVERITY);

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_overScaleHiAlarmSeverity = m_databaseAccessor->getAlarmSeverityName(alarmSeverityKey).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale high alarm severity");
		}

		// load m_overScaleHiAlarmMmsControl
        try
        {
            std::string mmsState = m_currentEntity->getParameterValue(OH_ALARM_MMS);
            ScadaDetailView::setMmsStateComboSelection(m_overScaleHiAlarmMmsControl, mmsState);
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale high alarm mms state");
        }

        FUNCTION_EXIT;
	}


	void AnalogueAlarmConfiguration::loadOverScaleLoData()
	{
        FUNCTION_ENTRY("loadOverScaleLoData");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

		// load m_overScaleLoAlarmChecked
		try
		{
            //TD15446
			m_overScaleLoAlarmChecked = m_databaseAccessor->getDpStateHelper().getOverScaleLoAlarmEnabled(m_currentEntity->getKey());
		}
		catch ( ... )
		{
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale low alarm was enabled or not");
		}

		// load m_overScaleLoAlarmMessage
        try
        {
	        m_overScaleLoAlarmMessage = m_currentEntity->getParameterValue ( OL_ALARM_MESSAGE ).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale low alarm message");
        }

		// load m_overScaleLoAlarmDelay
        try
        {
	        m_overScaleLoAlarmDelay = m_currentEntity->getParameterValue ( OL_ALARM_DELAY ).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale low alarm delay");
        }

		// load m_overScaleLoAlarmLimit
        try
        {
			//TD15446
			m_overScaleLoAlarmLimit.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getOverScaleLoValue(m_currentEntity->getKey()));
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale low alarm limit");
        }

		// load m_overScaleLoAlarmSeverity
        try
        {
            std::string parameterValue = m_currentEntity->getParameterValue ( OL_ALARM_SEVERITY );

            // Now convert the string into an unsigned long
            std::istringstream stringToConvert;
            stringToConvert.str(parameterValue.c_str());
            unsigned long alarmSeverityKey;
            stringToConvert >> alarmSeverityKey;
            
            m_overScaleLoAlarmSeverity = m_databaseAccessor->getAlarmSeverityName ( alarmSeverityKey ).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale low alarm severity");
		}

		// load m_overScaleLoAlarmMmsControl
        try
        {
            std::string mmsState = m_currentEntity->getParameterValue ( OL_ALARM_MMS );
            ScadaDetailView::setMmsStateComboSelection ( m_overScaleLoAlarmMmsControl, mmsState );
        }
        catch (...)
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the overscale low alarm mms state");
        }


        FUNCTION_EXIT;
	}
    
    
    void AnalogueAlarmConfiguration::changeSeverity(CString& currentSeverity, const std::string& entityParameterName)
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

        
    void AnalogueAlarmConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        UpdateData(TRUE);

        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_HIHI)->EnableWindow(m_highHighCheck);
        GetDlgItem(IDC_EDIT_SEVERITY_HIHI)->EnableWindow(m_highHighCheck);
        GetDlgItem(IDC_EDIT_MESSAGE_HIHI)->EnableWindow(m_highHighCheck);
        GetDlgItem(IDC_EDIT_DELAY_HIHI)->EnableWindow(m_highHighCheck);
        GetDlgItem(IDC_EDIT_LIMIT_HIHI)->EnableWindow(m_highHighCheck);
        GetDlgItem(IDC_COMBO_MMS_HIHI)->EnableWindow(m_highHighCheck);


        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_HI)->EnableWindow(m_highCheck);
        GetDlgItem(IDC_EDIT_SEVERITY_HI)->EnableWindow(m_highCheck);
        GetDlgItem(IDC_EDIT_MESSAGE_HI)->EnableWindow(m_highCheck);
        GetDlgItem(IDC_EDIT_DELAY_HI)->EnableWindow(m_highCheck);
        GetDlgItem(IDC_EDIT_LIMIT_HI)->EnableWindow(m_highCheck);
        GetDlgItem(IDC_COMBO_MMS_HI)->EnableWindow(m_highCheck);


        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_LO)->EnableWindow(m_lowCheck);
        GetDlgItem(IDC_EDIT_SEVERITY_LO)->EnableWindow(m_lowCheck);
        GetDlgItem(IDC_EDIT_MESSAGE_LO)->EnableWindow(m_lowCheck);
        GetDlgItem(IDC_EDIT_DELAY_LO)->EnableWindow(m_lowCheck);
        GetDlgItem(IDC_EDIT_LIMIT_LO)->EnableWindow(m_lowCheck);
        GetDlgItem(IDC_COMBO_MMS_LO)->EnableWindow(m_lowCheck);


        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_LOLO)->EnableWindow(m_lowLowCheck);
        GetDlgItem(IDC_EDIT_SEVERITY_LOLO)->EnableWindow(m_lowLowCheck);
        GetDlgItem(IDC_EDIT_MESSAGE_LOLO)->EnableWindow(m_lowLowCheck);
        GetDlgItem(IDC_EDIT_DELAY_LOLO)->EnableWindow(m_lowLowCheck);
        GetDlgItem(IDC_EDIT_LIMIT_LOLO)->EnableWindow(m_lowLowCheck);
        GetDlgItem(IDC_COMBO_MMS_LOLO)->EnableWindow(m_lowLowCheck);


        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_OH)->EnableWindow(m_overScaleHiAlarmChecked);
        GetDlgItem(IDC_EDIT_SEVERITY_OH)->EnableWindow(m_overScaleHiAlarmChecked);
        GetDlgItem(IDC_EDIT_MESSAGE_OH)->EnableWindow(m_overScaleHiAlarmChecked);
        GetDlgItem(IDC_EDIT_DELAY_OH)->EnableWindow(m_overScaleHiAlarmChecked);
        GetDlgItem(IDC_EDIT_LIMIT_OH)->EnableWindow(m_overScaleHiAlarmChecked); // been requested to make this parameter not editable.  Its value is coming from the Eng Unit High Limit
        GetDlgItem(IDC_COMBO_MMS_OH)->EnableWindow(m_overScaleHiAlarmChecked);


        // If checked then enable
        GetDlgItem(IDC_CHANGE_SEVERITY_OL)->EnableWindow(m_overScaleLoAlarmChecked);
        GetDlgItem(IDC_EDIT_SEVERITY_OL)->EnableWindow(m_overScaleLoAlarmChecked);
        GetDlgItem(IDC_EDIT_MESSAGE_OL)->EnableWindow(m_overScaleLoAlarmChecked);
        GetDlgItem(IDC_EDIT_DELAY_OL)->EnableWindow(m_overScaleLoAlarmChecked);
        GetDlgItem(IDC_EDIT_LIMIT_OL)->EnableWindow(m_overScaleLoAlarmChecked); // been requested to make this parameter not editable.  Its value is coming from the Eng Unit Low Limit
        GetDlgItem(IDC_COMBO_MMS_OL)->EnableWindow(m_overScaleLoAlarmChecked);
        
        UpdateData(true);

        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::checkValidity(const std::string& parameterName,
                                                   int displayNameStringId,
                                                   const FieldValidityChecker& checker,
                                                   std::vector<std::string>& params )
    {
        if( !checker.isValidRealNumber(m_currentEntity->getParameterValue(parameterName)) )
        {
            CString dialogTitle;
            dialogTitle.LoadString(IDS_PROPPAGE_ANALOGUE_ALARMS);

            CString fieldTitle;
            fieldTitle.LoadString(displayNameStringId);
            
            std::string parameter(dialogTitle);
            parameter += " - ";
            parameter += fieldTitle;

            params.push_back(parameter);
        }
    }


    void AnalogueAlarmConfiguration::onSelChangeHiMms()
    {
        FUNCTION_ENTRY("onSelChangeHiMms");

        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_hiMmsControl);
        m_currentEntity->setParameterValue(mmsState, HI_ALARM_MMS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::onSelChangeHiHiMms()
    {
        FUNCTION_ENTRY("onSelChangeHiHiMms");

        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_hihiMmsControl);
        m_currentEntity->setParameterValue(mmsState, HIHI_ALARM_MMS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::onSelChangeLowMms()
    {
        FUNCTION_ENTRY("onSelChangeLowMms");

        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_loMmsControl);
        m_currentEntity->setParameterValue(mmsState, LO_ALARM_MMS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void AnalogueAlarmConfiguration::onSelChangeLowLowMms()
    {
        FUNCTION_ENTRY("onSelChangeLowLowMms");

        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_loloMmsControl);
        m_currentEntity->setParameterValue(mmsState, LOLO_ALARM_MMS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


	void AnalogueAlarmConfiguration::onChangeSeverityOverScaleHi() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        changeSeverity(m_overScaleHiAlarmSeverity, OH_ALARM_SEVERITY);
	}

	void AnalogueAlarmConfiguration::onChangeSeverityOverScaleLo() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        changeSeverity(m_overScaleLoAlarmSeverity, OL_ALARM_SEVERITY);
	}

	void AnalogueAlarmConfiguration::onCheckAnalogueOverScaleHiAlarm() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onCheckAnalogueOverScaleLoAlarm() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        refresh();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onSelchangeComboMmsOverScaleHi() 
	{
        FUNCTION_ENTRY("onSelchangeComboMmsOverScaleHi");

        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_overScaleHiAlarmMmsControl);
        m_currentEntity->setParameterValue(mmsState, OH_ALARM_MMS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
	}

	void AnalogueAlarmConfiguration::onSelchangeComboMmsOverScaleLo() 
	{
        FUNCTION_ENTRY("onSelchangeComboMmsOverScaleLo");

        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        std::string mmsState = ScadaDetailView::getMmsStateFromCombo(m_overScaleLoAlarmMmsControl);
        m_currentEntity->setParameterValue(mmsState, OL_ALARM_MMS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
	}

	void AnalogueAlarmConfiguration::onKillfocusEditDelayOverScaleHi() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_overScaleHiAlarmDelay),OH_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onKillfocusEditDelayOverScaleLo() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_overScaleLoAlarmDelay),OL_ALARM_DELAY);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onKillfocusEditLimitOverScaleHi() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        //TD15446
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onKillfocusEditLimitOverScaleLo() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onKillfocusEditMessageOverScaleHi() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_overScaleHiAlarmMessage),OH_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::onKillfocusEditMessageOverScaleLo() 
	{
        TA_ASSERT(m_currentEntity != NULL, "Cannot update data as the entity pointer is NULL");
	    
        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_overScaleLoAlarmMessage),OL_ALARM_MESSAGE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
	}

	void AnalogueAlarmConfiguration::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar) 
	{
		// TODO: Add your message handler code here and/or call default

		//CPropertyPage::OnVScroll(nSBCode, nPos, pScrollBar);

		// get current scroll box position
		int currentScrollBoxPos = GetScrollPos ( SB_VERT );

		switch ( nSBCode )
		{
			case SB_LINEDOWN:
			{
				// if within the range
				if ( MAX_SCROLL_BAR_SIZE > currentScrollBoxPos )
				{
					// calculate the vertical movement from the current and previous positions
					int yAmount = 2;

					// scroll the window up
					ScrollWindow ( 0, -yAmount );

					// save current position as previous
					m_previousScrollBoxPos = currentScrollBoxPos + yAmount;

					// make the scroll box move to the next position accordingly
					SetScrollPos ( SB_VERT, m_previousScrollBoxPos );
				}
				// else do nothing

				break;
			}
			case SB_LINEUP:
			{
				// if within the range
				if ( 0 < currentScrollBoxPos )
				{
					// calculate the vertical movement from the current and previous positions
					int yAmount = 2;

					// scroll the window down
					ScrollWindow ( 0, yAmount );

					// save current position as previous
					m_previousScrollBoxPos = currentScrollBoxPos - yAmount;

					// make the scroll box move to the next position accordingly
					SetScrollPos ( SB_VERT, m_previousScrollBoxPos );
				}
				// else do nothing

				break;
			}
			case SB_PAGEDOWN:
			{
				// if within the range
				if ( MAX_SCROLL_BAR_SIZE > currentScrollBoxPos )
				{
					// calculate the vertical movement from the current and previous positions
					int yAmount = 20;

					// scroll the window up
					ScrollWindow ( 0, -yAmount );

					// save current position as previous
					m_previousScrollBoxPos = currentScrollBoxPos + yAmount;

					// make the scroll box move to the next position accordingly
					SetScrollPos ( SB_VERT, m_previousScrollBoxPos );

					//as per TD 12129
					if (m_previousScrollBoxPos > MAX_SCROLL_BAR_SIZE)
					{
						ScrollWindow ( 0, m_previousScrollBoxPos - MAX_SCROLL_BAR_SIZE);
					}
				}
				// else do nothing

				break;
			}
			case SB_PAGEUP:
			{
				// if within the range
				if ( 0 < currentScrollBoxPos )
				{
					// calculate the vertical movement from the current and previous positions
					int yAmount = 20;

					// scroll the window down
					ScrollWindow ( 0, yAmount );

					// save current position as previous
					m_previousScrollBoxPos = currentScrollBoxPos - yAmount;

					// make the scroll box move to the next position accordingly
					SetScrollPos ( SB_VERT, m_previousScrollBoxPos );
				}
				// else do nothing

				break;
			}
			case SB_THUMBTRACK:
			{
				// if within the range
				if ( ( 0 <= nPos ) && 
					 ( MAX_SCROLL_BAR_SIZE >= nPos ) )
				{
					// calculate the vertical movement between current and previous positions
					int yAmount = nPos - m_previousScrollBoxPos;

					// scroll the window up/down
					ScrollWindow ( 0, -yAmount );

					// save current position as previous
					m_previousScrollBoxPos = nPos;

					// make the scroll box move to the next position accordingly
					SetScrollPos ( SB_VERT, m_previousScrollBoxPos );
				}
				// else do nothing

				break;
			}
			case SB_THUMBPOSITION:
			case SB_TOP:
			case SB_BOTTOM:
			case SB_ENDSCROLL:
			default:
			{
				// do nothing
				break;
			}
		}
		// end of switch statement
	}

	//TD15446
	bool AnalogueAlarmConfiguration::getIsAlarmStatesChanged()
	{
		if (m_hWnd == NULL || m_currentEntity == NULL ||  m_dataPointEntityData == NULL)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "The dislog is not ready yet.");
			return false;
		}

		UpdateData(TRUE);

		return(	(m_highCheck == TRUE) != m_databaseAccessor->getDpStateHelper().getHiAlarmEnabled(m_currentEntity->getKey()) ||
				(m_highHighCheck == TRUE) != m_databaseAccessor->getDpStateHelper().getHiHiAlarmEnabled(m_currentEntity->getKey()) ||
				(m_lowCheck == TRUE) != m_databaseAccessor->getDpStateHelper().getLoAlarmEnabled(m_currentEntity->getKey()) ||
				(m_lowLowCheck == TRUE) != m_databaseAccessor->getDpStateHelper().getLoLoAlarmEnabled(m_currentEntity->getKey()) ||
				(m_overScaleHiAlarmChecked == TRUE) != m_databaseAccessor->getDpStateHelper().getOverScaleHiAlarmEnabled(m_currentEntity->getKey()) ||
				(m_overScaleLoAlarmChecked == TRUE) != m_databaseAccessor->getDpStateHelper().getOverScaleLoAlarmEnabled(m_currentEntity->getKey()) ||
				atof(m_highLimit.GetBuffer(m_highLimit.GetLength())) != m_databaseAccessor->getDpStateHelper().getHiValue(m_currentEntity->getKey()) ||
				atof(m_highHighLimit.GetBuffer(m_highHighLimit.GetLength())) != m_databaseAccessor->getDpStateHelper().getHiHiValue(m_currentEntity->getKey()) ||
				atof(m_lowLimit.GetBuffer(m_lowLimit.GetLength())) != m_databaseAccessor->getDpStateHelper().getLoValue(m_currentEntity->getKey()) ||
				atof(m_lowLowLimit.GetBuffer(m_lowLowLimit.GetLength())) != m_databaseAccessor->getDpStateHelper().getLoLoValue(m_currentEntity->getKey()) ||
				atof(m_overScaleHiAlarmLimit.GetBuffer(m_overScaleHiAlarmLimit.GetLength())) != m_databaseAccessor->getDpStateHelper().getOverScaleHiValue(m_currentEntity->getKey()) ||
				atof(m_overScaleLoAlarmLimit.GetBuffer(m_overScaleLoAlarmLimit.GetLength())) != m_databaseAccessor->getDpStateHelper().getOverScaleLoValue(m_currentEntity->getKey())
			 );
	}

	//TD15446
	void AnalogueAlarmConfiguration::saveAlarmStates()
	{
		if (m_hWnd == NULL || m_currentEntity == NULL ||  m_dataPointEntityData == NULL)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "The dislog is not ready yet.");
			return;
		}

		UpdateData(TRUE);

		bool alarmEnabled = false;

		alarmEnabled = m_highCheck == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getHiAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setHiAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}

		alarmEnabled = m_highHighCheck == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getHiHiAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setHiHiAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}
		
		alarmEnabled = m_lowCheck == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getLoAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setLoAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}
		
		alarmEnabled = m_lowLowCheck == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getLoLoAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setLoLoAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}
		
		alarmEnabled = m_overScaleHiAlarmChecked == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getOverScaleHiAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setOverScaleHiAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}
		
		alarmEnabled = m_overScaleLoAlarmChecked == TRUE;
		if (alarmEnabled != m_databaseAccessor->getDpStateHelper().getOverScaleLoAlarmEnabled(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setOverScaleLoAlarmEnabled(m_currentEntity->getKey(), alarmEnabled);
		}

		double alarmValue;

		alarmValue = atof(m_highLimit.GetBuffer(m_highLimit.GetLength()));
		if (alarmValue != m_databaseAccessor->getDpStateHelper().getHiValue(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setHiValue(m_currentEntity->getKey(), alarmValue);
		}
		
		alarmValue = atof(m_highHighLimit.GetBuffer(m_highHighLimit.GetLength()));
		if (alarmValue != m_databaseAccessor->getDpStateHelper().getHiHiValue(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setHiHiValue(m_currentEntity->getKey(), alarmValue);
		}
		
		alarmValue = atof(m_lowLimit.GetBuffer(m_lowLimit.GetLength()));
		if (alarmValue != m_databaseAccessor->getDpStateHelper().getLoValue(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setLoValue(m_currentEntity->getKey(), alarmValue);
		}
		
		alarmValue = atof(m_lowLowLimit.GetBuffer(m_lowLowLimit.GetLength()));
		if (alarmValue != m_databaseAccessor->getDpStateHelper().getLoLoValue(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setLoLoValue(m_currentEntity->getKey(), alarmValue);
		}
		
		alarmValue = atof(m_overScaleHiAlarmLimit.GetBuffer(m_overScaleHiAlarmLimit.GetLength()));
		if (alarmValue != m_databaseAccessor->getDpStateHelper().getOverScaleHiValue(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setOverScaleHiValue(m_currentEntity->getKey(), alarmValue);
		}
		
		alarmValue = atof(m_overScaleLoAlarmLimit.GetBuffer(m_overScaleLoAlarmLimit.GetLength()));
		if (alarmValue != m_databaseAccessor->getDpStateHelper().getOverScaleLoValue(m_currentEntity->getKey()))
		{
			m_databaseAccessor->getDpStateHelper().setOverScaleLoValue(m_currentEntity->getKey(), alarmValue);
		}
	
	}

	//TD15446
	TA_Base_Core::ItemChanges AnalogueAlarmConfiguration::getAllItemChanges()
	{
		TA_Base_Core::ItemChanges changes;

		if (m_hWnd == NULL || m_currentEntity == NULL ||  m_dataPointEntityData == NULL)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "The dislog is not ready yet.");
			return changes;
		}
		
		UpdateData(TRUE);
		
		TA_Base_Core::Values values;
		
		values.newValue = m_highCheck == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getHiAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( HI_ALARM_ENABLED, values));
		}

		values.newValue = m_highHighCheck == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getHiHiAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( HIHI_ALARM_ENABLED, values));
		}

		values.newValue = m_lowCheck == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getLoAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( LO_ALARM_ENABLED, values));
		}
		
		values.newValue = m_lowLowCheck == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getLoLoAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( LOLO_ALARM_ENABLED, values));
		}
		
		values.newValue = m_overScaleHiAlarmChecked == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getOverScaleHiAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( OH_ALARM_ENABLED, values));
		}
		
		values.newValue = m_overScaleLoAlarmChecked == TRUE ? "1" : "0";
		values.oldValue = m_databaseAccessor->getDpStateHelper().getOverScaleLoAlarmEnabled(m_currentEntity->getKey()) ? "1" : "0";
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( OL_ALARM_ENABLED, values));
		}

		CString oldValue;
		
		values.newValue = m_highLimit;
		oldValue.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getHiValue(m_currentEntity->getKey()));
		values.oldValue = oldValue;
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( HI_ALARM_LIMIT, values));
		}
		
		values.newValue = m_highHighLimit;
		oldValue.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getHiHiValue(m_currentEntity->getKey()));
		values.oldValue = oldValue;
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( HIHI_ALARM_LIMIT, values));
		}
		
		values.newValue = m_lowLimit;
		oldValue.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getLoValue(m_currentEntity->getKey()));
		values.oldValue = oldValue;
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( LO_ALARM_LIMIT, values));
		}
		
		values.newValue = m_lowLowLimit;
		oldValue.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getLoLoValue(m_currentEntity->getKey()));
		values.oldValue = oldValue;
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( LOLO_ALARM_LIMIT, values));
		}
		
		values.newValue = m_overScaleHiAlarmLimit;
		oldValue.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getOverScaleHiValue(m_currentEntity->getKey()));
		values.oldValue = oldValue;
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( OH_ALARM_LIMIT, values));
		}
		
		values.newValue = m_overScaleLoAlarmLimit;
		oldValue.Format(m_alarmLimitFormatString.c_str(), m_databaseAccessor->getDpStateHelper().getOverScaleLoValue(m_currentEntity->getKey()));
		values.oldValue = oldValue;
		if (values.newValue != values.oldValue)
		{
			changes.insert(TA_Base_Core::ItemChanges::value_type( OL_ALARM_LIMIT, values));
		}
		
		return changes;
		
	}
}

