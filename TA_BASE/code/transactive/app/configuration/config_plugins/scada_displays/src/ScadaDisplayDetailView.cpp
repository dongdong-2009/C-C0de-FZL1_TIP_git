/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_displays/src/ScadaDisplayDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a ScadaDisplay's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada_displays/src/StdAfx.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayDetailView.h"

#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayDatabaseAccessor.h"
#include "app/configuration/config_plugins/scada_displays/src/ScadaDisplayListView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigScadaDisplay.h"

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
    const int ScadaDisplayDetailView::NAME_FIELD_LENGTH = 80;
    const int ScadaDisplayDetailView::PATH_FIELD_LENGTH = 1024;

    // This is the title to display in the dialog that allows the user to change the location
    const CString ScadaDisplayDetailView::CHANGE_LOCATION_DIALOG_TITLE = "Change Location";


    IMPLEMENT_DYNCREATE(ScadaDisplayDetailView, CFormView)


    ScadaDisplayDetailView::ScadaDisplayDetailView() : 
        m_currentScadaDisplay(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(ScadaDisplayDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(ScadaDisplayDetailView)
	//}}AFX_DATA_INIT
    }


    ScadaDisplayDetailView::~ScadaDisplayDetailView()
    {
    }


    void ScadaDisplayDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ScadaDisplayDetailView)
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_LOCATION_BITMAP, m_locationBitmap);
	DDX_Control(pDX, IDC_PATH_BITMAP, m_pathBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_EDIT_LOCATION, m_location);
	DDX_Control(pDX, IDC_EDIT_PATH, m_path);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ScadaDisplayDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(ScadaDisplayDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_PATH, onKillfocusEditPath)
	ON_BN_CLICKED(IDC_BUTTON_LOCATION, onButtonLocation)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ScadaDisplayDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_path.SetLimitText(PATH_FIELD_LENGTH);

        // Hide all the ScadaDisplay details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void ScadaDisplayDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_PATH)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_PATH)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_LOCATION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_PATH_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_LOCATION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        
    }


    void ScadaDisplayDetailView::refreshButtonAvailability()
    {
        if (m_currentScadaDisplay == NULL)
        {
            return;
        }

        if (m_currentScadaDisplay->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentScadaDisplay->hasChanged())
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


    void ScadaDisplayDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateScadaDisplayDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<ScadaDisplayDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<ScadaDisplayListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper ScadaDisplayDetailView");
        }


        m_currentScadaDisplay = dynamic_cast<TA_Base_Core::IConfigScadaDisplay*>(currentItem);

        if (m_currentScadaDisplay == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigScadaDisplay* object

	        m_name.SetWindowText( m_currentScadaDisplay->getName().c_str() );
	        m_path.SetWindowText( m_currentScadaDisplay->getPath().c_str() );
            m_location.SetWindowText( m_currentScadaDisplay->getLocationName().c_str() );

            if( m_currentScadaDisplay->isNew() )
            {
                // This is a new ScadaDisplay or at least a copy of an ScadaDisplay so we indicate this in the type
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
                time_t modify = m_currentScadaDisplay->getDateModified();
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
                COleDateTime dateCreated ( m_currentScadaDisplay->getDateCreated() );
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve ScadaDisplay details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve ScadaDisplay details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void ScadaDisplayDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigScadaDisplay* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentScadaDisplay != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentScadaDisplay->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentScadaDisplay->getUniqueIdentifier(), m_currentScadaDisplay->getName().c_str() );
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


    void ScadaDisplayDetailView::onKillfocusEditPath() 
    {
        FUNCTION_ENTRY("onKillfocusEditPath");

        // When the user changes the path we must update the IConfigScadaDisplay* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentScadaDisplay != NULL )
        {
            try
            {
                CString path;
                m_path.GetWindowText(path);
                m_currentScadaDisplay->setPath( std::string(path) );
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


    void ScadaDisplayDetailView::onButtonLocation() 
    {
	    FUNCTION_ENTRY("onButtonLocation");
	 
        // Retrieve the current location this display is linked to
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
            TA_ASSERT(m_currentScadaDisplay !=  NULL, "current scada display is NULL.");

            // If this is a different location then set it in our IConfigScadaDisplay*.
            m_currentScadaDisplay->setLocation( dlg.getSelectedKey() );

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


    void ScadaDisplayDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentScadaDisplay);

        FUNCTION_EXIT;
    }


    void ScadaDisplayDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentScadaDisplay);

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // ScadaDisplayDetailView diagnostics

    #ifdef _DEBUG
    void ScadaDisplayDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void ScadaDisplayDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
