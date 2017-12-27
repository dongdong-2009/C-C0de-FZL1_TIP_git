/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/HistoryReportsDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// HistoryReportsDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crutil.h"
#include "HistoryReportsDlg.h"
#include "globals.h"
#include "strings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/TransActiveMessage.h"

namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

        /////////////////////////////////////////////////////////////////////////////
        // HistoryReportsDlg dialog


        HistoryReportsDlg::HistoryReportsDlg(CWnd* pParent /*=NULL*/)
	        : CDialog(HistoryReportsDlg::IDD, pParent)
        {
	        //{{AFX_DATA_INIT(HistoryReportsDlg)
	        //}}AFX_DATA_INIT
        }


        void HistoryReportsDlg::DoDataExchange(CDataExchange* pDX)
        {
	        CDialog::DoDataExchange(pDX);
	        //{{AFX_DATA_MAP(HistoryReportsDlg)
	        DDX_Control(pDX, IDC_REPORTS_LISTING, m_lbHistoricalReports);
	        //}}AFX_DATA_MAP
        }


        BEGIN_MESSAGE_MAP(HistoryReportsDlg, CDialog)
	        //{{AFX_MSG_MAP(HistoryReportsDlg)
	        ON_LBN_DBLCLK(IDC_REPORTS_LISTING, OnDblclkReportsListing)
	        //}}AFX_MSG_MAP
        END_MESSAGE_MAP()

        /////////////////////////////////////////////////////////////////////////////
        // HistoryReportsDlg message handlers

        BOOL HistoryReportsDlg::OnInitDialog() 
        {
	        CDialog::OnInitDialog();
	        m_cstrSelection = "";
	        populateListBox();

	        return TRUE;  // return TRUE unless you set the focus to a control
	                      // EXCEPTION: OCX Property Pages should return FALSE
        }

        void HistoryReportsDlg::OnOK() 
        {
	        if (m_lbHistoricalReports.GetCurSel() != LB_ERR)
	        {
		        m_lbHistoricalReports.GetText( m_lbHistoricalReports.GetCurSel(), m_cstrSelection );
		        CDialog::OnOK();
	        }
	        else
	        {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UI_140016);
            }
        }

        void HistoryReportsDlg::OnCancel() 
        {
	        m_cstrSelection = "";
	        CDialog::OnCancel();
        }

        void HistoryReportsDlg::populateListBox()
        {
	        WIN32_FIND_DATA lpData;
	        int num;

	        CString cstrFullDir = m_cstrDirectory;
	        cstrFullDir += m_cstrFileSpec;

	        HANDLE handle=FindFirstFile( cstrFullDir, &lpData);
	        
	        while (handle!=INVALID_HANDLE_VALUE)
	        {
		        num=lpData.dwFileAttributes;
		        num>>=4;
		        num&=0x00000001;

		        if (num==1) 
		        {
			        // A directory, so ignore.
		        }
		        else
		        {
			        m_lbHistoricalReports.AddString( lpData.cFileName );
		        }

		        if(FindNextFile(handle,&lpData) )
		        {}
		        else break;
	        }
        }

        bool HistoryReportsDlg::filesExist()
        {
	        WIN32_FIND_DATA lpData;
	        int num;

	        CString cstrFullDir = m_cstrDirectory;
	        cstrFullDir += m_cstrFileSpec;

	        HANDLE handle=FindFirstFile( cstrFullDir, &lpData);
	        
	        while (handle!=INVALID_HANDLE_VALUE)
	        {
		        num=lpData.dwFileAttributes;
		        num>>=4;
		        num&=0x00000001;

		        if (num==1) 
		        {
			        // A directory, so ignore.
		        }
		        else
		        {
			        return( true );
		        }

		        if(FindNextFile(handle,&lpData) )
		        {}
		        else break;
	        }
	        return( false );
        }

        void HistoryReportsDlg::OnDblclkReportsListing() 
        {
            OnOK();
        }

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
