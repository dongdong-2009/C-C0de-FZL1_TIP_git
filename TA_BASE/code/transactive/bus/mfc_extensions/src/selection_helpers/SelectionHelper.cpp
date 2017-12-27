/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/selection_helpers/SelectionHelper.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * SelectionHelper provides a couple of helper functions for tasks related
  * to CListCtrl selection
  *
  */

#include "../stdafx.h"

#include <vector>
#include <algorithm>
#include <sstream>

#include "core/exceptions/src/TransactiveException.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    namespace SelectionHelper
    {

        void deselectAll(CListCtrl& listControl)
        {
            UINT uSelectedCount = listControl.GetSelectedCount();
            UINT i;
            int  nItem = -1;
            
            if (uSelectedCount > 0)
            {
                // Remove all selection masks
                for (i = 0; i < uSelectedCount; i++)
                {
                    nItem = listControl.GetNextItem(nItem, LVNI_SELECTED);
                    
                    // Valid selection should be returned
                    ASSERT(nItem != -1);
                                
                    listControl.SetItemState(nItem, ~LVIS_SELECTED, LVIS_SELECTED);
                }
            }
        }
        
        
        std::vector<unsigned int> getSelectedItemIndices(const CListCtrl& listControl)
        {
            std::vector<unsigned int> result;
        
            UINT uSelectedCount = listControl.GetSelectedCount();
            UINT i;
            int  nItem = -1;
            
            if (uSelectedCount > 0)
            {
                // Remove all selection masks
                for (i = 0; i < uSelectedCount;i++)
                {
                    nItem = listControl.GetNextItem(nItem, LVNI_SELECTED);
                    
                    // Valid selection should be returned
                    ASSERT(nItem != -1);
        
                    result.push_back((unsigned int)(nItem));
                }
            }
        
            return result;
        }


        std::vector<DWORD> getSelectedItemDataValues(const CListCtrl& listControl)
        {
            std::vector<DWORD> result;
        
            UINT uSelectedCount = listControl.GetSelectedCount();
            UINT i;
            int  nItem = -1;
            
            if (uSelectedCount > 0)
            {
                // Remove all selection masks
                for (i = 0; i < uSelectedCount;i++)
                {
                    nItem = listControl.GetNextItem(nItem, LVNI_SELECTED);
                    
                    // Valid selection should be returned
                    ASSERT(nItem != -1);
        
                    result.push_back(listControl.GetItemData(nItem));
                }
            }
        
            return result;
        }


        unsigned int deleteSelectedItems(CListCtrl& listControl)
        {
            // Take the current selection, deleting each item from list
            std::vector<unsigned int> sel = getSelectedItemIndices(listControl);
            
            return deleteItems(listControl, sel);
        }


        unsigned int deleteItems(CListCtrl& listControl, std::vector<unsigned int> sel)
        {
            // First sort list in ascending order
            std::sort(sel.begin(), sel.end());
            
            unsigned int removed = 0;

            // then remove items in reverse order (since indices later in array invalidated)                
            for (std::vector<unsigned int>::reverse_iterator itr = sel.rbegin(); 
                    itr != sel.rend(); 
                    itr ++)
            {
                if ((*itr) >= (unsigned int)listControl.GetItemCount())
                {
                    // Out of range
                    continue;
                }
                
                listControl.DeleteItem(*itr);
                removed ++;                
            }

            return removed;
        }
        

        int findStringInComboCaseSensitive(const CComboBox& combo, const std::string& str)
        {
            int nIndex = -1;
        
            // Because the FindString function is case insensitive, we have to manually have this
            // function to allow case sensitive searches (since the names are case sensitive)
            while(nIndex < combo.GetCount())
            {
                nIndex = combo.FindStringExact(nIndex, str.c_str());
        
                if (LB_ERR == nIndex)
                {
                    break;
                }
            
                CString comboElement; 
                combo.GetLBText(nIndex, comboElement) ;
        
                if (comboElement == str.c_str())
                {
                    // Case sensitive match
                    return nIndex;
                }
        
                // Otherwise only case insensitive match, continue search from nIndex
            }
        
            return LB_ERR;
        }

    } // namespace SelectionHelper

} // namespace TA_Base_Bus
