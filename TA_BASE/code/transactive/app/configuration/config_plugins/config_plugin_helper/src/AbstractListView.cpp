/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all items available to be configured. This view must communicate
  * with the AbstractDetailView as this will show the details of the item selected in this view.
  */

#pragma warning(disable:4786)

#include <set>

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractListView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractFrameworkView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/IDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/HelperResource.h"

#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/data_access_interface/src/IItem.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include <boost/lexical_cast.hpp>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;
using namespace TA_Base_App::TA_Configuration;


namespace TA_Base_App
{
   // This is the difference between the size of our window and the size of the column in
   // the list control. We need a small difference so the scroll bar does not appear.
    const int AbstractListView::MARGIN_BUFFER = 10;
    std::map<IDatabaseAccessor*, AbstractListView::CurrentState> AbstractListView::s_currentState;

    AbstractListView::AbstractListView()
        : m_detailView(NULL),
          m_databaseAccessor(NULL),
          m_messageSender(NULL),
          m_statusBar(NULL)
    {
    }


    AbstractListView::~AbstractListView()
    {
    }


BEGIN_MESSAGE_MAP(AbstractListView, CListView)
	//{{AFX_MSG_MAP(AbstractListView)
	ON_WM_CREATE()
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, onItemchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void AbstractListView::OnInitialUpdate() 
    {
	    CListView::OnInitialUpdate();
	    
        // Create our image list ready to add icons to it
        m_listIcons.Create(SMALL_ICON_SIZE,SMALL_ICON_SIZE,ILC_COLOR32,10,10);
        GetListCtrl().SetImageList(&m_listIcons,LVSIL_SMALL);

        // Insert a column where we will display the entity names
        GetListCtrl().InsertColumn(0,"",LVCFMT_LEFT, AbstractFrameworkView::LIST_WIDTH - MARGIN_BUFFER);
    }


    int AbstractListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT | LVS_SHOWSELALWAYS | LVS_SORTASCENDING | LVS_NOCOLUMNHEADER | LVS_NOSORTHEADER | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }

    
    void AbstractListView::onItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        FUNCTION_ENTRY("onItemchanged");

	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
        TA_ASSERT(pNMListView != NULL, "The NM_LISTVIEW structure passed to us was NULL");
        TA_ASSERT(m_detailView != NULL, "The view we have to show the details is NULL");
        TA_ASSERT(m_databaseAccessor != NULL, "The Database accessor is NULL");

		if (!(LVIF_STATE & pNMListView->uChanged))
		{
			// If this is not a state change (ie change in selection), we may ignore it
			FUNCTION_EXIT;
			return;
		}

        // if previously not selected and now selected
        if ( !(pNMListView->uOldState & LVIS_SELECTED) && (pNMListView->uNewState & LVIS_SELECTED) )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Setting the detail view to show the selected item");

            // An item is selected
            int selection = GetListCtrl().GetItemData(pNMListView->iItem);
            TA_Base_Core::IConfigItem* item = m_databaseAccessor->getItem(selection);
            m_detailView->populateItemDetails(item);
            s_currentState[m_databaseAccessor].save(pNMListView->iItem, item);
        }
        // if previously selected and now not selected
        else if ( (pNMListView->uOldState & LVIS_SELECTED) && !(pNMListView->uNewState & LVIS_SELECTED) )
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug,"Setting the detail view to show nothing");
            // Hide the item details as this item is unselected
            s_currentState[m_databaseAccessor].clear();
            m_detailView->populateItemDetails(NULL);
        }

	    *pResult = 0;
        FUNCTION_EXIT;
    }


    void AbstractListView::itemDetailsChanged(unsigned long id, const CString& name)
    {
        FUNCTION_ENTRY("itemDetailsChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        int foundItemIndex = GetListCtrl().FindItem(&info);
        if (foundItemIndex == -1)
        {
            // This is a new item so we add it into our list
            int position = GetListCtrl().InsertItem(0,name);
            GetListCtrl().SetItemData(position, id);

            // Now select this as the current item
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);

            FUNCTION_EXIT;
            return;
        }

        // Now remove and re-add the item if the name has changed so that it is added back into the
        // list in sorted order.
        if (name != GetListCtrl().GetItemText(foundItemIndex,0) )
        {
            GetListCtrl().DeleteItem(foundItemIndex);
            int position = GetListCtrl().InsertItem(foundItemIndex,name);
            GetListCtrl().SetItemData(position,id);

            // Now select this as the current item
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);
        }

        FUNCTION_EXIT;
    }


    void AbstractListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT(m_databaseAccessor != NULL, "setDatabaseAccessor() must be called before this method");
        TA_ASSERT(m_detailView != NULL, "The detail view has not been set yet");
        TA_ASSERT(getStatusBar() != NULL, "The status bar has not been set yet");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

        std::multimap<CString,unsigned long> items;
        try
        {
            AfxGetApp()->DoWaitCursor(1);

            // Tell the database accessor to load all items from the database
            m_databaseAccessor->loadItems();
            
            // Now get a list of item names and ids so we can populate our list
            // The progress bar will start moving in this method. This method also sets the range
            // on the status bar.
            items = m_databaseAccessor->getItemNames(progress);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);

            getStatusBar()->hideProgressBar();
            AfxGetApp()->DoWaitCursor(-1);

            FUNCTION_EXIT;
            return;
        }

        if (items.empty())
        {
            getStatusBar()->hideProgressBar();

            FUNCTION_EXIT;
            return;
        }


        // Set the range to be the number of entities we have then we can just step by one each time.

        // Add the item icon for each item in the list
        int iconPosition = m_listIcons.Add( getComponentIcon() );

        // Now add all the items into our list and set their data to be their ids
        for (std::multimap<CString,unsigned long>::iterator iter = items.begin(); iter != items.end(); ++iter)
        {
            int position = GetListCtrl().InsertItem(0,iter->first,iconPosition);
            GetListCtrl().SetItemData(position, iter->second);
            progress.StepIt();
            m_names.insert((const char*)iter->first);
        }

        CurrentState& state = s_currentState[m_databaseAccessor];

        if ( !state.empty() )
        {
            GetListCtrl().SetItemState(state.m_index, LVIS_SELECTED, LVIS_SELECTED);
            m_detailView->populateItemDetails(state.m_item);
        }

        getStatusBar()->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void AbstractListView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            CString actionName;
            actionName.LoadString(IDS_COPY);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030059);
            FUNCTION_EXIT;
            return;
        }

        std::vector<TA_Base_Core::IConfigItem*> newItems;

        while(pos != NULL)
        {
            // An item is selected
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            try
            {
                TA_ASSERT(m_databaseAccessor != NULL, "Database accesssor is NULL");
                TA_Base_Core::IConfigItem* newItem = m_databaseAccessor->copyItem(itemId);

                if (newItem !=  NULL)
                {
                    // Append something to the end of the name to indicate that this is a new item
                    CString name = dynamic_cast<TA_Base_Core::IItem*>(newItem)->getName().c_str();
                    //newName += AbstractDetailView::NEW_LABEL;
                    std::string copyName = getCopyName( (const char*)name );
                    newItem->setName( copyName.c_str() );

                    // Store the item so we can add it to the list later
                    newItems.push_back(newItem);
                    m_names.insert( copyName );
                    GetListCtrl().SetItemState(selected, 0, LVIS_SELECTED);
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
        }

        // Now add all the new items to the list now that we've created them.
        for(std::vector<TA_Base_Core::IConfigItem*>::iterator iter = newItems.begin(); iter != newItems.end(); ++iter)
        {
            // Insert this item into the list and set it to be the selected item
            int position = GetListCtrl().InsertItem(-1,dynamic_cast<TA_Base_Core::IItem*>(*iter)->getName().c_str());
            GetListCtrl().SetItemData(position, (*iter)->getUniqueIdentifier());

            // By setting this as the selected item the item detail view will automatically
            // be updated.
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);
        }

        FUNCTION_EXIT;
    }


    void AbstractListView::deleteItem()
    {
        FUNCTION_ENTRY("deleteItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if (pos == NULL)
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

        unsigned int numberOfItemsToDelete = GetListCtrl().GetSelectedCount();

        CProgressCtrl& progress = m_statusBar->displayProgressBar();
        progress.SetRange(1,numberOfItemsToDelete);
        AfxGetApp()->DoWaitCursor(1);

        std::set<int> itemsToDelete;
        std::set<std::string> namesToDelete;

        while (pos != NULL)
        {
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            try
            {
                TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");
                TA_Base_Core::IConfigItem* item = m_databaseAccessor->getItem(itemId);

                // If this is not a new item then we want to log the fact that it has been deleted
                if (item == NULL)
                {
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugWarn, "No item was found. Cannot delete it");
                    FUNCTION_EXIT;
                    return;
                }

                MessageSender::UpdateDetails updateDetails;
                updateDetails.isNewItem = item->isNew();
            
                if (!updateDetails.isNewItem)
                {
                    updateDetails.isDelete = true;
                    updateDetails.keyOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(item)->getKey();
                    updateDetails.nameOfChangedItem = dynamic_cast<TA_Base_Core::IItem*>(item)->getName();
                    updateDetails.configUpdateMessage = getConfigMessageType();
                }

                namesToDelete.insert( dynamic_cast<TA_Base_Core::IItem*>(item)->getName() );

                m_databaseAccessor->deleteItem(itemId);

                // Flag the item to be removed from our list
                itemsToDelete.insert(selected);

                if (!updateDetails.isNewItem)
                {
                    TA_ASSERT(getMessageSender() != NULL, "The message sender is NULL");
                    getMessageSender()->sendMessagesForTheChange( updateDetails );
                }
            }
            catch ( const TA_Base_Core::DataException& ex)
            {
                LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException","Could not delete the item");
                if (ex.getFailType() == TA_Base_Core::DataException::CANNOT_DELETE)
                {
                    CString actionName, reason;
                    actionName.LoadString(IDS_DELETE);
                    reason.LoadString(IDS_LINKED_ITEM_EXISTS);
                    
                    if(!ex.getWhichData().empty())
                    {
                        CString details;
                        details.LoadString(IDS_REMOVE_LINKED_ITEM);
                        reason += details;
                        reason += " ";

						// TD #11533
						// Limit the data elements listed to avoid an overly long error message,
						// and a consequently large and user-unfriendly error dialog.
						const int maxDataElements = 5;
						int counter = 0;
                        
						// tokenise the display string by the ',' delimiter
                        std::string whichData = ex.getWhichData();
						char* tok;
					    tok = strtok(const_cast<char*>(whichData.c_str()), ",");

						// loop until we've displayed all the tokens, or until we've displayed
						// the maximum number of data elements already
					    while((tok != NULL) && (counter <= maxDataElements))
						{
							// already displayed the maximum number of elements, but there's
							// at least one more, so append a "..." to the message
							if(counter == maxDataElements)
							{
								reason += "...";
							}
							else
							{
								reason += tok;
								reason += "\n";
								tok = strtok(NULL, ",");
							}

							counter++;
						}
                    }

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

            progress.StepIt();
        }

        // Tell the detail view to hide what it is showing.
        m_detailView->populateItemDetails(NULL);

        // Now delete all the items from the list
        for(std::set<int>::reverse_iterator iter = itemsToDelete.rbegin(); iter != itemsToDelete.rend(); ++iter)
        {
            GetListCtrl().DeleteItem(*iter);
        }

        for ( std::set<std::string>::iterator it = namesToDelete.begin(); it != namesToDelete.end(); ++it )
        {
            m_names.erase( *it );
        }

        m_statusBar->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void AbstractListView::newItem()
    {
        FUNCTION_ENTRY("newItem");
        
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor is NULL");

        try
        {
            TA_Base_Core::IConfigItem* newItem = m_databaseAccessor->newItem();

            if (newItem !=  NULL)
            {
                // Append something to the end of the name to indicate that this is a new item
                std::string name = dynamic_cast<TA_Base_Core::IItem*>(newItem)->getName();
                //newName += AbstractDetailView::NEW_LABEL;
                std::string newName = getNewName( name );
                newItem->setName( newName );

                // Insert this item into the list and set it to be the selected item
                int position = GetListCtrl().InsertItem(-1,newName.c_str());
                GetListCtrl().SetItemData(position, newItem->getUniqueIdentifier());

                // Unselect the current selections
                POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
                while (pos != NULL)
                {
                    int selected = GetListCtrl().GetNextSelectedItem(pos);
                    GetListCtrl().SetItemState(selected,0,LVIS_SELECTED);
                }

                // By setting this as the selected item the item detail view will automatically
                // be updated.
                GetListCtrl().SetItemState(position,LVIS_SELECTED,LVIS_SELECTED);
                m_names.insert( newName );
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


    /////////////////////////////////////////////////////////////////////////////
    // AbstractListView diagnostics

    #ifdef _DEBUG
    void AbstractListView::AssertValid() const
    {
	    CListView::AssertValid();
    }

    void AbstractListView::Dump(CDumpContext& dc) const
    {
	    CListView::Dump(dc);
    }
    #endif //_DEBUG


    std::string AbstractListView::getNewName( const std::string& name, const std::string& prefix )
    {
        std::string newName = prefix + name;

        for ( size_t i = 1; m_names.find( newName ) != m_names.end(); ++i )
        {
            newName = prefix + name + "(" + boost::lexical_cast<std::string>(i) + ")";
        }

        return newName;
    }


    std::string AbstractListView::getCopyName( const std::string& name, const std::string& suffix )
    {
        std::string newName = name + suffix;
        size_t pos = newName.find( suffix +"(" );

        if ( pos != std::string::npos )
        {
            size_t n = newName.find( ')', pos ) - pos - 1;

            for ( size_t i = 2; m_names.find( newName ) != m_names.end(); ++i )
            {
                std::string tmp = suffix + "(" + boost::lexical_cast<std::string>(i) + ")";
                newName.replace(pos, n, tmp );
            }
        }
        else
        {
            for ( size_t i = 2; m_names.find( newName ) != m_names.end(); ++i )
            {
                newName = name + suffix + "(" + boost::lexical_cast<std::string>(i) + ")";
            }
        }

        return newName;
    }
}
