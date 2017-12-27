/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaTreeView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a tree of all items available to be configured. This view must communicate
  * with the ScadaDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_SCADATREEVIEW_H__CC9A5373_FA1F_4375_B030_BB4672F442E1__INCLUDED_)
#define AFX_SCADATREEVIEW_H__CC9A5373_FA1F_4375_B030_BB4672F442E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <string>

#include "app/configuration/config_plugins/scada/src/resource.h"
#include "app/configuration/config_plugins/scada/src/ScadaDetailView.h"
#include "app/configuration/config_plugins/scada/src/ScadaDatabaseAccessor.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/IAllView.h"

namespace TA_Base_Bus
{
    class CScadaTreeCtrl;
}


namespace TA_Base_App
{
    class IProgressStatusBar;
    class MessageSender;

    class ScadaTreeView : virtual public IAllView, public CView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    ScadaTreeView();

	    DECLARE_DYNCREATE(ScadaTreeView)

        /**
         * Destructor
         */
	    virtual ~ScadaTreeView();

    public:

        /**
         * setDetailView
         *
         * This method passes us the detail view so we can let it know when the user selects
         * different items. It stores this view in m_detailView.
         *
         * @param IDetailView* - The view which allows the user to configure a single item.
         */
        void setDetailView( IDetailView* detailView )
        {
            m_detailView = dynamic_cast<ScadaDetailView*>(detailView);
        }


        /**
         * setDatabaseAccessor
         *
         * This method passes us the object that allows us to retrieve entity data from the database.
         *
         * @param IDatabaseAccessor* - The object that accesses the database
         */
        void setDatabaseAccessor( IDatabaseAccessor* databaseAccessor )
        {
            m_databaseAccessor = dynamic_cast<ScadaDatabaseAccessor*>(databaseAccessor);
        }


        /**
         * setMessageSender
         *
         * This method passes us the object that allows us to send messages.
         *
         * @param MessageSender* - The object that sends the messages
         */
        void setMessageSender(MessageSender* messageSender)
        {
            m_messageSender = messageSender;
        }

        
        /**
         * setStatusBar
         *
         * This sets the status bar to use to show the progress of loading etc.
         *
         * @param IProgressStatusBar* - The status bar to use for displaying a progress bar.
         */
        void setStatusBar(IProgressStatusBar* statusBar)
        {
            m_statusBar = statusBar;
        }

        
        /**
         * itemDetailsChanged
         *
         * This method is called when an items details change. If the name of the item changes then
         * we must update our list with the new name. If the parent changes the tree needs to be redrawn.
         *
         * @param unsigned long - The id of the object that has changed
         * @param CString - The type of change that has taken place. This will be one of the public constants
         *                  declared below.
         */
        virtual void itemDetailsChanged(unsigned long id, const CString& type);


        /**
         * copyItem
         *
         * This requires an item to be selected. It will then copy that item and set it to be the
         * newly selected item.
         */
        virtual void copyItem();


        /**
         * deleteItem
         *
         * This requires an item to be selected. It will then delete the specified item from the database
         * and the list.
         */
        virtual void deleteItem();


        /**
         * newItem
         *
         * This will create a new entity. If this component represents several entity types then the
         * user must select which entity type they want to create.
         */
        virtual void newItem();


        /**
         * loadAllItems
         *
         * This retrieves all the items and populates the display list for the user.
         * PreCondition: setDatabaseAccessor has been called
         */
        virtual void loadAllItems( );



    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ScadaTreeView)
	public:
	virtual void OnInitialUpdate();
	protected:
	    virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    // Implementation
    protected:
    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
	    //{{AFX_MSG(ScadaTreeView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    
    private:
        
        struct ParentDetails
        {
            unsigned long key;
            std::string name;
        };


        /**
         * reloadTreeForItem
         *
         * When an item's parent changes or a new item is added then the tree must be reloaded
         * This method does that and then expands the tree and selects the item passed in.
         *
         * @param string - The name of the tree item to select
         */
        void reloadTreeForItem(const std::string& itemToSelect);

    
        /**
         * resizeTreeControl
         *
         * This will resize the tree control to fit into the view
         */
        void resizeTreeControl();


        /**
         * getEntityTypeForNewEntity
         *
         * This determines what the entity type will be for the new entity and returns it.
         * 
         * @return string - The new entity type. If this is blank then the user cancelled the
         *                  creation of the new entity.
         */
        std::string getEntityTypeForNewEntity();


        /**
         * getParentForNewEntity
         *
         * This determines what the parent should be for the new entity and returns it.
         *
         * @return unsigned long - The key and name of the parent. The key will be 0 if the user cancelled the
         *                         creation of the new entity.
         */
        ParentDetails getParentForNewEntity();


        /**
         * deleteItem
         *
         * This will delete the specified item an all it's children. This is a recursive method that will
         * call itself.
         *
         * @param HTREEITEM - The item to delete
         * @param CProgressCtrl& - The progress display showing how many items have been deleted. Every time
         *                         an item is deleted this should be stepped.
         * @param vector<HTREEITEM>& - Whenever an item is deleted it should be added to this list so
         *                             when we are finished we can remove all the items from the tree.
         *                             This is an in/out parameter.
         *
         * @exception DataException
         * @exception DatabaseException
         */
        void deleteItem(HTREEITEM item, 
                        CProgressCtrl& progress,
                        std::vector<HTREEITEM>& itemsToRemoveFromTree);


    public:
        // These are used to determine the type of change that has taken place
        static const CString CHANGE_OF_PARENT;
        static const CString CHANGE_OF_NAME;

    private:
        static const CString SELECT_ENTITYTYPE_DIALOG_TITLE;


	    TA_Base_Bus::CScadaTreeCtrl*  m_scadaTree; // This is the main control that will be displayed in the view

        
        ScadaDetailView* m_detailView;   // This is the view in the right pane of the splitter
                                         // showing the details for a single item. We need to use this to
                                         // tell it which item the user has selected in our list.

        ScadaDatabaseAccessor* m_databaseAccessor; // The object that we can use to retrieve information
                                                   // from the database.

        MessageSender* m_messageSender; // The object we can use to send messages when an update is made

        IProgressStatusBar* m_statusBar; // The status bar to display our progress in.

        CImageList m_listIcons;      // This list will contain the icons to use for the items
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_SCADATREEVIEW_H__CC9A5373_FA1F_4375_B030_BB4672F442E1__INCLUDED_)
