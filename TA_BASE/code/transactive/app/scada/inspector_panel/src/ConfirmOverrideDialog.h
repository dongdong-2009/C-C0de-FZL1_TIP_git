/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmOverrideDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_CONFIRMOVERRIDEDIALOG_H__B71B96E3_9427_447A_9324_1156EA18E105__INCLUDED_)
#define AFX_CONFIRMOVERRIDEDIALOG_H__B71B96E3_9427_447A_9324_1156EA18E105__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ConfirmOverrideDialog.h : header file
//

#include "app/scada/Inspector_Panel/src/Equipment.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CConfirmOverrideDialog dialog

	class CConfirmOverrideDialog : public CDialog
	{
	// Construction
	public:
		CConfirmOverrideDialog(CWnd* pParent, int iPoint, CEquipment *equipment, CString currentValue = "");

		std::string getOverrideValue();

	// Dialog Data
		//{{AFX_DATA(CConfirmOverrideDialog)
		enum { IDD = IDD_CONFIRM_OVERRIDE_DIALOG };
		//}}AFX_DATA

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CConfirmOverrideDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		void OnChangeNewValue();

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CConfirmOverrideDialog)
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnHelpButton(); //TD15124 azenitha
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		int				m_iPoint;
		CEquipment*		m_equipment;
		CComboBox		m_valueComboBox;
		CNumericEdit	m_valueNumericEdit;
		CEdit			m_valueEdit;
		std::string		m_overrideValue;
		CString			m_currentValue;
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(AFX_CONFIRMOVERRIDEDIALOG_H__B71B96E3_9427_447A_9324_1156EA18E105__INCLUDED_)
