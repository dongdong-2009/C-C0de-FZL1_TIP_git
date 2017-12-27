/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/grid_control/InPlaceDateTime.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_INPLACEDATETIME_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
#define AFX_INPLACEDATETIME_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// InPlaceEdit.h : header file
//
// Written by Chris Maunder (chrismaunder@codeguru.com)
// Adapted by Jade Lee into a CDateTimeCtrl
// Copyright (c) 1998.
//
// The code contained in this file is based on the original
// CInPlaceDateTime from http://www.codeguru.com/listview/edit_subitems.shtml
//
// This code may be used in compiled form in any way you desire. This
// file may be redistributed unmodified by any means PROVIDING it is 
// not sold for profit without the authors written consent, and 
// providing that this notice and the authors name is included. If 
// the source code in  this file is used in any commercial application 
// then acknowledgement must be made to the author of this file 
// (in whatever form you wish).
//
// This file is provided "as is" with no expressed or implied warranty.
// The author accepts no liability for any damage/loss of business that
// this product may cause.
//
// Expect bugs!
// 
// Please use and enjoy. Please let me know of any bugs/mods/improvements 
// that you have found/implemented and I will fix/incorporate them into this
// file. 
//
/////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////
// CInPlaceDateTime window
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CInPlaceDateTime : public CDateTimeCtrl
#else
class AFX_EXT_CLASS CInPlaceDateTime : public CDateTimeCtrl
#endif
{
// Construction
public:
    CInPlaceDateTime(CWnd* pParent, CRect& rect, DWORD dwStyle, const char* dtFormat, UINT nID,
                 int nRow, int nColumn, time_t initTimeUTC, UINT nFirstChar);

// Attributes
public:
 
// Operations
public:
     void EndEdit();
 
// Overrides
     // ClassWizard generated virtual function overrides
     //{{AFX_VIRTUAL(CInPlaceDateTime)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void PostNcDestroy();
	//}}AFX_VIRTUAL
 
// Implementation
public:
     virtual ~CInPlaceDateTime();
 
// Generated message map functions
protected:
    //{{AFX_MSG(CInPlaceDateTime)
    afx_msg void OnKillFocus(CWnd* pNewWnd);
    afx_msg void OnChar(UINT nChar, UINT nRepCnt, UINT nFlags);
    afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg UINT OnGetDlgCode();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

private:
    int     m_nRow;
    int     m_nColumn;
    time_t  m_initTimeUTC;
    UINT    m_nLastChar;
    BOOL    m_bExitOnArrows;
    CRect   m_Rect;
};
 
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_INPLACEDATETIME_H__ECD42822_16DF_11D1_992F_895E185F9C72__INCLUDED_)
