/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tree_ctrl_multi_sel/TrCtrlVw.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_TRCTRLVW_H__67E6E225_6FD4_11D2_9AE7_0060084B904C__INCLUDED_)
#define AFX_TRCTRLVW_H__67E6E225_6FD4_11D2_9AE7_0060084B904C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// TrCtrlVw.h : header file
//
//This class shows how to use a derived CTreeCtrl class
// inside a derived CTreeView class.
//The method came from source supplied on CodeGuru,
// unfortunately I can't find the original article to
// credit the author.
//As such, I take no credit for this CTreeView class.

#include <afxcview.h>
#include "MltiTree.h"	//actual tree ctrl

/////////////////////////////////////////////////////////////////////////////

class CTC : public TA_Base_Bus::CMultiTree
{
#ifdef _DEBUG
	//skip assert because of dual-mapped CWnd
	virtual void AssertValid() const {};
	//if a non-CTC derived CTreeCtrl class is used
	// then make sure the AssertValid doesn't call
	// the base class
#endif
	//need access to m_pfnSuper
	friend class CTreeControlView;
};

/////////////////////////////////////////////////////////////////////////////
// CTreeControlView view

#define CTreeControlView_BASE		CTreeView

#ifdef IMPORT_EXTENSIONS
class AFX_CLASS_IMPORT CTreeControlView : public CTreeControlView_BASE
#else
class AFX_EXT_CLASS CTreeControlView : public CTreeControlView_BASE
#endif
{
protected:
	CTreeControlView();           // protected constructor used by dynamic creation
	DECLARE_DYNCREATE(CTreeControlView)

// Attributes
public:

// Operations
public:
	//over-scope base version
	CTC& GetTreeCtrl() const {return *m_pTree;};

protected:
	virtual void AllocEmbeddedTree();

protected:
	CTC		*m_pTree;

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CTreeControlView)
	public:
	virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	virtual BOOL OnChildNotify(UINT message, WPARAM wParam, LPARAM lParam, LRESULT* pLResult);
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

// Implementation
protected:
	virtual ~CTreeControlView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CTreeControlView)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_TRCTRLVW_H__67E6E225_6FD4_11D2_9AE7_0060084B904C__INCLUDED_)
