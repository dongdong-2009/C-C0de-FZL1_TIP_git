/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/EditReport.cpp $
 * @author:  Ripple
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which encapsulates the dialog box and is used to create a new report or a report
 * based on an existing report
 *
 */

/////////////////////////////////////////////////////////////////////////////
// EditReport dialog
// EditReport.cpp : implementation file
//
#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "EditReport.h"


#include "AuditMessageUtility.h"

// NOTE: for displaying message boxes
#include "bus/generic_gui/src/TransActiveMessage.h"

// NOTE : for auditing
#include "core/message/types/ESVAudit_MessageTypes.h"

// NOTE : Location related
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// NOTE : Equipment Report related
#include "core/data_access_interface/src/EquipReport.h"
#include "core/data_access_interface/src/EquipReportAccessFactory.h"
#include "core/data_access_interface/src/ConfigEquipReport.h"
#include "core/data_access_interface/src/EquipReportData.h"
#include "core/data_access_interface/src/EquipReportDataAccessFactory.h"
#include "core/data_access_interface/src/ConfigEquipReportData.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

// NOTE : Debug related
#include "core/utilities/src/DebugUtil.h"

// NOTE: Database query related
#include "core/data_access_interface/src/IOperator.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"

#include "core/data_access_interface/src/Session.h"

#include "progressbar.h"
#include "CustomDefines.h"

using namespace TA_Base_Core;
using namespace TA_Base_App;

namespace TA_Base_App
{
	EditReport::EditReport(	unsigned long locationKey,
                            std::string& reportKey,
							CWnd* pParent /*=NULL*/)
		: CDialog(EditReport::IDD, pParent)
		, m_locationKey(locationKey)
		, m_reportKey(reportKey)
	{
		try
		{
			// Get report
			m_report = dynamic_cast<IConfigEquipReport *>(TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(reportKey, true));
		}
		catch (...)
		{
			return;
		}
		try
		{
			// Get location
			m_location = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey( m_locationKey, false );
		}
		catch (...)
		{
			return;
		}
		
		//{{AFX_DATA_INIT(EditReport)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void EditReport::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(EditReport)
			// NOTE: the ClassWizard will add DDX and DDV calls here
			DDX_Control(pDX, IDOK, m_BtnOk);
			DDX_Control(pDX, IDC_REPORT_NAME, m_ReportName);
			DDX_Control(pDX, IDC_LOCATION, m_Location);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(EditReport, CDialog)
		//{{AFX_MSG_MAP(EditReport)
			ON_EN_CHANGE(IDC_REPORT_NAME, OnChangeReportName)
			ON_WM_SHOWWINDOW()
			// NOTE: the ClassWizard will add message map macros here
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// EditReport message handlers
	void EditReport::CheckInputValidity()
	{
		CString ReportName;
		BOOL isValid = FALSE;

		m_ReportName.GetWindowText(ReportName);
        ReportName.TrimLeft();
        ReportName.TrimRight();

		// Check if new location has been selected

        // TD 12817 Check if report name is already in use at this location
        // as it is entered so that the OK button is enabled/disabled.
        // Watch out for apostrophes because these are expanded to two apostrophes
        // when stored so that they don't break the SQL statement.

		if ( !(ReportName.IsEmpty()) && (m_Location.GetCurSel() != CB_ERR))
		{
            CString newReportName( ReportName);
		    newReportName.Replace("'","''");

			if( newReportName.GetLength() <= 80)
			{
		        std::string repName(newReportName.GetBuffer(newReportName.GetLength()));

		        // check if a report with newReportName already exists in the database
		        if( repName != m_report->getName() )
		        {
			        try
			        {
        				isValid = !(TA_Base_Core::EquipReportAccessFactory::getInstance().equipReportAlreadyExistsAtLocationByName(m_locationKey, 
																												           repName) == TRUE);
			        } 
			        catch (...)
			        {			
				        TA_Base_Bus::TransActiveMessage userMsg;
				        userMsg << newReportName;
				        UINT selectedButton = userMsg.showMsgBox(IDS_UE_083001);
			        }
		        }
			}
			else
			{
				TA_Base_Bus::TransActiveMessage userMsg;
				UINT selectedButton = userMsg.showMsgBox(IDS_UE_083010);
				m_ReportName.SetSel(0, -1, TRUE);
			}
		}

		m_BtnOk.EnableWindow(isValid);
	}


	void EditReport::OnChangeReportName() 
	{
		CheckInputValidity();	
	}


	BOOL EditReport::OnInitDialog() 
	{	
		CDialog::OnInitDialog();
		
		PopulateAllLocations();		
		
		m_Location.SetFocus();
		//caiwei++
		m_ReportName.GetWindowText(oldReportName);
		//TD17481 ++caiwei

		return TRUE;  
	}


	void EditReport::PopulateAllLocations()
	{
		// Set the current report name.
		m_ReportName.SetWindowText(m_report->getName().c_str());

		// Set location name
		m_Location.AddString(m_location->getName().c_str());
		m_Location.EnableWindow(FALSE);
	}


	/*
	*	Only called when all input satisfies basic input checks as per CheckInputValidity() function.
	*	Only the report name can be modified.
	*/ 
	void EditReport::OnOK() 
	{
		// get location name
		CString locationName("");
		m_Location.GetLBText(m_Location.GetCurSel(), locationName);

		// get the new name
		CString newReportName;
		m_ReportName.GetWindowText(newReportName);
		newReportName.Replace("'","''");
		std::string repName(newReportName.GetBuffer(newReportName.GetLength()));

		// check if a report with newReportName already exists in the database
		if( repName != m_report->getName() )
		{
			try
			{
				if (TA_Base_Core::EquipReportAccessFactory::getInstance().equipReportAlreadyExistsAtLocationByName(m_locationKey, 
																												   repName) == TRUE)
				{
					TA_Base_Bus::TransActiveMessage userMsg;
					userMsg << repName;
					UINT selectedButton = userMsg.showMsgBox(IDS_UW_083007);
					m_ReportName.SetSel(0, -1, TRUE);
					return;
				}
			} 
			catch (...)
			{			
				TA_Base_Bus::TransActiveMessage userMsg;
				
				userMsg << newReportName;

				UINT selectedButton = userMsg.showMsgBox(IDS_UE_083001);
				return;
			}
		}

		m_report->setName(repName);
		m_report->applyChanges();

		newReportName.Replace("''","'");
		TA_Base_Bus::TransActiveMessage userMsg;
		//caiwei++
		//userMsg << newReportName;
		userMsg << oldReportName;
		std::string oldName(oldReportName.GetBuffer(oldReportName.GetLength()));
		//TD17481 ++caiwei

		UINT selectedButton = userMsg.showMsgBox(IDS_UI_083008);

		std::string locName(locationName.GetBuffer(locationName.GetLength()));

		// write audit message
		CAuditMessageUtility auditLogWriter;
		//caiwei++
		//auditLogWriter.LogAuditMessage(repName, locName, ESVAudit::ModifyReportLog);
		auditLogWriter.LogAuditMessage(oldName, locName, ESVAudit::ModifyReportLog);
		//TD17481 ++caiwei 
		
		CDialog::OnOK();
	}


	void EditReport::OnShowWindow(BOOL bShow, UINT nStatus) 
	{
		CDialog::OnShowWindow(bShow, nStatus);
		
		m_Location.SetFocus();
		
		if (m_Location.GetCount() == 1)
		{
			m_Location.SetCurSel(0);
		}
		
	}
} // TA_Base_App


