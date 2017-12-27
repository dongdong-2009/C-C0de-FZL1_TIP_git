//
// The source code in this file is the property of
// Ripple Systems and is not for redistribution
// in any form.
//
// Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/CategoryListCtrl.h $
// @author:  Bart Golab
// @version: $Revision: #1 $
//
// Last modification: $DateTime: 2012/06/12 13:35:44 $
// Last modified by:  $Author: builder $
//
// <description>

#if !defined(AFX_CATEGORYLISTCTRL_H__D2213021_C829_4808_9BA1_FD8799FB2BC0__INCLUDED_)
#define AFX_CATEGORYLISTCTRL_H__D2213021_C829_4808_9BA1_FD8799FB2BC0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


namespace TA_Base_App
{
    class TreeNode;


    /////////////////////////////////////////////////////////////////////////////
    // CCategoryListCtrl window

    class CCategoryListCtrl : public CListCtrl
    {
    // Construction
    public:
        CCategoryListCtrl();
        virtual ~CCategoryListCtrl();


    // Operations
    public:
        void RemoveNode(TreeNode *treeNode);

    protected:
        int FindNode(TreeNode *treeNode);  // returns -1 if node is not found

    // Attributes
    private:


    // Overrides
    protected:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CCategoryListCtrl)
        virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
        // }}AFX_VIRTUAL

    // Implementation
    protected:
        // Generated message map functions
        // {{AFX_MSG(CCategoryListCtrl)
        afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
        afx_msg void OnGetDispInfo(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult);
        afx_msg void OnDestroy();
        // }}AFX_MSG

        DECLARE_MESSAGE_MAP()
    };

    /////////////////////////////////////////////////////////////////////////////

    // {{AFX_INSERT_LOCATION}}
    // Microsoft Visual C++ will insert additional declarations immediately before the previous line.

} // namespace TA_Base_App

#endif // !defined(AFX_CATEGORYLISTCTRL_H__D2213021_C829_4808_9BA1_FD8799FB2BC0__INCLUDED_)
