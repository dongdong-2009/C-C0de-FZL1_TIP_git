/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ReportCommentDialog.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which encapsulated a dialog class and simply gets the printing comment from the operator
 *
 */

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "ReportCommentDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    // Maximum value of 64 taken from TES#768
    static DWORD MAX_COMMENT_LENGTH = 64;

	CReportCommentDialog::CReportCommentDialog(CWnd* pParent /*=NULL*/)
		: CDialog(CReportCommentDialog::IDD, pParent)
	{
		//{{AFX_DATA_INIT(CReportCommentDialog)
			// NOTE: the ClassWizard will add member initialization here
		//}}AFX_DATA_INIT
	}


	void CReportCommentDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CReportCommentDialog)
		DDX_Control(pDX, IDC_COMMENT, m_ReportComment);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CReportCommentDialog, CDialog)
		//{{AFX_MSG_MAP(CReportCommentDialog)
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	BOOL CReportCommentDialog::OnInitDialog() 
	{
		CDialog::OnInitDialog();

		m_szComment = "";

		m_ReportComment.SetWindowText(m_szComment.GetBuffer(m_szComment.GetLength()));
        
        
        m_ReportComment.SetLimitText(MAX_COMMENT_LENGTH);

        // Give text field default focus
        m_ReportComment.SetFocus();
        // Must return false to retain focus in this control
		return FALSE;  
	}


	CString& CReportCommentDialog::GetComment()
	{
		return m_szComment;
	}

	void CReportCommentDialog::OnOK() 
	{
		m_ReportComment.GetWindowText(m_szComment);
		
		CDialog::OnOK();
	}

} // TA_Base_App
