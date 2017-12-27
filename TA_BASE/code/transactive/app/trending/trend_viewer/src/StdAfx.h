/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/trending/trend_viewer/src/StdAfx.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// stdafx.h : include file for standard system include files,
//  or project specific include files that are used frequently, but
//      are changed infrequently
//

#if !defined(AFX_STDAFX_H__D0BB3C0B_825C_11D6_B243_0050BAB25370__INCLUDED_)
#define AFX_STDAFX_H__D0BB3C0B_825C_11D6_B243_0050BAB25370__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#if defined( WIN32 )
#pragma warning( disable : 4786 )
#endif   // defined( WIN32 ) 

#define VC_EXTRALEAN		// Exclude rarely-used stuff from Windows headers

#include <afxwin.h>         // MFC core and standard components
#include <afxext.h>         // MFC extensions
#include <afxdisp.h>        // MFC Automation classes
#include <afxdtctl.h>		// MFC support for Internet Explorer 4 Common Controls
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>			// MFC support for Windows Common Controls
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <comdef.h>         // For _bstr_t

// Application specific defines
#define WM_LOADING_COMPLETE WM_USER+0x500 //MFC does not use the range WM_USER to 0x7FFF (so it's safe for us to use)
#define WM_TRENDS_DISPLAY WM_LOADING_COMPLETE+0x500 
#define WM_TRENDS_COPY WM_TRENDS_DISPLAY+0x500 
#define WM_TRENDS_MODIFY WM_TRENDS_COPY+0x500
#define WM_PROCESS_ERROR_MSG WM_TRENDS_MODIFY+0x500
#define WM_LOAD_ITEMS WM_PROCESS_ERROR_MSG+0x500
#define WM_LARGE_RANGE WM_LOAD_ITEMS+0x500
#define WM_LAUNCH_TRENDS_DLG WM_LARGE_RANGE+0x500

#pragma warning(disable:4786)

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


// #ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_IA64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='ia64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
// #endif




#endif // !defined(AFX_STDAFX_H__D0BB3C0B_825C_11D6_B243_0050BAB25370__INCLUDED_)

