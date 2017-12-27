/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/about_box/src/TransActiveAboutDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/02/06 17:22:20 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// TransActiveAboutDlg.cpp : implementation file
//

#include "stdafx.h"
#include "TransActiveAboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTransActiveAboutDlg dialog


CTransActiveAboutDlg::CTransActiveAboutDlg(const char buildVersion[],
										   const char buildDate[],
										   const char componentName[],
										   const char componentVersion[],
										   const int headerArraySize,
										   char* headerArray[],
										   CWnd* pParent /*=NULL*/)
	: CDialog(CTransActiveAboutDlg::IDD, pParent),
	m_ComponentName( componentName ),
	m_AppVersion( componentVersion )
{
	// NOTE: To put the copyright symbol into the dialog: Hold down the
	// ALT key and type 0169

	//{{AFX_DATA_INIT(CTransActiveAboutDlg)
	m_AppName = _T(componentName);
	m_BuildTime = _T(buildDate);
	m_RelNumber = _T(buildVersion);
	m_ProjectName = _T("TransActive");
	//}}AFX_DATA_INIT
	if( headerArraySize > 0 )
	{
		m_ProjectName = _T(headerArray[0]);
	}

    ::GetPrivateProfileString( _T("Project"), _T("name"), m_ProjectName, m_ProjectName.GetBuffer( 100 ), 100, _T("C:\\transactive\\bin\\transactive.ini") );

    // m_DialogTitle = "About "+m_AppName;
    m_DialogTitle = _T("¹ØÓÚ ")+m_AppName;
}


void CTransActiveAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTransActiveAboutDlg)
	DDX_Control(pDX, IDC_ANIMATE1, m_Movie);
	DDX_Text(pDX, IDC_APP_NAME, m_AppName);
	DDX_Text(pDX, IDC_BUILD_TIME, m_BuildTime);
	DDX_Text(pDX, IDC_REL_NUMBER, m_RelNumber);
	DDX_Text(pDX, IDC_PROJECT_NAME, m_ProjectName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTransActiveAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CTransActiveAboutDlg)
	ON_WM_CTLCOLOR()
	ON_WM_LBUTTONDBLCLK()
	ON_WM_TIMER()
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTransActiveAboutDlg message handlers

BOOL CTransActiveAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_brush.CreateSolidBrush(RGB(255, 255, 255)); // color white brush

	// Set the dialog title
	SetWindowText( m_DialogTitle );

	// Play the AVI movie.
	m_Movie.Open(IDR_AVI1);
	m_Movie.Play(0, -1, 1);

	// Create a timer that sends a message every 10 seconds 
	// (so the movie can be played).
	m_TimerID = SetTimer(1, 7000, NULL);

	// Create a timer that sends a message every 10 seconds 
	// (so the movie can be played).
	//SetTimer(IDT_MOVIE_TIMER, 10000, NULL);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}

HBRUSH CTransActiveAboutDlg::OnCtlColor(CDC* pDC, CWnd* pWnd, UINT nCtlColor) 
{
	//HBRUSH hbr = CDialog::OnCtlColor(pDC, pWnd, nCtlColor);

	// Return the white brush
	return m_brush;
}

void CTransActiveAboutDlg::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	// Leave here for the moment...
	
	CDialog::OnLButtonDblClk(nFlags, point);
}


void CTransActiveAboutDlg::OnTimer(UINT nIDEvent) 
{
	m_Movie.Play(0, -1, 1);
	CDialog::OnTimer(nIDEvent);
}

void CTransActiveAboutDlg::OnClose() 
{
	KillTimer(m_TimerID);
	
	CDialog::OnClose();
}
