/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/OutputAssociationConfiguration.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for derived datapoint output associations
  */
#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/OutputAssociationConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/AddAssociationDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

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

/////////////////////////////////////////////////////////////////////////////
// OutputAssociationConfiguration property page

namespace TA_Base_App
{
    IMPLEMENT_DYNCREATE(OutputAssociationConfiguration, CPropertyPage)


    OutputAssociationConfiguration::OutputAssociationConfiguration()
        : m_currentEntity(NULL),
          m_databaseAccessor(NULL),
          m_parent(NULL), 
          CPropertyPage(OutputAssociationConfiguration::IDD,IDS_PROPPAGE_OUTPUT)
    {
	    //{{AFX_DATA_INIT(OutputAssociationConfiguration)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    OutputAssociationConfiguration::~OutputAssociationConfiguration()
    {
    }


    void OutputAssociationConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(OutputAssociationConfiguration)
	    DDX_Control(pDX, IDC_OUTPUT_LIST, m_outputList);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(OutputAssociationConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(OutputAssociationConfiguration)
	ON_BN_CLICKED(IDC_OUTPUT_BUTTON_ADD, onButtonAdd)
	ON_BN_CLICKED(IDC_OUTPUT_BUTTON_REMOVE, onButtonRemove)
	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void OutputAssociationConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                      ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;
        m_databaseAccessor = &databaseAccessor;

        loadEntityIntoPage();

        FUNCTION_EXIT;
    }

    
    void OutputAssociationConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");
        FUNCTION_EXIT;
    }


    void OutputAssociationConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");
        FUNCTION_EXIT;
    }


    void OutputAssociationConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // OutputAssociationConfiguration message handlers

    BOOL OutputAssociationConfiguration::OnInitDialog() 
    {
	    CPropertyPage::OnInitDialog();
	    
	    loadEntityIntoPage();
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void OutputAssociationConfiguration::OnShowWindow(BOOL bShow, UINT nStatus) 
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
        m_outputList.DeleteAllItems();

        loadEntityIntoPage();	    
    }


    void OutputAssociationConfiguration::onButtonAdd() 
    {
	    m_outputList.addNewItem();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }


    void OutputAssociationConfiguration::onButtonRemove() 
    {
        m_outputList.removeSelectedItem();

        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
    }
 
    
    /////////////////////////////////////////////////////////////////////////////
    // Private helper methods

    void OutputAssociationConfiguration::loadEntityIntoPage()
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
            m_outputList.populateControl( dp );
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve derived data point output association item details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve derived data point output association item details to display");

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
