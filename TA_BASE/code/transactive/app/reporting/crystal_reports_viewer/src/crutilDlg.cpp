/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/reporting/crystal_reports_viewer/src/crutilDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #8 $
  *
  * Last modification: $DateTime: 2012/01/13 10:05:23 $
  * Last modified by:  $Author: liwei.gong $
  *
  */
// crutilDlg.cpp : implementation file
//

#include "stdafx.h"
#include "crutil.h"
#include "crutilDlg.h"
#include "OpenFileDialog.h"
#include "AppSettings.hpp"
#include "HistoryReportsDlg.h"
#include "resource.h"
#include "globals.h"
#include "strings.h"
#include <afxpriv.h>
#include <io.h> // for access function

#ifdef TRANSACTIVE_BUILD
#include "bus/generic_gui/src/HelpLauncher.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "core/data_access_interface/src/OperatorAccessFactory.h"
#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus/about_box/src/TransActiveAboutBox.h"
#include "core/exceptions/src/ReportManagerException.h"

#include "core/data_access_interface/src/ReportDirectoryAccessFactory.h"
#include "core/data_access_interface/src/ReportFileAccessFactory.h"
#include "core/data_access_interface/src/IReportDirectory.h"
#include "core/data_access_interface/src/IReportFile.h"

#include "cots/VisualCpp/Include/Winspool.h"

const int DATABASENAME_INDEX = 1;
const int USERID_INDEX = 2;
const int PASSWORD_INDEX = 3;
const int STANDARD_MIN_HEIGHT = 205;
const int STANDARD_MIN_WIDTH = 500;

#endif // TRANSACTIVE_BUILD

// TD #2372 - 4669 Report Manager - Initiating main window for the very first time
// The code below fixes the above defect.
const static long DEFAULT_LEFT_VALUE = 50;
const static long DEFAULT_TOP_VALUE = 50;
const static long DEFAULT_RIGHT_VALUE = 700;
const static long DEFAULT_BOTTOM_VALUE = 500;
// End Defect Fix.

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#ifdef TRANSACTIVE_BUILD
using TA_Base_Bus::TransActiveDialog;
using namespace TA_Base_Core;

using TA_Base_Core::ReportDirectoryAccessFactory;
using TA_Base_Core::ReportFileAccessFactory;
using TA_Base_Core::IReportDirectory;
using TA_Base_Core::IReportFile;

namespace TA_Base_App
{
    namespace TA_Reporting
    {
#endif // TRANSACTIVE_BUILD

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
			virtual BOOL OnInitDialog();
			//}}AFX_MSG
			DECLARE_MESSAGE_MAP()
		};

		CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
		{
			//{{AFX_DATA_INIT(CAboutDlg)
			//}}AFX_DATA_INIT
		}

		BOOL CAboutDlg::OnInitDialog() 
		{
			CDialog::OnInitDialog();

			CString cstrAppVersion;
			cstrAppVersion = APPLICATION_NAME;
			cstrAppVersion += " V";
			cstrAppVersion += APPLICATION_VERSION;
			this->SetDlgItemText( IDC_STATIC_VERSION, cstrAppVersion );

			return TRUE;  // return TRUE unless you set the focus to a control
						  // EXCEPTION: OCX Property Pages should return FALSE
		}

		void CAboutDlg::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(CAboutDlg)
			//}}AFX_DATA_MAP
		}

		BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
			//{{AFX_MSG_MAP(CAboutDlg)
			//}}AFX_MSG_MAP
		END_MESSAGE_MAP()


		/////////////////////////////////////////////////////////////////////////////
		// CCrutilDlg dialog

		#ifdef TRANSACTIVE_BUILD
		CCrutilDlg::CCrutilDlg(TA_Base_Bus::IGUIAccess* genericGUICallback, CWnd* pParent /*=NULL*/)
		: TransActiveDialog(*genericGUICallback, CCrutilDlg::IDD, pParent), m_bShowMaxWindow( false )
		#else
		CCrutilDlg::CCrutilDlg(CWnd* pParent /*=NULL*/)
			: CDialog(CCrutilDlg::IDD, pParent), m_bShowMaxWindow( false )
		#endif // TRANSACTIVE_BUILD
		{
			//{{AFX_DATA_INIT(CCrutilDlg)
				// NOTE: the ClassWizard will add member initialization here
			//}}AFX_DATA_INIT
			// Note that LoadIcon does not require a subsequent DestroyIcon in Win32
			m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

			#ifdef TRANSACTIVE_BUILD

            // Create an instance of CrystalReportHelper and setup
            // its default operating parameters.
			m_pCrystalReports = new CrystalReportsHelper();

            // Set report viewing to OnDemand.
			m_pCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );


            // Defect #2843 - Report Manager not launched within the schematic area
            // Set the resizing properties
            TA_Base_Bus::ResizingProperties properties;
            properties.canMaximise = true;
            properties.maxHeight = -1;
            properties.maxWidth = -1;
            properties.minHeight = STANDARD_MIN_HEIGHT;
            properties.minWidth = STANDARD_MIN_WIDTH;
            setResizingProperties(properties);

			#else

			m_pCrystalReports = NULL;

			#endif // TRANSACTIVE_BUILD
		}

		CCrutilDlg::~CCrutilDlg()
		{
			if (m_pCrystalReports)
            {
                m_pCrystalReports->setViewerInstance( NULL );
				m_pCrystalReports->setBrowserInstance( NULL );
    			#ifdef TRANSACTIVE_BUILD
                delete m_pCrystalReports;
				m_pCrystalReports = NULL;
                #endif // TRANSACTIVE_BUILD
            }
		}

		void CCrutilDlg::DoDataExchange(CDataExchange* pDX)
		{
			CDialog::DoDataExchange(pDX);
			//{{AFX_DATA_MAP(CCrutilDlg)
			DDX_Control(pDX, IDC_CRVIEWER1, m_ReportViewer);
			DDX_Control(pDX, IDC_EXPLORER1, m_HistoryReportViewer);
			//}}AFX_DATA_MAP
			DDX_Control(pDX, IDC_REPORT_TREE, m_ReportTree);
		}
        
        #ifdef TRANSACTIVE_BUILD
        BEGIN_MESSAGE_MAP(CCrutilDlg, TransActiveDialog)
        #else
		BEGIN_MESSAGE_MAP(CCrutilDlg, CDialog)
        #endif
			//{{AFX_MSG_MAP(CCrutilDlg)
			ON_WM_SYSCOMMAND()
			ON_WM_PAINT()
			ON_WM_QUERYDRAGICON()
			ON_WM_SIZE()
			ON_COMMAND(IDM_FILE_EXIT, OnFileExit)
			ON_COMMAND(ID_HELP_ABOUT, OnHelpAbout)
			ON_COMMAND(IDM_FILE_OPEN, OnFileOpen)
			ON_WM_DESTROY()
			ON_COMMAND(ID_VIEW_HISTORY, OnViewHistory)
			ON_COMMAND(ID_VIEW_REFRESH, OnViewRefresh)
	ON_WM_CLOSE()
	ON_UPDATE_COMMAND_UI(ID_VIEW_HISTORY, OnUpdateViewHistory)
	//}}AFX_MSG_MAP
			ON_MESSAGE(WM_KICKIDLE, OnKickIdle)
			ON_NOTIFY(NM_DBLCLK, IDC_REPORT_TREE, &CCrutilDlg::OnNMDblclkTreeSel)
		END_MESSAGE_MAP()

        #ifdef TRANSACTIVE_BUILD
        BEGIN_EVENTSINK_MAP(CCrutilDlg, TransActiveDialog)
        #else
		BEGIN_EVENTSINK_MAP(CCrutilDlg, CDialog)
        #endif
            //{{AFX_EVENTSINK_MAP(CCrutilDlg)
	        ON_EVENT(CCrutilDlg, IDC_CRVIEWER1, 409 /* PrintButtonClicked */, OnPrintButtonClicked, VTS_PBOOL)
			ON_EVENT(CCrutilDlg, IDC_CRVIEWER1, 408 /* RefreshButtonClicked */, OnRefreshButtonClicked, VTS_PBOOL)////cl-18123--lkm
			ON_EVENT(CCrutilDlg, IDC_CRVIEWER1, 424, OnReportSourceErrorCrviewer1, VTS_BSTR VTS_I4 VTS_PBOOL)
	        //}}AFX_EVENTSINK_MAP
        END_EVENTSINK_MAP()

        /////////////////////////////////////////////////////////////////////////////
		// CCrutilDlg message handlers

		void CCrutilDlg::init()
		{
            // Create an instance of the Command Line Parser object.
            CommandLineParser objArgv;
			
            // If the report name parameter exists, then set the report name.
            if (objArgv.parameterExists( REPORTNAME_LABEL ))
			{
                std::vector<std::string> value = objArgv.getParameterValue( REPORTNAME_LABEL );
				m_cstrReportName = value[0].c_str();
				m_pCrystalReports->setReportName( m_cstrReportName.GetBuffer( m_cstrReportName.GetLength() ) );
            }
			
            // Set to fullscreen if the parameter has been supplied.
            if (objArgv.parameterExists( FULLSCREEN_LABEL )) m_bShowMaxWindow = true;
			
			//TD18040 
			//xxs add
			//get the historical report path
			if ( objArgv.parameterExists( HISTORY_REPORT_PATH_LABEL ) )
			{
                std::vector<std::string> value = objArgv.getParameterValue( HISTORY_REPORT_PATH_LABEL );
                m_pCrystalReports->setHistoryReportPathFromCmdLine( value[0].c_str());
			}
			//end xxs add
			
            // Enable parameter prompting if the parameter has been supplied.
			if (objArgv.parameterExists( NOPARAMPROMPTING_LABEL )) m_pCrystalReports->enableParameterPrompting( false );
			
            // Configure the Database connection parameters if the parameter 
            // has been supplied.
            if (objArgv.parameterExists( RPARAM_DBCONNECTION )) 
            {
                std::vector<std::string> value = objArgv.getParameterValue( RPARAM_DBCONNECTION );
                std::string connectionString = value[0];
                std::vector<std::string> dbConnData;
                SimpleDb::ParseConnectStr(connectionString, dbConnData);
                if (dbConnData.size() >=5)
                {
					m_pCrystalReports->setServerName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
					m_pCrystalReports->setDatabaseName( const_cast<char*>(dbConnData[DATABASENAME_INDEX].c_str()) );
					m_pCrystalReports->setUserID( const_cast<char*>(dbConnData[USERID_INDEX].c_str()) );
					m_pCrystalReports->setPassword( const_cast<char*>(dbConnData[PASSWORD_INDEX].c_str()) );
                }
            }

			// Setup Report Viewing.
			if (m_pCrystalReports)
			{
				m_pCrystalReports->setViewerInstance( &m_ReportViewer );
				m_pCrystalReports->setBrowserInstance( &m_HistoryReportViewer );
				
#ifdef TRANSACTIVE_BUILD
                
                // Retrieve current Session ID and Operator Name and pass it
                // to the Crystal Reports Helper Class.
                std::string sessionId = TA_Base_Core::RunParams::getInstance().get(RPARAM_SESSIONID).c_str();
                std::string operatorName = "";
                if (sessionId != "")
                {
                    TA_Base_Core::IOperator* op = TA_Base_Core::OperatorAccessFactory::
						getInstance().getOperatorFromSession(sessionId, false);
                    operatorName = op->getName();
                    delete op;
					
                    m_pCrystalReports->setSessionID( (char*)(sessionId.c_str()) );
                    m_pCrystalReports->setOperatorName( (char*)(operatorName.c_str()) );
                }
				
#endif // TRANSACTIVE_BUILD
				
                // Create an instance of the Command Line Parser object and retrieve the report parameters
                CommandLineParser objArgv;
                std::vector<std::string> reportParameters;
                if(objArgv.parameterExists( REPORTPARAMETER_LABEL ))
                {
                    reportParameters = objArgv.getParameterValue( REPORTPARAMETER_LABEL );
                }
				
                // And view the report.
                try
                {
                    if (m_pCrystalReports->viewReport(reportParameters) == false)
					{
						// Error.
					}
                }
                catch(const TA_Base_Core::ReportManagerException& ex)
                {
                    LOG_EXCEPTION_CATCH(SourceInfo, "ReportManagerException", ex.what());
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UE_140036);
                }
				m_ReportTree.EnableWindow(TRUE);
			}
			m_pCrystalReports->showWindowCorrectly(m_treeListBorder,0);
			
			// Restore the previous Windows size.
			restoreWindowSize();
			{
				int cx = m_pCrystalReports->getWindowWidth()*4/3;
				int cy = m_pCrystalReports->getWindowHeight();
				int m_treeListBorder = cx / 4;

				if (m_pCrystalReports)
				{
					/*m_pCrystalReports->setWindowWidth( cx - m_treeListBorder );
					m_pCrystalReports->setWindowHeight( cy );
					m_pCrystalReports->setTreeBorder( m_treeListBorder );*/

				if (m_pCrystalReports->getViewingType() == CrystalReportsHelper::crhOnDemand)
				{
					if (m_ReportViewer.m_hWnd)
					{
						//m_ReportViewer.MoveWindow(0, 0, cx, cy);	
						m_ReportTree.MoveWindow(0,0,m_treeListBorder,cy);
						m_ReportViewer.MoveWindow(m_treeListBorder,0,cx-m_treeListBorder,cy);
					}
				}
				else
				{
					if (m_ReportViewer.m_hWnd) 
					{
						//m_HistoryReportViewer.MoveWindow(0, 0, cx, cy);	
						m_ReportTree.MoveWindow(0,0,m_treeListBorder,cy);
						m_HistoryReportViewer.MoveWindow(m_treeListBorder,0,cx-m_treeListBorder,cy);
					}
				}
				}
			}
			
			m_bViewHistory = false;			//not allow to view history reports when no report open
			
#ifndef TRANSACTIVE_BUILD
			
			// And display the window.
			if (m_bShowMaxWindow) ::ShowWindow( this->m_hWnd, SW_SHOWMAXIMIZED );
			
#endif
		}

		BOOL CCrutilDlg::OnInitDialog()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Start OnInitDialog...");
            #ifdef TRANSACTIVE_BUILD

			m_bShowMaxWindow = TA_Base_Core::RunParams::getInstance().isSet("FullScreen");
			
            // Defect #2843 - Report Manager not launched within the schematic area
            TransActiveDialog::OnInitDialog();
 
			//TD18065
			//xxs add
			if ( m_bShowMaxWindow )
			{
				ShowWindow(SW_SHOWMAXIMIZED);   
			}
			//end xxs add

			
            #else
            
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
            
            #endif // TRANSACTIVE_BUILD

			
			// Set the icon for this dialog.  The framework does this automatically
			//  when the application's main window is not a dialog
			SetIcon(m_hIcon, TRUE);			// Set big icon
			SetIcon(m_hIcon, FALSE);		// Set small icon

			m_ReportTree.DeleteAllItems();
			m_reportDirVec.clear();
			{
				initializeReportDirectoryVec();
			}
			{
				initializeReportFileMap();
			}
			m_ImageList.Create(16,16,ILC_COLOR4,0,4);
			m_BitmapFolder.LoadBitmap(IDB_FOLDER);
			m_BitmapFile.LoadBitmap(IDB_FILE);

			m_ImageList.Add(&m_BitmapFolder,RGB(0,0,0));
			m_ImageList.Add(&m_BitmapFile,RGB(0,0,0));

			m_ReportTree.SetImageList(&m_ImageList,TVSIL_NORMAL);

			createReportTreeListFromDB();

			m_maxOrRestore = 1;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "End OnInitDialog...");
			
			return TRUE;  // return TRUE  unless you set the focus to a control
		}

		void CCrutilDlg::OnSysCommand(UINT nID, LPARAM lParam)
		{
			if ((nID & 0xFFF0) == IDM_ABOUTBOX)
			{
				displayAboutBox();
			}
			else
			{
				CDialog::OnSysCommand(nID, lParam);
			}
		}

		// If you add a minimize button to your dialog, you will need the code below
		//  to draw the icon.  For MFC applications using the document/view model,
		//  this is automatically done for you by the framework.

		void CCrutilDlg::OnPaint() 
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
		HCURSOR CCrutilDlg::OnQueryDragIcon()
		{
			return (HCURSOR) m_hIcon;
		}

		void CCrutilDlg::OnSize(UINT nType, int cx, int cy) 
		{
			CDialog::OnSize(nType, cx, cy);

			 if(nType == SIZE_MAXHIDE || nType == SIZE_MAXSHOW) 
				 return;

			int m_treeListBorder = cx / 4;

			if (m_pCrystalReports)
			{
				m_pCrystalReports->setWindowWidth( cx - m_treeListBorder );
				m_pCrystalReports->setWindowHeight( cy );
				m_pCrystalReports->setTreeBorder( m_treeListBorder );

				if (m_pCrystalReports->getViewingType() == CrystalReportsHelper::crhOnDemand)
				{
					if (m_ReportViewer.m_hWnd)
					{
						//m_ReportViewer.MoveWindow(0, 0, cx, cy);	
						m_ReportTree.MoveWindow(0,0,m_treeListBorder,cy);
						m_ReportViewer.MoveWindow(m_treeListBorder,0,cx-m_treeListBorder,cy);
					}
				}
				else
				{
					if (m_ReportViewer.m_hWnd) 
					{
						//m_HistoryReportViewer.MoveWindow(0, 0, cx, cy);	
						m_ReportTree.MoveWindow(0,0,m_treeListBorder,cy);
						m_HistoryReportViewer.MoveWindow(m_treeListBorder,0,cx-m_treeListBorder,cy);
					}
				}
			}
		}

        void CCrutilDlg::OnPrintButtonClicked(BOOL FAR* UseDefault) 
        {
            // Crystal Reports only prints to the default printer. There isn't a way to set the desired printer. 
            // The only way is to set the default printer. So here we retrieve the current default printer, then
            // launch the standard print dialog so the user can select a printer. This printer is set as the default
            // printer and once printing is complete, the orginal default printer will be reset as the default.

			// This OnPrintButtonClicked can only be called by ReportManager GUI, so set the m_isPrintFromGUI
			// in CrystalReportsHelper to true.
			m_pCrystalReports->setIsPrintFromGUI(true);

            DWORD lpPrinterStringSize = 200;
            CString lpOriginalPrinter;
            ::GetDefaultPrinter(lpOriginalPrinter.GetBuffer(lpPrinterStringSize), &lpPrinterStringSize);

	        CPrintDialog dlg(false,  PD_ALLPAGES | PD_USEDEVMODECOPIES | PD_NOPAGENUMS | PD_HIDEPRINTTOFILE | PD_NOSELECTION, this);
            if(dlg.DoModal() == IDOK)
            {
                LPDEVNAMES lpDevNames;
                LPTSTR lpszDeviceName;

                lpDevNames = (LPDEVNAMES) GlobalLock(dlg.m_pd.hDevNames);
                lpszDeviceName = (LPTSTR )lpDevNames + lpDevNames->wDeviceOffset;
                
                // Set the default printer
                ::SetDefaultPrinter(lpszDeviceName);
                
                // No parameters to pass in - pass in empty vector
                std::vector<std::string> parameterValuePairs;
                m_pCrystalReports->printReport(parameterValuePairs);

                // Reset original default printer
                ::SetDefaultPrinter(lpOriginalPrinter);
            }
                
			repaintDlgAfterOperate();
            // Setting this parameter to false will prevent the default Crystal Reports print dialog from
            // being launched
            *UseDefault = false;
	    }

        void CCrutilDlg::OnFileExit() 
		{	
			DestroyWindow();
		}

		void CCrutilDlg::OnHelpAbout() 
		{
			displayAboutBox();
        }

		void CCrutilDlg::OnViewHistory() 
		{
			if (m_pCrystalReports)
			{
				HistoryReportsDlg selectHistory;
				selectHistory.m_cstrDirectory = m_pCrystalReports->getHistoryReportPath( true );
				selectHistory.m_cstrFileSpec = "*";
				selectHistory.m_cstrFileSpec += m_pCrystalReports->getReportName();
				selectHistory.m_cstrFileSpec += ".*";

				//TD18040 
				//xxs add
				CString fileName_log = "dir=";
				fileName_log += selectHistory.m_cstrDirectory;
				fileName_log += " fileName=";
				fileName_log += selectHistory.m_cstrFileSpec;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, fileName_log );
				//end xxs add

                if (selectHistory.filesExist())
				{
					int nResponse = selectHistory.DoModal();
					if (nResponse == IDOK)
					{
						CString cstrTemp;
						//cstrTemp = m_pCrystalReports->getHistoryReportPath(  );
						cstrTemp = selectHistory.m_cstrDirectory;
						cstrTemp += selectHistory.m_cstrSelection;
						m_pCrystalReports->setReportName( cstrTemp.GetBuffer( cstrTemp.GetLength() ) );
						m_pCrystalReports->setViewingType( CrystalReportsHelper::crhHistorical );
						
                        // No parameters to pass in - pass in empty vector
                        std::vector<std::string> parameterValuePairs;
                        m_pCrystalReports->viewReport(parameterValuePairs);

						repaintDlgAfterOperate();
					}
					else if (nResponse == IDCANCEL)
					{
					}
				}
				else
				{
                    TA_Base_Bus::TransActiveMessage userMsg;
                    UINT selectedButton = userMsg.showMsgBox(IDS_UI_140015);
                }
			}
		}
		//cl-18123--lkm
		void CCrutilDlg::OnRefreshButtonClicked(BOOL FAR* UseDefault)
		{
		// Setting this parameter to false will prevent the default Crystal Reports print dialog from
        // being launched
		* UseDefault = false;
		OnViewRefresh();

		}
		//cl-18123--lkm
		void CCrutilDlg::OnViewRefresh() 
		{
			if (m_pCrystalReports) m_pCrystalReports->refreshReport();
		}

		void CCrutilDlg::OnFileOpen() 
		{

			if (m_pCrystalReports)
			{
				OpenFileDialog objOpenFile( TRUE, NULL, "", 0, "Report Templates (*.rpt)|*.rpt||", 0);
				objOpenFile.m_ofn.Flags = OFN_HIDEREADONLY | OFN_ENABLEHOOK | OFN_EXPLORER;
				// TD18033
				//HINSTANCE hInst = ::ShellExecute(this->m_hWnd, "explore", "\\\\adm-occ-1\\C$\\transactive\\reports\\R-Report", NULL, NULL, SW_HIDE);
				HINSTANCE hInst = ::ShellExecute(this->m_hWnd, "explore", "C:\\transactive\\reports\\R-Report", NULL, NULL, SW_HIDE);


				if(reinterpret_cast<int>(hInst) <= 32)
				{
					objOpenFile.m_ofn.lpstrInitialDir="C:\\transactive\\reports\\R-Report";

					// popup message dialog, "Unable to connect to ADMIN MFT."
					TA_Base_Bus::TransActiveMessage userMsg;
					UINT selectedButton = userMsg.showMsgBox(IDS_UW_140002);
				}
				else
				{
					objOpenFile.m_ofn.lpstrInitialDir="C:\\transactive\\reports\\R-Report";
				}
				// TD18033
				
				if (objOpenFile.DoModal() == IDOK)
				{
					GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_GRAYED);
					m_ReportTree.EnableWindow(FALSE);

					m_pCrystalReports->setReportName( objOpenFile.GetPathName().GetBuffer(100) );
					m_pCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );
					m_pCrystalReports->setDialogInstance( this );

                    // No parameters to pass in - pass in empty vector
                    std::vector<std::string> parameterValuePairs;
					if (m_pCrystalReports->viewReport(parameterValuePairs) == false)
					{
                        TA_Base_Bus::TransActiveMessage userMsg;
                        UINT selectedButton = userMsg.showMsgBox(IDS_UE_140033);
                    }
					else
					{
						m_bViewHistory = true;		//allow to view history
  					}

					repaintDlgAfterOperate();
				}
				GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_ENABLED);
				m_ReportTree.EnableWindow(TRUE);
			}
		}

		void CCrutilDlg::saveWindowSize()
		{
			RECT rcSize;
			GetWindowRect( &rcSize );
			AppSettings obj( COMPANY_NAME, APPLICATION_NAME );
			obj.setNumberValue( REG_WINDOWTOP_LABEL, rcSize.top );		
			obj.setNumberValue( REG_WINDOWLEFT_LABEL, rcSize.left );		
			obj.setNumberValue( REG_WINDOWBOTTOM_LABEL, rcSize.bottom - rcSize.top );		
			obj.setNumberValue( REG_WINDOWRIGHT_LABEL, rcSize.right - rcSize.left );		
		}

		void CCrutilDlg::restoreWindowSize()
		{
            // Disabled to address the following TransActive defect.
            // Defect #2843 - Report Manager not launched within the schematic area
			//AppSettings obj( COMPANY_NAME, APPLICATION_NAME );
			//SetWindowPos( &wndTop, 
			//			  obj.getNumberValue( REG_WINDOWLEFT_LABEL),
			//			  obj.getNumberValue( REG_WINDOWTOP_LABEL),
			//			  obj.getNumberValue( REG_WINDOWRIGHT_LABEL),
			//			  obj.getNumberValue( REG_WINDOWBOTTOM_LABEL),
			//			  SWP_SHOWWINDOW );
		}

		void CCrutilDlg::OnDestroy() 
		{
			CDialog::OnDestroy();
			saveWindowSize();
		}

		void CCrutilDlg::OnClose() 
		{
			try
			{
			#ifdef TRANSACTIVE_BUILD
				TransActiveDialog::OnClose();
			#else
				CDialog::OnClose();
			#endif // TRANSACTIVE_BUILD
				DestroyWindow();
			}
			catch(...)
			{
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"Something incorrect happened when exit ReportManager...");
			}
		}

		void CCrutilDlg::displayAboutBox()
		{
			#ifdef TRANSACTIVE_BUILD

        	TA_Base_Bus::HelpLauncher::getInstance().displayAboutBox();

			#else

			CAboutDlg dlgAbout;
			dlgAbout.DoModal();

			#endif // TRANSACTIVE_BUILD

		}

		void CCrutilDlg::OnUpdateViewHistory(CCmdUI* pCmdUI)
		{
			pCmdUI->Enable(m_bViewHistory);
			pCmdUI->SetCheck(false);
		}

		LRESULT CCrutilDlg::OnKickIdle(WPARAM, LPARAM)
		{
			CMenu* pMainMenu = GetMenu();
			CCmdUI cmdUI;
			for (UINT n = 0; n < pMainMenu->GetMenuItemCount(); ++n)
			{
				CMenu* pSubMenu = pMainMenu->GetSubMenu(n);
				cmdUI.m_nIndexMax = pSubMenu->GetMenuItemCount();
				for (UINT i = 0; i < cmdUI.m_nIndexMax;++i)
				{
					cmdUI.m_nIndex = i;
					cmdUI.m_nID = pSubMenu->GetMenuItemID(i);
					cmdUI.m_pMenu = pSubMenu;
					cmdUI.DoUpdate(this, FALSE);
				}
			}
			return TRUE;
		}

		void CCrutilDlg::createReportTreeListFromDB()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start createReportTreeListFromDB...");

			HTREEITEM itemParent,itemInsert;
			int tempcount = 0;
			//(HTREEITEM)TVI_ROOT
			if ( !m_reportDirVec.empty() )
			{
				tempcount = 0;
				std::vector<REPORT_DIRECTORY>::iterator dirIter;
				REPORT_DIRECTORY reportDirTemp;

				for ( dirIter = m_reportDirVec.begin(); dirIter != m_reportDirVec.end(); ++dirIter )
				{
					++tempcount;
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In createReportTreeListFromDB, for-loop %d, m_dirId = %ld, m_dirName = %s, m_dirParentId = %ld...",
						tempcount,(*dirIter).report_directory_id,((*dirIter).report_directory_name).c_str(),(*dirIter).report_parent_dir_id);

					if ( (*dirIter).report_parent_dir_id == 0)
					{
						itemParent = m_ReportTree.InsertItem(((*dirIter).report_directory_name).c_str(),0,0,(HTREEITEM)TVI_ROOT,TVI_LAST);
						m_ReportTree.SetItemData(itemParent,(*dirIter).report_directory_id);
					}
					else if ( (*dirIter).report_parent_dir_id > 0 )
					{
						HTREEITEM itemDirTemp = findItemWithDirId(m_ReportTree,m_ReportTree.GetRootItem(),(*dirIter).report_parent_dir_id,TRUE);
						if ( itemDirTemp != NULL )
						{
							itemInsert = m_ReportTree.InsertItem((*dirIter).report_directory_name.c_str(),0,0,itemDirTemp,TVI_LAST);
							m_ReportTree.SetItemData(itemInsert,(DWORD)((*dirIter).report_directory_id));
						}
					}
					else
					{
						TA_THROW(TA_Base_Core::ReportManagerException("Some error(s) happen when create Tree List Dir."));
					}
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In createReportTreeListFromDB, m_reportDirVec is empty...");
			}

			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			//        After create directory node for report tree, then create file node for it.                                //
			//////////////////////////////////////////////////////////////////////////////////////////////////////////////////
			
			if ( !m_reportFileMap.empty() )
			{
				std::map<unsigned long, REPORT_FILE>::iterator fileIter;
				tempcount = 0;

				for ( fileIter = m_reportFileMap.begin(); fileIter != m_reportFileMap.end(); ++fileIter )
				{
					HTREEITEM itemFileTemp = findItemWithDirId(m_ReportTree,m_ReportTree.GetRootItem(),fileIter->second.report_directory_id,TRUE);
					if ( itemFileTemp != NULL )
					{
						itemInsert = m_ReportTree.InsertItem((fileIter->second.report_file_name).c_str(),1,1,itemFileTemp,TVI_LAST);
						m_ReportTree.SetItemData(itemInsert,(DWORD)(fileIter->first));
					}
				}
			}
			else
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In createReportTreeListFromDB, m_reportFileMap is empty...");
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End createReportTreeListFromDB...");
		}


		HTREEITEM CCrutilDlg::findItemWithDirId(CTreeCtrl& treeCtrl,HTREEITEM item, int dirId,BOOL findChild)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start findItemWithDirId, dirId = %d...",dirId);

			HTREEITEM  hfind=NULL;

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In findItemWithDirId, step 1");
			// Empty Tree, return NULL
			if(item==NULL)
			{
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End findItemWithDirId, empty tree, return NULL...");
				return NULL; 
			}

			int nImage,nSelectedImage;

			// Traverse the tree and search the item
			while(item!=NULL)
			{ 
				m_ReportTree.GetItemImage(item,nImage,nSelectedImage);
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In findItemWithDirId, step 2");				
				
				int itemDirId = treeCtrl.GetItemData(item);

				// Current node is the required node
				if ( ( itemDirId == dirId ) && ( nImage == 0 ) )
				{
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End findItemWithDirId, found item in current node...");

					return item;
				}

				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In findItemWithDirId, step 3");
				
				// Search the child node of current node
				if(treeCtrl.ItemHasChildren(item)&&findChild == TRUE)
				{ 
					item=treeCtrl.GetChildItem(item);

					// Recursive call to search the nodes under this child node
					hfind=findItemWithDirId(treeCtrl,item,dirId,TRUE);

					if(hfind)
					{ 
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End findItemWithDirId, found item in child node...");

						return  hfind; 
					} 
					else
					{
						LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In findItemWithDirId, search child node's brother node...");

						// Search sibling node when item not found in the child node
						item =treeCtrl.GetNextSiblingItem(treeCtrl.GetParentItem(item));
					}
				}
				else
				{  
					LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In findItemWithDirId, search brother node...");

					// If no child node exist, search sibling node
					item =treeCtrl.GetNextSiblingItem(item);
				} 
			}
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End findItemWithDirId, normally end...");

			return hfind; 
		}

		void CCrutilDlg::repaintDlgAfterOperate()
		{
			/*
			CRect   rect   ; 
			this-> GetWindowRect(rect); 
			ScreenToClient(rect)   ; 
			this-> OnSize(SIZE_RESTORED,rect.Width(),rect.Height());
			*/

			//if ( m_maxOrRestore % 2 == 0)
			//{
			//	BOOL showNormal = this->ShowWindow(SW_SHOWNORMAL);
			//}
			//else
			//{
			//	BOOL showMax = this->ShowWindow(SW_SHOWMAXIMIZED);
			//}
			//m_ReportTree.ShowWindow(TRUE);
			//++m_maxOrRestore;


		}

		void CCrutilDlg::OnNMDblclkTreeSel(NMHDR *pNMHDR, LRESULT *pResult)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start OnNMDblclkTreeSel...");

			HTREEITEM itemChild = m_ReportTree.GetSelectedItem();

			int nImage,nSelectedImage;
			m_ReportTree.GetItemImage(itemChild,nImage,nSelectedImage);

			// User click Crystal Report file for nImage=1;
			// while Folder for nImage=0;
			if ( nImage == 1 )
			{
				GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_GRAYED);
				m_ReportTree.EnableWindow(FALSE);

				m_pCrystalReports->setDoubleClickTree(true);

				CString strFileFullPath = "";
				unsigned long fileId = m_ReportTree.GetItemData(itemChild);
				std::map<unsigned long,REPORT_FILE>::iterator filepos;
				filepos = m_reportFileMap.find(fileId);
				if ( filepos != m_reportFileMap.end() )
				{
					strFileFullPath = (filepos->second.report_file_physical_path).c_str();

					// use access function to check whether the file is exist, use the second 
					// parameter 0, which stands for F_OK. If file exist, return 0; else return -1.
					if ( access(strFileFullPath,0) < 0 )
					{
						TA_Base_Bus::TransActiveMessage userMsg;
						strFileFullPath.Replace("\\\\","\\");
						userMsg << strFileFullPath;
						UINT selectedButton = userMsg.showMsgBox(IDS_UE_140029);

						GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_ENABLED);
						m_ReportTree.EnableWindow(TRUE);

						return;
					}

					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"In OnNMDblclkTreeSel, fileId = %ld, strFileFullPath = %s",fileId,(filepos->second.report_file_name).c_str());
				}
				
				m_pCrystalReports->setReportName( strFileFullPath.GetBuffer(100) );
				m_pCrystalReports->setViewingType( CrystalReportsHelper::crhOnDemand );
				m_pCrystalReports->setDialogInstance( this );

				// No parameters to pass in - pass in empty vector
				std::vector<std::string> parameterValuePairs;
				if (m_pCrystalReports->viewReport(parameterValuePairs) == false)
				{
					//repaintDlgAfterOperate();
					//TA_Base_Bus::TransActiveMessage userMsg;
					//UINT selectedButton = userMsg.showMsgBox(IDS_UE_140033);
					LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugWarn,"There is a problem when attempting to open the report.");
				}
				else
				{
					//repaintDlgAfterOperate();
					m_bViewHistory = true;		//allow to view history
				}
				GetSystemMenu(FALSE)->EnableMenuItem(SC_CLOSE,MF_BYCOMMAND | MF_ENABLED);
				m_ReportTree.EnableWindow(TRUE);

				repaintDlgAfterOperate();
			}

			*pResult = 0;
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End OnNMDblclkTreeSel...");
		}

		void CCrutilDlg::initializeReportDirectoryVec()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start initializeReportDirectoryVec...");

			std::vector< IReportDirectory* > reportDirectorys(ReportDirectoryAccessFactory::getInstance().getAllReportDirectories());

			std::vector< IReportDirectory* >::iterator reportDirIter;			

			int tempcount = 0;

			for ( reportDirIter = reportDirectorys.begin(); reportDirIter != reportDirectorys.end(); ++reportDirIter )
			{
				REPORT_DIRECTORY reportDirStruct;

				reportDirStruct.report_directory_id = (*reportDirIter)->getReportDirectoryId();
				reportDirStruct.report_directory_name = (*reportDirIter)->getReportDirectoryName();
				reportDirStruct.report_parent_dir_id = (*reportDirIter)->getReportParentDirectoryId();

				++tempcount;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In initializeReportDirectoryVec, for-loop %d, m_dirId = %ld, m_dirName = %s, m_dirParentId = %ld...",
					tempcount,reportDirStruct.report_directory_id,(reportDirStruct.report_directory_name).c_str(),reportDirStruct.report_parent_dir_id);

				m_reportDirVec.push_back(reportDirStruct);
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End initializeReportDirectoryVec...");
		}

		void CCrutilDlg::initializeReportFileMap()
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "Start initializeReportFileMap...");

			std::vector< IReportFile* > reportFiles(ReportFileAccessFactory::getInstance().getAllReportFiles());

			std::vector< IReportFile* >::iterator reportFileIter;

			int tempcount = 0;

			for ( reportFileIter = reportFiles.begin(); reportFileIter != reportFiles.end(); ++reportFileIter )
			{
				REPORT_FILE reportFileStruct;

				unsigned long reportFileId = (*reportFileIter)->getReportFileId();

				reportFileStruct.report_file_id = reportFileId;
				reportFileStruct.report_file_name = (*reportFileIter)->getReportFileName();
				reportFileStruct.report_file_physical_path = (*reportFileIter)->getReportFilePhysicalPath();
				reportFileStruct.report_directory_id = (*reportFileIter)->getReportFileDirectoryId();

				++tempcount;
				LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "In initializeReportFileMap, for-loop %d, m_fileId = %ld, m_fileName = %s, m_filePhysicalPath = %s, m_fileDirId = %ld...",
					tempcount,reportFileStruct.report_file_id,(reportFileStruct.report_file_name).c_str(),(reportFileStruct.report_file_physical_path).c_str(),reportFileStruct.report_directory_id);


				m_reportFileMap.insert(std::pair<unsigned long,REPORT_FILE>(reportFileId,reportFileStruct));
			}

			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug, "End initializeReportFileMap...");
		}

		std::vector<REPORT_DIRECTORY> CCrutilDlg::getReportDirectoriesUseParentId(unsigned long parentId)
		{
			std::vector< IReportDirectory* > reportDirectorys(ReportDirectoryAccessFactory::getInstance().getAllReportDirsInSpecifiedParentDirId(parentId));

			std::vector< REPORT_DIRECTORY > reportDirVec;

			std::vector< IReportDirectory* >::iterator reportDirIter;
			for ( reportDirIter = reportDirectorys.begin(); reportDirIter != reportDirectorys.end(); ++reportDirIter)
			{
				REPORT_DIRECTORY reportDirItem;
				reportDirItem.report_directory_id = (*reportDirIter)->getReportDirectoryId();
				reportDirItem.report_directory_name = (*reportDirIter)->getReportDirectoryName();
				reportDirItem.report_parent_dir_id = (*reportDirIter)->getReportParentDirectoryId();

				reportDirVec.push_back(reportDirItem);
			}

			return reportDirVec;
		}

		std::vector<REPORT_FILE> CCrutilDlg::getReportFilesUseDirId(unsigned long dirId)
		{
			std::vector< IReportFile* > reportFiles(ReportFileAccessFactory::getInstance().getAllReportFilesInSpecifiedDirId(dirId));

			std::vector< REPORT_FILE > reportFileVec;

			std::vector< IReportFile* >::iterator reportFileIter;
			for ( reportFileIter = reportFiles.begin(); reportFileIter != reportFiles.end(); ++reportFileIter )
			{
				REPORT_FILE reportFileItem;
				reportFileItem.report_file_id = (*reportFileIter)->getReportFileId();
				reportFileItem.report_file_name = (*reportFileIter)->getReportFileName();
				reportFileItem.report_file_physical_path = (*reportFileIter)->getReportFilePhysicalPath();
				reportFileItem.report_directory_id = (*reportFileIter)->getReportFileDirectoryId();

				reportFileVec.push_back(reportFileItem);
			}

			return reportFileVec;
		}

		void CCrutilDlg::OnReportSourceErrorCrviewer1(LPCTSTR errorMsg, long errorCode, BOOL* UseDefault)
		{
			LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugInfo,"On OnReportSourceErrorCrviewer1, errorCode = %ld", errorCode);
			// TODO: Add your message handler code here
			if( errorCode == -2147189180 )
			{
				// Do nothing except write log
				*UseDefault = FALSE;
				LOG_GENERIC(SourceInfo,TA_Base_Core::DebugUtil::DebugDebug,"User cancel open report file...");
			}
		}

#ifdef TRANSACTIVE_BUILD
	} // TA_Reporting
} // TA_Base_App
#endif // TRANSACTIVE_BUILD
