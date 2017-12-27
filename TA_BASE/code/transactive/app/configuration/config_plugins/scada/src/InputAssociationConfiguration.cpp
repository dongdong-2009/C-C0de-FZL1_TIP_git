/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/InputAssociationConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for derived datapoint input associations
  */

#pragma warning(disable:4786)
#pragma warning(disable:4284)

#include <set>

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/InputAssociationConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/ConfigConfiguredInputAssociation.h"

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


    // The names of the entity parameters we will use
    IMPLEMENT_DYNCREATE(InputAssociationConfiguration, CPropertyPage)


    InputAssociationConfiguration::InputAssociationConfiguration()
     : m_currentEntity(NULL),
       m_databaseAccessor(NULL),
       m_parent(NULL), 
       CPropertyPage(InputAssociationConfiguration::IDD,IDS_PROPPAGE_INPUT)
    {
	    //{{AFX_DATA_INIT(InputAssociationConfiguration)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    InputAssociationConfiguration::~InputAssociationConfiguration()
    {
    }


    void InputAssociationConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(InputAssociationConfiguration)
	    DDX_Control(pDX, IDC_INPUT_LIST, m_inputList);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(InputAssociationConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(InputAssociationConfiguration)
	ON_BN_CLICKED(IDC_INPUT_BUTTON_ADD, onButtonAdd)
	ON_BN_CLICKED(IDC_INPUT_BUTTON_REMOVE, onButtonRemove)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    
    
    void InputAssociationConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                     ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;
        m_databaseAccessor = &databaseAccessor;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void InputAssociationConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");
        FUNCTION_EXIT;
    }


    void InputAssociationConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");
        FUNCTION_EXIT;
    }


    void InputAssociationConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");

        TA_Base_Core::IConfigDerivedDataPoint& dp = m_databaseAccessor->getDerivedDataPointItem( m_currentEntity->getUniqueIdentifier() );

        // We must check that every input association has a different calculation order.
        typedef TA_Base_Core::IConfigDerivedDataPoint::InputAssociationMap InputAssociationMap;
        const InputAssociationMap& inputs = dp.getInputAssociations();

        // First generate a list of all calculation orders we should have. Each input should have a different 
        // number counting up from 0
        int numberOfInputs = inputs.size();
        std::set<int> calculationOrders;
        for(int i =0; i < numberOfInputs; ++i)
        {
            calculationOrders.insert(i);
        }

        // Now step through all inputs to see their calculation order
        for(InputAssociationMap::const_iterator input = inputs.begin(); input != inputs.end(); ++input)
        {
            std::set<int>::iterator matching = calculationOrders.find( input->second->getInputCalculationOrder() );
            // If the calculation order is no longer in our set then another input has the same calculation
            // order.  This is an error
            if (matching == calculationOrders.end())
            {
                CString parameter;
                parameter.LoadString(IDS_PROPPAGE_INPUT);
                parameter += " - ";
                parameter += InputDataPointListCtrl::CALC_ORDER_COLUMN_NAME;

                params.push_back( std::string(parameter) );
                
                FUNCTION_EXIT;
                return;
            }
            calculationOrders.erase( matching );
        }

        // If we get to here and there are still items in our calculation order list then once again
        // the values are invalid. One of them is not set properly.
        if (!calculationOrders.empty())
        {
            CString parameter;
            parameter.LoadString(IDS_PROPPAGE_INPUT);
            parameter += " - ";
            parameter += InputDataPointListCtrl::CALC_ORDER_COLUMN_NAME;

            params.push_back( std::string(parameter) );
        }
        
        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // InputAssociationConfiguration message handlers

    BOOL InputAssociationConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();
	    
        loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void InputAssociationConfiguration::onButtonAdd() 
    {
        m_inputList.addNewItem();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void InputAssociationConfiguration::onButtonRemove() 
    {
	    m_inputList.removeSelectedItem();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void InputAssociationConfiguration::loadEntityIntoPage()
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
            m_inputList.populateControl( dp );
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve derived data point input association item details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve derived data point input association item details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        // First GetParent() gives us the property sheet. The next one gives us the view
        m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }

        FUNCTION_EXIT;
    }

}
