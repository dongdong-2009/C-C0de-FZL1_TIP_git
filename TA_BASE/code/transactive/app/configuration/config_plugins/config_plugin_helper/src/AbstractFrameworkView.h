/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a splitter. It then loads the left side of the splitter as a list of items
  * and the right side as a view for the user to configure item details.
  */

#if !defined(AFX_ABSTRACTFRAMEWORKVIEW_H__1332EF35_428C_4AA3_8B05_008BBA40FA97__INCLUDED_)
#define AFX_ABSTRACTFRAMEWORKVIEW_H__1332EF35_428C_4AA3_8B05_008BBA40FA97__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/configuration/config_plugins/config_plugin_helper/src/IMainView.h"

namespace TA_Base_App
{
    class IComponent;
    class IAllView;
    class IDetailView;

    class AbstractFrameworkView : virtual public IMainView, public CView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    AbstractFrameworkView();

    public:
        
        /**
         * setComponentInterface
         *
         * This will call the base class version of this method first. Following that it will
         * pass itself (ie this) to the AgentTreeView so that it can get us to change views
         */
        virtual void setComponentInterface(IComponent* component);


        /**
         * itemSelected
         *
         * This is called when the user has chosen a toolbar or menu item.
         *
         * @param const CString& - The name of the item selected. This should be exactly
         *                         the same as one of the strings passed in the ToolBarItem::name
         *                         structure or MenuItem::menuItem structure.
         */
        virtual void itemSelected(const CString& item);


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AbstractFrameworkView)
	    public:
	    virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	    protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    //}}AFX_VIRTUAL

    // Implementation
    protected:
	    virtual ~AbstractFrameworkView();
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(EntityFrameworkView)
	    afx_msg void OnSize(UINT nType, int cx, int cy);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    protected:

        /**
         * getRuntimeClassForListView
         *
         * This returns the runtime class for the list view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
        virtual CRuntimeClass* getRuntimeClassForListView() =0;


        /**
         * getRuntimeClassForDetailView
         *
         * This returns the runtime class for the detail view.
         *
         * @return CRuntimeClass* - The result of a RUNTIME_CLASS call on the
         *                          class name required.
         */
        virtual CRuntimeClass* getRuntimeClassForDetailView() =0;


        /**
         * getListView
         *
         * This returns the view that contains the list of all items.
         *
         * @return IAllView* - A pointer to the list view object
         */
        IAllView* getListView() const
        {
            return m_listView;
        }


        /**
         * getDetailView
         *
         * This returns the view that contains the details of a single item
         *
         * @return IDetailView* - A pointer to the detail view object
         */
        IDetailView* getDetailView() const
        {
            return m_detailView;
        }


        /**
         * getComponent
         *
         * This returns the component that holds the database accessor and message sender etc
         *
         * @return IComponent* - A pointer to the component object
         */
        IComponent* getComponent() const
        {
            return m_component;
        }


        /**
         * setListView
         *
         * This sets the protected list view member. This allows the Create method to be overwritten if
         * necessary
         */
        void setListView(IAllView& listView)
        {
            m_listView = &listView;
        };


        /**
         * setDetailView
         *
         * This sets the protected detail view member. This allows the Create method to be overwritten if
         * necessary
         */
        void setDetailView(IDetailView& detailView)
        {
            m_detailView = &detailView;
        };


        /**
         * getSplitterWindow
         *
         * This method returns a reference to the splitter window this framework created. This allows
         * derived classes to modify the splitter if necessary (ie by loading new views into it etc)
         */
        CSplitterWnd& getSplitterWindow()
        {
            return m_splitter;
        }


    public:
        // This is the width of the entity list pane in the splitter window
        static const int LIST_WIDTH;

    protected:

        // These constants are used to pass to the splitter methods so the code is more readable
        static const int LEFT_PANE;
        static const int RIGHT_PANE;
        static const int ZERO_ROW;


    private:
        CSplitterWnd m_splitter;    // This splitter separates the list of items and the detail view.

        IAllView* m_listView;       // This is the view in the left pane of the splitter
                                    // showing the list of all matching items.

        IDetailView* m_detailView;  // This is the view in the right pane of the splitter
                                    // showing the details for a single item.

        IComponent* m_component;    // This is the component we use to retrieve message senders and
                                    // database accessors etc
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_ABSTRACTFRAMEWORKVIEW_H__1332EF35_428C_4AA3_8B05_008BBA40FA97__INCLUDED_)
