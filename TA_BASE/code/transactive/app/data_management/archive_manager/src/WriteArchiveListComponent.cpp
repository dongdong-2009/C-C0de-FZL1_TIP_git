/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/data_management/archive_manager/src/WriteArchiveListComponent.cpp $
  * @author:  Wenguang
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * List control for the Write Archive Dialog.
  *
  */

// WriteArchiveListComponent.cpp : source file
//

#include <cctype>
#include <string>
#include <cstdio>
#include <memory>

#include "app/data_management/archive_manager/src/stdafx.h"
#include "app/data_management/archive_manager/src/WriteArchiveListComponent.h"
#include "resource.h"
#include "bus/resources/resource.h"
#include "core/utilities/src/DebugUtil.h"

BEGIN_MESSAGE_MAP(WriteArchiveListComponent, CListCtrl)
	//{{AFX_MSG_MAP(WriteListComponent)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnclick) 
	//}}AFX_MSG_MAP
	ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTA, 0, 0xFFFF, OnToolTipText)
    ON_NOTIFY_EX_RANGE(TTN_NEEDTEXTW, 0, 0xFFFF, OnToolTipText)
END_MESSAGE_MAP()

WriteArchiveListComponent::WriteArchiveListComponent():m_nCurrentSortColumn(0),m_fSortAscending(true),m_bToolTipCtrlCustomizeDone(false)
{
}

void WriteArchiveListComponent::OnColumnclick(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;  
	
	if(pNMListView->iSubItem == m_nCurrentSortColumn)  
	{
		m_fSortAscending = !m_fSortAscending;
	}
	else  
	{  
		m_fSortAscending=true;  
		m_nCurrentSortColumn = pNMListView->iSubItem;  		
	}  

	//19726++
	CHeaderCtrl* headerCtrl = this->GetHeaderCtrl();
	
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
		if ( (i == pNMListView->iSubItem) /*&& (sortData.m_sortState.at(columnIndex) != UNKNOWN)*/ )
		{
			// Load the required image
#ifdef _DEBUG
			headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle("ArchiveManager.exe"),
				MAKEINTRESOURCE(m_fSortAscending? IDB_SORT_ASC : IDB_SORT_DESC),
				IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED);
			if (headerItem.hbm != NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[XF] Header: %i => %s", i, m_fSortAscending? "IDB_SORT_ASC" : "IDB_SORT_DESC");
			}
#else
			headerItem.hbm = (HBITMAP)LoadImage(GetModuleHandle("ArchiveManager.exe"),
				MAKEINTRESOURCE(m_fSortAscending? IDB_SORT_ASC : IDB_SORT_DESC),
				IMAGE_BITMAP, 0, 0, LR_LOADMAP3DCOLORS | LR_SHARED);
			if (headerItem.hbm != NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "[XF] Header: %i => %s", i, m_fSortAscending? "IDB_SORT_ASC" : "IDB_SORT_DESC");
			}
#endif
			
			if (headerItem.hbm != NULL)
			{
				headerItem.fmt |= HDF_BITMAP | HDF_BITMAP_ON_RIGHT;
			}
		}
		
		headerCtrl->SetItem( i, &headerItem );
	}
//++19726
	
	SortItems(Compare, (LPARAM)this);  
	
	*pResult   =   0;   
}

int CALLBACK WriteArchiveListComponent::Compare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort)  
{  
	WriteArchiveListComponent*   pV=(WriteArchiveListComponent*)lParamSort;  
	CString   strItem1,   strItem2;  
	LVFINDINFO   info;  
	int   nIndex;  
	info.flags   =   LVFI_PARAM;  
	
	info.lParam   =   lParam1;         
	if((nIndex=pV->FindItem(&info)) != -1)  
	{
		strItem1   =   pV->GetItemText(nIndex, pV->m_nCurrentSortColumn);  
	}
	
	info.lParam   =   lParam2;          
	if((nIndex=pV->FindItem(&info)) != -1)  
	{
		strItem2   =   pV->GetItemText(nIndex, pV->m_nCurrentSortColumn);  
	} 
	
	int iCompRes = strItem1.Compare(strItem2); 
	if(0 == iCompRes)
	{
		return iCompRes;
	}
	else
	{
		// size
		if(2 == pV->m_nCurrentSortColumn)
		{	
			// CL-16719 cancel
// 			strItem1.Replace("MB","");
// 			strItem2.Replace("MB","");

			iCompRes = atof(strItem1) > atof(strItem2) ? 1 : -1 ;	
		}
		//date MM/DD/YYYY
		else if(0 == pV->m_nCurrentSortColumn) 
		{
			strItem1 = strItem1.Right(4) + strItem1.Left(strItem1.GetLength() - 4);
			strItem2 = strItem2.Right(4) + strItem2.Left(strItem1.GetLength() - 4);
			// CL-16719 ++
			iCompRes = strItem1.Compare(strItem2); 
		}
		//else
		else
		{
			iCompRes = strItem1.Compare(strItem2); 
		}
		// ++ CL-16719
//		iCompRes = strItem1.Compare(strItem2); // CL-16719
	}
	
	if(pV->m_fSortAscending)  
		return   iCompRes;  
	else  
		return   iCompRes*-1;  
	
}   

void WriteArchiveListComponent::PreSubclassWindow() 
{
    EnableToolTips();
    CListCtrl::PreSubclassWindow();
}

BOOL WriteArchiveListComponent::OnToolTipText(UINT id, NMHDR* pNMHDR, LRESULT* pResult)
{
    UINT nID = pNMHDR->idFrom;
	
    *pResult = 0;
	
    // We want to disable automatic tooltips completely.
    if( 0 == nID )
    {
        return TRUE;
    }
	
    if(!m_bToolTipCtrlCustomizeDone)
    {
		/*
		When a tooltip was displayed in CWnd::DestroyWindow, an assertion error would occur. As a result, in MFC 7.0, 
		the following member variables were moved from AFX_THREAD_STATE to AFX_MODULE_THREAD_STATE: 
		//*/
		//_AFX_THREAD_STATE* pThreadState = AfxGetThreadState();
		//AFX_MODULE_THREAD_STATE* pThreadState = (AFX_MODULE_THREAD_STATE *)AfxGetThreadState();
		AFX_MODULE_THREAD_STATE* pThreadState = (AFX_MODULE_THREAD_STATE *)AfxGetModuleThreadState();
        CToolTipCtrl *pToolTip = pThreadState->m_pToolTip;
		if(pToolTip != NULL)
		{
			// Set max tip width in pixel.
			// you can change delay time, tip text or background color also. enjoy yourself!
			pToolTip->SetMaxTipWidth(1000); //TD14876
			m_bToolTipCtrlCustomizeDone = TRUE;
		}       
    }
	
    // need to handle both ANSI and UNICODE versions of the message
    TOOLTIPTEXTA* pTTTA = (TOOLTIPTEXTA*)pNMHDR;
    TOOLTIPTEXTW* pTTTW = (TOOLTIPTEXTW*)pNMHDR;
	
    // Extract the row and column numbers.
    int row = ((nID-1) >> 10) & 0x3fffff ;
    int col = (nID-1) & 0x3ff;
    CString strTip;
    strTip = GetItemText( row, col );
	
    // Convert the text as required.
    if(pNMHDR->code == TTN_NEEDTEXTA)
    {
        m_pchTip = std::auto_ptr<TCHAR>( new TCHAR[strTip.GetLength()+1] );
        lstrcpyn( m_pchTip.get(), strTip, strTip.GetLength() );
        m_pchTip.get()[strTip.GetLength()] = NULL;
        pTTTW->lpszText = (WCHAR*)m_pchTip.get();
    }
    else
    {
        m_pwchTip = std::auto_ptr<WCHAR>( new WCHAR[strTip.GetLength()+1] );
        _mbstowcsz( m_pwchTip.get(), strTip, strTip.GetLength() );
        m_pwchTip.get()[strTip.GetLength()] = NULL;
        pTTTW->lpszText = (WCHAR*)m_pwchTip.get();
    }
	
    return TRUE;
}

int WriteArchiveListComponent::OnToolHitTest(CPoint point, TOOLINFO * pTI) const
{
    // Determine the coordinates of the cell.
    int row, col;
    RECT cellrect;
    row = CellRectFromPoint(point, &cellrect, &col );

    if ( row == -1 )
    {
        return -1;
    }

    // Encode the cell coordinates the ID.
    pTI->hwnd = m_hWnd;
    pTI->uId = (UINT)((row<<10)+(col&0x3ff)+1);
    pTI->lpszText = LPSTR_TEXTCALLBACK;
    pTI->rect = cellrect;

    return pTI->uId;
}

int WriteArchiveListComponent::CellRectFromPoint(CPoint & point, RECT * cellrect, int * col) const
{
    int colnum;

    // Make sure that the ListView is in LVS_REPORT
    if( (GetWindowLong(m_hWnd, GWL_STYLE) & LVS_TYPEMASK) != LVS_REPORT )
    {
        return -1;
    }

    // Get the top and bottom row visible
    int row = GetTopIndex();
    int bottom = row + GetCountPerPage();
    if ( bottom > GetItemCount() )
    {
        bottom = GetItemCount();
    }

    // Get the number of columns
    CHeaderCtrl* pHeader = (CHeaderCtrl*)GetDlgItem(0);
    int nColumnCount = pHeader->GetItemCount();

    // Loop through the visible rows
    for ( ; row<=bottom; row++ )
    {
        // Get bounding rect of item and check whether point falls in it.
        CRect rect;
        GetItemRect( row, &rect, LVIR_BOUNDS );
        if ( rect.PtInRect(point) )
        {
            // Now find the column
            for ( colnum = 0; colnum < nColumnCount; colnum++ )
            {
                int colwidth = GetColumnWidth(colnum);
                if ( ( point.x >= rect.left ) && ( point.x <= (rect.left + colwidth ) ) )
                {
                    RECT rectClient;
                    GetClientRect( &rectClient );
                    if ( col ) *col = colnum;
                    rect.right = rect.left + colwidth;

                    // Make sure that the right extent does not exceed the client area
                    if( rect.right > rectClient.right )
                    {
                        rect.right = rectClient.right;
                    }
                    *cellrect = rect;
                    return row;
                }
                rect.left += colwidth;
            }
        }
    }
    return -1;
}

void  WriteArchiveListComponent::AutoResizeColumn()
{

	for(int i = 0;i < this->GetHeaderCtrl()->GetItemCount();++i)
		this->SetColumnWidth(i,LVSCW_AUTOSIZE_USEHEADER);
}