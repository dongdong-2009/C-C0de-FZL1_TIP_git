/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source: $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/focus_management/XorListSelectionController.cpp $
  * @author Jade Lee
  * @version $Revision: #1 $
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by: $Author: CM $
  * 
  * Accepts a set of list controls as parameters
  * but only allows a single list control to have a selection
  * at any one time
  *
  */

#include "../StdAfx.h"

#pragma warning (disable : 4786 )

#include "bus/mfc_extensions/src/focus_management/XorListSelectionController.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{

    void XorListSelectionController::registerList(CListCtrl* pList)
    {
        if (!m_listControls.insert(std::set<CListCtrl*>::value_type(pList)).second)
        {
            // Item already in the set
            ASSERT(FALSE);
        }
    }
    
    
    void XorListSelectionController::onListItemStateChange(NM_LISTVIEW* pSelType, CListCtrl* pList)
    {
        // Only interested in state changes
    	if (pSelType->uChanged != LVIF_STATE) 
    	{        
    		return;
    	}    
        if (!(pSelType->uNewState & 0x0002))
        {
            // Not a message being actively selected, may ignore
            return;
        }
    
        bool listPresent = false;
    
        for (std::set<CListCtrl*>::iterator it = m_listControls.begin();
                    it != m_listControls.end();
                    it ++)
        {
            if ((*it) != pList)
            {
                // Clear selection on any non-matching list controls
                SelectionHelper::deselectAll(*(*it));
            }
            else
            {
                listPresent = true;
            }
        }
    
        // confirm list is present, otherwise it couldn't be found in our set (unrecognized)
        ASSERT(listPresent);
    }

}   // namespace TA_Base_Bus
