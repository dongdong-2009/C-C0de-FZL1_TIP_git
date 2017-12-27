// GenaClientTestDlg.cpp : implementation file
//

#include "stdafx.h"
#include "GenaClientTest.h"
#include "GenaClientTestDlg.h"
#include "../../Gena/src/CommonDef.h"
#include <sstream>
#include "../../Gena/src/GENALibrary.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/RunParams.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CGenaClientTestDlg dialog




CGenaClientTestDlg::CGenaClientTestDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CGenaClientTestDlg::IDD, pParent)
	, strTopic(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGenaClientTestDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_LST_SERVER_RESPONSE, resultsList);
	//DDX_Text(pDX, IDC_EDT_URI, strTopic);
	//DDX_Text(pDX, IDC_EDT_HOST, strClientHost);
	//DDX_Text(pDX, IDC_EDT_PORT, strClientPort);
}

BEGIN_MESSAGE_MAP(CGenaClientTestDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_SUBSCRIBE, &CGenaClientTestDlg::OnBnClickedBtnSubscribe)
	ON_BN_CLICKED(IDC_BTN_UNSUBSCRIBE, &CGenaClientTestDlg::OnBnClickedBtnUnsubscribe)
	ON_BN_CLICKED(IDOK, &CGenaClientTestDlg::OnBnClickedOk)
END_MESSAGE_MAP()


// CGenaClientTestDlg message handlers

BOOL CGenaClientTestDlg::OnInitDialog()
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
	char LogPath[256];
	sprintf(LogPath, "GenalClientTesting.log");
	TA_Base_Core::DebugUtil::getInstance().setFile(LogPath);
	TA_Base_Core::DebugUtil::getInstance().setMaxSize(2000000);
	TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::DebugDebug);
	TA_Base_Core::RunParams::getInstance().parseCmdLine(AfxGetApp()->m_lpCmdLine);

	TA_Base_Core::CorbaUtil::getInstance().initialise(7777);
	TA_Base_Core::CorbaUtil::getInstance().activate();

	GENALibrary::getInstance().initialize();
	GENALibrary::getInstance().register_TopicObserver(this);
	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CGenaClientTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGenaClientTestDlg::OnPaint()
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
HCURSOR CGenaClientTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CGenaClientTestDlg::OnBnClickedBtnSubscribe()
{
	// TODO: Add your control notification handler code here
// 	GenaClientParams params;
// 	params.serverNames.push_back("http://localhost:8080");
// 	params.serverNames.push_back("http://localhost:8070");
// 	params.keepAlive = true;
// 	params.maxRetry = 3;
// 	params.timeout = 600000;
// 	params.callbackInfo.serverName = "http://localhost:8090";
// 	params.callbackInfo.keepAlive = true;
// 	params.callbackInfo.serverPort = 8090;
// 
// 	m_pGenaClient = new GenaClient(params);
// 	m_pGenaClient->start();
// 
// 	ITopicListener* pListener = this;
// 	T_TopicFilterSet filterset;
// 
// 	unsigned long newSID = m_pGenaClient->subscribe(strTopic.GetBuffer(0), "gena:birth,gena:death,gena:update", filterset, pListener);
// 	m_clientSIDs.push_back(newSID);

// 	char szTemp[250];
// 	sprintf(szTemp, "Subscribe successful with SID: %d", newSID);
// 
// 	resultsList.AddString(szTemp);
	GENALibrary::getInstance().startService();
	
}


void retrievedData(std::stringstream& outStream, boost::shared_ptr<Attribute> objAttr)
{
	outStream << "\n Attribute: " << objAttr->name;
	if (objAttr->subAttribute.size() == 0)
	{
		outStream << "=" << objAttr->value.c_str() << std::endl;
	}
	else
	{
		outStream << std::endl;

		T_AttribValueMap::iterator itr = objAttr->subAttribute.begin();
		for (itr; itr!=objAttr->subAttribute.end(); itr++)
		{
			boost::shared_ptr<Attribute> objAttr = itr->second;
			retrievedData(outStream, objAttr);
		}
	}
}


void CGenaClientTestDlg::OnBnClickedBtnUnsubscribe()
{
	// TODO: Add your control notification handler code here
	resultsList.ResetContent();
	GENALibrary::getInstance().stopService();
}

T_GENATopicDataList CGenaClientTestDlg::getAllData(const std::string& topicName )
{
	T_GENATopicDataList objRetVal;

	return objRetVal;
}

void CGenaClientTestDlg::synchronizeBegin(const std::string& topicName)
{
	resultsList.AddString("synchronizeBegin");
}

void CGenaClientTestDlg::synchronizeEnd(const std::string& topicName)
{
	resultsList.AddString("synchronizeEnd");
}

void CGenaClientTestDlg::updateDataImpl(const std::string& topicName, GENATopicDataPtr pData)
{
	char szTemp[500];
	sprintf(szTemp, "%s Message! Topic Name : %s, ObjID: %s", pData->getUpdateType().c_str(), pData->getTopicName().c_str(), pData->getObjectID().c_str());
	LOG3(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "%s Message! Topic Name : %s, ObjID: %s", pData->getUpdateType().c_str(), pData->getTopicName().c_str(), pData->getObjectID().c_str());
	if (resultsList.GetCount() > 100)
	{
		resultsList.ResetContent();
	}
	resultsList.AddString(szTemp);

	if (pData->getUpdateType() == GENA_UPDATE || pData->getUpdateType() == GENA_BIRTH)
	{
		std::stringstream strContent;
		strContent << "Message Attributes: ";
		T_AttribValueMap attributes = pData->getAttributes();
		T_AttribValueMap::const_iterator itr = attributes.begin();
		for (itr; itr!=attributes.end(); itr++)
		{
			boost::shared_ptr<Attribute> objAttr = itr->second;
			retrievedData(strContent, objAttr);
		}

		if (resultsList.GetCount() > 100)
		{
			resultsList.ResetContent();
		}
		resultsList.AddString(strContent.str().c_str());
	}
}

void CGenaClientTestDlg::OnBnClickedOk()
{
	// TODO: Add your control notification handler code here
	resultsList.ResetContent();
	GENALibrary::getInstance().stopService();
	CDialog::OnOK();
}
