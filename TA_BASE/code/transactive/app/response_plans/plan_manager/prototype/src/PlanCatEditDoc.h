/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanCatEditDoc.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANCATEDITDOC_H__9D413144_E2C5_4610_B0CE_959519D1C948__INCLUDED_)
#define AFX_PLANCATEDITDOC_H__9D413144_E2C5_4610_B0CE_959519D1C948__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanCatEditDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlanCatEditDoc document

class CPlanCatEditDoc : public CDocument
{
protected:
    CPlanCatEditDoc();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CPlanCatEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanCatEditDoc)
    public:
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
    protected:
    virtual BOOL OnNewDocument();
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanCatEditDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    // {{AFX_MSG(CPlanCatEditDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANCATEDITDOC_H__9D413144_E2C5_4610_B0CE_959519D1C948__INCLUDED_)
