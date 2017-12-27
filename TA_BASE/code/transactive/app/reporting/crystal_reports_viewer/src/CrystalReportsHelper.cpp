/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  B. Carbonaro
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This class is a wrapper for the Crystal Reports Report
  * Designer Components and as such it simplifies their use.
  */

#include "stdafx.h"
#include "time.h"
#include "direct.h"
#include "CrystalReportsHelper.hpp"
#include "EventLogger.hpp"
#include "resource.h"
#include "crutilDlg.h"
#include "globals.h"
#include "strings.h"
#include <sstream>
#include "CommandLineParser.hpp"

#include "ace/OS.h"
#include "ace/SString.h"
#include "ace/Tokenizer_T.h"
#include "oleauto.h"

#ifdef TRANSACTIVE_BUILD


#include "core/utilities/src/RunParams.h"
#include "core/data_access_interface/src/DbConnectionStrings.h"
#include "core/exceptions/src/DatabaseException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/corba/src/CorbaUtil.h"
#include "core/message/src/MessageSubscriptionManager.h"
#include "core/message/src/MessagePublicationManager.h"
#include "core/utilities/src/RunParams.h"
#include "core/message/src/AuditMessageSender.h"
#include "core/message/types/ReportsAudit_MessageTypes.h"
#include "core/exceptions/src/ReportManagerException.h"
#include "core/database/src/SimpleDb.h"

const int DATABASENAME_INDEX = 1;
const int USERID_INDEX = 2;
const int PASSWORD_INDEX = 3;
using namespace TA_Base_Core;
namespace TA_Base_App
{
    namespace TA_Reporting
    {

#endif // TRANSACTIVE_BUILD

        //const long REPORT_MANAGER_ENTITY = 17;

		const char* REPORT_TEMPLATE_EXTENSION = "rpt";
		const char* REPORT_FORMAT_PDF = "pdf";
		const char* REPORT_FORMAT_WORD = "word";
		const char* REPORT_FORMAT_EXCEL = "excel";
		const char* REPORT_FORMAT_RTF = "rtf";
		const char* REPORT_FORMAT_HTML = "html";
		const char* REPORT_FORMAT_XML = "xml";
		const char* REPORT_FORMAT_TEXT = "text";

		const char* PDF_EXTENSION = ".pdf";
		const char* WORD_EXTENSION = ".doc";
		const char* EXCEL_EXTENSION = ".xls";
		const char* RTF_EXTENSION = ".rtf";
		const char* HTML_EXTENSION = ".html";
		const char* XML_EXTENSION = ".xml";
		const char* TEXT_EXTENSION = ".txt";
		const char* CRYSTALREPORT_EXTENSION = ".rpt";

		const char* DEFAULT_DLLNAME = "crdb_oracle";
		const char* DEFAULT_SERVERNAME = "tra_dev1";
		const char* DEFAULT_DATABASENAME = "tra_dev1";
		const char* DEFAULT_USERID = "its";
		const char* DEFAULT_PASSWORD = "its";

		const char* HISTORY_PATH_EXTENSION = "history";

		const char* tblMonths[] = { "January", 
							        "February", 
							        "March", 
							        "April", 
							        "May", 
							        "June", 
							        "July", 
							        "August", 
							        "September", 
							        "October", 
							        "November", 
							        "December" };


		CrystalReportsHelper::CrystalReportsHelper()
		: m_pIApplication( NULL ), m_pIReport( NULL ), m_ccbstrReportSource( "" ),m_doubleClickTree( false ),
		  m_pReportViewer( NULL ), m_bDisplayPrompt( false ), m_bAddTimeStamp( false ), 
		  m_ccbstrPrinterDriver( "" ), m_ccbstrPrinterName( "" ), m_ccbstrPrinterPort( "" ),
		  m_cstrFormat( "" ), m_ccbstrOutputFilename( "" ), m_ccbstrOutputPath( "" ), m_isPrintFromGUI( false ),
		  m_ccbstrDllName( DEFAULT_DLLNAME ), m_ccbstrServerName( DEFAULT_SERVERNAME ), 
		  m_vtDatabaseName( DEFAULT_DATABASENAME ), m_vtUserID( DEFAULT_USERID ), 
		  m_vtPassword( DEFAULT_PASSWORD ), m_pIDatabase( NULL ), m_pITables( NULL ),
		  m_pEventLogger( NULL ), m_crhViewingType( crhOnDemand ), m_pHistoryReportViewer(NULL ),
		  m_cx( 0 ), m_cy( 0 ), m_bEnableParameterPrompting( true ), m_pCCrutilDlg( NULL ),
		  m_cstrUseLogon( YES_STRING ), m_cstrSessionId( "" ), m_cstrOperatorName( "" )

		{
			m_pEventLogger = new EventLogger( APPLICATION_NAME, EXECUTABLE_NAME );
			m_pAppSettings = new AppSettings( COMPANY_NAME, APPLICATION_NAME );

			initialise();
        }


        CrystalReportsHelper::~CrystalReportsHelper()
		{
			// Clean up.
			if (m_pIReport)
			{
				m_pIReport->Release();
				m_pIReport = NULL;
			}

			if (m_pIApplication) 
			{
				m_pIApplication->Release();
				m_pIApplication = NULL;
			}

			if (m_pEventLogger)
			{
				delete m_pEventLogger;
				m_pEventLogger = NULL;
			}

			if (m_pAppSettings)
			{
				delete m_pAppSettings;
				m_pAppSettings = NULL;
			}
		}


        bool CrystalReportsHelper::initialise()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::initialise");
			bool bResult = false;

            // The following code is necessary so because this Crystal Reports Viewer
            // component does not currently support a close report method. Consequently,
            // if you don't release the current instance, every time you open a new 
            // report, it consumes more memory. Hence, by placing the instance release 
            // code here, every call to this method ensures that before creating 
            // an instance, if one already exists, then release it first which releases
            // the memory that was used when the last report was opened.
			if (m_pIReport)
			{
				m_pIReport->Release();
				m_pIReport = NULL;
			}

			if (m_pIApplication) 
			{
				m_pIApplication->Release();
				m_pIApplication = NULL;
			}
           
            // Crystal Reports Viewer Initialisation.
			if (m_pIApplication == NULL)
			{
				HRESULT hr = CoCreateInstance(CLSID_Application, NULL, CLSCTX_ALL, IID_IApplication, (void **) &m_pIApplication);
				if (SUCCEEDED(hr))
				{}
				else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140032);
				}
			}
			if ((m_pIApplication) && (m_pIReport == NULL))
			{
				HRESULT hr = CoCreateInstance(CLSID_Report, NULL, CLSCTX_ALL, IID_IReport, (void **) &m_pIReport);
				if (SUCCEEDED(hr))
				{
					// Success.
					setupDefaults();
					bResult = true;
				}
				else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140032);
				}
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::initialise");

			return( bResult );
		}


        void CrystalReportsHelper::setupDefaults()
		{
			char* pszValue;

			// Use Logon.
			pszValue = m_pAppSettings->getStringValue( REG_USELOGON_LABEL );
			if (pszValue)
			{
				CString cstrTemp = pszValue;
				if (cstrTemp.CompareNoCase( YES_STRING ) == 0)
				{
					m_cstrUseLogon = YES_STRING;
				}
				else
				{
					m_cstrUseLogon = NO_STRING;
				}
			}
			else
			{
				m_cstrUseLogon = YES_STRING;
			}

			// Server Name
			pszValue = m_pAppSettings->getStringValue( REG_SERVERNAME_LABEL );
			if (pszValue)
			{
				m_ccbstrServerName = pszValue;
			}
			else
			{
				m_ccbstrServerName = "";
			}

			// Database Name
			pszValue = m_pAppSettings->getStringValue( REG_DATABASENAME_LABEL );
			if (pszValue)
			{
				m_vtDatabaseName = pszValue;
			}
			else
			{
				m_vtDatabaseName = "";
			}

			// UserID
			pszValue = m_pAppSettings->getStringValue( REG_USERID_LABEL );
			if (pszValue)
			{
				m_vtUserID = pszValue;
			}
			else
			{
				m_vtUserID = "";
			}

			// Password
			pszValue = m_pAppSettings->getStringValue( REG_PASSWORD_LABEL );
			if (pszValue)
			{
				m_vtPassword = pszValue;
			}
			else
			{
				m_vtPassword = "";
			}
		}


		bool CrystalReportsHelper::isReady()
		{
			if ((m_pIApplication ) && (m_pIReport))
			{
				return( true );
			}
			else
			{
				return( false );
			}
		}


        void CrystalReportsHelper::setReportName( char* p_szReportName )
		{
			m_ccbstrReportSource = p_szReportName;
		}


        void CrystalReportsHelper::setViewerInstance( CCrystalReportViewer9* p_pReportViewer )
		{
			m_pReportViewer = p_pReportViewer;
		}


        void CrystalReportsHelper::setBrowserInstance( CWebBrowser2* p_pHistoryReportViewer )
		{
			m_pHistoryReportViewer = p_pHistoryReportViewer;
		}


		void  CrystalReportsHelper::setDialogInstance( CCrutilDlg* p_pCCrutilDlg )
		{
			m_pCCrutilDlg = p_pCCrutilDlg;
		}


        void CrystalReportsHelper::setFormat( char* p_szFormat )
		{
			m_cstrFormat = p_szFormat;
		}


        void CrystalReportsHelper::setOutputFilename( char* p_szOutFile )
		{
			m_ccbstrOutputFilename = p_szOutFile;
		}


        void CrystalReportsHelper::setOutputPath( char* p_szOutPath )
		{
			m_ccbstrOutputPath = p_szOutPath;
		}


        void CrystalReportsHelper::enablePrompt( bool p_bPrompt )
		{
			m_bDisplayPrompt = p_bPrompt;
		}


        void CrystalReportsHelper::enableTimeStamping( bool p_bTimeStamping )
		{
			m_bAddTimeStamp = p_bTimeStamping;
		}


        void CrystalReportsHelper::enableParameterPrompting( bool p_bEnableParameterPrompting)
		{
			m_bEnableParameterPrompting = static_cast<VARIANT_BOOL>(p_bEnableParameterPrompting);
		}


        void CrystalReportsHelper::setDLLName( char* p_szDLLName )
		{
			m_ccbstrDllName = p_szDLLName;
			m_pAppSettings->setStringValue( REG_DLLNAME_LABEL, p_szDLLName );
		}


        void CrystalReportsHelper::setServerName( char* p_szServerName )
		{
			m_ccbstrServerName = p_szServerName;
			m_pAppSettings->setStringValue( REG_SERVERNAME_LABEL, p_szServerName );
		}


        void CrystalReportsHelper::setDatabaseName( char* p_szDatabaseName )
		{
			m_vtDatabaseName = p_szDatabaseName;
			m_pAppSettings->setStringValue( REG_DATABASENAME_LABEL, p_szDatabaseName );
		}


        void CrystalReportsHelper::setUserID( char* p_szUserID )
		{
			m_vtUserID = p_szUserID;
			m_pAppSettings->setStringValue( REG_USERID_LABEL, p_szUserID );
		}


        void CrystalReportsHelper::setPassword( char* p_szPassword )
		{
			m_vtPassword = p_szPassword;
			m_pAppSettings->setStringValue( REG_PASSWORD_LABEL, p_szPassword );
		}


        void CrystalReportsHelper::setViewingType( crhViewingType p_crhViewingType )
		{
			m_crhViewingType = p_crhViewingType;
		}

		
        void CrystalReportsHelper::setSessionID( char* p_szSessionID )
        {
            m_cstrSessionId = p_szSessionID;
        }

		
        void CrystalReportsHelper::setOperatorName( char* p_szOperatorName )
        {
            m_cstrOperatorName = p_szOperatorName;
        }


        CrystalReportsHelper::crhViewingType CrystalReportsHelper::getViewingType( void )
		{
			return( m_crhViewingType );
		}


        void CrystalReportsHelper::setWindowWidth( int p_cx )
		{
			m_cx = p_cx;
		}


        void CrystalReportsHelper::setWindowHeight( int p_cy )
		{
			m_cy = p_cy;
		}

		void CrystalReportsHelper::setTreeBorder( int p_treeBorder )
		{
			m_treeBorder = p_treeBorder;
		}

		bool CrystalReportsHelper::openReport()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::openReport");
			bool bResult = false;

			// If report template doesn't exist, exit immediately.
			if (reportTemplateExists() == false ) return( bResult );
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 1...");

			// Continue with the opening the report template.
			HRESULT hr = m_pIApplication->OpenReport( m_ccbstrReportSource, m_vtValue, &m_pIReport );
			if (SUCCEEDED(hr))
			{
				// The report has been opened successfully.
				bResult = true;

				// Save for future use.
				m_ccbstrReportTemplate = m_ccbstrReportSource;

				// Now lets setup the database logon information.
				if (m_cstrUseLogon.CompareNoCase( YES_STRING ) == 0)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 3...");
					hr = m_pIReport->get_Database( &m_pIDatabase );
					if (SUCCEEDED(hr))
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 4...");
						hr = m_pIDatabase->get_Tables( &m_pITables );
						if (SUCCEEDED(hr))
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 5...");
							IDatabaseTable* pITable;
							long lIndex;
							hr = m_pITables->get_Count( &lIndex );
							if (SUCCEEDED(hr))
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 6...");
								long i;
								for (i = 1; i <= lIndex; i++)
								{
									hr = m_pITables->get_Item( i, &pITable );
									if (SUCCEEDED(hr))
									{
										LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 7...");
										std::string dataBaseName = "";
										DataConnections connections ;
										try
										{
											//DbConnectionStrings connectionStrings(TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE));
											
											// jiangshengguang++ 08-11-06 IncidentReport print button crash
											//std::string filename = "C:\\transActive\\config\\database\\OCCConnectionStrings.CSV";
											/*std::string filename =TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
											if (filename.empty())
											{
												return false;
											}*/

											//libo++
											std::string filename=""; 
											#ifdef TRANSACTIVE_BUILD
												TA_Reporting::CommandLineParser objArgv;
										    //[[xinyang++ CL-20758			
											if (TA_Base_Core::RunParams::getInstance().isSet(RPARAM_DBCONNECTIONFILE))
											{
													std::string value = TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTIONFILE);
													filename = value;
											}
											//]]xinyang++ CL-20758
											#else
												CommandLineParser objArgv;

											if (objArgv.parameterExists( RPARAM_DBCONNECTIONFILE)) 
											{
												 std::vector<std::string> value = objArgv.getParameterValue(RPARAM_DBCONNECTIONFILE);
												 filename = value[0].c_str();
 											}

											#endif // TRANSACTIVE_BUILD
												
 											if(filename.empty())
 											{
												//[[xinyang++ CL-20758
 												TA_Base_Core::DatabaseException("Unable find Local DbConnectionfile!");
												//]]xinyang++ CL-20758
 												return false;
 											}
											//++libo

											DbConnectionStrings connectionStrings(filename);
											// jiangshengguang++ 08-11-06 IncidentReport print button crash
											connections = connectionStrings.getConnectionList(TA_Base_Core::Archiving_Cd, TA_Base_Core::Write);											
											dataBaseName = connections[0].first;
										}
										catch (...) 
										{
											throw TA_Base_Core::DatabaseException("Unable to find database connection infomation");
										}
										 
										//std::string connectionString = connections[0].second;
										//TD 13191
											std::string connectionString ;
											if(connections[0].second.length() != 0)
												connectionString = connections[0].second;
											else
												connectionString= TA_Base_Core::RunParams::getInstance().get(RPARAM_DBCONNECTION);

										std::vector<std::string> dbConnData;
										SimpleDb::ParseConnectStr(connectionString, dbConnData);										
										if (dbConnData.size() >=5)
										{
											LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 9...");
											dbConnData[DATABASENAME_INDEX]=dataBaseName;											
											m_vtDatabaseName = const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str());
											m_vtUserID = const_cast<char*>(dbConnData[USERID_INDEX].c_str());
											m_vtPassword = const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str());
                                            //TD 13180
                                            setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
                                            setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
                                            setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );
                                            //TD 13180
										}
										//TD 13191
										BSTR bstrDllName;
										hr = pITable->get_DllName(&bstrDllName);

										CString strDllName = bstrDllName;
										if(strDllName.Compare("crdb_ado.dll") ==0)
										{
											CString strConnectionString ;
											strConnectionString.Format("Provider=MSDAORA;;Data Source=%s;;User ID=%s;;Password=%s;;Use DSN Default Properties=0;;Locale Identifier=2052;;OLE DB Services=-5",
												                                         dbConnData[DATABASENAME_INDEX].c_str(),
																						 dbConnData[USERID_INDEX].c_str(),
																						dbConnData[PASSWORD_INDEX].c_str());


											BSTR bstrConnectionString = strConnectionString.AllocSysString();
											pITable->put_ConnectBufferString(bstrConnectionString);
											::SysFreeString(bstrConnectionString);
										}
										else
										{
											hr = pITable->SetLogOnInfo( m_ccbstrServerName, 
												m_vtDatabaseName,
												m_vtUserID,
												m_vtPassword );
										}										
										::SysFreeString(bstrDllName);
										if (SUCCEEDED(hr))
										{
											LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 10...");
											CComBSTR ccbstrTableName;
											hr = pITable->get_Name( &ccbstrTableName );
											if (SUCCEEDED(hr))
											{
												hr = pITable->put_Location( ccbstrTableName );
												if (SUCCEEDED(hr))
												{}
											}
										}
										pITable->Release();
									}
								}
							}
							m_pITables->Release();
						}
						m_pIDatabase->Release();
					}
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, 11...");
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::openReport, <1>...");
				initialise();
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::openReport");

			return( bResult );
		}


        bool CrystalReportsHelper::viewReport(std::vector<std::string>& parameterValuePairs)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Begin CrystalReportsHelper::viewReport");
			bool bResult = false;

			CString cstrTempCaptionForDbClick;
			if ( true == getDoubleClickTree() )
			{
				m_pCCrutilDlg->GetWindowText(cstrTempCaptionForDbClick);
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"In viewReport, double click, title now is %s",((std::string)cstrTempCaptionForDbClick).c_str());
			}

			// Initially set the Window's caption to just the application title.
			if (m_pCCrutilDlg) m_pCCrutilDlg->SetWindowText( CAPTION_TITLE );

			// If viewing type is On Demand then use the RDC component
			// else use the Web Browser control to view an historical report.
			if (m_crhViewingType == crhOnDemand)
			{
				// Ensure that we have gain access to the crystal reports COM objects.
				if ((m_pIApplication ) && (m_pIReport))
				{
					// Ensure that there is a valid pointer to an instance of the 
					// ActiveX Report Viewer control.
					if (m_pReportViewer)
					{
						// Ensure that we have a report template to display.
						CString cstrTemp = m_ccbstrReportSource;
						if (cstrTemp != "")
						{
							LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 1...");
							// Open the report template.
							if (openReport() == true )
							{
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2...");
								m_pHistoryReportViewer->MoveWindow(m_treeBorder,0,0,0);
								m_pReportViewer->MoveWindow(m_treeBorder,0,m_cx,m_cy);

                                setupParameters(parameterValuePairs);
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.1...");
								m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
								try
								{
									m_pReportViewer->SetReportSource( m_pIReport );
								}
								catch(...)
								{
									LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "In viewReport, ReportManager show incorrect, need repaint ...");

									m_pCCrutilDlg->repaintDlgAfterOperate();
								}
								m_pReportViewer->SetEnablePrintButton( true );
								m_pReportViewer->SetEnableExportButton( true );
								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 2.4...");
								setupGroupButton();
								m_pReportViewer->SetEnableSearchControl(false);
								try
								{
									m_pReportViewer->ViewReport();
								}
								catch(...)
								{
									LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "In viewReport, ReportManager ViewReport incorrect, need reviewReport ...");

									reviewReportToShowCorrect(parameterValuePairs);
								}

								// Log this event.
								CString cstrLogMsg = "Report ";
								cstrLogMsg += getReportName();
                                #ifdef TRANSACTIVE_BUILD
								cstrLogMsg += " was viewed by ";
								cstrLogMsg += m_cstrOperatorName;
                                #else
								cstrLogMsg += " was opened for viewing";
                                #endif // TRANSACTIVE_BUILD
								cstrLogMsg += ".";

                                #ifdef TRANSACTIVE_BUILD
					            sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                                #else
					            m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                                #endif // TRANSACTIVE_BUILD

								LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, 4...");
                                // Indicate success.
								bResult = true;
							}
							else
							{
								if ( true == getDoubleClickTree() )
								{
									bResult = false;
									if (m_pCCrutilDlg) m_pCCrutilDlg->SetWindowText( cstrTempCaptionForDbClick );
								}
							}
						}
					}
				}
			}
			else
			{
				m_pReportViewer->MoveWindow(m_treeBorder,0,0,0);
				m_pHistoryReportViewer->MoveWindow(m_treeBorder,0,m_cx,m_cy);

				if (m_pHistoryReportViewer)
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "CrystalReportsHelper::viewReport, <2>...");
					CComVariant vtFile;
					vtFile = m_ccbstrReportSource;
					m_pHistoryReportViewer->Navigate2( &vtFile, NULL, NULL, NULL, NULL );

					// Log this event.
					CString cstrLogMsg = "Historical Report ";
					cstrLogMsg += getHistoryReportName();
                    #ifdef TRANSACTIVE_BUILD
					cstrLogMsg += " was viewed by ";
					cstrLogMsg += m_cstrOperatorName;
                    #else
					cstrLogMsg += " was opened for viewing";
                    #endif // TRANSACTIVE_BUILD
					cstrLogMsg += ".";

                    #ifdef TRANSACTIVE_BUILD
					sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #else
					m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #endif // TRANSACTIVE_BUILD

					// Indicate success.
					bResult = true;
				}
			}

			// If a report has been successfully displayed, update the caption accordingly.
			if (bResult)
			{
				CString cstrTemp = CAPTION_TITLE;
				cstrTemp += " - ";
				cstrTemp += getReportFileName();
				cstrTemp.Replace("\\\\","\\");
				if (m_pCCrutilDlg) m_pCCrutilDlg->SetWindowText( cstrTemp );
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End CrystalReportsHelper::viewReport");

			return( bResult );
		}

        bool CrystalReportsHelper::printReport(std::string incidentid)
        {
			bool bResult = false;

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport ))
			{
				// Open the report template.
				if (openReport() == true )
				{
                    // Create an instance of the Command Line Parser object.
                    std::ostringstream resultstream;
                    CommandLineParser objArgv;
                    // Configure the Database connection parameters if the parameter 
                    // has been supplied.
                    //Mintao ++
                    //Bug 643
                    if (objArgv.parameterExists( RPARAM_DBCONNECTION )) 
                    {
                        std::vector<std::string> value = objArgv.getParameterValue( RPARAM_DBCONNECTION );
                        std::string connectionString = value[0];
                        std::vector<std::string> dbConnData;
                        SimpleDb::ParseConnectStr(connectionString, dbConnData);
                        if (dbConnData.size() >=5)
                        {
    				        setServerName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
    				        setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
    				        setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
	    			        setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );
                        }
                    }

                    setupincidentId(incidentid);
					// And print.    
                    long retVal;
                    m_pIReport->PrinterSetupEx(0, &retVal);
                    if (retVal == 0)//user choose to print the report
                    {
                        m_pIReport->put_DisplayProgressDialog(true);
					    VARIANT vtbPrompt;
					    vtbPrompt.vt = VT_BOOL;
					    vtbPrompt.boolVal = false;
					    m_pIReport->PrintOut( vtbPrompt );
                    }
                    //Mintao ++
                    //Bug 643
					// Indicate success.
					bResult = true;
				}
			}

			return( bResult );

        }

        bool CrystalReportsHelper::setupincidentId(std::string incidentid)
        {
			bool bResult = false;

			IParameterFieldDefinitions* pIParameterFieldDefinitions = NULL;
			HRESULT hr = m_pIReport->get_ParameterFields( &pIParameterFieldDefinitions );
			if (SUCCEEDED( hr ))
			{
				IParameterFieldDefinition* pIParameterFieldDefinition = NULL;
                CComBSTR ccbstrLabel = "ININCI_ID";

				CComVariant vtSubReport = "";
				hr = pIParameterFieldDefinitions->GetItemByName( ccbstrLabel, vtSubReport, &pIParameterFieldDefinition );
				if (SUCCEEDED( hr ))
				{
                    CComVariant vtValue;
                    variant_t vt(vtValue);
                    vt = incidentid.c_str();
                    pIParameterFieldDefinition->SetCurrentValue(vt.Detach());
                    vt.Clear();
				}
				else
				{
					// Error.
				}
			}
			else
			{
				// Error.
			}

			return( bResult );
        }
        bool CrystalReportsHelper::printReport(std::vector<std::string>& parameterValuePairs)
		{
			bool bResult = false;

			try
			{
				// Ensure that we have gain access to the crystal reports COM objects.
				if ((m_pIApplication ) && (m_pIReport ))
				{
					// Open the report template.
					//if (openReport() == true )
					//{
						// And print.
						setupParameters(parameterValuePairs);
						m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );

						m_pIReport->put_DisplayProgressDialog( false );

						VARIANT vtbPrompt;
						vtbPrompt.vt = VT_BOOL;
						// If print function is called by ReportManager GUI,
						// then set vtbPrompt.boolVal to true, so that the 
						// print Dialog can display correctly.
						if ( true == getIsPrintFromGUI() )
						{
							vtbPrompt.boolVal = true;
						} 
						else
						{
							vtbPrompt.boolVal = m_bDisplayPrompt;
						}

						m_pIReport->PrintOut( vtbPrompt );

						// Log this event.
						CString cstrLogMsg = "Report ";
						cstrLogMsg += getReportName();
						cstrLogMsg += " was printed.";

						#ifdef TRANSACTIVE_BUILD
						// Disabled to address the following TransActive defect.
						// Defect #2868 - Report Manager: Running Report via the command line/Windows Scheduler.
						// The following line has been temporarily commented out due to the 
						// fact that an exception thrown when called. The exception is thrown
						// because the app is run as a console application and as such a
						// TransActive operator login has not been performed. Because a operator
						// login has not been performed, we don't have a session ID and without this
						// sending an audit message fails. When a fix is found, then this auditing
						// can be re-introduced.
						//sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
						#else
						m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
						#endif // TRANSACTIVE_BUILD

						// Indicate success.
						bResult = true;
					//}
				}
			}
			catch(...)
			{
				//
			}
			return( bResult );
		}


        bool CrystalReportsHelper::exportReport(std::vector<std::string>& parameterValuePairs)
		{
			bool bResult = false;

			// Ensure that we have gain access to the crystal reports COM objects.
			if ((m_pIApplication ) && (m_pIReport ))
			{
				// Open the report template.
				if (openReport() == true )
				{
					CComBSTR bstrFileName;
					if (m_bDisplayPrompt == false)
					{
						// Setup export options.	
						m_pIReport->get_ExportOptions( &m_pIExportOptions );
						m_pIExportOptions->put_DestinationType( crEDTDiskFile );
						m_pIExportOptions->put_FormatType( getValueForFormat() );
						bstrFileName = getExportFileName();	
						m_pIExportOptions->put_DiskFileName( bstrFileName );
					}

					// And export.
                    setupParameters(parameterValuePairs);
					m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
					_mkdir( getHistoryReportPath() );
					m_pIReport->put_DisplayProgressDialog( false );
					VARIANT vtbPrompt;
					vtbPrompt.vt = VT_BOOL;
					vtbPrompt.boolVal = m_bDisplayPrompt;
					m_pIReport->Export( vtbPrompt );
					

					// Log this event.
					CString cstrLogMsg = "Report ";
					cstrLogMsg += getReportName();
					cstrLogMsg += " was exported";

                    #ifdef TRANSACTIVE_BUILD
                    // Disabled to address the following TransActive defect.
                    // Defect #2868 - Report Manager: Running Report via the command line/Windows Scheduler.
                    // The following line has been temporarily commented out due to the 
                    // fact that an exception thrown when called. The exception is thrown
                    // because the app is run as a console application and as such a
                    // TransActive operator login has not been performed. Because a operator
                    // login has not been performed, we don't have a session ID and without this
                    // sending an audit message fails. When a fix is found, then this auditing
                    // can be re-introduced.
					//sendAuditMessage( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #else
					m_pEventLogger->logInformation( cstrLogMsg.GetBuffer(cstrLogMsg.GetLength()) );
                    #endif // TRANSACTIVE_BUILD

					// Indicate success.
					bResult = true;
				}
			}

			return( bResult );
		}


        void CrystalReportsHelper::refreshReport()
		{
			if (m_crhViewingType == crhOnDemand)
			{
				m_pReportViewer->RefreshEx(false);//cl-18123--lkm
			//	m_pReportViewer->Refresh();
			}
			else
			{
				m_pHistoryReportViewer->Refresh();
			}
		}


        bool CrystalReportsHelper::stripOffExtension( char* p_szFullReportName )
		{
			bool bResult = false;

			CString cstrReportName = p_szFullReportName;
			CString cstrTemp = cstrReportName.Right(3);
			if (cstrTemp == REPORT_TEMPLATE_EXTENSION)
			{
				cstrTemp = cstrReportName.Left(cstrReportName.GetLength() - sizeof(REPORT_TEMPLATE_EXTENSION));
				int iBufferLength = strlen( p_szFullReportName );
				strcpy( p_szFullReportName, cstrTemp.GetBuffer( iBufferLength ) );
				bResult = true;
			}

			return( bResult );
		}


        enum CRExportFormatType CrystalReportsHelper::getValueForFormat()
		{
			enum CRExportFormatType iValue = crEFTNoFormat;

			if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_PDF ) == 0)
			{
				iValue = crEFTPortableDocFormat;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_WORD ) == 0)
			{
				iValue = crEFTWordForWindows;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_EXCEL ) == 0)
			{
				iValue = crEFTExcel97;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_RTF ) == 0)
			{
				iValue = crEFTRichText;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_HTML ) == 0)
			{
				iValue = crEFTHTML32Standard;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_XML ) == 0)
			{
				iValue = crEFTXML;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_TEXT ) == 0)
			{
				iValue = crEFTText;
			}
				
			return( iValue );
		}


        char* CrystalReportsHelper::getExtensionForFormat()
		{
			if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_PDF ) == 0)
			{
				m_cstrTemp = PDF_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_WORD ) == 0)
			{
				m_cstrTemp = WORD_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_EXCEL ) == 0)
			{
				m_cstrTemp = EXCEL_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_RTF ) == 0)
			{
				m_cstrTemp = RTF_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_HTML ) == 0)
			{
				m_cstrTemp = HTML_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_XML ) == 0)
			{
				m_cstrTemp = XML_EXTENSION;
			}
			else if (m_cstrFormat.CompareNoCase( REPORT_FORMAT_TEXT ) == 0)
			{
				m_cstrTemp = TEXT_EXTENSION;
			}
			else
			{
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_140030);
            }
				
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}

		char* CrystalReportsHelper::getExportFileName()
		{
			CString cstrTemp("");
			
            // Set the path
            if(m_ccbstrOutputPath.Length() == 0)
            {
				cstrTemp = getHistoryReportPath();
            }
            else
            {
                cstrTemp = m_ccbstrOutputPath;
                
                // Append a '\' if one hasn't been added
                if(cstrTemp.GetAt(cstrTemp.GetLength() - 1) != '\\')
                {
                    cstrTemp += "\\";
                }
            }

            // Add the timestamp if required
            if (m_bAddTimeStamp)
            {
                cstrTemp += generateTimestamp();
            }

            // Set the filename
            if(m_ccbstrOutputFilename.Length() == 0)
            {
                cstrTemp += getReportName();
            }
            else
            {
                cstrTemp += (CString) m_ccbstrOutputFilename;
            }
				
            // Set the extension
            cstrTemp += getExtensionForFormat();
				
            m_cstrTemp = cstrTemp;
			
            return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getReportFileName()
		{
			m_cstrTemp = m_ccbstrReportSource.m_str;
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getReportName()
		{
			char szTemp[500];

			// Strip of extension.
			m_cstrTemp = m_ccbstrReportTemplate;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			stripOffExtension( szTemp );

			// And get the string after the last backslash.
            char buf[500];
            ACE_OS::strcpy( buf, szTemp );
            ACE_Tokenizer tok( buf );
            tok.delimiter_replace( '\\', 0 );
            for( char* p = tok.next();
                 p;
                 p = tok.next() )
            {
                if ( NULL != p ) m_cstrTemp = p;
            }

			// Return the report name.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getHistoryReportName()
		{
			char szTemp[500];

			// Strip of extension.
			m_cstrTemp = m_ccbstrReportSource;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			stripOffExtension( szTemp );

			// And get the string after the last backslash.
			char *token;
			char seps[] = "\\";
			token = strtok( szTemp, seps );
			while( token != NULL )
			{
			   token = strtok( NULL, seps );
			   if (token) m_cstrTemp = token;
			}

			// Return the report name.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        char* CrystalReportsHelper::getReportPath()
		{
			char szTemp[500];

			// Strip of extension.
			m_cstrTemp = m_ccbstrReportTemplate;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			stripOffExtension( szTemp );

			// Count up the number of tokens.
			char *token;
			char seps[] = "\\";
			int iTokenCount = 0;
			token = strtok( szTemp, seps );
			while( token != NULL )
			{
				token = strtok( NULL, seps );
				if (token) iTokenCount++;
			}

			// Create a string that only contains the report's path.
			int i = 0;
			strcpy( szTemp, m_cstrTemp.GetBuffer(500) );
			token = strtok( szTemp, seps );
			m_cstrTemp = token;
			m_cstrTemp += "\\";
			while( token != NULL )
			{
				token = strtok( NULL, seps );
				if (i++ < iTokenCount-1) 
				{
					m_cstrTemp += token;
   					m_cstrTemp += "\\";
				}
			}

			// And return the report's path.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}
		
		//TD18040
		//xxs add
		CString CrystalReportsHelper::getHistoryReportPath( bool useCmdlineParam )
		{
			CString tempPath = m_cstrHistoryReportPath;
			if ( tempPath.IsEmpty() )
			{
				tempPath = getHistoryReportPath();
				//because the getReportPath will omit string "\\\\"
				//so i add two replace operation
				tempPath.Replace( "\\\\adm-occ-1", "\\\\sta-occ-1" );
				tempPath.Replace( "adm-occ-1", "\\\\sta-occ-1" );

				//use hard code
				//tempPath = "\\\\sta-occ-1\\C$\\transactive\\reports\\R-Report\\historical Report\\";
				tempPath = "C:\\transactive\\reports\\R-Report\\historical Report\\";
				LOG_GENERIC( SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "use template file name" );
			}
			ASSERT( tempPath.GetLength() > 0 );
			if (tempPath[tempPath.GetLength()-1] != '\\')
			{
				tempPath += "\\";
			}

			//store the path
			m_cstrHistoryReportPath = tempPath;

			return m_cstrHistoryReportPath;
		}
		//end xxs add 


        char* CrystalReportsHelper::getHistoryReportPath()
		{
			// Add the History subdirectory to the report path.
			//xxs add for test
			CString tempStr = "report file full name = ";
			tempStr += m_ccbstrReportTemplate.m_str;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, tempStr );
			//end xxs for test

			getReportPath();

			//xxs add for test
			tempStr = "report file path=";
			tempStr += m_cstrTemp;
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, tempStr );
			//end xxs for test

			m_cstrTemp += HISTORY_PATH_EXTENSION;
			m_cstrTemp += "\\";

			// And return the history report path.
			return( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()) );
		}


        bool CrystalReportsHelper::reportTemplateExists()
		{
			bool bResult = false;
			FILE* fin = NULL;

			m_cstrTemp = m_ccbstrReportSource.m_str;

            // Defect #3023 - Assertion when no file given in --file parameter
            // The following IF statement checks that the file name supplied
            // is not an empty string. If so, it informs the operator that 
            // a report template filename needs to be supplied.
            if (m_cstrTemp.Compare( "" ) != 0)
            {
                // The fopen function is used to ensure that the actual report template 
                // file exists. If not, it informs the operator that the report template
                // file could not be found.
			    fin = fopen( m_cstrTemp.GetBuffer(m_cstrTemp.GetLength()), "rb" );
			    if (fin != NULL)
			    {
				    bResult = true;
				    fclose( fin );
			    }
			    else
			    {
                    TA_Base_Bus::TransActiveMessage userMsg;
					//userMsg << m_cstrTemp;
					m_cstrTemp.Replace("\\\\","\\");
					userMsg << m_cstrTemp;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140029);
			    }
            }
            else
            {
                TA_Base_Bus::TransActiveMessage userMsg;
                UINT selectedButton = userMsg.showMsgBox(IDS_UE_140031);
            }

			return( bResult );
		}


        char* CrystalReportsHelper::generateTimestamp()
		{
			struct tm newtime; 
			time_t aclock;
            time(&aclock);    
            
            if(ACE_OS::localtime_r(&aclock, &newtime) != NULL)
            {
			    char szAMPM[10];
			    int iHour;
			    if (newtime.tm_hour > 12)
			    {
				    iHour = newtime.tm_hour - 12;
				    strcpy( szAMPM, "PM" );
			    }
			    else
			    {
				    iHour = newtime.tm_hour;
				    strcpy( szAMPM, "AM" );
			    }

			    char szTemp[200] = {0};
			    sprintf( szTemp, "%02d %s, %d %02d.%02d%s - ", newtime.tm_mday, 
									  					       tblMonths[newtime.tm_mon], 
														       newtime.tm_year+1900,
														       iHour,
														       newtime.tm_min,
														       szAMPM );
 			    m_cstrTimestamp = szTemp;

                // And return the timestamp string.
			    return( m_cstrTimestamp.GetBuffer(m_cstrTimestamp.GetLength()) );
            }

			return NULL;
		}


        void CrystalReportsHelper::setupGroupButton()
		{
			if (isReady() && m_pReportViewer)
			{
				long lNoOfGroups = 0;
				HRESULT hr = m_pIReport->get_NumberOfGroup( & lNoOfGroups );
				if (SUCCEEDED( hr ))
				{
					if (lNoOfGroups > 0)
					{
						m_pReportViewer->SetDisplayGroupTree( true );
					}
					else
					{
						m_pReportViewer->SetDisplayGroupTree( false );
					}
				}
				else
				{
					// Error.
				}

			}
			else
			{
				// Error.
			}
		}


        void CrystalReportsHelper::setupParameters(std::vector<std::string>& parameterValuePairs)
		{
            using TA_Base_Core::DebugUtil;

            if(parameterValuePairs.empty())
            {
                return;
            }

			bool bResult = false;
            bool bInvalidParams = false;

            // Report parameters will be specified in the following format:
            // --report-parameters=<parametername>:<value>,<parametername>:<value>, ...
            // eg. --report-parameters=p_planName:PlanA,p_group:OCC

            LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Parsing Report ParameterValue pairs");
            
            std::map<std::string, std::string> parameterValueMap;
            std::vector<std::string>::iterator iter = parameterValuePairs.begin();
            for(; iter != parameterValuePairs.end(); ++iter)
            {
                std::string parameterValuePair = *iter;
                int colonPos = parameterValuePair.find(":");
                if(colonPos != std::string::npos)
                {
                    std::string parameter = parameterValuePair.substr(0, colonPos);
                    std::string value = parameterValuePair.substr(colonPos+1, parameterValuePair.size()-(colonPos+1));
                    parameterValueMap[parameter] = value;
                }
                else
                {
                    // Error
                    LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "ReportParameters - %s. Missing colon.", parameterValuePair.c_str());
                    bInvalidParams = true;
                }
            }

            // Open the report template.
			if (openReport() == true )
			{
                // Add the valid parameters to Crystal Reports
                IParameterFieldDefinitions* pIParameterFieldDefinitions = NULL;
			    HRESULT hr = m_pIReport->get_ParameterFields( &pIParameterFieldDefinitions );
			    if (SUCCEEDED( hr ))
			    {
                    std::map<std::string,std::string>::iterator iter = parameterValueMap.begin();
                    for(; iter != parameterValueMap.end(); ++iter)
                    {
				        IParameterFieldDefinition* pIParameterFieldDefinition = NULL;
				        CComBSTR ccbstrLabel = iter->first.c_str();
				        CComVariant vtSubReport = "";
				        hr = pIParameterFieldDefinitions->GetItemByName( ccbstrLabel, vtSubReport, &pIParameterFieldDefinition );
				        if (SUCCEEDED( hr ))
				        {
                            CComVariant vtValue = iter->second.c_str();
					        hr = pIParameterFieldDefinition->SetCurrentValue(vtValue);
                            if (SUCCEEDED( hr ))
					        {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugInfo, "Added report parameter/value: %s/%s", iter->first.c_str(), iter->second.c_str());
					        }
                            else
                            {
                                LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid value: %s for parameter: %s", iter->second.c_str(), iter->first.c_str());
                                break;
                            }
					    }
				        else
				        {
                            // This parameter doesn't exist in this template
                            LOG_GENERIC(SourceInfo, DebugUtil::DebugError, "Invalid parameter: %s", iter->first.c_str());
                            break;
				        }
                    }

                    if(iter == parameterValueMap.end())
                    {
                        if(!bInvalidParams)
                        {
                            bResult = true;
                        }
                    }
                }
                else
                {
                    // Error
                }
            }

            // Have kept the following code here in case there is a need for passing in a time parameter

            /*
			ccbstrLabel = "p_interval";
			vtSubReport = "";
			hr = pIParameterFieldDefinitions->GetItemByName( ccbstrLabel, vtSubReport, &pIParameterFieldDefinition );
			if (SUCCEEDED( hr ))
			{
				COleDateTime dateTest;
				dateTest.SetDate(2003,10, 1);
				VARIANT vtStart;
				vtStart.vt = VT_DATE;
				vtStart.date = dateTest;
				dateTest.SetDate(2003,12,8);
				VARIANT vtEnd;
				vtEnd.vt = VT_DATE;
				vtEnd.date = dateTest;
				enum CRRangeInfo crRangeInfo;
				crRangeInfo = (enum CRRangeInfo)(crRangeIncludeLowerBound | crRangeIncludeUpperBound);
				hr = pIParameterFieldDefinition->AddCurrentRange(vtStart, vtEnd, crRangeInfo );
				if (SUCCEEDED( hr ))
				{
					bResult = true;
				}
				else
				{
					// Error.
				}
			}
            */
				
			if(!bResult)
            {
                TA_THROW(TA_Base_Core::ReportManagerException("Some or all Report Parameters are invalid"));
            }
		}


        bool CrystalReportsHelper::sendAuditMessage( char* p_szAuditMessage )
		{
			bool bResult = false;

            #ifdef TRANSACTIVE_BUILD

    	    // Obtain a pointer to the audit message sender instance.
            TA_Base_Core::AuditMessageSender* sender = TA_Base_Core::MessagePublicationManager::getInstance().getAuditMessageSender( TA_Base_Core::ReportsAudit::Context );
		    
		    // Send AuditMessage
	        TA_Base_Core::NameValuePair description1( "AuditMessage", p_szAuditMessage );
	        TA_Base_Core::DescriptionParameters description;
	        description.push_back( &description1);

            // TD4213: Retrieve the correct entity key to pass as a parameter
            unsigned long lEntity = strtoul(TA_Base_Core::RunParams::getInstance().get(RPARAM_ENTITYKEY).c_str(), NULL, 10);
		    
            sender->sendAuditMessage( TA_Base_Core::ReportsAudit::Reports, 
								      lEntity, 
								      description, 
								      "", 
                                      m_cstrSessionId.GetBuffer( m_cstrSessionId.GetLength() ),
								      "", 
								      "", 
								      "" );

		    delete sender;

            // And return success.
            bResult = true;

            #else

            // Just return success.
            bResult = true;

            #endif // TRANSACTIVE_BUILD

			return( bResult );
		}

		//get history report path
		void CrystalReportsHelper::setHistoryReportPathFromCmdLine( const char * pathName )
		{
			m_cstrHistoryReportPath = pathName;
		}

		void CrystalReportsHelper::showWindowCorrectly(int left, int top)
		{
			m_pHistoryReportViewer->MoveWindow(left, top, 0, 0);
			m_pReportViewer->MoveWindow(left, top, m_cx, m_cy);
		}

		void CrystalReportsHelper::reviewReportToShowCorrect(std::vector<std::string>& parameterValuePairs)
		{
			int nloop = 0;
			bool reviewAgain = false;
			do 
			{
				try
				{
					setupParameters(parameterValuePairs);
					m_pIReport->put_EnableParameterPrompting( m_bEnableParameterPrompting );
					try
					{
						m_pReportViewer->SetReportSource( m_pIReport );
					}
					catch(...)
					{
						m_pCCrutilDlg->repaintDlgAfterOperate();
					}

					m_pReportViewer->SetEnablePrintButton( true );
					m_pReportViewer->SetEnableExportButton( true );

					setupGroupButton();

					m_pReportViewer->ViewReport();
				}
				catch(...)
				{
					++nloop;
					reviewAgain = true;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "In reviewReportToShowCorrect, error happen again %d ...",nloop);
				}
			} while ( (reviewAgain == true) && (nloop < 1) );
		}

		void CrystalReportsHelper::setDoubleClickTree( bool value )
		{
			m_doubleClickTree = value;
		}

		bool CrystalReportsHelper::getDoubleClickTree()
		{
			return m_doubleClickTree;
		}

		bool CrystalReportsHelper::getIsPrintFromGUI()
		{
			if ( m_isPrintFromGUI == true )
			{
				m_isPrintFromGUI = false;

				return true;
			}
			return m_isPrintFromGUI;
		}

		void CrystalReportsHelper::setIsPrintFromGUI( bool isPrintFromGUI )
		{
			m_isPrintFromGUI = isPrintFromGUI;
		}


#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
