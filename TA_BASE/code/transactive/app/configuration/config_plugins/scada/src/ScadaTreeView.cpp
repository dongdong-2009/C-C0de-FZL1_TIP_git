/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ScadaTreeView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a tree of all items available to be configured. This view must communicate
  * with the ScadaDetailView as this will show the details of the item selected in this view.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/ScadaTreeView.h"
#include "app/configuration/config_plugins/scada/src/ChangeParentDialog.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeDialog.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/IConfigEntity.h"
#include "core/data_access_interface/src/IConfigItem.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;
using TA_Base_Bus::CEntityTreeItem;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ScadaTreeView

namespace TA_Base_App
{
    const CString ScadaTreeView::SELECT_ENTITYTYPE_DIALOG_TITLE = "Select a Type";

    // These are types of changes the user makes in the details page that we need to update the tree for
    const CString ScadaTreeView::CHANGE_OF_PARENT = "Parent";
    const CString ScadaTreeView::CHANGE_OF_NAME   = "Name";

    IMPLEMENT_DYNCREATE(ScadaTreeView, CView)

    ScadaTreeView::ScadaTreeView()
        : m_scadaTree(NULL),
          m_detailView(NULL),
          m_databaseAccessor(NULL),
          m_messageSender(NULL),
          m_statusBar(NULL),
          CView()
    {
    }


    ScadaTreeView::~ScadaTreeView()
    {
        if (m_scadaTree != NULL)
        {
            delete m_scadaTree;
            m_scadaTree = NULL;
        }
    }


    void ScadaTreeView::DoDataExchange(CDataExchange* pDX)
    {
	    CView::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ScadaTreeView)
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ScadaTreeView, CView)
	//{{AFX_MSG_MAP(ScadaTreeView)
	ON_WM_SIZE()
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, onSelchangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ScadaTreeView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    void ScadaTreeView::OnInitialUpdate() 
    {
	    CView::OnInitialUpdate();
	    
	    
    }

    void ScadaTreeView::OnSize(UINT nType, int cx, int cy) 
    {
	    CView::OnSize(nType, cx, cy);

        resizeTreeControl();
    }


    void ScadaTreeView::resizeTreeControl()
    {
        // If our window has been created then we can resize it
        if (m_scadaTree != NULL && m_scadaTree->m_hWnd != NULL)
        {
            CRect viewPosition;
            GetWindowRect(viewPosition);
            ScreenToClient(viewPosition);

            m_scadaTree->MoveWindow(viewPosition,true);
        }
    }


    void ScadaTreeView::onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
	    
        if (m_scadaTree == NULL)
        {
            m_detailView->populateItemDetails(NULL);
            return;
        }

        CObjectTreeItem* item = m_scadaTree->getSelectedItem();

        if (item == NULL)
        {
            // Nothing is selected so pass in NULL
            m_detailView->populateItemDetails(NULL);
            return;
        }

        TA_Base_Core::IEntityData* data = dynamic_cast<CEntityTreeItem*>(item)->getEntityData();

        if (data == NULL)
        {
            m_detailView->populateItemDetails(NULL);
            return;
        }

        // If this cast is not successful then NULL will be passed automatically
        m_detailView->populateItemDetails( dynamic_cast<TA_Base_Core::IConfigItem*>(data) );
	    
	    *pResult = 0;
    }


    void ScadaTreeView::itemDetailsChanged(unsigned long id, const CString& type)
    {
        FUNCTION_ENTRY("itemChanged");

        TA_ASSERT( m_scadaTree != NULL, "The SCADA tree is NULL so it cannot be updated");

        HTREEITEM currentItem = m_scadaTree->GetSelectedItem();
        if (currentItem == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        CEntityTreeItem* item = dynamic_cast<CEntityTreeItem*>(m_scadaTree->getSelectedItem());

        std::string entityName( item->getEntityData()->getName() );

        if (type == CHANGE_OF_PARENT)
        {
            reloadTreeForItem( entityName );
        }
        else if (type == CHANGE_OF_NAME)
        {
            // Datapoint names etc are normally long with '.'s separating the sections. We only want the 
            // last section so we must pull it out and update the tree control with this name
            m_scadaTree->SetItemText(currentItem, item->getDisplayName(entityName.c_str()));
        }
        else
        {
            reloadTreeForItem( entityName );
        }

        FUNCTION_EXIT;
    }


    void ScadaTreeView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        try
        {
            if (m_scadaTree != NULL)
            {
                m_scadaTree->DestroyWindow();
                delete m_scadaTree;
                m_scadaTree = NULL;
            }


            // We don't need to load all items as this is done automatically in our tree. When the tree is
            // created the items are loaded.

            m_scadaTree = new TA_Base_Bus::CScadaTreeCtrl( dynamic_cast<TA_Base_Bus::IEntityFactory*>(m_databaseAccessor) );
            if (m_scadaTree != NULL)
            {
                m_scadaTree->Create(WS_VISIBLE | WS_CHILD | WS_TABSTOP | WS_BORDER | TVS_HASBUTTONS | TVS_SHOWSELALWAYS |
                                    TVS_LINESATROOT | TVS_HASLINES | TVS_DISABLEDRAGDROP,
                                    CRect(0, 0, 10, 10), this, IDC_TREE);
            }

            // Expand the first node so we have more than just 'Scada' showing.
            m_scadaTree->Expand(m_scadaTree->GetRootItem(), TVE_EXPAND);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            FUNCTION_EXIT;
            return;
        }


        FUNCTION_EXIT;
    }


    void ScadaTreeView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        if (m_scadaTree == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Get the currently selected item
        CObjectTreeItem* item = m_scadaTree->getSelectedItem();

        if (item == NULL)
        {
            CString actionName;
            actionName.LoadString(IDS_COPY);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        TA_Base_Core::IEntityData* data = dynamic_cast<CEntityTreeItem*>(item)->getEntityData();
        TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(data);

        if (entity == NULL)
        {
            CString actionName, reason;
            actionName.LoadString(IDS_COPY);
            reason.LoadString(IDS_DATA_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            FUNCTION_EXIT;
            return;
        }

        try
        {
            TA_ASSERT(m_databaseAccessor != NULL, "Database accesssor is NULL");

            TA_Base_Core::IConfigItem* newItem = m_databaseAccessor->copyItem(entity->getUniqueIdentifier());

            if (newItem !=  NULL)
            {
                // Append something to the end of the name to indicate that this is a new item
                std::string newName = entity->getName();
                newName += AbstractDetailView::NEW_LABEL;
                newItem->setName( newName );

                // Now insert the item into the tree
                reloadTreeForItem(newName);
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


    void ScadaTreeView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        if (m_scadaTree == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        // Get the currently selected item
        CObjectTreeItem* item = m_scadaTree->getSelectedItem();

        if (item == NULL)
        {
            CString actionName;
            actionName.LoadString(IDS_DELETE);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        CString actionName;
        actionName.LoadString(IDS_DELETE);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << actionName;
        UINT response = userMsg.showMsgBox(IDS_UW_010003);
        if (response == IDNO)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the item delete");
            FUNCTION_EXIT;
            return;
        }

        CProgressCtrl& progress = m_statusBar->displayProgressBar();
        AfxGetApp()->DoWaitCursor(1);

        HTREEITEM selectedTreeItem = m_scadaTree->GetSelectedItem();
        
        int numberOfNodes = m_scadaTree->countNodes(selectedTreeItem)+1;
        progress.SetRange(1, numberOfNodes);
        progress.SetStep(1);

        // Ok now delete the item and this will traverse through the tree and delete all children
        std::vector<HTREEITEM> itemsToRemoveFromTree;
        try
        {
            deleteItem(selectedTreeItem, progress, itemsToRemoveFromTree);
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
                CString actionName, reason;
                actionName.LoadString(IDS_DELETE);
                reason.LoadString(IDS_DATA_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException","Could not delete the item");
            CString actionName, reason;
            actionName.LoadString(IDS_DELETE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }


        // Finally we must remove the deleted items from the tree
        for(std::vector<HTREEITEM>::iterator iter = itemsToRemoveFromTree.begin(); iter != itemsToRemoveFromTree.end(); ++iter)
        {
            CEntityTreeItem* scadaItem = reinterpret_cast<CEntityTreeItem*>(m_scadaTree->GetItemData(*iter));

            // Remove the item from the tree
            m_scadaTree->DeleteItem(*iter);

            // Now delete our actual database object as this is now safe we have removed the item from the tree.
            delete scadaItem->getEntityData();                
        }

        // And then cleanup
        m_statusBar->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void ScadaTreeView::deleteItem(HTREEITEM item,
                                   CProgressCtrl& progress,
                                   std::vector<HTREEITEM>& itemsToRemoveFromTree)
    {
        CString text = m_scadaTree->GetItemText(item);

        // If this item has children then we want to delete each child first
        // First expand the item to ensure it's children have been loaded
        m_scadaTree->Expand(item, TVE_TOGGLE);
        m_scadaTree->Expand(item, TVE_TOGGLE);

        HTREEITEM childTreeItem = m_scadaTree->GetChildItem(item);
        while (childTreeItem != NULL)
        {
            deleteItem(childTreeItem,progress,itemsToRemoveFromTree);

            // Move onto the next child
            childTreeItem = m_scadaTree->GetNextItem(childTreeItem, TVGN_NEXT);
        }

        // Now we can finally delete the specified item but first we must retrieve it from the tree

        CEntityTreeItem* scadaItem = reinterpret_cast<CEntityTreeItem*>(m_scadaTree->GetItemData(item));

        TA_Base_Core::IEntityData* data = scadaItem->getEntityData();
        TA_Base_Core::IConfigEntity* entity = dynamic_cast<TA_Base_Core::IConfigEntity*>(data);

        if (entity == NULL)
        {
            CString actionName, reason;
            actionName.LoadString(IDS_DELETE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            return;
        }

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

        // Store the item to be removed from the actual tree later
        itemsToRemoveFromTree.push_back(item);

        // Send audit and update messages if necessary
        if (!updateDetails.isNewItem)
        {
            TA_ASSERT(m_messageSender != NULL, "The message sender is NULL");
            m_messageSender->sendMessagesForTheChange( updateDetails );
        }

        progress.StepIt();
    }


    void ScadaTreeView::newItem()
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

        //
        // Now determine the parent of the entity
        //
        ParentDetails newParent = getParentForNewEntity();
        if (newParent.key == 0)
        {
            FUNCTION_EXIT;
            return;
        }
        
        //
        // Now create the entity and add it into the tree
        //
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            TA_Base_Core::IConfigEntity* newEntity = dynamic_cast<TA_Base_Core::IConfigEntity*>(m_databaseAccessor->newItem(newType));

            if (newEntity !=  NULL)
            {
                // Append something to the end of the name to indicate that this is a new item
                std::string newName = newParent.name;
                if (!newName.empty())
                {
                    newName += ".";
                }
                newName += AbstractDetailView::NEW_LABEL;
                newEntity->setName( newName );

                // Set the  new parent
                newEntity->setParent(newParent.key);

                // Now insert the item into the tree
                reloadTreeForItem(newName);
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


    void ScadaTreeView::reloadTreeForItem(const std::string& itemToSelect)
    {
        FUNCTION_ENTRY("reloadTreeForItem");
        
        loadAllItems();
        
        resizeTreeControl();

        m_scadaTree->setInitialDataPoint( itemToSelect );

        FUNCTION_EXIT;
    }


    std::string ScadaTreeView::getEntityTypeForNewEntity()
    {
        FUNCTION_ENTRY("getEntityTypeForNewEntity");

        std::string typeToUse("");

        std::vector<std::string>& types = m_databaseAccessor->getEntityTypes();
        if (types.size() == 1)
        {
            // If we are only representing one type then we know this is the type we should create
            FUNCTION_EXIT;
            return types[0];
        }

        
        // Otherwise we have to let the user select the entity type

        // Generate the map to pass into the dialog. We are not interested in the id as we only use the
        // name so anything can be used there
        ChangeDialog::ItemsForSelection itemsForSelection;
        for( unsigned int i = 0; i < types.size(); ++i)
        {
            itemsForSelection.insert( ChangeDialog::ItemsForSelection::value_type(i,types[i]) );
        }

        // Pass in blank for the current item since the user hasn't previously selected an entity type
        ChangeDialog dlg(itemsForSelection,SELECT_ENTITYTYPE_DIALOG_TITLE,"", this);

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


    ScadaTreeView::ParentDetails ScadaTreeView::getParentForNewEntity()
    {
        FUNCTION_ENTRY("getParentForNewEntity");

        ParentDetails details;
        details.key = 0;
        details.name = "";

        ChangeParentDialog dlg("",this);

        if (dlg.DoModal() != IDOK)
        {
            LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User cancelled the creation of a new SCADA entity");
            FUNCTION_EXIT;
            return details;
        }

        details.key = dlg.getSelectedKey();
        details.name = dlg.getSelectedName();

        LOG_GENERIC( SourceInfo, DebugUtil::DebugInfo, "User selected the parent which is %lu - %s", details.key, details.name.c_str());
        FUNCTION_EXIT;
        return details;
    }

    
    /////////////////////////////////////////////////////////////////////////////
    // ScadaTreeView diagnostics

    #ifdef _DEBUG
    void ScadaTreeView::AssertValid() const
    {
	    CView::AssertValid();
    }

    void ScadaTreeView::Dump(CDumpContext& dc) const
    {
	    CView::Dump(dc);
    }
    #endif //_DEBUG

    /////////////////////////////////////////////////////////////////////////////
    // ScadaTreeView message handlers
}
