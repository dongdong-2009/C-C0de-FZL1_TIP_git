/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/ApprovedPlansForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_APPROVEDPLANSFORM_H__E965A897_D9C1_42C0_8A25_EF0810CA75A0__INCLUDED_)
#define AFX_APPROVEDPLANSFORM_H__E965A897_D9C1_42C0_8A25_EF0810CA75A0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ApprovedPlansForm.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CApprovedPlansForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CApprovedPlansForm : public CFormView
{
protected:
    CApprovedPlansForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CApprovedPlansForm)

// Form Data
public:
    // {{AFX_DATA(CApprovedPlansForm)
    enum { IDD = IDD_APPROVEDPLANS_FORM };
        // NOTE: the ClassWizard will add data members here
    // }}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CApprovedPlansForm)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CApprovedPlansForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CApprovedPlansForm)
        // NOTE - the ClassWizard will add and remove member functions here.
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_APPROVEDPLANSFORM_H__E965A897_D9C1_42C0_8A25_EF0810CA75A0__INCLUDED_)
