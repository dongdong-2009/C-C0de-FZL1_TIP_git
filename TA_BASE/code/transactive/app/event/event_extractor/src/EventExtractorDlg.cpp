/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3001_Review_Branch_DBSync/TA_BASE/transactive/app/event/event_extractor/src/EventExtractorDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2013/01/25 15:15:56 $
  * Last modified by:  $Author: huirong.luo $
  *
  */
// EventExtractorDlg.cpp : implementation file
//



#include "stdafx.h"
#include "EventExtractor.h"
#include "EventExtractorDlg.h"

#include "ace/OS.h"

#include <core/utilities/src/RunParams.h>
#include "bus/generic_gui/src/HelpLauncher.h"


// Subsystem
#include "core/data_access_interface/src/ISubsystem.h"
#include "core/data_access_interface/src/SubsystemAccessFactory.h"

// Database
#include "core/data_access_interface/src/DbConnection.h"
#include "core/data_access_interface/src/DataTypeEnums.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/data_access_interface/src/DatabaseFactory.h"
#include "core/data_access_interface/src/SQLCode.h"
#include "bus/mfc_extensions/src/standard_dialogs_arial_bold/FileDialogArial.h"
#include "bus/mfc_extensions/src/selection_helpers/SelectionHelper.h"
#include "core/utilities/src/DebugUtil.h"

#include <sstream>
#include <fstream>



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const std::string DEFAULT_VALUE("");
	// Alarm Comments identifiers
    const std::string COMMENT_IDENTIFIER("C");
	// Alarm DSS Plan identifiers
	const std::string AUTO_PLAN_NOT_RUN_ID("A");
	const std::string SEMI_AUTO_PLAN_NOT_RUN_ID("S");
	const std::string PLAN_RUN_ID("R");
	const std::string PLAN_POSTPONED_ID("X");
	// Alarm Avalanche Status identifiers
	const std::string AVL_HEAD_ID("H");
	const std::string AVL_CHILD_ID("F");
	// Alarm Mms Status identifiers
	const std::string MMS_AUTO_ID("A");
	const std::string MMS_SEMI_ID("S");
	const std::string MMS_JR_SENT_ID("R");
	const std::string MMS_FAILED_ID("D");
	const std::string MMS_INHIBIT_ID("I");
}

// Assign the required values to the column constants.
const std::string EventExtractorDlg::COLUMN_DATETIME("CREATETIME");
const std::string EventExtractorDlg::COLUMN_SEVERITY("SEVERITY_NAME");
const std::string EventExtractorDlg::COLUMN_ASSET("ASSET_NAME");
const std::string EventExtractorDlg::COLUMN_DESCRIPTION("DESCRIPTION");
const std::string EventExtractorDlg::COLUMN_VALUE("VALUE");
const std::string EventExtractorDlg::COLUMN_MMS("MMS_STATE");
const std::string EventExtractorDlg::COLUMN_DSS("DSS_STATE");
const std::string EventExtractorDlg::COLUMN_AVL("AVL_STATE");
const std::string EventExtractorDlg::COLUMN_OPERATOR("OPERATOR_NAME");
const std::string EventExtractorDlg::COLUMN_COMMENT("ALARM_COMMENT");


/*
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
*/

/////////////////////////////////////////////////////////////////////////////
// EventExtractorDlg dialog

EventExtractorDlg::EventExtractorDlg(CWnd* pParent /*=NULL*/)
	: CDialog(EventExtractorDlg::IDD, pParent),
	  m_isAllEvents(true)
{
	//{{AFX_DATA_INIT(EventExtractorDlg)
	//}}AFX_DATA_INIT
	// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void EventExtractorDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(EventExtractorDlg)
	DDX_Control(pDX, IDC_STARTTIME, m_startTimeCtrl);
	DDX_Control(pDX, IDC_STARTDATE, m_startDateCtrl);
	DDX_Control(pDX, IDC_ENDTIME, m_endTimeCtrl);
	DDX_Control(pDX, IDC_ENDDATE, m_endDateCtrl);
	DDX_Control(pDX, IDOK, m_OKButton);
	DDX_Control(pDX, IDC_FILENAME, m_fileNameEdit);
	DDX_Control(pDX, IDC_SUBSYSTEMLIST, m_subsystemList);
	DDX_Control(pDX, IDC_ALLEVENTS, m_allEvents_Radio);
	DDX_Control(pDX, IDC_SUBSYSTEM, m_subsystem_Radio);
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(EventExtractorDlg, CDialog)
	//{{AFX_MSG_MAP(EventExtractorDlg)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_ALLEVENTS, OnAllevents)
	ON_BN_CLICKED(IDC_SUBSYSTEM, OnSubsystem)
	ON_BN_CLICKED(IDC_PATH, OnPath)
	ON_EN_CHANGE(IDC_FILENAME, OnChangeFilename)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTDATE, OnDatetimechangeStartDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_STARTTIME, OnDatetimechangeStartTime)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_ENDDATE, OnDatetimechangeEndDate)
	ON_NOTIFY(DTN_DATETIMECHANGE, IDC_ENDTIME, OnDatetimechangeEndTime)
	ON_BN_CLICKED(IDC_CLEARSEL, OnClearSelection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// EventExtractorDlg message handlers

BOOL EventExtractorDlg::OnInitDialog()
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



	// Load the db connection strings
    std::string dbConnectionFile = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
    TA_Base_Core::DbConnectionStrings dbConnectionStrings(dbConnectionFile);

    //std::vector<TA_Base_Core::DataConnection> dataConnections = dbConnectionStrings.getConnectionList(TA_Base_Core::Archive_d, TA_Base_Core::Read);

	m_subsystems = TA_Base_Core::SubsystemAccessFactory::getInstance().getAllSubsystems();
	
	// Populate the subsystem list

	std::vector<TA_Base_Core::ISubsystem*>::iterator itr = m_subsystems.begin();

	unsigned int index = 0;

	//m_subsystemList.InsertColumn(0, "Subsystem",LVCFMT_LEFT, 300);

	RECT rectTemp;
	m_subsystemList.GetClientRect(&rectTemp);
	UINT uSubsystemListWidth = rectTemp.right - rectTemp.left;
	UINT uVScrollBarWidth = GetSystemMetrics(SM_CXVSCROLL);
	UINT uColumnWidth = uSubsystemListWidth - uVScrollBarWidth;
	m_subsystemList.InsertColumn(0, "Subsystem",LVCFMT_LEFT,uColumnWidth);

	for(; itr!=m_subsystems.end(); itr++)
	{
		if( (*itr)->getKey() != 0 )
		{
			m_subsystemList.InsertItem( index, (*itr)->getName().c_str());
			m_subsystemList.SetItemData( index, (*itr)->getKey());
			m_subsystemList.SetItemText( index, 0, (*itr)->getName().c_str() );
			index++;
		}
	}

	m_allEvents_Radio.setType(FooButton::Type::radio); 
	m_allEvents_Radio.setTextStyle(FooButton::Text::singleLine);
    m_allEvents_Radio.addToGroup("IS_ALL_EVENTS");


	m_subsystem_Radio.setType(FooButton::Type::radio);
	m_subsystem_Radio.setTextStyle(FooButton::Text::singleLine);
    m_subsystem_Radio.addToGroup("IS_ALL_EVENTS");

	checkButton(FooButton::Type::radio, IDC_ALLEVENTS, true);

	m_startDateCtrl.GetTime( m_startTime);
	m_startTimeCtrl.GetTime( m_startTime);
	m_endDateCtrl.GetTime( m_endTime);
	m_endDateCtrl.GetTime( m_endTime);

	CTime currentTime = CTime::GetCurrentTime();

	m_startDateCtrl.SetRange(NULL, &currentTime);
	m_startTimeCtrl.SetRange(NULL, &currentTime);
	m_endDateCtrl.SetRange(NULL, &currentTime);
	m_endDateCtrl.SetRange(NULL, &currentTime);
	


	m_OKButton.EnableWindow(false);

	return TRUE;  // return TRUE  unless you set the focus to a control
}


void EventExtractorDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		//CAboutDlg dlgAbout;
		//dlgAbout.DoModal();
		TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();
	}
	else
	{
		CDialog::OnSysCommand(nID, lParam);
	}
}


// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void EventExtractorDlg::OnPaint() 
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
HCURSOR EventExtractorDlg::OnQueryDragIcon()
{
	return (HCURSOR) m_hIcon;
}

void EventExtractorDlg::OnAllevents() 
{
	// TODO: Add your control notification handler code here
	m_isAllEvents = true;
	enableControl( IDC_SUBSYSTEMLIST, false);
	
}

void EventExtractorDlg::OnSubsystem() 
{
	// TODO: Add your control notification handler code here
	m_isAllEvents = false;
	enableControl(IDC_SUBSYSTEMLIST, true);
	
}

void EventExtractorDlg::checkButton(const FooButton::Type type, const int itemId, const bool wantCheck)
{
    FooButton* button = reinterpret_cast<FooButton*>(GetDlgItem(itemId));

    button->setType(type);
    button->check(wantCheck);
}


void EventExtractorDlg::enableControl(const int itemId, const int enable /*= TRUE */)
{
    CWnd* wnd = reinterpret_cast<CWnd*>(GetDlgItem(itemId));
    wnd->EnableWindow(enable);
}


void EventExtractorDlg::OnPath() 
{
	// TODO: Add your control notification handler code here

	CFileDialogArial dlg(TRUE,				// We want a Save dialog not an Open one
					".csv",					// The default file extension
					"Untitled",				// This is the initial file name
					OFN_HIDEREADONLY|		// Hide the readonly check box
					OFN_OVERWRITEPROMPT|	// Prompt to ask the user if they want to overwrite existing files
					OFN_PATHMUSTEXIST,		// Check that the path entered is correct and warn if it is not
					"CSV (Comma delimited) (*.csv)|*.csv||",	// Only allow them to save in csv format
					this);					// Parent window is the actual event extractor dialog

	if (dlg.DoModal() != IDOK)
	{
		return;
	}
	else
	{
		m_fileNameEdit.SetWindowText( dlg.GetPathName());
	}

}

void EventExtractorDlg::OnChangeFilename() 
{
	// TODO: If this is a RICHEDIT control, the control will not
	// send this notification unless you override the CDialog::OnInitDialog()
	// function and call CRichEditCtrl().SetEventMask()
	// with the ENM_CHANGE flag ORed into the mask.
	
	// TODO: Add your control notification handler code here

	m_fileNameEdit.GetWindowText(m_fileName);
	
}

void EventExtractorDlg::OnDatetimechangeStartDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here

	m_startDateCtrl.GetTime( m_startTime);
	m_startTimeCtrl.SetTime( &m_startTime);

	if( m_startTime < m_endTime )
	{
		m_OKButton.EnableWindow(true);
	}
	else
	{
		m_OKButton.EnableWindow(false);
	}

	*pResult = 0;
}

void EventExtractorDlg::OnDatetimechangeStartTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_startTimeCtrl.GetTime( m_startTime);
	m_startDateCtrl.SetTime( &m_startTime);

	if( m_startTime < m_endTime )
	{
		m_OKButton.EnableWindow(true);
	}
	else
	{
		m_OKButton.EnableWindow(false);
	}	
	*pResult = 0;
}

void EventExtractorDlg::OnDatetimechangeEndDate(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_endDateCtrl.GetTime( m_endTime);
	m_endTimeCtrl.SetTime( &m_endTime);

	if( m_startTime < m_endTime )
	{
		m_OKButton.EnableWindow(true);
	}
	else
	{
		m_OKButton.EnableWindow(false);
	}

	*pResult = 0;
}

void EventExtractorDlg::OnDatetimechangeEndTime(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// TODO: Add your control notification handler code here
	m_endTimeCtrl.GetTime( m_endTime);
	m_endDateCtrl.SetTime( &m_endTime);

	if( m_startTime < m_endTime )
	{
		m_OKButton.EnableWindow(true);
	}
	else
	{
		m_OKButton.EnableWindow(false);
	}	
	*pResult = 0;
}

void EventExtractorDlg::OnOK() 
{
	// TODO: Add extra validation here

	if(m_fileName.IsEmpty())
	{
		MessageBox( "No export file name specified", "Error", MB_ICONERROR );

		return;
	}


	//delete by lin
	//std::ostringstream subsystemClause;
	std::ostringstream subsystemList;


	if( !m_isAllEvents )
	{
		//m_subsystemList

		std::vector<unsigned int> selectedItemIndices = TA_Base_Bus::SelectionHelper::getSelectedItemIndices( m_subsystemList);

		if( selectedItemIndices.empty() )
		{
			MessageBox( "No subsystems selected", "Error", MB_ICONERROR );
			return;
		}

		//delete by lin
		//std::ostringstream subsystemList;

		std::vector<unsigned int>::iterator itr = selectedItemIndices.begin();

		for(; itr != selectedItemIndices.end(); itr++)
		{
			if( itr == selectedItemIndices.begin())
			{
				subsystemList << "(" << m_subsystemList.GetItemData(*itr);
			}
			else
			{
				subsystemList << "," << m_subsystemList.GetItemData(*itr);
			}
			
		}

		subsystemList <<")";

		//delete by lin
		//subsystemClause << " AND ( SUBSYSTEM_KEY IN "<< subsystemList.str()
		//				<< " OR PHYSICAL_SUBSYSTEM_KEY IN "<< subsystemList.str() << ")";
	}

	// Generate the time restriction clause

	const unsigned int bufLen = 27;
	char buf[bufLen];

	time_t fromTime = m_startTime.GetTime();
	std::string fromTimeString;
	LOGBEFORECOTS("ACE_OS::ctime_r" );
	fromTimeString = ACE_OS::ctime_r( &fromTime, buf, bufLen );
	LOGAFTERCOTS("ACE_OS::ctime_r");

	// and remove the characters we don't want - staring with the last two
	fromTimeString.erase(24,2);

	// and then the first 4
	fromTimeString.erase(0,4);

	// Add the coversions required.
	//delete by lin
	//fromTimeString = "TO_DATE('" + fromTimeString + "','Mon DD HH24:MI:SS YYYY')";
	
	time_t toTime = m_endTime.GetTime();
	std::string toTimeString;
	
	toTimeString = ACE_OS::ctime_r( &toTime, buf, bufLen );

	// and remove the characters we don't want - staring with the last two
	toTimeString.erase(24,2);

	// and then the first 4
	toTimeString.erase(0,4);

	// Add the coversions required.
	//delete by lin
	//toTimeString = "TO_DATE('" + toTimeString + "','Mon DD HH24:MI:SS YYYY')";

	TA_Base_Core::IDatabase* databaseConnection = TA_Base_Core::DatabaseFactory::getInstance().getDatabase(TA_Base_Core::Archive_d, TA_Base_Core::Read);

	TA_Base_Core::SQLTypeAdapter sqlParamFromTimeStr, sqlParamToTimeStr;
	sqlParamFromTimeStr.add(TA_Base_Core::enumDateFormat_5, TA_Base_Core::enumFuncFormat_2, fromTimeString, false);
	sqlParamToTimeStr.add(TA_Base_Core::enumDateFormat_5, TA_Base_Core::enumFuncFormat_2, toTimeString, false);

	//delete by lin
    //std::ostringstream selectSql;
	//selectSql << " SELECT TO_CHAR(" << COLUMN_DATETIME <<" ,'YYYYMMDDHH24MISSFF3') as"<< COLUMN_DATETIME << ","
	//						<< COLUMN_SEVERITY		<< ","
	//						<< COLUMN_ASSET			<< ","
	//						<< COLUMN_DESCRIPTION	<< ","
	//						<< COLUMN_VALUE			<< ","
	//						<< COLUMN_MMS			<< ","
	//						<< COLUMN_DSS			<< ","
	//						<< COLUMN_AVL			<< ","
	//						<< COLUMN_OPERATOR		<< ","
	//						<< COLUMN_COMMENT
	//						<< " FROM EV_COMBINED";

	//selectSql << " WHERE CREATEDATETIME >= " << fromTimeString << " AND "
	//						<< "CREATEDATETIME <= " << toTimeString;
	//if( !m_isAllEvents)
	//{
	//	selectSql << subsystemClause.str();
	//}
	//selectSql << " ORDER BY CREATEDATETIME DESC";

	
	TA_Base_Core::SQLStatement strSql;
	if( !m_isAllEvents)
	{
		databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13065,
			sqlParamFromTimeStr, sqlParamToTimeStr, subsystemList.str(), subsystemList.str());
	}
	else
	{
		databaseConnection->prepareSQLStatement(strSql, EV_COMBINED_SELECT_13064,
			sqlParamFromTimeStr, sqlParamToTimeStr);

	}

    std::vector<std::string> columnNames;
	columnNames.push_back(COLUMN_DATETIME);
	columnNames.push_back(COLUMN_SEVERITY);
	columnNames.push_back(COLUMN_ASSET);
	columnNames.push_back(COLUMN_DESCRIPTION);
	columnNames.push_back(COLUMN_VALUE);
	columnNames.push_back(COLUMN_MMS);
	columnNames.push_back(COLUMN_DSS);
	columnNames.push_back(COLUMN_AVL);
	columnNames.push_back(COLUMN_OPERATOR);
	columnNames.push_back(COLUMN_COMMENT);

    //std::auto_ptr<TA_Base_Core::IData> data( databaseConnection->executeQuery( selectSql.str(), columnNames ) );
	
	//waitforcheck delete by lin
	std::auto_ptr<TA_Base_Core::IData> data( databaseConnection->executeQuery( strSql, columnNames ) );

	std::ofstream outFile( m_fileName );
    if ( !outFile.is_open() )
    {
        throw std::exception( "Couldn't open file for writing." );
    }

	std::vector<std::string>::iterator columnItr;
	for( columnItr = columnNames.begin(); columnItr != columnNames.end(); columnItr++ )
	{
		if( columnItr != columnNames.begin())
		{
			outFile<< ",";
		}

		outFile << (*columnItr);
	}
	outFile << "\n";

	if ( 0 < data->getNumRows() )
    {
        for( unsigned int i=0; i< data->getNumRows(); i++ )
        {
			for( columnItr = columnNames.begin(); columnItr != columnNames.end(); columnItr++ )
			{
				if( columnItr != columnNames.begin())
				{
					outFile<< ",";
				}

				if((*columnItr) == COLUMN_DATETIME )
				{
					std::string dateTimeString(CTime(data->getTimestampData(i, COLUMN_DATETIME).time).Format("%d-%b-%Y %H:%M:%S"));
					outFile<< dateTimeString;
				}
				else
				{
					std::string strTemp = data->getStringData(i, (*columnItr));
					
					if((*columnItr) == COLUMN_MMS) 
					{
						outFile<< getMmsStatusIdentifier(strTemp);
					}
					else if((*columnItr) == COLUMN_DSS) 
					{
						outFile<< getDssPlanStatusIdentifier(strTemp);
					}
					else if((*columnItr) == COLUMN_AVL)
					{
						outFile<< getAvlStatusIdentifier(strTemp);	
					}
					else
					{
						outFile<< strTemp;
					}
				}
				
			}
			outFile<<"\n";
		}
	}

	TA_Base_Core::IData* dataPtr = NULL;
    while( databaseConnection->moreData( dataPtr ) && ( 0 < dataPtr->getNumRows() ) )
    {
        // Take care of memory management.
        std::auto_ptr<TA_Base_Core::IData> dataNext( dataPtr );

        for( unsigned int i=0; i< dataNext->getNumRows(); i++ )
        {
			for( columnItr = columnNames.begin(); columnItr != columnNames.end(); columnItr++ )
			{
				if( columnItr != columnNames.begin())
				{
					outFile<< ",";
				}

				if((*columnItr) == COLUMN_DATETIME )
				{
					std::string dateTimeString(CTime(dataNext->getTimestampData(i, COLUMN_DATETIME).time).Format("%d-%b-%Y %H:%M:%S"));
					outFile<< dateTimeString;
				}
				else
				{
					std::string strTemp = data->getStringData(i, (*columnItr));
					
					if((*columnItr) == COLUMN_MMS) 
					{
						outFile<< getMmsStatusIdentifier(strTemp);
					}
					else if((*columnItr) == COLUMN_DSS) 
					{
						outFile<< getDssPlanStatusIdentifier(strTemp);
					}
					else if((*columnItr) == COLUMN_AVL)
					{
						outFile<< getAvlStatusIdentifier(strTemp);	
					}
					else
					{
						outFile<< strTemp;
					}
				
				}
				
			}
			outFile<<"\n";
		}

		dataPtr = NULL;
    }
	
	outFile.flush();
	outFile.close();

	CDialog::OnOK();
}

void EventExtractorDlg::OnClearSelection() 
{
	// TODO: Add your control notification handler code here
	TA_Base_Bus::SelectionHelper::deselectAll( m_subsystemList);	
}
const std::string& EventExtractorDlg::getMmsStatusIdentifier(const std::string& mmsStatus)
{
	//check the given string with the static const used in the alarm data
	//as mms status is a number under alarm data, there is need to convert the string to number
	int status = atoi(mmsStatus.c_str());
	
	switch (status)
	{
		case TA_Base_Core::AlarmData::TYPE_SEMI:
			return MMS_SEMI_ID;
			break;
		case TA_Base_Core::AlarmData::TYPE_AUTO:
			return MMS_AUTO_ID;
			break;
		case TA_Base_Core::AlarmData::TYPE_JR_SENT:
			return MMS_JR_SENT_ID;
			break;
		case TA_Base_Core::AlarmData::TYPE_ALM_SENT:
			return MMS_AUTO_ID;
			break;
		case TA_Base_Core::AlarmData::TYPE_INHIBIT:
			return MMS_INHIBIT_ID;
			break;
		case TA_Base_Core::AlarmData::TYPE_FAILED:
			return MMS_FAILED_ID;
			break;
		default:
			return DEFAULT_VALUE;
	}
}

const std::string& EventExtractorDlg::getDssPlanStatusIdentifier(const std::string& dssStatus)
{
	if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::AUTO_PLAN_NOT_RUN) )
	{
		return AUTO_PLAN_NOT_RUN_ID;
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::SEMIAUTO_PLAN_NOT_RUN) )
	{
		return SEMI_AUTO_PLAN_NOT_RUN_ID;
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::PLAN_RUN) )
	{
		return PLAN_RUN_ID;
	}
	else if ( 0 == dssStatus.compare(TA_Base_Core::AlarmData::PLAN_POSTPONED) )
	{
		return PLAN_POSTPONED_ID;
	}
	else
	{
		return DEFAULT_VALUE;
	}
}

const std::string& EventExtractorDlg::getAvlStatusIdentifier(const std::string& avlStatus)
{
	//check the given string with the static const used in the alarm data
	//as avl status is a number under alarm data, there is need to convert the string to number
	int status = atoi(avlStatus.c_str());
	
	switch (status)
	{
		case TA_Base_Core::AlarmData::HEAD_AVL:
			return AVL_HEAD_ID;
			break;
		case TA_Base_Core::AlarmData::CHILD_AVL:
			return AVL_CHILD_ID;
			break;
		default:
			return DEFAULT_VALUE;
	}
}
