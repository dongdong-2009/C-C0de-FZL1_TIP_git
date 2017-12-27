/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/operator/src/OperatorDetailView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view allows the user to configure a Operator's details.
  */

#if !defined(AFX_OperatorDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
#define AFX_OperatorDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/operator/src/resource.h"
#include "app/configuration/config_plugins/operator/src/ProfileListCtrl.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/TransparentImage.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

namespace TA_Base_Core
{
    class IConfigOperator;
}


namespace TA_Base_App
{
    class OperatorDatabaseAccessor;
    class OperatorListView;


    class OperatorDetailView : public AbstractDetailView
    {
    protected:
        /**
         * Constructor
         *
         * protected constructor used by dynamic creation
         */
	    OperatorDetailView();


        /**
         * Destructor
         */
	    virtual ~OperatorDetailView();

        
        DECLARE_DYNCREATE(OperatorDetailView)


    // Form Data
    public:
	    //{{AFX_DATA(OperatorDetailView)
	enum { IDD = IDD_OPERATOR_VIEW };
	CButton	m_isSystemOnly;
	CTransparentImage	m_descriptionBitmap;
	CTransparentImage	m_passwordBitmap;
	CTransparentImage	m_nameBitmap;
	ProfileListCtrl	m_profileList;
	CEdit	m_password;
	CButton	m_type;
	CStatic	m_modifyDate;
	CStatic	m_createDate;
	CEdit	m_description;
	CEdit	m_name;
	//}}AFX_DATA


        /**
         * populateItemDetails
         *
         * This will populate all fields in the view using the database object passed in.
         *
         * @param IConfigItem* - The database object for the Operator we want to display. We will use this to
         *                         set new values and apply. If this is NULL then the details should all be hidden
         */
        void populateItemDetails(TA_Base_Core::IConfigItem* currentOperator);


        /**
         * refreshButtonAvailability
         *
         * This should be called after any modification to the fields. This will enable or disable
         * the reset and apply buttons based on whether or not any data has changed.
         */
        virtual void refreshButtonAvailability();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(OperatorDetailView)
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
	    //{{AFX_MSG(OperatorDetailView)
	    afx_msg void onButtonReset();
	    afx_msg void onButtonApply();
	afx_msg void onKillfocusEditName();
	afx_msg void onKillfocusEditDescription();
	afx_msg void onButtonPassword();
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
            return MessageSender::OPERATOR;
        }

    
    private:
        /**
         * Copy Constructor
         *
         * Hidden as a private method so it is not used.
         */
        OperatorDetailView( const OperatorDetailView&);


        /**
         * Operator =
         *
         * Hidden as a private method so it is not used.
         */
        OperatorDetailView& operator=(const OperatorDetailView&);


        /**
         * showItemDetails
         *
         * This will show or hide all the controls for the Operator details. These controls will be
         * hidden until the user selects a Operator from the list.
         *
         * @param bool - This indicates if the controls are shown or not. true will display the
         *               controls and false will hide them
         */
        virtual void showItemDetails(bool isShown);


        /**
         * updateReadOnlyFields
         *
         * This will look at the type of operator and enable or disable the fields as appropriate.
         * This is based on whether the operator is a system operator or not.
         */
        void updateReadOnlyFields();

        std::string getHash(const std::string& str );

    private:

        // These match the restrictions in the database for how long certain fields can be.
        // Hopefully in the future these could be read from the database.
        static const int NAME_FIELD_LENGTH;
        static const int DESCRIPTION_FIELD_LENGTH;


        TA_Base_Core::IConfigOperator* m_currentOperator;   // The current Operator the user is configuring.
                                                   // This is the database object for it.

        OperatorListView* m_listView; // This is the view in the left pane of the splitter
                                    // showing the list of all operators. We need to use this to
                                    // tell it when Operator details have changed.

        OperatorDatabaseAccessor* m_databaseAccessor;  // The object that we can use to retrieve information
                                                     // from the database.
    };
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

}


#endif // !defined(AFX_OperatorDetailView_H__4046FE10_EABC_4CF5_B97A_F54C40A0D214__INCLUDED_)
