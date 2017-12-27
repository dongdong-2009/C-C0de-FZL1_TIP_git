/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_displays/src/ScadaDisplayDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Scada display's details.
  */

#if !defined(AFX_ScadaDisplayDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_ScadaDisplayDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/scada_displays/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigScadaDisplay;
}


namespace TA_Base_App
{
    class ScadaDisplayDatabaseAccessor;
    class ScadaDisplayListView;


    class ScadaDisplayDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    ScadaDisplayDetailView();


        /**
         * Destructor
         */
	    virtual ~ScadaDisplayDetailView();

        
        DECLARE_DYNCREATE(ScadaDisplayDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(ScadaDisplayDetailView)
	enum { IDD = IDD_SCADADISPLAY_VIEW };
	CStatic	m_createDate;
	CStatic	m_modifyDate;
	CButton	m_type;
	CTransparentImage	m_locationBitmap;
	CTransparentImage	m_pathBitmap;
	CTransparentImage	m_nameBitmap;
	CEdit	m_location;
	CEdit	m_path;
	CEdit	m_name;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the Scada display we want to display. We will use this to
         *                       set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentItem);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ScadaDisplayDetailView)
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
	    //{{AFX_MSG(ScadaDisplayDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	afx_msg void onKillfocusEditName();
	afx_msg void onKillfocusEditPath();
	afx_msg void onButtonLocation();
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::SCADA_DISPLAY;
        }

    
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        ScadaDisplayDetailView( const ScadaDisplayDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        ScadaDisplayDetailView& operator=(const ScadaDisplayDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the ScadaDisplay details. These controls will be
         * hidden until the user selects a Scada display from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int PATH_FIELD_LENGTH;

        // This is the title to display in the dialog that allows the user to change the location
        static const CString CHANGE_LOCATION_DIALOG_TITLE;


        TA_Base_Core::IConfigScadaDisplay* m_currentScadaDisplay;   // The current Scada display the user is configuring.
                                                               // This is the database object for it.

        ScadaDisplayListView* m_listView; // This is the view in the left pane of the splitter
                                          // showing the list of all locations. We need to use this to
                                          // tell it when Scada display details have changed.

        ScadaDisplayDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                           // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_ScadaDisplayDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
