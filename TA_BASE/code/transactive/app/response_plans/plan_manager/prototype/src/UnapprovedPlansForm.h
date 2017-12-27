#if !defined(AFX_UNAPPROVEDPLANSFORM_H__40247025_FF55_4A95_83F0_F150C8EF7D72__INCLUDED_)
#define AFX_UNAPPROVEDPLANSFORM_H__40247025_FF55_4A95_83F0_F150C8EF7D72__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// UnapprovedPlansForm.h : header file
//

#include "Notes.h"


/////////////////////////////////////////////////////////////////////////////
// CUnapprovedPlansForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CUnapprovedPlansForm : public CFormView
{
protected:
    CUnapprovedPlansForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CUnapprovedPlansForm)

// Form Data
public:
    // {{AFX_DATA(CUnapprovedPlansForm)
    enum { IDD = IDD_UNAPPROVEDPLANS_FORM };
    CNotes    m_notes;
    // }}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CUnapprovedPlansForm)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CUnapprovedPlansForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CUnapprovedPlansForm)
        // NOTE - the ClassWizard will add and remove member functions here.
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_UNAPPROVEDPLANSFORM_H__40247025_FF55_4A95_83F0_F150C8EF7D72__INCLUDED_)
