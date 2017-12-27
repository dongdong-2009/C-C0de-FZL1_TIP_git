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

#include "stdafx.h"
#include "core/exceptions/src/TransactiveException.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanControllerDoc.h"
#include "FloatingDocTemplate.h"
#include "PlanNode.h"
#include "EndStep.h"
#include "PlanInstance.h"
#include "PlanAgentUpdateProcessor.h"
#include "MessageBox.h"
#include "OperatorRights.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc

IMPLEMENT_DYNCREATE(CPlanControllerDoc, CPlanBrowserDoc)

BEGIN_MESSAGE_MAP(CPlanControllerDoc, CPlanBrowserDoc)
    // {{AFX_MSG_MAP(CPlanControllerDoc)
    ON_UPDATE_COMMAND_UI(ID_PLAN_EXECUTE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_CUSTOMISE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_STOP, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_PAUSE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_RESUME, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_RESUME_SELECTED_STEP, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_TAKECONTROL, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_RELEASECONTROL, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_PLAN_SCHEDULE, OnUpdatePlanControlCmd)
    ON_UPDATE_COMMAND_UI(ID_STEP_CUSTOMISE, OnUpdatePlanControlCmd)
    ON_COMMAND(ID_PLAN_EXECUTE, OnStartPlan)
    ON_COMMAND(ID_PLAN_CUSTOMISE, OnCustomisePlan)
    ON_COMMAND(ID_PLAN_STOP, OnStopPlan)
    ON_COMMAND(ID_PLAN_PAUSE, OnPausePlan)
    ON_COMMAND(ID_PLAN_RESUME, OnResumePlan)
    ON_COMMAND(ID_PLAN_RESUME_SELECTED_STEP, OnResumePlan)
    ON_COMMAND(ID_PLAN_TAKECONTROL, OnTakeControlOfPlan)
    ON_COMMAND(ID_PLAN_RELEASECONTROL, OnReleaseControlOfPlan)
    ON_COMMAND(ID_PLAN_SCHEDULE, OnSchedulePlan)
    ON_COMMAND(ID_STEP_CUSTOMISE, OnCustomiseStep)
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc construction/destruction

CPlanControllerDoc::CPlanControllerDoc()
{
    FUNCTION_ENTRY( "CPlanControllerDoc" );
    FUNCTION_EXIT;
}


CPlanControllerDoc::~CPlanControllerDoc()
{
    FUNCTION_ENTRY( "~CPlanControllerDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanControllerDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    if (!CPlanBrowserDoc::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    CFloatingDocTemplate* docTemplate = static_cast<CFloatingDocTemplate*>(GetDocTemplate());
    SetCurrentNode(docTemplate->getNode());

    PlanAgentUpdateProcessor::getInstance().subscribeForActivePlanUpdates(this);
    PlanAgentUpdateProcessor::getInstance().subscribeForPlanCustomisationUpdates(this);
    PlanAgentUpdateProcessor::getInstance().subscribeForPlanConfigUpdates(this);  // Need to observe plan move updates for
                                                                                  // the title to be refreshed

    // Register for SessionId runtime parameter updates. Operator rights may change with the session.
    TA_Base_Core::RunParams::getInstance().registerRunParamUser(this, RPARAM_SESSIONID);

    FUNCTION_EXIT;
    return TRUE;
}


void CPlanControllerDoc::OnCloseDocument()
{
    FUNCTION_ENTRY( "OnCloseDocument" );

    PlanAgentUpdateProcessor::getInstance().unsubscribeFromActivePlanUpdates(this);
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromPlanCustomisationUpdates(this);
    PlanAgentUpdateProcessor::getInstance().unsubscribeFromPlanConfigUpdates(this);

    TA_Base_Core::RunParams::getInstance().deregisterRunParamUser(this);

    CPlanBrowserDoc::OnCloseDocument();

    FUNCTION_EXIT;
}


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc diagnostics

#ifdef _DEBUG
void CPlanControllerDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CPlanBrowserDoc::AssertValid();

    FUNCTION_EXIT;
}


void CPlanControllerDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CPlanBrowserDoc::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CPlanControllerDoc commands

void CPlanControllerDoc::OnFileExit()
{
    FUNCTION_ENTRY( "OnFileExit" );

    // TODO: Add your command handler code here
    AfxGetMainWnd()->SendMessage(WM_CLOSE);

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnUpdatePlanControlCmd(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdatePlanControlCmd" );

    PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
    TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

    try
    {
        switch (pCmdUI->m_nID)
        {
            case ID_PLAN_EXECUTE:
            case ID_PLAN_CUSTOMISE:
            case ID_PLAN_SCHEDULE:
                planNode->updateCmdUI(*pCmdUI);
                break;

            case ID_STEP_CUSTOMISE:
                // disable if no step selected or if the selected step is the End Step
                if (!GetCurrentStep() || dynamic_cast<EndStep *>(GetCurrentStep()) != NULL)
                {
                    pCmdUI->Enable(FALSE);
                    break;
                }
                // fall through to allow the instance to handle the update
            case ID_PLAN_RESUME_SELECTED_STEP:
            case ID_PLAN_STOP:
            case ID_PLAN_PAUSE:
            case ID_PLAN_RESUME:
            case ID_PLAN_TAKECONTROL:
            case ID_PLAN_RELEASECONTROL:
                if (GetCurrentInstance())
                {
                    GetCurrentInstance()->updateCmdUI(*pCmdUI, *planNode);
                    break;
                }
                // fall through to default

            default:
                pCmdUI->Enable(FALSE);
        }
    }
    catch (...)
    {
        pCmdUI->Enable(FALSE);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnStartPlan()
{
    FUNCTION_ENTRY( "OnStartPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
        TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

        planNode->startNewInstance();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_START_NEW_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnCustomisePlan()
{
    FUNCTION_ENTRY( "OnCustomisePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
        TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

        planNode->customiseNewInstance();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_CUSTOMISE_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnStopPlan()
{
    FUNCTION_ENTRY( "OnStopPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        TA_ASSERT(NULL != planInstance.get(), "No plan instance selected");

        planInstance->stop();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_STOP_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnPausePlan()
{
    FUNCTION_ENTRY( "OnPausePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        TA_ASSERT(NULL != planInstance.get(), "No plan instance selected");

        planInstance->pause();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_PAUSE_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnResumePlan()
{
    FUNCTION_ENTRY( "OnResumePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        TA_ASSERT(NULL != planInstance.get(), "No plan instance selected");

        planInstance->resume();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_RESUME_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnTakeControlOfPlan()
{
    FUNCTION_ENTRY( "OnTakeControlOfPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        TA_ASSERT(NULL != planInstance.get(), "No plan instance selected");

        planInstance->takeControl();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_TAKE_CONTROL_OF_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnReleaseControlOfPlan()
{
    FUNCTION_ENTRY( "OnReleaseControlOfPlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        TA_ASSERT(NULL != planInstance.get(), "No plan instance selected");

        planInstance->releaseControl();
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_RELEASE_CONTROL_OF_INSTANCE, ex);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnSchedulePlan()
{
    FUNCTION_ENTRY( "OnSchedulePlan" );

    // TODO: Add your command handler code here
    try
    {
        PlanNode *planNode = static_cast<PlanNode *>(GetCurrentNode());
        TA_ASSERT(planNode, "Plan controller document does not have a valid plan node");

        planNode->schedulePlan();
    }
    catch (TA_Base_Core::TransactiveException&)
    {
        CString applicationName;
        applicationName.LoadString(IDS_SCHEDULING_MANAGER);
        TA_Base_Bus::TransActiveMessage userMsg;
        userMsg << applicationName;
        UINT selectedButton = userMsg.showMsgBox(IDS_UE_010002);
    }

    FUNCTION_EXIT;
}


void CPlanControllerDoc::OnCustomiseStep()
{
    FUNCTION_ENTRY( "OnCustomiseStep" );

    // TODO: Add your command handler code here
    try
    {
        PlanStep *planStep = GetCurrentStep();
        TA_ASSERT(planStep, "Plan controller document does not have a valid plan step");

        PlanInstanceSharedPtr planInstance = GetCurrentInstance();
        TA_ASSERT(planInstance, "No plan instance selected");

        TA_Base_Core::StepCustomisableDetail customStepDetail = planInstance->getCustomStepDetail(planStep);

        if (planStep->customiseStep(customStepDetail))
        {
            planInstance->customiseStep(customStepDetail);
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_CUSTOMISE_STEP, ex);
    }

    FUNCTION_EXIT;
}


bool CPlanControllerDoc::ControlEnabled() const
{
    FUNCTION_ENTRY( "ControlEnabled" );
    FUNCTION_EXIT;
    return true;
}


