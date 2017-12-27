/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/MeteredConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for metered data points.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/MeteredConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"
#include "app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.h"
#include "app/configuration/config_plugins/scada/src/AddAssociationDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/FieldValidityChecker.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"

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
    const std::string MeteredConfiguration::INPUT_DEPENDENCY        = "InputDependency";
    const std::string MeteredConfiguration::ROLLOVER_CONSTANT       = "RolloverConstant";
    const std::string MeteredConfiguration::CORRECTION_VALUE        = "CorrectionValue";
    const std::string MeteredConfiguration::METER_FACTOR            = "MeterFactor";

    const int MeteredConfiguration::ROLLOVER_FIELD_LENGTH         = 10;
    const int MeteredConfiguration::CORRECTION_FIELD_LENGTH       = 10;
    const int MeteredConfiguration::METER_FACTOR_FIELD_LENGTH     = 9;

    const CString MeteredConfiguration::CHANGE_INPUT_DIALOG_TITLE = "Change Input Dependency";

    IMPLEMENT_DYNCREATE(MeteredConfiguration, CPropertyPage)


    MeteredConfiguration::MeteredConfiguration()
      : m_currentEntity(NULL), m_parent(NULL), CPropertyPage(MeteredConfiguration::IDD, IDS_PROPPAGE_METERED)
    {
	    //{{AFX_DATA_INIT(MeteredConfiguration)
	    m_correctionValue = _T("");
	    m_inputDependency = _T("");
	    m_meterFactor = _T("");
	    m_rolloverConstant = _T("");
	//}}AFX_DATA_INIT
    }


    MeteredConfiguration::~MeteredConfiguration()
    {
    }


    void MeteredConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(MeteredConfiguration)
	DDX_Control(pDX, IDC_EDIT_ROLLOVER_CONSTANT, m_rolloverConstantControl);
	DDX_Control(pDX, IDC_EDIT_METER_FACTOR, m_meterFactorControl);
	DDX_Control(pDX, IDC_EDIT_CORRECTION_VALUE, m_correctionValueControl);
	    DDX_Control(pDX, IDC_ROLLOVER_CONSTANT_BITMAP, m_bitmapRolloverConstant);
	    DDX_Control(pDX, IDC_METER_FACTOR_BITMAP, m_bitmapMeterFactor);
	    DDX_Control(pDX, IDC_INPUT_DEPENDENCY_BITMAP, m_bitmapInputDependency);
	    DDX_Text(pDX, IDC_EDIT_CORRECTION_VALUE, m_correctionValue);
	    DDX_Text(pDX, IDC_EDIT_INPUT_DEPENDENCY, m_inputDependency);
	    DDX_Text(pDX, IDC_EDIT_METER_FACTOR, m_meterFactor);
	    DDX_Text(pDX, IDC_EDIT_ROLLOVER_CONSTANT, m_rolloverConstant);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(MeteredConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(MeteredConfiguration)
	ON_BN_CLICKED(IDC_CHANGE_INPUT_DEPENDENCY, onChangeInputDependency)
	ON_EN_KILLFOCUS(IDC_EDIT_CORRECTION_VALUE, onKillfocusEditCorrectionValue)
	ON_EN_KILLFOCUS(IDC_EDIT_METER_FACTOR, onKillfocusEditMeterFactor)
	ON_EN_KILLFOCUS(IDC_EDIT_ROLLOVER_CONSTANT, onKillfocusEditRolloverConstant)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void MeteredConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                            ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void MeteredConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");

        parametersToIgnore.insert(INPUT_DEPENDENCY);
        parametersToIgnore.insert(ROLLOVER_CONSTANT);
        parametersToIgnore.insert(CORRECTION_VALUE);
        parametersToIgnore.insert(METER_FACTOR);
/**/
        FUNCTION_EXIT;
    }


    void MeteredConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");

        checkMandatory(INPUT_DEPENDENCY,  IDS_LABEL_INPUT_DEPENDENCY,  params);
        checkMandatory(ROLLOVER_CONSTANT, IDS_LABEL_ROLLOVER_CONSTANT, params);
        checkMandatory(METER_FACTOR,      IDS_LABEL_METER_FACTOR,      params);

        FUNCTION_EXIT;
    }


    void MeteredConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");

        FieldValidityChecker checker;

        checkValidity(CORRECTION_VALUE,        IDS_LABEL_CORRECTION_VALUE,  checker, params);
        checkValidity(METER_FACTOR,            IDS_LABEL_METER_FACTOR,      checker, params);

        FUNCTION_EXIT;
    }

    /////////////////////////////////////////////////////////////////////////////
    // MeteredConfiguration message handlers

    BOOL MeteredConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();
	    
        m_rolloverConstantControl.SetLimitText(ROLLOVER_FIELD_LENGTH);
        m_correctionValueControl.SetLimitText(CORRECTION_FIELD_LENGTH);
        m_meterFactorControl.SetLimitText(METER_FACTOR_FIELD_LENGTH);

        loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void MeteredConfiguration::onChangeInputDependency() 
    {
        FUNCTION_ENTRY("onChangeInputDependency");

        std::vector<unsigned long> dataPointsToIgnore;
        SelectAssociationEntityFactory factory(dataPointsToIgnore, false, false);

	    AddAssociationDialog dlg(0, factory, CHANGE_INPUT_DIALOG_TITLE, m_inputDependency); // pass in that there are zero states
        if (dlg.DoModal() == IDCANCEL)
        {
            FUNCTION_EXIT;
            return;
        }

        m_inputDependency = dlg.getSelectedName();

        CString newValue;
        newValue.Format("%lu",dlg.getSelectedKey() );
        m_currentEntity->setParameterValue(std::string(newValue), INPUT_DEPENDENCY);
	    
        UpdateData(FALSE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void MeteredConfiguration::onKillfocusEditCorrectionValue() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_correctionValue),CORRECTION_VALUE);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void MeteredConfiguration::onKillfocusEditMeterFactor() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_meterFactor),METER_FACTOR);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void MeteredConfiguration::onKillfocusEditRolloverConstant() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_rolloverConstant),ROLLOVER_CONSTANT);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void MeteredConfiguration::loadEntityIntoPage()
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

        TA_Base_Core::IEntityData* data = NULL;
        try
        {
            // This returns the input dependency key. We then need to retrieve the name of the input dependency
            std::string inputDependencyKeyString = m_currentEntity->getParameterValue(INPUT_DEPENDENCY).c_str();

            // Extract the unsigned long out of the string
            unsigned long inputDependencyKey;
            std::istringstream temp;
            temp.str(inputDependencyKeyString);
            temp >> inputDependencyKey;
	        
            data = TA_Base_Core::EntityAccessFactory::getInstance().getEntity(inputDependencyKey);
            m_inputDependency = data->getName().c_str();

            delete data;
            data = NULL;
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the input dependency");

            if (data != NULL)
            {
                delete data;
                data = NULL;
            }
        }

        try
        {
	        m_rolloverConstant = m_currentEntity->getParameterValue(ROLLOVER_CONSTANT).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the rollover constant");
        }

        try
        {
	        m_meterFactor = m_currentEntity->getParameterValue(METER_FACTOR).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the meter factor");
        }

        try
        {
	        m_correctionValue = m_currentEntity->getParameterValue(CORRECTION_VALUE).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the correction value");
        }

        UpdateData(FALSE);

        // First GetParent() gives us the property sheet. The next one gives us the view
        m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }


    void MeteredConfiguration::checkValidity(const std::string& parameterName,
                                             int displayNameStringId,
                                             const FieldValidityChecker& checker,
                                             std::vector<std::string>& params )
    {
        if( !checker.isValidRealNumber(m_currentEntity->getParameterValue(parameterName)) )
        {
            CString dialogTitle;
            dialogTitle.LoadString(IDS_PROPPAGE_METERED);

            CString fieldTitle;
            fieldTitle.LoadString(displayNameStringId);
            
            std::string parameter(dialogTitle);
            parameter += " - ";
            parameter += fieldTitle;

            params.push_back(parameter);
        }
    }


    void MeteredConfiguration::checkMandatory(const std::string& parameterName,
                                              int displayNameStringId,
                                              std::vector<std::string>& params )
    {
        if (m_currentEntity->getParameterValue(parameterName).empty())
        {
            CString dialogTitle;
            dialogTitle.LoadString(IDS_PROPPAGE_METERED);

            CString labelTitle;
            labelTitle.LoadString(displayNameStringId);

            std::string parameter(dialogTitle);
            parameter += " - ";
            parameter += labelTitle;

            params.push_back(parameter);
        }
    }
}
