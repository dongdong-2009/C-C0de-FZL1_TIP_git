// EventViewerSimulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EventViewerSimulator.h"
#include "EventViewerSimulatorDlg.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"

#include <time.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#define DEFAULT_QUERY_INTERVAL 500
#define LAPSE_TIMER_TICK 1000
#define DEFAULT_LOCATION 1
#define TEST_TIMER 1


using namespace TA_Base_Core;

// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	enum { IDD = IDD_ABOUTBOX };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

// Implementation
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
END_MESSAGE_MAP()


// CEventViewerSimulatorDlg dialog




CEventViewerSimulatorDlg::CEventViewerSimulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventViewerSimulatorDlg::IDD, pParent)
	, m_txtRetrieveInterval(_T(""))
	, m_txtEventCount(_T(""))
	, m_AgentLocation(_T(""))
	, m_bStartAgentQuery(true)
	, m_nQueryTime(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEventViewerSimulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LIST_EVENTS, m_objListCtrl);
	DDX_Text(pDX, IDC_EDT_RETRIEVE_INTERVAL, m_txtRetrieveInterval);
	DDX_Text(pDX, IDC_EDT_EVENT_COUNT, m_txtEventCount);
	DDX_Text(pDX, IDC_EDT_AGENT_LOCATION, m_AgentLocation);
	DDX_Check(pDX, IDC_CHK_LOG_EVENTS, m_bIsLogNeeded);
}

BEGIN_MESSAGE_MAP(CEventViewerSimulatorDlg, CDialog)
	ON_WM_TIMER()
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_START, &CEventViewerSimulatorDlg::OnBnClickedBtnStart)
	ON_MESSAGE(WMU_UPDATE_COUNTERS, &CEventViewerSimulatorDlg::OnUpdateCounters)
END_MESSAGE_MAP()


// CEventViewerSimulatorDlg message handlers

BOOL CEventViewerSimulatorDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// Add "About..." menu item to system menu.

	// IDM_ABOUTBOX must be in the system command range.
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		CString strAboutMenu;
		strAboutMenu.LoadString(IDS_ABOUTBOX);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon


	// TODO: Add extra initialization here
	RunParams::getInstance().parseCmdLine(GetCommandLine());
	std::string level = TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGLEVEL);
	if (!level.empty())
	{
		TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString(level.c_str()));
	}
	else
	{
		TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::getDebugLevelFromString("INFO"));
	}

	TA_Base_Core::DebugUtil::getInstance().setFile(TA_Base_Core::RunParams::getInstance().get(RPARAM_DEBUGFILE).c_str());
	DebugUtil::getInstance().setMaxSize(10000000);

	std::string m_mgrPrt;
	if(TA_Base_Core::RunParams::getInstance().isSet(RPARAM_MGRPORT))
	{
		m_mgrPrt = TA_Base_Core::RunParams::getInstance().get(RPARAM_MGRPORT);
	}

	// initialise using the specified port otherwise use default port 6666
	if (m_mgrPrt != "")
	{
		int port = atoi(m_mgrPrt.c_str());
		TA_Base_Core::CorbaUtil::getInstance().initialise(port);
	}
	else
	{
		TA_Base_Core::CorbaUtil::getInstance().initialise();
	}

	TA_Base_Core::CorbaUtil::getInstance().activate();


	m_txtRetrieveInterval.Format("%ld", DEFAULT_QUERY_INTERVAL);
	m_AgentLocation.Format("%ld", DEFAULT_LOCATION);

	UpdateData(FALSE);

	// TODO: Add extra initialization here
	m_objListCtrl.setUpEventList();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEventViewerSimulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CEventViewerSimulatorDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}
}

// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CEventViewerSimulatorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEventViewerSimulatorDlg::OnBnClickedBtnStart()
{
	// TODO: Add your control notification handler code here
	UpdateData(TRUE);
	if (m_bStartAgentQuery)
	{
		int nQueryLoc = atoi(m_AgentLocation.GetBuffer(0));
		unsigned long ulQueryInterval = atol(m_txtRetrieveInterval.GetBuffer(0));

		m_objListCtrl.startAgentRefresh(nQueryLoc, ulQueryInterval, (m_bIsLogNeeded==BST_CHECKED));
		
		m_nQueryTime = 0;
		SetTimer(TEST_TIMER, LAPSE_TIMER_TICK, NULL);

		SetDlgItemText(IDC_BTN_START, "Stop Agent Query");
		m_bStartAgentQuery = false;
	}
	else
	{
		m_objListCtrl.stopStopRefresh();

		SetDlgItemText(IDC_BTN_START, "Start Agent Query");
		m_bStartAgentQuery = true;
		
	}	

}

void CEventViewerSimulatorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case TEST_TIMER:
		{
			m_nQueryTime++;
			time_t tempt(m_nQueryTime);
			tm* ptm = gmtime(&tempt);

			char szTime[256];
			strftime (szTime,256,"%H:%M:%S", ptm);
			CString strTime = szTime;

			SetDlgItemText(IDC_EDT_LAPSED_TIME, strTime.GetBuffer(0));
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

LRESULT CEventViewerSimulatorDlg::OnUpdateCounters(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	CString strSize;
	unsigned long ulSize = (ULONG)wParam;
	strSize.Format("%ld", ulSize);

	SetDlgItemText(IDC_EDT_EVENT_COUNT, strSize.GetBuffer(0));
	return 0;
}
