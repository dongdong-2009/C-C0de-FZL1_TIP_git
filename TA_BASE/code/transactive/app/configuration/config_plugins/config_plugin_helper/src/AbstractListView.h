/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all items available to be configured. This view must communicate
  * with the AbstractDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_ABSTRACTLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
#define AFX_ABSTRACTLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_

#include <afxcview.h>
#include <string>

#include "app/configuration/config_plugins/config_plugin_helper/src/IAllView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/MessageSender.h"
#include <set>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Core
{
    class IConfigItem;
}

namespace TA_Base_App
{
    class AbstractDetailView;
    class IDatabaseAccessor;
    class IProgressStatusBar;


    class AbstractListView : public CListView, public IAllView
    {
        struct CurrentState
        {
            int m_index;
            TA_Base_Core::IConfigItem* m_item;
            CurrentState() { clear(); }
            bool empty() { return -1 == m_index && NULL == m_item; }
            void save(int index, TA_Base_Core::IConfigItem* item) { m_index = index; m_item = item; }
            void clear() { m_index = -1; m_item = NULL; }
        };

    protected:

        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    AbstractListView();

	    virtual ~AbstractListView();

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
            m_detailView = detailView;
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
            m_databaseAccessor = databaseAccessor;
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
         * we must update our list with the new name.
         *
         * @param unsigned long - The id of the object that has changed
         * @param CString - The name of the object. This may be new
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


    // Operations
    public:

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(AbstractListView)
	    virtual void OnInitialUpdate();
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

    #ifdef _DEBUG
	    virtual void AssertValid() const;
	    virtual void Dump(CDumpContext& dc) const;
    #endif

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(AbstractListViews)
	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	    afx_msg void onItemchanged(NMHDR* pNMHDR, LRESULT* pResult);
    	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


        IDetailView* getDetailView() const
        {
            return m_detailView;
        }


        IDatabaseAccessor* getDatabaseAccessor() const
        {
            return m_databaseAccessor;
        }


        MessageSender* getMessageSender() const
        {
            return m_messageSender;
        }


        IProgressStatusBar* getStatusBar() const
        {
            return m_statusBar;
        }


        /**
         * getComponentIcon
         *
         * This should return the component icon for items to be shown in the list
         *
         * @return HICON - The icon to be shown
         */
        virtual HICON getComponentIcon() = 0;


        /**
         * getConfigMessageType
         *
         * This should return the type of configuration update. ie ENTITY, OPERATOR etc.
         *
         * @return ConfigUpdateType - The type of component this update is for
         */
        virtual MessageSender::ConfigUpdateType getConfigMessageType() const =0;

        std::string getNewName( const std::string& name, const std::string& prefix = "新建" );
        std::string getCopyName( const std::string& name, const std::string& suffix = " - 副本" );

    private:
        
        // This is the difference between the size of our window and the size of the column in
        // the list control. We need a small difference so the scroll bar does not appear.
        static const int MARGIN_BUFFER;


        IDetailView* m_detailView;   // This is the view in the right pane of the splitter
                                            // showing the details for a single item. We need to use this to
                                            // tell it which item the user has selected in our list.

        IDatabaseAccessor* m_databaseAccessor; // The object that we can use to retrieve information
                                               // from the database.

        MessageSender* m_messageSender; // The object we can use to send messages when an update is made

        IProgressStatusBar* m_statusBar; // The status bar to display our progress in.

        CImageList m_listIcons;      // This list will contain the icons to use for the items
        std::set<std::string> m_names;
        static std::map<IDatabaseAccessor*, CurrentState> s_currentState;
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_ABSTRACTLISTVIEW_H__D8199CFB_594F_4ED1_9A1E_632168E5BF11__INCLUDED_)
