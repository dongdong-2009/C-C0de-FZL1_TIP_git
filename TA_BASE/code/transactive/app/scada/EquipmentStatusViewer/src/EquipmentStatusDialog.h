/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EquipmentStatusDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_EQUIPMENTSTATUSDIALOG_H__4D1504D2_2D44_4DC0_869F_5EBBD43098C5__INCLUDED_)
#define AFX_EQUIPMENTSTATUSDIALOG_H__4D1504D2_2D44_4DC0_869F_5EBBD43098C5__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "EquipmentStatusListCtrl.h"

namespace TA_Base_App
{

	class CEquipmentStatusDialog : public CDialog
	{
	// Construction
	public:
		CEquipmentStatusDialog(CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(CEquipmentStatusDialog)
		enum { IDD = IDD_EQUIPMENT_STATUS };
		CComboBox	m_reportName;
		CEquipmentStatusListCtrl	m_equipmentStatusList;
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CEquipmentStatusDialog)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(CEquipmentStatusDialog)
		virtual BOOL OnInitDialog();
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
	};

}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EQUIPMENTSTATUSDIALOG_H__4D1504D2_2D44_4DC0_869F_5EBBD43098C5__INCLUDED_)
