/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/AllPlansForm.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_ALLPLANSFORM_H__86BE106C_AA81_46D4_AFD8_2A6146B46F86__INCLUDED_)
#define AFX_ALLPLANSFORM_H__86BE106C_AA81_46D4_AFD8_2A6146B46F86__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// AllPlansForm.h : header file
//

#include "Notes.h"


/////////////////////////////////////////////////////////////////////////////
// CAllPlansForm form view

#ifndef __AFXEXT_H__
#include <afxext.h>
#endif

class CAllPlansForm : public CFormView
{
protected:
    CAllPlansForm();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CAllPlansForm)

// Form Data
public:
    // {{AFX_DATA(CAllPlansForm)
    enum { IDD = IDD_ALLPLANS_FORM };
    CNotes    m_notes;
    // }}AFX_DATA

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CAllPlansForm)
    protected:
    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
    // }}AFX_VIRTUAL

// Implementation
protected:
    virtual ~CAllPlansForm();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
    // {{AFX_MSG(CAllPlansForm)
        // NOTE - the ClassWizard will add and remove member functions here.
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ALLPLANSFORM_H__86BE106C_AA81_46D4_AFD8_2A6146B46F86__INCLUDED_)
