/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/DataPointSelectDlg.h $
  * @author:  Lizette Lingo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// DataPointSelectDlg.h : header file
//


#if !defined(AFX_DATAPOINTSELECTDLG_H__5216EACD_811C_4030_9B1E_0019BA02BFB6__INCLUDED_)
#define AFX_DATAPOINTSELECTDLG_H__5216EACD_811C_4030_9B1E_0019BA02BFB6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DataPointSelectDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDataPointSelectDlg dialog
#include "app/trending/trend_viewer/src/TrendTreeCtrl.h"
#include "core/data_access_interface/trends/src/ItemInfoDetails.h"

#include <map>

namespace TA_Base_App
{	
	class CDataPointSelectDlg : public CDialog
	{
		// Construction
	public:
		CDataPointSelectDlg( const std::string& itemName, 
			                 const std::map<std::string, 
							 TA_Base_Core::ItemInfo>& selectedItems, 
							 IItemStore& itemStore,
							 CWnd* pParent = NULL);   // standard constructor
		
		std::string getItemName() const { return m_selectedDpName; }
		// Dialog Data
		//{{AFX_DATA(CDataPointSelectDlg)
		enum { IDD = IDD_DATA_POINT_ADD_DLG };
		// NOTE: the ClassWizard will add data members here
		CTrendTreeCtrl	m_trendDpTree;
		CButton m_addButton;		
		CEdit m_datapointName;
		//}}AFX_DATA
		
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDataPointSelectDlg)
	protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL
		
		// Implementation
	protected:
		
		// Generated message map functions
		//{{AFX_MSG(CDataPointSelectDlg)
		// NOTE: the ClassWizard will add member functions here	
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnSelChangedTreeItem(NMHDR* pNMHDR, LRESULT* pResult);
		virtual void OnResetItem();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		std::string m_selectedDpName;
		std::map<std::string, TA_Base_Core::ItemInfo> m_selectedItems;
		bool m_bDataCleared;
	};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATAPOINTSELECTDLG_H__5216EACD_811C_4030_9B1E_0019BA02BFB6__INCLUDED_)
