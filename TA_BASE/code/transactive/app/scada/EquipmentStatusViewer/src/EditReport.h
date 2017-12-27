/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EditReport.h $
 * @author:  Ripple
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which encapsulates the dialog box and is used to edit a report
 *
 */

#if !defined(AFX_EDITREPORT_H__A0DA0DBE_C4B5_49E0_AB62_3145FC277325__INCLUDED_)
#define AFX_EDITREPORT_H__A0DA0DBE_C4B5_49E0_AB62_3145FC277325__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// EditReport.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// EditReport dialog

#include <string>

#include "core\utilities\src\RunParams.h"

namespace TA_Base_Core
{
    class IConfigEquipReport;
    class ILocation;
}

namespace TA_Base_App
{
	class EditReport : public CDialog
	{
	// Construction
	public:
		EditReport(	unsigned long locationKey,
                    std::string& reportKey,
					CWnd* pParent = NULL);   // standard constructor

	// Dialog Data
		//{{AFX_DATA(EditReport)
		enum { IDD = IDD_EDIT_REPORT };
			// NOTE: the ClassWizard will add data members here
		//}}AFX_DATA


	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(EditReport)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

		//{{AFX_DATA(EditReport)
		CButton	m_BtnOk;
		CEdit	m_ReportName;
		CComboBox	m_Location;
	//}}AFX_DATA


	// Implementation
	protected:

		// Generated message map functions
		//{{AFX_MSG(EditReport)
			// NOTE: the ClassWizard will add member functions here
		afx_msg void OnChangeReportName();
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:
		/** 
		 *	PopulateAllLocations()
		 *
		 *	Populates combo boxes with all available locations 
		 *
		 */
		void PopulateAllLocations();

		/**
		 *	CheckInputValidity()
		 *
		 *	Checks to see which information is selected/provided and if its valid in terms of 
		 *	GUI rules to enable or disable relevant GUI controls
		 *
		 */
		void CheckInputValidity();

		//cw++
		CString oldReportName;
		//TD17481 ++cw

		unsigned long m_locationKey;
        std::string m_reportKey;
		TA_Base_Core::IConfigEquipReport* m_report;
		TA_Base_Core::ILocation* m_location;
	};
}
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EDITREPORT_H__A0DA0DBE_C4B5_49E0_AB62_3145FC277325__INCLUDED_)
