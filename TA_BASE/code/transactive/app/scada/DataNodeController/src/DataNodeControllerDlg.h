/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/DataNodeController/src/DataNodeControllerDlg.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#if !defined(AFX_DATANODECONTROLLERDLG_H__1EA52942_C4E8_41F3_8375_72599BB4F27E__INCLUDED_)
#define AFX_DATANODECONTROLLERDLG_H__1EA52942_C4E8_41F3_8375_72599BB4F27E__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/scada/scada_tree/src/ScadaTreeCtrl.h"

#include "DataNodeTabControl.h"
#include "dlgbars.h"
#include "resource.h"

namespace TA_Base_App
{
	class CDataNodeControllerDlg : public TA_Base_Bus::TransActiveDialog
							
	{	
	public:		
		CDataNodeControllerDlg(TA_Base_Bus::IGUIAccess& genericGUICallback);	

	
		//{{AFX_DATA(CDataNodeControllerDlg)
	enum { IDD = IDD_DATANODECONTROLLER_DIALOG };	
	//}}AFX_DATA
		

		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDataNodeControllerDlg)
		protected:
			virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(CDataNodeControllerDlg)
		virtual BOOL OnInitDialog();
		afx_msg void OnHelpAboutDataNodeController();
		afx_msg void OnCloseClicked();
		afx_msg void OnSelchangingTree(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
	afx_msg void OnSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:		
		void CalculateToolbarHeight();
		void EnableToolbar(bool bEnable);
		void CreateToolbar();
		void AddTextToButton(UINT buttonId, CString buttonLabel);
		CDlgToolBar m_WndToolBar;
		unsigned int m_ToolBarHeight;
		
		TA_Base_Bus::CScadaTreeCtrl	m_ScadaTree;
		TA_Base_App::CDataNodeControllerTabControl m_TabControl;
	};

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DATANODECONTROLLERDLG_H__1EA52942_C4E8_41F3_8375_72599BB4F27E__INCLUDED_)
