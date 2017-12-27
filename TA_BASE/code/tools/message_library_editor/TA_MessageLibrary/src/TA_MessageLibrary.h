// TA_MessageLibrary.h : main header file for the TA_MessageLibrary application
//
#pragma once

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols


// MessageLibraryApp:
// See TA_MessageLibrary.cpp for the implementation of this class
//

BOOL CALLBACK EnumLocalesProc(LPTSTR lpLocaleString);

class MessageLibraryApp : public CWinApp
{
public:
	MessageLibraryApp();

	bool m_bInternalMode, m_bRunSilent, m_bNarrowOutput;
	CString m_sXMLSrcPath, m_sResourceSrcPath;
	LCID m_lcidAppLang;

	void InitialiseResourceSrcPath();
	void InitialiseXMLSrcPath();
	void GetXMLDirectory(CString& sOutput);
	
// Overrides
public:
	CString getXmlSrcPath();
	virtual BOOL InitInstance();

	CMap<LCID, LCID, CString, CString> m_mapAvailableLocales;

// Implementation
	afx_msg void OnAppAbout();
	afx_msg void OnFileOpen();

	DECLARE_MESSAGE_MAP()
};

extern MessageLibraryApp theApp;

inline MessageLibraryApp* GetApp() { return reinterpret_cast<MessageLibraryApp*>(AfxGetApp()); }

inline LCID GetAppLCID() { return GetApp()->m_lcidAppLang; }