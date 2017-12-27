/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada_alias/src/ScadaAliasListView.h $
  * @author:  Wu Mintao
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all entities available to be configured. This view must communicate
  * with the  as this will show the details of the item selected in this view.
  */

#if !defined(AFX_SCADAALIASLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_SCADAALIASLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/scada_alias/src/ScadaAliasListColumns.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class ScadaAliasListView : public AbstractListView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    ScadaAliasListView();


	    DECLARE_DYNCREATE(ScadaAliasListView)

	    virtual ~ScadaAliasListView();

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
         * This will create a new entity. If this component represents several entity types then the
         * user must select which entity type they want to create.
         */
        void newItem();

	    /**
	      * getCurrentDisplayItemsMap
	      * 
	      * return the current map of display items
	      * 
	      * @return std::map<unsigned long,ScadaAliasDatabaseAccessor::ScadaAliasRecord>* 
	      * 
	      * @exception <exceptions> Optional
	      */
        std::map<unsigned long,ScadaAliasDatabaseAccessor::ScadaAliasRecord>* getCurrentDisplayItemsMap() { return &m_currentDisplayedItems; }
    // Operations
    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ScadaAliasListView)
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
	    //{{AFX_MSG(ScadaAliasListView)
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
         * This should return the type of configuration update. ie SCADAALIAS, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const
        {
            //todo: use the correct message type
            return MessageSender::OPC_TAG_ALIAS;
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
         * @param LPARAM - This is a pointer to this object. ie the PaStationListView object.
         */
        static int CALLBACK compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl);
    
        static const bool ASCENDING;
        static const bool DESCENDING;

        HICON m_componentIcon;
        bool m_sortOrder;   // This indicates which order we are currently sorting in.
        unsigned int m_sortColumn;  // This indicates which column is currently sorted.

        // This holds all the items currently being displayed. We keep this up-to-date so we can use it to
        // sort the columns.
        std::map<unsigned long,ScadaAliasDatabaseAccessor::ScadaAliasRecord> m_currentDisplayedItems;

        ScadaAliasListColumns m_columnHelper;

    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_SCADAALIASLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
