//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File$
// @author:  Bart Golab
// @version: $Revision$
//
// Last modification: $DateTime$
// Last modified by:  $Author$
//
// <description>

#if !defined(AFX_PLANDETAILPAGE_H__89B1A01E_7AD6_4BDA_83A6_3CF9DF0B2578__INCLUDED_)
#define AFX_PLANDETAILPAGE_H__89B1A01E_7AD6_4BDA_83A6_3CF9DF0B2578__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class CPlanBrowserDoc;
    class PlanNode;
    class PlanInstanceFilter;


    /////////////////////////////////////////////////////////////////////////////
    // CPlanDetailPage dialog

    class CPlanDetailPage : public CPropertyPage
    {
    // Construction
    public:
        CPlanDetailPage(UINT nIDTemplate, UINT nIDCaption = 0);  // standard constructor
        virtual ~CPlanDetailPage();


    // Operations
    protected:
        virtual CPropertySheet *GetPropertySheet();
        virtual CPlanBrowserDoc *GetDocument();
        virtual PlanNode *GetPlanNode();
        virtual PlanInstanceFilter *GetPlanInstanceFilter();

        virtual bool IsActive();
        virtual void UpdatePlanDetail();
        virtual void AdjustControlPositions();
        virtual void SetPlanFilterButtonStates();
        virtual void ShowRelevantControls() = 0;
        virtual void RefreshControlContents() = 0;
        virtual void RefreshActiveInstances() = 0;
        virtual void PurgeActiveInstances() = 0;


    // Attributes
    private:
        bool m_isRefreshPending;

    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanDetailPage)
        virtual BOOL OnSetActive();
        virtual BOOL OnInitDialog();
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        // }}AFX_VIRTUAL

        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanDetailPage)
        afx_msg void OnAllOwnersCheck();
        afx_msg void OnAllLocationsCheck();
        // }}AFX_MSG
        afx_msg LRESULT OnResetTreeNodeView(WPARAM wParam, LPARAM lParam);
        DECLARE_MESSAGE_MAP()

    // Dialog Data
    protected:
        // {{AFX_DATA(CPlanDetailPage)
        CButton    m_allOwnersCheck;
        CButton    m_allLocationsCheck;
        // }}AFX_DATA
    };

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_PLANDETAILPAGE_H__89B1A01E_7AD6_4BDA_83A6_3CF9DF0B2578__INCLUDED_)
