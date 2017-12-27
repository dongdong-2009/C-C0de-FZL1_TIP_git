// TA_MessageLibrary.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include <iostream>

#include "TA_MessageLibrary.h"
#include "MainFrm.h"
#include "GlobalFunctions.h"

#include "MessageGrid\MessageGridDoc.h"
#include "MessageGrid\MessageGridView.h"
#include "CmdLine\CmdLine.h"
#include "CmdLine\AppParameters.h"
#include "MessageExtraction\MsgExtractor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// MessageLibraryApp

BEGIN_MESSAGE_MAP(MessageLibraryApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	// Standard file based document commands
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, OnFileOpen)
	// Standard print setup command
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()


// MessageLibraryApp construction

MessageLibraryApp::MessageLibraryApp()
	: m_bInternalMode(false), m_bRunSilent(false), m_bNarrowOutput(false),
		m_lcidAppLang(MAKELANGID(LANG_ENGLISH, SUBLANG_ENGLISH_AUS))
{
	
}


// The one and only MessageLibraryApp object

MessageLibraryApp theApp;

// MessageLibraryApp initialization

BOOL MessageLibraryApp::InitInstance()
{
	CString sTemp;
	// Parse command line for standard shell commands, DDE, file open
	CCommandLineInfoEx cmdInfo;
	ParseCommandLine(cmdInfo);

	m_bInternalMode = cmdInfo.GetOption(PARAM_INTERNAL);
	m_bRunSilent = cmdInfo.GetOption(PARAM_RUN_SILENT);
	m_bNarrowOutput = cmdInfo.GetOption(PARAM_NARROW_OUTPUT);

	if(!cmdInfo.GetOption(PARAM_LIBRARY_PATH, m_sXMLSrcPath))
		InitialiseXMLSrcPath();

	if(!cmdInfo.GetOption(PARAM_RESOURCE_SRC_PATH, m_sResourceSrcPath))
		InitialiseResourceSrcPath();

	if(cmdInfo.GetOption(PARAM_OVERRIDE_LCID, sTemp) && sTemp != _T(""))
		m_lcidAppLang = _ttoi(sTemp);
	else
		m_lcidAppLang = GetUserDefaultLCID();

	InitCommonControls();

	CWinApp::InitInstance();

	// Initialize OLE libraries
	if (!AfxOleInit())
	{
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	EnumSystemLocales(EnumLocalesProc, LCID_INSTALLED);
	
	AfxEnableControlContainer();

	SetRegistryKey(_T("Ripple Systems\\TA_MessageLibrary"));

	CNewMenu::SetMenuDrawMode(CNewMenu::STYLE_XP);

	LoadStdProfileSettings(10);  // Load standard INI file options (including MRU)
	// Register the application's document templates.  Document templates
	//  serve as the connection between documents, frame windows and views
	CSingleDocTemplate* pDocTemplate;
	pDocTemplate = new CSingleDocTemplate(
		(m_bInternalMode) ? IDR_MAINFRAME : IDR_EXTERNAL_MAINFRAME,
		RUNTIME_CLASS(MessageGridDoc),
		RUNTIME_CLASS(MainFrame),       // main SDI frame window
		RUNTIME_CLASS(MessageGridView));
	AddDocTemplate(pDocTemplate);
	
	// Dispatch commands specified on the command line.  Will return FALSE if
	// app was launched with /RegServer, /Register, /Unregserver or /Unregister.
	if (!ProcessShellCommand(cmdInfo))
		return FALSE;
	// The one and only window has been initialized, so show and update it
	if(m_bRunSilent)
	{
		m_pMainWnd->ShowWindow(SW_HIDE);

		CString sTemp;
		MsgExtractor msgExtractor;
		MessageGridDoc* pDoc = NULL;
		if(!cmdInfo.GetOption(PARAM_OUTPUT_RC, msgExtractor.m_sTargetRCPath))
		{
			AfxMessageBox(IDS_APP_MISSING_OUTPUT_RC, MB_ICONERROR);
			return FALSE;
		}

		cmdInfo.GetOption(PARAM_OUTPUT_LANG, sTemp);
		if(sTemp != _T(""))
			msgExtractor.m_lcidLanguageToExport = _ttoi(sTemp);

		//pDoc = (MessageGridDoc*)pDocTemplate->OpenDocumentFile(_T("test"), FALSE);
		msgExtractor.m_bIncludeENA = cmdInfo.GetOption(PARAM_OUTPUT_ENA);
		msgExtractor.ExportMessagesToRC();
		return FALSE;
	}
	else
	{
		m_pMainWnd->ShowWindow(SW_SHOWMAXIMIZED);
		m_pMainWnd->UpdateWindow();
	}
	// call DragAcceptFiles only if there's a suffix
	//  In an SDI app, this should occur after ProcessShellCommand
	return TRUE;
}

void MessageLibraryApp::InitialiseXMLSrcPath()
{
	GetXMLDirectory(m_sXMLSrcPath);
	AppendPath(m_sXMLSrcPath, _T("TAMessageLibrary.xml"));
}

void MessageLibraryApp::InitialiseResourceSrcPath()
{
	GetXMLDirectory(m_sResourceSrcPath);
	AppendPath(m_sResourceSrcPath, _T("ResourceSource.xml"));
}

void MessageLibraryApp::GetXMLDirectory(CString& sOutput)
{
	TCHAR szFilePath[MAX_PATH + 1] = {'\0'}; //SL
	GetModuleFileName(NULL, szFilePath, MAX_PATH);
	
	sOutput = szFilePath;
	int iStart = sOutput.ReverseFind('\\');
	sOutput.Delete(iStart, sOutput.GetLength() - iStart);

	iStart = sOutput.ReverseFind('\\');
	sOutput.Delete(iStart, sOutput.GetLength() - iStart);
	AppendPath(sOutput, _T("XML")); //SP

}

void MessageLibraryApp::OnFileOpen()
{
	CFileDialog dlg(TRUE, _T(".xml"), NULL, OFN_FILEMUSTEXIST|OFN_SHAREAWARE, _T("XML Files (*.xml)|*.xml||"));
	if(dlg.DoModal() == IDOK)
	{
		m_sXMLSrcPath = dlg.GetPathName();
		OpenDocumentFile(m_sXMLSrcPath);
	}
}

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

// App command to run the dialog
void MessageLibraryApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}


// MessageLibraryApp message handlers

// the enumeration callback function
BOOL CALLBACK EnumLocalesProc(LPTSTR lpLocaleString)
{
	if (!lpLocaleString)
		return FALSE;
	
	LCID uiCurLocale = hexstring_to_uword(lpLocaleString);
	TCHAR szCurName[MAX_PATH + 1] = {'\0'};

	// Get the language name associated with this locale ID.
	if(!GetLocaleInfo(uiCurLocale, LOCALE_SLANGUAGE, szCurName, MAX_PATH))
		WindowsError(GetLastError());

	GetApp()->m_mapAvailableLocales.SetAt(uiCurLocale, szCurName);

	return TRUE;
}


CString MessageLibraryApp::getXmlSrcPath()
{
    return m_sXMLSrcPath;
}
