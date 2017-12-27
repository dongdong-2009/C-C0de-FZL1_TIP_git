/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ProcessListCtrl.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * This class is a specialised List View control that
  * displays process information. It is derived from
  * MFC's CListCtrl class.
  *
  */

#pragma warning(disable:4786)

#include "StdAfx.h"
#include "SystemManager.h"
#include "ProcessListCtrl.h"
#include "SystemManagerDlg.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include <afxcmn.h>
#include <sstream>
#include <string>

#include "core/utilities/src/DateFormat.h" // TD12474 
namespace TA_Base_App
{

    ProcessListCtrl::ProcessListCtrl()
        : CListCtrl(),  m_isFrozen(false), m_sortedCol(PROCESS_ID_COL), m_sortAscending(true), m_hostName("")
    {
    }

    ProcessListCtrl::~ProcessListCtrl()
    {
    }

    BEGIN_MESSAGE_MAP(ProcessListCtrl, CListCtrl)
    //{{AFX_MSG_MAP(ProcessListCtrl)
    // NOTE - the ClassWizard will add and remove mapping macros here.
    ON_NOTIFY(HDN_ITEMCLICKA, 0, OnHeaderClicked)  // ASCII 
    ON_NOTIFY(HDN_ITEMCLICKW, 0, OnHeaderClicked)  // Unicode
	ON_NOTIFY_REFLECT(LVN_ITEMCHANGED, OnItemchanged)
    ON_WM_CONTEXTMENU()
    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    void ProcessListCtrl::OnItemchanged(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        AdminManager::getMainWnd()->SendMessage( WM_PROCESS_LIST_SEL_CHANGED, reinterpret_cast<WPARAM>(pNMHDR), reinterpret_cast<LPARAM>(pResult) );

        *pResult = 0;
    }

    void ProcessListCtrl::OnContextMenu(CWnd* pWnd, CPoint point)
    {
        AdminManager::getMainWnd()->SendMessage( WM_TRACK_CONTEX_MENU, reinterpret_cast<WPARAM>(pWnd), reinterpret_cast<LPARAM>(&point) );
    }

    void ProcessListCtrl::populate(const std::vector<ProcessExactData>& processes)
    {
        if (m_isFrozen)
        {
            return;
        }

        DeleteAllItems();
        for (unsigned int i = 0; i < processes.size(); i++)
        {
            int pos = InsertItem( i, processes[i].strProcessName.c_str() );
            SetItemData(pos, pos);
            SetItemText(pos, HOST_COL, processes[i].pOrgData->host);

            bool isManagedProcess = processes[i].bCanBeManagered;

            populateProcessState(pos,processes[i].pOrgData->state, processes[i].pOrgData->started, isManagedProcess);

            populateProcessOperationMode(pos, processes[i].pOrgData->operationMode);

            populateProcessNewState(pos, processes[i].pOrgData->requestedState, isManagedProcess);

			// Populate restarts
            std::ostringstream restarts;
            restarts << processes[i].pOrgData->restarts;
            SetItemText(pos, RESTARTS_COL, restarts.str().c_str());

			// Populate debug level
			SetItemText(pos, DEBUG_LEVEL_COL, processes[i].pOrgData->debugLevel.in());
        }
		sortListControl();
   }


    void ProcessListCtrl::populateProcessState(int insertPosition,
                                               TA_Base_Core::EProcessStatus state,
                                               const CTime& processStartTime,
                                               bool isManagedProcess)
    {
		TA_Base_Core::DateFormat dateFormat; 
		std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);

        switch (state)
        {
        case TA_Base_Core::Unstarted:
            SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::NOT_STARTED.c_str());
            break;
                
        case TA_Base_Core::Startup:
            SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::STARTUP.c_str());

			// TD12474 ++
			//SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format("%d/%m/%Y %H:%M:%S"));
			SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format(dateFormatStr.c_str()));
			// ++ TD12474

            break;

        case TA_Base_Core::RunningControl:
            if (isManagedProcess)
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::RUNNING_CONTROL.c_str());
            }
            else
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::RUNNING.c_str());
            }

            // TD12474 ++
			//SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format("%d/%m/%Y %H:%M:%S"));
			SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format(dateFormatStr.c_str()));
			// ++ TD12474

            break;

        case TA_Base_Core::RunningMonitor:
            if (isManagedProcess)
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::RUNNING_MONITOR.c_str());
            }
            else
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::RUNNING.c_str());
            }

			// TD12474 ++
			//SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format("%d/%m/%Y %H:%M:%S"));
			SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format(dateFormatStr.c_str()));
			// ++ TD12474

            break;

        case TA_Base_Core::GoingToControl:
            if (isManagedProcess)
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::GOING_TO_CONTROL.c_str());
            }
            else
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::RUNNING.c_str());
            }

			// TD12474 ++
			//SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format("%d/%m/%Y %H:%M:%S"));
			SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format(dateFormatStr.c_str()));
			// ++ TD12474

            break;

        case TA_Base_Core::GoingToMonitor:
            if (isManagedProcess)
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::GOING_TO_MONITOR.c_str());
            }
            else
            {
                SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::RUNNING.c_str());
            }

			// TD12474 ++
			//SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format("%d/%m/%Y %H:%M:%S"));
			SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format(dateFormatStr.c_str()));
			// ++ TD12474

            break;

        case TA_Base_Core::Terminating:

            SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::TERMINATING.c_str());
			// TD12474 ++
			//SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format("%d/%m/%Y %H:%M:%S"));
			SetItemText(insertPosition, RUNNING_SINCE_COL, processStartTime.Format(dateFormatStr.c_str()));
			// ++ TD12474

            break;

        case TA_Base_Core::NotRunning:
            SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::NOT_RUNNING.c_str());
            SetItemText(insertPosition, RUNNING_SINCE_COL, "");
            break;

        case TA_Base_Core::Stopped:
            SetItemText(insertPosition, STATUS_COL, SystemManagerDlg::STOPPED.c_str());
            SetItemText(insertPosition, RUNNING_SINCE_COL, "");
            break;
        }
    }


    void ProcessListCtrl::populateProcessOperationMode(int insertPosition,
                                                       TA_Base_Core::EOperationMode operationMode)
    {
        switch(operationMode)
        {
        case(TA_Base_Core::Control):
            SetItemText(insertPosition, OPERATION_MODE_COL, SystemManagerDlg::CONTROL.c_str());
            break;

        case(TA_Base_Core::Monitor):
            SetItemText(insertPosition, OPERATION_MODE_COL, SystemManagerDlg::MONITOR.c_str());
            break;

        default:
            SetItemText(insertPosition, OPERATION_MODE_COL, SystemManagerDlg::NOT_APPLICABLE.c_str());
        }
    }
    
    
    void ProcessListCtrl::populateProcessNewState(int insertPosition,
                                                  TA_Base_Core::EProcessStatus state,
                                                  bool isManagedProcess)
    {
        if (!isManagedProcess)
        {
            // We don't have requested states for non-managed processes so just set this to N/A
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::NOT_APPLICABLE.c_str());
            return;
        }

        switch (state)
        {

        case TA_Base_Core::Unstarted:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::NOT_STARTED.c_str());
            break;
                
        case TA_Base_Core::Startup:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::STARTUP.c_str());
            break;

        case TA_Base_Core::RunningControl:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::RUNNING_CONTROL.c_str());
            break;

        case TA_Base_Core::RunningMonitor:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::RUNNING_MONITOR.c_str());
            break;

        case TA_Base_Core::GoingToControl:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::GOING_TO_CONTROL.c_str());
            break;

        case TA_Base_Core::GoingToMonitor:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::GOING_TO_MONITOR.c_str());
            break;

        case TA_Base_Core::Terminating:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::TERMINATING.c_str());
            break;

        case TA_Base_Core::NotRunning:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::NOT_RUNNING.c_str());
            break;

        case TA_Base_Core::Stopped:
            SetItemText(insertPosition, NEW_STATUS_COL, SystemManagerDlg::STOPPED.c_str());
            break;
        }
    }


    void ProcessListCtrl::setupHeader()
    {
        this->SetExtendedStyle(LVS_EX_HEADERDRAGDROP | LVS_EX_FULLROWSELECT);

        LVCOLUMN* col = new LVCOLUMN;
        col->mask = LVCF_FMT |LVCF_SUBITEM |LVCF_TEXT |LVCF_WIDTH;
        col->fmt = LVCFMT_LEFT;
        col->cx = 250;
        col->pszText = "Process Name";
        col->iSubItem = 0;
        int colNum = this->InsertColumn(PROCESS_ID_COL, col);

        col->cx = 120;
        col->pszText = "Location";
        colNum = this->InsertColumn(HOST_COL, col);

        col->cx = 120;
        col->pszText = "Operation Mode";
        colNum = this->InsertColumn(OPERATION_MODE_COL, col);

        col->cx = 150;
        col->pszText = "Status";
        colNum = this->InsertColumn(STATUS_COL, col);

        col->pszText = "Requested Status";
        colNum = this->InsertColumn(NEW_STATUS_COL, col);

        col->pszText = "Running Since";
        colNum = this->InsertColumn(RUNNING_SINCE_COL, col);

        col->cx = 75;
        col->pszText = "Restarts";
        colNum = this->InsertColumn(RESTARTS_COL, col);

		col->cx = 100;
		col->pszText = "Debug Level";
		colNum = this->InsertColumn(DEBUG_LEVEL_COL, col);

        delete col;
    }

//     void ProcessListCtrl::PreSubclassWindow()
//     {
//         CListCtrl::PreSubclassWindow();
//     }

    void ProcessListCtrl::update( const std::vector<ProcessExactData>& processes)
    {
        if ( 0 == processes.size() )
        {
            return;
        }

        if (m_isFrozen)
        {
            return;
        }

        // Get selected item
        CStringList SelectedProcesses;
        POSITION pos = GetFirstSelectedItemPosition();
        while (pos != NULL)
        {
            SelectedProcesses.AddTail(getProcessName(GetNextSelectedItem(pos)).c_str());
        }

        // Update display
        populate(processes);

        // Reselect items selected before update
        select(SelectedProcesses);
    }

    void ProcessListCtrl::freeze()
    {
        m_isFrozen = true;
    }

    void ProcessListCtrl::unfreeze()
    {
        m_isFrozen = false;
    }

    std::string ProcessListCtrl::getProcessName(unsigned int index)
    {
        return GetItemText(index, PROCESS_ID_COL);
    }

    std::string ProcessListCtrl::getStatus(unsigned int index)
    {
        return GetItemText(index, STATUS_COL);
    }

    bool ProcessListCtrl::select(CStringList& processes)
    {
        bool isSelected = false;
        POSITION pos;

        // Iterate through the list in head-to-tail order.
        for(pos = processes.GetHeadPosition(); pos != NULL; )
        {
            LVFINDINFO itemInfo = {0};
            itemInfo.flags = LVFI_STRING;
            CString tmpProcess = processes.GetNext(pos);
            itemInfo.psz   = tmpProcess.GetBuffer(0);
            int newItem = FindItem(&itemInfo);
            if (newItem != (-1)) // item found
            {
                SetItemState(newItem, LVIS_SELECTED , LVIS_SELECTED);
                isSelected = true;
            }
        }

        return isSelected;
    }

    void ProcessListCtrl::OnHeaderClicked(NMHDR* pNMHDR, LRESULT* pResult)
    {
        HD_NOTIFY* hdn = (HD_NOTIFY*)pNMHDR;

        if (hdn->iButton == 0)
        {
            // Check if the user clicked on header using left mouse button.
            if(hdn->iItem == m_sortedCol)
            {
                m_sortAscending = !m_sortAscending;
            }
            else
            {
                m_sortAscending = true;
            }

            // Update the direction and location of the sort arrow.
            updateSortArrow(hdn->iItem);

			sortListControl();
       }

        *pResult = 0;
    }

	void ProcessListCtrl::sortListControl()
	{
		// Put some parameters into a struct so it can be passed to SortItems.
		SortParams params;
		params.hwnd = this->GetSafeHwnd();
		params.colIndex = m_sortedCol;
		params.sortAscending = m_sortAscending;

		// Update the item data (required by SortItems).
		int nCount = GetItemCount();
		for (int pos = 0; pos < nCount; pos++)
		{
			SetItemData(pos, pos);
		}

		if (m_sortedCol == RESTARTS_COL)
		{
			SortItems((PFNLVCOMPARE)compareIntegers, (DWORD)&params);
		}
		else if (m_sortedCol == RUNNING_SINCE_COL)
		{
			SortItems((PFNLVCOMPARE)compareDates, (DWORD)&params);
		}
		// TD16592 ++
		else if (m_sortedCol == PROCESS_ID_COL)
		{
			SortItems((PFNLVCOMPARE)compareStrings2, (DWORD)&params);
		}
		// ++ TD16592
		else
		{
			SortItems((PFNLVCOMPARE)compareStrings, (DWORD)&params);
		}
	}

    int CALLBACK ProcessListCtrl::compareStrings(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        SortParams* sp = (SortParams*)lParamSort;
	    
        // Get the text out of the required column.
        ProcessListCtrl* plist = (ProcessListCtrl*)CWnd::FromHandle(sp->hwnd);
	    CString item1, item2;
	    item1 = plist->GetItemText(lParam1, sp->colIndex);
	    item2 = plist->GetItemText(lParam2, sp->colIndex);

        // Make the comparison
        if (sp->sortAscending)
        {
            return item1.CompareNoCase(item2);
        }
        else
        {
            return item2.CompareNoCase(item1);
        }   
    }

	// TD16592 ++
	int CALLBACK ProcessListCtrl::compareStrings2(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
	{
		SortParams* sp = (SortParams*)lParamSort;

		// Get the text out of the required column.
		ProcessListCtrl* plist = (ProcessListCtrl*)CWnd::FromHandle(sp->hwnd);
		CString item1;
        CString item2;
		item1 = plist->GetItemText(lParam1, sp->colIndex);
		item2 = plist->GetItemText(lParam2, sp->colIndex);

		bool isLocalItem1 = false;
		bool isLocalItem2 = false;

        if ( -1 != item1.Find(plist->getLocationName().c_str()))
		{
			isLocalItem1 = true;
		}

		if ( -1 != item2.Find(plist->getLocationName().c_str()) )
		{
			isLocalItem2 = true;
		}
		
		if (true == isLocalItem1 && false == isLocalItem2 )
		{
			return -1;
		}
		else if (false == isLocalItem1 && true == isLocalItem2 )
		{
			return 1;
		}
		else
		{
			// Make the comparison
			if (sp->sortAscending)
			{
				return item1.CompareNoCase(item2);
			}
			else
			{
				return item2.CompareNoCase(item1);
			}
		}
	}
	// ++ TD16592

    int CALLBACK ProcessListCtrl::compareIntegers(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        SortParams* sp = (SortParams*)lParamSort;
	    
        // Get the text out of the required column.
        ProcessListCtrl* plist = (ProcessListCtrl*)CWnd::FromHandle(sp->hwnd);
	    CString item1, item2;
	    item1 = plist->GetItemText(lParam1, sp->colIndex);
	    item2 = plist->GetItemText(lParam2, sp->colIndex);

		// Convert them to integers
		int int1, int2;
		int1 = atoi(item1.GetBuffer(0));
		int2 = atoi(item2.GetBuffer(0));

        // Make the comparison
        if (sp->sortAscending)
        {
            return int1 > int2;
        }
        else
        {
            return int2 > int1;
        }   
    }

	int CALLBACK ProcessListCtrl::compareDates(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)
    {
        SortParams* sp = (SortParams*)lParamSort;
	    
        // Get the text out of the required column.
        ProcessListCtrl* plist = (ProcessListCtrl*)CWnd::FromHandle(sp->hwnd);
	    CString item1, item2;
	    item1 = plist->GetItemText(lParam1, sp->colIndex);
	    item2 = plist->GetItemText(lParam2, sp->colIndex);

		// Check if the fields are the right size before converting to a
		// date (they could be blank if the process isn't running).
		if (item1.GetLength() < 19)
		{
			return !(sp->sortAscending);
		}
		else if (item2.GetLength() < 19)
		{
			return sp->sortAscending;
		}

		//TD16179++
		CTime time1;
		CTime time2;

		if (TA_Base_Core::RunParams::getInstance().isSet(RPRAM_KRTCDATEFORMAT))
		{
			//FORMAT: %Y/%m/%d %H:%M:%S - 2006/07/04 11:42:29
			
			// Convert them to dates.
			CTime timeA(atoi(item1.Mid(0, 4)),   // Year
						atoi(item1.Mid(5, 2)),   // Month
						atoi(item1.Mid(8, 2)),   // Day
						atoi(item1.Mid(11, 2)),  // Hour
						atoi(item1.Mid(14, 2)),  // Min
						atoi(item1.Mid(17, 2))); // Sec
			time1 = timeA;

			CTime timeB(atoi(item2.Mid(0, 4)),   // Year
						atoi(item2.Mid(5, 2)),   // Month
						atoi(item2.Mid(8, 2)),   // Day
						atoi(item2.Mid(11, 2)),  // Hour
						atoi(item2.Mid(14, 2)),  // Min
						atoi(item2.Mid(17, 2))); // Sec
			time2 = timeB;
		}
		else
		{
			//FORMAT: %d/%m/%Y %H:%M:%S - 04/07/2006 11:43:05

			// Convert them to dates.
			CTime timeA(atoi(item1.Mid(6, 4)),   // Year
						atoi(item1.Mid(3, 2)),   // Month
						atoi(item1.Mid(0, 2)),   // Day
						atoi(item1.Mid(11, 2)),  // Hour
						atoi(item1.Mid(14, 2)),  // Min
						atoi(item1.Mid(17, 2))); // Sec
			time1 = timeA;

			CTime timeB(atoi(item2.Mid(6, 4)),   // Year
						atoi(item2.Mid(3, 2)),   // Month
						atoi(item2.Mid(0, 2)),   // Day
						atoi(item2.Mid(11, 2)),  // Hour
						atoi(item2.Mid(14, 2)),  // Min
						atoi(item2.Mid(17, 2))); // Sec
			time2 = timeB;
		}
		//++TD16179		
		
        // Make the comparison
        if (sp->sortAscending)
        {
            return time1 > time2;
        }
        else
        {
            return time2 > time1;
        }   
    }

    void ProcessListCtrl::updateSortArrow(int newColumnNum)
    {
        // Add sort arrows to list columns
	    HDITEM HeaderItem; 
	    HeaderItem.mask = HDI_FORMAT | HDI_BITMAP; 
	    CHeaderCtrl* HeaderCtrl = GetHeaderCtrl(); 
        if ( NULL == HeaderCtrl )
        {
            return;
        }
	    HeaderCtrl->GetItem(newColumnNum, &HeaderItem); 
	    if (HeaderItem.hbm != 0) 
	    { 
		    DeleteObject(HeaderItem.hbm); 
		    HeaderItem.hbm = 0; 
	    } 
	    
	    HeaderItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT; 

	    // Load the required image
	    HeaderItem.hbm = (HBITMAP)LoadImage(AfxGetInstanceHandle(), MAKEINTRESOURCE((m_sortAscending) ? IDB_UP : IDB_DOWN), IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS); 

	    // Setup the header with the image
	    HeaderCtrl->SetItem(newColumnNum, &HeaderItem); 
	    if (m_sortedCol != newColumnNum) 
	    {
		    // Remove image from original sort column
            removeSortArrow();

            // Now store the new location of the arrow.
            m_sortedCol = newColumnNum;
        }
    }

    void ProcessListCtrl::removeSortArrow()
    {
   	    HDITEM HeaderItem; 
	    HeaderItem.mask = HDI_FORMAT | HDI_BITMAP; 
	    CHeaderCtrl* HeaderCtrl = GetHeaderCtrl(); 

   		// Remove image from original sort column
		HeaderCtrl->GetItem(m_sortedCol, &HeaderItem); 
		HeaderItem.fmt &= ~(HDF_BITMAP | HDF_BITMAP_ON_RIGHT); 
		if (HeaderItem.hbm != 0) 
		{ 
			DeleteObject(HeaderItem.hbm); 
			HeaderItem.hbm = 0; 
		} 
		HeaderCtrl->SetItem(m_sortedCol, &HeaderItem);
    }

    void ProcessListCtrl::init( const std::string& strHostName )
    {
        m_hostName = strHostName;
        setupHeader();
        updateSortArrow(0);
    }

    std::string& ProcessListCtrl::getLocationName()
    {
        return m_hostName;
    }

} // namespace TA_Base_App
