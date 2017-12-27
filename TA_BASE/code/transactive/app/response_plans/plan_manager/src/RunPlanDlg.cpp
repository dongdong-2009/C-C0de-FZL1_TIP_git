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

#include "Stdafx.h"
#include <sstream>
#include "core/exceptions/src/TransactiveException.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "RunPlanDlg.h"
#include "LabelMaker.h"
#include "PlanNode.h"
#include "TreeNodeFactory.h"
#include "PlanAgentAccess.h"
#include "MessageBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CRunPlanDlg dialog


CRunPlanDlg::CRunPlanDlg(const TA_Base_Core::CurrentPlanStepDetail& stepDetail) :
    CInteractiveStepDlg(stepDetail, TA_Base_Bus::IPlanManagerCorbaDef::RUNPLAN_RESPONSE_TIMEOUT),
    m_message("")
{
    FUNCTION_ENTRY( "CRunPlanDlg" );

    // {{AFX_DATA_INIT(CRunPlanDlg)
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


void CRunPlanDlg::DoDataExchange(CDataExchange* pDX)
{
    FUNCTION_ENTRY( "DoDataExchange" );

    CInteractiveStepDlg::DoDataExchange(pDX);
    // {{AFX_DATA_MAP(CRunPlanDlg)
    DDX_Text(pDX, IDC_MESSAGE, m_message);
    // }}AFX_DATA_MAP

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CRunPlanDlg, CInteractiveStepDlg)
    // {{AFX_MSG_MAP(CRunPlanDlg)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CRunPlanDlg message handlers

BOOL CRunPlanDlg::OnInitDialog()
{
    FUNCTION_ENTRY( "OnInitDialog" );

    CInteractiveStepDlg::OnInitDialog();

    // TODO: Add extra initialization here
    TA_Base_Core::CurrentPlanStepDetail stepDetail(getStepDetail());
    std::ostringstream childPlan;

    try
    {
        childPlan << "'" << PlanAgentAccess::getInstance().getNodePath(stepDetail.data.runPlan().plan) << "'";
    }
    catch (...)
    {
        childPlan << "ID " << stepDetail.data.runPlan().plan;
    }

    TA_Base_Bus::TransActiveMessage userMsg;
    userMsg << childPlan.str() << stepDetail.data.runPlan().instance;

    m_message = userMsg.constructMessage(IDS_UI_210057);

    UpdateData(FALSE);

    FUNCTION_EXIT;
    return TRUE;  // return TRUE unless you set the focus to a control
                  // EXCEPTION: OCX Property Pages should return FALSE
}


CString CRunPlanDlg::getTitlePrefix() const
{
    FUNCTION_ENTRY( "getTitlePrefix" );
    FUNCTION_EXIT;
    return LabelMaker::getRunPlanStepDialogTitlePrefix();
}


void CRunPlanDlg::reportCompletionStatus() const
{
    FUNCTION_ENTRY( "reportCompletionStatus" );

    TA_Base_Core::EPlanStepCompletionStatus completionStatus(TA_Base_Core::PSCS_UNDEFINED);
    std::string completionError;

    try
    {
        TA_Base_Core::ActivePlanId planToControl(getStepDetail().data.runPlan());

        // Get the plan tree node corresponding to the child plan. Create if necessary.
        TreeNode* treeNode = TreeNodeFactory::getInstance().getTreeNode(planToControl.plan);
        if (!treeNode)
        {
            treeNode = TreeNodeFactory::getInstance().createTreeNode(planToControl.plan);
            TA_ASSERT(treeNode, "Tree node produced by factory is null");
        }

        PlanNode *planNode = static_cast<PlanNode *>(treeNode);
        TA_ASSERT(planNode, "Tree node produced by factory is not a plan");

        // Launch the Plan Controller for the child plan and select the pending instance.
        planNode->controlPlan(PlanInstanceId(planToControl.instance, planToControl.location));

        completionStatus = TA_Base_Core::PSCS_SUCCESS;
        completionError = "";
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "TransactiveException", ex.what());

        completionStatus = TA_Base_Core::PSCS_FAILURE;
        completionError = ex.what();
    }
    catch (...)
    {
        LOG_EXCEPTION_CATCH(SourceInfo,    "UnknownException", "");

        completionStatus = TA_Base_Core::PSCS_FAILURE;
        completionError = "Failed to launch GUI application";
    }

    try
    {
        PlanAgentAccess::getInstance().reportRunPlanStepResult(getStepDetail().activePlan, completionStatus, completionError);
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_REPORT_COMPLETION_STATUS, ex);
    }

    FUNCTION_EXIT;
}


