// AtsAlarmGeneratorDlg.cpp : implementation file
//

#include "stdafx.h"
#include "AtsAlarmGenerator.h"
#include "AtsAlarmGeneratorDlg.h"
#include "CommonDefine.h"
//#include "SystemSubSystem.h"

#include "DataGenerator.h"
#include "CommandHandler.h"

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


// CAtsAlarmGeneratorDlg dialog




CAtsAlarmGeneratorDlg::CAtsAlarmGeneratorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CAtsAlarmGeneratorDlg::IDD, pParent)
	, m_strAlarmID(_T(""))
	, m_strSystemName(_T(""))
	, m_strSubSystemName(_T(""))
	, m_strDescription(_T(""))
	, m_strAlarmValue(_T(""))
	, m_strAssetName(_T(""))
	, m_strAlarmType(_T(""))
	, m_strLocation(_T(""))
	, m_strSeverity(_T(""))
	, m_pThreadWorkerMgr(0)
	, m_bStartAutoSubmit(true)
	, m_bChkAckClose(FALSE)
	, m_strAlarmCount(_T(""))
	, m_strThreads(_T(""))
	, m_strSubmitInterval(_T(""))
	, m_strAckCloseInterval(_T(""))
	, m_strUnSubmitted(_T(""))
	, m_strUnclose(_T(""))
	, m_nAutoAckTime(0)
	, m_strMMSState(_T(""))
	, m_pInterfaceATS(0)
	, m_strAlarmState(_T(""))
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CAtsAlarmGeneratorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_CBO_SYSTEM, m_cboSystem);
	DDX_Control(pDX, IDC_CBO_SUBSYSTEM, m_cboSubSystem);
	DDX_Control(pDX, IDC_CBO_SEVERITY, m_cboSeverity);
	DDX_Control(pDX, IDC_CBO_LOCATION_ID, m_cboLocationID);
	DDX_Control(pDX, IDC_CBO_ALARM_TYPE, m_cboAlarmType);
	DDX_Control(pDX, IDC_EDT_ALARM_ID, m_edtAlarmID);
	DDX_Text(pDX, IDC_EDT_ALARM_ID, m_strAlarmID);
	DDX_CBString(pDX, IDC_CBO_SYSTEM, m_strSystemName);
	DDX_CBString(pDX, IDC_CBO_SUBSYSTEM, m_strSubSystemName);
	DDX_Text(pDX, IDC_EDT_DESCRIPTION, m_strDescription);
	DDX_Text(pDX, IDC_EDT_ALARM_VALUE, m_strAlarmValue);
	DDX_Text(pDX, IDC_EDT_ASSET_NAME, m_strAssetName);
	DDX_CBString(pDX, IDC_CBO_ALARM_TYPE, m_strAlarmType);
	DDX_CBString(pDX, IDC_CBO_LOCATION_ID, m_strLocation);
	DDX_CBString(pDX, IDC_CBO_SEVERITY, m_strSeverity);
	DDX_Control(pDX, IDC_LST_ALARMS, m_listATSAlarms);
	DDX_Check(pDX, IDC_CHK_ACK_CLOSE, m_bChkAckClose);
	DDX_Control(pDX, IDC_EDT_ACK_CLOSE_INTERVAL, m_edtAckClose);
	DDX_Text(pDX, IDC_EDT_ALARM_COUNT, m_strAlarmCount);
	DDX_Text(pDX, IDC_EDT_THREADS, m_strThreads);
	DDX_Text(pDX, IDC_EDT_SUBMIT_INTERVAL, m_strSubmitInterval);
	DDX_Text(pDX, IDC_EDT_ACK_CLOSE_INTERVAL, m_strAckCloseInterval);
	DDX_Text(pDX, IDC_EDT_UNSUBMITTED, m_strUnSubmitted);
	DDX_Text(pDX, IDC_EDT_UNCLOSED, m_strUnclose);
	DDX_Control(pDX, IDC_CBO_MMS_STATE, m_cboMMSState);
	DDX_CBString(pDX, IDC_CBO_MMS_STATE, m_strMMSState);
	DDX_Control(pDX, IDC_CBO_ALARM_STATE, m_cboAlarmState);
	DDX_CBString(pDX, IDC_CBO_ALARM_STATE, m_strAlarmState);
}

BEGIN_MESSAGE_MAP(CAtsAlarmGeneratorDlg, CDialog)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDC_BTN_EXIT, &CAtsAlarmGeneratorDlg::OnBnClickedBtnExit)
	ON_BN_CLICKED(IDC_BTN_SUBMIT, &CAtsAlarmGeneratorDlg::OnBnClickedBtnSubmit)
	ON_BN_CLICKED(IDC_BTN_GEN_ALARMID, &CAtsAlarmGeneratorDlg::OnBnClickedBtnGenAlarmid)

	ON_CBN_SELCHANGE(IDC_CBO_SYSTEM, &CAtsAlarmGeneratorDlg::OnCbnSelchangeCboSystem)
	ON_BN_CLICKED(IDC_BTN_CLEAR_ATS, &CAtsAlarmGeneratorDlg::OnBnClickedBtnClearAts)
	ON_BN_CLICKED(IDC_BTN_AUTO_SUBMIT, &CAtsAlarmGeneratorDlg::OnBnClickedBtnAutoSubmit)
	ON_BN_CLICKED(IDC_CHK_ACK_CLOSE, &CAtsAlarmGeneratorDlg::OnBnClickedChkAckClose)
	ON_MESSAGE(WMU_AUTO_SUBMIT_FINISH, &CAtsAlarmGeneratorDlg::OnAutoSubmitFinished)
	ON_MESSAGE(WMU_AUTO_ACKCLOSE_FINISH, &CAtsAlarmGeneratorDlg::OnAutoAckCloseFinished)
	ON_WM_TIMER()
	ON_BN_CLICKED(IDC_BTN_ACK, &CAtsAlarmGeneratorDlg::OnBnClickedBtnAck)
	ON_BN_CLICKED(IDC_BTN_CLOSE, &CAtsAlarmGeneratorDlg::OnBnClickedBtnClose)
	ON_WM_DESTROY()
	ON_BN_CLICKED(IDC_BTN_CLEAR, &CAtsAlarmGeneratorDlg::OnBnClickedBtnClear)
	ON_BN_CLICKED(IDC_BTN_GEN_RANDOM, &CAtsAlarmGeneratorDlg::OnBnClickedBtnGenRandom)
	ON_BN_CLICKED(IDC_BTN_NORMALIZE, &CAtsAlarmGeneratorDlg::OnBnClickedBtnNormalize)
END_MESSAGE_MAP()


// CAtsAlarmGeneratorDlg message handlers

BOOL CAtsAlarmGeneratorDlg::OnInitDialog()
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

	Initialize();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CAtsAlarmGeneratorDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CAtsAlarmGeneratorDlg::OnPaint()
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
HCURSOR CAtsAlarmGeneratorDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


void CAtsAlarmGeneratorDlg::OnBnClickedBtnExit()
{
	// TODO: Add your control notification handler code here
	CDialog::OnOK();
}

void CAtsAlarmGeneratorDlg::Initialize(void)
{
	CDataGenerator::getInstance().LoadData();
	// Initialize combo system
	STRING_LIST systems;
	CDataGenerator::getInstance().getStringList(SystemName, systems);
	for (int nIdx = 0; nIdx < (int)systems.size(); nIdx++)
	{
		CString strSystem(systems[nIdx].c_str());
		m_cboSystem.AddString(strSystem);	
	}
	
	// Initialize Severity
	for (int nCtr = 1; nCtr <= HIGHEST_SEVERITY; nCtr++ )
	{
		CString strData;
		strData.Format("%d", nCtr);
		m_cboSeverity.AddString(strData);
		strData.Empty();
	}

	// Initialize LocationIDs
	STRING_LIST locIDList;
	CDataGenerator::getInstance().getStringList(LocationID, locIDList);
	for (int nIdx = 0; nIdx < (int)locIDList.size(); nIdx++)
	{
		CString strLocID(locIDList[nIdx].c_str());
		m_cboLocationID.AddString(strLocID);	
	}

	// AlarmType
	STRING_LIST alarmTypeList;
	CDataGenerator::getInstance().getStringList(AlarmType, alarmTypeList);
	for (int nIdx = 0; nIdx < (int)alarmTypeList.size(); nIdx++)
	{
		CString strAlarmType(alarmTypeList[nIdx].c_str());
		m_cboAlarmType.AddString(strAlarmType);	
	}

	// MMS State
	STRING_LIST mmsStateList;
	CDataGenerator::getInstance().getStringList(MMSState, mmsStateList);
	for (int nIdx = 0; nIdx < (int)mmsStateList.size(); nIdx++)
	{
		CString strMMSState(mmsStateList[nIdx].c_str());
		m_cboMMSState.InsertString(m_cboMMSState.GetCount(), strMMSState);	
	}

	// Alarm State
	for (int nIdx=0; nIdx<= (int)I_ALARM_PROVIDER::AlarmAcknowledged; nIdx++)
	{
		CString strAlarmState(_formatState((I_ALARM_PROVIDER::AlarmState)nIdx).c_str());
		m_cboAlarmState.InsertString(m_cboAlarmState.GetCount(), strAlarmState);
	}

	// Initialize List Control
	m_listATSAlarms.setupListControl();
	m_listATSAlarms.registerCountObserver(this);

	// Setup command handler profile information
	I_ALARM_PROVIDER::ProfileInfo profileInfo;
	CDataGenerator::getInstance().getProfileConfig(profileInfo);
//	CCommandHandler::getInstance().setProfileInfo(profileInfo);
//	CCommandHandler::getInstance().registerObserver(&m_listATSAlarms);
	CCommandHandler::getInstance().setTargetWnd(m_listATSAlarms.GetSafeHwnd());
	
	// Setup Request handler
	m_pRequestHandler = new CRequestHandler(&m_listATSAlarms);
	m_pRequestHandler->start();

	// SetUp profile information
	if(!m_pInterfaceATS)
	{
		m_pInterfaceATS = getISCSInterface();
	}
	profileInfo.updateTime = time(NULL);
	m_pInterfaceATS->setProfileInfo(profileInfo);

	// Initialize button auto submit caption
	if (m_bStartAutoSubmit)
		this->SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Start Auto Submit");
	else
		this->SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Stop Auto Submit");

	// Initialize Auto Configuration
 	m_strAlarmCount.Format("%d", CDataGenerator::getInstance().GetAutoAlarmCount());
 	m_strThreads.Format("%d", CDataGenerator::getInstance().GetAutoThreadCount());
 	m_strSubmitInterval.Format("%d", CDataGenerator::getInstance().GetAutoSubmitInterval());
 	m_strAckCloseInterval.Format("%d", CDataGenerator::getInstance().GetAutoAckCloseInterval());

	GetDlgItem(IDC_EDT_ACK_CLOSE_INTERVAL)->EnableWindow(m_bChkAckClose);

	UpdateData(FALSE);
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnSubmit()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	I_ALARM_PROVIDER::AlarmDetail* pDetail = NULL;

	bool bError = false;
	CString strErrorMsg = "Please fill-in data to the following fields: ";
	//verify all fields have data
	//AlarmID 
	if (m_strAlarmID.GetLength() == 0)
	{
		bError = true;
		strErrorMsg += "AlarmID";
	}

	//Alarm Value
	if (m_strAlarmValue.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "AlarmValue";
		bError = true;
	}

	//System Name
	if (m_strSystemName.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "System";
		bError = true;
	}

	//SubSystem Name
	if (m_strSubSystemName.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "SubSystem";
		bError = true;
	}

	//Description
	if (m_strDescription.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "Description";
		bError = true;
	}

	//Asset Name
	if (m_strAssetName.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "Asset Name";
		bError = true;
	}

	//Severity
	if (m_strSeverity.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "Severity";
		bError = true;
	}

	//AlarmType
	if (m_strAlarmType.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "AlarmType";
		bError = true;
	}
	
	//Location
	if (m_strLocation.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "Location";
		bError = true;
	}

	//MMS State
	if (m_strMMSState.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "MMS State";
		bError = true;
	}

	//Alarm State
	if (m_strAlarmState.GetLength() == 0)
	{
		if (bError)
			strErrorMsg +=", ";
		strErrorMsg+= "Alarm State";
		bError = true;
	}

	if (bError)
	{
		MessageBox(strErrorMsg, "Error: Missing fields", MB_ICONINFORMATION | MB_OK);
	}
	else
	{
		pDetail = CDataGenerator::getInstance().createAlarm(m_strAlarmID, 
			m_strAlarmValue, m_strSystemName, 
			m_strSubSystemName, m_strDescription, 
			m_strAssetName, m_strAlarmType, 
			m_strLocation, atoi(m_strSeverity.GetBuffer(0)), m_cboMMSState.GetCurSel(), m_cboAlarmState.GetCurSel());

		if (NULL != pDetail)
		{
			bool bRet = CCommandHandler::getInstance().submitAlarm(pDetail);
			if (bRet)
			{
				//m_listATSAlarms.insertAlarm(pDetail);
				ClearManualFields();
			}
		}
	}
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnGenAlarmid()
{
	// TODO: Add your control notification handler code here
	m_strAlarmID.Empty();
	m_strAlarmID = CDataGenerator::getInstance().GenStringGUID().c_str();
	UpdateData(FALSE);
}

void CAtsAlarmGeneratorDlg::OnCbnSelchangeCboSystem()
{
	// TODO: Add your control notification handler code here
	m_cboSubSystem.ResetContent();

	// Initialize combo subSystem
	STRING_LIST subsystems;
	std::string strSystem;
	m_cboSystem.GetLBText(m_cboSystem.GetCurSel(), m_strSystemName);
	strSystem = m_strSystemName.GetBuffer(0);
	CDataGenerator::getInstance().getSubSystemList(strSystem, subsystems);
	for (int nIdx = 0; nIdx < (int)subsystems.size(); nIdx++)
	{
		CString strSubSystem(subsystems[nIdx].c_str());
		m_cboSubSystem.AddString(strSubSystem);	
	}
}

void CAtsAlarmGeneratorDlg::ClearManualFields(void)
{
	m_strAlarmID.Empty();
	m_strAlarmValue.Empty();
	m_strSystemName.Empty();
	m_strSubSystemName.Empty();
	m_strDescription.Empty();
	m_strAssetName.Empty();
	m_strSeverity.Empty();
	m_strAlarmType.Empty();
	m_strLocation.Empty();
	m_strMMSState.Empty();
	m_strAlarmState.Empty();
	UpdateData(FALSE);
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnClearAts()
{
	// TODO: Add your control notification handler code here
	ClearManualFields();
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnAutoSubmit()
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

void CAtsAlarmGeneratorDlg::startAutoSubmit()
{
	m_pThreadWorkerMgr = new CThreadWorkerMgr(
		GetSafeHwnd(),
		atoi(m_strThreads.GetBuffer(0)),
		atoi(m_strSubmitInterval.GetBuffer(0)),
		atol(m_strAlarmCount.GetBuffer(0))
		);

	if (NULL != m_pThreadWorkerMgr)
	{
		m_listATSAlarms.resetSubmitCounter();
		m_pThreadWorkerMgr->startAutoSubmit();
		SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Stop Auto Submit");
		m_bStartAutoSubmit = false;
		m_nAutoAckTime = 0;
		SetTimer(UPDATE_AUTO_TIMER, UPDATE_TIME, NULL);

	}

	if (m_bChkAckClose)
	{
		m_listATSAlarms.startAutoAckClose(atoi(m_strAckCloseInterval.GetBuffer(0)), GetSafeHwnd());
	}

	updateManualButtons(FALSE);
}

void CAtsAlarmGeneratorDlg::stopAutoSubmit()
{
	if (NULL != m_pThreadWorkerMgr)
	{
		m_pThreadWorkerMgr->stopAutoSubmit();
		SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Start Auto Submit");
		m_bStartAutoSubmit = true;

		delete m_pThreadWorkerMgr;
		m_pThreadWorkerMgr = NULL;

		KillTimer(UPDATE_AUTO_TIMER);
	}

	if (m_bChkAckClose)
	{
		m_listATSAlarms.stopAutoAckClose();
	}

	updateManualButtons(TRUE);
}

void CAtsAlarmGeneratorDlg::OnBnClickedChkAckClose()
{
	// TODO: Add your control notification handler code here
	UpdateData();
	m_edtAckClose.EnableWindow(m_bChkAckClose);
}

void CAtsAlarmGeneratorDlg::ClearAutoFields(void)
{
	m_strAlarmCount.Empty();
	m_strSubmitInterval.Empty();
	m_strThreads.Empty();
	m_strAckCloseInterval.Empty();
	m_strUnSubmitted.Empty();
	m_strUnclose.Empty();

}

LRESULT CAtsAlarmGeneratorDlg::OnAutoSubmitFinished(WPARAM wParam, LPARAM lParam)
{
	if (m_bStartAutoSubmit == false)
	{
		if (NULL != m_pThreadWorkerMgr)
		{
			m_pThreadWorkerMgr->stopAutoSubmit();
			delete m_pThreadWorkerMgr;
			m_pThreadWorkerMgr = NULL;
		}
		
		if (m_bChkAckClose)
		{
			m_listATSAlarms.finishAutoAck();
		}
		else
		{
			m_bStartAutoSubmit = true;
			SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Start Auto Submit");
			KillTimer(UPDATE_AUTO_TIMER);
		}

		updateManualButtons(TRUE);
	}
	return 0;
}

LRESULT CAtsAlarmGeneratorDlg::OnAutoAckCloseFinished(WPARAM wParam, LPARAM lParam)
{
	if (m_bChkAckClose)
	{
		m_listATSAlarms.stopAutoAckClose();
	}

	m_bStartAutoSubmit = true;
	SetDlgItemText(IDC_BTN_AUTO_SUBMIT, "Start Auto Submit");
	KillTimer(UPDATE_AUTO_TIMER);
	updateManualButtons(TRUE);
	return 0;
}

void CAtsAlarmGeneratorDlg::OnTimer(UINT_PTR nIDEvent)
{
	// TODO: Add your message handler code here and/or call default
	switch(nIDEvent)
	{
	case UPDATE_AUTO_TIMER:
		{
			m_nAutoAckTime++;
			tm* ptm = gmtime(&m_nAutoAckTime);

			char szTime[256];
			strftime (szTime,256,"%H:%M:%S", ptm);
			CString strTime = szTime;

			SetDlgItemText(IDC_EDT_LAPSED_TIME, strTime.GetBuffer(0));
		}
		break;
	}

	CDialog::OnTimer(nIDEvent);
}

void CAtsAlarmGeneratorDlg::UpdateCounters(ULONG ulSubmitted, ULONG ulUnCloseCount)
{
	ULONG ulTotalAlarmCount = atol(m_strAlarmCount);
	CString strUnSub, strUnClose;

	strUnSub.Format("%ld", ulTotalAlarmCount - ulSubmitted);
	strUnClose.Format("%ld", ulUnCloseCount);

	SetDlgItemText(IDC_EDT_UNSUBMITTED, strUnSub.GetBuffer(0));
	SetDlgItemText(IDC_EDT_UNCLOSED, strUnClose.GetBuffer(0));
}
void CAtsAlarmGeneratorDlg::OnBnClickedBtnAck()
{
	// TODO: Add your control notification handler code here
	ALARMDETAIL_LIST alarmList;
	m_listATSAlarms.getSelectedAlarms(alarmList);

//	m_pThreadFunctionExe->executeFunction(alarmList, ACKNOWLEDGE);
	for(int nIdx = 0; nIdx < (int)alarmList.size(); nIdx++)
	{
		if (alarmList[nIdx]->state == I_ALARM_PROVIDER::AlarmOpen || alarmList[nIdx]->state == I_ALARM_PROVIDER::AlarmReturned)
		{
			alarmList[nIdx]->ackTime = time(0);
			alarmList[nIdx]->alarmAcknowledgeBy = "C952-OPT";
			CCommandHandler::getInstance().acknowledgeAlarm(alarmList[nIdx]);
			Sleep(1);
		}
	}

}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnClose()
{
	// TODO: Add your control notification handler code here
	ALARMDETAIL_LIST alarmList;
	m_listATSAlarms.getSelectedAlarms(alarmList);

//	m_pThreadFunctionExe->executeFunction(alarmList, CLOSE);
	for(int nIdx = 0; nIdx < (int)alarmList.size(); nIdx++)
	{
		CCommandHandler::getInstance().closeAlarm(alarmList[nIdx]);
		Sleep(1);
	}
}

void CAtsAlarmGeneratorDlg::OnDestroy()
{
	__super::OnDestroy();
	
	// Clean up
	if (m_pRequestHandler)
	{
		m_pRequestHandler->terminateAndWait();
		delete m_pRequestHandler;
		m_pRequestHandler = NULL;
	}

	if (m_pThreadWorkerMgr)
	{
		m_pThreadWorkerMgr->stopAutoSubmit();
		delete m_pThreadWorkerMgr;
		m_pThreadWorkerMgr = NULL;
	}

	if (m_bChkAckClose)
	{
		m_listATSAlarms.stopAutoAckClose();
	}

	CCommandHandler::freeInstance();

	// TODO: Add your message handler code here
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnClear()
{
	// TODO: Add your control notification handler code here
	if (!m_bStartAutoSubmit)
	{
		OnBnClickedBtnAutoSubmit();
	}
		
	CCommandHandler::getInstance().setUpdateTime();
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnGenRandom()
{
	// TODO: Add your control notification handler code here
	I_ALARM_PROVIDER::AlarmDetail * pAlarm = NULL;
	pAlarm = CDataGenerator::getInstance().createRandomAlarm();

	m_strAlarmID = pAlarm->alarmID.c_str();
	m_strAlarmValue = pAlarm->alarmValue.c_str();
	m_strSystemName = pAlarm->systemkey.c_str();
	m_strSubSystemName = pAlarm->subsytemkey.c_str();
	m_strDescription = pAlarm->alarmDescription.c_str();
	m_strAssetName = pAlarm->assetName.c_str();
	m_strSeverity.Format("%d", pAlarm->alarmSeverity);
	m_strAlarmType = pAlarm->alarmType.c_str();
	m_strLocation = pAlarm->locationId.c_str();
	UpdateData(FALSE);

	m_cboMMSState.SetCurSel((int)pAlarm->mmsState);
	m_cboAlarmState.SetCurSel((int)pAlarm->state);
	if (pAlarm != NULL)
	{
		delete pAlarm;
		pAlarm = NULL;
	}
}

std::string CAtsAlarmGeneratorDlg::_formatState(I_ALARM_PROVIDER::AlarmState state)
{
	CString strState;
	switch (state)
	{
	case I_ALARM_PROVIDER::AlarmOpen:
		strState.Format("%s", "OPEN");
		break;
	case I_ALARM_PROVIDER::AlarmAcknowledged:
		strState.Format("%s", "ACKNOWLEDGE");
		break;
	case I_ALARM_PROVIDER::AlarmReturned:
		strState.Format("%s", "RETURNED");
		break;
	case I_ALARM_PROVIDER::AlarmClosed:
		strState.Format("%s", "CLOSED");
		break;
	default:
		strState.Format("%s", "unknown");
		break;

	}
	return strState.GetBuffer(0);
}


void CAtsAlarmGeneratorDlg::updateManualButtons(BOOL bEnable)
{
	this->GetDlgItem(IDC_BTN_ACK)->EnableWindow(bEnable);
	this->GetDlgItem(IDC_BTN_CLOSE)->EnableWindow(bEnable);
	this->GetDlgItem(IDC_BTN_NORMALIZE)->EnableWindow(bEnable);
}

void CAtsAlarmGeneratorDlg::OnBnClickedBtnNormalize()
{
	// TODO: Add your control notification handler code here
	ALARMDETAIL_LIST alarmList;
	m_listATSAlarms.getSelectedAlarms(alarmList);

	//	m_pThreadFunctionExe->executeFunction(alarmList, NORMALIZE);
	for(int nIdx = 0; nIdx < (int)alarmList.size(); nIdx++)
	{
		if (alarmList[nIdx]->state == I_ALARM_PROVIDER::AlarmOpen || alarmList[nIdx]->state == I_ALARM_PROVIDER::AlarmAcknowledged)
		{
			CCommandHandler::getInstance().normalizeAlarm(alarmList[nIdx]);
			Sleep(1);
		}
	}
}
