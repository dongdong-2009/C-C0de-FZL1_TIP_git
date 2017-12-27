/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/MainTabCtrl.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_MAINTABCTRL_H__E3939585_E37A_479C_940B_7D2C1539F4A6__INCLUDED_)
#define AFX_MAINTABCTRL_H__E3939585_E37A_479C_940B_7D2C1539F4A6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/mfc_extensions/src/tab_dialog_control/tabdialogctrl.h"
#include "EquipmentStatusDialog.h"
#include "ConfigurationDialog.h"

namespace TA_Base_App
{

	class CMainTabCtrl : public CTabDialogCtrl
	{
	// Construction
	public:
		CMainTabCtrl();

	// Attributes
	public:

	// Operations
	public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMainTabCtrl)
		//}}AFX_VIRTUAL

	// Implementation
	public:
		virtual ~CMainTabCtrl();

		// Generated message map functions
	protected:
		//{{AFX_MSG(CMainTabCtrl)
			// NOTE - the ClassWizard will add and remove member functions here.
		//}}AFX_MSG

		CEquipmentStatusDialog	m_equipmentStatusDialog;
		CConfigurationDialog	m_configurationDialog;

		DECLARE_MESSAGE_MAP()
	};

} // TA_Base_App
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MAINTABCTRL_H__E3939585_E37A_479C_940B_7D2C1539F4A6__INCLUDED_)
