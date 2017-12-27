/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/PowerDemandEditor/src/PowerDemandEditorDlg.h $
  * @author:  wenching
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class provides the main dialog used to display the power demand information.  
  * 
  */

#ifndef POWERDEMANDEDITORDLG_H
#define POWERDEMANDEDITORDLG_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <string>

#include "app/scada/PowerDemandEditor/src/PowerDemandEditor.h"
#include "app/scada/PowerDemandEditor/src/resource.h"

#include "bus/generic_gui/src/TransActiveDialog.h"
#include "bus/mfc_extensions/src/numeric_edit_control/NumericEdit.h"

#include "core/utilities/src/RunParams.h"

/////////////////////////////////////////////////////////////////////////////
// CPowerDemandEditorDlg dialog

namespace TA_Base_Bus
{
    class IGUIAccess;
}

namespace TA_Base_App
{
	class CPowerDemandEditorDlg : public TA_Base_Bus::TransActiveDialog
	{			
	// Construction
	public:

		CPowerDemandEditorDlg(TA_Base_Bus::IGUIAccess& genericGUICallback,CWnd* pParent = NULL);	// standard constructor
		virtual ~CPowerDemandEditorDlg();

		//TD18095, jianghp, to fix the performance of showing manager application
		virtual void init();

		// Dialog Data
		//{{AFX_DATA(CPowerDemandEditorDlg)
		enum { IDD = IDD_PWR_DMD_EDIT_DLG };
		CComboBox	m_meterPointCombo;
		CDateTimeCtrl	m_intervalDatePicker;
		CDateTimeCtrl	m_intervalTimePicker;
		CEdit			m_recordedValueEdit;
		CEdit			m_updatedValueEdit;
		CNumericEdit	m_newValueEdit;

	//}}AFX_DATA


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CPowerDemandEditorDlg)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
		//}}AFX_VIRTUAL

	protected:
	
		// Generated message map functions
		//{{AFX_MSG(CPowerDemandEditorDlg)
		virtual BOOL OnInitDialog();
		virtual void OnCancel();
		afx_msg void OnApply();
		//}}AFX_MSG
		void OnMeterPointSelectionChange();
		void OnIntervalDateSelectionChange(NMHDR* pNMHDR, LRESULT* pResult);
		void OnIntervalTimeSelectionChange(NMHDR* pNMHDR, LRESULT* pResult);
		DECLARE_MESSAGE_MAP()

		
	
	protected:


	private:

		void refreshPowerDemandValues();
        
    private:


	};

	//{{AFX_INSERT_LOCATION}}
	// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
}

#endif // ifndef POWERDEMANDEDITOR_H
