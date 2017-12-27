/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourDetailView.cpp $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a EventColour's details.
  */

// EventColourDetailView.cpp: implementation of the EventColourDetailView class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_colour/src/StdAfx.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourDetailView.h"

#include "app/configuration/config_plugins/event_colour/src/EventColourComponent.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourDatabaseAccessor.h"
#include "app/configuration/config_plugins/event_colour/src/EventColourListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigEventColourData.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DataConfigurationException.h"

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/DateFormat.h" // TD12474 

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // These match the restrictions in the database for how long certain fields can be.
    const int EventColourDetailView::NAME_FIELD_LENGTH        = 80;

    IMPLEMENT_DYNCREATE(EventColourDetailView, CFormView)


	//////////////////////////////////////////////////////////////////////
	// Construction/Destruction
	//////////////////////////////////////////////////////////////////////
    EventColourDetailView::EventColourDetailView() : 
        m_currentEventColour(NULL),
        m_listView(NULL),
        m_databaseAccessor(NULL),
        AbstractDetailView(EventColourDetailView::IDD)
    {
	    //{{AFX_DATA_INIT(EventColourDetailView)
		//}}AFX_DATA_INIT

		// Initialise the four brushes to NULL.
		m_colourBackgroundBrushes[IDC_EDIT_FG_COLOUR1] = NULL;
		m_colourBackgroundBrushes[IDC_EDIT_BG_COLOUR1] = NULL;
    }


	EventColourDetailView::~EventColourDetailView()
	{
	}

	void EventColourDetailView::DoDataExchange(CDataExchange* pDX)
    {
	    CFormView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(EventColourDetailView)
	DDX_Control(pDX, IDC_BITMAP_FG_COLOUR1, m_fgColour1Bitmap);
	DDX_Control(pDX, IDC_BITMAP_BG_COLOUR1, m_bgColour1Bitmap);
	DDX_Control(pDX, IDC_EVENT_COLOUR_STATE, m_eventColourStateCombo);
	DDX_Control(pDX, IDC_NAME_BITMAP, m_nameBitmap);
	DDX_Control(pDX, IDC_STATIC_TYPE, m_type);
    DDX_Control(pDX, IDC_STATIC_MODIFYDATE, m_modifyDate);
    DDX_Control(pDX, IDC_STATIC_CREATEDATE, m_createDate);
	DDX_Control(pDX, IDC_EDIT_NAME, m_name);
	//}}AFX_DATA_MAP
    }


	BEGIN_MESSAGE_MAP(EventColourDetailView, AbstractDetailView)
		//{{AFX_MSG_MAP(EventColourDetailView)
		ON_BN_CLICKED(IDC_BUTTON_RESET, onButtonReset)
		ON_BN_CLICKED(IDC_BUTTON_APPLY, onButtonApply)
		ON_EN_KILLFOCUS(IDC_EDIT_NAME, onKillfocusEditName)
		ON_WM_CTLCOLOR()
		ON_BN_CLICKED(IDC_BUTTON_FG_COLOUR1, OnButtonFgColour1)
		ON_BN_CLICKED(IDC_BUTTON_BG_COLOUR1, OnButtonBgColour1)
		ON_CBN_EDITCHANGE(IDC_EVENT_COLOUR_STATE, OnEditchangeEventColourState)
		ON_CBN_SELCHANGE(IDC_EVENT_COLOUR_STATE, OnSelchangeEventColourState)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


    void EventColourDetailView::OnInitialUpdate() 
    {
        FUNCTION_ENTRY("OnInitialUpdate");

	    CFormView::OnInitialUpdate();
	    
        // Limit the number of characters that can be entered into fields so that they match the database schema.
        m_name.SetLimitText(NAME_FIELD_LENGTH);

		m_eventColourStateCombo.SetCurSel(0);

        // Hide all the EventColour details until we are told to populate.
	    showItemDetails(false);

        FUNCTION_EXIT;
    }


    void EventColourDetailView::showItemDetails(bool isShown)
    {
        // Show or hide all the controls from the view
        GetDlgItem(IDC_STATIC_TYPE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_NAME)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_CREATEDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_MODIFYDATE)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_CREATED)->ShowWindow(isShown);
        GetDlgItem(IDC_STATIC_DATE_MODIFIED)->ShowWindow(isShown);
        GetDlgItem(IDC_EDIT_NAME)->ShowWindow(isShown);
		GetDlgItem(IDC_STATIC_EVENT_COLOUR_STATE)->ShowWindow(isShown);
		GetDlgItem(IDC_EVENT_COLOUR_STATE)->ShowWindow(isShown);

		GetDlgItem(IDC_STATIC_FG_COLOUR1)->ShowWindow(isShown);
		GetDlgItem(IDC_STATIC_BG_COLOUR1)->ShowWindow(isShown);

        GetDlgItem(IDC_EDIT_FG_COLOUR1)->ShowWindow(isShown);
		GetDlgItem(IDC_EDIT_BG_COLOUR1)->ShowWindow(isShown);

        GetDlgItem(IDC_BUTTON_FG_COLOUR1)->ShowWindow(isShown);
		GetDlgItem(IDC_BUTTON_BG_COLOUR1)->ShowWindow(isShown);

		GetDlgItem(IDC_BITMAP_FG_COLOUR1)->ShowWindow(isShown);
		GetDlgItem(IDC_BITMAP_BG_COLOUR1)->ShowWindow(isShown);

        GetDlgItem(IDC_NAME_BITMAP)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_RESET)->ShowWindow(isShown);
        GetDlgItem(IDC_BUTTON_APPLY)->ShowWindow(isShown);
    }


	TA_Base_Core::IEventColourData::EEventColourType EventColourDetailView::convertUserSelectionsToColourType(unsigned int editResourceID, EEventColourState eventColourState)
	{
		// This assumes that the resource IDs for the edit boxes are declared sequentially
		// in the resource file, and that the enum values in the EEventColourState enum are in the
		// same order as in the EEventColourType enum.

		int editBoxNum = editResourceID - IDC_EDIT_FG_COLOUR1;
		return static_cast<TA_Base_Core::IEventColourData::EEventColourType>(eventColourState*2 + editBoxNum);
	}


    void EventColourDetailView::refreshButtonAvailability()
    {
        if (m_currentEventColour == NULL)
        {
            return;
        }

        if (m_currentEventColour->isNew() )
        {
            GetDlgItem(IDC_BUTTON_RESET)->EnableWindow(false);
            GetDlgItem(IDC_BUTTON_APPLY)->EnableWindow(true);
            return;
        }
        
        // We have an old item
        if (m_currentEventColour->hasChanged())
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

    
    void EventColourDetailView::populateItemDetails(TA_Base_Core::IConfigItem* currentEventColour)
    {
        FUNCTION_ENTRY("populateItemDetails");

        if (m_databaseAccessor == NULL || m_listView == NULL)
        {
            m_databaseAccessor = dynamic_cast<EventColourDatabaseAccessor*>(getDatabaseAccessor());
            m_listView = dynamic_cast<EventColourListView*>(getListView());

            TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
            TA_ASSERT(m_listView != NULL, "The list view has not been set yet or is not a proper EventColourDetailView");
        }


        m_currentEventColour = dynamic_cast<TA_Base_Core::IConfigEventColourData*>(currentEventColour);

        if (m_currentEventColour == NULL)
        { 
            showItemDetails(false);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            //  Now populate all the fields with the data retrieved from the IConfigEventColourData* object

	        m_name.SetWindowText( m_currentEventColour->getName().c_str() );

            m_type.SetWindowText(NO_VALUE);

			EEventColourState eventColourState = static_cast<EEventColourState>(m_eventColourStateCombo.GetCurSel());
		
			for (ResourceToBrushMap::iterator iter = m_colourBackgroundBrushes.begin();
				iter != m_colourBackgroundBrushes.end(); iter++)
			{
				COLORREF colour = static_cast<COLORREF>(
					m_currentEventColour->getEventColourKey(
						convertUserSelectionsToColourType(iter->first, eventColourState)));
				setColourEditBackground(iter->first, colour);
			}

			// TD12474 ++
			TA_Base_Core::DateFormat dateFormat; 
			std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);
			// ++ TD12474

            // Retrieve the modify time and date.
            // If this is zero then one hasn't been set yet
            // so leave the modified field as blank
            time_t modify = m_currentEventColour->getDateModified();
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

            // The create time should always be set so we don't need to
            // perform the same checks as the modify date.
            COleDateTime dateCreated ( m_currentEventColour->getDateCreated() );
            // TD12474 ++
            //m_createDate.SetWindowText( dateCreated.Format(TA_Configuration::DATE_FORMAT) );
            m_createDate.SetWindowText( dateCreated.Format(dateFormatStr.c_str()) );
            // ++ TD12474

            showItemDetails(true);

            refreshButtonAvailability();
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve EventColour details to display");
            
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve EventColour details to display");

            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }

        FUNCTION_EXIT;
    }


    void EventColourDetailView::onKillfocusEditName() 
    {
        FUNCTION_ENTRY("onKillfocusEditName");

        // When the user changes the name we must update the IConfigEventColourData* object so that it
        // has a record of all changes when the user chooses to apply.
        if (m_currentEventColour != NULL )
        {
            try
            {
                CString name;
                m_name.GetWindowText(name);
                m_currentEventColour->setName( std::string(name) );

                if (m_listView!=NULL)
                {
                    std::string name = m_currentEventColour->getName();
                    m_listView->itemDetailsChanged( m_currentEventColour->getUniqueIdentifier(), name.c_str() );
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve event colour details to display");
            
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve event colour details to display");

                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);
            }
        }
        
        refreshButtonAvailability();
            
        FUNCTION_EXIT;
    }


    void EventColourDetailView::onButtonReset() 
    {
        FUNCTION_ENTRY("onButtonReset");

        resetButtonPressed(m_currentEventColour);

        refreshButtonAvailability();
            
        FUNCTION_EXIT;
    }


    void EventColourDetailView::onButtonApply() 
    {
	    FUNCTION_ENTRY("onButtonApply");
        
        GetDlgItem(IDC_BUTTON_APPLY)->SetFocus();

        applyButtonPressed(m_currentEventColour);

        FUNCTION_EXIT;
    }


    HBRUSH EventColourDetailView::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor)
    {
		ResourceToBrushMap::iterator iter;

        // This method is called when window controls are being painted.
        switch (nCtlColor)
        {
            // When the Edit fields are being painted
            case CTLCOLOR_EDIT:
                
                // Use a brush from m_colourBackgroundBrushes for the Colour edit boxes.

				iter = m_colourBackgroundBrushes.find(pWnd->GetDlgCtrlID());
                if(m_currentEventColour && iter != m_colourBackgroundBrushes.end())
                {
					EEventColourState eventColourState = static_cast<EEventColourState>(m_eventColourStateCombo.GetCurSel());
		            COLORREF colour = static_cast<COLORREF>(
						m_currentEventColour->getEventColourKey(
							convertUserSelectionsToColourType(iter->first, eventColourState)));
					pDC->SetBkColor(colour);
                    return (HBRUSH)(iter->second->GetSafeHandle());
                }

            // Use the default for everything else.
            default:
                return CFormView::OnCtlColor(pDC, pWnd, nCtlColor);
        }
    }


    void EventColourDetailView::setColourEditBackground(unsigned int editResourceID, COLORREF colour)
    {
        // Hide the edit, set its background color, then show it again.
        GetDlgItem(editResourceID)->ShowWindow(false);

        delete m_colourBackgroundBrushes[editResourceID];
        m_colourBackgroundBrushes[editResourceID] = new CBrush(colour);

        GetDlgItem(editResourceID)->ShowWindow(true);
    }


    void EventColourDetailView::getNewColourFromUser(unsigned int editResourceID, EEventColourState eventColourState)
    {
	    FUNCTION_ENTRY("onButtonColour");

        CColorDialog dialog;
        if(dialog.DoModal() == IDOK)
        {
            COLORREF colour = dialog.GetColor();
            setColourEditBackground(editResourceID, colour);

            try
            {
                // When the user changes the colour we must update the
                // IConfigAlarmType* object so that it has a record of all
                // changes when the user chooses to apply.
                if(m_currentEventColour != NULL)
                {
                    unsigned long colourUlong = static_cast<unsigned long>(colour);
                    m_currentEventColour->setEventColourKey(colourUlong, 
							convertUserSelectionsToColourType(editResourceID, eventColourState));
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Cannot retrieve event colour details to display");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_030001);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Cannot retrieve event colour details to display");
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg.showMsgBox(IDS_UE_010001);
            }
        }

        refreshButtonAvailability();

        FUNCTION_EXIT;
    }


	void EventColourDetailView::OnButtonFgColour1() 
	{
		EEventColourState eventColourState = static_cast<EEventColourState>(m_eventColourStateCombo.GetCurSel());
		getNewColourFromUser(IDC_EDIT_FG_COLOUR1, eventColourState); 
	}


	void EventColourDetailView::OnButtonBgColour1() 
	{
		EEventColourState eventColourState = static_cast<EEventColourState>(m_eventColourStateCombo.GetCurSel());
		getNewColourFromUser(IDC_EDIT_BG_COLOUR1, eventColourState); 		
	}


	void EventColourDetailView::OnEditchangeEventColourState() 
	{
	}

	void EventColourDetailView::OnSelchangeEventColourState() 
	{
		populateItemDetails(m_currentEventColour);	
	}

    /////////////////////////////////////////////////////////////////////////////
    // EventColourDetailView diagnostics

    #ifdef _DEBUG
    void EventColourDetailView::AssertValid() const
    {
	    AbstractDetailView::AssertValid();
    }

    void EventColourDetailView::Dump(CDumpContext& dc) const
    {
	    AbstractDetailView::Dump(dc);
    }
    #endif //_DEBUG

} //close namespace
