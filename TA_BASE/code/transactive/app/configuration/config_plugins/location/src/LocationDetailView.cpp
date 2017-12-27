/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/location/src/LocationDetailView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Location's details.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/location/src/StdAfx.h"
#include "app/configuration/config_plugins/location/src/LocationDetailView.h"

#include "app/configuration/config_plugins/location/src/LocationDatabaseAccessor.h"
#include "app/configuration/config_plugins/location/src/LocationListView.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigLocation.h"

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
    const int LocationDetailView::NAME_FIELD_LENGTH        = 85;
    const int LocationDetailView::DESCRIPTION_FIELD_LENGTH = 1024;
    const int LocationDetailView::ORDERID_FIELD_LENGTH = 9;

    IMPLEMENT_DYNCREATE(LocationDetailView, CFormView)


    LocationDetailView::LocationDetailView() : 
        m_currentLocation(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(LocationDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(LocationDetailView)
	//}}AFX_DATA_INIT
    }


    LocationDetailView::~LocationDetailView()
    {
    }


    void LocationDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(LocationDetailView)
	DDX_Control(pDX, IDC_DESCRIPTION_BITMAP, m_descriptionBitmap);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_ORDERID_BITMAP, m_orderIdBitmap);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
	DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
	DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_DESCRIPTION, m_description);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	DDX_Control(pDX, IDC_EDIT_ORDERID, m_orderId);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(LocationDetailView, AbstractDetailView)
	//{{AFX_MSG_MAP(LocationDetailView)
	ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
	ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
	ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
	ON_EN_KILLFOCUS(IDC_EDIT_DESCRIPTION, onKillfocusEditDescription)
	ON_EN_KILLFOCUS(IDC_EDIT_ORDERID, onKillfocusEditOrderId)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void LocationDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);
        m_description.SetLimitText(DESCRIPTION_FIELD_LENGTH);
        m_orderId.SetLimitText(ORDERID_FIELD_LENGTH);

        // Hide all the Location details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void LocationDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_ORDERID)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_DESCRIPTION)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_ORDERID)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_DESCRIPTION_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_ORDERID_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        
    }


    void LocationDetailView::refreshButtonAvailability()
    {
        if (m_currentLocation == NULL)
        {
            return;
        }

        if (m_currentLocation->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentLocation->hasChanged())
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


    void LocationDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentItem)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<LocationDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<LocationListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper LocationDetailView");
        }


        m_currentLocation = dynamic_cast<TA_Base_Core::IConfigLocation*>(currentItem);

        if (m_currentLocation == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigLocation* object

	        m_name.SetWindowText( m_currentLocation->getName().c_str() );
	        m_description.SetWindowText( m_currentLocation->getDescription().c_str() );

            unsigned long orderId = m_currentLocation->getOrderId();
            std::stringstream stream;
            stream << orderId;
	        m_orderId.SetWindowText( stream.str().c_str());

            if( m_currentLocation->isNew() )
            {
                // This is a new Location or at least a copy of an Location so we indicate this in the type
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
                time_t modify = m_currentLocation->getDateModified();
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
                COleDateTime dateCreated ( m_currentLocation->getDateCreated() );
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
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve Location details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve Location details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void LocationDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigLocation* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentLocation != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentLocation->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    m_listView->itemDetailsChanged( m_currentLocation->getUniqueIdentifier(), m_currentLocation->getName().c_str() );
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


    void LocationDetailView::onKillfocusEditDescription() 
    {
        FUNCTION_ENTRY("onKillfocusEditDescription");

        // When the user changes the description we must update the IConfigLocation* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentLocation != NULL )
        {
            try
            {
                CString description;
                m_description.GetWindowText(description);
                m_currentLocation->setDescription( std::string(description) );
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
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void LocationDetailView::onKillfocusEditOrderId() 
    {
        FUNCTION_ENTRY("onKillfocusEditOrderId");

        // When the user changes the description we must update the IConfigLocation* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentLocation != NULL )
        {
            try
            {
                CString orderId;
                m_orderId.GetWindowText(orderId);
                m_currentLocation->setOrderId( atol( (LPCTSTR) orderId) );
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
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


    void LocationDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentLocation);

        FUNCTION_EXIT;
    }


    void LocationDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");

        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentLocation);

        FUNCTION_EXIT;
    }


    /////////////////////////////////////////////////////////////////////////////
    // LocationDetailView diagnostics

    #ifdef _DEBUG
    void LocationDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void LocationDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

}
