/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tab_control_with_disable/TabCtrlEx.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This tab control class could be used within the property sheet to disable tab controls,
  * which is not a functionality provided by the TabCtrlWithDisableEx.  
  *  
  * Obtained from http://www.codeguru.com/Cpp/controls/propertysheet/article.php/c549/
  *
  */

/////////////////////////////////////////////////////////////////////////////
// TabCtrlEx.h : header file
//
// Extended Tab Control
// Copyright (C) 1998 RedCreek Communications
// All rights reserved.
//
// Written by Kevin Lussier (klussier@redcreek.com)
// Version 1.02
//
// Distribute freely, except: don't remove my name from the source or
// documentation (don't take credit for my work), mark your changes (don't
// get me blamed for your possible bugs), don't alter or remove this
// notice.
// No warrantee of any kind, express or implied, is included with this
// software; use at your own risk, responsibility for damages (if any) to
// anyone resulting from the use of this software rests entirely with the
// user.
//
// Send bug reports, bug fixes, enhancements, requests, flames, etc. to
//    klussier@redcreek.com
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_TABCTRLEX_H__51EAA32E_9330_11D1_8B30_0000B44160C7__INCLUDED_)
#define AFX_TABCTRLEX_H__51EAA32E_9330_11D1_8B30_0000B44160C7__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CTabCtrlEx window
namespace TA_Base_Bus
{
    #ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) CTabCtrlEx : public CTabCtrl
    #else
    class AFX_EXT_CLASS CTabCtrlEx : public CTabCtrl
    #endif
    {
    // Construction
    public:
	    CTabCtrlEx();

    // Attributes
    public:

    // Operations
    public:
	    BOOL	Install( CTabCtrl *pTabCtrl );
	    BOOL	Install( CPropertySheet *pPropSheet );
	    BOOL	EnableItem( int nItem, BOOL bEnable = TRUE );
	    BOOL	IsItemEnabled( int nItem, int *disabledIdx = NULL );

	    int		SetItemImage( int nItem, int nResBmp, int nResMsk = -1 );
	    int		SetItemImage( int nItem, HICON hIcon );

	    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

    protected:
	    int		FindNextEnabled( int nStart, BOOL bNext );
	    void	SetCurrentTab( int nItem );
	    BOOL	SendKillActive( int nItem = -1 );

    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CTabCtrlEx)
	    protected:
	    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
	    virtual ~CTabCtrlEx();

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CTabCtrlEx)
	    afx_msg void OnSelchanging(NMHDR* pNMHDR, LRESULT* pResult);
	    afx_msg void OnDestroy();
	    //}}AFX_MSG

	    DECLARE_MESSAGE_MAP()

    private:
	    CDWordArray	m_disabled;
	    NMHDR		m_nmhdr;
    };
};
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Developer Studio will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TABCTRLEX_H__51EAA32E_9330_11D1_8B30_0000B44160C7__INCLUDED_)
