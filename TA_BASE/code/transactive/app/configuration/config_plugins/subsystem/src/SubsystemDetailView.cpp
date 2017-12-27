/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/subsystem/src/SubsystemDetailView.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Subsystem's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/subsystem/src/StdAfx.h"
#include "app/configuration/config_plugins/subsystem/src/SubsystemDetailView.h"

#include "app/configuration/config_plugins/subsystem/src/SubsystemDatabaseAccessor.h"
#include "app/configuration/config_plugins/subsystem/src/SubsystemListView.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigSubsystem.h"

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
    const int SubsystemDetailView::NAME_FIELD_LENGTH        = 80;

    IMPLEMENT_DYNCREATE(SubsystemDetailView, CFormView)


    SubsystemDetailView::SubsystemDetailView() : 
        m_currentSubsystem(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(SubsystemDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(SubsystemDetailView)
	//}}AFX_DATA_INIT
    }


    SubsystemDetailView::~SubsystemDetailView()
    {
    }


    void SubsystemDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(SubsystemDetailView)
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
    DDX_Control(pDX, IDC_EXCLUSIVE_BITMAP, m_exclusiveBitmap);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
    DDX_Control(pDX, IDC_CHECK_EXCLUSIVE, m_exclusiveCheck);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(SubsystemDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(SubsystemDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
    ON_BN_CLICKED(IDC_CHECK_EXCLUSIVE, onCheckExclusive)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void SubsystemDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);

        // Hide all the Subsystem details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void SubsystemDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_EXCLUSIVE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_EXCLUSIVE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_EXCLUSIVE_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
    }


    void SubsystemDetailView::refreshButtonAvailability()
    {
        if (m_currentSubsystem == NULL)
        {
            return;
        }

        if (m_currentSubsystem->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentSubsystem->hasChanged())
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


    void SubsystemDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<SubsystemDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<SubsystemListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper SubsystemDetailView");
        }


        m_currentSubsystem = dynamic_cast<TA_Base_Core::IConfigSubsystem*>(currentItem);

        if (m_currentSubsystem == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigSubsystem* object

	        m_name.SetWindowText( m_currentSubsystem->getName().c_str() );

            //                         Should the checkbox be checked?     yes  no
            m_exclusiveCheck.SetCheck( m_currentSubsystem->isExclusive() ? 1  : 0 );


            if( m_currentSubsystem->isPhysical() )
            {
                m_name.SetReadOnly(false);
                m_exclusiveCheck.EnableWindow(true);
            }
            else
            {
                m_name.SetReadOnly(true);
                m_exclusiveCheck.EnableWindow(false);
            }


            if( m_currentSubsystem->isNew() )
            {
                // This is a new Subsystem or at least a copy of an Subsystem so we indicate this in the type
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
                time_t modify = m_currentSubsystem->getDateModified();
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
                COleDateTime dateCreated ( m_currentSubsystem->getDateCreated() );
                // TD12474 ++
                //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
                //m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                m_createDate.SetWindowText( dateCreated.Format() );
                // ++ TD12474
            }
            
            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Subsystem details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Subsystem details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void SubsystemDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigSubsystem* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentSubsystem != NULL )
        {
            try
            {
                // Non-physical subsystems cannot be modified
                if( ! m_currentSubsystem->isPhysical() )
                {
                    return;
                }

                CString name;
                m_name.GetWindowText(name);

                TA_ASSERT(m_currentSubsystem->isPhysical(),
                          "Non-physical subsystems cannot be modified.");

                m_currentSubsystem->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged(
                        m_currentSubsystem->getUniqueIdentifier(),
                        m_currentSubsystem->getName().c_str() );
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


    void SubsystemDetailView::onCheckExclusive()
    {
	    TA_ASSERT(m_currentSubsystem != NULL, "The current message type is NULL");

        TA_ASSERT(m_currentSubsystem->isPhysical(),
                  "Non-physical subsystems cannot be modified.");

        try
        {
            int checked = m_exclusiveCheck.GetCheck();
            if (checked == 1)
            {
                m_currentSubsystem->setExclusiveControlStatus(true);
            }
            else
            {
                m_currentSubsystem->setExclusiveControlStatus(false);
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

        refreshButtonAvailability();
    }



    void SubsystemDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentSubsystem);

        FUNCTION_EXIT;
    }


    void SubsystemDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentSubsystem);

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // SubsystemDetailView diagnostics

    #ifdef _DEBUG
    void SubsystemDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void SubsystemDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}

