/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/alarm/alarm_manager/src/MultipleSelectionDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_MULTIPLESELECTIONDLG_H__DC407973_7A3A_4552_B004_22F58A9D731E__INCLUDED_)
#define AFX_MULTIPLESELECTIONDLG_H__DC407973_7A3A_4552_B004_22F58A9D731E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// MultipleSelectionDlg.h : header file
//

#include <string>
#include <list>

#include "app/alarm/alarm_manager/src/Resource.h"

/////////////////////////////////////////////////////////////////////////////
// MultipleSelectionDlg dialog
/////////////////////////////////////////////////////////////////////////////
namespace TA_Base_App
{
	// A ListItem is used to map a PKEY to a description string.
	// They are used for data from the locations, subsystems, alarmTypes and operators tables.

	struct ListItem
	{
		ListItem(unsigned long k, std::string s) : pkey(k), str(s) {}

		
		bool operator<(const ListItem& rhs)
		{
			return str.compare(rhs.str) < 0;
		}

		unsigned long	pkey;
		std::string		str;
	};

	// Use a list so we can sort it
	typedef std::list<ListItem> ListItems;


	/////////////////////////////////////////////////////////////////////////////
	class MultipleSelectionDlg : public CDialog
	{
	// Construction
	public:
		MultipleSelectionDlg(const CString& windowTitle, CWnd* pParent = NULL);

		ListItems getSelection() const;
		void setSelection(const ListItems& selection);
		void setItems(const ListItems& items, bool includeAllItem);

        void setSortItems( const bool sortItems );

	private:
		// Helper function. Returns true IFF m_includeAllItem && the ALL item
		// is one of the selections
		bool IsAllItemSelected() const;

		// The title of this dialog window
		CString m_windowTitle;

		// A map containing all items in the list
		ListItems m_listItems;

		// A map containing only the selected items (subset of m_listItems)
		ListItems m_selectedItems;

		// Does this control have an "<All>" item that returns an empty selection
		// when it is included in the selected items?
		// Note that the behaviour of the "<All>" item is slightly non-intuitive:
		// if the ALL item is selected then this dialog returns no selection.
		// This is because this dialog is used to filter out alarms and if ALL
		// is selected for a particular filter then no filter is required.
		bool m_includeAllItem;

        bool m_sortItems;

	// Dialog Data
		//{{AFX_DATA(MultipleSelectionDlg)
	enum { IDD = IDD_MULTIPLE_SEL_DLG };
	CListBox	m_multiList;
	//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(MultipleSelectionDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(MultipleSelectionDlg)
	afx_msg void OnMultiok();
	virtual BOOL OnInitDialog();
	afx_msg void OnDestroy();
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
		
	};
}

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIPLESELECTIONDLG_H__DC407973_7A3A_4552_B004_22F58A9D731E__INCLUDED_)
