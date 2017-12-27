/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/MultipleSelectionDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2012/06/18 14:24:00 $
  * Last modified by:  $Author: lu.xiangmei $
  *
  */
// MultipleSelectionDlg.cpp : implementation file
//

#include "stdafx.h"
#include "app/alarm/alarm_manager/src/MultipleSelectionDlg.h"
#include "core\exceptions\src\DatabaseException.h"
#include "core\exceptions\src\DataException.h"
#include "core\utilities\src\DebugUtil.h"
#include <algorithm>
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
// MultipleSelectionDlg dialog
/////////////////////////////////////////////////////////////////////////////

namespace TA_Base_App
{

	MultipleSelectionDlg::MultipleSelectionDlg(const CString& windowTitle, CWnd* pParent /*=NULL*/)
		: CDialog(MultipleSelectionDlg::IDD, pParent),
		  m_windowTitle(windowTitle),
		  m_includeAllItem(false),
          m_sortItems(true)
	{
		//{{AFX_DATA_INIT(MultipleSelectionDlg)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void MultipleSelectionDlg::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(MultipleSelectionDlg)
	DDX_Control(pDX, IDC_MULTI_SEL_LIST, m_multiList);
	//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(MultipleSelectionDlg, CDialog)
		//{{AFX_MSG_MAP(MultipleSelectionDlg)
	ON_BN_CLICKED(ID_MULTIOK, OnMultiok)
	ON_WM_DESTROY()
	//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// Public methods
/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	ListItems MultipleSelectionDlg::getSelection() const
	{
		return m_selectedItems;
	}

	/////////////////////////////////////////////////////////////////////////////
	void MultipleSelectionDlg::setSelection(const ListItems& selection)
	{
		m_selectedItems = selection;
	}

	/////////////////////////////////////////////////////////////////////////////
	void MultipleSelectionDlg::setItems(const ListItems& items, bool includeAllItem)
	{
		m_includeAllItem = includeAllItem;
		m_listItems = items;
	}
	
	class Equal : public std::binary_function<ListItem, std::string, bool>
	{	 
	public :
		bool operator()(const ListItem& _Left, const std::string& _Right) const
		{	 
			if (_Left.str.compare(_Right) == 0)
			{
			   return true;
			}
			else
			{
			   return false;
			}
		}
	};


/////////////////////////////////////////////////////////////////////////////
// MultipleSelectionDlg message handlers
/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	void MultipleSelectionDlg::OnMultiok() 
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
				//unsigned long key = m_multiList.GetItemData(selIndex[i]);
				CString tmpStr;
				m_multiList.GetText(selIndex[i],tmpStr);
				ListItems::iterator itr = find_if(m_listItems.begin(),m_listItems.end(),std::bind2nd(Equal(),tmpStr));
				//ListItem item(key,LPCSTR(tmpStr));
				if(itr != m_listItems.end())
				{
					m_selectedItems.push_back(*itr);
				}
			}
			delete[] selIndex;
		}
		
		CDialog::OnOK();
	}

	/////////////////////////////////////////////////////////////////////////////
	BOOL MultipleSelectionDlg::OnInitDialog() 
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
            
            // Use a different method for inserting the string depending on whether sorting is
            // required. Ideally the style should be changed so that AddString can always be
            // used, however the style can't be changed once the list control is created
            // unless it is completely reconstructed.
            if ( m_sortItems )
            {
                // Sort items, so use AddString as it will automatically
                // sort the items in alphabetical order.
                index = m_multiList.AddString( iter->str.c_str() );
            }
            else
            {
                // Dont sort items, so use InsertString as it will not
                // apply any form of automatic sort.
                index = m_multiList.InsertString( -1, iter->str.c_str() );
            }
			//m_multiList.SetItemData(index, iter->pkey);
			// Set this item as selected if necessary
			for(ListItems::iterator selIter = m_selectedItems.begin(); selIter != m_selectedItems.end(); ++selIter)
			{
				if (iter->pkey == selIter->pkey)
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
	void MultipleSelectionDlg::OnDestroy() 
	{
		CDialog::OnDestroy();
	}

/////////////////////////////////////////////////////////////////////////////
// MultipleSelectionDlg Private helper functions
/////////////////////////////////////////////////////////////////////////////

	/////////////////////////////////////////////////////////////////////////////
	bool MultipleSelectionDlg::IsAllItemSelected() const
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

    void MultipleSelectionDlg::setSortItems( const bool sortItems )
    {
        m_sortItems = sortItems;
    }
}




