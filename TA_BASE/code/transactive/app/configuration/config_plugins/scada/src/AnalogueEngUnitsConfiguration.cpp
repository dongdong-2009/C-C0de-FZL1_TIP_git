/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AnalogueEngUnitsConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for analogue engineering units.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/AnalogueEngUnitsConfiguration.h"
#include "app/configuration/config_plugins/scada/src/AnalogueAlarmConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/src/IData.h"
#include "core/data_access_interface/entity_access/src/EntityData.h"
#include "core/data_access_interface/entity_access/src/EntityHelper.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

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
    const std::string AnalogueEngUnitsConfiguration::ENGUNITS_ENABLED   = "ConversionEnabled";
    const std::string AnalogueEngUnitsConfiguration::UNITS              = "UnitLabel";
    const std::string AnalogueEngUnitsConfiguration::RAW_HIGH_LIMIT     = "RawLimitHigh";
    const std::string AnalogueEngUnitsConfiguration::RAW_LOW_LIMIT      = "RawLimitLow";
    const std::string AnalogueEngUnitsConfiguration::ENG_HIGH_LIMIT     = "EngineeringLimitHigh";
    const std::string AnalogueEngUnitsConfiguration::ENG_LOW_LIMIT      = "EngineeringLimitLow";
    const std::string AnalogueEngUnitsConfiguration::UPDATE_THRESHOLD   = "UpdateThreshold";
    const std::string AnalogueEngUnitsConfiguration::UPDATE_TIME_LIMIT  = "UpdateTimeLimit";
    const std::string AnalogueEngUnitsConfiguration::ZERO_VAL_THRESHOLD = "ZeroValueThreshold";
    const std::string AnalogueEngUnitsConfiguration::PRECISION          = "Precision";

    // The limit in length for the text fields.
    const int AnalogueEngUnitsConfiguration::RAW_HIGH_FIELD_LENGTH          = 128;
    const int AnalogueEngUnitsConfiguration::RAW_LOW_FIELD_LENGTH           = 128;
    const int AnalogueEngUnitsConfiguration::ENG_HIGH_FIELD_LENGTH          = 128;
    const int AnalogueEngUnitsConfiguration::ENG_LOW_FIELD_LENGTH           = 128;
    const int AnalogueEngUnitsConfiguration::UPDATE_THRESHOLD_FIELD_LENGTH  = 9;
    const int AnalogueEngUnitsConfiguration::UNITS_FIELD_LENGTH             = 128;
    const int AnalogueEngUnitsConfiguration::UPDATE_TIME_LIMIT_FIELD_LENGTH = 9;
    const int AnalogueEngUnitsConfiguration::ZERO_VALUE_FIELD_LENGTH        = 9;
    const int AnalogueEngUnitsConfiguration::PRECISION_FIELD_LENGTH         = 2;

    
    IMPLEMENT_DYNCREATE(AnalogueEngUnitsConfiguration, CPropertyPage)

    AnalogueEngUnitsConfiguration::AnalogueEngUnitsConfiguration() 
    : m_currentEntity(NULL), m_parent(NULL), CPropertyPage(AnalogueEngUnitsConfiguration::IDD, IDS_PROPPAGE_ANALOGUE_ENGUNITS)
    {
	    //{{AFX_DATA_INIT(AnalogueEngUnitsConfiguration)
	    m_isEngUnitsChecked = FALSE;
	    m_engHighLimit = _T("");
	    m_engLowLimit = _T("");
	    m_precision = _T("");
	    m_rawHighLimit = _T("");
	    m_rawLowLimit = _T("");
	    m_units = _T("");
	    m_updateThreshold = _T("");
	    m_updateTimeLimit = _T("");
	    m_zeroThreshold = _T("");
	//}}AFX_DATA_INIT
    }


    AnalogueEngUnitsConfiguration::~AnalogueEngUnitsConfiguration()
    {
    }


    void AnalogueEngUnitsConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AnalogueEngUnitsConfiguration)
	DDX_Control(pDX, IDC_UPDATE_TIME_BITMAP, m_bitmapUpdateTime);
	DDX_Control(pDX, IDC_UPDATE_THRESH_BITMAP, m_bitmapUpdateThresh);
	DDX_Control(pDX, IDC_RAWLOW_BITMAP, m_bitmapRawLow);
	DDX_Control(pDX, IDC_RAWHIGH_BITMAP, m_bitmapRawHigh);
	    DDX_Control(pDX, IDC_ZEROVAL_BITMAP, m_bitmapZeroValue);
	    DDX_Control(pDX, IDC_UNITS_BITMAP, m_bitmapUnits);
	    DDX_Control(pDX, IDC_ENGLOW_BITMAP, m_bitmapEngLow);
	    DDX_Control(pDX, IDC_ENGHIGH_BITMAP, m_bitmapEngHigh);
	    DDX_Control(pDX, IDC_EDIT_ZERO_THRESH, m_zeroThresholdControl);
	    DDX_Control(pDX, IDC_EDIT_UPDATE_TIME_LIMIT, m_updateTimeLimitControl);
	    DDX_Control(pDX, IDC_EDIT_UPDATE_THRESH, m_updateThresholdControl);
	    DDX_Control(pDX, IDC_EDIT_UNITS, m_unitsControl);
	    DDX_Control(pDX, IDC_EDIT_RAW_LOW_LIMIT, m_rawLowLimitControl);
	    DDX_Control(pDX, IDC_EDIT_RAW_HIGH_LIMIT, m_rawHighLimitControl);
	    DDX_Control(pDX, IDC_EDIT_PRECISION, m_precisionControl);
	    DDX_Control(pDX, IDC_EDIT_ENG_LOW_LIMIT, m_engLowLimitControl);
	    DDX_Control(pDX, IDC_EDIT_ENG_HIGH_LIMIT, m_engHighLimitControl);
	    DDX_Check(pDX, IDC_CHECK_ANALOGUE_ENG_UNITS, m_isEngUnitsChecked);
	    DDX_Text(pDX, IDC_EDIT_ENG_HIGH_LIMIT, m_engHighLimit);
	    DDX_Text(pDX, IDC_EDIT_ENG_LOW_LIMIT, m_engLowLimit);
	    DDX_Text(pDX, IDC_EDIT_PRECISION, m_precision);
	    DDX_Text(pDX, IDC_EDIT_RAW_HIGH_LIMIT, m_rawHighLimit);
	    DDX_Text(pDX, IDC_EDIT_RAW_LOW_LIMIT, m_rawLowLimit);
	    DDX_Text(pDX, IDC_EDIT_UNITS, m_units);
	    DDX_Text(pDX, IDC_EDIT_UPDATE_THRESH, m_updateThreshold);
	    DDX_Text(pDX, IDC_EDIT_UPDATE_TIME_LIMIT, m_updateTimeLimit);
	    DDX_Text(pDX, IDC_EDIT_ZERO_THRESH, m_zeroThreshold);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AnalogueEngUnitsConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(AnalogueEngUnitsConfiguration)
	ON_BN_CLICKED(IDC_CHECK_ANALOGUE_ENG_UNITS, onCheckAnalogueEngUnits)
	ON_EN_KILLFOCUS(IDC_EDIT_ENG_HIGH_LIMIT, onKillfocusEditEngHighLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_ENG_LOW_LIMIT, onKillfocusEditEngLowLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_PRECISION, onKillfocusEditPrecision)
	ON_EN_KILLFOCUS(IDC_EDIT_RAW_HIGH_LIMIT, onKillfocusEditRawHighLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_RAW_LOW_LIMIT, onKillfocusEditRawLowLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_UNITS, onKillfocusEditUnits)
	ON_EN_KILLFOCUS(IDC_EDIT_UPDATE_THRESH, onKillfocusEditUpdateThresh)
	ON_EN_KILLFOCUS(IDC_EDIT_UPDATE_TIME_LIMIT, onKillfocusEditUpdateTimeLimit)
	ON_EN_KILLFOCUS(IDC_EDIT_ZERO_THRESH, onKillfocusEditZeroThresh)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AnalogueEngUnitsConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                     ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void AnalogueEngUnitsConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");

        parametersToIgnore.insert(ENGUNITS_ENABLED);
        parametersToIgnore.insert(UNITS);
        parametersToIgnore.insert(RAW_HIGH_LIMIT);
        parametersToIgnore.insert(RAW_LOW_LIMIT);
        parametersToIgnore.insert(ENG_HIGH_LIMIT);
        parametersToIgnore.insert(ENG_LOW_LIMIT);
        parametersToIgnore.insert(UPDATE_THRESHOLD);
        parametersToIgnore.insert(UPDATE_TIME_LIMIT);
        parametersToIgnore.insert(ZERO_VAL_THRESHOLD);
        parametersToIgnore.insert(PRECISION);
/**/
        FUNCTION_EXIT;
    }


    void AnalogueEngUnitsConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");

        if(m_currentEntity->getParameterValue(ENGUNITS_ENABLED) == ScadaDetailView::TA_TRUE)
        {
            // Check all parameters that are mandatory when the engineering units check-box is ticked
            checkMandatory(ENG_LOW_LIMIT,IDS_LABEL_ENG_LOW_LIMIT,params);
            checkMandatory(ENG_HIGH_LIMIT,IDS_LABEL_ENG_HIGH_LIMIT,params);
            checkMandatory(RAW_HIGH_LIMIT,IDS_LABEL_RAW_HIGH_LIMIT,params);
            checkMandatory(RAW_LOW_LIMIT,IDS_LABEL_RAW_LOW_LIMIT,params);
            checkMandatory(ZERO_VAL_THRESHOLD,IDS_LABEL_ZERO_VALUE,params);
            checkMandatory(UNITS,IDS_LABEL_UNITS,params);
            checkMandatory(UPDATE_THRESHOLD,IDS_LABEL_UPDATE_THRESHOLD,params);
            checkMandatory(UPDATE_TIME_LIMIT,IDS_LABEL_UPDATE_TIME_LIMIT,params);
        }
        
        FUNCTION_EXIT;
    }

    
    void AnalogueEngUnitsConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");

        FieldValidityChecker checker;

        checkValidity(ENG_LOW_LIMIT,IDS_LABEL_ENG_LOW_LIMIT,checker,params);
        checkValidity(ENG_HIGH_LIMIT,IDS_LABEL_ENG_HIGH_LIMIT,checker,params);
        checkValidity(RAW_HIGH_LIMIT,IDS_LABEL_RAW_HIGH_LIMIT,checker,params);
        checkValidity(RAW_LOW_LIMIT,IDS_LABEL_RAW_LOW_LIMIT,checker,params);

        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // AnalogueEngUnitsConfiguration message handlers

    BOOL AnalogueEngUnitsConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();

	    m_zeroThresholdControl.SetLimitText(ZERO_VALUE_FIELD_LENGTH);
        m_updateTimeLimitControl.SetLimitText(UPDATE_TIME_LIMIT_FIELD_LENGTH);
        m_updateThresholdControl.SetLimitText(UPDATE_THRESHOLD_FIELD_LENGTH);
        m_unitsControl.SetLimitText(UNITS_FIELD_LENGTH);
        m_rawLowLimitControl.SetLimitText(RAW_LOW_FIELD_LENGTH);
        m_rawHighLimitControl.SetLimitText(RAW_HIGH_FIELD_LENGTH);
        m_precisionControl.SetLimitText(PRECISION_FIELD_LENGTH);
        m_engLowLimitControl.SetLimitText(ENG_LOW_FIELD_LENGTH);
        m_engHighLimitControl.SetLimitText(ENG_HIGH_FIELD_LENGTH);

        loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void AnalogueEngUnitsConfiguration::onCheckAnalogueEngUnits() 
    {
        FUNCTION_ENTRY("onCheckAnalogueEngUnits");

        TA_ASSERT(m_currentEntity != NULL, "Cannot populate with data as the entity pointer is NULL");

        UpdateData(TRUE);

        if (m_isEngUnitsChecked)
        {
            m_currentEntity->setParameterValue(ScadaDetailView::TA_TRUE,ENGUNITS_ENABLED);
        }
        else
        {
            m_currentEntity->setParameterValue(ScadaDetailView::TA_FALSE,ENGUNITS_ENABLED);
        }
	    
        refresh();
        
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
        
        FUNCTION_EXIT;
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditEngHighLimit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_engHighLimit),ENG_HIGH_LIMIT);

		// update the OverScaleHi alarm limit
		m_currentEntity->setParameterValue(std::string(m_engHighLimit), TA_Base_App::AnalogueAlarmConfiguration::OH_ALARM_LIMIT);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditEngLowLimit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_engLowLimit),ENG_LOW_LIMIT);

		// update the OverScaleLo alarm limit
		m_currentEntity->setParameterValue(std::string(m_engLowLimit), TA_Base_App::AnalogueAlarmConfiguration::OL_ALARM_LIMIT);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditPrecision() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_precision),PRECISION);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditRawHighLimit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_rawHighLimit),RAW_HIGH_LIMIT);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditRawLowLimit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_rawLowLimit),RAW_LOW_LIMIT);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditUnits() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_units),UNITS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditUpdateThresh() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_updateThreshold),UPDATE_THRESHOLD);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditUpdateTimeLimit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_updateTimeLimit),UPDATE_TIME_LIMIT);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void AnalogueEngUnitsConfiguration::onKillfocusEditZeroThresh() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_zeroThreshold),ZERO_VAL_THRESHOLD);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }

    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void AnalogueEngUnitsConfiguration::loadEntityIntoPage()
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
            std::string value = m_currentEntity->getParameterValue(ENGUNITS_ENABLED);
            m_isEngUnitsChecked = (0 == value.compare(ScadaDetailView::TA_TRUE));
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine if the engineering units are enabled or not");
        }

        try
        {
	        m_engLowLimit = m_currentEntity->getParameterValue(ENG_LOW_LIMIT).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering low limit.");
        }

        try
        {
	        m_engHighLimit = m_currentEntity->getParameterValue(ENG_HIGH_LIMIT).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering high limit.");
        }

        try
        {
	        m_rawLowLimit = m_currentEntity->getParameterValue(RAW_LOW_LIMIT).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the raw low limit.");
        }

        try
        {
	        m_rawHighLimit = m_currentEntity->getParameterValue(RAW_HIGH_LIMIT).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the raw high limit.");
        }

        try
        {
	        m_units = m_currentEntity->getParameterValue(UNITS).c_str();
			//TD17818 marc_bugfix
			preProcessUnitLabel(m_units);
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering units.");
        }

        try
        {
	        m_updateThreshold = m_currentEntity->getParameterValue(UPDATE_THRESHOLD).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering units.");
        }

        try
        {
	        m_updateTimeLimit = m_currentEntity->getParameterValue(UPDATE_TIME_LIMIT).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering units.");
        }
        
        try
        {
	        m_zeroThreshold = m_currentEntity->getParameterValue(ZERO_VAL_THRESHOLD).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering units.");
        }

        try
        {
	        m_precision = m_currentEntity->getParameterValue(PRECISION).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the engineering units.");
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


    void AnalogueEngUnitsConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        // If checked then enable
        GetDlgItem(IDC_EDIT_ENG_HIGH_LIMIT)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_ENG_LOW_LIMIT)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_RAW_HIGH_LIMIT)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_RAW_LOW_LIMIT)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_UNITS)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_UPDATE_THRESH)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_UPDATE_TIME_LIMIT)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_ZERO_THRESH)->EnableWindow(m_isEngUnitsChecked);
        GetDlgItem(IDC_EDIT_PRECISION)->EnableWindow(m_isEngUnitsChecked);

        UpdateData(true);

        FUNCTION_EXIT;
    }


    void AnalogueEngUnitsConfiguration::checkMandatory(const std::string& parameterName,
                                                       int displayNameStringId,
                                                       std::vector<std::string>& params )
    {
        if (m_currentEntity->getParameterValue(parameterName).empty())
        {
            
			//set the Default value if the value for the mandatory parameter is not entered.
			std::string entityName(m_currentEntity->getName());

			TA_Base_Core::IEntityData* iEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(entityName);
            TA_Base_Core::EntityData* entityData = dynamic_cast <TA_Base_Core::EntityData*> (iEntityData);	
			TA_Base_Core::IData& data = entityData->getHelper()->getConfigParameters();
			std::string parameterDefaultValue("");
			for (unsigned long i = 0;i < data.getNumRows();i++)
			{
					
				if( 0 == data.getStringData(i,entityData->getHelper()->PARAMETERNAME).compare(parameterName) )
				{
				 parameterDefaultValue = data.getStringData(i,entityData->getHelper()->PARAMETERDEFAULTVALUE);
				 m_currentEntity->setParameterValue(parameterDefaultValue, parameterName);
				 break;
				}
			}	
			
			//if the default value for the mandator parameter is not set in the database.
			if (m_currentEntity->getParameterValue(parameterName).empty())
			{

				CString dialogTitle;
				dialogTitle.LoadString(IDS_PROPPAGE_ANALOGUE_ENGUNITS);

				CString labelTitle;
				labelTitle.LoadString(displayNameStringId);
				
				std::string parameter(dialogTitle);
				parameter += " - ";
				parameter += labelTitle;

				params.push_back(parameter);
			}
        }
    }


    void AnalogueEngUnitsConfiguration::checkValidity(const std::string& parameterName,
                                                      int displayNameStringId,
                                                      const FieldValidityChecker& checker,
                                                      std::vector<std::string>& params )
    {
        if( !checker.isValidRealNumber(m_currentEntity->getParameterValue(parameterName)) )
        {
            CString dialogTitle;
            dialogTitle.LoadString(IDS_PROPPAGE_ANALOGUE_ENGUNITS);

            CString fieldTitle;
            fieldTitle.LoadString(displayNameStringId);
            
            std::string parameter(dialogTitle);
            parameter += " - ";
            parameter += fieldTitle;

            params.push_back(parameter);
        }
    }




    void AnalogueEngUnitsConfiguration::preProcessUnitLabel(CString& parameterName)
    {
		const CString szCelsius = "°C";
		const CString szFahrenheit = "°F";
		const CString szKelvin = "°K";


		if (parameterName.IsEmpty())
			return;

		if (parameterName == "?C")
		{
			parameterName = szCelsius;
		}
		else if (parameterName == "?F")
		{
			parameterName = szFahrenheit;
		}
		else if (parameterName == "?K")
		{
			parameterName = szKelvin;
		}

    }



}

