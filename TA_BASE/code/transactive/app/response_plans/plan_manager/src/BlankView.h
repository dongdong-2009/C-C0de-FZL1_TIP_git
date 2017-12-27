//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/BlankView.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_BLANKVIEW_H__73A0F940_FCBE_4E66_AD0A_8863B99E7213__INCLUDED_)
#define AFX_BLANKVIEW_H__73A0F940_FCBE_4E66_AD0A_8863B99E7213__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CBlankView form view

    class CBlankView : public CView
    {
    protected:
        CBlankView();           // protected constructor used by dynamic creation
        DECLARE_DYNCREATE(CBlankView)

    // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CBlankView)
    public:
        virtual void OnDraw(CDC* pDC);
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        virtual ~CBlankView();
    #ifdef _DEBUG
        virtual void AssertValid() const;
        virtual void Dump(CDumpContext& dc) const;
    #endif

        // Generated message map functions
        // {{AFX_MSG(CBlankView)
        afx_msg BOOL OnEraseBkgnd(CDC* pDC);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_BLANKVIEW_H__73A0F940_FCBE_4E66_AD0A_8863B99E7213__INCLUDED_)
