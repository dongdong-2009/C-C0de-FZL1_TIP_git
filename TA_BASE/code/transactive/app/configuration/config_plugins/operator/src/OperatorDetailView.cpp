/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/OperatorDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Operator's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/operator/src/StdAfx.h"
#include "app/configuration/config_plugins/operator/src/OperatorDetailView.h"

#include "app/configuration/config_plugins/operator/src/OperatorComponent.h"
#include "app/configuration/config_plugins/operator/src/OperatorDatabaseAccessor.h"
#include "app/configuration/config_plugins/operator/src/OperatorListView.h"
#include "app/configuration/config_plugins/operator/src/ChangePasswordDialog.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigOperator.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 
#include <openssl/sha.h>
#include <boost/locale.hpp>


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
    const int OperatorDetailView::NAME_FIELD_LENGTH        = 85;
    const int OperatorDetailView::DESCRIPTION_FIELD_LENGTH = 80;
    const char* const DISPLAY_PASSWORD = "********";

    IMPLEMENT_DYNCREATE(OperatorDetailView, CFormView)


    OperatorDetailView::OperatorDetailView() : 
        m_currentOperator(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(OperatorDetailView::IDD)
    {
        //{{AFX_DATA_INIT(OperatorDetailView)
        //}}AFX_DATA_INIT
    }


    OperatorDetailView::~OperatorDetailView()
    {
    }


    void OperatorDetailView::DoDataExchange(CDataExchange* pDX)
    {
        CFormView::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(OperatorDetailView)
        DDX_Control(pDX, IDC_CHECK_SYSTEMONLY, m_isSystemOnly);
        DDX_Control(pDX, IDC_DESCRIPTION_BITMAP, m_descriptionBitmap);
        DDX_Control(pDX, IDC_PASSWORD_BITMAP, m_passwordBitmap);
        DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
        DDX_Control(pDX, IDC_LIST_PROFILES, m_profileList);
        DDX_Control(pDX, IDC_EDIT_PASSWORD, m_password);
        DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
        DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
        DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
        DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
        DDX_Control(pDX, IDC_EDIT_NAME, m_name);
        //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(OperatorDetailView, AbstractDetailView)
    //{{AFX_MSG_MAP(OperatorDetailView)
    ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
    ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
    ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
    ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
    ON_BN_CLICKED(IDC_BUTTON_PASSWORD, onButtonPassword)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void OperatorDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

        CFormView::OnInitialUpdate();
        
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_description.SetLimitText(DESCRIPTION_FIELD_LENGTH);

        // Hide all the Operator details until we are told to populate.
        showItemDetails(false);

        FUNCTION_EXIT;
    }


    void OperatorDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PASSWORD)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PROFILES)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PASSWORD)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_PASSWORD)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_PASSWORD_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_LIST_PROFILES)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_SYSTEMONLY)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_SYSTEMONLY)->ShowWindow(isShown);  
    }


    void OperatorDetailView::updateReadOnlyFields()
    {
        FUNCTION_ENTRY("updateReadOnlyFields");

        if (m_currentOperator->isSystemOperator())
        {
            //Disable everything
            GetDlgItem(IDC_BUTTON_PASSWORD)->EnableWindow(false);
            m_profileList.EnableWindow(false);

            m_name.SetReadOnly(true);
            m_description.SetReadOnly(true);
        }
        else
        {
            // Enable everything
            GetDlgItem(IDC_BUTTON_PASSWORD)->EnableWindow(true);
            m_profileList.EnableWindow(true);

            m_name.SetReadOnly(false);
            m_description.SetReadOnly(false);
        }

        FUNCTION_EXIT;
    }


    void OperatorDetailView::refreshButtonAvailability()
    {
        if (m_currentOperator == NULL)
        {
            return;
        }

        if (m_currentOperator->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentOperator->hasChanged())
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

    
    void OperatorDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentOperator)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<OperatorDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<OperatorListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper OperatorDetailView");
        }


        m_currentOperator = dynamic_cast<TA_Base_Core::IConfigOperator*>(currentOperator);

        if (m_currentOperator == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigOperator* object

            m_name.SetWindowText( m_currentOperator->getName().c_str() );
            m_description.SetWindowText( m_currentOperator->getDescription().c_str() );
            m_password.SetWindowText( m_currentOperator->getPassword().empty() ? "" : DISPLAY_PASSWORD );

            if (m_currentOperator->isSystemOperator())
            {
                m_isSystemOnly.SetCheck(1);
            }
            else
            {
                m_isSystemOnly.SetCheck(0);
            }

            if( m_currentOperator->isNew() )
            {
                // This is a new Operator or at least a copy of an Operator so we indicate this in the type
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
                time_t modify = m_currentOperator->getDateModified();
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
                COleDateTime dateCreated ( m_currentOperator->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                //m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                m_createDate.SetWindowText( dateCreated.Format() );
                // ++ TD12474
            }
            
            bool shouldEnable = !m_currentOperator->isSystemOperator();
            m_profileList.populateControl(m_currentOperator, shouldEnable);

            updateReadOnlyFields();

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Operator details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Operator details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void OperatorDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigOperator* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentOperator != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentOperator->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentOperator->getUniqueIdentifier(), m_currentOperator->getName().c_str() );
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


    void OperatorDetailView::onKillfocusEditDescription() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // When the user changes the description we must update the IConfigOperator* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentOperator != NULL )
        {
            try
            {
                CString description;
                m_description.GetWindowText(description);
                m_currentOperator->setDescription( std::string(description) );
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


    void OperatorDetailView::onButtonPassword() 
    {
        FUNCTION_ENTRY("onButtonPassword");
        
        // Now construct the dialog and then wait for the user to finish with it
        ChangePasswordDialog dlg;
        int response = dlg.DoModal();

        if (response != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the entering of a new password");
            FUNCTION_EXIT;
            return;
        }

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User entered a new password");

        try
        {
            TA_ASSERT(m_currentOperator !=  NULL, "current operator is NULL.");

            CString password = (const char*)dlg.getPassword();
            std::string plaintext = m_currentOperator->getName() + std::string("/") + (const char*)password;
            std::string encryptedPassword = getHash( plaintext );

            // If this is a different location then set it in our IConfigEntity*.
            m_currentOperator->setPassword( encryptedPassword );

            // Now update the location the details view is showing
            m_password.SetWindowText( password.IsEmpty() ? "" : DISPLAY_PASSWORD );
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


    void OperatorDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentOperator);

        FUNCTION_EXIT;
    }


    void OperatorDetailView::onButtonApply() 
    {
        FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentOperator);

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // OperatorDetailView diagnostics

    #ifdef _DEBUG
    void OperatorDetailView::AssertValid() const
    {
        AbstractDetailView::AssertValid();
    }

    void OperatorDetailView::Dump(CDumpContext& dc) const
    {
        AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG


    std::string OperatorDetailView::getHash( const std::string& str )
    {
        unsigned char buff[SHA_DIGEST_LENGTH] = { 0 };
        std::string utf8 =  boost::locale::conv::to_utf<char>(str, "gb18030");
        SHA1( (const unsigned char *)(utf8.c_str()), utf8.size(), buff);
        std::stringstream strm;
        strm << std::hex << std::uppercase;

        for ( int i = 0; i < SHA_DIGEST_LENGTH; i++ )
        {
            strm << std::setw(2) << std::setfill('0') << (int) (buff[i]);
        }

        return strm.str();
    }

}
