/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/DutyDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Duty's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/action_groups/src/StdAfx.h"
#include "app/configuration/config_plugins/action_groups/src/DutyDetailView.h"

#include "app/configuration/config_plugins/action_groups/src/DutyComponent.h"
#include "app/configuration/config_plugins/action_groups/src/DutyDatabaseAccessor.h"
#include "app/configuration/config_plugins/action_groups/src/DutyListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigActionGroup.h"

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
    const int DutyDetailView::NAME_FIELD_LENGTH = 256;

    IMPLEMENT_DYNCREATE(DutyDetailView, CFormView)


    DutyDetailView::DutyDetailView() : 
        m_currentDuty(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(DutyDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(DutyDetailView)
	//}}AFX_DATA_INIT
    }


    DutyDetailView::~DutyDetailView()
    {
    }


    void DutyDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(DutyDetailView)
	DDX_Control(pDX, IDC_CHECK_CONTROL, m_controlCheck);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_LIST_ACTIONS, m_actionList);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(DutyDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(DutyDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_BN_CLICKED(IDC_CHECK_CONTROL, onCheckControl)
	ON_BN_CLICKED(IDC_BUTTON_SELECT, onButtonAddSelected)
	ON_BN_CLICKED(IDC_BUTTON_UNSELECT, onButtonRemoveSelected)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void DutyDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);

        // Hide all the Duty details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void DutyDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_LIST_ACTIONS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ACTIONS)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CONTROL)->ShowWindow(isShown);
        GetDlgItem(IDC_CHECK_CONTROL)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_SELECT)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_UNSELECT)->ShowWindow(isShown);
    }


    void DutyDetailView::refreshButtonAvailability()
    {
        if (m_currentDuty == NULL)
        {
            return;
        }

        if (m_currentDuty->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentDuty->hasChanged())
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

    
    void DutyDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentDuty)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<DutyDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<DutyListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper DutyDetailView");
        }


        m_currentDuty = dynamic_cast<TA_Base_Core::IConfigActionGroup*>(currentDuty);

        if (m_currentDuty == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigActionGroup* object

	        m_name.SetWindowText( m_currentDuty->getName().c_str() );

            if (m_currentDuty->isControlType() )
            {
                m_controlCheck.SetCheck(1);
            }
            else
            {
                m_controlCheck.SetCheck(0);
            }

			if( m_currentDuty->isNew() )
            {
                // This is a new Duty or at least a copy of an Duty so we indicate this in the type
                // field and make sure the modify and create dates are blank.
                m_type.SetWindowText(NEW_LABEL);
                m_modifyDate.SetWindowText(NO_VALUE);
	            m_createDate.SetWindowText(NO_VALUE);
            }
            else
            {
                m_type.SetWindowText( NO_VALUE );

				TA_Base_Core::DateFormat dateFormat; // TD12474

                // Retrieve the modify time and date. If this is zero then one hasn't been set yet
                // so leave the modified field as blank
                time_t modify = m_currentDuty->getDateModified();
                if (modify == 0)
                {
                    m_modifyDate.SetWindowText(NO_VALUE);
                }
                else
                {
                    COleDateTime dateModified( modify );
					// TD12474 ++
					//m_modifyDate.SetWindowText( dateModified.Format(TA_Configuration::DATE_FORMAT) );
					std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
                    //m_modifyDate.SetWindowText( dateModified.Format(dateFormatStr.c_str()) );
                    m_modifyDate.SetWindowText( dateModified.Format() );
					// ++ TD12474 
                }

                // The create time should always be set so we don't need to perform the same checks as the
                // modify date.
                COleDateTime dateCreated ( m_currentDuty->getDateCreated() );
				// TD12474 ++
				//m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
				std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat6);
                //m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
                m_createDate.SetWindowText( dateCreated.Format() );
				// ++ TD12474 
            }
            
            m_actionList.populateControl(m_currentDuty);

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Duty details to display");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Duty details to display");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void DutyDetailView::onButtonAddSelected() 
    {
        FUNCTION_ENTRY("onButtonAddSelected");
	    
        m_actionList.changeSelected(true);
        
        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void DutyDetailView::onButtonRemoveSelected() 
    {
        FUNCTION_ENTRY("onButtonRemoveSelected");

	    m_actionList.changeSelected(false);

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void DutyDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigActionGroup* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentDuty != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentDuty->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentDuty->getUniqueIdentifier(), m_currentDuty->getName().c_str() );
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


    void DutyDetailView::onCheckControl() 
    {
        FUNCTION_ENTRY("onCheckControl");

		bool isChecked = (m_controlCheck.GetCheck() == 1);
	    
        try
        {
            m_currentDuty->setIsControlType(isChecked);
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


    void DutyDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentDuty);

        FUNCTION_EXIT;
    }


    void DutyDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentDuty);

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // DutyDetailView diagnostics

    #ifdef _DEBUG
    void DutyDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void DutyDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
