/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentTreeView.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a tree of all items available to be configured. This view must communicate
  * with the AgentDetailView as this will show the details of the item selected in this view.
  */

#if !defined(AFX_AgentTreeView_H__CC9A5373_FA1F_4375_B030_BB4672F442E1__INCLUDED_)
#define AFX_AgentTreeView_H__CC9A5373_FA1F_4375_B030_BB4672F442E1__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include <string>

#include "app/configuration/config_plugins/agent/src/resource.h"
#include "app/configuration/config_plugins/agent/src/AgentDatabaseAccessor.h"
#include "app/configuration/config_plugins/agent/src/AgentFrameworkView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/IAllView.h"

namespace TA_Base_Core
{
    class IItem;
}

namespace TA_Base_App
{
    class IProgressStatusBar;
    class MessageSender;
    class AgentTreeCtrl;
    class IDetailView;


    class AgentTreeView : virtual public IAllView, public CView
    {
    protected:
        /**
         * Constructor
         *
         * Protected constructor used by dynamic creation
         */
	    AgentTreeView();

	    DECLARE_DYNCREATE(AgentTreeView)

        /**
         * Destructor
         */
	    virtual ~AgentTreeView();

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
            m_databaseAccessor = dynamic_cast<AgentDatabaseAccessor*>(databaseAccessor);
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
         * setFramework
         *
         * This sets the framework in this class so that it can change the view in the right side
         * of the splitter
         *
         * @param AgentFrameworkView& - The framework object
         */
        void setFramework(AgentFrameworkView& framework)
        {
            m_framework = &framework;
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
	    //{{AFX_VIRTUAL(AgentTreeView)
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
	    //{{AFX_MSG(AgentTreeView)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    
    private:
        
        struct LocationDetails
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
         * @param unsigned long - The unique identifier of the tree item to select
         */
        void reloadTreeForItem(unsigned long identifierOfItemToSelect);

    
        /**
         * resizeTreeControl
         *
         * This will resize the tree control to fit into the view
         */
        void resizeTreeControl();


        /**
         * deleteAnItem
         *
         * This will determine the time of item to delete and call the relevant delete method. Following
         * a call to this method the item should be removed from the tree and the database object deleted
         * by the caller.
         *
         * @param IItem* - The item to delete
         *
         * @exception DataException - This implies that something the user has entered is incorrect. Check
         *                            the type for full details.
         * @exception DatabaseException - This indicates something was wrong in the database and the delete
         *                                could not be performed
         */
        void deleteAnItem(TA_Base_Core::IItem* item);


        /**
         * deleteAgent
         *
         * This will delete the agent item from the database and send audit and online update
         * messages.
         *
         * @param ConfigAgent* - The item to delete
         *
         * @exception DataException - This implies that something the user has entered is incorrect. Check
         *                            the type for full details.
         * @exception DatabaseException - This indicates something was wrong in the database and the delete
         *                                could not be performed
         */
        void deleteAgent(ConfigAgent* agent);


        /**
         * deleteEntity
         *
         * This will delete the entity item from the database and send audit and online update
         * messages.
         *
         * @param IConfigEntity* - The item to delete
         *
         * @exception DataException - This implies that something the user has entered is incorrect. Check
         *                            the type for full details.
         * @exception DatabaseException - This indicates something was wrong in the database and the delete
         *                                could not be performed
         */
        void deleteEntity(TA_Base_Core::IConfigEntity* entity);


        /**
         * createNewAgent
         *
         * This will create a new agent of the entity type passed in
         *
         * @param string - The entity type of the agent to create. This can be NO_TYPE.
         */
        void createNewAgent(const std::string& type);


        /**
         * createNewEntity
         *
         * This will create a new entity of the entity type passed in
         *
         * @param string - The entity type of the entity to create.
         */
        void createNewEntity(const std::string& type);


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
         * getLocationForNewEntity
         *
         * This determines what the location should be for the new entity and returns it.
         *
         * @return LocationDetails - The key and name of the location. The name will be empty if the user cancelled the
         *                           creation of the new location.
         */
        LocationDetails getLocationForNewEntity();


        /**
         * getParentForNewEntity
         *
         * This determines what the parent should be for the new entity and returns it.
         *
         * @param IConfigEntity& - This is the new entity we can use to retrieve possible parent entities
         *
         * @return LocationDetails - The key of the new parent. This will be 0 if the user cancelled the selection of the
         *                           new parent.
         */
        unsigned long getParentForNewEntity(TA_Base_Core::IConfigEntity& entity);


    public:
        // These are used to determine the type of change that has taken place
        static const CString CHANGE_OF_PARENT_IN_TREE;
        static const CString CHANGE_OF_NAME_IN_TREE;

    private:


        // The titles for the change dialog box
        static const CString SELECT_AGENTTYPE_DIALOG_TITLE;
        static const CString SELECT_LOCATION_DIALOG_TITLE;
        static const CString SELECT_PARENT_DIALOG_TITLE;

        // For Gui's the user can select that they do not want to create a particular Gui type. This is the
        // string that will be displayed for that option.
        static const std::string NO_TYPE;

        HICON m_componentIcon;


	    AgentTreeCtrl*  m_agentTree; // This is the main control that will be displayed in the view

        
        IDetailView* m_detailView;       // This is the view in the right pane of the splitter
                                         // showing the details for a single item. We need to use this to
                                         // tell it which item the user has selected in our list.

        AgentDatabaseAccessor* m_databaseAccessor; // The object that we can use to retrieve information
                                                   // from the database.

        AgentFrameworkView* m_framework;    // The framework that holds the splitter so we can change the
                                            // detail view that is loaded

        MessageSender* m_messageSender; // The object we can use to send messages when an update is made

        IProgressStatusBar* m_statusBar; // The status bar to display our progress in.
    };

    /////////////////////////////////////////////////////////////////////////////

    //{{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}
#endif // !defined(AFX_AgentTreeView_H__CC9A5373_FA1F_4375_B030_BB4672F442E1__INCLUDED_)
