//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ActivePlanFrame.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_ACTIVEPLANFRAME_H__0030AE2D_E475_4236_BFC6_A3B626929C8D__INCLUDED_)
#define AFX_ACTIVEPLANFRAME_H__0030AE2D_E475_4236_BFC6_A3B626929C8D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanBrowserFrame.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CActivePlanFrame frame

    class CActivePlanFrame : public CPlanBrowserFrame
    {
        DECLARE_DYNCREATE(CActivePlanFrame)

    protected:
        CActivePlanFrame();           // protected constructor used by dynamic creation

    // Attributes
    public:

    // Operations
    public:

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CActivePlanFrame)
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CActivePlanFrame();

        // Generated message map functions
        // {{AFX_MSG(CActivePlanFrame)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_ACTIVEPLANFRAME_H__0030AE2D_E475_4236_BFC6_A3B626929C8D__INCLUDED_)
