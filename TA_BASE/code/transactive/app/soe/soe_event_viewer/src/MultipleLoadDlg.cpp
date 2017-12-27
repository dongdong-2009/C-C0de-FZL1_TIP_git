/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/MultipleLoadDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2009/12/03 17:15:17 $
  * Last modified by:  $Author: miao.yu $
  *
  */
// MultipleLoadDlg.cpp : implementation file
//

#include "stdafx.h"

#include "app/soe/soe_event_viewer/src/MultipleLoadDlg.h"

#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/DataException.h"
#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace // unnamed
{
	const int LIST_ITEM_ALL_INDEX(0);
	const std::string LIST_ITEM_ALL_STRING("<È«²¿>");
} // unnamed namespace

/////////////////////////////////////////////////////////////////////////////
// MultipleLoadDlg dialog
/////////////////////////////////////////////////////////////////////////////

MultipleLoadDlg::MultipleLoadDlg(const CString& windowTitle, CWnd* pParent /*=NULL*/)
	: CDialog(MultipleLoadDlg::IDD, pParent),
	  m_windowTitle(windowTitle),
	  m_includeAllItem(false)
{
	//{{AFX_DATA_INIT(MultipleLoadDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void MultipleLoadDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(MultipleLoadDlg)
DDX_Control(pDX, IDC_LOAD_LIST, m_multiList);
//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(MultipleLoadDlg, CDialog)
	//{{AFX_MSG_MAP(MultipleLoadDlg)
ON_BN_CLICKED(IDOK, OnMultiok)
ON_WM_DESTROY()
//}}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Public methods
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
SelectedItems MultipleLoadDlg::getSelection() const
{
	return m_selectedItems;
}

/////////////////////////////////////////////////////////////////////////////
void MultipleLoadDlg::setSelection(const SelectedItems& selection)
{
	m_selectedItems = selection;
}

/////////////////////////////////////////////////////////////////////////////
void MultipleLoadDlg::setItems(const ListItems& items, bool includeAllItem)
{
	m_includeAllItem = includeAllItem;
	m_listItems = items;
}


/////////////////////////////////////////////////////////////////////////////
// MultipleLoadDlg message handlers
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
void MultipleLoadDlg::OnMultiok() 
{
	m_selectedItems.clear();

	// Check if the ALL item is selected (if it is we return an empty selection)
	if(!IsAllItemSelected())
	{
		// Return the selected items
		unsigned int selCount = m_multiList.GetSelCount();
		int* selIndex = new int[selCount];

		m_multiList.GetSelItems(selCount, selIndex);
		for(unsigned int i = 0; i < selCount; i++)
		{
			unsigned long key = m_multiList.GetItemData(selIndex[i]);
			CString tmpStr;
			m_multiList.GetText(selIndex[i],tmpStr);
			m_selectedItems.push_back(key);
		}
		delete[] selIndex;
	}
	
	CDialog::OnOK();
}

/////////////////////////////////////////////////////////////////////////////
BOOL MultipleLoadDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	// Clear the items
	m_multiList.ResetContent();

	// Add the ALL item to the list if specified.
	if(m_includeAllItem)
	{
		m_multiList.InsertString(LIST_ITEM_ALL_INDEX,LIST_ITEM_ALL_STRING.c_str());
		// Set this as selected if no items are selected
		if(m_selectedItems.empty())
		{
			m_multiList.SetSel(LIST_ITEM_ALL_INDEX,TRUE);
		}
	}

	// Set the item list and the selected items
	ListItems::iterator iter;
	for (iter = m_listItems.begin(); iter != m_listItems.end(); iter++)
	{
		int index = 0;
		//int index = m_multiList.AddString(iter->str.c_str());
		index = m_multiList.InsertString( -1, iter->str.c_str() );
		m_multiList.SetItemData(index, iter->pkey);
		// Set this item as selected if necessary
		for(SelectedItems::iterator selIter = m_selectedItems.begin(); selIter != m_selectedItems.end(); ++selIter)
		{
			if (iter->pkey == *selIter)
			{
				m_multiList.SetSel(index, TRUE);
				break;
			}
		}
	}

	// Set the window title
	CDialog::SetWindowText(m_windowTitle);

	return TRUE;  // return TRUE unless you set the focus to a control
				  // EXCEPTION: OCX Property Pages should return FALSE
}

/////////////////////////////////////////////////////////////////////////////
void MultipleLoadDlg::OnDestroy() 
{
	CDialog::OnDestroy();
}

/////////////////////////////////////////////////////////////////////////////
// MultipleSelectionDlg Private helper functions
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
bool MultipleLoadDlg::IsAllItemSelected() const
{
	if(m_includeAllItem)
	{
		// Check if all item is selected
		unsigned int selCount = m_multiList.GetSelCount();
		int* selIndex = new int[selCount];

		m_multiList.GetSelItems(selCount, selIndex);
		for(unsigned int i = 0; i < selCount; i++)
		{
			if(selIndex[i] == LIST_ITEM_ALL_INDEX)
			{
				return true;
			}
		}
	}
	return false;
}
