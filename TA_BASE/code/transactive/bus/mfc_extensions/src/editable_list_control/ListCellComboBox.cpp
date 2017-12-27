/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/editable_list_control/ListCellComboBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// ListCellComboBox.cpp : implementation file
//

#include "..\stdafx.h"
//#include "resource.h"
#include "ListCellComboBox.h"
#include "EditableListCtrl.h"

#define IDC_LIST_EDIT_CTRL 5000

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CListCellComboBox

CListCellComboBox::CListCellComboBox(CEditableListCtrl* parent, ICellManager* cellManager, CRect cellRect, int item, int subItem, CString text, ICellManager::CellData data, BOOL sortable) 
  : m_parent(parent), 
    m_cellManager(cellManager), 
    m_esc(false), 
    m_item(item), 
    m_subItem(subItem),
	m_strCurrentValue(text),
    m_parentUpdated(false),
	m_sortable(sortable)
{
    ASSERT(m_parent!=NULL);
	ASSERT(m_cellManager!=NULL);
	
	cellRect.top -= 3;
    cellRect.bottom += 200;
	
	DWORD dwStyle = WS_CHILD | CBS_DROPDOWNLIST | CBS_SORT | WS_VSCROLL;
	if (FALSE == m_sortable)
	{
		dwStyle = dwStyle & (~CBS_SORT);
	}
    Create( dwStyle, cellRect, parent, IDC_LIST_EDIT_CTRL);


    for (ICellManager::CellData::iterator iter = data.begin(); iter != data.end(); iter++)
    {
        std::string value = *iter;
        AddString(value.c_str());
    }

    SetFont(parent->GetFont(), FALSE);
    SetCurSel(FindStringExact(-1, text));
    SetFocus();

    ShowWindow(SW_SHOW);
}

CListCellComboBox::~CListCellComboBox()
{
}


BEGIN_MESSAGE_MAP(CListCellComboBox, CComboBox)
	//{{AFX_MSG_MAP(CListCellComboBox)
	ON_WM_CHAR()
	ON_WM_KILLFOCUS()
	ON_CONTROL_REFLECT(CBN_CLOSEUP, OnCloseup)
	//TD18272_jinhua++
	ON_WM_CTLCOLOR()
	ON_WM_DESTROY()
	//++TD18272_jinhua
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CListCellComboBox message handlers

BOOL CListCellComboBox::PreTranslateMessage(MSG* pMsg) 
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
	
	return CComboBox::PreTranslateMessage(pMsg);
}

void CListCellComboBox::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	// wenching++
	// bug 812(TD12622)
	if (nChar != VK_RETURN && nChar != VK_ESCAPE)
		ShowDropDown(true);
	// ++wenching
	// bug 812(TD12622)


	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		if( nChar == VK_ESCAPE )
			m_esc = TRUE;
		
		GetParent()->SetFocus();
		return;
	}
	
	CComboBox::OnChar(nChar, nRepCnt, nFlags);
}


void CListCellComboBox::Update() 
{
	if (m_esc)
	{
		m_esc = false;
	}
	else if (!m_parentUpdated)
	{
		// TD #11930
		// Set this flag to true now. This way, if Update() gets called a second
		// time before it has returned from the first call, UpdateCellValue() will
		// not get called twice. This was causing the multiple confirmation dialog
		// problem in the Inspector Panel for this defect.
        m_parentUpdated = true;

        ASSERT(m_cellManager != NULL);
        ASSERT(m_parent != NULL);

		CString str;
		
		if (GetCurSel() >= 0)
		{
			GetLBText(GetCurSel(), str);
		}
		else
		{
			str = "";
		}

		if (m_strCurrentValue != str)
		{
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
			}
			else
			{
				// TD #12167
				// Leave the m_parentUpdated flag set to true. The value in the combo box will not
				// get updated, and will stay as it was from the last successful update.
				int oldSelection = FindString(0, m_parent->GetItemText(m_item, m_subItem));
				if(oldSelection != CB_ERR)
				{
					// make sure the selected index in the combo box is the
					// saved value from the list control.
					SetCurSel(oldSelection);
				}
	            
			}

			// wenching++
			// bug 814 (TD12608)
			m_parentUpdated = false;
			SetCurSel( CB_ERR );
			// ++wenching
			// bug 814 (TD12608)
		}
	}

	ShowWindow(SW_HIDE);
}

void CListCellComboBox::OnKillFocus(CWnd* pNewWnd) 
{
	CString str;

	if (GetCurSel() >= 0)
	{
		GetLBText(GetCurSel(), str);
	}
	else
	{
		str = "";
	}

	// wenching++
	// bug 814 (TD12608)
	// TD #11237
	// check if the value displayed in the cell differs from the value stored in the cell.
	// if it does, we need to apply an update. this allows for correct selection of data if
	// for example a user is focused in the cell, then types a letter to jump to a certain
	// value, and finally de-focuses the control. previously in this case, the control
	// would revert to the selection as it was before the user focused.
	//if(m_parent->GetItemText(m_item, m_subItem).Compare(str) != 0)

	//Modified by Luxiangmei. To fix the issue - in Configuration Editor->AlarmRule->choose an avalanche rule. 
	//In the avalanche children table, the alarm type can't be changed from the Combobox.
	//if( str != "")  
	if( str != "" && str!=m_strCurrentValue)
	{
		Update();
	}
	// ++wenching
	// bug 814 (TD12608)

	CComboBox::OnKillFocus(pNewWnd);

	// TD #11237
	// if this call is made to ShowWindow(SW_HIDE), then the head of the combo box becomes
	// hidden, however the expanded drop-down list remains visible until it is disposed
	// (eg. a selection is made, or escape is pressed). as a result, the list appears as 
	// a floating box rather than an extension of the cell. also since the head of the combo
	// box is larger than the cell it resides in, there is a gap between the cell and the list,
	// making the detached floating box effect all the more apparent.
	//ShowWindow(SW_HIDE);
}

void CListCellComboBox::OnCloseup() 
{
	Update();
	//ShowWindow(SW_HIDE);	
}

//TD18272_jinhua++
HBRUSH CListCellComboBox::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	if (nCtlColor == CTLCOLOR_LISTBOX)
	{
		if (m_listbox.GetSafeHwnd() == NULL)
		{
			TRACE(_T("subclassing listbox\n"));
			m_listbox.SubclassWindow(pWnd->GetSafeHwnd());
		}
	}
	HBRUSH hbr = CComboBox::OnCtlColor(pDC, pWnd, nCtlColor);
	return hbr;
}


void CListCellComboBox::OnDestroy() 
{
	if (m_listbox.GetSafeHwnd() != NULL)
		m_listbox.UnsubclassWindow();
	
	CComboBox::OnDestroy();
}
//++TD18272_jinhua
