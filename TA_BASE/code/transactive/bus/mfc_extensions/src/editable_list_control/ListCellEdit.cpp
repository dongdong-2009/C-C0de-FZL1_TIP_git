/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellEdit.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ListCellEdit.cpp : implementation file
//

#include "..\stdafx.h"
//#include "resource.h"
#include "ListCellEdit.h"
#include "EditableListCtrl.h"

#define IDC_LIST_EDIT_CTRL 5000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCellEdit

CListCellEdit::CListCellEdit(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem, CString text) 
  : m_acceptableNumChars("\b01234567890-."), 
    m_parent(parent), 
    m_cellManager(cellManager), 
    m_esc(false), 
    m_item(item), 
    m_subItem(subItem),
    m_parentUpdated(false)
{
    ASSERT(m_parent!=NULL);
	ASSERT(m_cellManager!=NULL);

    Create(ES_AUTOHSCROLL | WS_BORDER | WS_CHILD | ES_LEFT | ES_WANTRETURN, cellRect, parent, IDC_LIST_EDIT_CTRL);

    SetFont(parent->GetFont(), FALSE);
    SetWindowText(text);
	SetSel(0, -1);
    SetFocus();

    ShowWindow(SW_SHOW);
}

CListCellEdit::~CListCellEdit()
{
}


BEGIN_MESSAGE_MAP(CListCellEdit, CEdit)
	//{{AFX_MSG_MAP(CListCellEdit)
	ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellEdit message handlers

void CListCellEdit::OnChange() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CEdit::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here
	
}

BOOL CListCellEdit::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}	
	return CEdit::PreTranslateMessage(pMsg);
}

void CListCellEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
		{
			m_esc = TRUE;
		}

		GetParent()->SetFocus();
		return;
	}
	else if(m_numeric && (m_acceptableNumChars.Find(nChar) == -1))
	{
		return;
	}

	
	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CListCellEdit::OnKillFocus(CWnd* pNewWnd) 
{
	Update();
	CEdit::OnKillFocus(pNewWnd);	
}

void CListCellEdit::Update() 
{
	if (m_esc)
	{
		m_esc = false;
	}
	else if (!m_parentUpdated)
	{
        ASSERT(m_cellManager != NULL);
        ASSERT(m_parent != NULL);

		CString str;
		GetWindowText(str);

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


void CListCellEdit::SetNumeric(bool numeric/* = true*/)
{
	m_numeric = numeric;
}