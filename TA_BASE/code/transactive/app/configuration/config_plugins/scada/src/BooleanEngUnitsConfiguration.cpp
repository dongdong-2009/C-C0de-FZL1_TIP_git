/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/BooleanEngUnitsConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for boolean engineering units.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/BooleanEngUnitsConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

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
    const std::string BooleanEngUnitsConfiguration::TRUE_ENGUNITS    = "FalseLabel";
    const std::string BooleanEngUnitsConfiguration::FALSE_ENGUNITS   = "TrueLabel";

    // The limit in length for the text fields.
    const int BooleanEngUnitsConfiguration::TRUE_FIELD_LENGTH  = 128;
    const int BooleanEngUnitsConfiguration::FALSE_FIELD_LENGTH = 128;


    IMPLEMENT_DYNCREATE(BooleanEngUnitsConfiguration, CPropertyPage)


    BooleanEngUnitsConfiguration::BooleanEngUnitsConfiguration()
    : m_currentEntity(NULL), m_parent(NULL), CPropertyPage(BooleanEngUnitsConfiguration::IDD, IDS_PROPPAGE_BOOLEAN_ENGUNITS)
    {
	    //{{AFX_DATA_INIT(BooleanEngUnitsConfiguration)
	    m_falseEngUnits = _T("");
	    m_trueEngUnits = _T("");
	//}}AFX_DATA_INIT
    }


    BooleanEngUnitsConfiguration::~BooleanEngUnitsConfiguration()
    {
    }


    void BooleanEngUnitsConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(BooleanEngUnitsConfiguration)
	    DDX_Control(pDX, IDC_EDIT_TRUE_ENGUNIT, m_trueEngUnitsControl);
	    DDX_Control(pDX, IDC_EDIT_FALSE_ENGUNIT, m_falseEngUnitsControl);
	    DDX_Text(pDX, IDC_EDIT_FALSE_ENGUNIT, m_falseEngUnits);
	    DDX_Text(pDX, IDC_EDIT_TRUE_ENGUNIT, m_trueEngUnits);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(BooleanEngUnitsConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(BooleanEngUnitsConfiguration)
	ON_EN_KILLFOCUS(IDC_EDIT_FALSE_ENGUNIT, onKillfocusEditFalseEngUnit)
	ON_EN_KILLFOCUS(IDC_EDIT_TRUE_ENGUNIT, onKillfocusEditTrueEngUnit)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void BooleanEngUnitsConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                    ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void BooleanEngUnitsConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");

        parametersToIgnore.insert(TRUE_ENGUNITS);
        parametersToIgnore.insert(FALSE_ENGUNITS);
/**/
        FUNCTION_EXIT;
    }

    
    void BooleanEngUnitsConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");
        FUNCTION_EXIT;
    }

    
    void BooleanEngUnitsConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // BooleanEngUnitsConfiguration message handlers

    BOOL BooleanEngUnitsConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();
	    
        m_trueEngUnitsControl.SetLimitText(TRUE_FIELD_LENGTH);
        m_falseEngUnitsControl.SetLimitText(FALSE_FIELD_LENGTH);

        loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void BooleanEngUnitsConfiguration::onKillfocusEditFalseEngUnit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_falseEngUnits),FALSE_ENGUNITS);
    
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void BooleanEngUnitsConfiguration::onKillfocusEditTrueEngUnit() 
    {
        TA_ASSERT(m_currentEntity !=  NULL, "Current entity is NULL.");

        UpdateData(TRUE);
        m_currentEntity->setParameterValue(std::string(m_trueEngUnits),TRUE_ENGUNITS);

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void BooleanEngUnitsConfiguration::loadEntityIntoPage()
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
	        m_trueEngUnits = m_currentEntity->getParameterValue(TRUE_ENGUNITS).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the true engineering units");
        }

        try
        {
	        m_falseEngUnits = m_currentEntity->getParameterValue(FALSE_ENGUNITS).c_str();
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not determine the false engineering units");
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


    void BooleanEngUnitsConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        // If checked then enable
        GetDlgItem(IDC_EDIT_TRUE_ENGUNIT)->EnableWindow(true);
        GetDlgItem(IDC_EDIT_FALSE_ENGUNIT)->EnableWindow(true);

        UpdateData(true);

        FUNCTION_EXIT;
    }
}
