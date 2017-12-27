/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/subsystem/src/SubsystemDetailView.h $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Subsystem's details.
  */

#if !defined(AFX_SubsystemDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_SubsystemDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/subsystem/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigSubsystem;
}


namespace TA_Base_App
{
    class SubsystemDatabaseAccessor;
    class SubsystemListView;


    class SubsystemDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    SubsystemDetailView();


        /**
         * Destructor
         */
	    virtual ~SubsystemDetailView();

        
        DECLARE_DYNCREATE(SubsystemDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(SubsystemDetailView)
	    enum { IDD = IDD_SUBSYSTEM_VIEW };
	    CTransparentImage	m_nameBitmap;
	    CTransparentImage	m_exclusiveBitmap;
	    CStatic	m_type;
	    CStatic	m_modifyDate;
	    CStatic	m_createDate;
	    CEdit	m_name;
        CButton m_exclusiveCheck;
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
	    //{{AFX_VIRTUAL(SubsystemDetailView)
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
	    //{{AFX_MSG(SubsystemDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
        afx_msg void onCheckExclusive();
	    afx_msg void onKillfocusEditName();
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
            return MessageSender::SUBSYSTEM;
        }


    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        SubsystemDetailView( const SubsystemDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        SubsystemDetailView& operator=(const SubsystemDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Subsystem details. These controls will be
         * hidden until the user selects a Subsystem from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;


        TA_Base_Core::IConfigSubsystem* m_currentSubsystem;   // The current Subsystem the user is configuring.
                                                   // This is the database object for it.

        SubsystemListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all subsystems. We need to use this to
                                    // tell it when Subsystem details have changed.

        SubsystemDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.


    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_SUBSYSTEM}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_SubsystemDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
