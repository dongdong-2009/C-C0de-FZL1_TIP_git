/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/PlanStepEditorPropertySheet.h $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  *
  *
  **/

#if !defined(AFX_PLANSTEPEDITORPROPERTYSHEET_H__93EDBD58_0177_42FE_B0F4_BC319A4F3E00__INCLUDED_)
#define AFX_PLANSTEPEDITORPROPERTYSHEET_H__93EDBD58_0177_42FE_B0F4_BC319A4F3E00__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "PlanStepEditorTabCtrl.h"


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // CPlanStepEditorPropertySheet

    class CPlanStepEditorPropertySheet : public CPropertySheet
    {
        DECLARE_DYNAMIC(CPlanStepEditorPropertySheet)

    // Construction/destruction
    public:
        CPlanStepEditorPropertySheet();
        CPlanStepEditorPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
        CPlanStepEditorPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);
        virtual ~CPlanStepEditorPropertySheet();

    // Operations
    public:
        virtual void Add(CPropertyPage* propertyPage, bool tabEnabled);
        virtual void Activate(CWnd* pParentWnd);


    // Attributes
    private:
        CPlanStepEditorTabCtrl m_tabCtrl;


    // Overrides
    public:
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CPlanStepEditorPropertySheet)
        virtual BOOL OnInitDialog();
        virtual BOOL PreTranslateMessage(MSG* pMsg);
        // }}AFX_VIRTUAL

        // Generated message map functions
    protected:
        // {{AFX_MSG(CPlanStepEditorPropertySheet)
            // NOTE - the ClassWizard will add and remove member functions here.
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_PLANSTEPEDITORPROPERTYSHEET_H__93EDBD58_0177_42FE_B0F4_BC319A4F3E00__INCLUDED_)
