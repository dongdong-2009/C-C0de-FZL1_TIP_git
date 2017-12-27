#if !defined (INPUTNEWVALUEDIALOG_H)
#define INPUTNEWVALUEDIALOG_H

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ConfirmCommandDialog.h $
  * @author:  Andy Parker
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2010/01/07 14:43:16 $
  * Last modified by:  $Author: yong.liu $
  * 
  * This class manages the dialog which confirms that the data point is to be updated.
  * It provides details of the data point and the new value.
  * The return from DoModal() is used to indicate if the update should proceed (IDOK) or
  * be cancelled (IDCANCEL).
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "NumberEdit.h"

namespace TA_Base_App
{
	class CInputNewValueDialog : public CDialog
	{
		// Construction
		public:
			CInputNewValueDialog(CWnd* pParent = NULL);   // standard constructor

			void setValueList(std::map<unsigned long, std::string>& valueMap);

		// Dialog Data
			//{{AFX_DATA(CConfirmCommandDialog)
			enum { IDD = IDD_INPUT_VALUE_DIALOG };
			CString	m_equipment;
			CString	m_newValue;
			CString	m_dropListValue;
			CString	m_dataPoint;
			CString m_unit;
			bool	m_isTextValue;
			//}}AFX_DATA

		// Overrides
			// ClassWizard generated virtual function overrides
			//{{AFX_VIRTUAL(CInputNewValueDialog)
			protected:
			virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
			virtual BOOL OnInitDialog();

			//}}AFX_VIRTUAL

		protected:

			// Generated message map functions
			//{{AFX_MSG(CConfirmCommandDialog)
				// NOTE: the ClassWizard will add member functions here
			afx_msg void OnHelpButton(); //TD15124 azenitha
			afx_msg void OnBnClickedOk();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()

			bool m_bListValue;
		 	std::map<unsigned long, std::string> m_valueMap;

		private:
			CNumberEdit m_newValueEdit;
	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // !defined(INPUTNEWVALUEDIALOG_H)
