//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanDoc.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ACTIVEPLANDOC_H__E9ACE209_C546_4AC6_9197_81EAC428667D__INCLUDED_)
#define AFX_ACTIVEPLANDOC_H__E9ACE209_C546_4AC6_9197_81EAC428667D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanBrowserDoc.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CActivePlanDoc document

    class CActivePlanDoc : public CPlanBrowserDoc
    {
    public:
        // TD18095,jianghp
        virtual void OnGenericGuiInitCompleted();
        // TD18095,jianghp
    protected:
        CActivePlanDoc();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CActivePlanDoc)

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CActivePlanDoc)
    public:
        virtual BOOL OnNewDocument();
    virtual void OnCloseDocument();
    // }}AFX_VIRTUAL

    // Implementation
    public:
        virtual ~CActivePlanDoc();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

    // Generated message map functions
    protected:
        // {{AFX_MSG(CActivePlanDoc)
            // NOTE - the ClassWizard will add and remove member functions here.
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_ACTIVEPLANDOC_H__E9ACE209_C546_4AC6_9197_81EAC428667D__INCLUDED_)
