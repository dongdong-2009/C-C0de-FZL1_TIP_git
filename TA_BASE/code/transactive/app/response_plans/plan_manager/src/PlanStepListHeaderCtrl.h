/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepListHeaderCtrl.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * Header control for the plan step list shown on the Steps tab of plan node view.
  * Allows column labels to be split into 2 lines.
  *
  **/

#if !defined(AFX_PLANSTEPLISTHEADERCTRL_H__8839C76A_39CB_4CD1_A9B3_D77B3E64E8C8__INCLUDED_)
#define AFX_PLANSTEPLISTHEADERCTRL_H__8839C76A_39CB_4CD1_A9B3_D77B3E64E8C8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepListHeaderCtrl window

    class CPlanStepListHeaderCtrl : public CHeaderCtrl
    {
    // Construction/destruction
    public:
        CPlanStepListHeaderCtrl();
        virtual ~CPlanStepListHeaderCtrl();


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepListHeaderCtrl)
        virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);
        // }}AFX_VIRTUAL

    // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanStepListHeaderCtrl)
        afx_msg LRESULT OnLayout(WPARAM wParam, LPARAM lParam);
        afx_msg BOOL OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPLISTHEADERCTRL_H__8839C76A_39CB_4CD1_A9B3_D77B3E64E8C8__INCLUDED_)
