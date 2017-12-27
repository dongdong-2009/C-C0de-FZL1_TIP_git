/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/application/src/ApplicationListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all applications available to be configured. This view must communicate
  * with the ApplicationDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_ApplicationListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_ApplicationListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class ApplicationListView : public AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    ApplicationListView();

	    virtual ~ApplicationListView();

	    DECLARE_DYNCREATE(ApplicationListView)


    // Operations
    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ApplicationListView)
	    public:
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(ApplicationListView)
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    protected:

        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon()
        {
            return m_componentIcon;
        }


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            return MessageSender::APPLICATION;
        }


        /**
         * deleteItem
         *
         * This requires an item to be selected. It will then delete the specified item from the database
         * and the list. We are overwriting the abstract version as we can't delete all GUI applications
         */
        virtual void deleteItem();


        /**
         * newItem
         *
         * This will create a new gui. The user must select which entity type they want to create.
         */
        virtual void newItem();

        
    private:
        // The titles for the change dialog box
        static const CString SELECT_GUITYPE_DIALOG_TITLE;

        // For Gui's the user can select that they do not want to create a particular Gui type. This is the
        // string that will be displayed for that option.
        static const std::string NO_TYPE;

        HICON m_componentIcon;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ApplicationListView_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
