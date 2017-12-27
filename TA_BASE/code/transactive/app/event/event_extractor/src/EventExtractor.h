/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_extractor/src/EventExtractor.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// EventExtractor.h : main header file for the EVENTEXTRACTOR application
//

#if !defined(AFX_EVENTEXTRACTOR_H__05FBE595_83B7_4F4B_A090_D99BA6DD9F42__INCLUDED_)
#define AFX_EVENTEXTRACTOR_H__05FBE595_83B7_4F4B_A090_D99BA6DD9F42__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"		// main symbols

/////////////////////////////////////////////////////////////////////////////
// EventExtractor:
// See EventExtractor.cpp for the implementation of this class
//

class EventExtractor : public CWinApp
{
public:
	EventExtractor();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(EventExtractor)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation

	//{{AFX_MSG(EventExtractor)
		// NOTE - the ClassWizard will add and remove member functions here.
		//    DO NOT EDIT what you see in these blocks of generated code !
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_EVENTEXTRACTOR_H__05FBE595_83B7_4F4B_A090_D99BA6DD9F42__INCLUDED_)
