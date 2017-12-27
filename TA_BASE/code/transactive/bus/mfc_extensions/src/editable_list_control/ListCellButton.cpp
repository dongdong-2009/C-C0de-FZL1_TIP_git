/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ListCellButton.cpp : implementation file
//

#include "..\stdafx.h"
//#include "resource.h"
#include "ListCellButton.h"
#include "EditableListCtrl.h"


#define IDC_LIST_EDIT_CTRL 5000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListCellButton

CListCellButton::CListCellButton(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem, CString text, ICellManager::CellData data, CString caption, int buttonWidth, IListCellButtonProcessor* listCellButtonProcessor) 
    : m_parent(parent), m_cellManager(cellManager), m_esc(false), m_item(item), m_subItem(subItem), m_listCellButtonProcessor(listCellButtonProcessor)
{
    ASSERT(m_parent!=NULL);
	ASSERT(m_cellManager!=NULL);
	
	CRect buttonRect(cellRect.right - buttonWidth, cellRect.top, cellRect.right, cellRect.bottom);

    Create(caption, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, buttonRect, parent, IDC_LIST_EDIT_CTRL);

    SetFocus();

    ShowWindow(SW_SHOW);
}

CListCellButton::~CListCellButton()
{
}


BEGIN_MESSAGE_MAP(CListCellButton, CButton)
	//{{AFX_MSG_MAP(CListCellButton)
	ON_WM_KILLFOCUS()
    ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellButton message handlers

BOOL CListCellButton::PreTranslateMessage(MSG* pMsg) 
{
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


void CListCellButton::OnLButtonDown(UINT nFlags, CPoint point)
{
    if (m_esc)
	{
		m_esc = false;
	}
    else
    {
        m_listCellButtonProcessor->processButtonClick(m_item, m_subItem, m_parent, m_cellManager);
    }
}


void CListCellButton::Update() 
{
	
	ShowWindow(SW_HIDE);
}

void CListCellButton::OnKillFocus(CWnd* pNewWnd) 
{
	//CComboBox::OnKillFocus(pNewWnd);
	ShowWindow(SW_HIDE);
}

