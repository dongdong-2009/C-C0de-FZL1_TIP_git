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
  * Header control for the plan step list shown on the Steps tab of plan node view.
  * Allows column labels to be split into 2 lines.
  *
  **/

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "PlanStepListHeaderCtrl.h"
#include "PlanStepListCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CPlanStepListHeaderCtrl

CPlanStepListHeaderCtrl::CPlanStepListHeaderCtrl()
{
    FUNCTION_ENTRY( "CPlanStepListHeaderCtrl" );
    FUNCTION_EXIT;
}


CPlanStepListHeaderCtrl::~CPlanStepListHeaderCtrl()
{
    FUNCTION_ENTRY( "~CPlanStepListHeaderCtrl" );
    FUNCTION_EXIT;
}


void CPlanStepListHeaderCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
    FUNCTION_ENTRY( "DrawItem" );

    // Get column label text
    HDITEM hdi;
    CString text;

    hdi.mask = HDI_TEXT;
    hdi.cchTextMax = 256;
    hdi.pszText = text.GetBuffer(hdi.cchTextMax);

    GetItem(lpDrawItemStruct->itemID, &hdi);

    text.ReleaseBuffer();

    // Determine the size of the text in logical units
    CSize originalTextSize;
    ::GetTextExtentPoint(lpDrawItemStruct->hDC, text, text.GetLength(), &originalTextSize);

    // Calculate text rectangle
    CRect tmpRect = lpDrawItemStruct->rcItem;
    int requiredTextHeight = ::DrawText(lpDrawItemStruct->hDC, text, text.GetLength(), &tmpRect, DT_CALCRECT | DT_WORDBREAK);
    if (tmpRect.bottom > lpDrawItemStruct->rcItem.bottom)
    {
        lpDrawItemStruct->rcItem.bottom = tmpRect.bottom;
    }

    // Draw the button frame based on calculated text rectangle
    ::DrawFrameControl(lpDrawItemStruct->hDC, &lpDrawItemStruct->rcItem, DFC_BUTTON, DFCS_BUTTONPUSH);

    // Determine drawing format.
    UINT format = DT_CENTER;
    if (requiredTextHeight > originalTextSize.cy || text.Find('\n') != -1)
    {
        format |= DT_WORDBREAK;
    }
    else
    {
        format |= DT_SINGLELINE | DT_VCENTER;
    }

    // Draw label text.
    ::DrawText(lpDrawItemStruct->hDC, text, text.GetLength(), &lpDrawItemStruct->rcItem, format);

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CPlanStepListHeaderCtrl, CHeaderCtrl)
    // {{AFX_MSG_MAP(CPlanStepListHeaderCtrl)
    ON_MESSAGE(HDM_LAYOUT, OnLayout)
    ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGINGA, OnItemChanging)
    ON_NOTIFY_REFLECT_EX(HDN_ITEMCHANGINGW, OnItemChanging)
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CPlanStepListHeaderCtrl message handlers

LRESULT CPlanStepListHeaderCtrl::OnLayout(WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "OnLayout" );

    // This method determines the size of the header rectangle and the list control below it.

    CClientDC dc(this);
    CSize textSize = dc.GetTextExtent( "single line" );

    HDLAYOUT* layout = reinterpret_cast<HDLAYOUT*>(lParam);

    layout->pwpos->hwnd = GetSafeHwnd();
    layout->pwpos->hwndInsertAfter = NULL;
    layout->pwpos->flags = SWP_FRAMECHANGED;

    layout->pwpos->x = layout->prc->left;
    layout->pwpos->y = 0;
    layout->pwpos->cx = layout->prc->right - layout->prc->left;
    layout->pwpos->cy = layout->prc->top = 2 /*lines*/ * textSize.cy + 2;

    FUNCTION_EXIT;
    return 0;
}


BOOL CPlanStepListHeaderCtrl::OnItemChanging(NMHDR* pNMHDR, LRESULT* pResult)
{
    FUNCTION_ENTRY( "OnItemChanging" );

    NMHEADER *nmHeader = (NMHEADER *) pNMHDR;

    // Ignore all events related to resizing the column used to show plan execution progress.
    if (nmHeader->iItem == CPlanStepListCtrl::SLC_STEP_EXEC_STATE &&
        nmHeader->pitem && ((nmHeader->pitem->mask & HDI_WIDTH) == HDI_WIDTH))
    {
        *pResult = 1;
    }
    else
    {
        *pResult = 0;
    }

    // TD8877: Allow the step list to handle the notification as well.
    // The list ctrl will adjust the horizontal scrollbar if necessary.
    FUNCTION_EXIT;
    return *pResult;
}


