/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/BlankForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_BLANKFORM_H__E39DC7E3_A802_11D5_B63F_0050BAB1D92B__INCLUDED_)
#define AFX_BLANKFORM_H__E39DC7E3_A802_11D5_B63F_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// BlankForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CBlankForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

#include "resource.h"

class CBlankForm : public CFormView
{
protected:
    CBlankForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CBlankForm)

// Form Data
public:
    // {{AFX_DATA(CBlankForm)
    enum { IDD = IDD_BLANK_FORM };
    // }}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CBlankForm)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CBlankForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CBlankForm)
    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_BLANKFORM_H__E39DC7E3_A802_11D5_B63F_0050BAB1D92B__INCLUDED_)
