/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanViewerDoc.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanViewerDoc.h : interface of the CPlanViewerDoc class
//
/////////////////////////////////////////////////////////////////////////////

#if !defined(AFX_PLANVIEWERDOC_H__24FA27DA_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_)
#define AFX_PLANVIEWERDOC_H__24FA27DA_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlanViewerDoc : public CDocument
{
protected: // create from serialization only
    CPlanViewerDoc();
    DECLARE_DYNCREATE(CPlanViewerDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanViewerDoc)
    public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanViewerDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    // {{AFX_MSG(CPlanViewerDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        // DO NOT EDIT what you see in these blocks of generated code !
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANVIEWERDOC_H__24FA27DA_B3B3_11D5_B644_0050BAB1D92B__INCLUDED_)
