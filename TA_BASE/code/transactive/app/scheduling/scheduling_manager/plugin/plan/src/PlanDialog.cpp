/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scheduling/scheduling_manager/plugin/plan/src/PlanDialog.cpp $
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

#include "app/scheduling/scheduling_manager/plugin/plan/src/stdafx.h"
#include "app/scheduling/scheduling_manager/plugin/plan/src/PlanDialog.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// PlanDialog dialog

namespace TA_Base_App
{

    PlanDialog::PlanDialog(bool readOnly, const CString& selectedPlanPath, CWnd* pParent /*=NULL*/)
	    : CDialog(PlanDialog::IDD, pParent),
        m_planCtrl(TA_Base_Bus::PlanFilter::PFT_APPROVED_PLANS, false), 
        m_readOnly(readOnly),
        m_selectedPlanPath(selectedPlanPath)
    {
	    //{{AFX_DATA_INIT(PlanDialog)
	    //}}AFX_DATA_INIT

    }


    void PlanDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(PlanDialog)
	    DDX_Control(pDX, IDOK, m_okButton);
	    DDX_Control(pDX, IDC_TREE_PLAN, m_planCtrl);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(PlanDialog, CDialog)
	    //{{AFX_MSG_MAP(PlanDialog)
	    ON_NOTIFY(TVN_SELCHANGED, IDC_TREE_PLAN, OnPlanTreeItemSelected)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // PlanDialog message handlers

    BOOL PlanDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

        if (!m_selectedPlanPath.IsEmpty())
        {
            try
            {
                // TODO: need to disable the plan tree
                m_planCtrl.setSelectedPlan(static_cast<LPCTSTR>(m_selectedPlanPath));
            }
            catch (...)
            {
                // Failed to select the specified plan. The plan may have been renamed/moved/deleted.
                // Do nothing. The operator will manually select their plan.
            }
        }    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void PlanDialog::OnPlanTreeItemSelected(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);

	    // Enable if it is a plan, disable if it is a catagory
	    TA_Base_Bus::CPlanItem* planItem = m_planCtrl.getSelectedPlan();
        if (planItem == NULL)
        {
            m_okButton.EnableWindow(FALSE);
        }
        else
        {
            m_selectedPlanPath = planItem->getPlanPath().c_str();
            m_okButton.EnableWindow(!m_readOnly);
        }

        *pResult = 0;
    }
};

