/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_severity/src/AlarmSeverityDetailView.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a AlarmSeverity's details.
  */

#if !defined(AFX_AlarmSeverityDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_AlarmSeverityDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/alarm_severity/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "core/data_access_interface/src/IAlarmSeverityData.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigAlarmSeverityData;
}


namespace TA_Base_App
{
    class AlarmSeverityDatabaseAccessor;
    class AlarmSeverityListView;

    class AlarmSeverityDetailView : public AbstractDetailView
    {

	public:

		enum EAlarmState
		{
			OPEN_ACKED,
			OPEN_UNACKED,
			CLOSED_UNACKED,
			SELECTED_OPEN_ACKED,
			SELECTED_OPEN_UNACKED,
			SELECTED_CLOSED_UNACKED
		};


    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    AlarmSeverityDetailView();


        /**
         * Destructor
         */
	    virtual ~AlarmSeverityDetailView();

        
        DECLARE_DYNCREATE(AlarmSeverityDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(AlarmSeverityDetailView)
	enum { IDD = IDD_ALARM_SEVERITY_VIEW };
	CComboBox	m_alarmStateCombo;
	CTransparentImage	m_nameBitmap;
	CStatic	m_type;
    CStatic	m_modifyDate;
    CStatic	m_createDate;
	CEdit	m_name;
	CTransparentImage	m_fgColour1Bitmap;
	CTransparentImage	m_fgColour2Bitmap;
	CTransparentImage	m_bgColour1Bitmap;
	CTransparentImage	m_bgColour2Bitmap;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the AlarmSeverity we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentAlarmSeverity);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AlarmSeverityDetailView)
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
	    //{{AFX_MSG(AlarmSeverityDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
        afx_msg HBRUSH OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor);
	afx_msg void OnButtonFgColour1();
	afx_msg void OnButtonFgColour2();
	afx_msg void OnButtonBgColour1();
	afx_msg void OnButtonBgColour2();
	afx_msg void OnEditchangeAlarmState();
	afx_msg void OnSelchangeAlarmState();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, ALARM_SEVERITY etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::ALARM_SEVERITY;
        }

        /**
         *  Sets the background colour of the a colour edit field.
		 *
		 *  @param editResourceID  The resource ID of the colour edit field to change.
		 *  @param colour		   The edit field's new colour.
         */
        void AlarmSeverityDetailView::setColourEditBackground(unsigned int editResourceID, 
																COLORREF colour);

    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        AlarmSeverityDetailView( const AlarmSeverityDetailView&);

        /**
         * AlarmSeverity =
         *
         * Hidden as a private method so it is not used.
         */
        AlarmSeverityDetailView& operator=(const AlarmSeverityDetailView&);
		
        /** 
          * getNewColourFromUser
          *
          * Prompts the user to enter a new colour by displaying a colour
		  * dialog, and then saves the new colour to the IConfigAlarmType
		  * object.
          *
          * @param editResourceID  The resource ID of the edit control that
		  *						   should be filled in with the new colour.
		  * @param alarmState	   The alarm state that we're updating a colour
		  *						   for.
          */
        void getNewColourFromUser(unsigned int editResourceID, EAlarmState alarmState);

        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the AlarmSeverity details. These controls will be
         * hidden until the user selects a AlarmSeverity from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);

		TA_Base_Core::IAlarmSeverityData::EColourType convertUserSelectionsToColourType(unsigned int editResourceID, EAlarmState alarmState);

    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int SOUND_FIELD_LENGTH;


        // The title of the select sound dialog
        static const char* SELECT_SOUND_DIALOG_TITLE;


        // The brushes used to paint the Colour edit controls. Four CBrush objects are
		// needed, one for each edit box. They are keyed on the edit boxes' Resource ID.
		typedef std::map<unsigned int, CBrush*> ResourceToBrushMap;
		ResourceToBrushMap m_colourBackgroundBrushes;
        

        TA_Base_Core::IConfigAlarmSeverityData* m_currentAlarmSeverity;   // The current AlarmSeverity the user is configuring.
																	 // This is the database object for it.

        AlarmSeverityListView* m_listView; // This is the view in the left pane of the splitter
										   // showing the list of all alarm severities. We need to use this to
										   // tell it when AlarmSeverity details have changed.

        AlarmSeverityDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
															// from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_AlarmSeverityDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
