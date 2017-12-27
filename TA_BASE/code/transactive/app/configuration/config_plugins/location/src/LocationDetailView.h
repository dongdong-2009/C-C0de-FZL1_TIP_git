/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/location/src/LocationDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Location's details.
  */

#if !defined(AFX_LocationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_LocationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/location/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigLocation;
}


namespace TA_Base_App
{
    class LocationDatabaseAccessor;
    class LocationListView;


    class LocationDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    LocationDetailView();


        /**
         * Destructor
         */
	    virtual ~LocationDetailView();

        
        DECLARE_DYNCREATE(LocationDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(LocationDetailView)
	    enum { IDD = IDD_LOCATION_VIEW };
	    CTransparentImage	m_descriptionBitmap;
	    CTransparentImage	m_nameBitmap;
	    CTransparentImage	m_orderIdBitmap;
	    CButton	m_type;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
	    CEdit	m_description;
	    CEdit	m_name;
	    CEdit	m_orderId;
	    //}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the item we want to display. We will use this to
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
	    //{{AFX_VIRTUAL(LocationDetailView)
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
	    //{{AFX_MSG(LocationDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	    afx_msg void onKillfocusEditName();
	    afx_msg void onKillfocusEditDescription();
	    afx_msg void onKillfocusEditOrderId();
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
            return MessageSender::LOCATION;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        LocationDetailView( const LocationDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        LocationDetailView& operator=(const LocationDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Location details. These controls will be
         * hidden until the user selects a Location from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;
        static const int ORDERID_FIELD_LENGTH;


        TA_Base_Core::IConfigLocation* m_currentLocation;   // The current Location the user is configuring.
                                                   // This is the database object for it.

        LocationListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all locations. We need to use this to
                                    // tell it when Location details have changed.

        LocationDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_LocationDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
