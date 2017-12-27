//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/AllPlanTreeView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ALLPLANTREEVIEW_H__9FEBFAE7_1B5C_45E2_B883_2FFF1BA6D2A3__INCLUDED_)
#define AFX_ALLPLANTREEVIEW_H__9FEBFAE7_1B5C_45E2_B883_2FFF1BA6D2A3__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "core/data_access_interface/PlanAgent/IDL/src/PlanAgentDataCorbaDef.h"
#include "PlanTreeView.h"


namespace TA_Base_App
{
    class CAllPlanDoc;


    /////////////////////////////////////////////////////////////////////////////
    // CAllPlanTreeView view

    class CAllPlanTreeView : public CPlanTreeView
    {
    protected:
        CAllPlanTreeView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CAllPlanTreeView)

    // Operations
    public:
        CAllPlanDoc* GetDocument();

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CAllPlanTreeView)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CAllPlanTreeView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
    protected:
        // {{AFX_MSG(CAllPlanTreeView)
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    #ifndef _DEBUG  // debug version in AllPlanTreeView.cpp
    inline CAllPlanDoc* CAllPlanTreeView::GetDocument()
        {
        FUNCTION_ENTRY( "GetDocument" );

        return (CAllPlanDoc*)CPlanTreeView::GetDocument(); }


    #endif

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_ALLPLANTREEVIEW_H__9FEBFAE7_1B5C_45E2_B883_2FFF1BA6D2A3__INCLUDED_)
