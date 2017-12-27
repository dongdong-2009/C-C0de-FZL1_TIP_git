/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_colour/src/EventColourDetailView.h $
  * @author:  Derrick Liew
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a EventColour's details.
  */

// EventColourDetailView.h: interface for the EventColourDetailView class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_EVENTCOLOURDETAILVIEW_H__4A0970DD_AD4A_4896_9485_7714C51CA63C__INCLUDED_)
#define AFX_EVENTCOLOURDETAILVIEW_H__4A0970DD_AD4A_4896_9485_7714C51CA63C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include "app/configuration/config_plugins/event_colour/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/src/IEventColourData.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigEventColourData;
}


namespace TA_Base_App
{
    class EventColourDatabaseAccessor;
    class EventColourListView;


	class EventColourDetailView : public AbstractDetailView  
	{
	
	public:

		enum EEventColourState
		{
			OPEN_ACKED,
			OPEN_UNACKED,
			CLOSED_UNACKED,
			CLOSED_ACKED,
		};

    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
		EventColourDetailView();

        /**
         * Destructor
         */
		virtual ~EventColourDetailView();
        
        DECLARE_DYNCREATE(EventColourDetailView)

    // Form Data
    public:

	    //{{AFX_DATA(EventColourDetailView)
		enum { IDD = IDD_EVENT_COLOUR_VIEW };
		CComboBox	m_eventColourStateCombo;
		CTransparentImage	m_nameBitmap;
		CStatic	m_type;
		CStatic	m_modifyDate;
		CStatic	m_createDate;
		CEdit	m_name;
		CTransparentImage	m_fgColour1Bitmap;
		CTransparentImage	m_bgColour1Bitmap;
		//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the EventColour we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentEventColour);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


		// Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(EventColourDetailView)
	public:
		virtual void OnInitialUpdate();
	protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

    protected:

		#ifdef _DEBUG
			virtual void AssertValid() const;
			virtual void Dump(CDumpContext& dc) const;
		#endif


	    // Generated message map functions
	    //{{AFX_MSG(EventColourDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
		afx_msg void OnButtonFgColour1();
		afx_msg void OnButtonBgColour1();
		afx_msg void OnEditchangeEventColourState();
		afx_msg void OnSelchangeEventColourState();
		//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, EVENT_COLOUR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::EVENT_COLOUR;
        }

        /**
         *  Sets the background colour of the a colour edit field.
		 *
		 *  @param editResourceID  The resource ID of the colour edit field to change.
		 *  @param colour		   The edit field's new colour.
         */
        void EventColourDetailView::setColourEditBackground(unsigned int editResourceID, COLORREF colour);

    private:

        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        EventColourDetailView( const EventColourDetailView&);

        /**
         * EventColour =
         *
         * Hidden as a private method so it is not used.
         */
        EventColourDetailView& operator=(const EventColourDetailView&);
		
        /** 
          * getNewColourFromUser
          *
          * Prompts the user to enter a new colour by displaying a colour
		  * dialog, and then saves the new colour to the IConfigEventType
		  * object.
          *
          * @param editResourceID  The resource ID of the edit control that
		  *						   should be filled in with the new colour.
		  * @param eventColourState	   The event colour state that we're updating a colour
		  *						   for.
          */
        void getNewColourFromUser(unsigned int editResourceID, EEventColourState eventColourState);

        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the EventColour details. These controls will be
         * hidden until the user selects a EventColour from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);

		TA_Base_Core::IEventColourData::EEventColourType convertUserSelectionsToColourType(unsigned int editResourceID, EEventColourState eventColourState);

    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;


        // The brushes used to paint the Colour edit controls. Four CBrush objects are
		// needed, one for each edit box. They are keyed on the edit boxes' Resource ID.
		typedef std::map<unsigned int, CBrush*> ResourceToBrushMap;
		ResourceToBrushMap m_colourBackgroundBrushes;
        
		// The current EventColour the user is configuring.
		// This is the database object for it.
        TA_Base_Core::IConfigEventColourData* m_currentEventColour; 
																	 

        // This is the view in the left pane of the splitter
		// showing the list of all event colours. We need to use this to
		// tell it when EventColour details have changed.
		EventColourListView* m_listView;

        // The object that we can use to retrieve information from the database.
		EventColourDatabaseAccessor* m_databaseAccessor;  

	};

} //close of namespace

#endif // !defined(AFX_EVENTCOLOURDETAILVIEW_H__4A0970DD_AD4A_4896_9485_7714C51CA63C__INCLUDED_)
