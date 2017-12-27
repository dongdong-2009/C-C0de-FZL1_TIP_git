/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutil.cpp $
  * @author:  Ripple
  * @version: $Revision: #4 $
  *
  * Last modification: $DateTime: 2012/01/13 10:05:23 $
  * Last modified by:  $Author: liwei.gong $
  *
  */
// crutil.cpp : Defines the class behaviors for the application.
//

#include <sstream>

#include "stdafx.h"
#include "crutil.h"
#include "crutilDlg.h"
#include "EventLogger.hpp"
#include "globals.h"
#include "strings.h"

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/utilities/src/DebugUtilInit.h"
#include "core/utilities/src/DebugSETranslator.h"
#include "core/utilities/src/RunParams.h"
#include "core/database/src/SimpleDb.h"
#include "core/database/src/OCIAccess.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/process_management/src/UtilityInitialiser.h"
#include "core/exceptions/src/ReportManagerException.h"

// marvin++
// bug 716(TD12457)
#include "bus/generic_gui/src/HelpLauncher.h"
#include "core/versioning/src/Version.h"
// ++marvin
// bug 716(TD12457)

//#include "app/system_control/system_controller/src/ProcessManager.h"
#include "crutilGui.h"
#endif // TRANSACTIVE_BUILD

// Cannot include full ACE header because of conflicting typedef between
// fstream.h (which ACE includes) and oratypes.h (which SimpleDb imports).
// We're going to trick the OS.h file that we have already included fstream.h
#define _INC_FSTREAM
#include "ace/ACE.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef TRANSACTIVE_BUILD
using TA_Base_Bus::TransActiveWinApp;

namespace TA_Base_App
{
#endif // TRANSACTIVE_BUILD

	/////////////////////////////////////////////////////////////////////////////
	// CCrutilApp

	BEGIN_MESSAGE_MAP(CCrutilApp, CWinApp)
		//{{AFX_MSG_MAP(CCrutilApp)
			// NOTE - the ClassWizard will add and remove mapping macros here.
			//    DO NOT EDIT what you see in these blocks of generated code!
		//}}AFX_MSG
		ON_COMMAND(ID_HELP, CWinApp::OnHelp)
	END_MESSAGE_MAP()

	void reportCreateMiniDump( EXCEPTION_POINTERS* pep )
	{
		// Open the file 

		HANDLE hFile = CreateFile( _T("MiniDump.dmp"), GENERIC_READ | GENERIC_WRITE, 
			0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL ); 

		if( ( hFile != NULL ) && ( hFile != INVALID_HANDLE_VALUE ) ) 
		{
			// Create the minidump 

			MINIDUMP_EXCEPTION_INFORMATION mdei; 

			mdei.ThreadId           = GetCurrentThreadId(); 
			mdei.ExceptionPointers  = pep; 
			mdei.ClientPointers     = FALSE; 

			MINIDUMP_TYPE mdt       = MiniDumpNormal; 

			BOOL rv = MiniDumpWriteDump( GetCurrentProcess(), GetCurrentProcessId(), 
				hFile, mdt, (pep != 0) ? &mdei : 0, 0, 0 ); 

			if( !rv ) 
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"In reportCreateMiniDump, MiniDumpWriteDump failed. Error: %u", GetLastError());
			else 
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"In reportCreateMiniDump, Minidump created.");

			// Close the file 

			CloseHandle( hFile ); 

		}
		else 
		{
			_tprintf( _T("CreateFile failed. Error: %u \n"), GetLastError() ); 
		}

	}

	// reportUnhandleFilter for unhandled exceptions
	LONG __stdcall reportUnhandleFilter( EXCEPTION_POINTERS* pep )
	{
		reportCreateMiniDump( pep ); 

		return EXCEPTION_EXECUTE_HANDLER; 
	}

	/////////////////////////////////////////////////////////////////////////////
	// CCrutilApp construction

	#ifdef TRANSACTIVE_BUILD
	CCrutilApp::CCrutilApp()
	: TransActiveWinApp(new TA_Reporting::crutilGui(), "报告管理器")
	{
	}
	#else
	CCrutilApp::CCrutilApp()
	{
	}
	#endif // TRANSACTIVE_BUILD

	/////////////////////////////////////////////////////////////////////////////
	// The one and only CCrutilApp object

	CCrutilApp theApp;

	/////////////////////////////////////////////////////////////////////////////
	// CCrutilApp initialization

	BOOL CCrutilApp::InitInstance()
	{
    	CoInitialize(NULL);

		SetUnhandledExceptionFilter( reportUnhandleFilter );

        // TD 4567: Ace needs to be initialised otherwise localtime_r cannot be used later
        ACE::init();

		#ifndef TRANSACTIVE_BUILD

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

		#else
    
            using TA_Base_Core::DebugUtil;
    		TA_Base_Core::RunParams::getInstance().parseCmdLine( this->m_lpCmdLine );

		#endif // TRANSACTIVE_BUILD

		// Lets parse the command line.
		#ifdef TRANSACTIVE_BUILD
        TA_Reporting::CommandLineParser objArgv;
		#else
        CommandLineParser objArgv;
		#endif // TRANSACTIVE_BUILD

		// First, lets see if the usage information needs to be displayed.
		if (objArgv.parameterExists( HELP_LABEL ) ||
			objArgv.parameterExists( H_LABEL ) ||
			objArgv.parameterExists( QUESTION_LABEL ))
		{
            TA_Base_Bus::TransActiveMessage userMsg;
#ifdef TRANSACTIVE_BUILD
            userMsg.showMsgBox(IDS_UE_140035);
#else
            userMsg.showMsgBox(IDS_UE_140034);
#endif
			return( FALSE );
		}

		// next, lets see if the version needs to be displayed.
		if (objArgv.parameterExists( VERSION_LABEL ))
		{

			// marvin++
			// bug 716(TD12457)
			#ifdef TRANSACTIVE_BUILD

        	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

			#endif // TRANSACTIVE_BUILD
			// ++marvin
			// bug 716(TD12457)
            
			return( FALSE );
		}

		// Next, lets create an instance of the Crystal Reports Helper class.
		#ifdef TRANSACTIVE_BUILD

        TA_Base_Core::gSetDebugUtilFromRunParams();
        TA_Base_Core::gInitStructuredExceptionHandler();   // Only initialises the calling thread
		TA_Reporting::CrystalReportsHelper* pobjCrystalReports = new TA_Reporting::CrystalReportsHelper();
        
		#else
		CrystalReportsHelper* pobjCrystalReports = new CrystalReportsHelper();
		#endif // TRANSACTIVE_BUILD
		if ((pobjCrystalReports) && (pobjCrystalReports->isReady()))
		{
			// Next, lets process any database connection command line options 
			// if they exist.
			if (objArgv.parameterExists( DLLNAME_LABEL )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( DLLNAME_LABEL );
                pobjCrystalReports->setDLLName( const_cast <char*> (value[0].c_str()) );
            }
			if (objArgv.parameterExists( SERVERNAME_LABEL )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( SERVERNAME_LABEL );
                pobjCrystalReports->setServerName( const_cast <char*> (value[0].c_str()) );
            }
			if (objArgv.parameterExists( DATABASENAME_LABEL )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( DATABASENAME_LABEL );
				pobjCrystalReports->setDatabaseName( const_cast <char*> (value[0].c_str()) );
            }
			if (objArgv.parameterExists( USERID_LABEL )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( USERID_LABEL );
				pobjCrystalReports->setUserID( const_cast <char*> (value[0].c_str()) );
            }
			
			if (objArgv.parameterExists( PASSWORD_LABEL )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( PASSWORD_LABEL );
				pobjCrystalReports->setPassword( const_cast <char*> (value[0].c_str()) );
            }

            // And any other remaining parameters that need to be processed.
			if (objArgv.parameterExists( NOPARAMPROMPTING_LABEL )) 
            {
				pobjCrystalReports->enableParameterPrompting( false );
            }
			
			// Now, lets perform the view, print or export function as determined
			// by the command line options.
			if (objArgv.parameterExists( REPORTNAME_LABEL ) == false)
			{
				#ifdef TRANSACTIVE_BUILD
                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Report file name was not provided.");
				if (pobjCrystalReports) 
				{
					delete pobjCrystalReports;
					pobjCrystalReports = NULL;
				}
				return TransActiveWinApp::InitInstance();
				#else
				display( &objArgv, "", pobjCrystalReports );
				#endif // TRANSACTIVE_BUILD
			}
			else
			{
				// Next, ensure that a report template file name has been provided.
				if (objArgv.parameterExists( REPORTNAME_LABEL ))
				{
                    std::vector<std::string> values = objArgv.getParameterValue( REPORTNAME_LABEL );
					char* pszReportName = const_cast <char*> (values[0].c_str());
					if (pszReportName)
					{
						pobjCrystalReports->setReportName( pszReportName );

                        //************************************************
						// Set Report Parameters.
						//
                        std::vector<std::string> reportParameters;
                        if(objArgv.parameterExists( REPORTPARAMETER_LABEL ))
                        {
                            reportParameters = objArgv.getParameterValue( REPORTPARAMETER_LABEL );
                        }

						//************************************************
						// Export Reports.
						//
						if (objArgv.parameterExists( EXPORTREPORT_LABEL ))
						{
							if (objArgv.parameterExists( ENABLEPROMPT_LABEL )) 
                            {
								pobjCrystalReports->enablePrompt();
                            }
							if (objArgv.parameterExists( EXPORTFORMAT_LABEL )) 
                            {
                                std::vector<std::string> value = objArgv.getParameterValue( EXPORTFORMAT_LABEL );
								pobjCrystalReports->setFormat( const_cast <char*> (value[0].c_str()) );
                            }
							if (objArgv.parameterExists( OUTFILE_LABEL )) 
                            {
                                std::vector<std::string> value = objArgv.getParameterValue( OUTFILE_LABEL );
								pobjCrystalReports->setOutputFilename(const_cast <char*> (value[0].c_str()));
                            }
                            if (objArgv.parameterExists( OUTPATH_LABEL )) 
                            {
                                std::vector<std::string> value = objArgv.getParameterValue( OUTPATH_LABEL );
								pobjCrystalReports->setOutputPath( const_cast <char*> (value[0].c_str()) );
                            }
							if (objArgv.parameterExists( ENABLETIMESTAMP_LABEL )) 
                            {
								pobjCrystalReports->enableTimeStamping();
                            }

                            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Exporting Report");

                            try
                            {
							    pobjCrystalReports->exportReport(reportParameters);
                            }
                            catch(const TA_Base_Core::ReportManagerException& ex)
                            {
                                LOG_EXCEPTION_CATCH(SourceInfo, "ReportManagerException", ex.what());
                                TA_Base_Bus::TransActiveMessage userMsg;
                                UINT selectedButton = userMsg.showMsgBox(IDS_UE_140036);
                            }
						}

						//************************************************
						// Print Incident Reports.
						//
						if (objArgv.parameterExists( PRINT_INCIDENT_LABEL ))
						{
                    
                            std::vector<std::string> reportidstr = objArgv.getParameterValue(INCIDENT_REPORT_ID_LABEL);
                            char* idstring = const_cast <char*> (reportidstr[0].c_str());
                            std::string reportid = std::string(idstring);
                            
							if (objArgv.parameterExists( ENABLEPROMPT_LABEL )) pobjCrystalReports->enablePrompt();
                
                                pobjCrystalReports->printReport(reportid);

   						}


						//************************************************
						// Print Reports.
						//
						if (objArgv.parameterExists( PRINTREPORT_LABEL ))
						{
							if (objArgv.parameterExists( ENABLEPROMPT_LABEL )) pobjCrystalReports->enablePrompt();

                            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Printing Report");

                            try
                            {
							    pobjCrystalReports->printReport(reportParameters);
                            }
                            catch(const TA_Base_Core::ReportManagerException& ex)
                            {
                                LOG_EXCEPTION_CATCH(SourceInfo, "ReportManagerException", ex.what());
                                TA_Base_Bus::TransActiveMessage userMsg;
                                UINT selectedButton = userMsg.showMsgBox(IDS_UE_140036);
                            }
						}

                        //************************************************
						// View Reports.
						//
						if (objArgv.parameterExists( VIEWREPORT_LABEL ))
						{
							#ifndef TRANSACTIVE_BUILD
							pobjCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );
							#endif // TRANSACTIVE_BUILD

							#ifdef TRANSACTIVE_BUILD
							if (pobjCrystalReports) 
							{
								delete pobjCrystalReports;
								pobjCrystalReports = NULL;
							}
                            
                            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Viewing Report");

                            return TransActiveWinApp::InitInstance();
							#else
							display( &objArgv, pszReportName, pobjCrystalReports );
							#endif // TRANSACTIVE_BUILD
						}
					}
					else
					{
                        LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Report template filename not provided.");

                        TA_Base_Bus::TransActiveMessage userMsg;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140031);
                    }
				}
				else
				{
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Report template filename not provided.");

                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140031);
				}
			}
		}

		delete pobjCrystalReports;
		pobjCrystalReports = NULL;

		CoUninitialize( );

		// Since the dialog has been closed, return FALSE so that we exit the
		//  application, rather than start the application's message pump.
		return FALSE;
	}

	int CCrutilApp::ExitInstance() 
	{
		try
		{
		#ifdef TRANSACTIVE_BUILD
			TransActiveWinApp::ExitInstance();
		#endif // TRANSACTIVE_BUILD

			ACE::fini();

			return CWinApp::ExitInstance();
		}
		catch(...)
		{
			// Do nothing
		}
	}

	#ifndef TRANSACTIVE_BUILD
	void CCrutilApp::display( CommandLineParser* p_objArgv, char* p_pszReportName, CrystalReportsHelper* p_pobjCrystalReports )
	{
		// Lets display the dialog.	
		CCrutilDlg dlg;

		if ((p_objArgv) && (p_objArgv->parameterExists( FULLSCREEN_LABEL ))) dlg.m_bShowMaxWindow = true;

		dlg.m_cstrReportName = p_pszReportName;
		dlg.m_pCrystalReports = p_pobjCrystalReports;
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
	}
	#endif // TRANSACTIVE_BUILD


#ifdef TRANSACTIVE_BUILD
} //TA_Base_App
#endif // TRANSACTIVE_BUILD
