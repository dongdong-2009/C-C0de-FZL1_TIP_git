/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/profile/src/ProfileDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Profile's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/profile/src/StdAfx.h"
#include "app/configuration/config_plugins/profile/src/ProfileDetailView.h"

#include "app/configuration/config_plugins/profile/src/ProfileDatabaseAccessor.h"
#include "app/configuration/config_plugins/profile/src/ProfileListView.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigProfile.h"

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
    // These match the restrictions in the database for how long certain fields can be.
    // Hopefully in the future these could be read from the database.
    const int ProfileDetailView::NAME_FIELD_LENGTH        = 85;

    // These are the keys used for local and central profiles. At the moment we have to 
    // hard-code them.
    const int ProfileDetailView::LOCAL_PROFILE    = 0;
    const int ProfileDetailView::CENTRAL_PROFILE  = 1;

    const CString ProfileDetailView::CHANGE_ACCESS_CONTROL_GROUP_DIALOG_TITLE = "更改角色控制组";
    const CString ProfileDetailView::CHANGE_RADIO_LOGIN_GROUP_DIALOG_TITLE = "更改广播登录组";
    const CString ProfileDetailView::CHANGE_PA_PRIORITY_GROUP_DIALOG_TITLE = "选择广播角色组";

    IMPLEMENT_DYNCREATE(ProfileDetailView, CFormView)


    ProfileDetailView::ProfileDetailView() : 
        m_currentProfile(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ProfileDetailView::IDD)
    {
        //{{AFX_DATA_INIT(ProfileDetailView)
	//}}AFX_DATA_INIT
    }


    ProfileDetailView::~ProfileDetailView()
    {
    }


    void ProfileDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ProfileDetailView)
	DDX_Control(pDX, IDC_CHECK_SYSTEMONLY, m_isSystemProfile);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_DISPLAY_LIST, m_displayList);
	DDX_Control(pDX, IDC_TYPE_BITMAP, m_typeBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_GROUP_BITMAP, m_groupBitmap);
	DDX_Control(pDX, IDC_TYPE1_RADIO, m_controlRadio);
	DDX_Control(pDX, IDC_TYPE2_RADIO, m_localRadio);
    DDX_Control(pDX, IDC_ACCESS_CTRL_GROUP_EDIT, m_accessGroup);
    DDX_Control(pDX, IDC_RADIO_LOGIN_GROUP_EDIT, m_radioGroup);
    DDX_Control(pDX, IDC_PA_PRIORITY_GROUP_EDIT, m_paPriorityGroup);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ProfileDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ProfileDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_BN_CLICKED(IDC_TYPE1_RADIO, onCentralRadio)
	ON_BN_CLICKED(IDC_TYPE2_RADIO, onLocalRadio)
    ON_BN_CLICKED(IDC_CHANGE_ACC_GROUP, onChangeAccessGroup)
    ON_BN_CLICKED(IDC_CHANGE_RADIO_GROUP, onChangeRadioGroup)
    ON_BN_CLICKED(IDC_CHANGE_PA_PRIORITY_GROUP, onChangePaPriorityGroup)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ProfileDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);

        // Hide all the profile details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ProfileDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PROFILETYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DISPLAYS)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_TYPE_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_GROUP_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_TYPE1_RADIO)->ShowWindow(isShown);
        GetDlgItem(IDC_TYPE2_RADIO)->ShowWindow(isShown);
        GetDlgItem(IDC_DISPLAY_LIST)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPEGROUP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SYSTEMONLY)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_SYSTEMONLY)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ACCESS_GROUP)->ShowWindow(isShown);
        //GetDlgItem(IDC_STATIC_RADIO_GROUP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PA_PRIORITY_GROUP)->ShowWindow(false);
        GetDlgItem(IDC_ACCESS_CTRL_GROUP_EDIT)->ShowWindow(isShown);
        //GetDlgItem(IDC_RADIO_LOGIN_GROUP_EDIT)->ShowWindow(isShown);
        GetDlgItem(IDC_PA_PRIORITY_GROUP_EDIT)->ShowWindow(false);
        GetDlgItem(IDC_CHANGE_ACC_GROUP)->ShowWindow(isShown);
        //GetDlgItem(IDC_CHANGE_RADIO_GROUP)->ShowWindow(isShown);
        GetDlgItem(IDC_CHANGE_PA_PRIORITY_GROUP)->ShowWindow(false);
    }


    void ProfileDetailView::updateReadOnlyFields()
    {
        FUNCTION_ENTRY("updateReadOnlyFields");

        if (m_currentProfile->isSystemProfile())
        {
            //Disable everything
            m_displayList.EnableWindow(false);
            m_controlRadio.EnableWindow(false);
            m_localRadio.EnableWindow(false);

            m_name.SetReadOnly(true);
        }
        else
        {
            // Enable everything
            m_displayList.EnableWindow(true);
            m_controlRadio.EnableWindow(true);
            m_localRadio.EnableWindow(true);

            m_name.SetReadOnly(false);
        }

        FUNCTION_EXIT;
    }


    void ProfileDetailView::refreshButtonAvailability()
    {
        if (m_currentProfile == NULL)
        {
            return;
        }

        if (m_currentProfile->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentProfile->hasChanged())
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


    void ProfileDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ProfileDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ProfileListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ProfileDetailView");

            m_displayList.setDatabaseAccessor(m_databaseAccessor);
        }


        m_currentProfile = dynamic_cast<TA_Base_Core::IConfigProfile*>(currentItem);

        if (m_currentProfile == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigProfile* object

	        m_name.SetWindowText( m_currentProfile->getName().c_str() );

            m_accessGroup.SetWindowText( m_currentProfile->getAccessControlGroupAsString().c_str() );

            m_radioGroup.SetWindowText( m_currentProfile->getItsiGroupName().c_str() );

            //m_paPriorityGroup.SetWindowText( m_currentProfile->getPaPriorityGroupName().c_str() );

            if ( LOCAL_PROFILE == m_currentProfile->getType() )
            {
                m_controlRadio.SetCheck(0);
                m_localRadio.SetCheck(1);
            }
            else
            {
                m_controlRadio.SetCheck(1);
                m_localRadio.SetCheck(0);
            }

            if (m_currentProfile->isSystemProfile())
            {
                m_isSystemProfile.SetCheck(1);
            }
            else
            {
                m_isSystemProfile.SetCheck(0);
            }

            if( m_currentProfile->isNew() )
            {
                // This is a new Profile or at least a copy of an Profile so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

				// TD12474 ++
				TA_Base_Core::DateFormat dateFormat; 
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
				// ++ TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentProfile->getDateModified();
                if (modify == 0)
                {
                    m_modifyDate.SetWindowText(NO_VALUE);
                }
                else
                {
                    COleDateTime dateModified( modify );
                    // TD12474 ++
                    //m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
                    //m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                    m_modifyDate.SetWindowText( dateModified.Format() );
                    // ++ TD12474
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_currentProfile->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                //m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                m_createDate.SetWindowText( dateCreated.Format() );
                // ++ TD12474
            }
            
            m_displayList.populateControl(m_currentProfile);

            updateReadOnlyFields();

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Profile details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Profile details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void ProfileDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigProfile* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentProfile != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentProfile->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentProfile->getUniqueIdentifier(), m_currentProfile->getName().c_str() );
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


    void ProfileDetailView::onCentralRadio() 
    {
	    TA_ASSERT(m_currentProfile != NULL, "The current profile is NULL");
        try
        {
	        m_currentProfile->setType(CENTRAL_PROFILE);
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


        // The list of displays must now change
        m_displayList.populateControl(m_currentProfile);

        refreshButtonAvailability();
    }


    void ProfileDetailView::onLocalRadio() 
    {
	    TA_ASSERT(m_currentProfile != NULL, "The current profile is NULL");

        try
        {
    	    m_currentProfile->setType(LOCAL_PROFILE);
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
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030001);
            return;
        }


        // The list of displays must now change
        m_displayList.populateControl(m_currentProfile);

        refreshButtonAvailability();
    }


    void ProfileDetailView::onChangeAccessGroup() 
    {
        FUNCTION_ENTRY("onChangeAccessGroup");
        CWaitCursor waitCursor;

        // Retrieve the current access group this profile belongs to
        CString currentAccessGroup("");
        m_accessGroup.GetWindowText( currentAccessGroup );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllAccessControlGroups();

            // Now construct the dialog and then wait for the user to
            // finish with it
            ChangeDialog dlg(
                items, CHANGE_ACCESS_CONTROL_GROUP_DIALOG_TITLE, currentAccessGroup, this);
            int response = dlg.DoModal();

            if (response != IDOK)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                             "User cancelled the selection of a new access control group");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "User selected a new access control group which is %s",
                         dlg.getSelectedName());

            m_currentProfile->setAccessControlGroup(dlg.getSelectedKey());

            // Now update the access control group details the view is showing
            m_accessGroup.SetWindowText( dlg.getSelectedName() );

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve access control group details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve access control group details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
        
        FUNCTION_EXIT;
    }


    void ProfileDetailView::onChangeRadioGroup() 
    {
        FUNCTION_ENTRY("onChangeRadioGroup");
        CWaitCursor waitCursor;

        // Retrieve the current alarm type this rule is using
        CString currentRadioGroup("");
        m_radioGroup.GetWindowText( currentRadioGroup );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllRadioLoginGroups();

            // Now construct the dialog and then wait for the user to
            // finish with it
            ChangeDialog dlg(
                items, CHANGE_RADIO_LOGIN_GROUP_DIALOG_TITLE, currentRadioGroup, this);
            int response = dlg.DoModal();

            if (response != IDOK)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                             "User cancelled the selection of a new radio login group");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                         "User selected a new radio login group which is %s",
                         dlg.getSelectedName());

            m_currentProfile->setRadioLoginGroup(dlg.getSelectedName().GetBuffer(0));

            // Now update the radio login group details the view is showing
            m_radioGroup.SetWindowText( dlg.getSelectedName() );

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve radio login group details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve radio login details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
        
        FUNCTION_EXIT;
    }

    void ProfileDetailView::onChangePaPriorityGroup()
    {
        FUNCTION_ENTRY("onChangePaPriorityGroup");
        CWaitCursor waitCursor;

        // Retrieve the current alarm type this rule is using
        CString currentPaPriorityGroup("");
        //m_paPriorityGroup.GetWindowText( currentPaPriorityGroup );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            ChangeDialog::ItemsForSelection items = m_databaseAccessor->getAllPaPriorityGroups();

            // Now construct the dialog and then wait for the user to
            // finish with it
            ChangeDialog dlg(
                items, CHANGE_PA_PRIORITY_GROUP_DIALOG_TITLE, currentPaPriorityGroup, this);
            int response = dlg.DoModal();

            if (response != IDOK)
            {
                LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                    "User cancelled the selection of a new radio login group");
                FUNCTION_EXIT;
                return;
            }

            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo,
                "User selected a new radio login group which is %s",
                dlg.getSelectedName());

            m_currentProfile->setPaPriorityGroup(dlg.getSelectedName().GetBuffer(0));

            // Now update the radio login group details the view is showing
            //m_paPriorityGroup.SetWindowText( dlg.getSelectedName() );

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve radio login group details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve radio login details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void ProfileDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentProfile);

        // We invalidate in case this reset does not cause a redraw. We need to ensure the display
        // bitmap is repainted on top.
        Invalidate();

        FUNCTION_EXIT;
    }


    void ProfileDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentProfile);

        // We invalidate in case this reset does not cause a redraw. We need to ensure the display
        // bitmap is repainted on top.
        Invalidate();

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // ProfileDetailView diagnostics

    #ifdef _DEBUG
    void ProfileDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ProfileDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
#endif //_DEBUG

}
