/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Defines a list control that performs automatic sorting (string by default) - can customise
  * to sort numerically for specific columns etc
  *
  * Static definitions are provided so that customised list control can use
  * the sorting functionality without actually having to derive from AutoSortListCtrl
  * (because often we want to derive from a different base list control, and multiple
  *  inheritance is not suitable for these cases)
  * 
  * Classes not wanting to inherit from this class need to maintain a SortInfo structure locally
  *      (call defineSortingSemantic for each column, otherwise defaults to sorting by string)
  *
  * they should declare in the header
    
    protected:
        LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
        
  * and forward the notification on to the static function in the source as follows:   

    LRESULT DerivedClass::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {     
        AutoSortListCtrl::WindowProc(message,
                                     wParam.
                                     lParam,
                                     *this, 
                                     m_colSortingInfo);
        return BaseClass::WindowProc(message, wParam, lParam);
    }
  */

#pragma warning ( disable : 4786 )

#include "bus/mfc_extensions/src/StdAfx.h"
#include "bus/mfc_extensions/src/Resource.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/AutoSortListCtrl.h"
#include "bus/mfc_extensions/src/auto_sort_list_ctrl/ISortingCallback.h"
#include <map>

namespace TA_Base_Bus
{
    static const AutoSortListCtrl::ECurrentSort DEFAULT_SORT_ORDER = AutoSortListCtrl::ASCENDING;
    static const DWORD WM_REAPPLY_SORT = RegisterWindowMessage("AutoSortListCtrl_reSort");
    
    AutoSortListCtrl::SortInfo::SortInfo(int colCount, bool autoResort)
    :
    m_reSortRequired(false),
    m_repostRefreshMessageOnEnable(false),
    m_pSortingCallback(0),
    m_currentSortColumn(-1),
    m_automaticResort(autoResort),
    m_isCurrentlySorting(false)
    {
        m_sortingSemantics.resize(colCount, BY_STRING);
        m_sortState.resize(colCount, UNKNOWN);
    }


    void AutoSortListCtrl::SortInfo::defineSortingSemantic(int column, ESortType sortType)
    {
        m_sortingSemantics.at(column) = sortType;
    }


    void AutoSortListCtrl::SortInfo::setCurrentSort(int column, ECurrentSort sort)
    {
        m_sortState.at(column) = sort;
    }


    void AutoSortListCtrl::SortInfo::activateSort(int column) 
    { 
        m_currentSortColumn = column; 
        m_reSortRequired = true; 
    }


    void AutoSortListCtrl::SortInfo::setCallback(ISortingCallback* callback) 
    { 
        m_pSortingCallback = callback; 
    }


    /// Structure passed into sorting function
    struct SortInformation
    {
        std::vector<CString> columnData;
        AutoSortListCtrl::ESortType sortMethod;
        AutoSortListCtrl::ECurrentSort sortOrder;
        int columnIndex;
        ISortingCallback* pCallback;
    };

    static int CALLBACK 
    sortFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        // Return negative if first item should come first            
        const SortInformation* info = reinterpret_cast<const SortInformation*>(lParamSort);
        
        if (AutoSortListCtrl::BY_CALLBACK == info->sortMethod)
        {
            // Use user-defined sorting method
            if (0 == info->pCallback)
            {
                ASSERT(FALSE);
            }
            return info->pCallback->sortCallback(
                                lParam1,
                                lParam2,
                                info->columnIndex,
                                info->sortOrder);
        }

        ASSERT(info);

        const char* str1 = (LPCTSTR)info->columnData.at(lParam1);
        const char* str2 = (LPCTSTR)info->columnData.at(lParam2);

        int result = 0;

        switch (info->sortMethod)
        {
		case AutoSortListCtrl::BY_MIXED:
		{
			long a1 = atol(str1);
            long a2 = atol(str2);
			if (a1 == 0 && a2 == 0) 
			{
				result = stricmp(str1, str2);
		
				if (0 == result)
				{
					// Go to case sensitive when strings the same
					result = -strcmp(str1, str2);
				}
			}
			else if (a1 != 0 && a2 != 0)
			{
				result = a1 - a2;

				// Cap range to possible avoid integer overflow
				if (result > 0)
				{
					result = 1;
				} 
				else if (result < 0)
				{
					result = -1;
				}
			}
			else if (a1 == 0 && a2 != 0)
			{
				result = 1;
			}
			else
			{
				result = -1;
			}

			break;
		}
        case AutoSortListCtrl::BY_STRING:
       
            // Attempt case insensitive string ordering
            result = stricmp(str1, str2);
            if (0 == result)
            {
                // Go to case sensitive when strings the same
                result = strcmp(str1, str2);
            }
            break;
        
        case AutoSortListCtrl::BY_INTEGER:
        {
            long a1 = atol(str1);
            long a2 = atol(str2);
            
            // If first item less than second, should come first (so return negative value)
            result = a1 - a2;

            // Cap range to possible avoid integer overflow
            if (result > 0)
            {
                result = 1;
            } 
            else if (result < 0)
            {
                result = -1;
            }

            break;
        }               
        default:
            ASSERT(FALSE);            
        }

        switch (info->sortOrder)
        {
        case AutoSortListCtrl::ASCENDING:
            return result;
        case AutoSortListCtrl::DESCENDING:
            return -result;
        default:
            ASSERT(FALSE);
            return result;
        }        
    }


    AutoSortListCtrl::AutoSortListCtrl(int maxColumns)
    :
    m_colSortingInfo(maxColumns)
    {
        // Make the first column the active sorting column - in ascending order
        m_colSortingInfo.setCurrentSort(0, AutoSortListCtrl::ASCENDING);
        m_colSortingInfo.activateSort(0);
    }


    BEGIN_MESSAGE_MAP(AutoSortListCtrl, ListCtrlSelNoFocus)
        //{{AFX_MSG_MAP(AutoSortListCtrl)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()    

    
    LRESULT AutoSortListCtrl::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
    {
        WindowProc(message, wParam, lParam, *this, m_colSortingInfo);
            
        return ListCtrlSelNoFocus::WindowProc(message, wParam, lParam);
    }


    BOOL AutoSortListCtrl::SortOnColumnClick(CListCtrl& listControl, 
                                    NMHDR* pNMHDR, 
                                    LRESULT* pResult,
                                    SortInfo& sortData)
    {
        *pResult = 0;
    
        NMLISTVIEW* view = (NMLISTVIEW*) pNMHDR; 
        int columnIndex = view->iItem;

        if (columnIndex >= 0)
        {
            requestColumnSort(columnIndex, listControl, sortData);
        }
    
        // Allow processing by parent
        return FALSE;
    }
    

    void AutoSortListCtrl::requestColumnSort(
            DWORD columnIndex, 
            CListCtrl& listControl,             
            SortInfo& sortData)
    {
        // Cycle the sorting mode..
        if (columnIndex < sortData.m_sortState.size())
        {
            switch (sortData.m_sortState.at(columnIndex))
            {
            case UNKNOWN: 
                sortData.m_sortState.at(columnIndex) = DEFAULT_SORT_ORDER;
                break;
            case DESCENDING:
                // Switch to ascending order
                sortData.m_sortState.at(columnIndex) = ASCENDING;
                break;
            case ASCENDING:
                // Switch to descending order
                sortData.m_sortState.at(columnIndex) = DESCENDING;
                break;
            default:
                ASSERT(FALSE);
            }
        }

        sortData.m_currentSortColumn = columnIndex;

        requestColumnSort(listControl, sortData);
    }


    void AutoSortListCtrl::requestColumnSort(
            CListCtrl& listControl,            
            const SortInfo& sortData)
    {
        // Save the current item data state (index specific), maps indices to saved item data
        int columnIndex = sortData.m_currentSortColumn;

        SortInformation sortFunctionInfo;
        sortFunctionInfo.pCallback  = sortData.m_pSortingCallback;
        sortFunctionInfo.columnIndex = columnIndex;
        
        // Define the type of sort - default to string if column out of range
        if (columnIndex >= (int)sortData.m_sortingSemantics.size())
        {
            // Default to string
            sortFunctionInfo.sortMethod = BY_STRING;
        }
        else
        {
            sortFunctionInfo.sortMethod = sortData.m_sortingSemantics.at(columnIndex);
        }

        if (columnIndex >= (int)sortData.m_sortingSemantics.size())
        {
            // Default to ascending sort
            sortFunctionInfo.sortOrder = DEFAULT_SORT_ORDER;
        }
        else
        {
            sortFunctionInfo.sortOrder = sortData.m_sortState.at(columnIndex);
        }
        

        // put the sort indicator on the right column
        // get the header control
	    CHeaderCtrl* headerCtrl = listControl.GetHeaderCtrl();

        // get each header item - and put the sort inidicator on the right one
        // make sure it isnt on any other headers
        for (int i = 0; i < headerCtrl->GetItemCount(); i++)
        {
            HDITEM headerItem; 
	        headerItem.mask = HDI_FORMAT | HDI_BITMAP;
	        headerCtrl->GetItem( i, &headerItem );

            // if there was a bitmap set it to null (LR_SHARED cleans up automatically)
	        if (headerItem.hbm != NULL) 
	        {
		        headerItem.hbm = NULL; 
	        }

            headerItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT);

            // if sorting is enabled on this column, and its in a known direction
            if ( (i == columnIndex) && (sortData.m_sortState.at(columnIndex) != UNKNOWN) )
            {
                // Load the required image
#ifdef _DEBUG
                static HBITMAP s_bitMapAsc = (HBITMAP)LoadImage(GetModuleHandle("TA_MFCExtensions_d.dll"),
                    MAKEINTRESOURCE(IDB_SORT_ASC), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED);

                static HBITMAP s_bitMapDesc = (HBITMAP)LoadImage(GetModuleHandle("TA_MFCExtensions_d.dll"),
                    MAKEINTRESOURCE(IDB_SORT_DESC), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED);
#else
                static HBITMAP s_bitMapAsc = (HBITMAP)LoadImage(GetModuleHandle("TA_MFCExtensions.dll"),
                    MAKEINTRESOURCE(IDB_SORT_ASC), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED);

                static HBITMAP s_bitMapDesc = (HBITMAP)LoadImage(GetModuleHandle("TA_MFCExtensions.dll"),
                    MAKEINTRESOURCE(IDB_SORT_DESC), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED);
#endif

				headerItem.hbm = (sortData.m_sortState.at(columnIndex) == ASCENDING) ? s_bitMapAsc : s_bitMapDesc;

                if (headerItem.hbm != NULL)
                {
                    headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
                }
            }

            headerCtrl->SetItem( i, &headerItem );
        }


        if (BY_CALLBACK == sortFunctionInfo.sortMethod)
        {
            if (0 == sortFunctionInfo.pCallback)
            {
                // User hasn't provided sorting callback
                ASSERT(FALSE);
            }
            
            // When sorting using a callback, don't need to do
            // anything special (relating to caching row strings etc)
            listControl.SortItems(sortFunction, reinterpret_cast<DWORD>(&sortFunctionInfo));
        }
        else
        {
            int index;

            sortData.m_itemDataMap.clear();

            for (index = 0; index < listControl.GetItemCount(); ++index )
            {
                DWORD data = listControl.GetItemData(index);
                sortData.m_itemDataMap.insert(std::map<int, DWORD>::value_type(index, data));
            }
            
            // Set the sorting flag to intercept GetItemData notifications that may be
            // requested externally in response to the SetItemData calls
            sortData.m_isCurrentlySorting = true;

            for (index = 0; index < listControl.GetItemCount(); ++index )
            {
                // Refresh the item data to become the control index - to allow
                // our sorting routine to work properly
                listControl.SetItemData(index, index);
    
                // Store the column being sorted's string data
                CString rString = listControl.GetItemText(index, columnIndex);                
                sortFunctionInfo.columnData.push_back(rString);            
            }
    
            // Perform the actual sort - during this we won't get any calls to list control
            listControl.SortItems(sortFunction, reinterpret_cast<DWORD>(&sortFunctionInfo));

            std::map<int, DWORD> tempMap;
            // Turn off the intercept, while remapping item data map
            sortData.m_isCurrentlySorting = false;

            // Update the item data map to reflect new indices (moved around as result of sort)
            for (index = 0; index < listControl.GetItemCount(); ++index )
            {
                // Get the original item data value we stored earlier
                int originalIndex = listControl.GetItemData(index);

                // Obtain original item data value
                std::map<int, DWORD>::iterator itr = sortData.m_itemDataMap.find(originalIndex);

                ASSERT(sortData.m_itemDataMap.end() != itr);
                
                // Remap to restore correspondence between literal item index and it's original item data value
                tempMap.insert(std::map<int, DWORD>::value_type(index, itr->second));
            }

            // Make the change, now using new map, to correct return values from GetItemData
            sortData.m_itemDataMap.swap(tempMap);

            sortData.m_isCurrentlySorting = true;            
                
            // Restore the item data for the control
            for (index = 0; index < listControl.GetItemCount(); ++index )
            {
                // Remember this item data request will be intercepted due to m_isCurrentlySorting,
                // so it's not how it looks (a redundant call, setting the item data to the value it was anyway)
                DWORD originalItemData = listControl.GetItemData(index);
    
                // Restore the itemdata state
                listControl.SetItemData(index, originalItemData);
            }

            // Lose the currently sorting state to revert the GetItemData behaviour
            sortData.m_isCurrentlySorting = false;

            // Don't need this information any more
            sortData.m_itemDataMap.clear();
        }
    }

    
    bool AutoSortListCtrl::isSetToCustomSort(const SortInfo& sortData)
    {
        if (sortData.m_currentSortColumn >= 0 && 
            sortData.m_currentSortColumn < (int)sortData.m_sortingSemantics.size() &&
            BY_CALLBACK == sortData.m_sortingSemantics.at(sortData.m_currentSortColumn))
        {
            return true;
        }
        else
        {
            return false;
        }
    }


    void AutoSortListCtrl::onItemTextChanged(const LVITEM& changeDetails, 
                                                CListCtrl& listControl,
                                                SortInfo& sortData)
    {
        // Check if sorting on active column is a custom sort
        if ( isSetToCustomSort(sortData) )
        {
            // Changes in item text will not affect sorting order, only changes
            // in item data will
            return;
        }

        if (changeDetails.iSubItem != sortData.m_currentSortColumn)
        {
            // Text changing at this cell won't affect the sorting order
            return;
        }

        // Otherwise instruct the control to resort itself before displaying again
        sortData.m_reSortRequired = true;
    }


    void AutoSortListCtrl::onItemDataChanged(const LVITEM& changeDetails, 
                                                CListCtrl& listControl,
                                                SortInfo& sortData)
    {
        // Check if sorting on active column is a custom sort
        if ( isSetToCustomSort(sortData) )
        {
            // Currently sorting by callback for this column, so item data changes
            // should force a redraw
            // instruct the control to resort itself before displaying again
            sortData.m_reSortRequired = true;
        }
    }


    void AutoSortListCtrl::reApplyCurrentSort(CListCtrl& listControl,
                                                const SortInfo& sortData)
    {        
        if (!isSetToCustomSort(sortData))
        {
            // If a custom callback sort is not applied, in certain
            // situations we can return early
            int col = sortData.m_currentSortColumn;
        
            if (col < 0 || 
                col >= (int) sortData.m_sortState.size() ||
                UNKNOWN == sortData.m_sortState.at(col))
            {
                // If unknown sort applied to current column, or out of range, nothing required
                return;
            }
        }
        
        requestColumnSort(listControl, sortData);
    }


    void AutoSortListCtrl::WindowProc(UINT message, 
                                        WPARAM wParam, 
                                        LPARAM lParam,
                                        CListCtrl& listControl,
                                        SortInfo& sortData)
    {    
        if (sortData.m_isCurrentlySorting)
        {
            // While a sort is taking place, we must intercept the LVM_GETITEM
            // notification and provide the proper ItemData value (in case
            // client code interleaves with sorting function, for example in response
            // to SetItemData call)
            if (LVM_GETITEM == message)
            {    
                LV_ITEM* item = reinterpret_cast<LV_ITEM*>(lParam);
                if (LVIF_PARAM & item->mask)
                {
                    std::map<int, DWORD>::iterator itr = sortData.m_itemDataMap.find(item->iItem);
                    
                    ASSERT (sortData.m_itemDataMap.end() != itr);
                    
                    // Determine proper LPARAM value, then strip from mask
                    // (to ensure it's not overwritten later)
                    item->mask = item->mask & (~LVIF_PARAM);
                    item->lParam = itr->second;                    
                }
            }            
        }

        if (WM_REAPPLY_SORT == message)
        {
            if (!AfxGetMainWnd()->IsWindowEnabled())
            {
                // Not allowed to re-sort during modal dialog activation.
                // Otherwise an item index may be invalidated during a call
                // to a MessageBox or other modal display function (and it's
                // very easy to forget that this can happen in client code)
                // Re-post the message (try again later)
                sortData.m_repostRefreshMessageOnEnable = true;
            }
            else
            {
                // Once state of the list control has settled it can re-sort itself.  
                // Previously used the WM_PAINT message
                // to prompt a refresh, but this is not suitable, because it's it received
                // when some resets the re-draw state (using SetRedraw(TRUE)) - 
                // and can cause the re-sort operation to occur mid-way through someone
                // setting item text (and the re-sort may invalidate the item index)
                // NOTE: If a message box (error dialog etc) appears half way through
                // setting item text, the re-sort will be applied, and could screw up the item ordering
                // if the user has set automatic sorting                
                
                // Reset internal states indicate we need to be sorted
                sortData.m_reSortRequired = false;                
                        
                reApplyCurrentSort(listControl, sortData);                            
            }
        }
        else switch (message)
        {
        case LVM_SETITEMTEXT:       // SetItemText 
            {
                // Text in first column may have changed..
                LV_ITEM* item = reinterpret_cast<LV_ITEM*>(lParam);                
                onItemTextChanged(*item, listControl, sortData);
                break;
            }
        case LVM_INSERTITEM:        // FALLTHROUGH (InsertItem call)
            // While it would be nice to return the sorted insertion point
            // at this stage (updating the iItem parameter) - in practice, 
            // the insert item notification is received with an incomplete
            // data set (doesn't use LVITEM structure), meaning the sorted
            // point is unavailable until row contents are fully elaborated
        case LVM_SETITEM:           // SetItem (and SetItemData..)
            {
                // Both notifications need to be handled
                LV_ITEM* item = reinterpret_cast<LV_ITEM*>(lParam);

                // If only setting item data (LVIF_PARAM), may ignore this request
                // (otherwise get recursion, since our sorting routine
                // calls SetItemData!)
                if (LVIF_TEXT & item->mask)
                {               
                    // InsertItem will cause entry into here..     
                    onItemTextChanged(*item, listControl, sortData);                
                }
                if (LVIF_PARAM & item->mask)
                {
                    onItemDataChanged(*item, listControl, sortData);
                }
                break;
            }
        case WM_NOTIFY:             
            {
                NMHDR* pNMHDR;
                pNMHDR = reinterpret_cast<NMHDR*>(lParam);
                
                // Check if user clicked on column header
                // I don't know if this is a bug, but HDN_ITEMCLICK (hashdef which
                // we should use) doesn't respond when there is a click
                // of a header item - it maps to HDN_ITEMCLICKA, but we
                // only ever receive HDN_ITEMCLICKW notifications??
                // Windows converts this to an LVN_COLUMNCLICK
                // notification before reaching an ON_NOTIFY_REFLECT function, so
                // can't really confirm if this is a bug in the windows framework..
                if (HDN_ITEMCLICKW == pNMHDR->code)
                {
                    LRESULT tmp = 0;
                    SortOnColumnClick(listControl, pNMHDR, &tmp, sortData);
                }
                break;
            }       
        }

        if (sortData.m_repostRefreshMessageOnEnable)
        {
            if (AfxGetMainWnd()->IsWindowEnabled())
            {
                // Main window enabled again, may repost
                // Set has been posted flag, so refresh will be posted again (once this flag cleared)
                sortData.m_repostRefreshMessageOnEnable = false;
            }
        }

        // Check if only allowed to post an message on an enable message (if so, 
        // this flag will have been cleared if we have received an enabled message)
        if (!sortData.m_repostRefreshMessageOnEnable)
        {
            if (sortData.m_automaticResort && 
                sortData.m_reSortRequired)
            {
                MSG msg;
                if (0 == ::PeekMessage(&msg, listControl.m_hWnd, WM_REAPPLY_SORT, WM_REAPPLY_SORT, PM_NOREMOVE))
                {                
                    // Only require a single WM_REAPPLY_SORT message to remain in the queue
                    ::PostMessage(listControl.m_hWnd, WM_REAPPLY_SORT, 0, 0);
                }
            }
        }
    } 

} // end namespace TA_Base_Bus
