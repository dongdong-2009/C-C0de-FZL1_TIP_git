/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/prototype/src/PlanEditDoc.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// PlanEditDoc.h
//

#if !defined(AFX_PLANEDITDOC_H__24FA27DA_B3B3_11D5_B614_0078BFB1E915__INCLUDED_)
#define AFX_PLANEDITDOC_H__24FA27DA_B3B3_11D5_B614_0078BFB1E915__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


class CPlanEditDoc : public CDocument
{
protected: // create from serialization only
    CPlanEditDoc();
    DECLARE_DYNCREATE(CPlanEditDoc)

// Attributes
public:

// Operations
public:

// Overrides
    // ClassWizard generated virtual function overrides
    // {{AFX_VIRTUAL(CPlanEditDoc)
    public:
    virtual BOOL OnNewDocument();
    // }}AFX_VIRTUAL

// Implementation
public:
    virtual ~CPlanEditDoc();
    virtual void Serialize(CArchive& ar);   // overridden for document i/o
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    // {{AFX_MSG(CPlanEditDoc)
        // NOTE - the ClassWizard will add and remove member functions here.
        // DO NOT EDIT what you see in these blocks of generated code !
    // }}AFX_MSG
    DECLARE_MESSAGE_MAP()
};

#endif // AFX_PLANEDITDOC_H__24FA27DA_B3B3_11D5_B614_0078BFB1E915__INCLUDED_
