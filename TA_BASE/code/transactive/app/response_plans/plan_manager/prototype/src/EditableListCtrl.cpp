/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/EditableListCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// EditableListCtrl.cpp : implementation file
//

#include "stdafx.h"
#include "planviewer.h"
#include "ListCellEdit.h"
#include "ListCellComboBox.h"
#include "ListCellCheckBox.h"
#include "EditableListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CEditableListCtrl

CEditableListCtrl::CEditableListCtrl(ICellManager *cellManager)
    : m_cellManager(cellManager), m_cellEditCtrl(NULL)
{
    FUNCTION_ENTRY( "CEditableListCtrl" );

    ASSERT(m_cellManager != NULL);

    FUNCTION_EXIT;
}


CEditableListCtrl::~CEditableListCtrl()
{
    FUNCTION_ENTRY( "~CEditableListCtrl" );

    if (m_cellEditCtrl != NULL)
    {
        delete m_cellEditCtrl;
    }

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CEditableListCtrl, CListCtrl)
    // {{AFX_MSG_MAP(CEditableListCtrl)
    ON_NOTIFY_REFLECT(LVN_BEGINLABELEDIT, OnBeginlabeledit)
    ON_NOTIFY_REFLECT(LVN_ENDLABELEDIT, OnEndlabeledit)
    ON_WM_HSCROLL()
    ON_WM_VSCROLL()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CEditableListCtrl message handlers

void CEditableListCtrl::OnBeginlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnBeginlabeledit" );

    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    // TODO: Add your control notification handler code here

    *pResult = 1; // suppress the default item edit box

    // Get cell coordinates
    LVHITTESTINFO hitTestInfo;
    ::GetCursorPos(&hitTestInfo.pt);
    ScreenToClient(&hitTestInfo.pt);
    SubItemHitTest(&hitTestInfo);

    int item = hitTestInfo.iItem;
    int subItem = hitTestInfo.iSubItem;
    ICellManager::CellType cellType = m_cellManager->GetCellType(GetDlgCtrlID(), item, subItem);

    if (!(hitTestInfo.flags & LVHT_ONITEM))
    {
        FUNCTION_EXIT;
        return;
    }


    if (cellType == ICellManager::CellType::CT_NOTEDITABLE)
    {
        FUNCTION_EXIT;
        return;
    }


    if (EnsureVisible(item, FALSE) == FALSE)
    {
        FUNCTION_EXIT;
        return;
    }


    if (m_cellEditCtrl != NULL)
    {
        delete m_cellEditCtrl;
        m_cellEditCtrl = NULL;
    }

    CRect cellRect;
    GetSubItemRect(item, subItem, LVIR_LABEL, cellRect);

    switch (cellType)
    {
        case ICellManager::CellType::CT_TEXT:
        case ICellManager::CellType::CT_NUMBER:
        {
            CListCellEdit *edit = new CListCellEdit(this, cellRect, item, subItem, GetItemText(item, subItem));
            m_cellEditCtrl = edit;

            edit->SetNumeric(cellType == ICellManager::CellType::CT_NUMBER);
            break;
        }

        case ICellManager::CellType::CT_SELECTION:
        {
            ICellManager::CellData cellData = m_cellManager->GetCellData(GetDlgCtrlID(), item, subItem);
            CListCellComboBox *combo = new CListCellComboBox(this, cellRect, item, subItem, GetItemText(item, subItem), cellData);
            m_cellEditCtrl = combo;

            break;
        }

        case ICellManager::CellType::CT_DECISION:
        {
            CListCellCheckBox *check = new CListCellCheckBox(this, cellRect, item, subItem, GetItemText(item, subItem));
            m_cellEditCtrl = check;

            break;
        }

        default:
            FUNCTION_EXIT;
            return;
    }

    FUNCTION_EXIT;
}


void CEditableListCtrl::OnEndlabeledit(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnEndlabeledit" );

    LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;

    // TODO: Add your control notification handler code here
    ASSERT(m_cellManager != NULL);

    if (pDispInfo->item.pszText != NULL)
    {
        SetItemText(pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
        m_cellManager->UpdateCellValue(GetDlgCtrlID(), pDispInfo->item.iItem, pDispInfo->item.iSubItem, pDispInfo->item.pszText);
    }

    *pResult = 0;

    FUNCTION_EXIT;
}


void CEditableListCtrl::OnHScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    FUNCTION_ENTRY( "OnHScroll" );

    // TODO: Add your message handler code here and/or call default
    if (GetFocus() != this)
    {
        SetFocus();
    }


    CListCtrl::OnHScroll(nSBCode, nPos, pScrollBar);

    FUNCTION_EXIT;
}


void CEditableListCtrl::OnVScroll(UINT nSBCode, UINT nPos, CScrollBar* pScrollBar)
{
    FUNCTION_ENTRY( "OnVScroll" );

    // TODO: Add your message handler code here and/or call default
    if (GetFocus() != this)
    {
        SetFocus();
    }


    CListCtrl::OnVScroll(nSBCode, nPos, pScrollBar);

    FUNCTION_EXIT;
}


