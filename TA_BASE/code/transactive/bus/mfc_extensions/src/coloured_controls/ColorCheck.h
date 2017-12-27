/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/coloured_controls/ColorCheck.h $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
#if !defined(AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_)
#define AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ColorCheck.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CColorCheck window

// Make this work in a DLL
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CColorCheck : public CButton
#else
class AFX_EXT_CLASS CColorCheck : public CButton
#endif
{
DECLARE_DYNAMIC(CColorCheck)

// Construction
public:

	CColorCheck();
	virtual ~CColorCheck();
	
//	BOOL Attach(const UINT nID, CWnd* pParent);	
	virtual void SetCheck(int nCheck);
	virtual int GetCheck();
	COLORREF SetBkColor(COLORREF color);
	COLORREF SetArrowColor(COLORREF newColor);
	COLORREF SetMyTextColor(COLORREF txtColor);
// Implementation
public:
	BOOL checkFlag;
	UINT oldAction;
	UINT oldState;
	BOOL drawFocus;
	COLORREF newColor, newArrowColor, newTextColor;

	// Generated message map functions
protected:
	void DrawCheckCaption(CDC *pDC, CRect R, const char *Buf, COLORREF TextColor);
	virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItem);	//override the standard function (no WM_DRAWITEM !!!)
	COLORREF GetDisabledColor() { return disabled; }
	//{{AFX_MSG(CColorCheck)
	//}}AFX_MSG
private:
	COLORREF disabled;

	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_COLORCHECK_H__A897D857_09A3_11D6_8CAD_00500439B545__INCLUDED_)
