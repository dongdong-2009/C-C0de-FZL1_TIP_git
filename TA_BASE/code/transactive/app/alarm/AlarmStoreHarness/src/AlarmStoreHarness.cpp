// AlarmStoreHarness.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "AlarmStoreHarness.h"
#include "AlarmStoreHarnessDlg.h"

#include "core/utilities/src/RunParams.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/src/UtilityInitialiser.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessApp

BEGIN_MESSAGE_MAP(CAlarmStoreHarnessApp, CWinApp)
	//{{AFX_MSG_MAP(CAlarmStoreHarnessApp)
		// NOTE - the ClassWizard will add and remove mapping macros here.
		//    DO NOT EDIT what you see in these blocks of generated code!
	//}}AFX_MSG
	ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessApp construction

CAlarmStoreHarnessApp::CAlarmStoreHarnessApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CAlarmStoreHarnessApp object

CAlarmStoreHarnessApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CAlarmStoreHarnessApp initialization

BOOL CAlarmStoreHarnessApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	//  of your final executable, you should remove from the following
	//  the specific initialization routines you do not need.

#ifdef _AFXDLL
	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

// 	TA_Base_Core::CorbaUtil::getInstance().initialise();
// 	TA_Base_Core::CorbaUtil::getInstance().activate();
// 
// 
// 	// constructor command line parameter
// 	std::string param = GetCommandLine();
// 	/*param += "--notify-hosts=";
// 	param += "localhost:6667,localhost:6667";
// 	param += " ";
// 	param += "--debug-level=";
// 	param += "DEBUG";
// 	param += " ";
// 	param += "--db-connection-file=";
// 	param += "c:\\transactive\\config\\database\\OCCConnectionStrings.csv";
// 	param += " ";
// 	param += "--db-connection=";
// 	param += "TRANSACT:it01271301_site:it01271301_site";
// 	param += " ";
// 	param += "--DataBase_TRANSACT=DataBaseGood ";
// 	param += "--location-key=";
// 	param += "1";*/
// 	TA_Base_Core::RunParams::getInstance().parseCmdLine( param.c_str() );
// 
// 	// set session id
// /*	TA_Base_Core::RunParams::getInstance().set("SessionId", "165442950dc530460b0bd9211faa580c6deb");
// 	//TA_Base_Core::RunParams::getInstance().set("DebugLevel", "DEBUG");
// 	//TA_Base_Core::RunParams::getInstance().set("NotifyHosts", "occa:6667,occb:6667");
// 	//std::string debugLevel = TA_Base_Core::RunParams::getInstance().get("DebugLevel");
// 
// 
// 	// log file
// 	TA_Base_Core::DebugUtil::getInstance().setFile("./AlarmStoreHarness.log");
// 	TA_Base_Core::DebugUtil::getInstance().setMaxSize(90000000);
// 	TA_Base_Core::DebugUtil::getInstance().setMaxFiles(10);
// 
// 	// set debug level
// 	TA_Base_Core::DebugUtil::getInstance().setLevel(TA_Base_Core::DebugUtil::EDebugLevel::DebugMsgPub);
// */
// 	//TA_Base_Core::RunParams::getInstance().set("EntityName", "OccAlarmAgent");
// 	std::string debugFile = TA_Base_Core::RunParams::getInstance().get("DebugFile");
// 	if( !debugFile.empty() )
// 	{
// 		TA_Base_Core::DebugUtil::getInstance().setFile( debugFile.c_str() );
// 
// 		std::string dbgParam = "DebugFileMaxSize";
// 		unsigned int debugFileSize = TA_Base_Core::getRunParamValue( dbgParam, 50000000 );
// 		TA_Base_Core::DebugUtil::getInstance().setMaxSize( debugFileSize );
// 
// 		std::string dbgLevel = TA_Base_Core::RunParams::getInstance().get("DebugLevel");
// 		TA_Base_Core::DebugUtil::EDebugLevel eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugInfo;
// 		if( dbgLevel.compare("DEBUG") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugDebug;
// 		}
// 		else if( dbgLevel.compare("SQL") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugSQL;
// 		}
// 		else if( dbgLevel.compare("CORBA") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugCORBA;
// 		}
// 		else if( dbgLevel.compare("MSG") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugMsg;
// 		}
// 		else if( dbgLevel.compare("MSGPUB") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugMsgPub;
// 		}
// 		else if( dbgLevel.compare("PERIODIC") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugPeriodic;
// 		}
// 		else if( dbgLevel.compare("DATABASE") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugDatabase;
// 		}
// 		else if( dbgLevel.compare("EXTERNAL") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugExternal;
// 		}
// 		else if( dbgLevel.compare("TRACE") == 0 )
// 		{
// 			eLevel = TA_Base_Core::DebugUtil::EDebugLevel::DebugTrace;
// 		}
// 
// 		TA_Base_Core::DebugUtil::getInstance().setLevel( eLevel );
// 	}

// 	// Test DataParser
// 	const std::string fileName = "E:\\P4Space\\4669_TIP\\TA_BASE\\transactive\\app\\alarm\\AlarmStoreHarness\\Configuration\\0004\\New Ack close.cfg";
// 	DataParser::getInstance().populateFromFile(fileName);
// 
// 	std::ofstream file("c:\\alarmConfig.txt");
// 	AlarmMessageDetail detail;
// 	while(DataParser::getInstance().nextAlarmDetail(detail))
// 	{
// 		std::string alarm = printAMD(detail);	
// 		if(file.is_open())
// 		{
// 			file << alarm ;//<< std::endl;
// 		}
// 	}
// 
// 	file.close();


	// constructor command line parameter
	std::string param = GetCommandLine();

	TA_Base_Core::UtilityInitialiser::initialiseUtilities(param);

	// ACE init
	LOGBEFORECOTS("ACE::init" );
	ACE::init();
	LOGAFTERCOTS("ACE::init");
	
	CAlarmStoreHarnessDlg dlg;
	dlg.Initialize();
	m_pMainWnd = &dlg;
	int nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
