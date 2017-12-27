/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellDateTime.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ListCellDateTime.cpp : implementation file
//

#include "..\stdafx.h"
#include "ListCellDateTime.h"
#include "EditableListCtrl.h"

#define IDC_LIST_DATE_TIME_CTRL 5000

#define	WMU_KILLFOCUS	(WM_USER+1)

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListCellDateTime

CListCellDateTime::CListCellDateTime( CEditableListCtrl* parent, ICellManager* cellManager, CRect& cellRect,
		int item, int subItem, CString text, ICellManager::CellType cellType )
  : m_parent(parent), 
    m_cellManager(cellManager), 
    m_esc(false), 
    m_item(item), 
    m_subItem(subItem),
	m_bMouseDown(false),
    m_parentUpdated(false)
{
    ASSERT(m_parent!=NULL);
	ASSERT(m_cellManager!=NULL);
	
	UINT uStyle = WS_CHILD;

	if( cellType == ICellManager::CT_DATE )
		uStyle|= DTS_SHORTDATEFORMAT;
	else if( cellType == ICellManager::CT_TIME )
		uStyle|= DTS_TIMEFORMAT;
	else
		ASSERT(0);

    Create( uStyle, cellRect, parent, IDC_LIST_DATE_TIME_CTRL);

    SetFont(parent->GetFont(), FALSE);
    ShowWindow(SW_SHOW);
    SetFocus();
    m_text = text;
    COleDateTime timeDest;    

    if ( m_text.IsEmpty() == FALSE )
    {
        timeDest.ParseDateTime( m_text );
        SetTime( timeDest );
    }        
}

CListCellDateTime::~CListCellDateTime()
{
}


BEGIN_MESSAGE_MAP(CListCellDateTime, CDateTimeCtrl)
	//{{AFX_MSG_MAP(CListCellDateTime)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_MESSAGE(WMU_KILLFOCUS, OnWmuKillFocus)
	ON_NOTIFY_REFLECT(DTN_CLOSEUP, OnCloseup)
	ON_NOTIFY_REFLECT(DTN_DROPDOWN, OnDropDown)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellDateTime message handlers

BOOL CListCellDateTime::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE || pMsg->wParam == VK_DELETE )
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
		else if( pMsg->wParam == WM_LBUTTONDOWN || pMsg->wParam == WM_LBUTTONUP )
		{
			m_bMouseDown = pMsg->wParam == WM_LBUTTONDOWN;
		}
        
	}
	
	return CDateTimeCtrl::PreTranslateMessage(pMsg);
}

void CListCellDateTime::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_esc = TRUE;

		GetParent()->SetFocus();
		return;
	}
	
	CDateTimeCtrl::OnChar(nChar, nRepCnt, nFlags);
}

// Update the ListView with the new value
//
void CListCellDateTime::Update() 
{
	if (m_esc)
	{
        m_esc = FALSE;
	}
    else if (!m_parentUpdated)
	{
        ASSERT(m_cellManager != NULL);
        ASSERT(m_parent != NULL);

		CString str;

        COleDateTime timeDest;
        DWORD dwResult = 0;

        if ( m_text.IsEmpty() == FALSE )
        {
            timeDest.ParseDateTime( m_text );
        }        
        dwResult = GetTime(timeDest);

        if ( dwResult != 0 )
        {
            // Convert to locale's date/time format
            if ( ( GetStyle() & DTS_TIMEFORMAT ) == DTS_TIMEFORMAT )
            {
                str = timeDest.Format( LOCALE_NOUSEROVERRIDE | VAR_TIMEVALUEONLY, LANG_USER_DEFAULT );
            }
            else
            {
                str = timeDest.Format( LOCALE_NOUSEROVERRIDE | VAR_DATEVALUEONLY, LANG_USER_DEFAULT );
            }
    
            timeDest.ParseDateTime( str );
        }
        
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

void CListCellDateTime::OnKillFocus(CWnd* pNewWnd) 
{
    Update();
	CDateTimeCtrl::OnKillFocus(pNewWnd);

	PostMessage( WMU_KILLFOCUS );

	if( !m_bMouseDown )
		ShowWindow(SW_HIDE);
}

void CListCellDateTime::OnCloseup(NMHDR * pNotifyStruct, LRESULT* result) 
{
	m_bMouseDown = false;
	Update();
	//ShowWindow(SW_HIDE);	
}

void CListCellDateTime::OnDropDown(NMHDR * pNotifyStruct, LRESULT* result) 
{
	m_bMouseDown = true;
	Update();
	//ShowWindow(SW_HIDE);	
}

LRESULT CListCellDateTime::OnWmuKillFocus(WPARAM wParam, LPARAM lParam)
{
	if( !m_bMouseDown )
		ShowWindow(SW_HIDE);
	return 0;
}
