/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ListCellComboBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// ListCellComboBox.cpp : implementation file
//

#include "stdafx.h"
#include "resource.h"
#include "ListCellComboBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListCellComboBox

CListCellComboBox::CListCellComboBox(CWnd *parent, CRect cellRect, int item, int subItem, CString text, ICellManager::CellData data)
    : m_esc(false), m_item(item), m_subItem(subItem)
{
    FUNCTION_ENTRY( "CListCellComboBox" );

    cellRect.top -= 3;
    cellRect.bottom += 200;

    Create(WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | /*CBS_DISABLENOSCROLL |*/ WS_VSCROLL, cellRect, parent, IDC_LIST_EDIT_CTRL);

    for (ICellManager::CellData::iterator iter = data.begin(); iter != data.end(); iter++)
    {
        std::string value = *iter;
        AddString(value.c_str());
    }

    SetFont(parent->GetFont(), FALSE);
    SetCurSel(FindStringExact(-1, text));
    SetFocus();

    ShowWindow(SW_SHOW);

    FUNCTION_EXIT;
}


CListCellComboBox::~CListCellComboBox()
{
    FUNCTION_ENTRY( "~CListCellComboBox" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CListCellComboBox, CComboBox)
    // {{AFX_MSG_MAP(CListCellComboBox)
    ON_WM_CHAR()
    ON_WM_KILLFOCUS()
    ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellComboBox message handlers

BOOL CListCellComboBox::PreTranslateMessage(MSG* pMsg)
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
    return CComboBox::PreTranslateMessage(pMsg);
}


void CListCellComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags)
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

    CComboBox::OnChar(nChar, nRepCnt, nFlags);

    FUNCTION_EXIT;
}


void CListCellComboBox::Update()
{
    FUNCTION_ENTRY( "Update" );

    if (m_esc)
    {
        m_esc = false;
    }
    else
    {
        CString str;
        GetLBText(GetCurSel(), str);

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


void CListCellComboBox::OnKillFocus(CWnd* pNewWnd)
{
    FUNCTION_ENTRY( "OnKillFocus" );

    CComboBox::OnKillFocus(pNewWnd);
    ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


void CListCellComboBox::OnCloseup()
{
    FUNCTION_ENTRY( "OnCloseup" );

    Update();
    ShowWindow(SW_HIDE);

    FUNCTION_EXIT;
}


