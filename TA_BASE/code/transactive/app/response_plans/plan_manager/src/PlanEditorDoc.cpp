/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Bart Golab
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  * Plan Editor GUI is based on an SDI. This class is a representation of the document associated with the
  * plan editor frame and its plan node view.
  *
  **/

#include "stdafx.h"

#include <set>

#include "core/utilities/src/TAAssert.h"
#include "core/utilities/src/RunParams.h"
#include "bus/generic_gui/src/TransActiveMessage.h"
#include "PlanManager.h"
#include "PlanEditorDoc.h"
#include "ViewUpdate.h"
#include "Timer.h"
#include "FloatingDocTemplate.h"
#include "PlanStep.h"
#include "EndStep.h"
#include "PlanNode.h"
#include "MessageBox.h"
#include "NewPlanStepDlg.h"
#include "PlanStepPastePositionDlg.h"
#include "OperatorRights.h"
#include "PlanManagerCommonDefs.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanEditorDoc

IMPLEMENT_DYNCREATE(CPlanEditorDoc, CPlanBrowserDoc)

CPlanEditorDoc::CPlanEditorDoc() : m_copyStepAction(SCA_NO_ACTION)
{
    FUNCTION_ENTRY( "CPlanEditorDoc" );
    FUNCTION_EXIT;
}


BOOL CPlanEditorDoc::OnNewDocument()
{
    FUNCTION_ENTRY( "OnNewDocument" );

    CFloatingDocTemplate* floatDocTemplate = dynamic_cast<CFloatingDocTemplate*>(GetDocTemplate());
    SetCurrentNode(floatDocTemplate->getNode());

    if (!CDocument::OnNewDocument())
    {
        FUNCTION_EXIT;
        return FALSE;
    }


    // Lock the plan for editing
    FUNCTION_EXIT;
    return TRUE;
}


void CPlanEditorDoc::SetCurrentNode(TreeNode *node)
{
    FUNCTION_ENTRY( "SetCurrentNode" );

    TA_ASSERT(node!=NULL,"No node specified.");

    // Delete any existing plan copy
    delete GetCurrentNode();

    // Make a copy of the specified plan for editing
    PlanNode*   planNode = dynamic_cast<PlanNode*>(node);
    TA_ASSERT(planNode!=NULL,"Not a plan node.");
    PlanNode*   planNodeCopy  = new PlanNode(*planNode);
    CPlanBrowserDoc::SetCurrentNode(planNodeCopy);

    FUNCTION_EXIT;
}


CPlanEditorDoc::~CPlanEditorDoc()
{
    FUNCTION_ENTRY( "~CPlanEditorDoc" );

    m_copyStepList.clear();

    delete GetCurrentNode();

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanEditorDoc, CPlanBrowserDoc)
    // {{AFX_MSG_MAP(CPlanEditorDoc)
    ON_UPDATE_COMMAND_UI(ID_PLAN_SAVE, OnUpdatePlanSave)
    ON_UPDATE_COMMAND_UI(ID_STEP_EDIT, OnUpdateCmdUI)
    ON_UPDATE_COMMAND_UI(ID_STEP_DELETE, OnUpdateCmdUI)
    ON_UPDATE_COMMAND_UI(ID_STEP_NEW, OnUpdateCmdUI)
    ON_UPDATE_COMMAND_UI(ID_STEP_COPY, OnUpdateCmdUI)
    ON_UPDATE_COMMAND_UI(ID_STEP_CUT, OnUpdateCmdUI)
    ON_UPDATE_COMMAND_UI(ID_STEP_PASTE, OnUpdateCmdUI)
    ON_COMMAND(ID_PLAN_SAVE, OnPlanSave)
    ON_COMMAND(ID_STEP_EDIT, OnStepEdit)
    ON_COMMAND(ID_STEP_DELETE, OnStepDelete)
    ON_COMMAND(ID_STEP_NEW, OnStepNew)
    ON_COMMAND(ID_STEP_COPY, OnStepCopy)
    ON_COMMAND(ID_STEP_CUT, OnStepCut)
    ON_COMMAND(ID_STEP_PASTE, OnStepPaste)
    ON_COMMAND(ID_FILE_EXIT, OnFileExit)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanEditorDoc diagnostics

#ifdef _DEBUG
void CPlanEditorDoc::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CDocument::AssertValid();

    FUNCTION_EXIT;
}


void CPlanEditorDoc::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CDocument::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


void CPlanEditorDoc::OnFileExit()
{
    FUNCTION_ENTRY( "OnFileExit" );

    AfxGetMainWnd()->SendMessage(WM_CLOSE);

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnPlanSave()
{
    FUNCTION_ENTRY( "OnPlanSave" );

    DoFileSave();

    FUNCTION_EXIT;
}


BOOL CPlanEditorDoc::DoFileSave()
{
    FUNCTION_ENTRY( "DoFileSave" );

    PlanNode*   planNode = dynamic_cast<PlanNode*>(GetCurrentNode());
    TA_ASSERT(planNode!=NULL,"Not a plan node.");

    try
    {
        CWaitCursor waitCursor;

        planNode->savePlan();

        RefreshTitle(); // TD#2555: Update plan name in title bar

        SetModifiedFlag(false);

        FUNCTION_EXIT;
        return TRUE;
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_SAVE_PLAN, ex);
    }
    catch (...)
    {
        MessageBox::error(IDS_SAVE_PLAN);
    }

    FUNCTION_EXIT;
    return FALSE;
}


void CPlanEditorDoc::OnUpdateCmdUI(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdateCmdUI" );

    PlanStepList currentSteps = GetCurrentSteps();

    if (currentSteps.size() == 0)
    {
        pCmdUI->Enable(FALSE);

        FUNCTION_EXIT;
        return;
    }

    CCmdUIProbe cmdUIProbe(pCmdUI->m_nID);

    switch (cmdUIProbe.m_nID)
    {
        case ID_STEP_PASTE:
            cmdUIProbe.Enable(m_copyStepAction != SCA_NO_ACTION);

        case ID_STEP_NEW:
        case ID_STEP_EDIT:
            cmdUIProbe.Enable(currentSteps.size() == 1);
    }

    for (PlanStepList::iterator iter = currentSteps.begin(); iter != currentSteps.end(); iter++)
    {
        PlanStep *planStep = *iter;
        planStep->updateCmdUI(cmdUIProbe);
    }

    pCmdUI->Enable(cmdUIProbe.m_enabled);

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnUpdatePlanSave(CCmdUI* pCmdUI)
{
    FUNCTION_ENTRY( "OnUpdatePlanSave" );

    // Enable when plan is modified
    pCmdUI->Enable(IsModified() && OperatorRights::getInstance().canEditPlans());

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepEdit()
{
    FUNCTION_ENTRY( "OnStepEdit" );

    try
    {
        // Start the step editor
        PlanStep *planStep = GetCurrentStep();
        if (planStep)
        {
            PlanNode*   planNode = dynamic_cast<PlanNode*>(GetCurrentNode());
            TA_ASSERT(planNode!=NULL,"Not a plan node.");

            bool stepModified = planNode->editPlanStep(planStep);

            if (stepModified)
            {
                SetModifiedFlag();
            }
        }
    }
    catch (TA_Base_Core::TransactiveException &ex)
    {
        MessageBox::error(IDS_EDIT_PLAN_STEP, ex);
    }
    catch (...)
    {
        MessageBox::error(IDS_EDIT_PLAN_STEP);
    }

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepNew()
{
    FUNCTION_ENTRY( "OnStepNew" );

    PlanNode* planNode = dynamic_cast<PlanNode*>(GetCurrentNode());
    TA_ASSERT(planNode!=NULL,"Not a plan node.");

    PlanStep *planStep = GetCurrentStep();
    TA_ASSERT(planStep!=NULL,"Plan step is null");

    bool isEndStep(dynamic_cast<EndStep *>(planStep) != NULL);
    CNewPlanStepDlg dlg(isEndStep);

    if (dlg.DoModal() == IDOK)
    {
        // Get insert position
        CNewPlanStepDlg::EInsertPositionType insertPosition = dlg.getInsertPosition();
        PlanStepNumber stepNumber = (insertPosition == CNewPlanStepDlg::IPT_INSERT_BEFORE)?
                                    planStep->getStepPosition() : planStep->getStepPosition()+1;

        // Get step type
        TA_Base_Core::EStep stepType = dlg.getStepType();

        try
        {
            CWaitCursor waitCursor;

            // Create the new step

            planNode->insertPlanStep(stepType, stepNumber);

            SetModifiedFlag();
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_CREATE_NEW_STEP, ex);
        }
    }

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepDelete()
{
    FUNCTION_ENTRY( "OnStepDelete" );

    PlanStepList currentSteps = GetCurrentSteps();

    TA_Base_Bus::TransActiveMessage userMsg;
    CString actionName;
    actionName.LoadString((currentSteps.size() == 1) ? IDS_DELETE_SINGLE_STEP : IDS_DELETE_MULTIPLE_STEPS);
    userMsg << actionName;
    UINT selectedButton = userMsg.showMsgBox(IDS_UW_010003);

    // Check if the user really wishes to do this
    if (selectedButton ==IDYES)
    {
        CWaitCursor waitCursor;

        // Delete the step(s)
        PlanNode* planNode = dynamic_cast<PlanNode*>(GetCurrentNode());
        TA_ASSERT(planNode!=NULL,"Not a plan node.");

        // Remove deleted step from copy step list
        if (!m_copyStepList.empty())
        {
            std::set<PlanStep*> setCurSteps;
            for (PlanStepList::iterator itLoop = currentSteps.begin(); currentSteps.end() != itLoop; ++itLoop)
            {
                setCurSteps.insert(*itLoop);
            }
            
            for (PlanStepList::iterator itLoop = m_copyStepList.begin(); m_copyStepList.end() != itLoop;)
            {
                std::set<PlanStep*>::iterator itFound = setCurSteps.find(*itLoop);
                if (setCurSteps.end() == itFound)
                {
                    ++itLoop;
                    continue;
                }

                itLoop = m_copyStepList.erase(itLoop);

                if (m_copyStepList.empty())
                {
                    m_copyStepAction = SCA_NO_ACTION;
                    break;
                }

                setCurSteps.erase(itFound);

                if (setCurSteps.empty())
                {
                    break;
                }
            }

            setCurSteps.clear();
        }

        planNode->deletePlanSteps(currentSteps);
        TA_ASSERT(currentSteps.empty(), "Deleted step list is not empty");

        SetModifiedFlag();
    };

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepCopy()
{
    FUNCTION_ENTRY( "OnStepCopy" );

    m_copyStepAction = SCA_COPY_STEP;
   m_copyStepList = GetCurrentSteps();

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepCut()
{
    FUNCTION_ENTRY( "OnStepCut" );

    m_copyStepAction = SCA_CUT_STEP;
   m_copyStepList = GetCurrentSteps();

    FUNCTION_EXIT;
}


void CPlanEditorDoc::OnStepPaste()
{
    FUNCTION_ENTRY( "OnStepPaste" );

    if ( m_copyStepList.empty())
    {
        m_copyStepAction = SCA_NO_ACTION;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "%s", "No plan has been copied or cut - cannot paste");

        return;
    }

    if (m_copyStepAction != SCA_CUT_STEP && m_copyStepAction != SCA_COPY_STEP)
    {
        m_copyStepAction = SCA_NO_ACTION;
        LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Invalid paste action %d", m_copyStepAction);

        return;
    }

    PlanStep *planStep = GetCurrentStep();
    bool isEndStep = (dynamic_cast<EndStep *>(planStep) != NULL);

    CPlanStepPastePositionDlg dlg(isEndStep);

    if (dlg.DoModal() == IDOK)
    {
        // Get paste position
        CPlanStepPastePositionDlg::EPastePositionType pastePosition = dlg.getPastePosition();
        PlanStepNumber stepNumber = (pastePosition == CPlanStepPastePositionDlg::PPT_PASTE_BEFORE)?
                                    planStep->getStepPosition() : planStep->getStepPosition() + 1;

        try
        {
            CWaitCursor waitCursor;

            PlanNode* planNode = dynamic_cast<PlanNode*>(GetCurrentNode());
            TA_ASSERT(planNode!=NULL,"Not a plan node.");

            if (m_copyStepAction == SCA_CUT_STEP)
            {
                // Shift the step(s) to their new position
                planNode->movePlanSteps(m_copyStepList, stepNumber);
                m_copyStepAction = SCA_NO_ACTION; // Can paste only once for any given cut.
                m_copyStepList.clear();

                SetModifiedFlag();
            }
            else if (m_copyStepAction == SCA_COPY_STEP)
            {
                // Duplicate the step(s)
                planNode->copyPlanSteps(m_copyStepList, stepNumber);

                SetModifiedFlag();
            }
        }
        catch (TA_Base_Core::TransactiveException  &ex)
        {
            MessageBox::error(IDS_PASTE_STEP, ex);
        }
    }

    FUNCTION_EXIT;
}


bool CPlanEditorDoc::EditingEnabled() const
{
    FUNCTION_ENTRY( "EditingEnabled" );
    FUNCTION_EXIT;
    return true;
}


