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
// Vertically movable splitter window. Automatically resizes controls below and above.

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "YSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CySplitterWnd

CySplitterWnd::CySplitterWnd() :
    m_highestCtrl(-1), m_lowestCtrl(-1), m_minAbove(0), m_minBelow(0), m_splitRatio(0.5)
{
    FUNCTION_ENTRY( "CySplitterWnd" );
    FUNCTION_EXIT;
}


CySplitterWnd::~CySplitterWnd()
{
    FUNCTION_ENTRY( "~CySplitterWnd" );
    FUNCTION_EXIT;
}


BOOL CySplitterWnd::BindWithControl(CWnd *parent, DWORD ctrlId)
{
    FUNCTION_ENTRY( "BindWithControl" );

    m_Parent = parent;

    SubclassWindow(m_Parent->GetDlgItem(ctrlId)->GetSafeHwnd());

    // Make sure to get mouse message from the dialog window
    DWORD style = GetStyle();
    ::SetWindowLong(GetSafeHwnd(), GWL_STYLE, style | SS_NOTIFY);

    FUNCTION_EXIT;
    return TRUE;
}


void CySplitterWnd::Unbind(void)
{
    FUNCTION_ENTRY( "Unbind" );

    DetachAll();
    UnsubclassWindow();

    FUNCTION_EXIT;
}


void CySplitterWnd::SetMinHeight(int above, int below)
{
    FUNCTION_ENTRY( "SetMinHeight" );

    m_minAbove = above;
    m_minBelow = below;

    FUNCTION_EXIT;
}


void CySplitterWnd::SetSplitRatio(double splitRatio)
{
    FUNCTION_ENTRY( "SetSplitRatio" );

    m_splitRatio = splitRatio;

    FUNCTION_EXIT;
}


BOOL CySplitterWnd::AttachAbove(DWORD ctrlId, int offset, BOOL fixedHeight)
{
    FUNCTION_ENTRY( "AttachAbove" );

    ASSERT(m_Parent != NULL);

    RECT rcPane;
    m_Parent->GetDlgItem(ctrlId)->GetWindowRect(&rcPane);
    m_Parent->ScreenToClient(&rcPane);

    ResizableControl ctrl;
    ctrl.m_ctrlId = ctrlId;
    ctrl.m_offset = offset;
    ctrl.m_fixedHeight = fixedHeight;
    ctrl.m_height = rcPane.bottom - rcPane.top;

    int idx = m_aboveControls.Add(ctrl);

    if (m_highestCtrl >= 0) {
        RECT rcHighest;
        int highestCtrlId;

        highestCtrlId = m_aboveControls.GetAt(m_highestCtrl).m_ctrlId;
        m_Parent->GetDlgItem(highestCtrlId)->GetWindowRect(&rcHighest);
        m_Parent->ScreenToClient(&rcHighest);

        if (rcPane.top < rcHighest.top) {
            m_highestCtrl = idx;
        }
    }
    else {
        m_highestCtrl = idx;
    }

    FUNCTION_EXIT;
    return TRUE;
}


BOOL CySplitterWnd::AttachBelow(DWORD ctrlId, int offset, BOOL fixedHeight)
{
    FUNCTION_ENTRY( "AttachBelow" );

    ASSERT(m_Parent != NULL);

    RECT rcPane;
    m_Parent->GetDlgItem(ctrlId)->GetWindowRect(&rcPane);
    m_Parent->ScreenToClient(&rcPane);

    ResizableControl ctrl;
    ctrl.m_ctrlId = ctrlId;
    ctrl.m_offset = offset;
    ctrl.m_fixedHeight = fixedHeight;
    ctrl.m_height = rcPane.bottom - rcPane.top;

    int idx = m_belowControls.Add(ctrl);

    if (m_lowestCtrl >= 0) {
        RECT rcLowest;
        int lowestCtrlId = m_belowControls.GetAt(m_lowestCtrl).m_ctrlId;
        m_Parent->GetDlgItem(lowestCtrlId)->GetWindowRect(&rcLowest);
        m_Parent->ScreenToClient(&rcLowest);

        if (rcPane.bottom > rcLowest.bottom) {
            m_lowestCtrl = idx;
        }
    }
    else {
        m_lowestCtrl = idx;
    }

    FUNCTION_EXIT;
    return TRUE;
}


BOOL CySplitterWnd::DetachAll(void)
{
    FUNCTION_ENTRY( "DetachAll" );

    m_aboveControls.RemoveAll();
    m_belowControls.RemoveAll();

    FUNCTION_EXIT;
    return TRUE;
}


void CySplitterWnd::RecalcLayout(void)
{
    FUNCTION_ENTRY( "RecalcLayout" );

    RECT rcHighest, rcLowest, rcBar, rcPane, rcParent;
    int highestCtrlId, lowestCtrlId;

    // Adjust the position of the splitter bar to maintain the required split ratio
    highestCtrlId = m_aboveControls.GetAt(m_highestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(highestCtrlId)->GetWindowRect(&rcHighest);
    m_Parent->ScreenToClient(&rcHighest);

    lowestCtrlId = m_belowControls.GetAt(m_lowestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(lowestCtrlId)->GetWindowRect(&rcLowest);
    m_Parent->ScreenToClient(&rcLowest);

    m_Parent->GetWindowRect(&rcParent);
    m_Parent->ScreenToClient(&rcParent);

    GetWindowRect(&rcBar);
    m_Parent->ScreenToClient(&rcBar);

    int splitterHeight = rcBar.bottom - rcBar.top;
    rcBar.left = rcParent.left;
    rcBar.right = rcParent.right;
    rcBar.top = rcHighest.top + (long)(m_splitRatio * (rcLowest.bottom - rcHighest.top)) - splitterHeight/2;
    rcBar.bottom = rcBar.top + splitterHeight;

    if (rcBar.bottom > rcParent.bottom) {
        rcBar.bottom = rcParent.bottom;
        rcBar.top = rcBar.bottom - splitterHeight;
    }

    if ((rcBar.top - rcHighest.top) < m_minAbove ||
        (rcLowest.bottom - rcBar.top) < m_minBelow) {
        FUNCTION_EXIT;
        return;
    }

    MoveWindow(&rcBar);

    // Resize the attached controls according to the current position of the splitter
    int i;
    ResizableControl ctrl;
    CWnd *pane;

    for (i=0; i<m_aboveControls.GetSize(); i++) {
        ctrl = m_aboveControls.GetAt(i);
        pane = m_Parent->GetDlgItem(ctrl.m_ctrlId);
        pane->GetWindowRect(&rcPane);
        m_Parent->ScreenToClient(&rcPane);
        rcPane.bottom = rcBar.top - ctrl.m_offset;
        if (ctrl.m_fixedHeight)
        {
            rcPane.top = rcPane.bottom - ctrl.m_height;
            if (rcPane.top < rcHighest.top)
            {
                rcPane.top = rcPane.bottom;
            }
  // hide the control as it cannot be shown at required height
        }
        else
        {
            if (rcPane.bottom < rcPane.top)
            {
                rcPane.bottom = rcPane.top;
            }

        }
        pane->MoveWindow(&rcPane, FALSE);
    }

    for (i=0; i<m_belowControls.GetSize(); i++) {
        ctrl = m_belowControls.GetAt(i);
        pane = m_Parent->GetDlgItem(ctrl.m_ctrlId);
        pane->GetWindowRect(&rcPane);
        m_Parent->ScreenToClient(&rcPane);
        rcPane.top = rcBar.bottom + ctrl.m_offset;
        if (ctrl.m_fixedHeight)
        {
            rcPane.bottom = rcPane.top + ctrl.m_height;
            if (rcPane.bottom > rcLowest.bottom)
            {
                rcPane.bottom = rcPane.top;
            }
  // hide the control as it cannot be shown at required height
        }
        else
        {
            if (rcPane.top > rcPane.bottom)
            {
                rcPane.top = rcPane.bottom;
            }

        }

        pane->MoveWindow(&rcPane, FALSE);
    }

    m_Parent->Invalidate();

    FUNCTION_EXIT;
}


BOOL CySplitterWnd::GetMouseClipRect(LPRECT rectClip, CPoint point)
{
    FUNCTION_ENTRY( "GetMouseClipRect" );

    RECT rectOrg, rectTarget, rectPane;
    int topLimit, bottomLimit;
    DWORD id;

    GetWindowRect(&rectOrg);

    id = m_aboveControls.GetAt(m_highestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(id)->GetWindowRect(&rectPane);
    topLimit = rectPane.top;

    id = m_belowControls.GetAt(m_lowestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(id)->GetWindowRect(&rectPane);
    bottomLimit = rectPane.bottom;

    rectTarget = rectOrg;
    rectTarget.top = topLimit + m_minAbove;
    rectTarget.bottom = bottomLimit - m_minBelow;

    if (rectTarget.top >= rectTarget.bottom) {
        TRACE("No room to drag the y-splitter bar");

        FUNCTION_EXIT;
        return FALSE;
    }

    rectClip->left = rectTarget.left;
    rectClip->right = rectTarget.right;
    rectClip->top = rectTarget.top + point.y;
    rectClip->bottom = rectTarget.bottom - (rectOrg.bottom - rectOrg.top - point.y) + 1;

    FUNCTION_EXIT;
    return TRUE;
}


BEGIN_MESSAGE_MAP(CySplitterWnd, CWnd)
    // {{AFX_MSG_MAP(CySplitterWnd)
    ON_WM_LBUTTONDOWN()
    ON_WM_SETCURSOR()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CySplitterWnd message handlers

BOOL CySplitterWnd::OnSetCursor(CWnd* /*pWnd*/, UINT /*nHitTest*/, UINT /*message*/)
{
    FUNCTION_ENTRY( "OnSetCursor" );

    // TODO: Add your message handler code here and/or call default
    ::SetCursor(AfxGetApp()->LoadCursor(AFX_IDC_VSPLITBAR));
    // ::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
    FUNCTION_EXIT;
    return TRUE;

    // return CWnd::OnSetCursor(pWnd, nHitTest, message);
}


void CySplitterWnd::OnLButtonDown(UINT /*nFlags*/, CPoint point)
{
    FUNCTION_ENTRY( "OnLButtonDown" );

    // TODO: Add your message handler code here and/or call default

    // don't handle if capture already set
    if (::GetCapture() != NULL)
    {
        FUNCTION_EXIT;
        return;
    }


    // don't handle if no room to drag
    RECT rectMouseClip;
    if (!GetMouseClipRect(&rectMouseClip, point))
    {
        FUNCTION_EXIT;
        return;
    }

    ::ClipCursor(&rectMouseClip);

    // set capture to the window which received this message
    SetCapture();
    ASSERT(this == CWnd::GetCapture());

    // get DC for drawing
    CDC* pDrawDC;
    pDrawDC = m_Parent->GetDC();
    ASSERT_VALID(pDrawDC);

    int     curX, curY;
    int     xDiff, yDiff;
    CRect   rectOrg, rectCur, rectOld;
    CSize   sizeBar;

    GetWindowRect(rectOrg);
    sizeBar = CSize(rectOrg.Width(), rectOrg.Height());

    m_Parent->ScreenToClient(rectOrg);
    pDrawDC->DrawDragRect(&rectOrg, sizeBar, NULL, sizeBar);
    rectOld = rectCur = rectOrg;
    xDiff = yDiff = 0;

    // get messages until capture lost or cancelled/accepted
    for (;;) {
        MSG msg;
        VERIFY(::GetMessage(&msg, NULL, 0, 0));

        if (CWnd::GetCapture() != this)
        {
            break;
        }


        switch (msg.message) {
        // handle movement/accept messages
        case WM_MOUSEMOVE:
            // handle resize cases (and part of move)
            curX = (int)(short)LOWORD(msg.lParam);
            curY = (int)(short)HIWORD(msg.lParam);

            xDiff = curX - point.x;
            yDiff = curY - point.y;

            rectCur = rectOrg;
            rectCur.top += yDiff;
            rectCur.bottom = rectCur.top + 4;  // use a fixed height for the dragged bar

            pDrawDC->DrawDragRect(&rectCur, sizeBar, &rectOld, sizeBar);
            rectOld = rectCur;

            break;

        // handle cancel messages
        case WM_KEYDOWN:
            if (msg.wParam != VK_ESCAPE)
            {
                break;
            }

        case WM_LBUTTONUP:
        case WM_RBUTTONDOWN:
            goto ExitLoop;

        // just dispatch rest of the messages
        default:
            DispatchMessage(&msg);
            break;
        }
    }

ExitLoop:
    rectCur.bottom = rectCur.top + rectOrg.Height(); // restore original bar height
    pDrawDC->DrawDragRect(&rectCur, sizeBar, NULL, sizeBar);

    m_Parent->ReleaseDC(pDrawDC);
    ReleaseCapture();
    ::ClipCursor(NULL);

    if (yDiff == 0)
    {
        FUNCTION_EXIT;
        return;
    }


    // calculate the position of the splitter with respect to the top of the highest
    // control and the bottom of the lowest one
    RECT rcHighest, rcLowest;
    int highestCtrlId, lowestCtrlId;

    highestCtrlId = m_aboveControls.GetAt(m_highestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(highestCtrlId)->GetWindowRect(&rcHighest);
    m_Parent->ScreenToClient(&rcHighest);

    lowestCtrlId = m_belowControls.GetAt(m_lowestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(lowestCtrlId)->GetWindowRect(&rcLowest);
    m_Parent->ScreenToClient(&rcLowest);

    DWORD barCentre = rectCur.bottom + rectCur.Height() / 2;
    DWORD above = barCentre - rcHighest.top;

    m_splitRatio = (double)above / double(rcLowest.bottom - rcHighest.top);

    // Reposition the bar and the attached controls
    RecalcLayout();

    FUNCTION_EXIT;
}


