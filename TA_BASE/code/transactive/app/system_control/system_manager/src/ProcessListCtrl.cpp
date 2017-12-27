/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ProcessListCtrl.cpp $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
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
#include "AdminManager.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"
#include "core/utilities/src/DateFormat.h" // TD12474
#include <afxcmn.h>
#include <sstream>
#include <boost/lexical_cast.hpp>

namespace TA_Base_App
{
    ProcessListCtrl::ProcessListCtrl()
        : CListCtrl(),
          m_isFrozen(false),
          m_sortedCol(PROCESS_ID_COL),
          m_sortAscending(true)
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
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    void ProcessListCtrl::populate(const std::vector<TA_Base_Core::ProcessData>& processes)
    {
        if (m_isFrozen)
        {
            return;
        }

        DeleteAllItems();

        for (unsigned int i = 0; i < processes.size(); i++)
        {
            bool isManagedProcess = AdminManager::getInstance().isManagedProcessFromName(processes[i].ProcessId.in());
            int pos = this->InsertItem(i, AdminManager::getInstance().getEntityDescription(processes[i].ProcessId.in()).c_str());
            this->SetItemData(pos, pos);
            this->SetItemText(pos, HOST_COL, processes[i].host);
            populateProcessState(pos, processes[i].state, processes[i].started, isManagedProcess);
            populateProcessOperationMode(pos, processes[i].operationMode);
            populateProcessNewState(pos, processes[i].requestedState, isManagedProcess);
            this->SetItemText(pos, RESTARTS_COL, boost::lexical_cast<std::string>(processes[i].restarts).c_str());
            this->SetItemText(pos, DEBUG_LEVEL_COL, processes[i].debugLevel.in());
        }

        sortListControl();
    }


    void ProcessListCtrl::updateChange(const std::vector<TA_Base_Core::ProcessData>& processData)
    {
        for (size_t i = 0; i < processData.size(); ++i)
        {
            bool isManagedProcess = AdminManager::getInstance().isManagedProcessFromName(processData[i].ProcessId.in());
            int pos = findIndex(m_processes[i].ProcessId.in());

            if (-1 == pos)
            {
                continue;
            }

            if (strcmp(m_processes[i].ProcessId.in(), processData[i].ProcessId.in()) != 0)
            {
                this->SetItemText(pos, PROCESS_ID_COL, AdminManager::getInstance().getEntityDescription(processData[i].ProcessId.in()).c_str());
            }

            if (strcmp(m_processes[i].host.in(), processData[i].host.in()) != 0)
            {
                this->SetItemText(pos, HOST_COL, processData[i].host.in());
            }

            if (m_processes[i].operationMode != processData[i].operationMode)
            {
                populateProcessOperationMode(pos, processData[i].operationMode);
            }

            if ((m_processes[i].state != processData[i].state) || m_processes[i].started != processData[i].started)
            {
                populateProcessState(pos, processData[i].state, processData[i].started, isManagedProcess);
            }

            if (m_processes[i].requestedState != processData[i].requestedState)
            {
                populateProcessNewState(pos, processData[i].requestedState, isManagedProcess);
            }

            if (m_processes[i].restarts != processData[i].restarts)
            {
                this->SetItemText(pos, RESTARTS_COL, boost::lexical_cast<std::string>(processData[i].restarts).c_str());
            }

            if (strcmp(m_processes[i].debugLevel.in(), processData[i].debugLevel.in()) != 0)
            {
                this->SetItemText(pos, DEBUG_LEVEL_COL, processData[i].debugLevel.in());
            }
        }

        sortListControl();
    }


    int ProcessListCtrl::findIndex(const std::string& processId)
    {
        const std::string& description = AdminManager::getInstance().getDescriptionfromName(processId);

        for (int i = 0; i < this->GetItemCount(); ++i)
        {
            if (0 == strcmp(description.c_str(), GetItemText(i, PROCESS_ID_COL)))
            {
                return i;
            }
        }

        return -1;
    }



    void ProcessListCtrl::populateProcessState(int insertPosition, TA_Base_Core::EProcessStatus state, const CTime& processStartTime, bool isManagedProcess)
    {
        TA_Base_Core::DateFormat dateFormat;
        //std::string dateFormatStr = dateFormat.getDateFormat(TA_Base_Core::DateFormat::DateTimeFormat1);//localization. cuilike. temp.
        std::string dateFormatStr = _T("%Y-%m-%d %H:%M:%S");//localization. cuilike. temp.

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


    void ProcessListCtrl::populateProcessOperationMode(int insertPosition, TA_Base_Core::EOperationMode operationMode)
    {
        switch (operationMode)
        {
        case (TA_Base_Core::Control):
            SetItemText(insertPosition, OPERATION_MODE_COL, SystemManagerDlg::CONTROL.c_str());
            break;

        case (TA_Base_Core::Monitor):
            SetItemText(insertPosition, OPERATION_MODE_COL, SystemManagerDlg::MONITOR.c_str());
            break;

        default:
            SetItemText(insertPosition, OPERATION_MODE_COL, SystemManagerDlg::NOT_APPLICABLE.c_str());
        }
    }


    void ProcessListCtrl::populateProcessNewState(int insertPosition, TA_Base_Core::EProcessStatus state, bool isManagedProcess)
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
        col->mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
        col->fmt = LVCFMT_LEFT;
        col->cx = 250;
        //col->pszText = "Process Name";		//localization cuilike++

        col->pszText = _T("进程名称");
        col->iSubItem = 0;
        int colNum = this->InsertColumn(PROCESS_ID_COL, col);

        col->cx = 120;
        //col->pszText = "Location";//localization cuilike++
        col->pszText = _T("位置");
        colNum = this->InsertColumn(HOST_COL, col);

        col->cx = 120;
        //col->pszText = "Operation Mode";//localization cuilike++
        col->pszText = _T("操作模式");
        colNum = this->InsertColumn(OPERATION_MODE_COL, col);

        col->cx = 150;
        //col->pszText = "Status";//localization cuilike++
        col->pszText = _T("状态");
        colNum = this->InsertColumn(STATUS_COL, col);

        //col->pszText = "Requested Status";//localization cuilike++
        col->pszText = _T("请求状态");
        colNum = this->InsertColumn(NEW_STATUS_COL, col);

        //col->pszText = "Running Since";//localization cuilike++
        col->pszText = _T("开始运行时间");
        colNum = this->InsertColumn(RUNNING_SINCE_COL, col);

        col->cx = 75;
        //col->pszText = "Restarts";//localization cuilike++
        col->pszText = _T("重启次数");
        colNum = this->InsertColumn(RESTARTS_COL, col);

        col->cx = 100;
        //col->pszText = "Debug Level";//localization cuilike++
        col->pszText = _T("日志级别");
        colNum = this->InsertColumn(DEBUG_LEVEL_COL, col);

        delete col;
    }

    void ProcessListCtrl::PreSubclassWindow()
    {
        setupHeader();
        updateSortArrow(0);

        CListCtrl::PreSubclassWindow();
    }

    void ProcessListCtrl::update(const std::vector<TA_Base_Core::ProcessData>& processes)
    {
        if (m_isFrozen)
        {
            return;
        }

        if (processes.empty())
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
        if (m_processes.size() != processes.size())
        {
            populate(processes);
        }
        else
        {
            updateChange(processes);
        }

        m_processes = processes;

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
        return (LPCTSTR)GetItemText(index, PROCESS_ID_COL);
    }

    std::string ProcessListCtrl::getStatus(unsigned int index)
    {
        return (LPCTSTR)GetItemText(index, STATUS_COL);
    }

    bool ProcessListCtrl::select(CStringList& processes)
    {
        bool isSelected = false;
        POSITION pos;

        // Iterate through the list in head-to-tail order.
        for (pos = processes.GetHeadPosition(); pos != NULL;)
        {
            LVFINDINFO itemInfo = {0};
            itemInfo.flags = LVFI_STRING;
            CString tmpProcess = processes.GetNext(pos);
            itemInfo.psz   = tmpProcess.GetBuffer(0);
            int newItem = FindItem(&itemInfo);

            if (newItem != (-1)) // item found
            {
                SetItemState(newItem, LVIS_SELECTED, LVIS_SELECTED);
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
            if (hdn->iItem == m_sortedCol)
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
        CString item1, item2;
        item1 = plist->GetItemText(lParam1, sp->colIndex);
        item2 = plist->GetItemText(lParam2, sp->colIndex);

        // get location name which system manager login
        std::string locationName = AdminManager::getInstance().getLocationName();

        bool isLocalItem1 = false;
        bool isLocalItem2 = false;

        if (-1 != item1.Find(locationName.c_str()))
        {
            isLocalItem1 = true;
        }

        if (-1 != item2.Find(locationName.c_str()))
        {
            isLocalItem2 = true;
        }

        if (true == isLocalItem1 && false == isLocalItem2)
        {
            return -1;
        }
        else if (false == isLocalItem1 && true == isLocalItem2)
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

    BOOL ProcessListCtrl::DeleteAllItems()
    {
        m_processes.clear();
        return CListCtrl::DeleteAllItems();
    }
} // namespace TA_Base_App
