/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanCatForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANCATFORM_H__E39DC7E4_A802_11D5_B63F_0050BAB1D92B__INCLUDED_)
#define AFX_PLANCATFORM_H__E39DC7E4_A802_11D5_B63F_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanCatForm.h : header file
//


/////////////////////////////////////////////////////////////////////////////
// CPlanCatForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "PvCommonDefs.h"
#include "Notes.h"


class CPlanCatForm : public CFormView
{
protected:
    CPlanCatForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CPlanCatForm)

// Form Data
public:
    // {{AFX_DATA(CPlanCatForm)
    enum { IDD = IDD_PLANCAT_FORM };
    CNotes    m_notes;
    CStatic    m_categoryHeader;
    CEdit    m_categoryNameEdit;
    CEdit    m_categoryDescEdit;
    // }}AFX_DATA

// Attributes
public:

protected:
    CFont m_boldFont;
    bool m_readOnly;

// Operations
protected:
    void ResetMenusAndToolbars();

public:
    void PopulateData(const PV::Category *category);
    void EnableEditing();

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanCatForm)
    public:
    virtual void OnInitialUpdate();
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CPlanCatForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CPlanCatForm)
    afx_msg void OnChangeCategoryDescriptionEdit();
    afx_msg void OnChangeCategoryNameEdit();
    afx_msg void OnSize(UINT nType, int cx, int cy);
    afx_msg void OnCatExit();
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()

friend class CMainFrame;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANCATFORM_H__E39DC7E4_A802_11D5_B63F_0050BAB1D92B__INCLUDED_)
