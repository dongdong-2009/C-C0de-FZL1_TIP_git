// YSplitterWnd.cpp : implementation file
//

#include "stdafx.h"

#include "YSplitterWnd.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


/////////////////////////////////////////////////////////////////////////////
// CySplitterWnd

CySplitterWnd::CySplitterWnd() : m_highestCtrl(-1), m_lowestCtrl(-1), m_minAbove(0), m_minBelow(0)
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


BOOL CySplitterWnd::AttachAbove(DWORD ctrlId, int offset)
{
    FUNCTION_ENTRY( "AttachAbove" );

    ResizableControl ctrl;
    ctrl.m_ctrlId = ctrlId;
    ctrl.m_offset = offset;
    m_aboveControls.Add(ctrl);

    FUNCTION_EXIT;
    return TRUE;
}


BOOL CySplitterWnd::AttachBelow(DWORD ctrlId, int offset)
{
    FUNCTION_ENTRY( "AttachBelow" );

    ResizableControl ctrl;
    ctrl.m_ctrlId = ctrlId;
    ctrl.m_offset = offset;
    m_belowControls.Add(ctrl);

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

    CWnd *pane;
    RECT rcBar, rcPane;

    GetWindowRect(&rcBar);
    m_Parent->ScreenToClient(&rcBar);

    int i;
    DWORD id;
    int offset;

    for (i=0; i<m_aboveControls.GetSize(); i++) {
        id = m_aboveControls.GetAt(i).m_ctrlId;
        offset = m_aboveControls.GetAt(i).m_offset;
        pane = m_Parent->GetDlgItem(id);
        pane->GetWindowRect(&rcPane);
        m_Parent->ScreenToClient(&rcPane);
        rcPane.bottom = rcBar.top - offset;
        pane->MoveWindow(&rcPane, FALSE);
    }

    for (i=0; i<m_belowControls.GetSize(); i++) {
        id = m_belowControls.GetAt(i).m_ctrlId;
        offset = m_belowControls.GetAt(i).m_offset;
        pane = m_Parent->GetDlgItem(id);
        pane->GetWindowRect(&rcPane);
        m_Parent->ScreenToClient(&rcPane);
        rcPane.top = rcBar.bottom + offset;
        if (rcPane.top > rcPane.bottom)
        {
            rcPane.top = rcPane.bottom;
        }


        pane->MoveWindow(&rcPane, FALSE);
    }

    m_Parent->Invalidate();

    FUNCTION_EXIT;
}


BOOL CySplitterWnd::GetMouseClipRect(LPRECT rectClip, CPoint point)
{
    FUNCTION_ENTRY( "GetMouseClipRect" );

    RECT rectOrg, rectTarget, rectParent, rectPane;
    int i, topLimit, bottomLimit;
    DWORD id;

    GetWindowRect(&rectOrg);
    m_Parent->GetClientRect(&rectParent);
    m_Parent->ClientToScreen(&rectParent);

    if (m_highestCtrl < 0) {
        topLimit = rectParent.bottom;

        for (i=0; i<m_aboveControls.GetSize(); i++) {
            id = m_aboveControls.GetAt(i).m_ctrlId;
            m_Parent->GetDlgItem(id)->GetWindowRect(&rectPane);

            if (rectPane.top < topLimit) {
                topLimit = rectPane.top;
                m_highestCtrl = i;
            }
        }
    }
    else {
        id = m_aboveControls.GetAt(m_highestCtrl).m_ctrlId;
        m_Parent->GetDlgItem(id)->GetWindowRect(&rectPane);

        topLimit = rectPane.top;
    }

    if (m_lowestCtrl < 0) {
        bottomLimit = rectParent.top;

        for (i=0; i<m_belowControls.GetSize(); i++) {
            id = m_belowControls.GetAt(i).m_ctrlId;
            m_Parent->GetDlgItem(id)->GetWindowRect(&rectPane);

            if (rectPane.bottom > bottomLimit) {
                bottomLimit = rectPane.bottom;
                m_lowestCtrl = i;
            }
        }
    }
    else {
        id = m_belowControls.GetAt(m_lowestCtrl).m_ctrlId;
        m_Parent->GetDlgItem(id)->GetWindowRect(&rectPane);

        bottomLimit = rectPane.bottom;
    }

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


    // move the splitter bar & re-position the attached panes if necessary
    MoveWindow(rectCur, FALSE);
    RecalcLayout();

    // calculate the position of the splitter with respect to the top of the highest
    // control and the bottom of the lowest one
    RECT rectHighest, rectLowest;
    int highestCtrlId, lowestCtrlId;

    highestCtrlId = m_aboveControls.GetAt(m_highestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(highestCtrlId)->GetWindowRect(&rectHighest);
    m_Parent->ScreenToClient(&rectHighest);

    lowestCtrlId = m_belowControls.GetAt(m_lowestCtrl).m_ctrlId;
    m_Parent->GetDlgItem(lowestCtrlId)->GetWindowRect(&rectLowest);
    m_Parent->ScreenToClient(&rectLowest);

    int barCentre = rectCur.top + rectCur.Height() / 2;
    WORD above = (WORD)(barCentre - rectHighest.top);
    WORD below = (WORD)(rectLowest.bottom - barCentre);

    m_Parent->SendMessage(WM_SPLITTER_MOVED, MAKEWPARAM(below, above), GetDlgCtrlID());

    // CWnd::OnLButtonDown(nFlags, point);

    FUNCTION_EXIT;
}


