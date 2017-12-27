/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/FZL1_TIP/TA_BASE/transactive/app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleListView.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/03/07 10:41:13 $
  * Last modified by:  $Author: xiaoxia.li $
  * 
  * This view contains a list of all alarm plan associations available to be configured. This view must communicate
  * with the PlanDssRuleDetailView as this will show the details of the item selected in this view.
  */


#include "app/configuration/config_plugins/plan_dss_rule/src/StdAfx.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleListView.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/PlanDssRuleComponent.h"
#include "app/configuration/config_plugins/plan_dss_rule/src/resource.h"

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
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::RunParams;

typedef TA_Base_App::PlanDssRuleDatabaseAccessor::PlanDssDpRuleText PlanDssRuleiation;
//using TA_Base_App::PlanDssRuleDatabaseAccessor::PlanDssRuleiation;

using namespace TA_Base_App::TA_Configuration;

namespace TA_Base_App
{
    enum PlanDssRuleColumns
    {
        COL_APPLIED = 0,
        COL_LOCATION,
        COL_TRIGGER_ENTITY,
        COL_ACTIVE_PLAN,
        COL_MINIMUM_INTERVAL,
		COL_ACTIVED_TOLERANT_TIME,
		COL_DENY_STRATEGY,
		COL_ACTIVATE_VALUE,
		COL_NEED_DEACTIVATE,
		COL_ALLOW_MULTIPLE,
		COL_ENABLE,
		COL_VALID_TIME_IN_DAY,
		COL_VALID_DAYS_IN_WEEK
    };

    const char* const COL_NAME_APPLIED = "编辑状态";
    const char* const COL_NAME_LOCATION = "车站";
    const char* const COL_NAME_TRIGGER_ENTITY = "触发实体";
    const char* const COL_NAME_ACTIVE_PLAN = "激活预案";
    const char* const COL_NAME_MINIMUM_INTERVAL = "最小间隔";
	const char* const COL_NAME_ACTIVED_TOLERANT_TIME = "允许激活有效时间";
	const char* const COL_NAME_DENY_STRATEGY = "拒绝多实例方式";
	const char* const COL_NAME_ACTIVATE_VALUE = "激活值";
	const char* const COL_NAME_NEED_DEACTIVATE = "需要停止预案";
	const char* const COL_NAME_ALLOW_MULTIPLE = "允许预案多实例";
    const char* const COL_NAME_ENABLE = "启用";
	const char* const COL_NAME_VALID_TIME_IN_DAY = "每日有效时间";
	const char* const COL_NAME_VALID_DAYS_IN_WEEK = "每周有效时间";

    const int COL_WIDTH_APPLIED = 75;
    const int COL_WIDTH_LOCATION = 150;
    const int COL_WIDTH_TRIGGER_ENTITY = 300;
    const int COL_WIDTH_ACTIVE_PLAN = 200;
    const int COL_WIDTH_MINIMUM_INTERVAL = 200;
	const int COL_WIDTH_ACTIVED_TOLERANT_TIME = 200;
	const int COL_WIDTH_DENY_STRATEGY = 200;
	const int COL_WIDTH_ACTIVATE_VALUE = 200;
	const int COL_WIDTH_NEED_DEACTIVATE = 200;
	const int COL_WIDTH_ALLOW_MULTIPLE = 200;
	const int COL_WIDTH_ENABLE = 80;
	const int COL_WIDTH_VALID_TIME_IN_DAY = 200;
	const int COL_WIDTH_VALID_DAYS_IN_WEEK = 300;

    // These are the possible values for the status of each alarm plan association
    const char* const STATUS_NEW = "新建的";
    const char* const STATUS_MODIFIED = "已修改";
    const char* const STATUS_UPTODATE = "";

    IMPLEMENT_DYNCREATE(PlanDssRuleListView, CListView)

    PlanDssRuleListView::PlanDssRuleListView() :
    m_bAscending(true),        // Set to descending so the first time we sort it switches to ASCENDING
    m_sortColumn(COL_LOCATION)  // This is the column to sort on when the list control gets created
    {
        m_componentIcon = LoadIcon(GetModuleHandle(PlanDssRuleComponent::DLL_NAME), MAKEINTRESOURCE(IDI_CONFIG_ALARM_PLANS) );
    }


    PlanDssRuleListView::~PlanDssRuleListView()
    {
    }


BEGIN_MESSAGE_MAP(PlanDssRuleListView, AbstractListView)
	//{{AFX_MSG_MAP(PlanDssRuleListView)
	ON_WM_CREATE()
    ON_WM_SIZE()
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    int PlanDssRuleListView::OnCreate(LPCREATESTRUCT lpCreateStruct) 
    {
        // Set the styles for our list control before it is created
        lpCreateStruct->style = LVS_REPORT | LVS_SHOWSELALWAYS | WS_BORDER;

	    if (CListView::OnCreate(lpCreateStruct) == -1)
		    return -1;
	    
	    return 0;
    }


    void PlanDssRuleListView::OnInitialUpdate() 
    {
	    CListView::OnInitialUpdate();
	    
        GetListCtrl().SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP | LVS_EX_GRIDLINES );

        // Insert the columns to display the data
        GetListCtrl().InsertColumn(COL_APPLIED,   COL_NAME_APPLIED,   LVCFMT_LEFT, COL_WIDTH_APPLIED );
        GetListCtrl().InsertColumn(COL_LOCATION, COL_NAME_LOCATION, LVCFMT_LEFT, COL_WIDTH_LOCATION );
        GetListCtrl().InsertColumn(COL_TRIGGER_ENTITY,COL_NAME_TRIGGER_ENTITY,LVCFMT_LEFT, COL_WIDTH_TRIGGER_ENTITY);
        GetListCtrl().InsertColumn(COL_ACTIVE_PLAN,    COL_NAME_ACTIVE_PLAN,    LVCFMT_LEFT, COL_WIDTH_ACTIVE_PLAN    );
        GetListCtrl().InsertColumn(COL_MINIMUM_INTERVAL,      COL_NAME_MINIMUM_INTERVAL,      LVCFMT_LEFT, COL_WIDTH_MINIMUM_INTERVAL      );
		GetListCtrl().InsertColumn(COL_ACTIVED_TOLERANT_TIME,    COL_NAME_ACTIVED_TOLERANT_TIME, LVCFMT_LEFT, COL_WIDTH_ACTIVED_TOLERANT_TIME    );
		GetListCtrl().InsertColumn(COL_DENY_STRATEGY,    COL_NAME_DENY_STRATEGY, LVCFMT_LEFT, COL_WIDTH_DENY_STRATEGY    );
		GetListCtrl().InsertColumn(COL_ACTIVATE_VALUE,    COL_NAME_ACTIVATE_VALUE, LVCFMT_LEFT, COL_WIDTH_ACTIVATE_VALUE    );
		GetListCtrl().InsertColumn(COL_NEED_DEACTIVATE,    COL_NAME_NEED_DEACTIVATE, LVCFMT_LEFT, COL_WIDTH_NEED_DEACTIVATE    );
		GetListCtrl().InsertColumn(COL_ALLOW_MULTIPLE,    COL_NAME_ALLOW_MULTIPLE, LVCFMT_LEFT, COL_WIDTH_ALLOW_MULTIPLE    );
		GetListCtrl().InsertColumn(COL_ENABLE,    COL_NAME_ENABLE, LVCFMT_LEFT, COL_WIDTH_ENABLE    );
		GetListCtrl().InsertColumn(COL_VALID_TIME_IN_DAY,    COL_NAME_VALID_TIME_IN_DAY, LVCFMT_LEFT, COL_WIDTH_VALID_TIME_IN_DAY    );
		GetListCtrl().InsertColumn(COL_VALID_DAYS_IN_WEEK,    COL_NAME_VALID_DAYS_IN_WEEK, LVCFMT_LEFT, COL_WIDTH_VALID_DAYS_IN_WEEK    );
    }


    void PlanDssRuleListView::OnSize(UINT nType, int cx, int cy) 
    {
        CListView::OnSize(nType, cx, cy);
        // Call the CListView::ONSize instead of the one in the abstract class above
    }


    void PlanDssRuleListView::loadAllItems()
    {
        FUNCTION_ENTRY("loadAllItems");

        TA_ASSERT(getDatabaseAccessor() != NULL, "setDatabaseAccessor() must be called before this method");
        TA_ASSERT(getDetailView() != NULL, "The detail view has not been set yet");
        TA_ASSERT(getStatusBar() != NULL, "The status bar has not been set yet");

        // Retrieve the progress bar and display it on the status bar
        CProgressCtrl& progress = getStatusBar()->displayProgressBar();
        progress.SetStep(1);
        progress.SetPos(3); // Indicate that we have started loading and use the wait cursor to do the rest.

//        using PlanDssRuleDatabaseAccessor::PlanDssRuleiation;
        PlanDssRuleDatabaseAccessor* dbAccessor = dynamic_cast<PlanDssRuleDatabaseAccessor*>(getDatabaseAccessor());
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
        for (std::map<unsigned long,PlanDssRuleiation>::iterator iter = m_currentDisplayedItems.begin(); 
             iter != m_currentDisplayedItems.end(); ++iter)
        {
            if (dbAccessor->getItem(iter->first)->isNew())
            {
                iter->second.status = STATUS_NEW;
            }
            else if (dbAccessor->getItem(iter->first)->hasChanged())
            {
                iter->second.status = STATUS_MODIFIED;
            }
            else
            {
                iter->second.status = STATUS_UPTODATE;
            }

            int position = GetListCtrl().InsertItem(0,"",0);
            GetListCtrl().SetItemText(position, COL_APPLIED, iter->second.status.c_str());
            GetListCtrl().SetItemText(position, COL_LOCATION, iter->second.strLocation.c_str());
            GetListCtrl().SetItemText(position, COL_TRIGGER_ENTITY, iter->second.strTriggerEntity.c_str());
            GetListCtrl().SetItemText(position, COL_ACTIVE_PLAN, iter->second.strActivePlan.c_str());
            GetListCtrl().SetItemText(position, COL_MINIMUM_INTERVAL, iter->second.strMinimumInterval.c_str());
			GetListCtrl().SetItemText(position, COL_ACTIVED_TOLERANT_TIME, iter->second.strActivedTolerantTime.c_str());
			GetListCtrl().SetItemText(position, COL_DENY_STRATEGY, iter->second.strDenyStrategy.c_str());
			GetListCtrl().SetItemText(position, COL_ACTIVATE_VALUE, iter->second.strActivateValue.c_str());
            GetListCtrl().SetItemText(position, COL_NEED_DEACTIVATE, iter->second.strNeedDeactivate.c_str());
			GetListCtrl().SetItemText(position, COL_ALLOW_MULTIPLE, iter->second.strAllowMultiple.c_str());
			GetListCtrl().SetItemText(position, COL_ENABLE, iter->second.strEnable.c_str());
			GetListCtrl().SetItemText(position, COL_VALID_TIME_IN_DAY, iter->second.strValidTimeInDay.c_str());
			GetListCtrl().SetItemText(position, COL_VALID_DAYS_IN_WEEK, iter->second.strValidDaysInWeek.c_str());

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

    
    void PlanDssRuleListView::itemDetailsChanged(unsigned long id, const CString& name)
    {
        FUNCTION_ENTRY("itemChanged");

        // We must find the item in our list with it's data set to be the same as the id
        // passed in.

        // Search all lParams stored with each list item for the id we have.
        LVFINDINFO info;
        info.flags = LVFI_PARAM;
        info.lParam = id;

        PlanDssRuleDatabaseAccessor* dbAccessor = dynamic_cast<PlanDssRuleDatabaseAccessor*>(getDatabaseAccessor());
        TA_ASSERT( dbAccessor != NULL, "DB accessor could not be cast to an Alarm Plan Assocation one");

        PlanDssRuleDatabaseAccessor::PlanDssDpRuleText details = dbAccessor->getItemDetails( id );

        // If the item is found then update it with the new details

        int foundItemIndex = GetListCtrl().FindItem(&info);
        if (foundItemIndex != -1)
        {
            if (dbAccessor->getItem(id)->isNew())
            {
                details.status = STATUS_NEW;
            }
            else if (dbAccessor->getItem(id)->hasChanged())
            {
                details.status = STATUS_MODIFIED;
            }
            else
            {
                details.status = STATUS_UPTODATE;
            }

            GetListCtrl().SetItemText(foundItemIndex, COL_APPLIED, details.status.c_str());
            GetListCtrl().SetItemText(foundItemIndex, COL_LOCATION, details.strLocation.c_str());
            GetListCtrl().SetItemText(foundItemIndex, COL_TRIGGER_ENTITY, details.strTriggerEntity.c_str());
            GetListCtrl().SetItemText(foundItemIndex, COL_ACTIVE_PLAN, details.strActivePlan.c_str());
            GetListCtrl().SetItemText(foundItemIndex, COL_MINIMUM_INTERVAL, details.strMinimumInterval.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_ACTIVED_TOLERANT_TIME, details.strActivedTolerantTime.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_DENY_STRATEGY, details.strDenyStrategy.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_ACTIVATE_VALUE, details.strActivateValue.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_NEED_DEACTIVATE, details.strNeedDeactivate.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_ALLOW_MULTIPLE, details.strAllowMultiple.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_ENABLE, details.strEnable.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_VALID_TIME_IN_DAY, details.strValidTimeInDay.c_str());
			GetListCtrl().SetItemText(foundItemIndex, COL_VALID_DAYS_IN_WEEK, details.strValidDaysInWeek.c_str());

        }
        else
        {
            // Add the new details
            details.status = STATUS_NEW;

            int position = GetListCtrl().InsertItem(0,"",0);
            GetListCtrl().SetItemText(position, COL_APPLIED, details.status.c_str());
            GetListCtrl().SetItemText(position, COL_LOCATION, details.strLocation.c_str());
            GetListCtrl().SetItemText(position, COL_TRIGGER_ENTITY, details.strTriggerEntity.c_str());
            GetListCtrl().SetItemText(position, COL_ACTIVE_PLAN, details.strActivePlan.c_str());
            GetListCtrl().SetItemText(position, COL_MINIMUM_INTERVAL, details.strMinimumInterval.c_str());
			GetListCtrl().SetItemText(position, COL_ACTIVED_TOLERANT_TIME, details.strActivedTolerantTime.c_str());
			GetListCtrl().SetItemText(position, COL_DENY_STRATEGY, details.strDenyStrategy.c_str());
			GetListCtrl().SetItemText(position, COL_ACTIVATE_VALUE, details.strActivateValue.c_str());
			GetListCtrl().SetItemText(position, COL_NEED_DEACTIVATE, details.strNeedDeactivate.c_str());
			GetListCtrl().SetItemText(position, COL_ENABLE, details.strEnable.c_str());
			GetListCtrl().SetItemText(position, COL_VALID_TIME_IN_DAY, details.strValidTimeInDay.c_str());
			GetListCtrl().SetItemText(position, COL_VALID_DAYS_IN_WEEK, details.strValidDaysInWeek.c_str());
            GetListCtrl().SetItemData(position, id);

            // Now select this as the current item
            GetListCtrl().SetItemState(position, 0xFFFFFF, LVIS_SELECTED);
        }

        // Keep our stored map up-to-date so we can perform accurate and quick sorting
        std::map<unsigned long,PlanDssRuleiation>::iterator matching;
        matching = m_currentDisplayedItems.find(id);
        if (matching != m_currentDisplayedItems.end())
        {
            matching->second = details;
        }
        else
        {
            m_currentDisplayedItems.insert( std::map<unsigned long,PlanDssRuleiation>::value_type(id,details) );
        }

        // Now sort the columns again
        m_bAscending = !m_bAscending; // Make sure we sort in the same order. To do this we must change the
                                    // sort order as the sorting method will change it again
        updateHeaderSort(m_sortColumn);
        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        FUNCTION_EXIT;
    }


    void PlanDssRuleListView::copyItem()
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


    void PlanDssRuleListView::newItem()
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
    

    void PlanDssRuleListView::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        updateHeaderSort(pNMListView->iSubItem);

        GetListCtrl().SortItems(compareItems, reinterpret_cast<unsigned long>(this));
	    
	    *pResult = 0;
    }


	void PlanDssRuleListView::updateHeaderSort(const unsigned int column)
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
			m_bAscending = !m_bAscending;
		}
        else
        {
            // If this is a different column reset the sort order to be ascending first.
            m_bAscending = true;
        }

        // Set up ready to add our bitmap
		headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

        /// Load the bitmap based on whether we are sorting ascending or descending
        int resourceToLoad = m_bAscending? IDB_ASCENDING : IDB_DESCENDING;

        headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(PlanDssRuleComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
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


    int CALLBACK PlanDssRuleListView::compareItems(LPARAM index1, LPARAM index2, LPARAM listCtrl)
    {
        PlanDssRuleListView* list = reinterpret_cast<PlanDssRuleListView*>(listCtrl);
        // Double check it has converted ok
        list = dynamic_cast<PlanDssRuleListView*>(list);

        TA_ASSERT(list != NULL, "The wrong type was passed as the third parameter to this method");

        // Now retrieve the data for these items so we can start comparing it
        std::map<unsigned long,PlanDssRuleiation>::iterator item1 = list->m_currentDisplayedItems.find( index1 );
        std::map<unsigned long,PlanDssRuleiation>::iterator item2 = list->m_currentDisplayedItems.find( index2 );

        if (item1 == list->m_currentDisplayedItems.end() || 
            item2 == list->m_currentDisplayedItems.end())
        {
            // Cannot compare
            return 0;
        }

        std::string string1 = "";
        std::string string2 = "";

        switch(list->m_sortColumn)
        {
            case(COL_APPLIED):
                    string1 = item1->second.status;
                    string2 = item2->second.status;
                    break;

            case(COL_LOCATION):
                    string1 = item1->second.strLocation;
                    string2 = item2->second.strLocation;
                    break;

            case(COL_TRIGGER_ENTITY):
                    string1 = item1->second.strTriggerEntity;
                    string2 = item2->second.strTriggerEntity;
                    break;

            case(COL_ACTIVE_PLAN):
                    string1 = item1->second.strActivePlan;
                    string2 = item2->second.strActivePlan;
                    break;

            case(COL_MINIMUM_INTERVAL):
                    string1 = item1->second.strMinimumInterval;
                    string2 = item2->second.strMinimumInterval;
                    break;
			case(COL_ACTIVED_TOLERANT_TIME):
				    string1 = item1->second.strActivedTolerantTime;
					string2 = item2->second.strActivedTolerantTime;
					break;
			case(COL_DENY_STRATEGY):
				    string1 = item1->second.strDenyStrategy;
				    string2 = item2->second.strDenyStrategy;
				    break;
			case(COL_ACTIVATE_VALUE):
				    string1 = item1->second.strActivateValue;
				    string2 = item2->second.strActivateValue;
				    break;
			case(COL_NEED_DEACTIVATE):
				    string1 = item1->second.strNeedDeactivate;
				    string2 = item2->second.strNeedDeactivate;
				    break;
			case(COL_ALLOW_MULTIPLE):
				    string1 = item1->second.strAllowMultiple;
				    string2 = item2->second.strAllowMultiple;
				    break;
			case(COL_ENABLE):
					string1 = item1->second.strEnable;
					string2 = item2->second.strEnable;
					break;
			case(COL_VALID_TIME_IN_DAY):
				    string1 = item1->second.strValidTimeInDay;
				    string2 = item2->second.strValidTimeInDay;
				    break;
			case(COL_VALID_DAYS_IN_WEEK):
					string1 = item1->second.strValidDaysInWeek;
					string2 = item2->second.strValidDaysInWeek;
					break;
        }
        
        int returnVal = string1.compare(string2);

        // If we are sorting descending then switch the sign of the returnVal
        if (!list->m_bAscending)
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }

    
    void PlanDssRuleListView::OnDraw(CDC* pDC)
    {
	    CDocument* pDoc = GetDocument();
    }


    /////////////////////////////////////////////////////////////////////////////
    // PlanDssRuleListView diagnostics

    #ifdef _DEBUG
    void PlanDssRuleListView::AssertValid() const
    {
	    AbstractListView::AssertValid();
    }

    void PlanDssRuleListView::Dump(CDumpContext& dc) const
    {
	    AbstractListView::Dump(dc);
    }
    #endif //_DEBUG

    MessageSender::ConfigUpdateType PlanDssRuleListView::getConfigMessageType() const
    {
        return MessageSender::ALARM_PLAN_ASSOCIATION;
    }
}

