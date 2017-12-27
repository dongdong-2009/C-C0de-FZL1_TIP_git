/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an entity's details.
  */

#pragma warning(disable:4284 4786)

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"
#include "app/configuration/config_plugins/scada/src/ScadaComponent.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada/src/ScadaTreeView.h"
#include "app/configuration/config_plugins/scada/src/ChangeParentDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#define IMPORT_EXTENSIONS
//#include "bus/mfc_extensions/src/mfc_extensions.h"

#include "bus/generic_gui/src/TransActiveMessage.h"


#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/DataNodeEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"
#include "core/data_access_interface/derived_datapoints/src/IConfigDerivedDataPoint.h"
#include "core/data_access_interface/derived_datapoints/src/DerivedDataPointAccessFactory.h"


#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

//TD18137 - marc_bugfix begin
#include "core/exceptions/src/MathematicalEvaluationException.h"
#include "bus/mathematical_evaluation/src/ExpressionEvaluator.h"
//TD18137 - marc_bugfix end

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

// This message is posted to this view when a watched parameter is changed by the user.
#define WM_PARAMETER_UDPATED (WM_USER+201)

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::Tokeniser;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{

    // The titles for the change dialog box
    const CString ScadaDetailView::CHANGE_LOCATION_DIALOG_TITLE  = "Change Location";
    const CString ScadaDetailView::CHANGE_SUBSYSTEM_DIALOG_TITLE = "Change Subsystem";
    const CString ScadaDetailView::CHANGE_AGENT_DIALOG_TITLE = "Change Agent";

	// The names of fields.
    const std::string ScadaDetailView::ADDRESS_FIELD_NAME = "Address";

    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int ScadaDetailView::NAME_FIELD_LENGTH        = 80;
    const int ScadaDetailView::ADDRESS_FIELD_LENGTH     = 100;
    const int ScadaDetailView::DESCRIPTION_FIELD_LENGTH = 2000;

    // The name of the parameter that can be used to retrieve the type of a datapoint
    const std::string ScadaDetailView::DATAPOINT_TYPE = "DataPointType";
    const std::string ScadaDetailView::ALLOW_WRITE = "AllowWrite";

	//TD18317 parameters for LCC and RCC
    const std::string ScadaDetailView::LAUNCH_CONDITION = "LaunchCondition";
    const std::string ScadaDetailView::RETURN_CONDITION = "ReturnCondition";


    // The different possible SCADA datapoint types
    const std::string ScadaDetailView::ANALOGUE_TYPE         = "Analogue";
    const std::string ScadaDetailView::TEXT_TYPE             = "Text";
    const std::string ScadaDetailView::BOOLEAN_TYPE          = "Boolean";
    const std::string ScadaDetailView::DERIVED_TYPE          = "Derived";
    const std::string ScadaDetailView::METERED_TYPE          = "Metered";
    const std::string ScadaDetailView::SAFETY_OUTPUT_TYPE    = "Safety Output";
    const std::string ScadaDetailView::COMBINED_OUTPUT_TYPE  = "Combined Output";
    const std::string ScadaDetailView::ENUM_TYPE             = "Enum";

    // The strings we must compare against to determine true or false values
    const std::string ScadaDetailView::TA_FALSE = "No";
    const std::string ScadaDetailView::TA_TRUE  = "Yes";
    
    IMPLEMENT_DYNCREATE(ScadaDetailView, CFormView)


    ScadaDetailView::ScadaDetailView() : 
        m_currentEntity(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        m_currentlySelectedPage(NULL),
        AbstractDetailView(ScadaDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(ScadaDetailView)
	//}}AFX_DATA_INIT
    }


    ScadaDetailView::~ScadaDetailView()
    {
    }


    void ScadaDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ScadaDetailView)
	DDX_Control(pDX, IDC_AGENT_BITMAP, m_agentBitmap);
	DDX_Control(pDX, IDC_EDIT_AGENT, m_agent);
	DDX_Control(pDX, IDC_SUBSYSTEM_BITMAP, m_subsystemBitmap);
	DDX_Control(pDX, IDC_ADDRESS_BITMAP, m_addressBitmap);
	DDX_Control(pDX, IDC_DESCRIPTION_BITMAP, m_descriptionBitmap);
	DDX_Control(pDX, IDC_PARENT_BITMAP, m_parentBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_LOCATION_BITMAP, m_locationBitmap);
	DDX_Control(pDX, IDC_EDIT_PARENT, m_parent);
	DDX_Control(pDX, IDC_EDIT_SUBSYSTEM, m_subsystem);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_location);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_address);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ScadaDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ScadaDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonChangeLocation)
	ON_BN_CLICKED(IDC_BUTTON_PARENT, onButtonChangeParent)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDRESS, onKillfocusEditAddress)
	ON_BN_CLICKED(IDC_BUTTON_SUBSYSTEM, onButtonChangeSubsystem)
	ON_BN_CLICKED(IDC_BUTTON_AGENT, onButtonChangeAgent)
	//}}AFX_MSG_MAP
    ON_MESSAGE(WM_PARAMETER_UDPATED, onWatchedParameterUpdated)
END_MESSAGE_MAP()


    void ScadaDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_address.SetLimitText(ADDRESS_FIELD_LENGTH);
        m_description.SetLimitText(DESCRIPTION_FIELD_LENGTH);

        m_propertySheet = new CMultiFontPropertySheet("",this);

        // Add the attribute page as this is needed by every SCADA item
        m_propertySheet->AddPage( &m_attributePage );

        // Make the property sheet visible
        m_propertySheet->Create(this,WS_CHILD | WS_VISIBLE );

        // Move the property page into position
        CRect position;
        GetDlgItem(IDC_PROPERTY_SHEET_POSITION)->GetWindowRect(position);
        GetDlgItem(IDC_PROPERTY_SHEET_POSITION)->ShowWindow(false);
        ScreenToClient(position);
        m_propertySheet->MoveWindow(position,TRUE);

        // We must tell the attribute page which entity parameters we are interested in
        // When any of these parameters change then it must post a message back to us
        std::set<std::string> paramsToWatch;
        paramsToWatch.insert(DATAPOINT_TYPE);
        paramsToWatch.insert(ALLOW_WRITE);
        m_attributePage.setWatchParameters(paramsToWatch, this, WM_PARAMETER_UDPATED);

        // Hide all the scada item details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ScadaDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PARENT)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_AGENT)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PARENT)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_AGENT)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_PARENT)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_AGENT)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_ADDRESS_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_PARENT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_AGENT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_SUBSYSTEM_BITMAP)->ShowWindow(isShown);

        if (m_propertySheet != NULL)
        {
            m_propertySheet->ShowWindow(isShown);
            for (int i =0; i< m_propertySheet->GetPageCount(); ++i)
            {
                if (m_propertySheet->GetPage(i)->m_hWnd != NULL)
                {
                    m_propertySheet->GetPage(i)->ShowWindow(isShown);
                }
            }
        }
    }


    void ScadaDetailView::refreshButtonAvailability()
    {
        if (m_currentEntity == NULL)
        {
            return;
        }

        if (m_currentEntity->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentEntity->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }

        if (m_currentEntity->getType() == TA_Base_Core::DataPointEntityData::getStaticType() )
        {
            try
            {
                std::string dataPointType = m_currentEntity->getParameterValue(ScadaDetailView::DATAPOINT_TYPE);

				//TD15446, Specially deal with Analogue Alarm and Boolean Alarm
				bool isAlarmStateChanged = false;
				if ( m_currentEntity->getParameterValue(ALLOW_WRITE) != TA_TRUE )
				{
					if (dataPointType == ANALOGUE_TYPE)
					{
						isAlarmStateChanged = m_analogueAlarmPage.getIsAlarmStatesChanged();
					}
					else if (dataPointType == BOOLEAN_TYPE)
					{
						isAlarmStateChanged = m_booleanAlarmPage.getIsAlarmStatesChanged();
					}
					
					if (isAlarmStateChanged == true)
					{
						GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
						GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
						return;
					}
				}
				

                if( ScadaDatabaseAccessor::isDerivedDataPointType(dataPointType) )
                {
                    if (m_databaseAccessor->getDerivedDataPointItem(m_currentEntity->getUniqueIdentifier()).hasChanged())
                    {
                        GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
                        GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
                        return;
                    }
                }
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                return;
            }
        }

        GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
        GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
    }
    
    
    void ScadaDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentEntity)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ScadaDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ScadaTreeView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ScadaTreeView");
        }


        m_currentEntity = dynamic_cast<TA_Base_Core::IConfigEntity*>(currentEntity);

        if (m_currentEntity == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        // First save the currently selected page so we can select it again if possible
        m_currentlySelectedPage = m_propertySheet->GetActivePage();

        // We must remove all pages except for the attribute one so that the correct pages are then re-added
        // to the sheet. We remove them in reverse order so the number does not get confused
        if (m_propertySheet != NULL)
        {
            for (int i = m_propertySheet->GetPageCount()-1; i > 0; --i)
            {
                m_propertySheet->RemovePage(i);
            }
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigEntiy* object

	        m_address.SetWindowText( m_currentEntity->getAddress().c_str() );
	        m_name.SetWindowText( m_currentEntity->getName().c_str() );
	        m_description.SetWindowText( m_currentEntity->getDescription().c_str() );
	        m_location.SetWindowText( m_currentEntity->getLocationName().c_str() );
            m_parent.SetWindowText( m_currentEntity->getParentName().c_str() );
            m_agent.SetWindowText( m_currentEntity->getAgentName().c_str() );

            if( m_currentEntity->isNew() )
            {
                // This is a new SCADA item or at least a copy of an entity so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                CString type;
                type.Format("%s%s",m_currentEntity->getType().c_str(),NEW_LABEL);
                m_type.SetWindowText(type);

                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( m_currentEntity->getType().c_str());

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				// ++ TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentEntity->getDateModified();
                if (modify == 0)
                {
                    m_modifyDate.SetWindowText(NO_VALUE);
                }
                else
                {
                    COleDateTime dateModified( modify );
                    // TD12474 ++
                    //m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
                    m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                    // ++ TD12474
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_currentEntity->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474
            }

            if (0 == m_currentEntity->getType().compare( TA_Base_Core::DataPointEntityData::getStaticType()) )
            {
                // For datapoints we display the physical subsystem and the user can change it
                m_subsystem.SetWindowText( m_currentEntity->getPhysicalSubsystemName().c_str() );
                GetDlgItem(IDC_BUTTON_SUBSYSTEM)->EnableWindow(TRUE);

                setUpForDataPoint();
            }
			//TD13623
			else if (0 == m_currentEntity->getType().compare( TA_Base_Core::DataNodeEntityData::getStaticType()) )
            {
            //TD13623 // display the physical subsystem and the user can change it
                m_subsystem.SetWindowText( m_currentEntity->getPhysicalSubsystemName().c_str() );
                GetDlgItem(IDC_BUTTON_SUBSYSTEM)->EnableWindow(TRUE);

                setUpForDataNode();
            }
            else
            {
                // For anything else we show the TransActive subsystem and the user cannot change it
                m_subsystem.SetWindowText( m_currentEntity->getSubsystemName().c_str() );
                GetDlgItem(IDC_BUTTON_SUBSYSTEM)->EnableWindow(FALSE);
            }


            // Get each property page to populate themselves with the current entity
            if (m_propertySheet != NULL)
            {
                for (int i =0; i< m_propertySheet->GetPageCount(); ++i)
                {
                    IConfigurationPage* page = dynamic_cast<IConfigurationPage*>(m_propertySheet->GetPage(i));
                    if (page != NULL)
                    {
                        page->populatePage( *m_currentEntity, *m_databaseAccessor );
                    }
                }
            }

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve SCADA item details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve SCADA item details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


	void ScadaDetailView::setUpForDataNode()
	{
		FUNCTION_ENTRY("setUpForDataNode");

        TA_ASSERT(m_propertySheet != NULL, "The property sheet is NULL");
        TA_ASSERT(m_currentEntity != NULL, "The current entity is NULL so we cannot determine what pages to add for the data node");

		// add the SynthesisConfiguration page
		m_propertySheet->AddPage ( & m_dataNodeSynthesisConfiguration );

		FUNCTION_EXIT;
	}


    void ScadaDetailView::setUpForDataPoint()
    {
        FUNCTION_ENTRY("setUpForDataPoint");

        TA_ASSERT(m_propertySheet != NULL, "The property sheet is NULL");
        TA_ASSERT(m_currentEntity != NULL, "The current entity is NULL so we cannot determine what pages to add for the datapoint");

        // We must determine what entity parameters are already being used by the other pages so we can
        // hide them in the attribute list. We will generate this list and then pass it to the attribute page.
        std::set<std::string> parametersToIgnore;
        m_booleanAlarmPage.getUsedParameters(parametersToIgnore);
        m_analogueAlarmPage.getUsedParameters(parametersToIgnore);
        m_booleanEngUnitsPage.getUsedParameters(parametersToIgnore);
        m_analogueEngUnitsPage.getUsedParameters(parametersToIgnore);
        m_helpPage.getUsedParameters(parametersToIgnore);
        m_statePage.getUsedParameters(parametersToIgnore);
        m_inputPage.getUsedParameters(parametersToIgnore);
        m_outputPage.getUsedParameters(parametersToIgnore);
        m_meteredPage.getUsedParameters(parametersToIgnore);

        // Now tell the attribute page which parameters we're using so it can ignore them
        m_attributePage.setParametersToIgnore(parametersToIgnore);

        std::string dataPointType("");
        bool isOutputDataPoint(false);

        try
        {
            dataPointType = m_currentEntity->getParameterValue( DATAPOINT_TYPE );
            isOutputDataPoint = (m_currentEntity->getParameterValue(ALLOW_WRITE) == TA_TRUE);
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not retrieve the data point type so could not determine what pages to add to the property sheet");
        }

        if (dataPointType == ANALOGUE_TYPE)
        {
            // If this is not an output datapoint then we can add the alarm configuration page
            if ( m_currentEntity->getParameterValue(ALLOW_WRITE) != TA_TRUE )
            {
                // If AllowWrite != Yes then this is not an output datapoint
                m_propertySheet->AddPage(&m_analogueAlarmPage);
            }
            m_propertySheet->AddPage(&m_analogueEngUnitsPage);
        }
        else if (dataPointType == BOOLEAN_TYPE)
        {
            // If this is not an output datapoint then we can add the alarm configuration page
            if ( m_currentEntity->getParameterValue(ALLOW_WRITE) != TA_TRUE )
            {
                // If AllowWrite != Yes then this is not an output datapoint
                m_propertySheet->AddPage(&m_booleanAlarmPage);
            }
            m_propertySheet->AddPage(&m_booleanEngUnitsPage);

#if 0	//TD18137 marc_bugfix interlocking tab not necessary (new requirement)
            // TD12257: Show the Interlocking tab on output datapoints only.
            if (isOutputDataPoint)
            {
    		    m_propertySheet->AddPage(&m_interlockingConfigurationPage );
            }
#endif
        }
        else if (dataPointType == DERIVED_TYPE)
        {
            m_propertySheet->AddPage(&m_inputPage);
            m_propertySheet->AddPage(&m_statePage);
            m_propertySheet->AddPage(&m_outputPage);
        }
        else if (dataPointType == TEXT_TYPE)
        {
            // Do nothing for now
        }
        else if (dataPointType == METERED_TYPE)
        {
            m_propertySheet->AddPage(&m_meteredPage);
        }
        else if ( dataPointType == SAFETY_OUTPUT_TYPE)
        {
            m_propertySheet->AddPage(&m_statePage);
            m_propertySheet->AddPage(&m_outputPage);
        }
        else if (dataPointType == COMBINED_OUTPUT_TYPE)
        {
            m_propertySheet->AddPage(&m_statePage);
            m_propertySheet->AddPage(&m_outputPage);
        }
        else if (dataPointType == ENUM_TYPE)
        {
			//TD 4348: been requested to remove analogueEngUnitsPage for enum dp configuration
            //m_propertySheet->AddPage(&m_analogueEngUnitsPage);
                      
            m_propertySheet->AddPage(&m_statePage);

            if (isOutputDataPoint)
            {
                // Display the output associations page, provided this is an output datapoint
                m_propertySheet->AddPage(&m_outputPage);
            }            
        }


        // All datapoints need the help page
        m_propertySheet->AddPage(&m_helpPage);

/*
// It would be nice to keep a record of the currently selected page and set it again when the user changes
// to a different datapoint. Unfortunately the active page is not being set correctly in the call to
// SetActivePage.

        // Reset the user to the page they were previously viewing
        if (m_currentlySelectedPage != NULL)
        {
            if (0 == m_propertySheet->SetActivePage( m_currentlySelectedPage ) )
            {
                // We could not select the specified page so select the attribute page (the first page)
                m_propertySheet->SetActivePage( 0 );
            }
            m_propertySheet->Invalidate();
        }
/**/

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigEntity* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentEntity != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentEntity->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentEntity->getUniqueIdentifier(), ScadaTreeView::CHANGE_OF_NAME );
                }
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }   

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onKillfocusEditDescription() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // When the user changes the description we must update the IConfigEntity* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentEntity != NULL )
        {
            try
            {
                CString description;
                m_description.GetWindowText(description);
                m_currentEntity->setDescription( std::string(description) );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onKillfocusEditAddress() 
    {
        FUNCTION_ENTRY("onKillfocusEditAddress");

        // When the user changes the address we must update the IConfigEntity* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentEntity != NULL )
        {
            try
            {
                CString address;
                m_address.GetWindowText(address);
                m_currentEntity->setAddress( std::string(address) );
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);

                FUNCTION_EXIT;
                return;
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);

                FUNCTION_EXIT;
                return;
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onButtonChangeLocation() 
    {
	    FUNCTION_ENTRY("onButtonChangeLocation");
	 
        // Retrieve the current location this SCADA item is using
        CString currentLocation("");
        m_location.GetWindowText( currentLocation );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllLocations();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_LOCATION_DIALOG_TITLE, currentLocation, this);
        dlg.setSortItems( false );
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new location");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new location which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentEntity !=  NULL, "current entity is NULL.");

            // If this is a different location then set it in our IConfigEntity*.
            m_currentEntity->setLocation( dlg.getSelectedKey() );

            // Now update the location the details view is showing
            m_location.SetWindowText( dlg.getSelectedName() );

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onButtonChangeSubsystem() 
    {
	    FUNCTION_ENTRY("onButtonChangeSubsystem");
	 
        // Retrieve the current subsystem this SCADA item is using
        CString currentSubsystem("");
        m_subsystem.GetWindowText( currentSubsystem );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllSubsystems();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_SUBSYSTEM_DIALOG_TITLE, currentSubsystem, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new subsystem");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new subsystem which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentEntity !=  NULL, "current entity is NULL.");

            // If this is a different subsystem then set it in our IConfigEntity*.
            m_currentEntity->setSubsystem( dlg.getSelectedKey() );

            // Now update the subsystem the details view is showing
            m_subsystem.SetWindowText( dlg.getSelectedName() );

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030001);
            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }

    
    void ScadaDetailView::onButtonChangeAgent() 
    {
	    FUNCTION_ENTRY("onButtonChangeParent");
	 
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_030020);        
        
        // Retrieve the current agent this SCADA item is using
        CString currentAgent("");
        m_agent.GetWindowText( currentAgent );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
        TA_ASSERT(m_currentEntity !=  NULL, "current entity is NULL.");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllAgents( m_currentEntity->getType() );

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_AGENT_DIALOG_TITLE, currentAgent, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new agent");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new agent which is %s", dlg.getSelectedName());

        try
        {
            // If this is a different agent then set it in our IConfigEntity*.
            m_currentEntity->setAgent( dlg.getSelectedKey() );

            // Now update the parent the details view is showing
            m_agent.SetWindowText( dlg.getSelectedName() );

        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onButtonChangeParent() 
    {
	    FUNCTION_ENTRY("onButtonChangeParent");
	 
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_030018);        
        
        // Retrieve the current parent this SCADA item is using
        CString currentParent("");
        m_parent.GetWindowText( currentParent );

       // Now construct the dialog and then wait for the user to finish with it
        ChangeParentDialog dlg(currentParent, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new parent");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new parent which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentEntity !=  NULL, "current entity is NULL.");

            // First update the agent to the same thing to ensure that it doesn't get changed when we change
            // the parent.
            m_currentEntity->setAgent( m_currentEntity->getAgent() );

            // If this is a different parent then set it in our IConfigEntity*.
            m_currentEntity->setParent( dlg.getSelectedKey() );

            // Now update the parent the details view is showing
            m_parent.SetWindowText( dlg.getSelectedName() );

            // Must also update the agent window in case the agent has changed along with the parent.
            m_agent.SetWindowText( m_currentEntity->getAgentName().c_str() );

            if (m_listView!=NULL)
            {
                m_listView->itemDetailsChanged( m_currentEntity->getUniqueIdentifier(), ScadaTreeView::CHANGE_OF_PARENT );
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentEntity);

        FUNCTION_EXIT;
    }


    void ScadaDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        // Get each property page to populate themselves with the current entity
        if (m_propertySheet != NULL)
        {
            std::vector<std::string> missingParams;
            std::vector<std::string> invalidParams;

            try
            {
                for (int i =0; i< m_propertySheet->GetPageCount(); ++i)
                {
                    IConfigurationPage* page = dynamic_cast<IConfigurationPage*>(m_propertySheet->GetPage(i));
                    if (page != NULL)
                    {
                        page->getMandatoryParametersNotEntered( missingParams );
                        page->getInvalidEnteredParameters( invalidParams );
                    }
                }
            }
            catch ( ... )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "Database error","Could not apply the item changes");

                CString actionName, reason;
                actionName.LoadString(IDS_APPLY);
                reason.LoadString(IDS_DATABASE_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
                
                FUNCTION_EXIT;
                return;
            }

            if (!invalidParams.empty())
            {
                CString actionName, reason;
                actionName.LoadString(IDS_APPLY);
                reason.LoadString(IDS_FIELD_INVALID);

                for (std::vector<std::string>::iterator iter = invalidParams.begin(); iter != invalidParams.end(); ++iter)
                {
                    reason += iter->c_str();
                    reason += "\n";
                }

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                userMsg.showMsgBox(IDS_UE_030060);
                FUNCTION_EXIT;
                return;

            }

            if (!missingParams.empty())
            {
                CString actionName, reason;
                actionName.LoadString(IDS_APPLY);
                reason.LoadString(IDS_FIELD_MISSING);

                for (std::vector<std::string>::iterator iter = missingParams.begin(); iter != missingParams.end(); ++iter)
                {
                    reason += iter->c_str();
                    reason += "\n";
                }

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                userMsg.showMsgBox(IDS_UE_030060);
                FUNCTION_EXIT;
                return;
            }
        }

		//TD18137 marc_bugfix begin 
		try
		{
		
			if (m_currentEntity->getType() == TA_Base_Core::DataPointEntityData::getStaticType() )
			{
			
				if (!checkLCCandRCCCondition())
				{
					return;
				}
			}
		}
		catch(...)
		{
			LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "Unknown exception when evaluating LCC/RCC expression" );
		}
		//TD18137 marc_bugfix begin 

		
        applyButtonPressed(m_currentEntity);


        FUNCTION_EXIT;
    }


    void ScadaDetailView::applyButtonPressed(TA_Base_Core::IConfigItem* currentItem) 
    {
	    FUNCTION_ENTRY("applyButtonPressed");

        try
        {
            // First determine if a derived data point exists and whether this entity needs one
		
            TA_Base_Core::IConfigDerivedDataPoint* derivedDataPoint = NULL;
            bool isDerivedDataPointNeeded = false;
			bool isDataPointType = false;
			std::string dataPointType = "";
        
            if (m_currentEntity->getType() == TA_Base_Core::DataPointEntityData::getStaticType() )
            {
				isDataPointType = true;

                dataPointType = m_currentEntity->getParameterValue(ScadaDetailView::DATAPOINT_TYPE);

                if( ScadaDatabaseAccessor::isDerivedDataPointType(dataPointType) )
                {
                    derivedDataPoint = &(m_databaseAccessor->getDerivedDataPointItem(currentItem->getUniqueIdentifier()) );
                    isDerivedDataPointNeeded = true;
                }
            }


	        if (currentItem != NULL)
            {
                AfxGetApp()->DoWaitCursor(1);

                MessageSender::UpdateDetails updateDetails;
                updateDetails.isNewItem = currentItem->isNew();
                updateDetails.isDelete = false;
                updateDetails.configUpdateMessage = getConfigMessageType();
                updateDetails.nameOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getName();
                if (!currentItem->isNew())
                {
                    updateDetails.changes = currentItem->getAllItemChanges();

					//TD15446, Specially deal with Analogue Alarm and Boolean Alarm
					if (isDataPointType == true)
					{
						if ( m_currentEntity->getParameterValue(ALLOW_WRITE) != TA_TRUE )
						{
							TA_Base_Core::ItemChanges changes;

							if (dataPointType == ANALOGUE_TYPE)
							{
								changes = m_analogueAlarmPage.getAllItemChanges();
							}
							else if (dataPointType == BOOLEAN_TYPE)
							{
								changes = m_booleanAlarmPage.getAllItemChanges();
							}

							if (changes.empty() == false)
							{
								std::copy(changes.begin(), changes.end(), std::inserter(updateDetails.changes, updateDetails.changes.begin()));
							}
						}
					}
					
                    // If this entity is a derived data point then add any of those changes as well
                    if ( (isDerivedDataPointNeeded) && (derivedDataPoint != NULL) )
                    {
                        TA_Base_Core::ItemChanges changes = derivedDataPoint->getAllItemChanges();
                        std::copy(changes.begin(), changes.end(), std::inserter(updateDetails.changes, updateDetails.changes.begin()));
					}

					if (isDataPointType)
					{
						// If the address was changed, display a warning.
						if (updateDetails.changes.find(ADDRESS_FIELD_NAME) != updateDetails.changes.end())
						{
                            TA_Base_Bus::TransActiveMessage userMsg;
                            UINT selectedButton = userMsg.showMsgBox(IDS_UW_030018);        
						}
                    }
                }

                // Apply the changes to the database
                currentItem->applyChanges();
				
				//TD15446, Specially deal with Analogue Alarm and Boolean Alarm
				if (isDataPointType == true)
				{
					if ( m_currentEntity->getParameterValue(ALLOW_WRITE) != TA_TRUE )
					{
						if (dataPointType == ANALOGUE_TYPE)
						{
							m_analogueAlarmPage.saveAlarmStates();
						}
						else if (dataPointType == BOOLEAN_TYPE)
						{
							m_booleanAlarmPage.saveAlarmStates();
						}
					}
				}
				
				
                if ( (isDerivedDataPointNeeded) && (derivedDataPoint != NULL) )
                {
                    derivedDataPoint->setKey( dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getKey() );
                    derivedDataPoint->applyChanges();
                }
                else
                {
                    // Delete any derived data point items that have been created for this. If one has not
                    // been created then this will do nothing
                    m_databaseAccessor->deleteDerivedDataPointItem( currentItem->getUniqueIdentifier() );
                }

                // Update the view
                populateItemDetails(currentItem);

                TA_ASSERT( getMessageSender() != NULL, "The MessageSender is NULL");

                // Retrieve the key of the item after we have applied in case this is a new item.
                // If it is a new item it doesn't get given a key until Apply is called.
                updateDetails.keyOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getKey();

                getMessageSender()->sendMessagesForTheChange( updateDetails );
            }
        }
        catch ( const TA_Base_Core::DataConfigurationException& exception )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataConfigurationException","");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_FIELD_MISSING);

            std::vector<std::string> missing = exception.getMissingFields();
            for (std::vector<std::string>::iterator iter = missing.begin(); iter != missing.end(); ++iter)
            {
                reason += iter->c_str();
                reason += "\n";
            }

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not apply the item changes");

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DataException& exception)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not apply the item changes");

            unsigned int errorId;
            switch(exception.getFailType())
            {
                case (TA_Base_Core::DataException::NOT_UNIQUE):
                {
                    errorId = IDS_FIELD_NOT_UNIQUE;
                    break;
                } 

                case (TA_Base_Core::DataException::WRONG_TYPE):
                {
                    errorId = IDS_FIELD_INVALID;
                    break;
                }
            
                case (TA_Base_Core::DataException::MISSING_MANDATORY):
                {
                    errorId = IDS_FIELD_MISSING;
                    break;
                }

                default:
                {
                    errorId = IDS_UNKNOWN_ERROR;
                    break;
                }
            }

            CString actionName, reason;
            actionName.LoadString(IDS_APPLY);
            reason.LoadString(errorId);
            reason += exception.getWhichData().c_str();
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            userMsg.showMsgBox(IDS_UE_030060);
        }

        AfxGetApp()->DoWaitCursor(-1);
        FUNCTION_EXIT;
    }



    void ScadaDetailView::resetButtonPressed(TA_Base_Core::IConfigItem* currentItem) 
    {
        FUNCTION_ENTRY("resetButtonPressed");

	    if (currentItem != NULL)
        {
            if (currentItem->hasChanged())
            {
                CString actionName;
                actionName.LoadString(IDS_RESET);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName;
                UINT response = userMsg.showMsgBox(IDS_UW_010003);
                if (response == IDNO)
                {
                    LOG_GENERIC( SourceInfo, DebugUtil::DebugDebug, "User cancelled the reset");
                    FUNCTION_EXIT;
                    return;
                }
            }


            try
            {
                // If this item has a derived data point then invalidate it
                if (m_currentEntity->getType() == TA_Base_Core::DataPointEntityData::getStaticType() )
                {
                    std::string dataPointType = m_currentEntity->getParameterValue(ScadaDetailView::DATAPOINT_TYPE);

                    if( ScadaDatabaseAccessor::isDerivedDataPointType(dataPointType) )
                    {
                        TA_Base_Core::IConfigDerivedDataPoint& derivedDataPoint = m_databaseAccessor->getDerivedDataPointItem(currentItem->getUniqueIdentifier());
                        derivedDataPoint.invalidate();
                    }
                }


                // Invalidate and re-populate the details. This will ensure all data is reloaded from the database.
                dynamic_cast<TA_Base_Core::IItem*>(currentItem)->invalidate();


                // Now if this item has a derived data point then invalidate it (just in case the datapointtype changed
                if (m_currentEntity->getType() == TA_Base_Core::DataPointEntityData::getStaticType() )
                {
                    std::string dataPointType = m_currentEntity->getParameterValue(ScadaDetailView::DATAPOINT_TYPE);

                    if( ScadaDatabaseAccessor::isDerivedDataPointType(dataPointType) )
                    {
                        TA_Base_Core::IConfigDerivedDataPoint& derivedDataPoint = m_databaseAccessor->getDerivedDataPointItem(currentItem->getUniqueIdentifier());
                        derivedDataPoint.invalidate();
                    }
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve SCADA item details to display");
            
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve SCADA item details to display");

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);
            }


            // Alert the list view that this has been updated so it can update the item's name in
            // the list if necessary.
            if (m_listView != NULL)
            {
                m_listView->itemDetailsChanged( currentItem->getUniqueIdentifier(), dynamic_cast<TA_Base_Core::IItem*>(currentItem)->getName().c_str() );
            }

            populateItemDetails(currentItem);
        }
        FUNCTION_EXIT;
    }

    
    LRESULT ScadaDetailView::onWatchedParameterUpdated(WPARAM,LPARAM paramName)
    {
        FUNCTION_ENTRY("onWatchedParameterUpdated");

        // Retrieve the parameter name and delete the pointer
        std::string* parameterNamePtr = reinterpret_cast<std::string*>(paramName);
        std::string parameterName = *parameterNamePtr;
        
        delete parameterNamePtr;
        parameterNamePtr = NULL;

        if (m_currentEntity == NULL)
        {
            FUNCTION_EXIT;
            return NULL;
        }


        try
        {
            if( m_currentEntity->getType() != TA_Base_Core::DataPointEntityData::getStaticType() )
            {
                // Wrong type so we can return
                FUNCTION_EXIT;
                return NULL;
            }

            if (parameterName == ALLOW_WRITE)
            {
                if ( m_currentEntity->getParameterValue(ALLOW_WRITE) == TA_TRUE )
                {
                    // This is an output datapoint now so the user can no longer configure alarms
                    if ( m_currentEntity->getParameterValue(DATAPOINT_TYPE) == BOOLEAN_TYPE )
                    {
                        m_currentEntity->setParameterValue(TA_FALSE,BooleanAlarmConfiguration::TRUE_ALARM_ENABLED);
                        m_currentEntity->setParameterValue(TA_FALSE,BooleanAlarmConfiguration::FALSE_ALARM_ENABLED);
                    }
                    else if ( ( m_currentEntity->getParameterValue(DATAPOINT_TYPE) == ANALOGUE_TYPE ) ||
							  ( m_currentEntity->getParameterValue(DATAPOINT_TYPE) == ENUM_TYPE ) )
                    {
                        m_currentEntity->setParameterValue(TA_FALSE,AnalogueAlarmConfiguration::LOLO_ALARM_ENABLED);
                        m_currentEntity->setParameterValue(TA_FALSE,AnalogueAlarmConfiguration::LO_ALARM_ENABLED);
                        m_currentEntity->setParameterValue(TA_FALSE,AnalogueAlarmConfiguration::HI_ALARM_ENABLED);
                        m_currentEntity->setParameterValue(TA_FALSE,AnalogueAlarmConfiguration::HIHI_ALARM_ENABLED);
                        m_currentEntity->setParameterValue(TA_FALSE,AnalogueAlarmConfiguration::OH_ALARM_ENABLED);
                        m_currentEntity->setParameterValue(TA_FALSE,AnalogueAlarmConfiguration::OL_ALARM_ENABLED);
                    }
                }
            }

            // We must remove all pages except for the attribute one so that the correct pages are then re-added
            // to the sheet. We remove them in reverse order so the number does not get confused
            if (m_propertySheet != NULL)
            {
                for (int i = m_propertySheet->GetPageCount()-1; i > 0; --i)
                {
                    m_propertySheet->RemovePage(i);
                }
            }

            setUpForDataPoint();

            // Get each property page to populate themselves with the current entity
            if (m_propertySheet != NULL)
            {
                for (int i =0; i< m_propertySheet->GetPageCount(); ++i)
                {
                    IConfigurationPage* page = dynamic_cast<IConfigurationPage*>(m_propertySheet->GetPage(i));
                    if (page != NULL)
                    {
                        page->populatePage( *m_currentEntity, *m_databaseAccessor );
                    }
                }
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve SCADA item details to display");
        
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve SCADA item details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
        
        FUNCTION_EXIT;
        return NULL;
    }


    std::string ScadaDetailView::getMmsStateFromCombo(CComboBox& combo)
    {
        int curSel = combo.GetCurSel();

        if (curSel >= 0)
        {
            CString selString;
            combo.GetLBText(curSel, selString);

            // Find the appropriate string in the alarm types map
            std::map<std::string, std::string> data = ScadaDatabaseAccessor::getMmsAlarmTypes();
            
            for (std::map<std::string, std::string>::iterator itr = data.begin();
                    itr != data.end(); ++itr)
            {
                if (stricmp(selString.GetBuffer(0), itr->second.c_str()) == 0)
                {
                    // Return the corresponding state value, as description field matches
                    return itr->first;                    
                }
            }
        }
        
        // If item not found, empty string returned (valid mms state)
        return "";        
    }


    void ScadaDetailView::setupMmsCombo(CComboBox& combo)
    {
        combo.ResetContent();

        std::map<std::string, std::string> data = ScadaDatabaseAccessor::getMmsAlarmTypes();
        for (std::map<std::string, std::string>::iterator itr = data.begin(); itr != data.end(); ++itr )
        {
            combo.AddString(itr->second.c_str());
        }

        // Empty selection initially
        setMmsStateComboSelection(combo, "");
    }


    void ScadaDetailView::setMmsStateComboSelection(CComboBox& combo, const std::string& state)
    {
        std::map<std::string, std::string> data = ScadaDatabaseAccessor::getMmsAlarmTypes();
        
        std::map<std::string, std::string>::iterator itr = data.find(state);

        if (itr == data.end())
        {
            // As a rule this should never happen (the default selection is "None") -
            // but not a fatal condition if this does happen
            combo.SetCurSel(-1);
        }
        else
        {
            // Find and set the appropriate mode in combo
            int selIndex = combo.FindStringExact(-1, itr->second.c_str());
            combo.SetCurSel(selIndex);
        }
    }

	//TD18137 marc_bugfix_begin implementation of syntax and datapoint variable validity checking

	bool ScadaDetailView::bEvaluateExpression(const std::string &expression, const int expression_type)
	{
		std::auto_ptr <TA_Base_Bus::ExpressionEvaluator> expEval(new TA_Base_Bus::ExpressionEvaluator);
		BooleanContextImpl* context = new BooleanContextImpl();
		RealNumberContextImpl* realContext = new RealNumberContextImpl();
		Tokeniser m_tokeniser;
		std::vector< TokenPtr > m_tokens;
		bool bSuccess = true;

		FUNCTION_ENTRY("bEvaluateExpression");

		try
		{

			expEval->setNewExpression(expression);
			expEval->addBooleanContext(context);				
			expEval->addRealNumberContext(realContext);
			expEval->evaluateBooleanExpression();

			m_tokeniser.setExpression(expression);
			//Now check for the validity of datapoint variables
			m_tokens = m_tokeniser.getTokens();
					
					
			for (int i = 0; i < m_tokens.size(); i++)
			{
				if ((m_tokens[i])->getType() == TA_Base_Bus::EETT_Variable)
				{
					std::auto_ptr<TA_Base_Core::IEntityData> entityData(TA_Base_Core::EntityAccessFactory::getInstance().getEntity((m_tokens[i])->getVariable()));
					TA_Base_Core::DataPointEntityData* dataPointEntityData = dynamic_cast<TA_Base_Core::DataPointEntityData *>(entityData.get());
					if (!dataPointEntityData)
					{
						//This is not a valid data point
						throw;
					}						
				}
			}
		}
		catch(TA_Base_Core::MathematicalEvaluationException e)
		{
		    TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName;
			
			if (expression_type == 1)
			{
				//LCC condition evaluated
				actionName.LoadString(IDS_PARSE_ERROR_LAUNCH_CONDITION);
			}
			else
			{
				//RCC condition evaluated
				actionName.LoadString(IDS_PARSE_ERROR_RETURN_CONDITION);
			}
			userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_110017);
			
			bSuccess =  false;
		}
		catch(...)
		{
		    TA_Base_Bus::TransActiveMessage userMsg;
			CString actionName;

			if (expression_type == 1)
			{
				//LCC condition evaluated
				actionName.LoadString(IDS_ERROR_LAUNCH_CONDITION);
			}
			else
			{
				//RCC condition evaluated
				actionName.LoadString(IDS_ERROR_RETURN_CONDITION);
			}
			userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_110017);

			bSuccess =  false;
			
		}
		
		delete realContext;
		delete context;

		FUNCTION_EXIT;

		return bSuccess;
	}

	bool ScadaDetailView::checkLCCandRCCCondition()
	{
		std::string current_expression = "";
		const int EVALUATE_LCC = 1;
		const int EVALUATE_RCC = 2;


		FUNCTION_ENTRY("checkLCCandRCCCondition");

		//check for launch Condition expression
		current_expression = m_currentEntity->getParameterValue(ScadaDetailView::LAUNCH_CONDITION);

		if (!current_expression.empty())
		{
			if (!bEvaluateExpression(current_expression, EVALUATE_LCC))
			{
				return false;
			}
		}


		//check for return condition expression
		current_expression = m_currentEntity->getParameterValue(ScadaDetailView::RETURN_CONDITION);

		if (!current_expression.empty())
		{
			if (!bEvaluateExpression(current_expression, EVALUATE_RCC))
			{
				return false;
			}

		}

		FUNCTION_EXIT;

		return true;
	}
	//TD18137 marc_bugfix_end implementation of syntax and datapoint variable validity checking

    /////////////////////////////////////////////////////////////////////////////
    // ScadaDetailView diagnostics

    #ifdef _DEBUG
    void ScadaDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ScadaDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
