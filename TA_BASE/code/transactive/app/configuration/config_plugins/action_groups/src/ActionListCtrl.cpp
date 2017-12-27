/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/action_groups/src/ActionListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available profiles for the operator
  * in the OperatorDetailView class
  */

#include <algorithm>

#include "app/configuration/config_plugins/action_groups/src/stdafx.h"
#include "app/configuration/config_plugins/action_groups/src/ActionListCtrl.h"
#include "app/configuration/config_plugins/action_groups/src/DutyComponent.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/resources/resource.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/src/IConfigActionGroup.h"
#include "core/data_access_interface/src/IAction.h"
#include "core/data_access_interface/src/ActionAccessFactory.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;
using TA_Base_Core::RunParams;

namespace TA_Base_App
{
    // These are the set up details for the action list control

    const CString ActionListCtrl::ACTION_CHECK_COLUMN_NAME = "";
    const int     ActionListCtrl::ACTION_CHECK_COLUMN_WIDTH = 30;
    const int     ActionListCtrl::ACTION_CHECK_COLUMN = 0;

    const CString ActionListCtrl::ACTION_NAME_COLUMN_NAME = "ÐÐÎª";
    const int     ActionListCtrl::ACTION_NAME_COLUMN_WIDTH = 300;
    const int     ActionListCtrl::ACTION_NAME_COLUMN = 1;

    const CString ActionListCtrl::ACTION_DESCRIPTION_COLUMN_NAME = "ÃèÊö";
    const int     ActionListCtrl::ACTION_DESCRIPTION_COLUMN_WIDTH = 430;
    const int     ActionListCtrl::ACTION_DESCRIPTION_COLUMN = 2;

    const CString ActionListCtrl::BOOLEAN_TRUE = "Yes";
    const CString ActionListCtrl::BOOLEAN_FALSE = "No";

    const bool ActionListCtrl::ASCENDING = true;
    const bool ActionListCtrl::DESCENDING = false;


    ActionListCtrl::ActionListCtrl()
        : m_currentActionGroup(NULL),
          m_sortOrder(DESCENDING), // Set to descending so the first time we sort it switches to ASCENDING
          m_sortColumn(ACTION_CHECK_COLUMN), // This is the column to sort on when the list control gets created
          CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    ActionListCtrl::~ActionListCtrl()
    {
        try
        {
            for(std::vector<TA_Base_Core::IAction*>::iterator iterToDelete = m_actions.begin(); iterToDelete != m_actions.end(); ++iterToDelete)
            {
                if (*iterToDelete != NULL)
                {
                    delete *iterToDelete;
                    *iterToDelete =  NULL;
                }
            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "Unknown", "Caught in the desctructor");
        }
    }


BEGIN_MESSAGE_MAP(ActionListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(ActionListCtrl)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, onColumnclick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ActionListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void ActionListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        m_font.CreatePointFont(90,"ËÎÌå");
        SetFont(&m_font,FALSE);

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES | LVS_EX_INFOTIP);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(ACTION_CHECK_COLUMN,  ACTION_CHECK_COLUMN_NAME,  LVCFMT_LEFT, ACTION_CHECK_COLUMN_WIDTH);
        InsertColumn(ACTION_NAME_COLUMN, ACTION_NAME_COLUMN_NAME, LVCFMT_LEFT ,ACTION_NAME_COLUMN_WIDTH);
        InsertColumn(ACTION_DESCRIPTION_COLUMN, ACTION_DESCRIPTION_COLUMN_NAME, LVCFMT_LEFT ,ACTION_DESCRIPTION_COLUMN_WIDTH);
        
        // On startup the m_sortColumn is set to the column we need to sort on first.
        updateHeaderSort(m_sortColumn);

        FUNCTION_EXIT;
    }


    void ActionListCtrl::populateControl(TA_Base_Core::IConfigActionGroup* currentActionGroup)
    {
        FUNCTION_ENTRY("populateControl");

        TA_ASSERT(currentActionGroup != NULL, "IConfigActionGroup passed was NULL");

        m_currentActionGroup = currentActionGroup;

        AfxGetApp()->DoWaitCursor(1);

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        for(std::vector<TA_Base_Core::IAction*>::iterator iterToDelete = m_actions.begin(); iterToDelete != m_actions.end(); ++iterToDelete)
        {
            if (*iterToDelete != NULL)
            {
                delete *iterToDelete;
                *iterToDelete =  NULL;
            }
        }
        m_actions.clear();


        try
        {
            // First retrieve all profiles and add them into the list
            using TA_Base_Core::IAction;
            m_actions = TA_Base_Core::ActionAccessFactory::getInstance().getAllActions();

            for (unsigned int i = 0; i < m_actions.size(); ++i)
            {
                // Insert something in the first column
                int position = InsertItem(-1,"", -1);
                SetItemText(position, ACTION_NAME_COLUMN, m_actions[i]->getName().c_str() );
                SetItemText(position, ACTION_DESCRIPTION_COLUMN, m_actions[i]->getDescription().c_str() );
                
                // Set the item data to be the position in the vector of actions
                SetItemData(position, i);

                // Now see if this action is associated with the ActionGroup
                bool isChecked = m_currentActionGroup->isActionIncluded( m_actions[i]->getKey() );

                // Now ensure the check box gets displayed and is checked correctly and enabled
                showCheckIcon(position, ACTION_CHECK_COLUMN, isChecked, true);
            }
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
            FUNCTION_EXIT;
            return;
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
            FUNCTION_EXIT;
            return;
        }


        SortItems(compareItems, reinterpret_cast<unsigned long>(this));

        AfxGetApp()->DoWaitCursor(-1);

        FUNCTION_EXIT;
    }


    void ActionListCtrl::changeSelected(bool isSelected)
    {
        FUNCTION_ENTRY("changeSelected");

        // Step through all our selected items
        int numSelected = GetSelectedCount();
        if (numSelected <= 0)
        {
            // Nothing to modify so let's get out of here
            FUNCTION_EXIT;
            return;
        }

        int item = -1; // This will hold the last selected item

        // Update all of the selected items.
        for (int i=0; i < numSelected; i++)
        {
              item = GetNextItem(item, LVNI_SELECTED);

              CString newValue = BOOLEAN_FALSE;
              if (isSelected)
              {
                  newValue = BOOLEAN_TRUE;
              }

              // This will update the Data Access Interface object and then update the actual check box icon.
              if (UpdateCellValue(this,item,ACTION_CHECK_COLUMN,newValue))
              {
                  showCheckIcon(item, ACTION_CHECK_COLUMN, isSelected, true);
              }
        }

        FUNCTION_EXIT;
    }


    ICellManager::CellType ActionListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
	    TA_ASSERT (m_currentActionGroup != NULL,"We do not yet have an IConfigEntity* item set");

	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

	    // Only value column is editable
	    if (subItem != ACTION_CHECK_COLUMN)
        {
            return CT_NOTEDITABLE;
        }

	    return CT_CHECKBOX;
    }


    unsigned long ActionListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }

    
    ICellManager::CellData ActionListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT (m_currentActionGroup != NULL,"We do not yet have an IConfigEntity* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

	    ICellManager::CellData data;

	    return data;
    }


    CString ActionListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char ActionListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool ActionListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT (m_currentActionGroup != NULL,"We do not yet have an IConfigActionGroup* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        bool isTicked = true;
        if ( 0 == value.CompareNoCase(BOOLEAN_FALSE) )
        {
            isTicked = false;
        }

        unsigned int index = listCtrl->GetItemData(item);
        if (index < 0 || index >= m_actions.size())
        {
            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Index stored in the actions list control does not reference a valid action in the vector");
            return false;
        }
        
        try
        {
            if (isTicked)
            {
                m_currentActionGroup->addAction( m_actions[index]->getKey() );
            }
            else
            {
                m_currentActionGroup->removeAction( m_actions[index]->getKey() );
            }

            AbstractDetailView* detailView = dynamic_cast<AbstractDetailView*>(GetParent());
            if (detailView != NULL)
            {
                detailView->refreshButtonAvailability();
            }

            return true;
        }
        catch ( const TA_Base_Core::DatabaseException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DatabaseException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_010001);
        }
        catch ( const TA_Base_Core::DataException& )
        {
            LOG_EXCEPTION_CATCH(SourceInfo, "DataException", "Could not set the data");
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg.showMsgBox(IDS_UE_030001);
        }


        return false;
    }


    void ActionListCtrl::onColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;
	    
        updateHeaderSort(pNMListView->iSubItem);

        SortItems(compareItems, reinterpret_cast<unsigned long>(this));
	    
	    *pResult = 0;
    }


	void ActionListCtrl::updateHeaderSort(const unsigned int column)
	{
		FUNCTION_ENTRY("updateHeaderSort");

		TA_ASSERT(column >= 0, "Column was -ve!");

		//First find if the selected column is currently sorted on. If so, toggle 
		//the sorting arrangment. 
		CHeaderCtrl* headerCtrl = GetHeaderCtrl(); 

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

        headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle(DutyComponent::DLL_NAME), MAKEINTRESOURCE(resourceToLoad), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS);
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


    int CALLBACK ActionListCtrl::compareItems(LPARAM index1, LPARAM index2, LPARAM actionListCtrl)
    {
        ActionListCtrl* actionList = reinterpret_cast<ActionListCtrl*>(actionListCtrl);
        // Double check it has converted ok
        actionList = dynamic_cast<ActionListCtrl*>(actionList);

        TA_ASSERT(actionList != NULL, "The wrong type was passed as the third parameter to this method");

        int returnVal = 0;

        try
        {
            switch(actionList->m_sortColumn)
            {
                case(ACTION_CHECK_COLUMN):
                {
                    unsigned long actionKey = actionList->m_actions[index1]->getKey();
                    bool isChecked1 = actionList->m_currentActionGroup->isActionIncluded( actionKey );
                    actionKey = actionList->m_actions[index2]->getKey();
                    bool isChecked2 = actionList->m_currentActionGroup->isActionIncluded( actionKey );

                    if (!isChecked1 && isChecked2)
                    {
                        returnVal = 1;
                        break;
                    }
                    else if (!isChecked2 && isChecked1)
                    {
                        returnVal = -1;
                        break;
                    }
                    // Otherwise these are the same so continue on (ie don't break) and sort by name
                }
                 
                case(ACTION_NAME_COLUMN):
                {
                    CString name1 = actionList->m_actions[index1]->getName().c_str();
                    CString name2 = actionList->m_actions[index2]->getName().c_str();
                    returnVal = name1.CompareNoCase(name2);
                    break;
                }

                case(ACTION_DESCRIPTION_COLUMN):
                {
                    CString description1 = actionList->m_actions[index1]->getDescription().c_str();
                    CString description2 = actionList->m_actions[index2]->getDescription().c_str();
                    returnVal = description1.CompareNoCase(description2);
                    break;
                }

            }
        }
        catch ( ... )
        {
            LOG_EXCEPTION_CATCH( SourceInfo, "DatabaseException", "Could not retrieve the data to sort it in the list control");
        }

        // If we are sorting descending then switch the sign of the returnVal
        if (actionList->m_sortOrder == DESCENDING)
        {
            returnVal = -returnVal;
        }

        return returnVal;
    }

}



