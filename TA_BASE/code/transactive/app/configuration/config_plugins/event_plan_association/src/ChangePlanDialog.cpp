/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/event_plan_association/src/ChangePlanDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to change a plan.
  */

#include "app/configuration/config_plugins/event_plan_association/src/StdAfx.h"
#include "app/configuration/config_plugins/event_plan_association/src/ChangePlanDialog.h"

#include "core/utilities/src/DebugUtil.h"

#define IMPORT_EXTENSIONS
#include "bus/response_plans/plan_tree/src/PlanItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

    ChangePlanDialog::ChangePlanDialog(const std::string& currentPlanPath, CWnd* pParent /*=NULL*/)
	    : m_planPath(currentPlanPath.c_str()),
          m_planTree(TA_Base_Bus::PlanFilter::PFT_APPROVED_PLANS),
          CDialog(ChangePlanDialog::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(ChangePlanDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void ChangePlanDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ChangePlanDialog)
	    DDX_Control(pDX, IDC_PLAN_TREE, m_planTree);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ChangePlanDialog, CDialog)
	//{{AFX_MSG_MAP(ChangePlanDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PLAN_TREE, onSelchangedPlanTree)
	ON_NOTIFY(NM_DBLCLK, IDC_PLAN_TREE, onDblclkPlanTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // ChangePlanDialog message handlers


    BOOL ChangePlanDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        if (!m_planPath.IsEmpty())
        {
            try
            {
	            m_planTree.setSelectedPlan( std::string(m_planPath) );
            }
            catch (...)
            {
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn,
                            "Invalid current plan path: %s", static_cast<LPCTSTR>(m_planPath));
            }
        }
	    
        GetDlgItem(IDOK)->EnableWindow( m_planTree.getSelectedPlan() !=  NULL );

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void ChangePlanDialog::onSelchangedPlanTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = reinterpret_cast<NM_TREEVIEW*>(pNMHDR);
	    
        GetDlgItem(IDOK)->EnableWindow( m_planTree.getSelectedPlan() !=  NULL );
	    
	    *pResult = 0;
    }

    
    void ChangePlanDialog::onDblclkPlanTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        onOK();

        *pResult = 0;
    }


    void ChangePlanDialog::onOK() 
    {
        TA_Base_Bus::CPlanItem* item = m_planTree.getSelectedPlan();
        if (item == NULL)
        {
            return;
        }

        m_planPath = item->getPlanPath().c_str();

        CDialog::OnOK();
    }
}
