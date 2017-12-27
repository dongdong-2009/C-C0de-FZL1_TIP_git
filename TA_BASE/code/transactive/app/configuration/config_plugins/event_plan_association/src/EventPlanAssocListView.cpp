/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/EventPlanAssocListView.cpp $
  * @author:  Brad Cleaver
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all event plan associations available to be configured. This view must communicate
  * with the EventPlanAssocDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/event_plan_association/src/StdAfx.h"
#include "app/configuration/config_plugins/event_plan_association/src/ChineseInfo.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocListView.h"
#include "app/configuration/config_plugins/event_plan_association/src/EventPlanAssocComponent.h"
#include "app/configuration/config_plugins/event_plan_association/src/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"
#include "bus/resources/resource.h"

#include "core/data_access_interface/src/IConfigItem.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;
// using TA_Base_App::EventPlanAssocDatabaseAccessor::EventPlanAssociation;
typedef TA_Base_App::EventPlanAssocDatabaseAccessor::EventPlanAssociation EventPlanAssociation;
using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    // TODO: When entitytype is enabled again reorder these columns.
    const int EventPlanAssocListView::APPLIED_COLUMN = 0;
    const int EventPlanAssocListView::ENABLED_COLUMN = APPLIED_COLUMN + 1;
    const int EventPlanAssocListView::EVENTTYPE_COLUMN = APPLIED_COLUMN + 2;
    const int EventPlanAssocListView::ENTITYTYPE_COLUMN = APPLIED_COLUMN + 5;
    const int EventPlanAssocListView::ENTITY_COLUMN =APPLIED_COLUMN + 3;
    const int EventPlanAssocListView::PLAN_COLUMN =APPLIED_COLUMN + 4;

    const CString EventPlanAssocListView::APPLIED_COLUMN_NAME = STATUS_MSG;
    const CString EventPlanAssocListView::ENABLED_COLUMN_NAME = ENABLED_MSG;
    const CString EventPlanAssocListView::EVENTTYPE_COLUMN_NAME = EVENT_TYPE_MSG;
    const CString EventPlanAssocListView::ENTITYTYPE_COLUMN_NAME = ENTITY_TYPE_MSG;
    const CString EventPlanAssocListView::ENTITY_COLUMN_NAME = ENTITY_MSG;
    const CString EventPlanAssocListView::PLAN_COLUMN_NAME = PLAN_MSG;

    const int EventPlanAssocListView::APPLIED_COLUMN_WIDTH = 75;
    const int EventPlanAssocListView::ENABLED_COLUMN_WIDTH = 75;
    const int EventPlanAssocListView::EVENTTYPE_COLUMN_WIDTH = 250;
    const int EventPlanAssocListView::ENTITYTYPE_COLUMN_WIDTH = 200;
    const int EventPlanAssocListView::ENTITY_COLUMN_WIDTH = 250;
    const int EventPlanAssocListView::PLAN_COLUMN_WIDTH = 300;

    // These are the possible values for the status of each event plan association
    const CString EventPlanAssocListView::NEW = "New";
    const CString EventPlanAssocListView::MODIFIED = "Modified";
    const CString EventPlanAssocListView::UPTODATE = "";

    const CString EventPlanAssocListView::YES = "Yes";
    const CString EventPlanAssocListView::NO = "No";

    const bool EventPlanAssocListView::ASCENDING = true;
    const bool EventPlanAssocListView::DESCENDING = false;


    IMPLEMENT_DYNCREATE(EventPlanAssocListView, CListView)


    EventPlanAssocListView::EventPlanAssocListView()
    :     m_sortOrder(DESCENDING),        // Set to descending so the first time we sort it switches to ASCENDING
          m_sortColumn(EVENTTYPE_COLUMN)  // This is the column to sort on when the list control gets created
    {
        m_componentIcon = LoadIcon( GetModuleHandle(EventPlanAssocComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_EVENT_PLANS) );
    }


    EventPlanAssocListView::~EventPlanAssocListView()
    {
    }


BEGIN_MESSAGE_MAP(EventPlanAssocListView, AbstractListView)
    //{{AFX_MSG_MAP(EventPlanAssocListView)
    ON_WM_CREATE()
    ON_WM_SIZE()
    ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
    //}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int EventPlanAssocListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT|LVS_SHOWSELALWAYS|WS_BORDER;

        if ( -1 == CListView::OnCreate(lpCreateStruct) )
        {
            return -1;
        }

        return 0;
    }


    void EventPlanAssocListView::OnInitialUpdate() 
    {
        CListView::OnInitialUpdate();

        GetListCtrl().SetExtendedStyle( LVS_EX_FULLROWSELECT|LVS_EX_INFOTIP|LVS_EX_GRIDLINES );

        // Insert the columns to display the data
        GetListCtrl().InsertColumn( APPLIED_COLUMN,   APPLIED_COLUMN_NAME,   LVCFMT_LEFT, APPLIED_COLUMN_WIDTH   );
        GetListCtrl().InsertColumn( ENABLED_COLUMN,   ENABLED_COLUMN_NAME,   LVCFMT_LEFT, ENABLED_COLUMN_WIDTH   );
        GetListCtrl().InsertColumn( EVENTTYPE_COLUMN, EVENTTYPE_COLUMN_NAME, LVCFMT_LEFT, EVENTTYPE_COLUMN_WIDTH );
        GetListCtrl().InsertColumn( ENTITY_COLUMN,    ENTITY_COLUMN_NAME,    LVCFMT_LEFT, ENTITY_COLUMN_WIDTH    );
        GetListCtrl().InsertColumn( PLAN_COLUMN,      PLAN_COLUMN_NAME,      LVCFMT_LEFT, PLAN_COLUMN_WIDTH      );

        // TODO: When entity type is enabled place the following line in the correct position above.
        //GetListCtrl().InsertColumn( ENTITYTYPE_COLUMN,ENTITYTYPE_COLUMN_NAME,LVCFMT_LEFT, ENTITYTYPE_COLUMN_WIDTH);
    }


    void EventPlanAssocListView::OnSize(UINT nType, int cx, int cy) 
    {
        CListView::OnSize(nType, cx, cy);
        // Call the CListView::ONSize instead of the one in the abstract class above
    }


    void EventPlanAssocListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT( getDatabaseAccessor() != NULL, "setDatabaseAccessor() must be called before this method" );
        TA_ASSERT( getDetailView() != NULL, "The detail view has not been set yet" );
        TA_ASSERT( getStatusBar() != NULL, "The status bar has not been set yet" );

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

        EventPlanAssocDatabaseAccessor* dbAccessor = dynamic_cast<EventPlanAssocDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an Event Plan Assocation one" );

        m_currentDisplayedItems.clear();

        try
        {
            AfxGetApp()->DoWaitCursor(1);

            // Tell the database accessor to load all items from the database
            dbAccessor->loadItems();
            
            // Now get a list of item names and ids so we can populate our list
            // The progress bar will start moving in this method. This method also sets the range
            // on the status bar.
            m_currentDisplayedItems = dbAccessor->getAllItemDetails( progress );
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

        if (m_currentDisplayedItems.empty())
        {
            getStatusBar()->hideProgressBar();

            FUNCTION_EXIT;
            return;
        }

        // Set the range to be the number of entities we have then we can just step by one each time.

        // Now add all the items into our list and set their data to be their ids
        std::map<unsigned long,EventPlanAssociation>::iterator assocIt;
        for ( assocIt=m_currentDisplayedItems.begin(); assocIt!=m_currentDisplayedItems.end(); assocIt++ )
        {
            if ( dbAccessor->getItem( assocIt->first )->isNew() )
            {
                assocIt->second.status = NEW;
            }
            else if ( dbAccessor->getItem( assocIt->first )->hasChanged() )
            {
                assocIt->second.status = MODIFIED;
            }
            else
            {
                assocIt->second.status = UPTODATE;
            }

            int position = GetListCtrl().InsertItem( 0, "", 0 );
            GetListCtrl().SetItemText( position, APPLIED_COLUMN, assocIt->second.status);
            GetListCtrl().SetItemText( position, ENABLED_COLUMN, assocIt->second.enabled?YES:NO);
            GetListCtrl().SetItemText( position, EVENTTYPE_COLUMN, assocIt->second.eventType);
            GetListCtrl().SetItemText( position, ENTITYTYPE_COLUMN, assocIt->second.entityType);
            GetListCtrl().SetItemText( position, ENTITY_COLUMN, assocIt->second.entity);
            GetListCtrl().SetItemText( position, PLAN_COLUMN, assocIt->second.plan);
            
            GetListCtrl().SetItemData( position, assocIt->first );
            progress.StepIt();
        }

        // Now sort the columns
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));
        
        getStatusBar()->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }

    
    void EventPlanAssocListView::itemDetailsChanged( unsigned long id, const CString& name )
    {
        FUNCTION_ENTRY("itemChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        EventPlanAssocDatabaseAccessor* dbAccessor = dynamic_cast<EventPlanAssocDatabaseAccessor*>( getDatabaseAccessor() );
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an Event Plan Assocation one");

        EventPlanAssociation details = dbAccessor->getItemDetails( id );

        // If the item is found then update it with the new details
        int foundItemIndex = GetListCtrl().FindItem( &info );
        if ( CB_ERR != foundItemIndex )
        {
            if ( dbAccessor->getItem(id)->isNew() )
            {
                details.status = NEW;
            }
            else if ( dbAccessor->getItem(id)->hasChanged() )
            {
                details.status = MODIFIED;
            }
            else
            {
                details.status = UPTODATE;
            }

            GetListCtrl().SetItemText( foundItemIndex, APPLIED_COLUMN, details.status );
            GetListCtrl().SetItemText( foundItemIndex, ENABLED_COLUMN, details.enabled?YES:NO );
            GetListCtrl().SetItemText( foundItemIndex, EVENTTYPE_COLUMN, details.eventType );
            GetListCtrl().SetItemText( foundItemIndex, ENTITYTYPE_COLUMN, details.entityType );
            GetListCtrl().SetItemText( foundItemIndex, ENTITY_COLUMN, details.entity );
            GetListCtrl().SetItemText( foundItemIndex, PLAN_COLUMN, details.plan );

        }
        else
        {
            // Add the new details
            details.status = NEW;

            int position = GetListCtrl().InsertItem( 0, "", 0 );
            GetListCtrl().SetItemText( position, APPLIED_COLUMN, details.status );
            GetListCtrl().SetItemText( position, ENABLED_COLUMN, details.enabled?YES:NO );
            GetListCtrl().SetItemText( position, EVENTTYPE_COLUMN, details.eventType );
            GetListCtrl().SetItemText( position, ENTITYTYPE_COLUMN, details.entityType );
            GetListCtrl().SetItemText( position, ENTITY_COLUMN, details.entity );
            GetListCtrl().SetItemText( position, PLAN_COLUMN, details.plan );
            GetListCtrl().SetItemData( position, id );

            // Now select this as the current item
            GetListCtrl().SetItemState( position, 0xFFFFFF, LVIS_SELECTED );
        }

        // Keep our stored map up-to-date so we can perform accurate and quick sorting
        std::map<unsigned long,EventPlanAssociation>::iterator matchingItem = m_currentDisplayedItems.find( id );
        if ( matchingItem != m_currentDisplayedItems.end() )
        {
            matchingItem->second = details;
        }
        else
        {
            m_currentDisplayedItems.insert( std::map<unsigned long,EventPlanAssociation>::value_type(id,details) );
        }

        // Now sort the columns again
        m_sortOrder = !m_sortOrder; // Make sure we sort in the same order. To do this we must change the
                                    // sort order as the sorting method will change it again
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        FUNCTION_EXIT;
    }


    void EventPlanAssocListView::copyItem()
    {
        FUNCTION_ENTRY("copyItem");

        POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
        if ( NULL == pos )
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

        while ( NULL != pos )
        {
            // An item is selected
            int selected = GetListCtrl().GetNextSelectedItem(pos);
            unsigned long itemId = GetListCtrl().GetItemData( selected );

            try
            {
                TA_ASSERT(getDatabaseAccessor() != NULL, "Database accesssor is NULL");

                TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->copyItem(itemId);

                if ( NULL != newItem )
                {
                    // Add the item so it can be added to the list later
                    newItems.push_back(newItem);
                }
            }
            catch ( const TA_Base_Core::DataException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DataException","Could not copy the item");
                CString actionName, reason;
                actionName.LoadString(IDS_COPY);
                reason.LoadString(IDS_DATA_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            }
            catch ( const TA_Base_Core::DatabaseException& )
            {
                LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException","Could not copy the item");
                CString actionName, reason;
                actionName.LoadString(IDS_COPY);
                reason.LoadString(IDS_DATABASE_ERROR);
                TA_Base_Bus::TransActiveMessage userMsg;
                userMsg << actionName << reason;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
            }
        }

        // Now finally add the items we have created into the list
        std::vector<TA_Base_Core::IConfigItem*>::iterator newItemIt;
        for ( newItemIt=newItems.begin(); newItemIt!=newItems.end(); newItemIt++ )
        {
            itemDetailsChanged( (*newItemIt)->getUniqueIdentifier(), "" );
        }

        FUNCTION_EXIT;
    }


    void EventPlanAssocListView::newItem()
    {
        FUNCTION_ENTRY("newItem");
        
        TA_ASSERT( getDatabaseAccessor() != NULL, "Database accessor is NULL" );

        try
        {
            TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->newItem();

            // Unselect the current selections
            POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
            while ( NULL != pos )
            {
                int selected = GetListCtrl().GetNextSelectedItem( pos );
                GetListCtrl().SetItemState( selected, 0, LVIS_SELECTED );
            }

            if ( NULL != newItem )
            {
                itemDetailsChanged( newItem->getUniqueIdentifier(), "" );
            }
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DataException","Could not create a new item");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATA_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException","Could not create a new item");
            CString actionName, reason;
            actionName.LoadString(IDS_CREATE);
            reason.LoadString(IDS_DATABASE_ERROR);
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << actionName << reason;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_030060);
        }

        FUNCTION_EXIT;
    }
    

    void EventPlanAssocListView::onColumnclick( NMHDR* pNMHDR, LRESULT* pResult ) 
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        updateHeaderSort(pNMListView->iSubItem);

        GetListCtrl().SortItems( compareItems, reinterpret_cast<unsigned long>(this) );
        
        *pResult = 0;
    }


    void EventPlanAssocListView::updateHeaderSort( const unsigned int column )
    {
        FUNCTION_ENTRY("updateHeaderSort");

        //First find if the selected column is currently sorted on. If so, toggle 
        //the sorting arrangment. 
        CHeaderCtrl* headerCtrl = GetListCtrl().GetHeaderCtrl(); 

        HDITEM headerItem; 
        headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
        headerCtrl->GetItem(column, &headerItem); 

        if ( 0 != headerItem.hbm ) 
        { 
            DeleteObject(headerItem.hbm); 
            headerItem.hbm = 0; 
        }
        
        // If this is the same column we sorted on last time then now we want to sort in the opposite order
        if( m_sortColumn == column )
        {
            m_sortOrder = !m_sortOrder;
        }
        else
        {
            // If this is a different column reset the sort order to be ascending first.
            m_sortOrder = ASCENDING;
        }

        // Set up ready to add our bitmap
        headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

        /// Load the bitmap based on whether we are sorting ascending or descending
        int resourceToLoad = IDB_DESCENDING;
        if ( m_sortOrder == ASCENDING )
        {
            resourceToLoad = IDB_ASCENDING;
        }

        headerItem.hbm = (HBITMAP)LoadImage( GetModuleHandle(EventPlanAssocComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS );
        headerCtrl->SetItem( column, &headerItem ); 

        // If the column we are sorting on has changed then we must remove the arrow bitmap from the old
        // column header
        if( m_sortColumn != column ) 
        { 
            headerCtrl->GetItem(m_sortColumn, &headerItem); 
            headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 

            if( 0 != headerItem.hbm )
            { 
                DeleteObject( headerItem.hbm ); 
                headerItem.hbm = 0; 
            } 
            headerCtrl->SetItem( m_sortColumn, &headerItem ); 

            // Now set our new sort column
            m_sortColumn = column;
        }

        FUNCTION_EXIT;
    }


    int CALLBACK EventPlanAssocListView::compareItems( LPARAM index1, LPARAM index2, LPARAM listCtrl )
    {
        EventPlanAssocListView* list = reinterpret_cast<EventPlanAssocListView*>(listCtrl);

        // Double check it has converted ok
        list = dynamic_cast<EventPlanAssocListView*>(list);

        TA_ASSERT( list != NULL, "The wrong type was passed as the third parameter to this method" );

        // Now retrieve the data for these items so we can start comparing it
        std::map<unsigned long,EventPlanAssociation>::iterator item1 = list->m_currentDisplayedItems.find( index1 );
        std::map<unsigned long,EventPlanAssociation>::iterator item2 = list->m_currentDisplayedItems.find( index2 );

        if (item1 == list->m_currentDisplayedItems.end() || 
            item2 == list->m_currentDisplayedItems.end())
        {
            // Cannot compare
            return 0;
        }

        CString string1("");
        CString string2("");

        switch( list->m_sortColumn )
        {
            case(APPLIED_COLUMN):
                    string1 = item1->second.status;
                    string2 = item2->second.status;
                    break;

            case(ENABLED_COLUMN):
                    string1 = item1->second.enabled?YES:NO;
                    string2 = item2->second.enabled?YES:NO;
                    break;

            case(EVENTTYPE_COLUMN):
                    string1 = item1->second.eventType;
                    string2 = item2->second.eventType;
                    break;

            case(ENTITYTYPE_COLUMN):
                    string1 = item1->second.entityType;
                    string2 = item2->second.entityType;
                    break;

            case(ENTITY_COLUMN):
                    string1 = item1->second.entity;
                    string2 = item2->second.entity;
                    break;

            case(PLAN_COLUMN):
                    string1 = item1->second.plan;
                    string2 = item2->second.plan;
                    break;
        }
        
        int returnVal = string1.CompareNoCase(string2);

        // If we are sorting descending then switch the sign of the returnVal
        if ( list->m_sortOrder == DESCENDING )
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }


    void EventPlanAssocListView::OnDraw(CDC* pDC)
    {
        CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // EventPlanAssocListView diagnostics

    #ifdef _DEBUG
    void EventPlanAssocListView::AssertValid() const
    {
        AbstractListView::AssertValid();
    }

    void EventPlanAssocListView::Dump(CDumpContext& dc) const
    {
        AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

}

