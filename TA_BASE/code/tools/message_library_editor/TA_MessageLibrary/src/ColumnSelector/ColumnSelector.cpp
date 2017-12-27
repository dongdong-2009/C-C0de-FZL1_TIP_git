// ColumnSelector.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"
#include "..\ColumnSelector\ColumnSelector.h"
#include "..\MessageGrid\MessageGridView.h"


// ColumnSelector dialog

IMPLEMENT_DYNAMIC(ColumnSelector, cdxCDynamicDialog)

ColumnSelector::ColumnSelector(CWnd* pParent /*=NULL*/)
	: cdxCDynamicDialog(ColumnSelector::IDD, pParent), m_pParent(pParent), m_bDragging(false),
	m_pDragImage(NULL), m_iDragIndex(-1), m_iDropIndex(-1)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

ColumnSelector::~ColumnSelector()
{
	DestroyIcon(m_hIcon);

	if(m_pDragImage)
	{
		m_pDragImage->DeleteImageList();
		delete m_pDragImage;
		m_pDragImage = NULL;
	}
}

void ColumnSelector::DoDataExchange(CDataExchange* pDX)
{
	cdxCDynamicDialog::DoDataExchange(pDX);

	DDX_Control(pDX, IDC_LIST_COLUMN_SELECTOR, m_listColumns);
}


BEGIN_MESSAGE_MAP(ColumnSelector, cdxCDynamicDialog)
	ON_NOTIFY(LVN_BEGINDRAG, IDC_LIST_COLUMN_SELECTOR, OnLvnBegindragListColumnSelector)
	ON_WM_MOUSEMOVE()
	ON_WM_LBUTTONUP()
END_MESSAGE_MAP()


// ColumnSelector message handlers

BOOL ColumnSelector::OnInitDialog()
{
	cdxCDynamicDialog::OnInitDialog();

	AddSzControl(m_listColumns, mdResize, mdResize);
	AddSzControl(IDOK, mdRepos, mdRepos);
	AddSzControl(IDCANCEL, mdRepos, mdRepos);

	if(InitialiseColumnList())
		PopulateColumnList();

	SetIcon(m_hIcon, FALSE);

	return TRUE;  // return TRUE unless you set the focus to a control
	// EXCEPTION: OCX Property Pages should return FALSE
}

bool ColumnSelector::InitialiseColumnList()
{
	CRect rect;

	m_listColumns.SetExtendedStyle(LVS_EX_CHECKBOXES|LVS_EX_FULLROWSELECT);
	
	m_listColumns.GetWindowRect(&rect);
	m_listColumns.InsertColumn(0, _T(""), LVCFMT_LEFT, rect.Width() - 5); //SL

	return true;
}

bool ColumnSelector::PopulateColumnList()
{
	MessageGridView* pParent = (MessageGridView*)m_pParent;
	ColumnHeader* pHeader = NULL;
	int iIndex = 0;
	for(int i = 0, iCount = pParent->m_rgColumnHeaders.GetSize(); i < iCount; i++)
	{
		pHeader = pParent->m_rgColumnHeaders[i];

		if(pHeader->m_bAvailable)
		{
			iIndex = m_listColumns.InsertItem(i, pHeader->m_sTitle, 0);
			m_listColumns.SetItemData(iIndex, pHeader->m_dwColumnID);
			m_listColumns.SetCheck(iIndex, pHeader->m_bVisible);
		}
	}
	return true;
}

void ColumnSelector::OnOK()
{
	MessageGridView* pParent = (MessageGridView*)m_pParent;
	ColumnHeader* pHeader = NULL;
	bool bRefreshGrid = false, bChecked = false;
	DWORD dwColumnID = 0;
	
	for(int i = 0, iCount = m_listColumns.GetItemCount(); i < iCount; i++)
	{
		dwColumnID = m_listColumns.GetItemData(i);
		pHeader = pParent->GetColumnHeader(dwColumnID);
		bChecked = (bool)m_listColumns.GetCheck(i);

		if(pHeader->m_bVisible != bChecked)
		{
			pHeader->m_bVisible = bChecked;
			pParent->MakeColumnVisible(pHeader->m_dwColumnID, pHeader->m_bVisible);
			bRefreshGrid = true;
		}
	}
	
	CArray<DWORD, DWORD> rgdwVisibleCols;
	for(i = 0, iCount = pParent->m_rgColumnHeaders.GetSize(); i < iCount; i++)
	{
		if(pParent->m_rgColumnHeaders[i]->m_bVisible)
			rgdwVisibleCols.Add(pParent->m_rgColumnHeaders[i]->m_dwColumnID);
	}

	for(i = 0, iCount = m_listColumns.GetItemCount(); i < iCount; i++)
	{
		dwColumnID = m_listColumns.GetItemData(i);
		pHeader = pParent->GetColumnHeader(dwColumnID);

		for(int j = 0; j < rgdwVisibleCols.GetSize(); j++)
		{
			if(rgdwVisibleCols[j] == dwColumnID)
				break;
		}
		
		if(j != i && (j < rgdwVisibleCols.GetSize()))
		{
			pParent->MoveColumn(pParent->GetColumnIndex(rgdwVisibleCols[j]), i);
			bRefreshGrid = true;
		}
	}

	for(i = 0, iCount = pParent->m_rgColumnHeaders.GetSize(); i < iCount; i++)
		pParent->MakeColumnVisible(pParent->m_rgColumnHeaders[i]->m_dwColumnID, pParent->m_rgColumnHeaders[i]->m_bVisible);

	if(bRefreshGrid)
		pParent->m_Grid.RedrawWindow();

	cdxCDynamicDialog::OnOK();
}

void ColumnSelector::OnLvnBegindragListColumnSelector(NMHDR *pNMHDR, LRESULT *pResult)
{
	LPNMLISTVIEW pNMLV = reinterpret_cast<LPNMLISTVIEW>(pNMHDR);
	// save the index of the item being dragged in m_iDragIndex
	m_iDragIndex = pNMLV->iItem;
	POINT pt;
	pt.x = 8;
	pt.y = 8;
	// create a drag image

	// Added 7/29/96
	if(m_pDragImage)
		delete m_pDragImage;
	
	m_pDragImage = m_listColumns.CreateDragImage (m_iDragIndex, &pt);
	ASSERT (m_pDragImage);
	// changes the cursor to the drag image (DragMove() is still required in 
	// OnMouseMove())
	VERIFY (m_pDragImage->BeginDrag (0, CPoint (8, 8)));
	VERIFY (m_pDragImage->DragEnter (GetDesktopWindow (), pNMLV->ptAction));
	// set dragging flag
	m_bDragging = TRUE;

	m_iDropIndex = -1;

	// capture all mouse messages
	SetCapture ();
	*pResult = 1;
}

void ColumnSelector::OnMouseMove(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		CPoint pt (point);
		ClientToScreen (&pt);
		// move the drag image
		VERIFY (m_pDragImage->DragMove (pt));
		// unlock window updates
		VERIFY (m_pDragImage->DragShowNolock (FALSE));

		// get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		if(pDropWnd->m_hWnd != m_listColumns.m_hWnd)
			return;

		// convert from screen coordinates to drop target client coordinates
		pDropWnd->ScreenToClient (&pt);
	
		UINT uFlags;		
		// get the item that is below cursor
		m_iDropIndex = ((CListCtrl*)pDropWnd)->HitTest (pt, &uFlags);
		// highlight it
		m_listColumns.SetHotItem(m_iDropIndex);
		
		// redraw item
		m_listColumns.RedrawItems (m_iDropIndex, m_iDropIndex);
		m_listColumns.UpdateWindow ();
		
		// lock window updates
		VERIFY (m_pDragImage->DragShowNolock (TRUE));
	}

	cdxCDynamicDialog::OnMouseMove(nFlags, point);
}

void ColumnSelector::OnLButtonUp(UINT nFlags, CPoint point)
{
	if (m_bDragging)
	{
		// release mouse capture
		VERIFY (::ReleaseCapture ());
		m_bDragging = FALSE;
		// end dragging
		VERIFY(m_pDragImage->DragLeave(GetDesktopWindow()));
		m_pDragImage->EndDrag ();

		CPoint pt (point);
		ClientToScreen (&pt);
		// get the CWnd pointer of the window that is under the mouse cursor
		CWnd* pDropWnd = WindowFromPoint (pt);
		ASSERT (pDropWnd);

		m_listColumns.SetHotItem(-1);

		CString sText = m_listColumns.GetItemText(m_iDragIndex, 0);
		DWORD dwColumnID = m_listColumns.GetItemData(m_iDragIndex);
		BOOL bChecked = m_listColumns.GetCheck(m_iDragIndex);
		
		m_listColumns.DeleteItem(m_iDragIndex);

		if(m_iDropIndex < 0)
			m_iDropIndex = m_listColumns.GetItemCount();

		m_listColumns.InsertItem(m_iDropIndex, sText);
		m_listColumns.SetItemData(m_iDropIndex,dwColumnID);
		m_listColumns.SetCheck(m_iDropIndex, bChecked);
	}

	cdxCDynamicDialog::OnLButtonUp(nFlags, point);
}
