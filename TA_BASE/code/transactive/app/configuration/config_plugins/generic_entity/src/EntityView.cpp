/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/generic_entity/src/EntityView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure an entity's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/generic_entity/src/StdAfx.h"
#include "app/configuration/config_plugins/generic_entity/src/EntityView.h"
#include "app/configuration/config_plugins/generic_entity/src/GenericEntityComponent.h"
#include "app/configuration/config_plugins/generic_entity/src/EntityDatabaseAccessor.h"
#include "app/configuration/config_plugins/generic_entity/src/EntityListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/entity_access/src/ConfigurationEditor.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Core::ConfigurationEditor;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{

    // The titles for the change dialog box
    const CString EntityView::CHANGE_PARENT_DIALOG_TITLE    = "Change Parent";
    const CString EntityView::CHANGE_LOCATION_DIALOG_TITLE  = "Change Location";
    const CString EntityView::CHANGE_REGION_DIALOG_TITLE  = "Change Region";

    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int EntityView::NAME_FIELD_LENGTH        = 80;
    const int EntityView::ADDRESS_FIELD_LENGTH     = 100;
    const int EntityView::DESCRIPTION_FIELD_LENGTH = 2000;

    IMPLEMENT_DYNCREATE(EntityView, CFormView)


    EntityView::EntityView() : 
        m_currentEntity(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(EntityView::IDD),
		m_showRegion(false)
    {
	    //{{AFX_DATA_INIT(EntityView)
	//}}AFX_DATA_INIT
    }


    EntityView::~EntityView()
    {
    }


    void EntityView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(EntityView)
	DDX_Control(pDX, IDC_EDIT_REGION, m_region);
	DDX_Control(pDX, IDC_ADDRESS_BITMAP, m_addressBitmap);
	DDX_Control(pDX, IDC_DESCRIPTION_BITMAP, m_descriptionBitmap);
	DDX_Control(pDX, IDC_PARENT_BITMAP, m_parentBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_LOCATION_BITMAP, m_locationBitmap);
	DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_parameterList);
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


BEGIN_MESSAGE_MAP(EntityView, AbstractDetailView)
	//{{AFX_MSG_MAP(EntityView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonChangeLocation)
	ON_BN_CLICKED(IDC_BUTTON_PARENT, onButtonChangeParent)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDRESS, onKillfocusEditAddress)
	ON_BN_CLICKED(IDC_BUTTON_REGION, OnButtonChangeRegion)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void EntityView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_address.SetLimitText(ADDRESS_FIELD_LENGTH);
        m_description.SetLimitText(DESCRIPTION_FIELD_LENGTH);

        // Hide all the entity details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void EntityView::showItemDetails(bool isShown)
    {

		// Only show the region controls is the show region flag is set
		if (m_showRegion == true)
		{
			GetDlgItem(IDC_STATIC_REGION)->ShowWindow(isShown);
			GetDlgItem(IDC_EDIT_REGION)->ShowWindow(isShown);
			GetDlgItem(IDC_BUTTON_REGION)->ShowWindow(isShown);
			GetDlgItem(IDC_EDIT_REGION)->EnableWindow(isShown);
			GetDlgItem(IDC_BUTTON_REGION)->EnableWindow(isShown);
		}
		else
		{
			GetDlgItem(IDC_STATIC_REGION)->ShowWindow(false);
			GetDlgItem(IDC_EDIT_REGION)->ShowWindow(false);
			GetDlgItem(IDC_BUTTON_REGION)->ShowWindow(false);
			GetDlgItem(IDC_EDIT_REGION)->EnableWindow(false);
			GetDlgItem(IDC_BUTTON_REGION)->EnableWindow(false);
		}

        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PARENT)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ATTRIBUTES)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PARENT)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_PARENT)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_LIST_ATTRIBUTES)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_ADDRESS_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_PARENT_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);

		// Invalidate the view
		Invalidate();

		// Refresh the view
		UpdateWindow();
    }


    void EntityView::refreshButtonAvailability()
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
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }
    }


    void EntityView::populateItemDetails(TA_Base_Core::IConfigItem* currentEntity)
    {
        FUNCTION_ENTRY("populateEntityDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<EntityDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<EntityListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper EntityDetailView");
        }


        m_currentEntity = dynamic_cast<TA_Base_Core::IConfigEntity*>(currentEntity);

        if (m_currentEntity == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigEntiy* object

            // The entityForPopulating contains some stuff we can add into the details so lets do it
	        m_address.SetWindowText( m_currentEntity->getAddress().c_str() );
	        m_name.SetWindowText( m_currentEntity->getName().c_str() );
	        m_description.SetWindowText( m_currentEntity->getDescription().c_str() );
	        m_location.SetWindowText( m_currentEntity->getLocationName().c_str() );
			m_region.SetWindowText( m_currentEntity->getRegionName().c_str() );
            m_subsystem.SetWindowText( m_currentEntity->getSubsystemName().c_str() );
            m_parent.SetWindowText( m_currentEntity->getParentName().c_str() );

            if( m_currentEntity->isNew() )
            {
                // This is a new entity or at least a copy of an entity so we indicate this in the type
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

            m_parameterList.populateControl(m_currentEntity);

			// Work out if the region controls need to be shown
			showRegionControls();

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve entity details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve entity details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void EntityView::onKillfocusEditName() 
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
                    m_listView->itemDetailsChanged( m_currentEntity->getUniqueIdentifier(), m_currentEntity->getName().c_str() );
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


    void EntityView::onKillfocusEditDescription() 
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


    void EntityView::onKillfocusEditAddress() 
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


    void EntityView::onButtonChangeLocation() 
    {
	    FUNCTION_ENTRY("onButtonChangeLocation");
	 
        // Retrieve the current location this entity is using
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


	void EntityView::OnButtonChangeRegion() 
	{
		FUNCTION_ENTRY("OnButtonChangeRegion");
	 
        // Retrieve the current region this entity is using
        CString currentRegion("");
        m_region.GetWindowText( currentRegion );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllRegions();

        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_REGION_DIALOG_TITLE, currentRegion, this);
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the selection of a new region");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected a new region which is %s", dlg.getSelectedName());

        try
        {
            TA_ASSERT(m_currentEntity !=  NULL, "current entity is NULL.");

            // If this is a different region then set it in our IConfigEntity*.
            m_currentEntity->setRegion( dlg.getSelectedKey() );

            // Now update the region the details view is showing
            m_region.SetWindowText( dlg.getSelectedName() );

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


    void EntityView::onButtonChangeParent() 
    {
	    FUNCTION_ENTRY("onButtonChangeParent");
	 
        TA_Base_Bus::TransActiveMessage userMsg;
        UINT selectedButton = userMsg.showMsgBox(IDS_UW_030018);
        
        // Retrieve the current parent this entity is using
        CString currentParent("");
        m_parent.GetWindowText( currentParent );

        ChangeDialog::ItemsForSelection items;
        try
        {
            TA_ASSERT(m_currentEntity !=  NULL, "current entity is NULL.");
            items = m_currentEntity->getPossibleParentEntities();
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


        // Now construct the dialog and then wait for the user to finish with it
        ChangeDialog dlg(items, CHANGE_PARENT_DIALOG_TITLE, currentParent, this);
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
            // If this is a different parent then set it in our IConfigEntity*.
            m_currentEntity->setParent( dlg.getSelectedKey() );

            // Now update the parent the details view is showing
            m_parent.SetWindowText( dlg.getSelectedName() );

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


    void EntityView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentEntity);

        FUNCTION_EXIT;
    }


    void EntityView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentEntity);

        FUNCTION_EXIT;
    }


	int EntityView::moveControlUpRelativeToControl(const CWnd& firstControl,
												    CWnd& secondControl) const
	{
		FUNCTION_ENTRY("moveControlUpRelativeToControl");

		TA_ASSERT(&firstControl != NULL, "firstControl is NULL");
		TA_ASSERT(&secondControl != NULL, "secondControl is NULL");

		// Get the parent area coordinates
		CRect parentRect;
		GetClientRect(&parentRect);

		// Convert the client area coordinates to screen coordinates
		ClientToScreen(parentRect);

		// Get the region control coordinates
		RECT firstRect = {0, 0, 0, 0};
		firstControl.GetWindowRect(&firstRect);

		// Get the next contol coordinates
		RECT secondRect = {0, 0, 0, 0};
		secondControl.GetWindowRect(&secondRect);

		// Variable for storing the shift upwards.  Do this before 
		// changing the values below
		const int returnValue = secondRect.top - firstRect.top;

		// Store the width
		const int width = secondRect.right - secondRect.left;

		// Make the coodinates relative to the screen
		secondRect.top = (secondRect.top - parentRect.top) - returnValue;
		secondRect.bottom = (secondRect.bottom - parentRect.top) - returnValue;
		secondRect.left = secondRect.left - parentRect.left;
		secondRect.right = secondRect.left + width;

		// Move the control
		secondControl.MoveWindow(&secondRect, true);

		// Refresh the control
		secondControl.Invalidate();
		secondControl.UpdateWindow();

		FUNCTION_EXIT;

		return returnValue;
	}


	int EntityView::moveControlUpByValue(CWnd& control, const int& moveUpBy) const
	{
		FUNCTION_ENTRY("moveControlUpByValue");

		TA_ASSERT(&control != NULL, "control is NULL");

		if (moveUpBy == 0)
		{
			return moveUpBy;
		}

		// Get the parent area coordinates
		CRect parentRect;
		GetClientRect(&parentRect);

		// Convert the client area coordinates to screen coordinates
		ClientToScreen(parentRect);

		// Get the region control coordinates
		RECT controlRect = {0, 0, 0, 0};
		control.GetWindowRect(&controlRect);

		// Store the width
		const int width = controlRect.right - controlRect.left;

		// Make the coodinates relative to the screen
		controlRect.top = (controlRect.top - parentRect.top) - moveUpBy;
		controlRect.bottom = (controlRect.bottom - parentRect.top) - moveUpBy;
		controlRect.left = controlRect.left - parentRect.left;
		controlRect.right = controlRect.left + width;

		// Move the control
		control.MoveWindow(&controlRect, true);

		// Refresh the control
		control.Invalidate();
		control.UpdateWindow();

		FUNCTION_EXIT;

		return moveUpBy;
	}


	void EntityView::getShowRegionFlag()
	{
		FUNCTION_ENTRY("getShowRegionFlag");
	
		TA_ASSERT(m_currentEntity != NULL, "No current entity is defined.");	

		try
		{
			m_showRegion = TA_Base_Core::ConfigurationEditor::getShowRegion();
		}
		catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve show region data.  Setting false.");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

			FUNCTION_EXIT;
			return;
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve show region data.  Setting false.");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

			FUNCTION_EXIT;
			return;
        }

		FUNCTION_EXIT;
	}


	void EntityView::showRegionControls()
	{
		FUNCTION_ENTRY("showRegionControls");

		// Work out if the Region fields should be shown or not
		getShowRegionFlag();

		// Adjust the controls to compensate for the fact that the
		// region controls are not there
		if (m_showRegion == false)
		{
			// Move the control below the region control up to where the
			// the region control
			const int moveUpBy =
				moveControlUpRelativeToControl(*(GetDlgItem(IDC_STATIC_REGION)),
										       *(GetDlgItem(IDC_STATIC_SUBSYSTEM)));

			// Move all the other controls up by the same amount
			moveControlUpByValue(*(GetDlgItem(IDC_EDIT_SUBSYSTEM)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_BUTTON_APPLY)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_BUTTON_RESET)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_STATIC_MODIFYDATE)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_STATIC_CREATEDATE)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_LIST_ATTRIBUTES)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_STATIC_ATTRIBUTES)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_STATIC_DATE_CREATED)), moveUpBy);
			moveControlUpByValue(*(GetDlgItem(IDC_STATIC_DATE_MODIFIED)), moveUpBy);

			// Resize the Group box
			resizeControlUpByValue(*(GetDlgItem(IDC_STATIC_TYPE)), moveUpBy);
		}

		FUNCTION_EXIT;
	}


	int EntityView::resizeControlUpByValue(CWnd& control, const int& moveUpBy) const
	{
		FUNCTION_ENTRY("resizeControlUpByValue");

		TA_ASSERT(&control != NULL, "control is NULL");

		if (moveUpBy == 0)
		{
			return moveUpBy;
		}

		// Get the parent area coordinates
		CRect parentRect;
		GetClientRect(&parentRect);

		// Convert the client area coordinates to screen coordinates
		ClientToScreen(parentRect);

		// Get the region control coordinates
		RECT controlRect = {0, 0, 0, 0};
		control.GetWindowRect(&controlRect);

		// Store the width
		const int width = controlRect.right - controlRect.left;

		// Make the coodinates relative to the screen
		controlRect.top = controlRect.top - parentRect.top;
		controlRect.bottom = (controlRect.bottom - parentRect.top) - moveUpBy;
		controlRect.left = controlRect.left - parentRect.left;
		controlRect.right = controlRect.left + width;

		// Move the control
		control.MoveWindow(&controlRect, true);
		
		// Refresh the control
		control.Invalidate();
		control.UpdateWindow();

		return moveUpBy;	

		FUNCTION_EXIT;
	}


    /////////////////////////////////////////////////////////////////////////////
    // EntityView diagnostics

    #ifdef _DEBUG
    void EntityView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void EntityView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

