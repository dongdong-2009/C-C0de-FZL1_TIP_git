//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/UnapprovedPlanTreeFrame.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_UNAPPROVEDPLANTREEFRAME_H__8DC118F5_80BF_4CE5_BD13_3D607FA8BE22__INCLUDED_)
#define AFX_UNAPPROVEDPLANTREEFRAME_H__8DC118F5_80BF_4CE5_BD13_3D607FA8BE22__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanTreeFrame.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CUnapprovedPlanTreeFrame frame

    class CUnapprovedPlanTreeFrame : public CPlanTreeFrame
    {
        DECLARE_DYNCREATE(CUnapprovedPlanTreeFrame)
    protected:
        CUnapprovedPlanTreeFrame();           // protected constructor used by dynamic creation

    // Operations
    protected:
        virtual CRuntimeClass *GetLeftPaneRuntimeClass();

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CUnapprovedPlanTreeFrame)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CUnapprovedPlanTreeFrame();

        // Generated message map functions
        // {{AFX_MSG(CUnapprovedPlanTreeFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_UNAPPROVEDPLANTREEFRAME_H__8DC118F5_80BF_4CE5_BD13_3D607FA8BE22__INCLUDED_)
