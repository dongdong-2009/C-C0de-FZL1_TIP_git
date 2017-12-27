/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/plugin/plan/src/PlanDialog.h $
  * @author:  San Teo
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * The dialog that will be shown to the operator when the showJobDetailsDialog() method is called from
  * the IJobDetailsComponent interface.  The dialog will allow the operator to select a plan.
  * 
  */

#if !defined(AFX_PLANDIALOG_H__980B399D_9CE2_4186_8547_638BDA034E3B__INCLUDED_)
#define AFX_PLANDIALOG_H__980B399D_9CE2_4186_8547_638BDA034E3B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "app/scheduling/scheduling_manager/plugin/plan/src/resource.h"
#include "bus/response_plans/plan_tree/src/PlanTreeCtrl.h"
#include "bus/response_plans/plan_tree/src/PlanItem.h"

// forces mfc extensions to be imported
#define IMPORT_EXTENSIONS

namespace TA_Base_Bus
{
    class CPlanTreeCtrl;
}


namespace TA_Base_App
{
    /////////////////////////////////////////////////////////////////////////////
    // PlanDialog dialog
    class PlanDialog : public CDialog
    {
    // Construction
    public:
        /** 
          * PlanDialog
          *
          * Shows the plan dialog.  If readOnly is true, the dialog will not allow a new plan to be selected.  The dialog
          * will highlight the specified plan.
          *
          * @param readOnly If true, operator cannot select new plan
          * @param selectedPlanPath The full path of the plan to be selected
          * @param pParent The parent of this dialog
          *
          */
	    PlanDialog(bool readOnly, const CString& selectedPlanPath, CWnd* pParent = NULL);   // standard constructor

        /** 
          * getSelectedPlanPath
          *
          * Returns the path of the selected plan
          *
          *
          * @return the path of the selected plan
          */
        CString getSelectedPlanPath() const
        {
            return m_selectedPlanPath;
        }


    // Dialog Data
	    //{{AFX_DATA(PlanDialog)
	    enum { IDD = IDD_PLAN_DIALOG };
	    CButton	                m_okButton;
	    TA_Base_Bus::CPlanTreeCtrl	m_planCtrl;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(PlanDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(PlanDialog)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnPlanTreeItemSelected(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        bool m_readOnly;
        CString m_selectedPlanPath;
    };
};
//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PLANDIALOG_H__980B399D_9CE2_4186_8547_638BDA034E3B__INCLUDED_)
