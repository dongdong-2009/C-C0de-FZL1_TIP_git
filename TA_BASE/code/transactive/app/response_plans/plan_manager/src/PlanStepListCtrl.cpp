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
  * Represents the plan step list shown on the Steps tab of plan node view.
  *
  **/

#include "app/response_plans/plan_manager/src/StdAfx.h"
#include "core/utilities/src/TAAssert.h"
#include "bus/response_plans/active_plans_display/src/StringUtilities.h"
#include "app/response_plans/plan_manager/src/PlanManager.h"
#include "app/response_plans/plan_manager/src/PlanStepListCtrl.h"
#include "app/response_plans/plan_manager/src/PlanStep.h"
#include "app/response_plans/plan_manager/src/LabelMaker.h"
#include "app/response_plans/plan_manager/src/Utilities.h"
#include "app/response_plans/plan_manager/src/PlanManagerCommonDefs.h"
#include "app/response_plans/plan_manager/src/ApplicationFont.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

static const int MAX_IMAGE_COUNT = 16;
static const int START_EDIT_TIMER = 100;

static const COLORREF UNEXECUTED_STEP_COLOUR         = RGB(0, 0, 0);          // Black
static const COLORREF SKIPPED_STEP_COLOUR            = RGB(0x80, 0x80, 0x80); // Grey
static const COLORREF FAILED_STEP_COLOUR             = RGB(0x80, 0, 0);       // Red
static const COLORREF EXECUTED_SIMPLE_STEP_COLOUR    = RGB(0, 0x80, 0);       // Green
static const COLORREF EXECUTED_BRANCHING_STEP_COLOUR = RGB(0, 0, 0x80);       // Blue

using namespace TA_Base_App;
using namespace TA_Base_Core;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepListCtrl

CPlanStepListCtrl::CPlanStepListCtrl(bool execStatusVisible) :
    CEditableListCtrl(true, MAX_IMAGE_COUNT),
    m_execStatusVisible(execStatusVisible),
    m_planExecutionUpdateId(0),
    m_stepCustomisationUpdateId(0),
	m_nItem(-1),//TD14921 liqiang++
	m_nSubItem(-1),//TD14921 liqiang++
	m_bEnableTips(FALSE)//TD14921 liqiang++

{
    FUNCTION_ENTRY( "CPlanStepListCtrl" );
    FUNCTION_EXIT;
}


CPlanStepListCtrl::~CPlanStepListCtrl()
{
    FUNCTION_ENTRY( "~CPlanStepListCtrl" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepListCtrl, CEditableListCtrl)
    // {{AFX_MSG_MAP(CPlanStepListCtrl)
    ON_NOTIFY_REFLECT(NM_CUSTOMDRAW, OnItemCustomDraw)
    ON_NOTIFY_REFLECT_EX(LVN_ITEMCHANGED, OnItemChanged)
    ON_NOTIFY_REFLECT_EX(NM_CLICK, OnItemClick)
    ON_NOTIFY_REFLECT_EX(NM_DBLCLK, OnItemDblclk)
    ON_WM_TIMER()
    ON_WM_DESTROY()
	ON_WM_MOUSEMOVE()//TD14921 liqiang++
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CPlanStepListCtrl message handlers

void CPlanStepListCtrl::PreSubclassWindow()
{
    FUNCTION_ENTRY( "PreSubclassWindow" );

    // TODO: Add your specialized code here and/or call the base class
    ModifyStyle(0, LVS_REPORT | LVS_SINGLESEL | LVS_SHOWSELALWAYS | LVS_EDITLABELS);
    SetExtendedStyle(GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_GRIDLINES | LVS_EX_SUBITEMIMAGES /*| LVS_EX_INFOTIP*/);//TD14921 liqiang
	EnableTips();//TD14921 liqiang++

    // Initialize columns
	//cl-14921;
    InsertColumn(SLC_STEP_EXEC_STATE, _T(""), LVCFMT_LEFT, (m_execStatusVisible? 20 : 0)); // step execution status
    InsertColumn(SLC_STEP_POSITION, _T(LabelMaker::getStepListStepColumnLabel()), LVCFMT_RIGHT, 45);
    InsertColumn(SLC_STEP_NAME, _T(LabelMaker::getStepListNameColumnLabel()), LVCFMT_LEFT, 175);
    InsertColumn(SLC_STEP_TYPE, _T(LabelMaker::getStepListTypeColumnLabel()), LVCFMT_LEFT, 170);
    InsertColumn(SLC_STEP_DESCRIPTION, _T(LabelMaker::getStepListDescriptionColumnLabel()), LVCFMT_LEFT, 275);
    InsertColumn(SLC_STEP_SKIP, _T(LabelMaker::getStepListSkipColumnLabel()), LVCFMT_LEFT, 100);
    InsertColumn(SLC_STEP_IGNORE_FAILURE, _T(LabelMaker::getStepListIgnoreFailureColumnLabel()), LVCFMT_LEFT, 100);
    InsertColumn(SLC_STEP_DELAY, _T(LabelMaker::getStepListDelayColumnLabel()), LVCFMT_CENTER, 120);
	InsertColumn(SLC_STEP_NOWAIT, _T(LabelMaker::getStepListNoWaitColumnLabel()), LVCFMT_LEFT, (m_execStatusVisible ? 140 : 160));

    // Initialise the images used for showing execution progress
    CImageList *stepImages = new CImageList();
    stepImages->Create(IDB_STEP_EXEC_STATE_IMAGE_LIST_BMP, 16, MAX_IMAGE_COUNT, RGB(0xFF, 0, 0));

    SetImageList(stepImages, LVSIL_SMALL);

    // Initialise the custom plan list header
    CHeaderCtrl* defaultHeaderCtrl = GetHeaderCtrl();

    if (defaultHeaderCtrl)
    {
        m_headerCtrl.SubclassWindow(defaultHeaderCtrl->GetSafeHwnd());

        for (int i = 0; i < m_headerCtrl.GetItemCount(); i++)
        {
            HDITEM hdItem;
            hdItem.mask = HDI_FORMAT;

            m_headerCtrl.GetItem(i, &hdItem);
            hdItem.fmt |= HDF_OWNERDRAW;
            m_headerCtrl.SetItem(i, &hdItem);
        }
    }

    CEditableListCtrl::PreSubclassWindow();

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::OnDestroy()
{
    FUNCTION_ENTRY( "OnDestroy" );

    // TD#2479: free the memory allocated for the image list.
    // Note that CEditableListCtrl will delete the image list ONLY if it had itself created it.
    CImageList *stepImages = GetImageList(LVSIL_SMALL);
    SetImageList(NULL, LVSIL_SMALL);

    delete stepImages;
	
	m_toolTip.DestroyWindow();//TD14921 liqiang++
	m_toolTip.Detach();//TD14921 liqiang++

    CEditableListCtrl::OnDestroy();

	m_toolTip.DestroyWindow();//TD14921 liqiang++
	m_toolTip.Detach();//TD14921 liqiang++

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::OnItemCustomDraw(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemCustomDraw" );

    NMLVCUSTOMDRAW* pLVCD = (NMLVCUSTOMDRAW *) pNMHDR;

    // If the control is about to be painted ask for each item's pre-paint notification
    if (pLVCD->nmcd.dwDrawStage == CDDS_PREPAINT)
    {
        *pResult = CDRF_NOTIFYITEMDRAW;

         FUNCTION_EXIT;
         return;
    }

    // If an item is about to be painted
    if (pLVCD->nmcd.dwDrawStage == CDDS_ITEMPREPAINT)
    {
         // Set the text colour based on the corresponding step's execution state. Leave background colour alone.
        EPlanStepImageType stepImage = GetStepImage(static_cast<int> (pLVCD->nmcd.dwItemSpec));

        switch (stepImage)
        {
            case PSIT_EXECUTED_STEP:
            {
                PlanStep *planStep = reinterpret_cast<PlanStep *>(pLVCD->nmcd.lItemlParam);
                pLVCD->clrText = (planStep->isStepBranching()? EXECUTED_BRANCHING_STEP_COLOUR : EXECUTED_SIMPLE_STEP_COLOUR);
                break;
            }

            case PSIT_SKIPPED_STEP:
                pLVCD->clrText = SKIPPED_STEP_COLOUR;
                break;


            case PSIT_FAILED_STEP:
                pLVCD->clrText = FAILED_STEP_COLOUR;
                break;

            default:
                pLVCD->clrText = UNEXECUTED_STEP_COLOUR;
        }
    }

    // Tell the control to repaint itself
    *pResult = CDRF_DODEFAULT;

    FUNCTION_EXIT;
}


BOOL CPlanStepListCtrl::OnItemChanged(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemChanged" );

    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

    // TODO: Add your control notification handler code here
    if ((pNMListView->uNewState & LVIS_SELECTED) != LVIS_SELECTED &&
        (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        // Ensure that when an item is deselected by clicking below the last entry in the list
        // we kill the dotted focus box that still remains visible.
        SetItemState(pNMListView->iItem, ~LVIS_FOCUSED, LVIS_FOCUSED);
    }

    *pResult = 0;

    FUNCTION_EXIT;
    return FALSE; // Allow the parent to handle the notification as well
}


BOOL CPlanStepListCtrl::OnItemClick(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemClick" );

    // TODO: Add your control notification handler code here
    // Delay the edit action for a fraction of the double-click time in case another click follows (thus completing
    // a double-click).
    SetTimer(START_EDIT_TIMER, ::GetDoubleClickTime() / 4, NULL);

    *pResult = 0;

    FUNCTION_EXIT;
    return FALSE; // Allow the parent to handle the notification as well
}


BOOL CPlanStepListCtrl::OnItemDblclk(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemDblclk" );

    // TODO: Add your control notification handler code here
    KillTimer(START_EDIT_TIMER);

    *pResult = 0;

    FUNCTION_EXIT;
    return FALSE; // Allow the parent to handle the notification as well
}


void CPlanStepListCtrl::OnTimer(UINT nIDEvent)
{
    FUNCTION_ENTRY( "OnTimer" );

    // TODO: Add your message handler code here and/or call default
    if (nIDEvent == START_EDIT_TIMER)
    {
        KillTimer(START_EDIT_TIMER);
        startEdit();

        FUNCTION_EXIT;
        return;
    }

    CEditableListCtrl::OnTimer(nIDEvent);

    FUNCTION_EXIT;
}


BOOL CPlanStepListCtrl::SetReadOnly(BOOL readOnly)
{
    FUNCTION_ENTRY( "SetReadOnly" );

    DWORD colorRef = ::GetSysColor(readOnly? COLOR_3DFACE : COLOR_WINDOW);

    if (SetBkColor(colorRef) && SetTextBkColor(colorRef))
    {
        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}

bool CPlanStepListCtrl::enableNoWaitCheckButton(PlanStep *planStep, bool fIsSkipped)
{
	TA_ASSERT(planStep != NULL, "plan step should not be NULL");
	return ( planStep->canEnableNoWait() && planStep->canIgnoreFailure() && !fIsSkipped);
}

void CPlanStepListCtrl::AddStep(const StepDetail &stepDetail, PlanStep *planStep)
{
    FUNCTION_ENTRY( "AddStep" );

    int item = InsertItem(GetItemCount(), _T(""), PSIT_UNEXECUTED_STEP);

    SetItemText(item, SLC_STEP_POSITION, _T(TA_Base_Bus::StringUtilities::convertToString(stepDetail.position)));
    SetItemText(item, SLC_STEP_NAME, _T(stepDetail.name));
    SetItemText(item, SLC_STEP_TYPE, _T(Utilities::convertToString(stepDetail.type).c_str()));
    SetItemText(item, SLC_STEP_DESCRIPTION, _T(stepDetail.description));
    showCheckIcon(item, SLC_STEP_SKIP, stepDetail.skip, stepDetail.skippable);
    showCheckIcon(item, SLC_STEP_IGNORE_FAILURE, stepDetail.ignoreFailure, planStep->canIgnoreFailure() && !stepDetail.skip);
    SetItemText(item, SLC_STEP_DELAY, _T(Utilities::convertToString(stepDetail.delay).c_str()));
    showCheckIcon(item, SLC_STEP_NOWAIT, stepDetail.nowait, enableNoWaitCheckButton(planStep, stepDetail.skip) );

    SetItemData(item, reinterpret_cast<DWORD>(planStep));

    FUNCTION_EXIT;
}


PlanStep *CPlanStepListCtrl::GetStep(int item)
{
    FUNCTION_ENTRY( "GetStep" );

    TA_ASSERT(item >= 0 && item < GetItemCount(), "Invalid item index");

    FUNCTION_EXIT;
    return reinterpret_cast<PlanStep *>(GetItemData(item));
}


PlanStep *CPlanStepListCtrl::GetSelectedStep()
{
    FUNCTION_ENTRY( "GetSelectedStep" );

    int selectedItem = GetNextItem(-1, LVNI_SELECTED);
    if (selectedItem != -1)
    {
        FUNCTION_EXIT;
        return GetStep(selectedItem);
    }

    FUNCTION_EXIT;
    return NULL;
}


void CPlanStepListCtrl::SetSelectedStep(PlanStep *planStep, bool scrollIntoView)
{
    FUNCTION_ENTRY( "SetSelectedStep" );

    if (planStep)
    {
        SetSelectedStep(planStep->getStepPosition(), scrollIntoView);
    }

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::SetSelectedStep(PlanStepNumber stepNumber, bool scrollIntoView)
{
    FUNCTION_ENTRY( "SetSelectedStep" );

    // Find the item to select
    int item = FindStep(stepNumber);
    if (item != -1)
    {
        SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);

        if (scrollIntoView)
        {
            EnsureVisible(item, FALSE);
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::SetSelectedSteps(PlanStepNumber stepNumber, unsigned long stepCount, bool scrollIntoView)
{
    FUNCTION_ENTRY( "SetSelectedSteps" );

    // Find the item to select
    int topItem = FindStep(stepNumber);
    if (topItem != -1)
    {
        // Iterate over the required steps
        for (int item = topItem + (stepCount - 1); item >= topItem; item--)
        {
            SetItemState(item, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
        }

        if (scrollIntoView)
        {
            EnsureVisible(topItem, FALSE);
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::SetPlanExecutionUpdateId(TA_Base_Core::UpdateId updateId)
{
    FUNCTION_ENTRY( "SetPlanExecutionUpdateId" );

    m_planExecutionUpdateId = updateId;

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::SetStepCustomisationUpdateId(TA_Base_Core::UpdateId updateId)
{
    FUNCTION_ENTRY( "SetStepCustomisationUpdateId" );

    m_stepCustomisationUpdateId = updateId;

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::ResetPlanExecutionProgress()
{
    FUNCTION_ENTRY( "ResetPlanExecutionProgress" );

    SetPlanExecutionUpdateId(0);

    int itemCount = GetItemCount();
    for (int item = 0; item < itemCount; item++)
    {
        // Set the step's image to unexecuted
        SetStepImage(item, PSIT_UNEXECUTED_STEP);
    }

    // Reset colours on the currently visible steps. Remaining items will be redrawn as the users scrolls the list.
    int topOfPage = GetTopIndex();
    int bottomOfPage = topOfPage + GetCountPerPage();

    RedrawItems(topOfPage, bottomOfPage);

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::ResetCustomStepSettings()
{
    FUNCTION_ENTRY( "ResetCustomStepSettings" );

    int itemCount = GetItemCount();
    for (int item = 0; item < itemCount; item++)
    {
        PlanStep *planStep = GetStep(item);

        showCheckIcon(item, SLC_STEP_SKIP, planStep->isStepSkipped(), planStep->isStepSkippable());
        showCheckIcon(item, SLC_STEP_IGNORE_FAILURE, planStep->ignoreFailure(), planStep->canIgnoreFailure() && !planStep->isStepSkipped());
        SetItemText(item, SLC_STEP_DELAY, _T(Utilities::convertToString(planStep->getStepDelay()).c_str()));
		showCheckIcon(item, SLC_STEP_NOWAIT, planStep->noWait(), enableNoWaitCheckButton(planStep, planStep->isStepSkipped() ) );
    }

    SetStepCustomisationUpdateId(0);

    FUNCTION_EXIT;
}


BOOL CPlanStepListCtrl::PreCreateWindow( CREATESTRUCT& cs )
{
    BOOL bRes = CEditableListCtrl::PreCreateWindow( cs );

    ApplicationFont::getInstance().adjustChildrenFont( GetSafeHwnd() );

    return bRes;
}


void CPlanStepListCtrl::UpdatePlanExecutionProgress(const TA_Base_Core::ActivePlanDetail &activePlanDetail)
{
    FUNCTION_ENTRY( "UpdatePlanExecutionProgress" );

	// Find the item to update
	int item = FindStep(activePlanDetail.currentStepDetail.position);

    // Accept the update only if it is more recent than the last update consumed
//     if (Utilities::compareUpdateIds(activePlanDetail.activeUpdateId, m_planExecutionUpdateId) > 0)
//     {
        switch (activePlanDetail.activePlanState)
        {
            case TA_Base_Core::STOPPED_ACTIVE_PLAN_STATE:
            {
//                 // Find the item to update
//                 int item = FindStep(activePlanDetail.currentStepDetail.position);
                if (item != -1)
                {
                    // The image set when a plan is stopped depends on the previous state of the step at which the plan is stopped.
                    // If the step was in a loaded, paused or paused state, then the image is cleared. For other step states (skipped and
                    // executed), the image remains untouched.
                    EPlanStepImageType stepImage = GetStepImage(item);

                    // Clear if necessary
                    if (stepImage == PSIT_NEXT_STEP || stepImage == PSIT_PAUSED_STEP || stepImage == PSIT_DELAYED_STEP)
                    {
                        stepImage = PSIT_UNEXECUTED_STEP;
                    }

                    // Set the item's image and selection state.
                    SetStepImageAndSelection(item, stepImage, false);
                }
                break;
            }

            case TA_Base_Core::NEW_OWNER_ACTIVE_PLAN_STATE:
                // Nothing to do
                break;

            default:
                UpdatePlanExecutionProgress(activePlanDetail.currentStepDetail, true);
        }

        SetPlanExecutionUpdateId(activePlanDetail.activeUpdateId);
    // }

	if (item != -1)
		Update( item );

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::UpdatePlanExecutionProgress(const TA_Base_Core::ActiveStepDetail &activeStepDetail, bool scrollIntoView)
{
    FUNCTION_ENTRY( "UpdatePlanExecutionProgress" );

    // The "current" step must be highlighted. A step is defined as current when a "step loaded" or a "state paused"
    // update is received. To highlight the step and, at the same time, to keep the parameter list in sync the step
    // is simply selected. All updates other than loaded/paused cause the step to be deselected.
    EPlanStepImageType stepImage = PSIT_UNEXECUTED_STEP;
    bool selected = false;

    switch (activeStepDetail.state)
    {
        case TA_Base_Core::LOADED_ACTIVE_STEP_STATE:
            stepImage = PSIT_NEXT_STEP;
            selected = true;
            break;

        case TA_Base_Core::PAUSED_ACTIVE_STEP_STATE:
            stepImage = PSIT_PAUSED_STEP;
            selected = true;
            break;

        case TA_Base_Core::EXECUTED_ACTIVE_STEP_STATE:
            stepImage = PSIT_EXECUTED_STEP;
            selected = false;
            break;

        case TA_Base_Core::SKIPPED_ACTIVE_STEP_STATE:
            stepImage = PSIT_SKIPPED_STEP;
            selected = false;
            break;

        case TA_Base_Core::FAILED_ACTIVE_STEP_STATE:
            stepImage = PSIT_FAILED_STEP;
            selected = false;
            break;

        case TA_Base_Core::DELAYED_ACTIVE_STEP_STATE:
            stepImage = PSIT_DELAYED_STEP;
            selected = true;
            break;

        default:
            FUNCTION_EXIT;
            return;
    }

    // Find the item to update
    int item = FindStep(activeStepDetail.position);
    if (item != -1)
    {
        // Set the item's image and selection state.
        SetStepImageAndSelection(item, stepImage, selected);

        if (scrollIntoView)
        {
            // Scroll the item into view
            EnsureVisible(item, FALSE);
        }
    }

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::UpdateCustomStepSettings(const TA_Base_Core::PlanCustomisationUpdate &planCustomUpdate)
{
    FUNCTION_ENTRY( "UpdateCustomStepSettings" );

    // Accept the update only if it is more recent than the last update consumed
    if (Utilities::compareUpdateIds(planCustomUpdate.customUpdateId, m_stepCustomisationUpdateId) > 0)
    {
        UpdateCustomStepSettings(planCustomUpdate.detail);

        SetStepCustomisationUpdateId(planCustomUpdate.customUpdateId);
    }

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::UpdateCustomStepSettings(const TA_Base_Core::StepCustomisableDetail &stepCustomDetail)
{
    FUNCTION_ENTRY( "UpdateCustomStepSettings" );

    // Find the item to update
    int item = FindStep(stepCustomDetail.position);
    if (item != -1)
    {
        PlanStep *planStep = GetStep(item);

        showCheckIcon(item, SLC_STEP_SKIP, stepCustomDetail.skip, planStep->isStepSkippable());
        showCheckIcon(item, SLC_STEP_IGNORE_FAILURE, stepCustomDetail.ignoreFailure, planStep->canIgnoreFailure() && !stepCustomDetail.skip);
        SetItemText(item, SLC_STEP_DELAY, _T(Utilities::convertToString(stepCustomDetail.delay).c_str()));
        showCheckIcon(item, SLC_STEP_NOWAIT, stepCustomDetail.nowait, enableNoWaitCheckButton(planStep, stepCustomDetail.skip) );
    }

    FUNCTION_EXIT;
}


int CPlanStepListCtrl::FindStep(PlanStepNumber stepNumber)
{
    FUNCTION_ENTRY( "FindStep" );

    int itemCount = GetItemCount();
    if (stepNumber >= 1 && stepNumber <= static_cast<unsigned long>(itemCount))
    {
        // Start the search with the item at position (stepNumber - 1).
        // This should result in a direct hit.
		int item = 0;
        for (item = stepNumber - 1; item < GetItemCount(); item++)
        {
            PlanStep *planStep = GetStep(item);

            if (planStep->getStepPosition() == stepNumber)
            {
                FUNCTION_EXIT;
                return item;
            }
        }

        // Item not found at or above position (stepNumber - 1). Wrap around
        // to the top and search from there.
        for (item = 0; static_cast<unsigned long>(item) < stepNumber - 1; item++)
        {
            PlanStep *planStep = GetStep(item);

            if (planStep->getStepPosition() == stepNumber)
            {
                FUNCTION_EXIT;
                return item;
            }
        }
    }

    FUNCTION_EXIT;
    return -1;
}


EPlanStepImageType CPlanStepListCtrl::GetStepImage(int item)
{
    FUNCTION_ENTRY( "GetStepImage" );

    TA_ASSERT(item != -1, "Invalid item index");

    LVITEM lvItem;
    lvItem.mask = LVIF_IMAGE;
    lvItem.iItem = item;
    lvItem.iSubItem = 0;

    GetItem(&lvItem);

    FUNCTION_EXIT;
    return (EPlanStepImageType) lvItem.iImage;
}


void CPlanStepListCtrl::SetStepImage(int item, EPlanStepImageType stepImage)
{
    FUNCTION_ENTRY( "SetStepImage" );

    TA_ASSERT(item != -1, "Invalid item index");

    LVITEM lvItem;
    lvItem.mask = LVIF_IMAGE;
    lvItem.iItem = item;
    lvItem.iSubItem = 0;
    lvItem.iImage = stepImage;;

    SetItem(&lvItem);

    FUNCTION_EXIT;
}


void CPlanStepListCtrl::SetStepImageAndSelection(int item, EPlanStepImageType stepImage, bool selected)
{
    FUNCTION_ENTRY( "SetStepImageAndSelection" );

    TA_ASSERT(item != -1, "Invalid item index");

    int selectedState = LVIS_SELECTED | LVIS_FOCUSED;
    if (!selected)
    {
        selectedState = ~selectedState;
    }

    LVITEM lvItem;
    lvItem.mask = LVIF_IMAGE | LVIF_STATE;
    lvItem.iItem = item;
    lvItem.iSubItem = 0;
    lvItem.iImage = stepImage;
    lvItem.stateMask = LVIS_SELECTED | LVIS_FOCUSED;
    lvItem.state = selectedState;

    SetItem(&lvItem);

    FUNCTION_EXIT;
}


//TD14921 liqiang++
BOOL CPlanStepListCtrl::PreTranslateMessage(MSG* pMsg) 
{
	// TODO: Add your specialized code here and/or call the base class
	if(::IsWindow(m_toolTip.GetSafeHwnd()))
	{
		m_toolTip.RelayEvent(pMsg);
	}
	
	return CListCtrl::PreTranslateMessage(pMsg);
}


//TD14921 liqiang++
void CPlanStepListCtrl::OnMouseMove(UINT nFlags, CPoint point) 
{
	// TODO: Add your message handler code here and/or call default

	if(m_bEnableTips)
	{
		std::string str;
		LVHITTESTINFO lvhti;
		
		lvhti.pt = point;	
		SubItemHitTest(&lvhti);
		
		if((lvhti.iItem != m_nItem) || (lvhti.iSubItem != m_nSubItem))
		{
			m_nItem = lvhti.iItem;
			m_nSubItem = lvhti.iSubItem;
			
			if((m_nItem != -1) && (m_nSubItem != -1))
			{		
				m_toolTip.SetMaxTipWidth(5000);
				str = GetItemText(m_nItem ,m_nSubItem);
				
				int columnLength = GetColumnWidth(m_nSubItem);
				int textWidth = GetStringWidth(str.c_str()) + 12;
				if(columnLength >= textWidth)		
					return;

				unsigned startPos=0;
				unsigned endPos=1;
				int textLength;
				std::string subString;
				while((startPos+endPos)<str.size())
				{					
					subString=str.substr(startPos,endPos);
					textLength=GetStringWidth(subString.c_str());
					
					if(textLength<500)			
						endPos++;
					else
					{						
						str.insert(startPos+endPos+1,"\r\n");						
						startPos=startPos+endPos+3;
						endPos=1;
					}
					
				}
		
				m_toolTip.AddTool(this, str.c_str());
				m_toolTip.Pop();
			}
			else
			{
				m_toolTip.AddTool(this, "");
				m_toolTip.Pop();
			}
		}
	}
	
	CListCtrl::OnMouseMove(nFlags, point);
}

//TD14921 liqiang++
BOOL CPlanStepListCtrl::EnableTips()
{
	EnableToolTips(TRUE);
	
    m_bEnableTips = m_toolTip.Create(this, TTS_ALWAYSTIP);
	
	if(m_bEnableTips)
	{
		m_toolTip.Activate(TRUE);
		m_toolTip.SetDelayTime(TTDT_INITIAL, 0);
	}
	
	return m_bEnableTips;
}
//TD14921 ++liqiang

