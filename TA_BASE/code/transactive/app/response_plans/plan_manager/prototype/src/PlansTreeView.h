/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlansTreeView.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlansTreeView.h : interface of the CPlansTreeView class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANSTREEVIEW_H__24FA27DE_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_)
#define AFX_PLANSTREEVIEW_H__24FA27DE_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PvData.h"
class CPlanViewerDoc;

class CPlansTreeView : public CTreeView
{
protected: // create from serialization only
    CPlansTreeView();
    DECLARE_DYNCREATE(CPlansTreeView)

// Attributes
public:
    CPlanViewerDoc* GetDocument();

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlansTreeView)
    public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
    protected:
    virtual void OnInitialUpdate(); // called first time after construct
    // }}AFX_VIRTUAL


// Implementation
public:
    virtual ~CPlansTreeView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif


protected:
    CImageList    m_treeImageList;
    UINT    m_startBusinessTimer;
    UINT    m_selectionTimer;
    UINT    m_currentView;
    CMenu  *m_catMenu;
    CMenu  *m_planMenu;

// Generated message map functions
protected:
      void PopulateTreeWithAllPlans();
    void PopulateTreeWithActivePlans();
    void PopulateTreeWithApprovedPlans();
    void PopulateTreeWithUnapprovedPlans();
    void ShowView(UINT viewTypeID);
    DWORD CreateItemData(int viewerType, unsigned long pkey);
    void SplitItemData(DWORD itemData, int &viewerType, unsigned long &pkey);
    bool UpdateView();
    void SetSplitterPos(int pos);

    // {{AFX_MSG(CPlansTreeView)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    afx_msg void OnTimer(UINT nIDEvent);
    afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnItemexpanded(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnDblclk(NMHDR* pNMHDR, LRESULT* pResult);
    afx_msg void OnEditCommand();
    afx_msg void OnPlanControl();
    // }}AFX_MSG
    // afx_msg LRESULT OnStartBusiness(WPARAM, LPARAM);
    afx_msg LRESULT OnResetMenusAndTbars(WPARAM, LPARAM);
    afx_msg LRESULT OnEditCategory(WPARAM, LPARAM);
    afx_msg LRESULT OnEditPlan(WPARAM, LPARAM);
    afx_msg LRESULT OnControlPlan(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in PlansTreeView.cpp
inline CPlanViewerDoc* CPlansTreeView::GetDocument()
   {
    FUNCTION_ENTRY( "GetDocument" );

    return (CPlanViewerDoc*)m_pDocument; }


#endif

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTREEVIEW_H__24FA27DE_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_)
