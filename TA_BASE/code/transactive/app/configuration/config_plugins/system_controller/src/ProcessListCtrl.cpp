/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/ProcessListCtrl.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manages the list display used to show the system controller's managed processes
  */

#include "app/configuration/config_plugins/system_controller/src/stdafx.h"
#include "app/configuration/config_plugins/system_controller/src/ProcessListCtrl.h"
#include "app/configuration/config_plugins/system_controller/src/resource.h"
#include "app/configuration/config_plugins/system_controller/src/AddProcessDialog.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/AbstractDetailView.h"

#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"

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
    const CString ProcessListCtrl::PROCESS_NAME_COLUMN_NAME   = "Process Name";
    const int     ProcessListCtrl::PROCESS_NAME_COLUMN_WIDTH  = 370;
    const int     ProcessListCtrl::PROCESS_NAME_COLUMN        = 0;
    const CString ProcessListCtrl::PROCESS_OPERATING_MODE_COLUMN_NAME  = "Default Operating Mode";
    const int     ProcessListCtrl::PROCESS_OPERATING_MODE_COLUMN_WIDTH = 170;
    const int     ProcessListCtrl::PROCESS_OPERATING_MODE_COLUMN       = 1;

    const std::string ProcessListCtrl::CONTROL_MODE = "Control";
    const std::string ProcessListCtrl::MONITOR_MODE = "Monitor";
    
    ProcessListCtrl::ProcessListCtrl() : m_currentSystemController(NULL), CEditableListCtrl(true)
    {
	    // We will manage the cell requests
	    SetManager(this);
    }


    ProcessListCtrl::~ProcessListCtrl()
    {
    }


BEGIN_MESSAGE_MAP(ProcessListCtrl, CEditableListCtrl)
	//{{AFX_MSG_MAP(ProcessListCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    void ProcessListCtrl::PreSubclassWindow() 
    {
	    setupList();
	    
	    CEditableListCtrl::PreSubclassWindow(); 
    }


    void ProcessListCtrl::setupList()
    {
        FUNCTION_ENTRY("setupList");

        SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | 
		                                      LVS_EDITLABELS | LVS_EX_SUBITEMIMAGES);

        // Insert columns into the attribute list ready for data to be inserted
        InsertColumn(PROCESS_NAME_COLUMN,  PROCESS_NAME_COLUMN_NAME,  LVCFMT_LEFT, PROCESS_NAME_COLUMN_WIDTH);
        InsertColumn(PROCESS_OPERATING_MODE_COLUMN, PROCESS_OPERATING_MODE_COLUMN_NAME, LVCFMT_LEFT ,PROCESS_OPERATING_MODE_COLUMN_WIDTH);

        FUNCTION_EXIT;
    }


    void ProcessListCtrl::populateControl(TA_Base_Core::IConfigSystemController& currentSystemController)
    {
        FUNCTION_ENTRY("populateControl");

        m_currentSystemController = &currentSystemController;

        // Remove all old attributes and add the new ones
        DeleteAllItems();

        try
        {
            using TA_Base_Core::IConfigSystemController;

            IConfigSystemController::ProcessMap processes = m_currentSystemController->getProcesses();
            
            SystemControllerDatabaseAccessor dbAccessor;
            std::map<unsigned long, std::string> processNames = dbAccessor.getAllManagedProcesses();

            for( IConfigSystemController::ProcessMap::iterator iter = processes.begin();
                 iter != processes.end();
                 ++iter)
            {
                // Insert the process name in the first column
                std::string processName("");
                std::map<unsigned long, std::string>::iterator matching = processNames.find( iter->first );
                if (matching != processNames.end() )
                {
                    processName = matching->second;
                }

                int position = InsertItem(-1,processName.c_str(), 0);
                SetItemData(position, iter->first);

                if (iter->second == IConfigSystemController::MONITOR)
                {
                    SetItemText(position,PROCESS_OPERATING_MODE_COLUMN, MONITOR_MODE.c_str());
                }
                else
                {
                    SetItemText(position,PROCESS_OPERATING_MODE_COLUMN, CONTROL_MODE.c_str());
                }
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


    void ProcessListCtrl::addNewItems()
    {
        FUNCTION_ENTRY("addNewItems");

	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");

        AddProcessDialog dlg(*m_currentSystemController);
        if (dlg.DoModal() == IDCANCEL)
        {
            FUNCTION_EXIT;
            return;
        }

        std::vector<AddProcessDialog::Process>& processesToAdd = dlg.getProcessesToAdd();

        for ( std::vector<AddProcessDialog::Process>::iterator iter = processesToAdd.begin();
              iter != processesToAdd.end(); ++iter)
        {
              try
              {
                  m_currentSystemController->setProcess( iter->key, iter->operatingMode, true );
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
        }

        // Then repopulate the control to incorporate the changes
        populateControl( *m_currentSystemController );

        FUNCTION_EXIT;
    }


    void ProcessListCtrl::removeSelectedItem()
    {
        FUNCTION_ENTRY("removeSelectedItem");

        POSITION pos = GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            FUNCTION_EXIT;
            return;
        }

        int selected = GetNextSelectedItem(pos);

	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");

        unsigned long key = GetItemData(selected);

        CString currentValue = GetItemText(selected, PROCESS_OPERATING_MODE_COLUMN);

        TA_Base_Core::IConfigSystemController::OperatingMode currentMode = TA_Base_Core::IConfigSystemController::MONITOR;
        if ( 0 == currentValue.Compare(CONTROL_MODE.c_str()) )
        {
            currentMode = TA_Base_Core::IConfigSystemController::CONTROL;
        }

        try
        {
            m_currentSystemController->setProcess(key, currentMode, false);

            // Then repopulate the control to incorporate the changes
            populateControl( *m_currentSystemController );
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

            
    ICellManager::CellType ProcessListCtrl::GetCellType(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");
	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");

	    // Check if item is valid  (We can get -1 sometimes but not sure why, should check editable list control)
	    if (item < 0)
        {
            return CT_NOTEDITABLE;
        }

	    // Only value column is editable
	    if (subItem != PROCESS_OPERATING_MODE_COLUMN)
        {
            return CT_NOTEDITABLE;
        }

        return CT_SELECTION;
    }


    unsigned long ProcessListCtrl::GetCellMaximumCharacters(CEditableListCtrl* listCtrl, int item, int subItem)
    {
        return 0;
    }

            
    ICellManager::CellData ProcessListCtrl::GetCellData(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    ICellManager::CellData data;
        data.push_back( CONTROL_MODE );
        data.push_back( MONITOR_MODE );

	    return data;
    }


    CString ProcessListCtrl::GetCellMask(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return "";
    }


    char ProcessListCtrl::GetCellMaskPromptChar(CEditableListCtrl* listCtrl, int item, int subItem)
    {
	    TA_ASSERT(false, "There are no cells of type CT_MASK");

        return 0;
    }


    bool ProcessListCtrl::UpdateCellValue(CEditableListCtrl* listCtrl, int item, int subItem, CString value)
    {
	    TA_ASSERT (m_currentSystemController != NULL,"We do not yet have an IConfigSystemController* item set");
        TA_ASSERT(listCtrl != NULL, "List control passed is NULL");

        using TA_Base_Core::IConfigSystemController;

        unsigned long processKey = listCtrl->GetItemData(item);

        IConfigSystemController::OperatingMode newMode = IConfigSystemController::MONITOR;
        if ( 0 == value.Compare(CONTROL_MODE.c_str()) )
        {
            newMode = IConfigSystemController::CONTROL;
        }

        try
        {
            m_currentSystemController->setProcess(processKey, newMode, true);

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

