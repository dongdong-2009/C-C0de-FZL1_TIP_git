/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This view contains a list of all alarm plan associations available to be configured. This view must communicate
  * with the AlarmPlanAssocDetailView as this will show the details of the item selected in this view.
  */


#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_plan_association/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocListView.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/AlarmPlanAssocComponent.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/resource.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"
#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigItem.h"

#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "app/configuration/config_plugins/alarm_plan_association/src/ChineseSupport.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;

typedef TA_Base_App::AlarmPlanAssocDatabaseAccessor::AlarmPlanAssociation AlarmPlanAssociation;
//using TA_Base_App::AlarmPlanAssocDatabaseAccessor::AlarmPlanAssociation;

using namespace TA_Base_App::TA_Configuration;
using namespace alarmPlanAsso_chinese;

namespace TA_Base_App
{
    const int AlarmPlanAssocListView::APPLIED_COLUMN = 0;
    const int AlarmPlanAssocListView::ALARMTYPE_COLUMN = APPLIED_COLUMN + 1;
    const int AlarmPlanAssocListView::ENTITYTYPE_COLUMN = APPLIED_COLUMN + 2;
    const int AlarmPlanAssocListView::ENTITY_COLUMN =APPLIED_COLUMN + 3;
    const int AlarmPlanAssocListView::PLAN_COLUMN =APPLIED_COLUMN + 4;



    const int AlarmPlanAssocListView::APPLIED_COLUMN_WIDTH = 75;
    const int AlarmPlanAssocListView::ALARMTYPE_COLUMN_WIDTH = 250;
    const int AlarmPlanAssocListView::ENTITYTYPE_COLUMN_WIDTH = 200;
    const int AlarmPlanAssocListView::ENTITY_COLUMN_WIDTH = 250;
    const int AlarmPlanAssocListView::PLAN_COLUMN_WIDTH = 300;


    const bool AlarmPlanAssocListView::ASCENDING = true;
    const bool AlarmPlanAssocListView::DESCENDING = false;

    IMPLEMENT_DYNCREATE(AlarmPlanAssocListView, CListView)


    AlarmPlanAssocListView::AlarmPlanAssocListView()
    :     m_sortOrder(DESCENDING),        // Set to descending so the first time we sort it switches to ASCENDING
          m_sortColumn(ALARMTYPE_COLUMN)  // This is the column to sort on when the list control gets created
    {
        m_componentIcon = LoadIcon(GetModuleHandle(AlarmPlanAssocComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_PLANS) );
    }


    AlarmPlanAssocListView::~AlarmPlanAssocListView()
    {
    }


BEGIN_MESSAGE_MAP(AlarmPlanAssocListView, AbstractListView)
	//{{AFX_MSG_MAP(AlarmPlanAssocListView)
	ON_WM_CREATE()
    ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int AlarmPlanAssocListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }


    void AlarmPlanAssocListView::OnInitialUpdate() 
    {
	    CListView::OnInitialUpdate();
	    
        GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

        // Insert the columns to display the data
        GetListCtrl().InsertColumn(APPLIED_COLUMN,   APPLIED_COLUMN_NAME,   LVCFMT_LEFT, APPLIED_COLUMN_WIDTH );
        GetListCtrl().InsertColumn(ALARMTYPE_COLUMN, ALARMTYPE_COLUMN_NAME, LVCFMT_LEFT, ALARMTYPE_COLUMN_WIDTH );
        GetListCtrl().InsertColumn(ENTITYTYPE_COLUMN,ENTITYTYPE_COLUMN_NAME,LVCFMT_LEFT, ENTITYTYPE_COLUMN_WIDTH);
        GetListCtrl().InsertColumn(ENTITY_COLUMN,    ENTITY_COLUMN_NAME,    LVCFMT_LEFT, ENTITY_COLUMN_WIDTH    );
        GetListCtrl().InsertColumn(PLAN_COLUMN,      PLAN_COLUMN_NAME,      LVCFMT_LEFT, PLAN_COLUMN_WIDTH      );
    }


    void AlarmPlanAssocListView::OnSize(UINT nType, int cx, int cy) 
    {
        CListView::OnSize(nType, cx, cy);
        // Call the CListView::ONSize instead of the one in the abstract class above
    }


    void AlarmPlanAssocListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT(getDatabaseAccessor() != NULL, "setDatabaseAccessor() must be called before this method");
        TA_ASSERT(getDetailView() != NULL, "The detail view has not been set yet");
        TA_ASSERT(getStatusBar() != NULL, "The status bar has not been set yet");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

//        using AlarmPlanAssocDatabaseAccessor::AlarmPlanAssociation;
        AlarmPlanAssocDatabaseAccessor* dbAccessor = dynamic_cast<AlarmPlanAssocDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an Alarm Plan Assocation one");

        m_currentDisplayedItems.clear();

        try
        {
            AfxGetApp()->DoWaitCursor(1);

            // Tell the database accessor to load all items from the database
            dbAccessor->loadItems();
            
            // Now get a list of item names and ids so we can populate our list
            // The progress bar will start moving in this method. This method also sets the range
            // on the status bar.
            m_currentDisplayedItems = dbAccessor->getAllItemDetails(progress);
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
        for (std::map<unsigned long,AlarmPlanAssociation>::iterator iter = m_currentDisplayedItems.begin(); 
             iter != m_currentDisplayedItems.end(); ++iter)
        {
            if (dbAccessor->getItem(iter->first)->isNew())
            {
                iter->second.status = NEW;
            }
            else if (dbAccessor->getItem(iter->first)->hasChanged())
            {
                iter->second.status = MODIFIED;
            }
            else
            {
                iter->second.status = UPTODATE;
            }

            int position = GetListCtrl().InsertItem(0,"",0);
            GetListCtrl().SetItemText(position, APPLIED_COLUMN, iter->second.status);
            GetListCtrl().SetItemText(position, ALARMTYPE_COLUMN, iter->second.alarmTypeDisplay);
            GetListCtrl().SetItemText(position, ENTITYTYPE_COLUMN, iter->second.entityType);
            GetListCtrl().SetItemText(position, ENTITY_COLUMN, iter->second.entity);
            GetListCtrl().SetItemText(position, PLAN_COLUMN, iter->second.plan);
            
            GetListCtrl().SetItemData(position, iter->first);
            progress.StepIt();
        }

        // Now sort the columns
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        
        getStatusBar()->hideProgressBar();
        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }

    
    void AlarmPlanAssocListView::itemDetailsChanged(unsigned long id, const CString& name)
    {
        FUNCTION_ENTRY("itemChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        AlarmPlanAssocDatabaseAccessor* dbAccessor = dynamic_cast<AlarmPlanAssocDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an Alarm Plan Assocation one");

        AlarmPlanAssocDatabaseAccessor::AlarmPlanAssociation details = dbAccessor->getItemDetails( id );

        // If the item is found then update it with the new details

        int foundItemIndex = GetListCtrl().FindItem(&info);
        if (foundItemIndex != -1)
        {
            if (dbAccessor->getItem(id)->isNew())
            {
                details.status = NEW;
            }
            else if (dbAccessor->getItem(id)->hasChanged())
            {
                details.status = MODIFIED;
            }
            else
            {
                details.status = UPTODATE;
            }

            GetListCtrl().SetItemText(foundItemIndex, APPLIED_COLUMN, details.status);
            GetListCtrl().SetItemText(foundItemIndex, ALARMTYPE_COLUMN, details.alarmTypeDisplay);
            GetListCtrl().SetItemText(foundItemIndex, ENTITYTYPE_COLUMN, details.entityType);
            GetListCtrl().SetItemText(foundItemIndex, ENTITY_COLUMN, details.entity);
            GetListCtrl().SetItemText(foundItemIndex, PLAN_COLUMN, details.plan);

        }
        else
        {
            // Add the new details
            details.status = NEW;

            int position = GetListCtrl().InsertItem(0,"",0);
            GetListCtrl().SetItemText(position, APPLIED_COLUMN, details.status);
            GetListCtrl().SetItemText(position, ALARMTYPE_COLUMN, details.alarmTypeDisplay);
            GetListCtrl().SetItemText(position, ENTITYTYPE_COLUMN, details.entityType);
            GetListCtrl().SetItemText(position, ENTITY_COLUMN, details.entity);
            GetListCtrl().SetItemText(position, PLAN_COLUMN, details.plan);
            GetListCtrl().SetItemData(position, id);

            // Now select this as the current item
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);
        }

        // Keep our stored map up-to-date so we can perform accurate and quick sorting
        std::map<unsigned long,AlarmPlanAssociation>::iterator matching;
        matching = m_currentDisplayedItems.find(id);
        if (matching != m_currentDisplayedItems.end())
        {
            matching->second = details;
        }
        else
        {
            m_currentDisplayedItems.insert( std::map<unsigned long,AlarmPlanAssociation>::value_type(id,details) );
        }

        // Now sort the columns again
        m_sortOrder = !m_sortOrder; // Make sure we sort in the same order. To do this we must change the
                                    // sort order as the sorting method will change it again
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocListView::copyItem()
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
                TA_ASSERT(getDatabaseAccessor() != NULL, "Database accesssor is NULL");

                TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->copyItem(itemId);

                if (newItem != NULL)
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
        for(std::vector<TA_Base_Core::IConfigItem*>::iterator iter = newItems.begin(); iter != newItems.end(); ++iter)
        {
            itemDetailsChanged((*iter)->getUniqueIdentifier(), "");
        }

        FUNCTION_EXIT;
    }


    void AlarmPlanAssocListView::newItem()
    {
        FUNCTION_ENTRY("newItem");
        
        TA_ASSERT(getDatabaseAccessor() != NULL, "Database accessor is NULL");

        try
        {
            TA_Base_Core::IConfigItem* newItem = getDatabaseAccessor()->newItem();

            // Unselect the current selections
            POSITION pos = GetListCtrl().GetFirstSelectedItemPosition();
            while (pos != NULL)
            {
                int selected = GetListCtrl().GetNextSelectedItem(pos);
                GetListCtrl().SetItemState(selected,0,LVIS_SELECTED);
            }

            if (newItem !=  NULL)
            {
                itemDetailsChanged(newItem->getUniqueIdentifier(), "");
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
    

    void AlarmPlanAssocListView::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        updateHeaderSort(pNMListView->iSubItem);

        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));
	    
	    *pResult = 0;
    }


	void AlarmPlanAssocListView::updateHeaderSort(const unsigned int column)
	{
		FUNCTION_ENTRY("updateHeaderSort");

		TA_ASSERT(column >= 0, "Column was -ve!");

		//First find if the selected column is currently sorted on. If so, toggle 
		//the sorting arrangment. 
		CHeaderCtrl* headerCtrl = GetListCtrl().GetHeaderCtrl(); 

        HDITEM headerItem; 
		headerItem.mask = HDI_FORMAT | HDI_BITMAP; 
		headerCtrl->GetItem(column, &headerItem); 

		if (headerItem.hbm != 0) 
		{ 
			DeleteObject(headerItem.hbm); 
			headerItem.hbm = 0; 
		}
        
        // If this is the same column we sorted on last time then now we want to sort in the opposite order
		if(m_sortColumn == column)
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
        if (m_sortOrder == ASCENDING)
        {
            resourceToLoad = IDB_ASCENDING;
        }

        headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(AlarmPlanAssocComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
		headerCtrl->SetItem(column, &headerItem); 

        // If the column we are sorting on has changed then we must remove the arrow bitmap from the old
        // column header
		if(m_sortColumn != column) 
		{ 
			headerCtrl->GetItem(m_sortColumn, &headerItem); 
			headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 

			if(headerItem.hbm != 0) 
			{ 
				DeleteObject(headerItem.hbm); 
				headerItem.hbm = 0; 
			} 
			headerCtrl->SetItem(m_sortColumn, &headerItem); 

            // Now set our new sort column
            m_sortColumn = column;
		}

		FUNCTION_EXIT;
	}


    int CALLBACK AlarmPlanAssocListView::compareItems(LPARAM index1, LPARAM index2, LPARAM listCtrl)
    {
        AlarmPlanAssocListView* list = reinterpret_cast<AlarmPlanAssocListView*>(listCtrl);
        // Double check it has converted ok
        list = dynamic_cast<AlarmPlanAssocListView*>(list);

        TA_ASSERT(list != NULL, "The wrong type was passed as the third parameter to this method");

        // Now retrieve the data for these items so we can start comparing it
        std::map<unsigned long,AlarmPlanAssociation>::iterator item1 = list->m_currentDisplayedItems.find( index1 );
        std::map<unsigned long,AlarmPlanAssociation>::iterator item2 = list->m_currentDisplayedItems.find( index2 );

        if (item1 == list->m_currentDisplayedItems.end() || 
            item2 == list->m_currentDisplayedItems.end())
        {
            // Cannot compare
            return 0;
        }

        CString string1("");
        CString string2("");

        switch(list->m_sortColumn)
        {
            case(APPLIED_COLUMN):
                    string1 = item1->second.status;
                    string2 = item2->second.status;
                    break;

            case(ALARMTYPE_COLUMN):
                    string1 = item1->second.alarmTypeDisplay;
                    string2 = item2->second.alarmTypeDisplay;
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
        if (list->m_sortOrder == DESCENDING)
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }

    
    void AlarmPlanAssocListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // AlarmPlanAssocListView diagnostics

    #ifdef _DEBUG
    void AlarmPlanAssocListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void AlarmPlanAssocListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

 
}

