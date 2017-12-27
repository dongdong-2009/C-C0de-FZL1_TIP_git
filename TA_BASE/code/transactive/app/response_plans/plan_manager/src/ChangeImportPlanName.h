/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/response_plans/plan_manager/src/ChangeImportPlanName.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_CHANGEIMPORTPLANNAME_H__3961DCAD_F79B_4A8D_9EA1_9A73C2A8B195__INCLUDED_)
#define AFX_CHANGEIMPORTPLANNAME_H__3961DCAD_F79B_4A8D_9EA1_9A73C2A8B195__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ChangeImportPlanName.h : header file
//
#pragma warning(disable: 4786)

#include "CategoryNode.h"

/////////////////////////////////////////////////////////////////////////////
// CChangeImportPlanName dialog
namespace TA_Base_App
{
    class CChangeImportPlanName : public CDialog
    {
        // Construction
    public:
        CChangeImportPlanName(CategoryNode& category, CString promptMessage, CWnd* pParent = NULL);   // standard constructor

        CString GetNewPlanName ();
    private:
        bool PlanNameExists (CString newName);

        // Dialog Data
        // {{AFX_DATA(CChangeImportPlanName)
        enum { IDD = IDD_CHANGE_IMPORT_PLAN_NAME };
        CEdit    m_PromptMessageEdit;
        CEdit    m_NewPlanNameEdit;
        CString    m_PromptMessageTxt;
        CString    m_NewPlanNameTxt;
        // }}AFX_DATA


        // Overrides
        // ClassWizard generated virtual function overrides
        // {{AFX_VIRTUAL(CChangeImportPlanName)
    protected:
        virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
        // }}AFX_VIRTUAL

        // Implementation
    protected:
    private:
        CategoryNode& m_categoryNode;

        // Generated message map functions
        // {{AFX_MSG(CChangeImportPlanName)
        // NOTE: the ClassWizard will add member functions here
        virtual void OnOK();
        // }}AFX_MSG
        DECLARE_MESSAGE_MAP()
    };
}//namespace
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_CHANGEIMPORTPLANNAME_H__3961DCAD_F79B_4A8D_9EA1_9A73C2A8B195__INCLUDED_)
