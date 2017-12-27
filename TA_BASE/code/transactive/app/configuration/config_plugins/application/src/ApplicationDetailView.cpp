/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Application's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/application/src/StdAfx.h"
#include "app/configuration/config_plugins/application/src/ApplicationDetailView.h"

#include "app/configuration/config_plugins/application/src/ApplicationDatabaseAccessor.h"
#include "app/configuration/config_plugins/application/src/ApplicationListView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigGui.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"

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
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // The titles for the change dialog box
    const CString ApplicationDetailView::CHANGE_LOCATION_DIALOG_TITLE  = "Change Location";

    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int ApplicationDetailView::SHORT_NAME_FIELD_LENGTH  = 50;
    const int ApplicationDetailView::FULL_NAME_FIELD_LENGTH   = 50;
    const int ApplicationDetailView::EXECUTABLE_FIELD_LENGTH  = 1024;
    const int ApplicationDetailView::NAME_FIELD_LENGTH        = 80;
    const int ApplicationDetailView::DESCRIPTION_FIELD_LENGTH = 2000;
    const int ApplicationDetailView::ADDRESS_FIELD_LENGTH     = 100;

    const CString ApplicationDetailView::ENTITYNAME = "--entity-name=";

    IMPLEMENT_DYNCREATE(ApplicationDetailView, CFormView)


    ApplicationDetailView::ApplicationDetailView() : 
        m_currentApplication(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ApplicationDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(ApplicationDetailView)
	//}}AFX_DATA_INIT
    }


    ApplicationDetailView::~ApplicationDetailView()
    {
    }


    void ApplicationDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ApplicationDetailView)
	DDX_Control(pDX, IDC_CHECK_REPOSITION, m_checkReposition);
	DDX_Control(pDX, IDC_EDIT_HELP, m_helpPath);
	DDX_Control(pDX, IDC_SHORTNAME_BITMAP, m_shortnameBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_LOCATION_BITMAP, m_locationBitmap);
	DDX_Control(pDX, IDC_DESCRIPTION_BITMAP, m_descriptionBitmap);
	DDX_Control(pDX, IDC_ADDRESS_BITMAP, m_addressBitmap);
	DDX_Control(pDX, IDC_FULLNAME_BITMAP, m_fullnameBitmap);
	DDX_Control(pDX, IDC_EXECUTABLE_BITMAP, m_executableBitmap);
	DDX_Control(pDX, IDC_LIST_ATTRIBUTES, m_attributeList);
	DDX_Control(pDX, IDC_EDIT_SUBSYSTEM, m_subsystem);
	DDX_Control(pDX, IDC_EDIT_SHORTNAME, m_shortname);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_location);
	DDX_Control(pDX, IDC_EDIT_FULLNAME, m_fullname);
	DDX_Control(pDX, IDC_EDIT_EXECUTABLE, m_executable);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_EDIT_ADDRESS, m_address);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_STATUS, m_status);
	DDX_Control(pDX, IDC_CHECK_MULTIPLE, m_checkLimitInstances);
	DDX_Control(pDX, IDC_CHECK_PASSPARAMS, m_checkPassParams);
	DDX_Control(pDX, IDC_CHECK_DISPLAY_ICON, m_checkDisplayIcon);
	DDX_Control(pDX, IDC_CHECK_CLOSE, m_checkClose);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ApplicationDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ApplicationDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
	ON_BN_CLICKED(IDC_CHECK_CLOSE, onCheckClose)
	ON_BN_CLICKED(IDC_CHECK_DISPLAY_ICON, onCheckDisplayIcon)
	ON_BN_CLICKED(IDC_CHECK_MULTIPLE, onCheckMultiple)
	ON_BN_CLICKED(IDC_CHECK_PASSPARAMS, onCheckPassparams)
	ON_EN_KILLFOCUS(IDC_EDIT_ADDRESS, onKillfocusEditAddress)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
	ON_EN_KILLFOCUS(IDC_EDIT_EXECUTABLE, onKillfocusEditExecutable)
	ON_EN_KILLFOCUS(IDC_EDIT_FULLNAME, onKillfocusEditFullname)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_SHORTNAME, onKillfocusEditShortname)
	ON_EN_CHANGE(IDC_EDIT_NAME, onChangeEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_HELP, onKillfocusEditHelp)
	ON_BN_CLICKED(IDC_CHECK_REPOSITION, onCheckReposition)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ApplicationDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_shortname.SetLimitText( SHORT_NAME_FIELD_LENGTH );
        m_fullname.SetLimitText( FULL_NAME_FIELD_LENGTH );
        m_executable.SetLimitText( EXECUTABLE_FIELD_LENGTH );
        m_name.SetLimitText( NAME_FIELD_LENGTH );
        m_description.SetLimitText( DESCRIPTION_FIELD_LENGTH );
        m_address.SetLimitText( ADDRESS_FIELD_LENGTH );

        // Hide all the Application details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ApplicationDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_SHORTNAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SHORTNAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_FULLNAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_FULLNAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_EXECUTABLE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_EXECUTABLE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_HELP)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_HELP)->ShowWindow(isShown);
        GetDlgItem(IDC_SHORTNAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_FULLNAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EXECUTABLE_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_STATUS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_MULTIPLE)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_PASSPARAMS)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_DISPLAY_ICON)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_CLOSE)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_REPOSITION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ADDRESS)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_SUBSYSTEM)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ATTRIBUTES)->ShowWindow(isShown);
        GetDlgItem(IDC_LIST_ATTRIBUTES)->ShowWindow(isShown);
        GetDlgItem(IDC_ADDRESS_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_LOCATION)->ShowWindow(isShown);
    }


    void ApplicationDetailView::enableEntitySection(bool isEnabled)
    {
        GetDlgItem(IDC_STATIC_TYPE)->EnableWindow(isEnabled);
        GetDlgItem(IDC_STATIC_NAME)->EnableWindow(isEnabled);
        GetDlgItem(IDC_EDIT_NAME)->EnableWindow(isEnabled);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->EnableWindow(isEnabled);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->EnableWindow(isEnabled);
        GetDlgItem(IDC_STATIC_ADDRESS)->EnableWindow(isEnabled);
        GetDlgItem(IDC_EDIT_ADDRESS)->EnableWindow(isEnabled);
        GetDlgItem(IDC_NAME_BITMAP)->EnableWindow(isEnabled);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->EnableWindow(isEnabled);
        GetDlgItem(IDC_STATIC_LOCATION)->EnableWindow(isEnabled);
        GetDlgItem(IDC_EDIT_LOCATION)->EnableWindow(isEnabled);
        GetDlgItem(IDC_STATIC_SUBSYSTEM)->EnableWindow(isEnabled);
        GetDlgItem(IDC_EDIT_SUBSYSTEM)->EnableWindow(isEnabled);
        GetDlgItem(IDC_STATIC_ATTRIBUTES)->EnableWindow(isEnabled);
        GetDlgItem(IDC_LIST_ATTRIBUTES)->EnableWindow(isEnabled);
        GetDlgItem(IDC_ADDRESS_BITMAP)->EnableWindow(isEnabled);
        GetDlgItem(IDC_LOCATION_BITMAP)->EnableWindow(isEnabled);
        GetDlgItem(IDC_BUTTON_LOCATION)->EnableWindow(isEnabled);
    }


    void ApplicationDetailView::refreshButtonAvailability()
    {
        if (m_currentApplication == NULL)
        {
            return;
        }

        if (m_currentApplication->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentApplication->hasChanged())
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(true);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
        }
        else
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(false);
        }    }

    
    void ApplicationDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ApplicationDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ApplicationListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ApplicationDetailView");
        }


        m_currentApplication = dynamic_cast<TA_Base_Core::IConfigGui*>(currentItem);

        if (m_currentApplication == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigGui* object

            m_shortname.SetWindowText( m_currentApplication->getName().c_str() );
            m_fullname.SetWindowText( m_currentApplication->getFullName().c_str() );
            m_executable.SetWindowText( m_currentApplication->getExecutable().c_str() );
			m_helpPath.SetWindowText( m_currentApplication->getHelpFilePath().c_str() );

            if (m_currentApplication->isIconDisplayed())
            {
                m_checkDisplayIcon.SetCheck(1);
            }
            else
            {
                m_checkDisplayIcon.SetCheck(0);
            }

            if (m_currentApplication->areInstancesLimited())
            {
                m_checkLimitInstances.SetCheck(1);
                m_checkReposition.EnableWindow(TRUE);
            }
            else
            {
                m_checkLimitInstances.SetCheck(0);
                m_checkReposition.EnableWindow(FALSE);
            }

            if (m_currentApplication->shouldPassParameters())
            {
                m_checkPassParams.SetCheck(1);
            }
            else
            {
                m_checkPassParams.SetCheck(0);
            }

            if ( m_currentApplication->shouldReposition() )
            {
                m_checkReposition.SetCheck(1);
            }
            else
            {
                m_checkReposition.SetCheck(0);
            }

            if (m_currentApplication->shouldTerminateOnLogout())
            {
                m_checkClose.SetCheck(1);
            }
            else
            {
                m_checkClose.SetCheck(0);
            }

            displayEntityDetails();
            
            if( m_currentApplication->isNew() )
            {
                // This is a new Application or at least a copy of an Application so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                m_status.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_status.SetWindowText( NO_VALUE );

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				// ++ TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentApplication->getDateModified();
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
                COleDateTime dateCreated ( m_currentApplication->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                // ++ TD12474 
            }
            
            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Application details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Application details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void ApplicationDetailView::displayEntityDetails()
    {
        FUNCTION_ENTRY("displayEntityDetails");

        TA_Base_Core::IConfigEntity* entity = NULL;
        try
        {
            entity = m_currentApplication->getRelatedEntity();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve related entity for the GUI Application");
        
            enableEntitySection(false);
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030065);
        }

        if (entity == NULL)
        {
            enableEntitySection(false);
            m_name.SetWindowText("");
            m_description.SetWindowText("");
            m_address.SetWindowText("");
            m_type.SetWindowText("");
            m_subsystem.SetWindowText("");
            m_location.SetWindowText("");

            m_attributeList.populateControl( entity );
            FUNCTION_EXIT;
            return;
        }

        enableEntitySection(true);

        m_name.SetWindowText( entity->getName().c_str() );
        m_description.SetWindowText( entity->getDescription().c_str());
        m_address.SetWindowText( entity->getAddress().c_str());
        m_type.SetWindowText( entity->getType().c_str());
        m_subsystem.SetWindowText( entity->getSubsystemName().c_str());
        m_location.SetWindowText( entity->getLocationName().c_str());

        m_attributeList.populateControl( entity );
    }

    void ApplicationDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentApplication);

        FUNCTION_EXIT;
    }


    void ApplicationDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        // Two updates must be sent for the application. One for the entity and one for the gui application
        // entry. We will ensure that we send one here for the entity and the other one (for the GUI application)
        // will be sent in the applyButtonPressed() method
        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        TA_ASSERT(m_currentApplication != NULL, "The current application pointer is NULL");

        TA_Base_Core::IConfigEntity* entity = NULL;
        try
        {
            entity = m_currentApplication->getRelatedEntity();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve related entity for the GUI Application");
        }

        // We obviously only need to send an update for the entity details if there is one. So only set up the
        // UpdateDetails structure if the entity is not NULL
        MessageSender::UpdateDetails updateDetails;
        if (entity != NULL)
        {
            updateDetails.isNewItem = entity->isNew();
            updateDetails.isDelete = false;
            updateDetails.configUpdateMessage = MessageSender::ENTITY;
            // Use the application name so the user doesn't get two different descriptions in the audit messages
            try
            {
                updateDetails.nameOfChangedItem = m_currentApplication->getName(); 
            }
            catch( ... )
            {
                updateDetails.nameOfChangedItem = "Unknown";
            }

            if (!entity->isNew())
            {
                updateDetails.changes = entity->getAllItemChanges();
            }
        }

        applyButtonPressed(m_currentApplication);

        if (m_currentApplication->hasChanged())
        {
            // Apply did not work properly so just return
            FUNCTION_EXIT;
            return;
        }
        
        // Otherwise we must now send our changes for the entity if one exists and changes have been made to it
        if( entity != NULL && !updateDetails.changes.empty() )
        {
            updateDetails.keyOfChangedItem = entity->getKey();
            getMessageSender()->sendMessagesForTheChange( updateDetails );
        }

        FUNCTION_EXIT;
    }


    void ApplicationDetailView::onCheckClose() 
    {
        FUNCTION_ENTRY("onCheckClose");

        if (m_currentApplication != NULL )
        {
	        int checked = m_checkClose.GetCheck();
            
            bool isChecked = true;
            if (checked == 0)
            {
                isChecked = false;
            }

            try
            {
                m_currentApplication->setShouldTerminateOnLogout(isChecked);
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


    void ApplicationDetailView::onCheckDisplayIcon() 
    {
        FUNCTION_ENTRY("onCheckDisplayIcon");

        if (m_currentApplication != NULL )
        {
	        int checked = m_checkDisplayIcon.GetCheck();
            
            bool isChecked = true;
            if (checked == 0)
            {
                isChecked = false;
            }

            try
            {
                m_currentApplication->setIsIconDisplayed(isChecked);
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


    void ApplicationDetailView::onCheckMultiple() 
    {
        FUNCTION_ENTRY("onCheckMultiple");

        if (m_currentApplication != NULL )
        {
	        int checked = m_checkLimitInstances.GetCheck();
            
            bool isChecked = true;
            if (checked == 0)
            {
                isChecked = false;
                m_checkReposition.EnableWindow(FALSE);
            }
            else
            {
                m_checkReposition.EnableWindow(TRUE);
            }

            try
            {
                m_currentApplication->setAreInstancesLimited(isChecked);
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


    void ApplicationDetailView::onCheckPassparams() 
    {
        FUNCTION_ENTRY("onCheckPassparams");

        if (m_currentApplication != NULL )
        {
	        int checked = m_checkPassParams.GetCheck();
            
            bool isChecked = true;
            if (checked == 0)
            {
                isChecked = false;
            }

            try
            {
                m_currentApplication->setShouldPassParameters(isChecked);
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

    void ApplicationDetailView::onCheckReposition() 
    {
        FUNCTION_ENTRY("onCheckReposition");

        if ( NULL != m_currentApplication )
        {
            int checked = m_checkReposition.GetCheck();

            try
            {
                m_currentApplication->setShouldReposition(0!=checked);
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

    void ApplicationDetailView::onKillfocusEditShortname() 
    {
        FUNCTION_ENTRY("onKillfocusEditShortname");

        // When the user changes the name we must update the IConfigGui* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentApplication != NULL )
        {
            try
            {
                CString name;
                m_shortname.GetWindowText(name);
                m_currentApplication->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentApplication->getUniqueIdentifier(), m_currentApplication->getName().c_str() );
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

   
    void ApplicationDetailView::onKillfocusEditFullname() 
    {
        FUNCTION_ENTRY("onKillfocusEditFullname");

        // When the user changes the full name we must update the IConfigLocation* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentApplication != NULL )
        {
            try
            {
                CString fullname;
                m_fullname.GetWindowText(fullname);
                m_currentApplication->setFullName( std::string(fullname) );
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


    void ApplicationDetailView::onKillfocusEditExecutable() 
    {
        FUNCTION_ENTRY("onKillfocusEditExecutable");

        // When the user changes the executable we must update the IConfigGui* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentApplication != NULL )
        {
            CString executable;
            m_executable.GetWindowText(executable);
            m_currentApplication->setExecutable( std::string(executable) );
        }
        else
        {
            FUNCTION_EXIT;
            return;
        }

        // Update the name and executable field in-case this method updated the entity name or shortened the fields
        try
        {
			// TD #2899
			// Only update the 'name' field if the related entity has been set.
			// Update the 'executable' field regardless
            TA_Base_Core::IConfigEntity* entity = m_currentApplication->getRelatedEntity();
            if (entity != NULL)
            {
	            m_name.SetWindowText( entity->getName().c_str() );
            }

            m_executable.SetWindowText( m_currentApplication->getExecutable().c_str() );
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


    void ApplicationDetailView::onKillfocusEditHelp() 
    {
        FUNCTION_ENTRY("onKillfocusEditHelp");

        // When the user changes the full name we must update the IConfigLocation* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentApplication != NULL )
        {
            try
            {
                CString helpPath;
                m_helpPath.GetWindowText(helpPath);
                m_currentApplication->setHelpFilePath( std::string(helpPath) );
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

    
    void ApplicationDetailView::onKillfocusEditAddress() 
    {
        FUNCTION_ENTRY("onKillfocusEditAddress");

        // When the user changes the address we must update the IConfigEntity* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentApplication == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            TA_Base_Core::IConfigEntity* entity = m_currentApplication->getRelatedEntity();
            if (entity == NULL)
            {
                FUNCTION_EXIT;
                return;
            }

            CString address;
            m_address.GetWindowText(address);
            entity->setAddress( std::string(address) );
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


    void ApplicationDetailView::onKillfocusEditDescription() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // When the user changes the description we must update the IConfigEntity* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentApplication == NULL )
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            TA_Base_Core::IConfigEntity* entity = m_currentApplication->getRelatedEntity();
            if (entity == NULL)
            {
                FUNCTION_EXIT;
                return;
            }

            CString description;
            m_description.GetWindowText(description);
            entity->setDescription( std::string(description) );
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


    void ApplicationDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the entity name then we must change the executable path to include the
        // new entity name. When we do this the entity name automatically gets updated.
        if (m_currentApplication != NULL )
        {
            try
            {
                CString executable;
                m_executable.GetWindowText(executable);

                CString name;
                m_name.GetWindowText(name);

                // Now we want to replace the entity name with the new entity name
                int pos = executable.Find( ENTITYNAME,0 );
                if (pos == -1)
                {
                    executable += " " + ENTITYNAME + name;
                }
                else
                {
                    int endPos = executable.Find(" ",pos);
                    if (endPos == -1)
                    {
                        // We're at the end
                        endPos = executable.GetLength();
                    }

                    int startPos = pos + ENTITYNAME.GetLength();

                    CString newExecutable = executable.Left(startPos);
                    newExecutable += name;
                    newExecutable += executable.Right(executable.GetLength() - endPos);

                    executable = newExecutable;
                }

                m_currentApplication->setExecutable( std::string(executable) );

                TA_Base_Core::IConfigEntity* entity = m_currentApplication->getRelatedEntity();
                if (entity != NULL)
                {
                    m_name.SetWindowText( entity->getName().c_str() );
                }
                m_executable.SetWindowText( m_currentApplication->getExecutable().c_str() );
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
 

    void ApplicationDetailView::onChangeEditName() 
    {
        CString name;
        m_name.GetWindowText(name);
    
        if (name.Find(' ') != -1)
        {
            // Get where the cursor currently is
            int startChar;
            int endChar;
            m_name.GetSel(startChar,endChar);
            
            // Remove the spaces
            name.Remove(' ');

            // Update the text and reset the user's cursor
            m_name.SetWindowText(name);
            m_name.SetSel(startChar,endChar);
        }
    }


    void ApplicationDetailView::onButtonLocation() 
    {
        FUNCTION_ENTRY("onButtonLocation");
	    
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
            TA_ASSERT(m_currentApplication !=  NULL, "current application is NULL.");

            // If this is a different location then set it in our IConfigApplication*.
            TA_Base_Core::IConfigEntity* entity = m_currentApplication->getRelatedEntity();
            if (entity == NULL)
            {
                FUNCTION_EXIT;
                return;
            }

            entity->setLocation( dlg.getSelectedKey() );

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


    /////////////////////////////////////////////////////////////////////////////
    // ApplicationDetailView diagnostics

    #ifdef _DEBUG
    void ApplicationDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ApplicationDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
