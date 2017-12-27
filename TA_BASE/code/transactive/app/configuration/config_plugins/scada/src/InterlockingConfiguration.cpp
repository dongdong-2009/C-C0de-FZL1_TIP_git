/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/InterlockingConfiguration.cpp $
  * @author:  
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration for interlocking.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"
#include "app/configuration/config_plugins/scada/src/InterlockingConfiguration.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"

//#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DllHandler.h"
#include "bus/interlocking/interlockingConfig/src/SingletonHelper.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

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
    // The title of the dialog that allows the interlocking rules to be configured
    const CString InterlockingConfiguration::INTERLOCKING_DIALOG_TITLE = "Interlocking Rules";

    IMPLEMENT_DYNCREATE(InterlockingConfiguration, CPropertyPage)

    InterlockingConfiguration::InterlockingConfiguration()
    : m_currentEntity(NULL), m_parent(NULL), CPropertyPage(InterlockingConfiguration::IDD, IDS_PROPPAGE_INTERLOCKING)
    {
	    //{{AFX_DATA_INIT(InterlockingConfiguration)
        
	    //}}AFX_DATA_INIT
        setUpSingletons();
    }


    InterlockingConfiguration::~InterlockingConfiguration()
    {
    }


    void InterlockingConfiguration::DoDataExchange(CDataExchange* pDX)
    {
	    CPropertyPage::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(InterlockingConfiguration)
        DDX_Control(pDX, IDC_RULELIST, m_ruleList);
        DDX_Control(pDX, IDC_EDITRULE, m_editRuleButton);
        DDX_Control(pDX, IDC_DELETERULE, m_deleteRuleButton);
        DDX_Control(pDX, IDC_NEWRULE, m_newRuleButton);
        DDX_Control(pDX, IDC_VIEWRULE, m_viewRuleButton);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(InterlockingConfiguration, CPropertyPage)
	//{{AFX_MSG_MAP(InterlockingConfiguration)
    ON_BN_CLICKED(IDC_EDITRULE, OnEditRule)
	ON_BN_CLICKED(IDC_DELETERULE, OnDeleteRule)
	ON_BN_CLICKED(IDC_NEWRULE, OnNewRule)
	ON_BN_CLICKED(IDC_VIEWRULE, OnView)
	ON_NOTIFY(NM_CLICK, IDC_RULELIST, OnClickRuleList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void InterlockingConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                                                 ScadaDatabaseAccessor& databaseAccessor)
    {
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;    

        FUNCTION_EXIT;
    }

    
    void InterlockingConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
    {
        FUNCTION_ENTRY("getUsedParameters");
        FUNCTION_EXIT;
    }


    void InterlockingConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getMandatoryParametersNotEntered");
        FUNCTION_EXIT;
    }

    
    void InterlockingConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
    {
        FUNCTION_ENTRY("getInvalidEnteredParameters");
        FUNCTION_EXIT;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // InterlockingConfiguration message handlers


    BOOL InterlockingConfiguration::OnInitDialog() 
    {
        FUNCTION_ENTRY("OnInitDialog");

	    CPropertyPage::OnInitDialog();
        m_ruleList.init();
        // load interlocking rules for this sign entity

		// TD #11229
		// the entity key cannot be retrieved until the data has been written to the database, so
		// only attempt to load the existing interlocking rules if this isn't a new data point.
		if(m_currentEntity->isNew())
		{
			// also disable the buttons for the interlocking rules until the data has been saved
			m_newRuleButton.EnableWindow(true);
			enableEditDelViewButtons(false);
		}
		else
		{			
			m_newRuleButton.EnableWindow(true);
			enableEditDelViewButtons(false);

			TA_Base_Core::DataPointEntityData* entityData = dynamic_cast<TA_Base_Core::DataPointEntityData*> ( TA_Base_Core::EntityAccessFactory::getInstance().getEntity( m_currentEntity->getKey() ) );

			// the data point type will be the actual value type (eg Boolean) concatenated with the entity type (DataPoint)
			std::string dataPointType = entityData->getDataPointType() + m_currentEntity->getType();

			m_ruleList.loadRules( m_currentEntity->getKey(), dataPointType, m_currentEntity->getName().c_str() );
		}

        FUNCTION_EXIT;
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void InterlockingConfiguration::OnDeleteRule() 
    {
		// TD 11932 Display a message box to confirm the delete action
 	    CString actionName;
        actionName.LoadString(IDS_DELETE);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT response = userMsg.showMsgBox(IDS_UW_010003);
        if (response == IDNO)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the item delete");
            return;
        }

		m_ruleList.deleteRule();

		if (m_ruleList.GetItemCount() == 0) // TD 11483 there is no item in the list as all rules been deleted
		{
			// disable the edit, delete & view buttons
			enableEditDelViewButtons(false);
		}
    }

    void InterlockingConfiguration::OnEditRule() 
    {
	    m_ruleList.editRule();
    }

    void InterlockingConfiguration::OnNewRule() 
    {
	    m_ruleList.newRule();
    }

    void InterlockingConfiguration::OnView() 
    {
	    m_ruleList.viewRule();
    }
        
    void InterlockingConfiguration::refresh()
    {
        FUNCTION_ENTRY("refresh");

        UpdateData(true);

        FUNCTION_EXIT;
    }
	
	void InterlockingConfiguration::OnClickRuleList(NMHDR* pNMHDR, LRESULT* pResult) 
	{
		bool enable = false; // TD 11483 enable or disable the edit, delete & view buttons

		// TD 11483 enable the buttons only when one item is selected from list
		if (m_ruleList.GetItemCount() > 0) 
		{
			POSITION pos = m_ruleList.GetFirstSelectedItemPosition( );
			int selectedIndex = m_ruleList.GetNextSelectedItem(pos);

			if ( selectedIndex >= 0 && selectedIndex < m_ruleList.GetItemCount() )
			{
				enable = true;
			}
		}
		enableEditDelViewButtons(enable);

		*pResult = 0;
	}
	
	void InterlockingConfiguration::enableEditDelViewButtons(bool enable)
	{
		// disable or enable the edit, delete & view buttons
		m_editRuleButton.EnableWindow(enable);
		m_deleteRuleButton.EnableWindow(enable);
		m_viewRuleButton.EnableWindow(enable);
	}

    void InterlockingConfiguration::setUpSingletons()
    {

        // Construct the DLL handler that we can pass when initialise the singletons in the DLLs.
        TA_Base_Core::DllHandler dllHandler;

        // Now initialise all the singletons inside the DLLs
        TA_Base_Core::SingletonHelper::initialiseUtilities( dllHandler );
    }
}

