// EventGeneratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "EventGenerator.h"
#include "EventGeneratorDlg.h"
#include "app/event/EventGenerator/CommonDef.h"
#include "app/event/EventGenerator/DataGenerator.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/data_access_interface/src/ILocation.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif

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


// CEventGeneratorDlg dialog




CEventGeneratorDlg::CEventGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CEventGeneratorDlg::IDD, pParent)
	, m_bStartAutoSubmit(true)
	, m_pSubmitMgr(0)
	, m_nAutoSubmitTime(0)
	, m_strPrefix(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CEventGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_EDT_EVENT_COUNT, m_strEventSize);
	DDX_Text(pDX, IDC_EDT_THREADS, m_strThreads);
	DDX_Text(pDX, IDC_EDT_SUBMIT_INTERVAL, m_strSubmitInterval);
	DDX_Text(pDX, IDC_EDT_SUBMIT_LOCATION, m_strSubmitLocation);
	DDX_Text(pDX, IDC_EDT_BATCH_SIZE, m_strBatchSize);
	DDX_Text(pDX, IDC_EDT_FAILED_SUBMIT, m_strErrorCount);

	DDX_Control(pDX, IDC_CBO_EVENT_TYPE, m_cboEventType);
	DDX_Text(pDX, IDC_EDT_PREFIX, m_strPrefix);
}

BEGIN_MESSAGE_MAP(CEventGeneratorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_EXIT, &CEventGeneratorDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_AUTO_SUBMIT, &CEventGeneratorDlg::OnBnClickedBtnAutoSubmit)
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CEventGeneratorDlg::OnBnClickedBtnClearAll)
	ON_MESSAGE(WMU_UPDATE_COUNTERS, &CEventGeneratorDlg::OnUpdateCounters)
	ON_MESSAGE(WMU_UPDATE_ERROR_COUNT, &CEventGeneratorDlg::OnUpdateErrorCount)
	ON_WM_TIMER()
END_MESSAGE_MAP()



UINT WorkerThreadProc( LPVOID Param ) //Sample function for using in AfxBeginThread
{
	Sleep(1000);
	CEventGeneratorDlg* pThis = reinterpret_cast<CEventGeneratorDlg*>(Param);
	if (0!=pThis)
	{
		//SendMessage(pThis->GetDlgItem(IDC_BTN_AUTO_SUBMIT)->GetSafeHwnd(), BM_CLICK, NULL, NULL);
		//::PostMessage(pThis->GetSafeHwnd(), )
		pThis->startAutoSubmit();
	}
	return TRUE;
}


// CEventGeneratorDlg message handlers

BOOL CEventGeneratorDlg::OnInitDialog()
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

	//set sessionId
	std::string StrSessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID);
	if (!StrSessionId.empty())
	{
		CDataGenerator::getInstance().setSessionID(StrSessionId.c_str());
	}
	else
	{
		CDataGenerator::getInstance().setSessionID("TransActive Super Session ID");
	}
	
	std::vector< std::string > locations;
	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_LOCATIONKEY))
	{
		locations.push_back(TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY));
		m_strSubmitLocation = TA_Base_Core::RunParams::getInstance().get(RPARAM_LOCATIONKEY).c_str();
	}
	else
	{
		locations.push_back("1"); //Default to OCC
		m_strSubmitLocation = "1";
	}
	CDataGenerator::getInstance().initialize(locations);
	CDataGenerator::getInstance().populateEventTypes(m_cboEventType);

	//Event Count
	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_EVENTCOUNT))
	{
		m_strEventSize = TA_Base_Core::RunParams::getInstance().get(RPARAM_EVENTCOUNT).c_str();
	}
	else
	{
		m_strEventSize.Format("%ld", DEFAULT_EVENT_SIZE);
	}

	//Thread Count
	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_THREADCOUNT))
	{
		m_strThreads = TA_Base_Core::RunParams::getInstance().get(RPARAM_THREADCOUNT).c_str();
	}
	else
	{
		m_strThreads.Format("%ld", DEFAULT_THREAD_COUNT);
	}

	//Submit Interval
	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_SUBMITINTERVAL))
	{
		m_strSubmitInterval = TA_Base_Core::RunParams::getInstance().get(RPARAM_SUBMITINTERVAL).c_str();
	}
	else
	{
		m_strSubmitInterval.Format("%ld", DEFAULT_INTERVAL);
	}

	//Batch Size
	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_BATCHCOUNT))
	{
		m_strBatchSize = TA_Base_Core::RunParams::getInstance().get(RPARAM_BATCHCOUNT).c_str();
	}
	else
	{
		m_strBatchSize.Format("%ld", DEFAULT_SUBMIT_BATCH_SIZE);
	}
	
	TA_Base_Core::ILocation* ploc = TA_Base_Core::LocationAccessFactory::getInstance().getLocationByKey(atoi(m_strSubmitLocation.GetBuffer(0)));
	CString strCaption = DEFAULT_PREFIX;
	if (0 != ploc)
	{
		strCaption = CString(ploc->getName().c_str()) + " " + strCaption;
		delete ploc;
	}
	else
	{
		strCaption = m_strSubmitLocation + " " + strCaption;
	}
	m_strPrefix = strCaption;
	::SetWindowText(this->GetSafeHwnd(), strCaption);

	UpdateData(FALSE);

	if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_AUTOSTART))
	{
		CString autoStartValue = TA_Base_Core::RunParams::getInstance().get(RPARAM_AUTOSTART).c_str();
		if (autoStartValue.CompareNoCase("1") == 0)
		{
			 AfxBeginThread(WorkerThreadProc,(LPVOID)this,THREAD_PRIORITY_NORMAL,0,0,NULL);
		}
	}

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CEventGeneratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CEventGeneratorDlg::OnPaint()
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
HCURSOR CEventGeneratorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CEventGeneratorDlg::OnBnClickedBtnAutoSubmit()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	if (m_bStartAutoSubmit)
	{
		startAutoSubmit();
	}
	else
	{
		stopAutoSubmit();
	}	

}

void CEventGeneratorDlg::startAutoSubmit()
{
	m_pSubmitMgr = new ThreadSubmitMgr(
		atoi(m_strSubmitInterval.GetBuffer(0)),
		atoi(m_strBatchSize.GetBuffer(0)),
		atoi(m_strThreads.GetBuffer(0)),
		atoi(m_strEventSize.GetBuffer(0)),	
		atol(m_strSubmitLocation.GetBuffer(0))
		);

	if (NULL != m_pSubmitMgr)
	{
		SetDlgItemText(IDC_EDT_UNSUBMITTED, "");
		SetDlgItemText(IDC_EDT_SUBMITTED, "");

		CString type;
		int index = m_cboEventType.GetCurSel();
		if (index >= 0)
		{
			m_cboEventType.GetLBText(index, type);
			std::string key(type.Left( type.Find(">") ));
			CDataGenerator::getInstance().setEventType(key);
		}
		else
		{
			CDataGenerator::getInstance().setEventType("");
		}
		CDataGenerator::getInstance().setPrefix(m_strPrefix.GetBuffer(0));
		CDataGenerator::getInstance().resetSeqNumber();

		m_pSubmitMgr->startAutoSubmit();
		SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Stop Auto Submit");
		m_bStartAutoSubmit = false;
		m_nAutoSubmitTime=0;

		SetTimer(UPDATE_AUTO_TIMER, UPDATE_TIME, NULL);
	}
	
}

void CEventGeneratorDlg::stopAutoSubmit()
{
	if (NULL != m_pSubmitMgr)
	{
		m_pSubmitMgr->stopAutoSubmit();
		SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Start Auto Submit");
		m_bStartAutoSubmit = true;

		delete m_pSubmitMgr;
		m_pSubmitMgr = NULL;

		KillTimer(UPDATE_AUTO_TIMER);
	}
}

void CEventGeneratorDlg::OnBnClickedBtnExit()
{
	exit(0);
}

void CEventGeneratorDlg::OnBnClickedBtnClearAll()
{

}

LRESULT CEventGeneratorDlg::OnUpdateCounters(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	unsigned long ulTargetSize = atol(m_strEventSize.GetBuffer(0));
	CString strSubmitted;
	GetDlgItemText(IDC_EDT_SUBMITTED, strSubmitted);
	unsigned long ulSubmitted = atol(strSubmitted.GetBuffer(0));

	ulSubmitted += (ULONG)wParam;
	CString strUnSubmitted;

	strSubmitted.Format("%ld", ulSubmitted);
	strUnSubmitted.Format("%ld", ulTargetSize - ulSubmitted);

	SetDlgItemText(IDC_EDT_UNSUBMITTED, strUnSubmitted.GetBuffer(0));
	SetDlgItemText(IDC_EDT_SUBMITTED, strSubmitted.GetBuffer(0));

	if (ulTargetSize == ulSubmitted)
	{
		stopAutoSubmit();
	}
	return 0;
}

LRESULT CEventGeneratorDlg::OnUpdateErrorCount(WPARAM wParam, LPARAM lParam)
{
	UpdateData(TRUE);
	unsigned long ulErrCount = atol(m_strErrorCount.GetBuffer(0));
	CString strNewCount;

	ulErrCount ++;

	strNewCount.Format("%ld", ulErrCount);
	SetDlgItemText(IDC_EDT_FAILED_SUBMIT, strNewCount.GetBuffer(0));

	return 0;
}

void CEventGeneratorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case UPDATE_AUTO_TIMER:
		{
			m_nAutoSubmitTime++;
			time_t objtTime(m_nAutoSubmitTime);
			tm* ptm = gmtime(&objtTime);

			char szTime[256];
			strftime (szTime,256,"%H:%M:%S", ptm);
			CString strTime = szTime;

			SetDlgItemText(IDC_EDT_LAPSED_TIME, strTime.GetBuffer(0));
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}