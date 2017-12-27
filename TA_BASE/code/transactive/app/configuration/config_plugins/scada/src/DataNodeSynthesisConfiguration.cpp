/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/DataNodeSynthesisConfiguration.cpp $
  * @author:  Hoa Vu
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This property page holds the configuration of alarm synthesis for a DataNode
  */

#pragma warning(disable:4284 4786)

#include "app/configuration/config_plugins/scada/src/stdafx.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/SelectAssociationEntityFactory.h"
#include "app/configuration/config_plugins/scada/src/AddAssociationDialog.h"
#include "app/configuration/config_plugins/scada/src/DataNodeSynthesisConfiguration.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
	const std::string HIERARCHY_STRING = "HIERARCHY";
	const std::string LIST_STRING = "LIST";
	const std::string BOTH_STRING = "BOTH";
	const std::string NONE_STRING = "NO";

	// enable  this class (derived from CObject) to be created dynamically
	// by Windows framework at run time.
    IMPLEMENT_DYNCREATE ( DataNodeSynthesisConfiguration, CPropertyPage )


	DataNodeSynthesisConfiguration::DataNodeSynthesisConfiguration()
	:
	m_currentEntity ( NULL ),
	m_databaseAccessor ( NULL ),
	m_parent ( NULL ), 
	m_synthesisType ( HIERARCHY_STRING ),
	m_listOfChildEntities ( "" ),
	CPropertyPage(DataNodeSynthesisConfiguration::IDD)
	{
		//{{AFX_DATA_INIT(DataNodeSynthesisConfiguration)
	//}}AFX_DATA_INIT
	}


	DataNodeSynthesisConfiguration::~DataNodeSynthesisConfiguration()
	{
	}


	void DataNodeSynthesisConfiguration::DoDataExchange(CDataExchange* pDX)
	{
		CPropertyPage::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(DataNodeSynthesisConfiguration)
	DDX_Control(pDX, IDC_LIST1, m_clistBox);
	//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(DataNodeSynthesisConfiguration, CPropertyPage)
		//{{AFX_MSG_MAP(DataNodeSynthesisConfiguration)
	ON_BN_CLICKED(ID_BUTTON_ADD_ENTITY, OnButtonAddEntity)
	ON_BN_CLICKED(ID_BUTTON_REMOVE_ENTITY, OnButtonRemoveEntity)
	ON_BN_CLICKED(IDC_RADIO_HIERARCHY, OnRadioHierarchy)
	ON_BN_CLICKED(IDC_RADIO_LIST, OnRadioList)
	ON_BN_CLICKED(IDC_RADIO_BOTH, OnRadioBoth)
	ON_BN_CLICKED(IDC_RADIO_NONE, OnRadioNone)
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	/////////////////////////////////////////////////////////////////////////////
	// specific implementation
	/////////////////////////////////////////////////////////////////////////////


    void DataNodeSynthesisConfiguration::populatePage(TA_Base_Core::IConfigEntity& currentEntity,
                              ScadaDatabaseAccessor& databaseAccessor)
	{
        FUNCTION_ENTRY("populatePage");

        m_currentEntity = &currentEntity;
        m_databaseAccessor = &databaseAccessor;

        loadEntityIntoPage();

        FUNCTION_EXIT;
	}


	void DataNodeSynthesisConfiguration::getUsedParameters(std::set<std::string>& parametersToIgnore)
	{
		// do nothing at this stage
	}


	void DataNodeSynthesisConfiguration::getMandatoryParametersNotEntered(std::vector<std::string>& params)
	{
		// do nothing at this stage
	}


	void DataNodeSynthesisConfiguration::getInvalidEnteredParameters(std::vector<std::string>& params)
	{
		// do nothing at this stage
	}
	
	
	void DataNodeSynthesisConfiguration::loadEntityIntoPage()
	{
        FUNCTION_ENTRY("loadEntityIntoPage");


        if (m_hWnd == NULL)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Could not load the entity details yet as OnInitDialog has not been called");
            FUNCTION_EXIT;
            return;
        }

        if (m_currentEntity == NULL)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Could not load the entity details yet as no current entity has been set yet");
            FUNCTION_EXIT;
            return;
        }

        try
        {
			if( m_currentEntity->isNew() )
			{
				m_synthesisType = HIERARCHY_STRING;
				m_listOfChildEntities = "";

				// group of radio buttons: hierachy, list, both, none
				// default to IDC_RADIO_HIERARCHY for new entity
				::CheckRadioButton ( m_hWnd, 
									 IDC_RADIO_HIERARCHY,
									 IDC_RADIO_NONE,
									 IDC_RADIO_HIERARCHY );
			}
			else
			{
				TA_Base_Core::IEntityData * dbEntityData = 0;

				dbEntityData = TA_Base_Core::EntityAccessFactory::getInstance().getEntity ( m_currentEntity->getKey() );

				// if able to get the database entity for current entity
				if ( 0 != dbEntityData )
				{
					setupSynthesisType ( dbEntityData );

					setupListOfChildEntities ( dbEntityData );

					//
					// clean up db entity
					//

					delete dbEntityData;
					dbEntityData = 0;
				}
				// else do nothing
			}
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve derived data point input association item details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, TA_Base_Core::DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve derived data point input association item details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

		/*
        // First GetParent() gives us the property sheet. The next one gives us the view
        m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
        if (m_parent != NULL)
        {
            m_parent->refreshButtonAvailability();
        }
		*/

        FUNCTION_EXIT;
 	}


	void DataNodeSynthesisConfiguration::setupSynthesisType ( TA_Base_Core::IEntityData * dbEntityData )
	{
		int checkedRadioButton = IDC_RADIO_HIERARCHY;
		TA_Base_Core::DataNodeEntityData * dnEntityData = 0;

		// cast the db entity to DataNode entity
		dnEntityData = dynamic_cast< TA_Base_Core::DataNodeEntityData *> ( dbEntityData );

		// if successfull
		if ( 0 != dnEntityData )
		{
			//
			// get the configured synthesis type
			//

			m_synthesisType = dnEntityData->getSynthesisType();

			// if synthesis type has been configured
			if ( 0 < m_synthesisType.length() )
			{
				if ( TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_HIERARCHY == m_synthesisType )
				{
					checkedRadioButton = IDC_RADIO_HIERARCHY;
				}
				else if ( TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_LIST == m_synthesisType )
				{
					checkedRadioButton = IDC_RADIO_LIST;
				}
				else if ( TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_BOTH == m_synthesisType )
				{
					checkedRadioButton = IDC_RADIO_BOTH;
				}
				else if ( TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_NONE == m_synthesisType )
				{
					checkedRadioButton = IDC_RADIO_NONE;
				}
				else
				{
					// unknown config type, default to hierarchy
					m_synthesisType = TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_HIERARCHY;
					checkedRadioButton = IDC_RADIO_HIERARCHY;
				}
			}

			// if no synthesis type has been assigned
			else
			{
				// default to hierarchy
				m_synthesisType = TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE_HIERARCHY;
				checkedRadioButton = IDC_RADIO_HIERARCHY;
			}
		}
		// do nothing if dynamic cast failed

		// group of radio buttons: hierachy, list, both, none
		// check the appropriated button accordingly
		::CheckRadioButton ( m_hWnd, 
							 IDC_RADIO_HIERARCHY,
							 IDC_RADIO_NONE,
							 checkedRadioButton );
	}


	void DataNodeSynthesisConfiguration::setupListOfChildEntities ( TA_Base_Core::IEntityData * dbEntityData )
	{
		TA_Base_Core::DataNodeEntityData * dnEntityData = 0;

		// cast the db entity to DataNode entity
		dnEntityData = dynamic_cast< TA_Base_Core::DataNodeEntityData *> ( dbEntityData );

		// if successfull
		if ( 0 != dnEntityData )
		{
			//
			// get the list of configured child entities
			//

			m_listOfChildEntities = dnEntityData->getListOfChildEntities();

			// if got something in the list
			if ( 0 < m_listOfChildEntities.length() )
			{
				std::vector < std::string > namesOfChildEntities;
				namesOfChildEntities.clear();

				parseTheListForEntityName ( m_listOfChildEntities, namesOfChildEntities );

				// for each names in the list
				for ( unsigned long i = 0; i < namesOfChildEntities.size(); i++ )
				{
					// display the selected name on screen
					m_clistBox.AddString ( namesOfChildEntities[i].c_str() );
				}
			}
			// else do nothing
		}
		// else do nothing if dynamic cast failed
	}


	void DataNodeSynthesisConfiguration::parseTheListForEntityName ( const std::string & listOfChildEntities, std::vector < std::string > & namesOfChildEntities )
	{
		// the agreed format of the string representing the list of child entities is:
		// {entity_1_name} + {entity_2_name} + ... + {entity_n_name}

		std::string tempString = listOfChildEntities;
		while ( true )
		{
			// find the first "{"
			int firstOpenCurlyPos = -1;
			firstOpenCurlyPos = tempString.find_first_of ( "{" );

			// if found none
			if ( 0 > firstOpenCurlyPos )
			{
				// exit loop
				break;
			}

			// find the first "}"
			int firstCloseCurlyPos = -1;
			firstCloseCurlyPos = tempString.find_first_of ( "}" );

			// get the characters between the "{" and "}"
			namesOfChildEntities.push_back ( tempString.substr ( firstOpenCurlyPos + 1, firstCloseCurlyPos - ( firstOpenCurlyPos + 1 ) ) );

			// move on to next {} pair
			tempString = tempString.substr ( firstCloseCurlyPos + 1 );
		}
	}


	bool DataNodeSynthesisConfiguration::addEntityNameToListOfChildEntities ( const std::string & selectedName )
	{
		bool matchedName = false;

		std::vector < std::string > namesOfChildEntities;
		namesOfChildEntities.clear();

		// get all names from the current list of child entities
		parseTheListForEntityName ( m_listOfChildEntities, namesOfChildEntities );

		// for each names in the list
		for ( unsigned long i = 0; i < namesOfChildEntities.size(); i++ )
		{
			// if the current entity name matches the specified selected name
			if ( 0 == selectedName.compare ( namesOfChildEntities[i] ) )
			{
				// set the flag to indicate so and exit loop
				matchedName = true;
				break;
			}
		}

		// if there is no match in the current list of child entities
		if ( false == matchedName )
		{
			// save the selected entity to the list of child entities
			if ( 0 < m_listOfChildEntities.length() )
			{
				m_listOfChildEntities += " + ";
			}
			m_listOfChildEntities += "{";
			m_listOfChildEntities += selectedName;
			m_listOfChildEntities += "}";

			return true;
		}

		return false;
	}


	void DataNodeSynthesisConfiguration::removeEntityNameFromListOfChildEntities ( const std::string & selectedName )
	{
		std::vector < std::string > namesOfChildEntities;
		namesOfChildEntities.clear();

		// get all names from the current list of child entities
		parseTheListForEntityName ( m_listOfChildEntities, namesOfChildEntities );

		// for each names in the list
		std::string currentListOfChildEntities ( "" );
		for ( unsigned long i = 0; i < namesOfChildEntities.size(); i++ )
		{
			// if the current entity name does not match the specified selected name
			if ( 0 != selectedName.compare ( namesOfChildEntities[i] ) )
			{
				// save the selected entity to the list of child entities
				if ( 0 < currentListOfChildEntities.length() )
				{
					currentListOfChildEntities += " + ";
				}
				currentListOfChildEntities += "{";
				currentListOfChildEntities += namesOfChildEntities[i];
				currentListOfChildEntities += "}";
			}
			// else do not insert the matched name into the list of child entities
		}

		// update the list of child entities
		m_listOfChildEntities = currentListOfChildEntities;
	}


	void DataNodeSynthesisConfiguration::OnButtonAddEntity() 
	{
		// TODO: Add your control notification handler code here
		
		std::vector < unsigned long > dataPointsToIgnore;
		dataPointsToIgnore.clear();

		TA_Base_App::SelectAssociationEntityFactory factory ( dataPointsToIgnore, false, false );

	    TA_Base_App::AddAssociationDialog addAssociationDialog ( 0,
															factory,
															"Add Association",
															"",
															NULL,
															true );

        if ( addAssociationDialog.DoModal() == IDCANCEL )
        {
            return;
        }

		std::string selectedName = addAssociationDialog.getSelectedName();

		// save the selected entity to the list of child entities
		if ( true == addEntityNameToListOfChildEntities ( selectedName ) )
		{
			// display the selected entity on screen
			m_clistBox.AddString ( selectedName.c_str() );

			// First GetParent() gives us the property sheet. The next one gives us the view
			m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
			if (m_parent != NULL)
			{
				// update parameter of the selected entity
				m_currentEntity->setParameterValue ( m_listOfChildEntities ,TA_Base_Core::DataNodeEntityData::DATANODE_LIST_OF_CHILD_ENTITIES );

				// show the "Save"/"Reset" button
				m_parent->refreshButtonAvailability();
			}
		}
		// else do nothing
	}


	void DataNodeSynthesisConfiguration::OnButtonRemoveEntity() 
	{
		// TODO: Add your control notification handler code here

		// get the index of the currently selected item in the list
		int currentIndex = LB_ERR;
		currentIndex = m_clistBox.GetCurSel();

		// only proceed if we have a valid index
		if ( LB_ERR != currentIndex )
		{
			// get the text associated with the selected index
			CString selectedName;
			m_clistBox.GetText ( currentIndex, selectedName );

			if ( false == selectedName.IsEmpty() )
			{
				// remove the selected name from the list of child entities
				std::string entityName ( selectedName );
				removeEntityNameFromListOfChildEntities ( entityName );

				// remove the string associated with the selected index from the list box
				m_clistBox.DeleteString ( currentIndex );

				// First GetParent() gives us the property sheet. The next one gives us the view
				m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
				if (m_parent != NULL)
				{
					// update parameter of the selected entity
					m_currentEntity->setParameterValue ( m_listOfChildEntities ,TA_Base_Core::DataNodeEntityData::DATANODE_LIST_OF_CHILD_ENTITIES );

					// show the "Save"/"Reset" button
					m_parent->refreshButtonAvailability();
				}
			}
			// else do nothing
		}
		// else do nothing
	}


	void DataNodeSynthesisConfiguration::OnRadioHierarchy() 
	{
		// TODO: Add your control notification handler code here

		m_synthesisType = HIERARCHY_STRING;

		// First GetParent() gives us the property sheet. The next one gives us the view
		m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
		if (m_parent != NULL)
		{
			// update parameter of the selected entity
			m_currentEntity->setParameterValue ( m_synthesisType ,TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE );

			// show the "Save"/"Reset" button
			m_parent->refreshButtonAvailability();
		}
	}


	void DataNodeSynthesisConfiguration::OnRadioList() 
	{
		// TODO: Add your control notification handler code here
		
		m_synthesisType = LIST_STRING;

		// First GetParent() gives us the property sheet. The next one gives us the view
		m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
		if (m_parent != NULL)
		{
			// update parameter of the selected entity
			m_currentEntity->setParameterValue ( m_synthesisType ,TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE );

			// show the "Save"/"Reset" button
			m_parent->refreshButtonAvailability();
		}
	}


	void DataNodeSynthesisConfiguration::OnRadioBoth() 
	{
		// TODO: Add your control notification handler code here
		
		m_synthesisType = BOTH_STRING;

		// First GetParent() gives us the property sheet. The next one gives us the view
		m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
		if (m_parent != NULL)
		{
			// update parameter of the selected entity
			m_currentEntity->setParameterValue ( m_synthesisType ,TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE );

			// show the "Save"/"Reset" button
			m_parent->refreshButtonAvailability();
		}
	}


	void DataNodeSynthesisConfiguration::OnRadioNone() 
	{
		// TODO: Add your control notification handler code here
		
		m_synthesisType = NONE_STRING;

		// First GetParent() gives us the property sheet. The next one gives us the view
		m_parent = dynamic_cast<AbstractDetailView*>(GetParent()->GetParent());
		if (m_parent != NULL)
		{
			// update parameter of the selected entity
			m_currentEntity->setParameterValue ( m_synthesisType ,TA_Base_Core::DataNodeEntityData::DATANODE_SYNTHESIS_TYPE );

			// show the "Save"/"Reset" button
			m_parent->refreshButtonAvailability();
		}
	}


	BOOL DataNodeSynthesisConfiguration::OnInitDialog() 
	{
		CPropertyPage::OnInitDialog();
		
		// TODO: Add extra initialization here

		loadEntityIntoPage();

		return TRUE;  // return TRUE unless you set the focus to a control
					  // EXCEPTION: OCX Property Pages should return FALSE
	}
}

