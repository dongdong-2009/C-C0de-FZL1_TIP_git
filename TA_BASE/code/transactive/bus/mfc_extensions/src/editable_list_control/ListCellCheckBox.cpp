/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellCheckBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ListCellCheckBox.cpp : implementation file
//

#include "..\stdafx.h"
//#include "resource.h"
#include "ListCellCheckBox.h"
#include "EditableListCtrl.h"

#define IDC_LIST_EDIT_CTRL 5000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCellCheckBox

CListCellCheckBox::CListCellCheckBox(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem, CString text) 
  : m_parent(parent), 
    m_cellManager(cellManager), 
    m_esc(false), 
    m_item(item), 
    m_subItem(subItem),
    m_parentUpdated(false)
{
    ASSERT(m_parent!=NULL);
	ASSERT(m_cellManager!=NULL);

    // ugly hack to crop the tick box
    CRect boxRect = cellRect;
    boxRect.left += 6;
    boxRect.right = boxRect.left + 14;
    Create("", WS_CHILD | BS_AUTOCHECKBOX, boxRect, m_parent, IDC_LIST_EDIT_CTRL);
    
    SetCheck(text == "Yes"? 1 : 0);
    SetFocus();

    CDC *cdc = parent->GetDC();
    cdc->FillSolidRect(cellRect, GetSysColor(COLOR_HIGHLIGHT));
    parent->ReleaseDC(cdc);    

    ShowWindow(SW_SHOW);
}

CListCellCheckBox::~CListCellCheckBox()
{
}


BEGIN_MESSAGE_MAP(CListCellCheckBox, CButton)
	//{{AFX_MSG_MAP(CListCellCheckBox)
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellCheckBox message handlers

BOOL CListCellCheckBox::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if( pMsg->message == WM_KEYDOWN )
	{
        if ( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
        {
            m_esc = pMsg->wParam == VK_ESCAPE;
            ShowWindow(SW_HIDE);
		    return TRUE;				// DO NOT process further
        }
	}
	
	return CButton::PreTranslateMessage(pMsg);
}

void CListCellCheckBox::OnKillFocus(CWnd* pNewWnd) 
{
	CButton::OnKillFocus(pNewWnd);
	
	// TODO: Add your message handler code here
	Update();
}

void CListCellCheckBox::Update() 
{
	if (m_esc)
	{
		m_esc = false;
	}
	else if (!m_parentUpdated)
	{
        ASSERT(m_cellManager != NULL);
        ASSERT(m_parent != NULL);

        CString str = GetCheck() == 1? "Yes" : "No";

        if (m_cellManager->UpdateCellValue(m_parent, m_item, m_subItem, str))
        {
		    m_parent->SetItemText(m_item, m_subItem, str);

            LV_DISPINFO dispinfo;
		    dispinfo.hdr.hwndFrom = GetParent()->m_hWnd;
		    dispinfo.hdr.idFrom = GetDlgCtrlID();
		    dispinfo.hdr.code = LVN_ENDLABELEDIT;

		    dispinfo.item.mask = LVIF_TEXT;
		    dispinfo.item.iItem = m_item;
		    dispinfo.item.iSubItem = m_subItem;
		    dispinfo.item.pszText = LPTSTR((LPCTSTR)str);
		    dispinfo.item.cchTextMax = str.GetLength();

            GetParent()->GetParent()->SendMessage(WM_NOTIFY, (WPARAM)GetParent()->GetDlgCtrlID(), (LPARAM)&dispinfo);

            m_parentUpdated = true;
        }
	}

	ShowWindow(SW_HIDE);
}
