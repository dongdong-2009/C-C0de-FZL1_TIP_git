/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocListView.h $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all event plan associations available to be configured. This view must
  * communicate with the EventPlanAssocDetailView as this will show the details of the item selected in this view.
  */

#if !defined(_EVENT_PLAN_ASSOC_LIST_VIEW_H_)
#define _EVENT_PLAN_ASSOC_LIST_VIEW_H_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocDatabaseAccessor.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class EventPlanAssocListView : public AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    EventPlanAssocListView();

	    virtual ~EventPlanAssocListView();

	    DECLARE_DYNCREATE(EventPlanAssocListView)


    // Operations
    public:

        //
        // Overrides from AbstractListView
        //

        /**
         * loadAllItems
         *
         * This retrieves all the items and populates the display list for the user.
         * PreCondition: setDatabaseAccessor has been called
         */
        virtual void loadAllItems( );

        
        /**
         * itemDetailsChanged
         *
         * This method is called when an items details change. If any of the fields change
         * we must update our list with the new details.
         *
         * @param unsigned long - The id of the object that has changed
         * @param CString - The name of the object. This will be empty and can be ignored
         */
        virtual void itemDetailsChanged(unsigned long id, const CString& name);


        /**
         * copyItem
         *
         * This requires an item to be selected. It will then copy that item and set it to be the
         * newly selected item.
         */
        virtual void copyItem();


        /**
         * newItem
         *
         * This will create a new item.
         */
        virtual void newItem();

        
        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(EventPlanAssocListView)
	    public:
	    virtual void OnInitialUpdate();
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
	    //{{AFX_MSG(EventPlanAssocListView)
    	afx_msg void OnSize(UINT nType, int cx, int cy);
	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void onColumnclick(NMHDR* pNMHDR, LRESULT* pResult);
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
            return MessageSender::EVENT_PLAN_ASSOCIATION;
        }

        
    private:
	  /**
	   * updateHeaderSort
	   * 
	   * This method updates the column header when clicked on, to indicate 
	   * the current sort status i.e. ascending/descending.
	   *
	   * @param The column index
	   */
	   void updateHeaderSort(const unsigned int column);


       /**
        * compareItems
        *
        * This compares two items at a time to determine whether they should be swapped or not. This
        * function is passed to the CListCtrl, SortItems() method.
        *
        * @param LPARAM - This is the associated data of the first item. This is an index into the
        *                 m_actions vector
        * @param LPARAM - This is the associated data of the second item. This is an index into the
        *                 m_actions vector
        * @param LPARAM - This is a pointer to this object. ie the EventPlanAssocListView object.
        */
       static int CALLBACK compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl);


        static const int APPLIED_COLUMN;
        static const int ENABLED_COLUMN;
        static const int EVENTTYPE_COLUMN;
        static const int ENTITYTYPE_COLUMN;
        static const int ENTITY_COLUMN;
        static const int PLAN_COLUMN;

        static const CString APPLIED_COLUMN_NAME;
        static const CString ENABLED_COLUMN_NAME;
        static const CString EVENTTYPE_COLUMN_NAME;
        static const CString ENTITYTYPE_COLUMN_NAME;
        static const CString ENTITY_COLUMN_NAME;
        static const CString PLAN_COLUMN_NAME;

        static const int APPLIED_COLUMN_WIDTH;
        static const int ENABLED_COLUMN_WIDTH;
        static const int EVENTTYPE_COLUMN_WIDTH;
        static const int ENTITYTYPE_COLUMN_WIDTH;
        static const int ENTITY_COLUMN_WIDTH;
        static const int PLAN_COLUMN_WIDTH;

        static const bool ASCENDING;
        static const bool DESCENDING;

        // These are the possible values for the status of each event plan association
        static const CString NEW;
        static const CString MODIFIED;
        static const CString UPTODATE;

        static const CString YES;
        static const CString NO;

        HICON m_componentIcon;

        bool m_sortOrder;   // This indicates which order we are currently sorting in.
        unsigned int m_sortColumn;  // This indicates which column is currently sorted.

        // This holds all the items currently being displayed. We keep this up-to-date so we can use it to
        // sort the columns.
        std::map<unsigned long,EventPlanAssocDatabaseAccessor::EventPlanAssociation> m_currentDisplayedItems;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(_EVENT_PLAN_ASSOC_LIST_VIEW_H_)
