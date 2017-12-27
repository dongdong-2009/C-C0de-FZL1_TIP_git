/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/NewReport.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which encapsulates the dialog box and is used to create a new report or a report
 * based on an existing report
 *
 */

#if !defined(AFX_NEWREPORT_H__D2A1011D_B67F_4EAE_8396_FF205E03D1AE__INCLUDED_)
#define AFX_NEWREPORT_H__D2A1011D_B67F_4EAE_8396_FF205E03D1AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include "core\utilities\src\RunParams.h"
#include "core/data_access_interface/src/ResourceAccessFactory.h"
#include "core/data_access_interface/src/IResource.h"

#include "bus\application_types\src\apptypes.h"

namespace TA_Base_App
{
	class CNewReport : public CDialog
	{

	public:
		
		/**
		 *	CNewReport()
		 *
		 *	Default Constructor
		 *
		 */
		CNewReport(CWnd* pParent = NULL);   			

		/**
		 *	Primary key of the location of the newly created report
		 *
		 */
		unsigned long m_newLocationKey;

		/**
		 *	Primary key of the newly created report
		 *
		 */
        std::string m_newReportKey;

		/**
		 *	Access control flag		 
		 *
		 */
		bool m_isAllowedToViewAllReports;

		/**
		 *	map to keep the string id with unsigned long id	 
		 *
		 */
		std::map<int, std::string> m_reportKeyStringToIdMap;

    private:
	
		//{{AFX_DATA(CNewReport)
		enum { IDD = IDD_NEW_REPORT };
		CButton	m_BtnOk;
		CEdit	m_ReportName;
		CComboBox	m_Location;
		CComboBox	m_BaseReport;
		CComboBox	m_BaseLocation;
		CButton	m_BasedOnCheck;
		//}}AFX_DATA


		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CNewReport)
		protected:
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
		//}}AFX_VIRTUAL

	protected:

		// Generated message map functions
		//{{AFX_MSG(CNewReport)
		afx_msg void OnBasedOnCheckClick();
		afx_msg void OnSelchangeLocation();
		afx_msg void OnChangeReportName();
		afx_msg void OnSelchangeBaseLocation();
		afx_msg void OnSelchangeBaseReport();
		virtual BOOL OnInitDialog();
		virtual void OnOK();
		afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	private:

		/**
		 *	CreateNewReportBasedOnExisting()
		 *
		 *	Creates a report based on an existing report at the same location
		 *
		 *	@param	baseReportKey	Primary key of the base report to use
		 *	@param	newReportName	Name of the newly created report
		 *	@return	aNewReportKey	Returns the primary key of the newly created report
		 *	@return					Bool to signify success (true) or failure (false)
		 *
		 */
        bool CreateNewReportBasedOnExisting(std::string& baseReportKey, const unsigned long locationKey, const std::string &newReportName, std::string& aNewReportKey);

		/** 
		 *	PopulateAllLocations()
		 *
		 *	Populates combo boxes with all available locations 
		 *
		 */
		void PopulateAllLocations();

		/**
		 *	PopulateBaseReports()
		 *
		 *	Populates the combo box with all available reports at a particular location
		 *
		 *	@param	locationKey		Primary key of the location from which reports will be fetched
		 *
		 */
		void PopulateBaseReports(const unsigned long locationKey);

		
		/**
		 *	CheckInputValidity()
		 *
		 *	Checks to see which information is selected/provided and if its valid in terms of 
		 *	GUI rules to enable or disable relevant GUI controls
		 *
		 */
		void CheckInputValidity();

	    /**
	      * RePopulateBaseLocation
	      * 
	      * re populate the base location based on the location combo box
	      * 
	      * @return void 
	      * 
	      * @exception <exceptions> Optional
	      */
        void RePopulateBaseLocation();
	};

} // TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWREPORT_H__D2A1011D_B67F_4EAE_8396_FF205E03D1AE__INCLUDED_)
