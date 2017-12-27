// security_simulatorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "security_simulator.h"
#include "security_simulatorDlg.h"
#include "FileParser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace
{
	const std::string CONFIG_FILE_NAME("security_simulator.ini");
}

/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About

class CAboutDlg : public CDialog
{
public:
	CAboutDlg();

// Dialog Data
	//{{AFX_DATA(CAboutDlg)
	enum { IDD = IDD_ABOUTBOX };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CAboutDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	//{{AFX_MSG(CAboutDlg)
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
		// No message handlers
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorDlg dialog

CSecurity_simulatorDlg::CSecurity_simulatorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSecurity_simulatorDlg::IDD, pParent),
	  m_sessionHelper(NULL),
	  m_dutyHelper(NULL),
	  m_rightsHelper(NULL),
	  m_rightsCheckThread(NULL)
{
	//{{AFX_DATA_INIT(CSecurity_simulatorDlg)
	m_loginLocation = 0;
	m_loginPassword = _T("transactive");
	m_loginUserId = 1;
	m_loginProfile = 0;
	m_loginWorkstation = 20000003;
	m_ORLocation = 0;
	m_ORPassword = _T("transactive");
	m_ORProfile = 5;//TCO1
	m_ORUserId = 2;
	m_ORWorkstation = 0;
	m_actionKey = 115;
	m_resourceKey = 20001689;
	m_locationKey = 0;
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	FileParser fileParser(CONFIG_FILE_NAME);
	fileParser.parseFile(m_configedAgentLocation);

	m_sessionHelper = new SessionHelper(m_configedAgentLocation);
	m_dutyHelper	= new DutyHelper(m_configedAgentLocation);
	m_rightsHelper  = new RightsHelper(m_configedAgentLocation);
	
}

CSecurity_simulatorDlg::~CSecurity_simulatorDlg()
{
	delete m_sessionHelper;
	delete m_dutyHelper;
	delete m_rightsHelper;
	delete m_rightsCheckThread;
}

void CSecurity_simulatorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSecurity_simulatorDlg)
	DDX_Control(pDX, IDC_CHK_AUTOCHECKDUTY, m_btnAutoCheckDuty);
	DDX_Control(pDX, IDC_CMB_LOCATION, m_locationCMB);
	DDX_Control(pDX, IDC_BTN_DUTYCHECK, m_btnDutyCheck);
	DDX_Control(pDX, IDC_EDIT_RESULT, m_result);
	DDX_Control(pDX, IDC_BTN_RM_OVERRIDE, m_btnRmOverride);
	DDX_Control(pDX, IDC_BTN_OVERRIDE, m_btnOverride);
	DDX_Control(pDX, IDC_BTN_LOGOUT, m_btnLogout);
	DDX_Control(pDX, IDC_BTN_LOGIN, m_btnLogin);
	DDX_Text(pDX, IDC_EDIT_LOCATION, m_loginLocation);
	DDX_Text(pDX, IDC_EDIT_PASSWORD, m_loginPassword);
	DDX_Text(pDX, IDC_EDIT_USERID, m_loginUserId);
	DDX_Text(pDX, IDC_EDIT_PROFILE, m_loginProfile);
	DDX_Text(pDX, IDC_EDIT_WORKSTATION, m_loginWorkstation);
	DDX_Text(pDX, IDC_EDIT_LOCATION2, m_ORLocation);
	DDX_Text(pDX, IDC_EDIT_PASSWORD2, m_ORPassword);
	DDX_Text(pDX, IDC_EDIT_PROFILE2, m_ORProfile);
	DDX_Text(pDX, IDC_EDIT_USERID2, m_ORUserId);
	DDX_Text(pDX, IDC_EDIT_WORKSTATION2, m_ORWorkstation);
	DDX_Text(pDX, IDC_EDIT_ACTIONKEY, m_actionKey);
	DDX_Text(pDX, IDC_EDIT_RESOURCEKEY, m_resourceKey);
	DDX_Text(pDX, IDC_EDIT_LOCATIONKEY, m_locationKey);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CSecurity_simulatorDlg, CDialog)
	//{{AFX_MSG_MAP(CSecurity_simulatorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BTN_LOGIN, OnBtnLogin)
	ON_BN_CLICKED(IDC_BTN_LOGOUT, OnBtnLogout)
	ON_BN_CLICKED(IDC_BTN_OVERRIDE, OnBtnOverride)
	ON_BN_CLICKED(IDC_BTN_RM_OVERRIDE, OnBtnRmOverride)
	ON_BN_CLICKED(IDC_BTN_DUTYCHECK, OnBtnDutycheck)
	ON_BN_CLICKED(IDC_CHK_AUTOCHECKDUTY, OnChkAutocheckduty)
	ON_BN_CLICKED(IDC_BTN_TAKEDUTY, OnBtnTakeduty)
	ON_BN_CLICKED(IDC_BTN_LOGSESSION, OnBtnLogsession)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSecurity_simulatorDlg message handlers

BOOL CSecurity_simulatorDlg::OnInitDialog()
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

	m_btnLogin.EnableWindow(true);
	m_btnLogout.EnableWindow(false);
	m_btnOverride.EnableWindow(false);
	m_btnRmOverride.EnableWindow(false);

	m_locationCMB.Clear();
	for (std::vector< std::string >::iterator it = m_configedAgentLocation.begin(); 
	it != m_configedAgentLocation.end(); ++it)
	{
		m_locationCMB.AddString(it->c_str());
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CSecurity_simulatorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CSecurity_simulatorDlg::OnPaint() 
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

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

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CSecurity_simulatorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void CSecurity_simulatorDlg::OnBtnLogin() 
{
	// TODO: Add your control notification handler code here

	try
	{
		UpdateData();
		m_loginSession = m_authLib.requestSession( m_loginUserId, m_loginProfile, 
			m_loginLocation, m_loginWorkstation, (LPCTSTR) m_loginPassword );

		m_btnLogin.EnableWindow(false);
		m_btnLogout.EnableWindow(true);
		m_btnOverride.EnableWindow(true);
		m_btnRmOverride.EnableWindow(false);
		m_btnDutyCheck.EnableWindow(true);

//		//now check the session info in every Authentication Agent
//		if (m_sessionHelper->checkSessionInfo())
//		{
//			showSessionResult("all the session in the configed agent are same");
//		}
//		else
//		{
//			showSessionResult(m_sessionHelper->getResultString());
//		}

	}
	catch (...)
	{
	}

}

void CSecurity_simulatorDlg::OnBtnLogout() 
{
	// TODO: Add your control notification handler code here

	try
	{
		UpdateData();
		m_authLib.endSession(m_loginSession);

		m_btnLogin.EnableWindow(true);
		m_btnLogout.EnableWindow(false);
		m_btnOverride.EnableWindow(false);
		m_btnRmOverride.EnableWindow(false);
		m_btnDutyCheck.EnableWindow(false);
		
//		//now check the session info in every Authentication Agent
//		if (m_sessionHelper->checkSessionInfo())
//		{
//			showSessionResult("all the session in the configed agent are same");
//		}
//		else
//		{
//			showSessionResult(m_sessionHelper->getResultString());
//		}
		
	}
	catch (...)
	{
	}

}

void CSecurity_simulatorDlg::OnBtnOverride() 
{
	// TODO: Add your control notification handler code here
	try
	{
		UpdateData();
		m_authLib.beginOverride(m_loginSession, m_ORUserId, m_ORProfile, 
			m_ORLocation, (LPCTSTR)m_ORPassword );
		
		m_btnLogin.EnableWindow(false);
		m_btnLogout.EnableWindow(false);
		m_btnOverride.EnableWindow(false);
		m_btnRmOverride.EnableWindow(true);
		
//		//now check the session info in every Authentication Agent
//		if (m_sessionHelper->checkSessionInfo())
//		{
//			showSessionResult("all the session in the configed agent are same");
//		}
//		else
//		{
//			showSessionResult(m_sessionHelper->getResultString());
//		}
		
	}
	catch (...)
	{
	}	
}

void CSecurity_simulatorDlg::OnBtnRmOverride() 
{
	// TODO: Add your control notification handler code here

	try
	{
		UpdateData();
		m_authLib.endOverride(m_loginSession);
		
		m_btnLogin.EnableWindow(false);
		m_btnLogout.EnableWindow(true);
		m_btnOverride.EnableWindow(true);
		m_btnRmOverride.EnableWindow(false);
		
		//now check the session info in every Authentication Agent
//		if (m_sessionHelper->checkSessionInfo())
//		{
//			showSessionResult("all the session in the configed agent are same");
//		}
//		else
//		{
//			showSessionResult(m_sessionHelper->getResultString());
//		}
		
	}
	catch (...)
	{
	}		
}

void CSecurity_simulatorDlg::showSessionResult(const std::string& result)
{
	m_result.SetWindowText(result.c_str());
}

void CSecurity_simulatorDlg::OnBtnDutycheck() 
{
	// TODO: Add your control notification handler code here
	m_dutyHelper->checkDuty(m_loginSession);

	std::string result("");

	if (!m_dutyHelper->getRegionDutyResult()) 
	{
		result += "RegionDuty different, detail: ";
		result += m_dutyHelper->getRegionDutyResultString();
	}

	if (!m_dutyHelper->getSubsystemDutyResult()) 
	{
		result += "SubsystemDuty different, detail: ";
		result += m_dutyHelper->getSubsystemDutyResultString();
	}
	
	if (!m_dutyHelper->getSubsystemStateResult()) 
	{
		result += "RegionDuty different, detail: ";
		result += m_dutyHelper->getSubsystemStateResultString();
	}
	
	m_result.SetWindowText(result.c_str());
}

void CSecurity_simulatorDlg::notify(bool result, std::string& resultString)
{
	if (result)
	{
		m_result.SetWindowText(resultString.c_str());
	}
	else
	{
		MessageBox(resultString.c_str());
	}
}

void CSecurity_simulatorDlg::OnChkAutocheckduty() 
{
	if (m_btnAutoCheckDuty.GetCheck() == 1) 
	{
		UpdateData();
		
		CString location;
		int result = m_locationCMB.GetCurSel();
		
		if (result ==CB_ERR)
		{
			MessageBox("can't get selected location");
			return;
		}
		
		m_locationCMB.GetLBText(result, location);
		
		if (location == "")
		{
			MessageBox("can't get selected location");
			return;
		}
		
		if(m_rightsCheckThread != NULL)
		{
			delete m_rightsCheckThread;
			m_rightsCheckThread = NULL;
		}
		m_rightsCheckThread = new RightsCheckingThread( *m_rightsHelper, *this, m_loginSession, 
			(LPCTSTR )location, m_resourceKey, m_actionKey, 
			m_loginProfile );
		m_rightsCheckThread->start();		
	}
	else
	{
		delete m_rightsCheckThread;
		m_rightsCheckThread = NULL;
	}
}

void CSecurity_simulatorDlg::OnBtnTakeduty() 
{
	UpdateData();
	
	CString location;
	int result = m_locationCMB.GetCurSel();
	
	if (result ==CB_ERR)
	{
		MessageBox("can't get selected location");
		return;
	}
	
	m_locationCMB.GetLBText(result, location);
	
	if (location == "")
	{
		MessageBox("can't get selected location");
		return;
	}

	std::vector<unsigned long > locations;
	locations.push_back(m_locationKey);
	m_dutyHelper->transferRegionDuty( (LPCTSTR )location, m_loginSession, locations);
}

void CSecurity_simulatorDlg::OnBtnLogsession() 
{
	m_sessionHelper->getSessionInfo();
	m_sessionHelper->logAllSessionInfo();
}
