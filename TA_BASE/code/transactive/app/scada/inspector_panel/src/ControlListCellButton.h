/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/PScadaListCellButton.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  *
  */
#if !defined(AFX_PSCADALISTCELLBUTTON_H_)
#define AFX_PSCADALISTCELLBUTTON_H_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// PScadaListCellButton.h : header file
//

#include "IButtonProcessor.h"


/////////////////////////////////////////////////////////////////////////////
// CListCellButton window
namespace TA_Base_App
{
	class CControlListCellButton : public CButton
	{
	// Construction
	public:
		CControlListCellButton(CWnd* pWnd, CRect cellRect, int item, CString strValue,IButtonProcessor* listCellButtonProcessor);

	// Attributes
	public:

	protected:
		int 					    m_item;
//		int 					    m_subItem;
		CWnd*						m_parent;
		IButtonProcessor*			m_listCellButtonProcessor;
		CString						m_stringValue;
		bool						m_bExpandDialog;
		bool						m_bInProccess;
		bool						m_bIsWaitingForConfirm;
		CWnd*						m_confirmDialogHandle;
	// Implementation
	public:
		virtual ~CControlListCellButton();
		int		getItem();
		void	setValue(CString strValue);
		void	displayButton(bool isDisplay);
		CString getValue();

		bool	isInProgress();
		void	EnterProcess();
		void	ExitProcess();

		void	expandDialog();
		void	unExpandDialog();
		bool	getIfExpandDialog();
		// Generated message map functions
		bool	getIsWaitingForConfirm();
		void	setIsWaitingForConfirm(bool isWaitConfirm);

		CWnd*	getConfirmDialogHandle();
		void	setConfirmDialogHandle(CWnd* handle);
	protected:
		//{{AFX_MSG(CListCellButton)
		afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
		//}}AFX_MSG

		DECLARE_MESSAGE_MAP()
	};
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PSCADALISTCELLBUTTON_H_)
