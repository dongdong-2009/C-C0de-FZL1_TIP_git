/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/agent/src/AgentTreeView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a tree of all items available to be configured. This view must communicate
  * with the AgentDetailView as this will show the details of the item selected in this view.
  */

#pragma warning(disable:4786)

#include <algorithm>

#include "app/configuration/config_plugins/agent/src/StdAfx.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeView.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeCtrl.h"
#include "app/configuration/config_plugins/agent/src/AgentTreeItem.h"
#include "app/configuration/config_plugins/agent/src/ConfigAgent.h"
#include "app/configuration/config_plugins/agent/src/EntityView.h"
#include "app/configuration/config_plugins/agent/src/AgentDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"
#include "core/data_access_interface/src/ILocation.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// AgentTreeView

namespace TA_Base_App
{
    const CString AgentTreeView::SELECT_AGENTTYPE_DIALOG_TITLE = "Select a Type";
    const CString AgentTreeView::SELECT_LOCATION_DIALOG_TITLE = "Select a Location";
    const CString AgentTreeView::SELECT_PARENT_DIALOG_TITLE = "Select a Parent";

    const std::string AgentTreeView::NO_TYPE = "<None>";

    // These are used to determine the type of change that has taken place
    const CString AgentTreeView::CHANGE_OF_PARENT_IN_TREE = "Parent";
    const CString AgentTreeView::CHANGE_OF_NAME_IN_TREE = "Name";


    IMPLEMENT_DYNCREATE(AgentTreeView, CView)

    AgentTreeView::AgentTreeView()
        : m_agentTree(NULL),
          m_detailView(NULL),
          m_databaseAccessor(NULL),
          m_messageSender(NULL),
          m_statusBar(NULL),
          m_framework(NULL),
          CView()
    {
    }


    AgentTreeView::~AgentTreeView()
    {
        if (m_agentTree != NULL)
        {
            delete m_agentTree;
            m_agentTree = NULL;
        }
    }


    void AgentTreeView::DoDataExchange(CDataExchange* pDX)
    {
	    CView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AgentTreeView)
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AgentTreeView, CView)
	//{{AFX_MSG_MAP(AgentTreeView)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, onSelchangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AgentTreeView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void AgentTreeView::OnInitialUpdate() 
    {
	    CView::OnInitialUpdate();
	    
	    
    }

    void AgentTreeView::OnSize(UINT nType, int cx, int cy) 
    {
	    CView::OnSize(nType, cx, cy);

        resizeTreeControl();
    }


    void AgentTreeView::resizeTreeControl()
    {
        // If our window has been created then we can resize it
        if( (m_agentTree != NULL) && (m_agentTree->m_hWnd != NULL) )
        {
            CRect viewPosition;
            GetWindowRect(viewPosition);
            ScreenToClient(viewPosition);

            m_agentTree->MoveWindow(viewPosition,true);
        }
    }


    void AgentTreeView::onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	    
        TA_ASSERT(m_agentTree != NULL, "The agent tree is NULL so we cannot retrieve the selected item");

        CObjectTreeItem* item = m_agentTree->getSelectedItem();

        if (item == NULL)
        {
            // Nothing is selected so pass in NULL
            m_detailView->populateItemDetails(NULL);
            return;
        }

        TA_Base_Core::IItem* data = dynamic_cast<AgentTreeItem*>(item)->getItemData();

        if (data == NULL)
        {
            m_detailView->populateItemDetails(NULL);
            return;
        }
        else if ( dynamic_cast<TA_Base_Core::ILocation*>(data) != NULL )
        {
            // This is a location which means we don't have any detail for the right view.
            m_detailView->populateItemDetails(NULL);
            return;
        }
        else if ( dynamic_cast<ConfigAgent*>(data) != NULL)
        {
            if ( dynamic_cast<AgentDetailView*>(m_detailView) == NULL)
            {
                // We have to change the view
                m_framework->changeDetailView( RUNTIME_CLASS(AgentDetailView) );
            }
            // After this call our m_detailView will have been updated with the new view
        }
        else if (dynamic_cast<TA_Base_Core::IConfigEntity*>(data) != NULL)
        {
            if ( dynamic_cast<EntityView*>(m_detailView) == NULL)
            {
                // We have to change the view
                m_framework->changeDetailView( RUNTIME_CLASS(EntityView) );
            }
            // After this call our m_detailView will have been updated with the new view
        }

        // If this cast is not successful then NULL will be passed automatically
        m_detailView->populateItemDetails( dynamic_cast<TA_Base_Core::IConfigItem*>(data) );
	    
	    *pResult = 0;
    }


    void AgentTreeView::itemDetailsChanged(unsigned long id, const CString& type)
    {
        FUNCTION_ENTRY("itemChanged");

        TA_ASSERT(m_agentTree != NULL, "The agent tree is NULL so we cannot retrieve the selected item");

        HTREEITEM currentItem = m_agentTree->GetSelectedItem();
        if (currentItem == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        AgentTreeItem* item = dynamic_cast<AgentTreeItem*>(m_agentTree->getSelectedItem());


        if (type == CHANGE_OF_PARENT_IN_TREE)
        {
            unsigned long uniqueIdentifier = dynamic_cast<TA_Base_Core::IConfigItem*>(item->getItemData())->getUniqueIdentifier();
            reloadTreeForItem( uniqueIdentifier );
        }
        else if (type == CHANGE_OF_NAME_IN_TREE)
        {
            std::string entityDisplayName( item->getItemData()->getName() );
            m_agentTree->SetItemText(currentItem, entityDisplayName.c_str());
        }
        else
        {
            unsigned long uniqueIdentifier = dynamic_cast<TA_Base_Core::IConfigItem*>(item->getItemData())->getUniqueIdentifier();
            reloadTreeForItem( uniqueIdentifier );
        }

        FUNCTION_EXIT;
    }


    void AgentTreeView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        CProgressCtrl& progress = m_statusBar->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.
            
        AfxGetApp()->DoWaitCursor(1);

        try
        {
            // Tell the database accessor to load all items from the database
            m_databaseAccessor->loadItems();
            progress.SetPos(80);

            if (m_agentTree != NULL)
            {
                m_agentTree->DestroyWindow();
                delete m_agentTree;
                m_agentTree = NULL;
            }

            // We don't need to load all items as this is done automatically in our tree. When the tree is
            // created the items are loaded.

            m_agentTree = new AgentTreeCtrl( *m_databaseAccessor );
            if (m_agentTree != NULL)
            {
                m_agentTree->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |
                                    TVS_LINESATROOT | TVS_HASLINES | TVS_DISABLEDRAGDROP,
                                    CRect(0, 0, 10, 10), this, IDC_TREE);
            
                m_agentTree->populateTree();
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            m_statusBar->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            m_statusBar->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }

        m_statusBar->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void AgentTreeView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        if (m_agentTree == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Get the currently selected item
        CObjectTreeItem* item = m_agentTree->getSelectedItem();

        CString actionName;
        actionName.LoadString(IDS_COPY);
        if (item == NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        TA_Base_Core::IItem* data = dynamic_cast<AgentTreeItem*>(item)->getItemData();
        if (dynamic_cast<TA_Base_Core::ILocation*>(data) != NULL)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            TA_ASSERT(m_databaseAccessor != NULL, "Database accesssor is NULL");

            TA_Base_Core::IConfigItem* configData = dynamic_cast<TA_Base_Core::IConfigItem*>(data);
            if (configData == NULL)
            {
                CString action, reason;
                action.LoadString(IDS_COPY);
                reason.LoadString(IDS_DATA_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << action.GetBuffer(action.GetLength()) << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
                FUNCTION_EXIT;
                return;
            }

            TA_Base_Core::IConfigItem* newItem = m_databaseAccessor->copyItem(configData->getUniqueIdentifier());

            if (newItem !=  NULL)
            {
                // Append something to the end of the name to indicate that this is a new item
                std::string newName = data->getName();
                newName += AbstractDetailView::NEW_LABEL;
                newItem->setName( newName );

                // Now insert the item into the tree
                reloadTreeForItem(newItem->getUniqueIdentifier());
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not copy the item");
            CString actionName, reason;
            actionName.LoadString(IDS_COPY);
            reason.LoadString(IDS_DATA_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not copy the item");
            CString actionName, reason;
            actionName.LoadString(IDS_COPY);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        FUNCTION_EXIT;
    }


    void AgentTreeView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        if (m_agentTree == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Get the currently selected item
        CObjectTreeItem* item = m_agentTree->getSelectedItem();

        CString actionName;
        actionName.LoadString(IDS_DELETE);
        TA_Base_Bus::TransActiveMessage userMsg;

        if (item == NULL)
        {
            userMsg << actionName;
            userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        userMsg << actionName;
        UINT response = userMsg.showMsgBox(IDS_UW_010003);
        if (response == IDNO)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the item delete");
            FUNCTION_EXIT;
            return;
        }

        std::vector<HTREEITEM> itemsToDeleteFromTree;

        CProgressCtrl& progress = m_statusBar->displayProgressBar();
        AfxGetApp()->DoWaitCursor(1);

        try
        {
            // Our largest hierarchy in this tree is:
            // LOCATION
            //   |_ AGENT
            //        |_ ENTITY
            //
            // So as long as we allow for these three levels we'll be able to delete everything if necessary

            HTREEITEM selectedTreeItem = m_agentTree->GetSelectedItem();
            progress.SetRange(1, m_agentTree->countNodes(selectedTreeItem)+1);
            HTREEITEM childTreeItem = m_agentTree->GetChildItem(selectedTreeItem);

            // Get the selected item.
            AgentTreeItem* level1Item = reinterpret_cast<AgentTreeItem*>(m_agentTree->GetItemData(selectedTreeItem));
            m_databaseAccessor->safeToDeleteItem( level1Item->getItemData() );

            while (childTreeItem != NULL)
            {
                AgentTreeItem* level2Item = reinterpret_cast<AgentTreeItem*>(m_agentTree->GetItemData(childTreeItem));
                m_databaseAccessor->safeToDeleteItem( level2Item->getItemData() );

                // Check if the item has children and delete them. Then delete the item itself
                HTREEITEM secondChildTreeItem = m_agentTree->GetChildItem(childTreeItem);
                while (secondChildTreeItem != NULL)
                {
                    AgentTreeItem* level3Item = reinterpret_cast<AgentTreeItem*>(m_agentTree->GetItemData(secondChildTreeItem));
                    m_databaseAccessor->safeToDeleteItem( level3Item->getItemData() );

                    // Delete the item
                    deleteAnItem(level3Item->getItemData());
                    progress.StepIt();
                    
                    // Add this to our list so it is removed from the tree later
                    itemsToDeleteFromTree.push_back(secondChildTreeItem);

                    // Move onto the next child
                    secondChildTreeItem = m_agentTree->GetNextItem(secondChildTreeItem, TVGN_NEXT);
                }

                // Delete the item
                deleteAnItem(level2Item->getItemData());
                progress.StepIt();

                // Add this to our list so it is removed from the tree later
                itemsToDeleteFromTree.push_back(childTreeItem);

                // Move onto the next child
                childTreeItem = m_agentTree->GetNextItem(childTreeItem, TVGN_NEXT);
            }

            // If the current item is not a location then we must delete that as well
            TA_Base_Core::IItem* data = dynamic_cast<AgentTreeItem*>(item)->getItemData();

            if( NULL == dynamic_cast<TA_Base_Core::ILocation*>(data) )
            {
                deleteAnItem(data);
                progress.StepIt();
                itemsToDeleteFromTree.push_back(selectedTreeItem);
            }
        }
        catch ( const TA_Base_Core::DataException& ex)
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not delete the item");
            if (ex.getFailType() == TA_Base_Core::DataException::CANNOT_DELETE)
            {
                CString actionName, reason;
                actionName.LoadString(IDS_DELETE);
                reason.LoadString(IDS_REMOVE_LINKED_ITEM);
                reason += ex.getWhichData().c_str();
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                userMsg.showMsgBox(IDS_UE_030060);
            }
            else
            {
                CString reason;
                reason.LoadString(IDS_DATA_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not delete the item");
                CString reason;
                reason.LoadString(IDS_DATABASE_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        // Remove the items from the actual tree
        for(std::vector<HTREEITEM>::iterator iter = itemsToDeleteFromTree.begin(); iter != itemsToDeleteFromTree.end(); ++iter)
        {
            AgentTreeItem* agentItem = reinterpret_cast<AgentTreeItem*>(m_agentTree->GetItemData(*iter));

            // Remove the item from the tree
            m_agentTree->DeleteItem(*iter);

            // Now delete our actual database object as this is now safe we have removed the item from the tree.
            delete agentItem->getItemData();                
        }

        m_statusBar->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void AgentTreeView::newItem()
    {
        FUNCTION_ENTRY("newItem");

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL.");

        //
        // First determine the entity type to use for the new entity
        //
        std::string newType = getEntityTypeForNewEntity();
        if (newType.empty())
        {
            FUNCTION_EXIT;
            return;
        }

        try
        {
            // If the entity type the user has chosen is NO_TYPE or is not one of the types in 
            // the types held by the database accessor then the user wishes to create a new agent
            if( (newType == NO_TYPE) ||
                (std::find(m_databaseAccessor->getEntityTypes().begin(),
                           m_databaseAccessor->getEntityTypes().end(),newType) == 
                                                                      m_databaseAccessor->getEntityTypes().end())
              )
            {
                createNewAgent(newType);
            }
            else
            {
                createNewEntity(newType);
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not create a new item");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATA_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not create a new item");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        FUNCTION_EXIT;
    }


    void AgentTreeView::reloadTreeForItem(unsigned long identifierOfItemToSelect)
    {
        FUNCTION_ENTRY("reloadTreeForItem");
        
        loadAllItems();
        
        resizeTreeControl();

        m_agentTree->setInitialSelection( identifierOfItemToSelect );

        FUNCTION_EXIT;
    }


    void AgentTreeView::deleteAnItem(TA_Base_Core::IItem* item)
    {
        FUNCTION_ENTRY("deleteAnItem");

        ConfigAgent* agent = dynamic_cast<ConfigAgent*>(item);
        if (agent != NULL)
        {
            deleteAgent(agent);
            FUNCTION_EXIT;
            return;
        }

        TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(item);
        if (entity != NULL)
        {
            deleteEntity(entity);
            FUNCTION_EXIT;
            return;
        }

        FUNCTION_EXIT;
    }


    void AgentTreeView::deleteAgent(ConfigAgent* agent)
    {
        FUNCTION_ENTRY("deleteAgent");

        TA_ASSERT(agent != NULL, "The agent passed in was NULL");

        // Set up the separate message senders for all the bits and pieces
        MessageSender::UpdateDetails auditUpdateDetails;
        auditUpdateDetails.isNewItem = agent->isNew();
        if (!auditUpdateDetails.isNewItem)
        {
            auditUpdateDetails.isDelete = true;
            auditUpdateDetails.keyOfChangedItem = agent->getKey();
            auditUpdateDetails.nameOfChangedItem = agent->getName();
            auditUpdateDetails.configUpdateMessage = MessageSender::ENTITY;
        }

        MessageSender::UpdateDetails mpUpdateDetails;
        mpUpdateDetails.isNewItem = agent->isNew();
        if (!mpUpdateDetails.isNewItem)
        {
            mpUpdateDetails.isDelete = true;
            mpUpdateDetails.keyOfChangedItem = agent->getMonitoredProcessEntity().getKey();
            mpUpdateDetails.nameOfChangedItem = agent->getMonitoredProcessEntity().getName();
            mpUpdateDetails.configUpdateMessage = MessageSender::ENTITY;
        }

        MessageSender::UpdateDetails agentUpdateDetails;
        if(agent->doesAgentEntityExist())
        {
            agentUpdateDetails.isNewItem = agent->isNew();
            if (!agentUpdateDetails.isNewItem)
            {
                agentUpdateDetails.isDelete = true;
                agentUpdateDetails.keyOfChangedItem = agent->getAgentEntity()->getKey();
                agentUpdateDetails.nameOfChangedItem = agent->getAgentEntity()->getName();
                agentUpdateDetails.configUpdateMessage = MessageSender::ENTITY;
            }

        }

        // Remove the item from our database store
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
        m_databaseAccessor->deleteItem(agent->getUniqueIdentifier());

        // Remove the item from the actual tree
//        HTREEITEM treeItem = m_agentTree->GetSelectedItem();
//        m_agentTree->DeleteItem(treeItem);

        // Send audit and update messages if necessary

        TA_ASSERT(m_messageSender != NULL, "The message sender is NULL");
        if (!auditUpdateDetails.isNewItem)
        {
            m_messageSender->sendAuditMessagesForTheChange( auditUpdateDetails );
        }
        if (!mpUpdateDetails.isNewItem)
        {
            m_messageSender->sendOnlineUpdateMessagesForTheChange(mpUpdateDetails);
        }
        if ( agent->doesAgentEntityExist() && !agentUpdateDetails.isNewItem )
        {
            m_messageSender->sendOnlineUpdateMessagesForTheChange(agentUpdateDetails);
        }

        // We delete the entity ourselves because it had to remain active until the item had been
        // removed from the tree
//        delete agent;
//        agent = NULL;

        FUNCTION_EXIT;
    }


    void AgentTreeView::deleteEntity(TA_Base_Core::IConfigEntity* entity)
    {
        FUNCTION_ENTRY("deleteEntity");

        TA_ASSERT(entity != NULL, "The entity passed in was NULL");

        // Set up the separate message senders for all the bits and pieces
        MessageSender::UpdateDetails updateDetails;
        updateDetails.isNewItem = entity->isNew();
        if (!updateDetails.isNewItem)
        {
            updateDetails.isDelete = true;
            updateDetails.keyOfChangedItem = entity->getKey();
            updateDetails.nameOfChangedItem = entity->getName();
            updateDetails.configUpdateMessage = MessageSender::ENTITY;
        }

        // Remove the item from our database store
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
        m_databaseAccessor->deleteItem(entity->getUniqueIdentifier());

        // Remove the item from the actual tree
//        HTREEITEM treeItem = m_agentTree->GetSelectedItem();
//        m_agentTree->DeleteItem(treeItem);

        // Send audit and update messages if necessary

        TA_ASSERT(m_messageSender != NULL, "The message sender is NULL");
        if (!updateDetails.isNewItem)
        {
            m_messageSender->sendMessagesForTheChange( updateDetails );
        }

        // We delete the entity ourselves because it had to remain active until the item had been
        // removed from the tree
//        delete entity;
//        entity = NULL;

        FUNCTION_EXIT;
    }


    void AgentTreeView::createNewAgent(const std::string& type)
    {
        FUNCTION_ENTRY("createNewAgent");

        // Now determine the location of the entity
        LocationDetails newLocation = getLocationForNewEntity();
        if (newLocation.name.empty())
        {
            FUNCTION_EXIT;
            return;
        }
        
        // Now create the entity and add it into the tree
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_Core::IConfigItem* newItem;
        if (type == NO_TYPE)
        {
            newItem = m_databaseAccessor->newItem("");
        }
        else
        {
            newItem = m_databaseAccessor->newItem(type);
        }

         

        if (newItem !=  NULL)
        {
            // Append something to the end of the name to indicate that this is a new item
            std::string newName = AbstractDetailView::NEW_LABEL;
            newItem->setName( newName );

            ConfigAgent* agent = dynamic_cast<ConfigAgent*>(newItem);
            if (agent != NULL)
            {
                // Set the new location
                agent->setLocation(newLocation.key);
            }

            // Now insert the item into the tree
            reloadTreeForItem(newItem->getUniqueIdentifier());
        }

        FUNCTION_EXIT;
    }


    void AgentTreeView::createNewEntity(const std::string& type)
    {
        FUNCTION_ENTRY("createNewEntity");

        // Now create the entity and add it into the tree
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        TA_Base_Core::IConfigItem* newItem = m_databaseAccessor->newItem(type);

        if (newItem !=  NULL)
        {
            // Append something to the end of the name to indicate that this is a new item
            std::string newName = AbstractDetailView::NEW_LABEL;
            newItem->setName( newName );

            TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(newItem);
            if (entity != NULL)
            {
                // Now get the user to select a parent for this entity so we can put it in the tree
                unsigned long parentKey = getParentForNewEntity(*entity);
                
                if (parentKey == 0)
                {
                    // User has cancelled the operation so delete what we have done and return
                    m_databaseAccessor->deleteItem( entity->getUniqueIdentifier() );
                    FUNCTION_EXIT;
                    return;
                }
                // Set the new location
                entity->setParent(parentKey);

                try
                {
                    // Now try to retrieve that parent so we can set the location key as well
                    ConfigAgent* agent = m_databaseAccessor->getAgentFromKey( parentKey );
                    if (agent != NULL)
                    {
                        entity->setLocation(agent->getLocation());
                    }
                }
                catch ( ... )
                {
                    // If anything fails while attempting to set the location we will just leave it unset
                    LOG_EXCEPTION_CATCH(SourceInfo, "Database", "Could not set the location key on the new entity");
                }
            }

            // Now insert the item into the tree
            reloadTreeForItem(newItem->getUniqueIdentifier());
        }

        FUNCTION_EXIT;
    }


    std::string AgentTreeView::getEntityTypeForNewEntity()
    {
        FUNCTION_ENTRY("getEntityTypeForNewEntity");

        std::string typeToUse("");

        std::vector<std::string> types = m_databaseAccessor->getAllEntityTypes();
        types.push_back(NO_TYPE);


        // We have to let the user select the entity type

        // Generate the map to pass into the dialog. We are not interested in the id as we only use the
        // name so anything can be used there
        ChangeDialog::ItemsForSelection itemsForSelection;
        for( unsigned int i = 0; i < types.size(); ++i)
        {
            itemsForSelection.insert( ChangeDialog::ItemsForSelection::value_type(i,types[i]) );
        }

        // Pass in blank for the current item since the user hasn't previously selected an entity type
        ChangeDialog dlg(itemsForSelection,SELECT_AGENTTYPE_DIALOG_TITLE,"", this);

        if (dlg.DoModal() != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the creation of a new SCADA entity");
            FUNCTION_EXIT;
            return typeToUse;
        }

        typeToUse = dlg.getSelectedName();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the entity type which is %s", typeToUse.c_str());
        FUNCTION_EXIT;
        return typeToUse;
    }


    AgentTreeView::LocationDetails AgentTreeView::getLocationForNewEntity()
    {
        FUNCTION_ENTRY("getParentForNewEntity");

        LocationDetails details;
        details.key = 0;
        details.name = "";

        // We have to let the user select the location

        // Generate the map to pass into the dialog. We are not interested in the id as we only use the
        // name so anything can be used there
        ChangeDialog::ItemsForSelection itemsForSelection = m_databaseAccessor->getAllLocations();

        // Pass in blank for the current item since the user hasn't previously selected an entity type
        ChangeDialog dlg(itemsForSelection,SELECT_LOCATION_DIALOG_TITLE,"", this);

        if (dlg.DoModal() != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the creation of a new agent entity");
            FUNCTION_EXIT;
            return details;
        }

        details.key = dlg.getSelectedKey();
        details.name = dlg.getSelectedName();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the location which is %lu - %s", details.key, details.name.c_str());//TD14337 yezh++
        FUNCTION_EXIT;
        return details;
    }


    unsigned long AgentTreeView::getParentForNewEntity(TA_Base_Core::IConfigEntity& entity)
    {
        FUNCTION_ENTRY("getParentForNewEntity");

        unsigned long parentKey = 0;

        ChangeDialog::ItemsForSelection itemsForSelection = entity.getPossibleParentEntities();

        // Pass in blank for the current item since the user hasn't previously selected an entity type
        ChangeDialog dlg(itemsForSelection,SELECT_PARENT_DIALOG_TITLE,"", this);

        if (dlg.DoModal() != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the creation of a new entity");
            FUNCTION_EXIT;
            return parentKey;
        }

        parentKey = dlg.getSelectedKey();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the parent which is %lu - %s", parentKey, dlg.getSelectedName());

        FUNCTION_EXIT;
        return parentKey;
    }


    /////////////////////////////////////////////////////////////////////////////
    // AgentTreeView diagnostics

    #ifdef _DEBUG
    void AgentTreeView::AssertValid() const
    {
	    CView::AssertValid();
    }

    void AgentTreeView::Dump(CDumpContext& dc) const
    {
	    CView::Dump(dc);
    }
    #endif //_DEBUG

    /////////////////////////////////////////////////////////////////////////////
    // AgentTreeView message handlers
}
