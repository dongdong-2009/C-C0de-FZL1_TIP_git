/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/GroupListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the available groups for the system controller
  * in the SystemControllerDetailView class
  */

#pragma warning(disable:4786)

#include <algorithm>
#include <set>

#include "app/configuration/config_plugins/system_controller/src/stdafx.h"
#include "app/configuration/config_plugins/system_controller/src/GroupListCtrl.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.h"
#include "app/configuration/config_plugins/system_controller/src/resource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/data_access_interface/system_controller/src/IConfigSystemController.h"

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
    // These are the set up details for the attribute list control
    const CString GroupListCtrl::GROUP_NAME_COLUMN_NAME   = "Group";
    const int     GroupListCtrl::GROUP_NAME_COLUMN_WIDTH  = 200;
    const int     GroupListCtrl::GROUP_NAME_COLUMN        = 0;
    const CString GroupListCtrl::GROUP_CHECK_COLUMN_NAME  = "Monitor";
    const int     GroupListCtrl::GROUP_CHECK_COLUMN_WIDTH = 75;
    const int     GroupListCtrl::GROUP_CHECK_COLUMN       = 1;

    const std::string GroupListCtrl::BOOLEAN_TRUE = "Yes";
    const std::string GroupListCtrl::BOOLEAN_FALSE = "No";

    GroupListCtrl::GroupListCtrl() : m_currentSystemController(NULL), CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    GroupListCtrl::~GroupListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(GroupListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(GroupListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void GroupListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void GroupListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(GROUP_NAME_COLUMN,  GROUP_NAME_COLUMN_NAME,  LVCFMT_LEFT, GROUP_NAME_COLUMN_WIDTH);
        InsertColumn(GROUP_CHECK_COLUMN, GROUP_CHECK_COLUMN_NAME, LVCFMT_LEFT ,GROUP_CHECK_COLUMN_WIDTH);

        FUNCTION_EXIT;
    }


    void GroupListCtrl::populateControl(TA_Base_Core::IConfigSystemController& currentSystemController)
    {
        FUNCTION_ENTRY("populateControl");

        m_currentSystemController = &currentSystemController;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        try
        {

            // First retrieve all groups and add them into the list
            SystemControllerDatabaseAccessor dbAccessor;
            std::map<unsigned long, std::string> groups = dbAccessor.getAllGroups();

            // Now retrieve all the groups monitored by this system controller
            std::set<unsigned long> monitoredGroups = m_currentSystemController->getMonitoredGroups();

            for (std::map<unsigned long, std::string>::iterator iter = groups.begin(); iter != groups.end(); ++iter)
            {
                if ( iter->first == m_currentSystemController->getGroupId() )
                {
                    // If the group that this System COntroller is assigned to is included then it should
                    // be removed as it cannot monitor itself.
                    if (monitoredGroups.end() != monitoredGroups.find(iter->first) )
                    {
                        m_currentSystemController->setMonitoredGroup( iter->first, false );
                    }

                    // Do not add the group that this System Controller is assigned to.
                    continue;
                }

                // Insert the group name in the first column
                int position = InsertItem(-1,iter->second.c_str(), -1);
                
                // Set the item data to be the key
                SetItemData(position, iter->first);

                // Now see if this group is monitored by the system controller
                bool isChecked = false;
                if (monitoredGroups.end() != monitoredGroups.find(iter->first) )
                {
                    // This is a match so we can check the box
                    isChecked = true;
                }

                // Now ensure the check box gets displayed an is checked correctly and enabled
                showCheckIcon(position, GROUP_CHECK_COLUMN, isChecked, true);
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

        

        FUNCTION_EXIT;
    }


    ICellManager::CellType GroupListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");

	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

	    // Only value column is editable
	    if (subItem != GROUP_CHECK_COLUMN)
        {
            return CT_NOTEDITABLE;
        }

	    return CT_CHECKBOX;
    }


    unsigned long GroupListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }

            
    ICellManager::CellData GroupListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

	    ICellManager::CellData data;

	    return data;
    }


    CString GroupListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char GroupListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool GroupListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        bool isTicked = true;
        if ( 0 == value.CompareNoCase("no") )
        {
            isTicked = false;
        }

        unsigned long groupKey = listCtrl->GetItemData(item);
        
        try
        {
            m_currentSystemController->setMonitoredGroup(groupKey,isTicked);

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
}

