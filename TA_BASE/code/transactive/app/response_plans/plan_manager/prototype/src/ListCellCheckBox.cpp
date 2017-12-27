/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ListCellCheckBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ListCellCheckBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ListCellCheckBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCellCheckBox

CListCellCheckBox::CListCellCheckBox(CWnd *parent, CRect cellRect, int item, int subItem, CString text)
    : m_esc(false), m_item(item), m_subItem(subItem)
{
    FUNCTION_ENTRY( "CListCellCheckBox" );

    // ugly hack to crop the tick box
    CRect boxRect = cellRect;
    boxRect.left += 6;
    boxRect.right = boxRect.left + 14;
    Create("", WS_CHILD | BS_AUTOCHECKBOX, boxRect, parent, IDC_LIST_EDIT_CTRL);

    SetCheck(text == "Yes"? 1 : 0);
    SetFocus();

    CDC *cdc = parent->GetDC();
    cdc->FillSolidRect(cellRect, GetSysColor(COLOR_HIGHLIGHT));
    parent->ReleaseDC(cdc);

    ShowWindow(SW_SHOW);

    FUNCTION_EXIT;
}


CListCellCheckBox::~CListCellCheckBox()
{
    FUNCTION_ENTRY( "~CListCellCheckBox" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CListCellCheckBox, CButton)
    // {{AFX_MSG_MAP(CListCellCheckBox)
    ON_WM_KILLFOCUS()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellCheckBox message handlers

BOOL CListCellCheckBox::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    // TODO: Add your specialized code here and/or call the base class
    if( pMsg->message == WM_KEYDOWN )
    {
        if ( pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE )
        {
            m_esc = pMsg->wParam == VK_ESCAPE;
            ShowWindow(SW_HIDE);

            FUNCTION_EXIT;
            return TRUE;                // DO NOT process further
        }
    }

    FUNCTION_EXIT;
    return CButton::PreTranslateMessage(pMsg);
}


void CListCellCheckBox::OnKillFocus(CWnd* pNewWnd)
{
    FUNCTION_ENTRY( "OnKillFocus" );

    CButton::OnKillFocus(pNewWnd);

    // TODO: Add your message handler code here
    Update();

    FUNCTION_EXIT;
}


void CListCellCheckBox::Update()
{
    FUNCTION_ENTRY( "Update" );

    if (m_esc)
    {
        m_esc = false;
    }
    else
    {
        CString str = GetCheck() == 1? "Yes" : "No";

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
    }

    ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


