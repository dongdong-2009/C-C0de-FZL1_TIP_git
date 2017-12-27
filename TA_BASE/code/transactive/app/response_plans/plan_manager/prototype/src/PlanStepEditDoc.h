/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanStepEditDoc.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_PLANSTEPEDITDOC_H__1B399D36_1E9A_42AE_BA62_38DEDBB10883__INCLUDED_)
#define AFX_PLANSTEPEDITDOC_H__1B399D36_1E9A_42AE_BA62_38DEDBB10883__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// PlanStepEditDoc.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CPlanStepEditDoc document

class CPlanStepEditDoc : public CDocument
{
protected:
    CPlanStepEditDoc();           // protected constructor used by dynamic creation
    DECLARE_DYNCREATE(CPlanStepEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanStepEditDoc)
    public:
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
    protected:
    virtual BOOL OnNewDocument();
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanStepEditDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

    // Generated message map functions
protected:
    // {{AFX_MSG(CPlanStepEditDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANSTEPEDITDOC_H__1B399D36_1E9A_42AE_BA62_38DEDBB10883__INCLUDED_)
