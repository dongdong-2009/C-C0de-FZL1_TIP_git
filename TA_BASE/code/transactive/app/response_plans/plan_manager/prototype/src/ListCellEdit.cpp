/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ListCellEdit.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ListCellEdit.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ListCellEdit.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CListCellEdit

CListCellEdit::CListCellEdit(CWnd *parent, CRect cellRect, int item, int subItem, CString text)
    : m_acceptableNumChars("\b01234567890-."), m_esc(false), m_item(item), m_subItem(subItem)
{
    FUNCTION_ENTRY( "CListCellEdit" );

    Create(ES_AUTOHSCROLL | WS_BORDER | WS_CHILD | ES_LEFT | ES_WANTRETURN, cellRect, parent, IDC_LIST_EDIT_CTRL);

    SetFont(parent->GetFont(), FALSE);
    SetWindowText(text);
    SetSel(0, -1);
    SetFocus();

    ShowWindow(SW_SHOW);

    FUNCTION_EXIT;
}


CListCellEdit::~CListCellEdit()
{
    FUNCTION_ENTRY( "~CListCellEdit" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CListCellEdit, CEdit)
    // {{AFX_MSG_MAP(CListCellEdit)
    ON_CONTROL_REFLECT(EN_CHANGE, OnChange)
    ON_WM_CHAR()
    ON_WM_KILLFOCUS()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellEdit message handlers

void CListCellEdit::OnChange()
{
    FUNCTION_ENTRY( "OnChange" );

    // TODO: If this is a RICHEDIT control, the control will not
    // send this notification unless you override the CEdit::OnInitDialog()
    // function and call CRichEditCtrl().SetEventMask()
    // with the ENM_CHANGE flag ORed into the mask.

    // TODO: Add your control notification handler code here

    FUNCTION_EXIT;
}


BOOL CListCellEdit::PreTranslateMessage(MSG* pMsg)
{
    FUNCTION_ENTRY( "PreTranslateMessage" );

    if( pMsg->message == WM_KEYDOWN )
    {
        if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
        {
            ::TranslateMessage(pMsg);
            ::DispatchMessage(pMsg);

            FUNCTION_EXIT;
            return TRUE;                // DO NOT process further
        }
    }

    FUNCTION_EXIT;
    return CEdit::PreTranslateMessage(pMsg);
}


void CListCellEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
{
    FUNCTION_ENTRY( "OnChar" );

    if( nChar == VK_ESCAPE || nChar == VK_RETURN)
    {
        if( nChar == VK_ESCAPE )
        {
            m_esc = TRUE;
        }

        GetParent()->SetFocus();

        FUNCTION_EXIT;
        return;
    }
    else if(m_numeric && (m_acceptableNumChars.Find(nChar) == -1))
    {
        FUNCTION_EXIT;
        return;
    }


    CEdit::OnChar(nChar, nRepCnt, nFlags);

    FUNCTION_EXIT;
}


void CListCellEdit::OnKillFocus(CWnd* pNewWnd)
{
    FUNCTION_ENTRY( "OnKillFocus" );

    Update();
    CEdit::OnKillFocus(pNewWnd);

    FUNCTION_EXIT;
}


void CListCellEdit::Update()
{
    FUNCTION_ENTRY( "Update" );

    if (m_esc)
    {
        m_esc = false;
    }
    else
    {
        CString str;
        GetWindowText(str);

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


void CListCellEdit::SetNumeric(bool numeric/* = true*/)
{
    FUNCTION_ENTRY( "SetNumeric" );

    m_numeric = numeric;

    FUNCTION_EXIT;
}


