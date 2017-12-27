/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Ripple
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  *
  */

// ExtStatusControlBar.cpp : implementation file
//

#include "stdafx.h"
#include "core/utilities/src/DebugUtil.h"
#include "ExtStatusControlBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CExtStatusControlBar

CExtStatusControlBar::CExtStatusControlBar()
{
    FUNCTION_ENTRY( "CExtStatusControlBar" );
    FUNCTION_EXIT;
}


CExtStatusControlBar::~CExtStatusControlBar()
{
    FUNCTION_ENTRY( "~CExtStatusControlBar" );

    for ( int i = 0; i < m_arrPaneControls.GetSize(); i++ ){
        if( m_arrPaneControls[i]->hWnd && ::IsWindow(m_arrPaneControls[i]->hWnd) ) {
            ::ShowWindow(m_arrPaneControls[i]->hWnd, SW_HIDE);
            if( m_arrPaneControls[i]->bAutoDestroy ) {
                ::DestroyWindow(m_arrPaneControls[i]->hWnd);
            }
        }
        _STATUSBAR_PANE_CTRL_ *pPaneCtrl = m_arrPaneControls[i];
        if( pPaneCtrl )
        {
            delete pPaneCtrl;
        }

    }

    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CExtStatusControlBar, CStatusBar)
    // {{AFX_MSG_MAP(CExtStatusControlBar)
    ON_WM_CREATE()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CExtStatusControlBar message handlers
//////////////////////////////////////////////////////////////////////////

int CExtStatusControlBar::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
    FUNCTION_ENTRY( "OnCreate" );

    if( CStatusBar::OnCreate(lpCreateStruct) == -1 )
    {
        FUNCTION_EXIT;
        return -1;
    }

    FUNCTION_EXIT;
    return 0;
}


//////////////////////////////////////////////////////////////////////////

LRESULT CExtStatusControlBar::WindowProc(UINT message, WPARAM wParam, LPARAM lParam)
{
    FUNCTION_ENTRY( "WindowProc" );

    LRESULT lResult =CStatusBar::WindowProc(message, wParam, lParam);
    if( message == WM_SIZE ){
        RepositionControls();
    }

    FUNCTION_EXIT;
    return lResult;
}


//////////////////////////////////////////////////////////////////////////

void CExtStatusControlBar::RepositionControls()
{
    FUNCTION_ENTRY( "RepositionControls" );

    HDWP _hDWP = ::BeginDeferWindowPos( m_arrPaneControls.GetSize() );

    CRect rcClient;
    GetClientRect(&rcClient);
    for (int i = 0; i < m_arrPaneControls.GetSize(); i++ )
    {
        int   iIndex  = CommandToIndex(m_arrPaneControls[i]->nID);
        HWND hWnd    = m_arrPaneControls[i]->hWnd;

        CRect rcPane;
        GetItemRect(iIndex, &rcPane);

        // CStatusBar::GetItemRect() sometimes returns invalid size
        // of the last pane - we will re-compute it
        int cx = ::GetSystemMetrics( SM_CXEDGE );
        DWORD dwPaneStyle = GetPaneStyle( iIndex );
        if( iIndex == (m_nCount-1) )
        {
            if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
            {
                UINT nID, nStyle;
                int  cxWidth;
                GetPaneInfo( iIndex, nID, nStyle, cxWidth );
                rcPane.right = rcPane.left + cxWidth + cx*3;
            } // if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
            else
            {
                CRect rcClient;
                GetClientRect( &rcClient );
                rcPane.right = rcClient.right;
                if( (GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP )
                {
                    int cxSmIcon = ::GetSystemMetrics( SM_CXSMICON );
                    rcPane.right -= cxSmIcon + cx;
                } // if( (GetStyle() & SBARS_SIZEGRIP) == SBARS_SIZEGRIP )
            } // else from if( (dwPaneStyle & SBPS_STRETCH ) == 0 )
        } // if( iIndex == (m_nCount-1) )

        if ((GetPaneStyle (iIndex) & SBPS_NOBORDERS) == 0){
            rcPane.DeflateRect(cx,cx);
        }else{
            rcPane.DeflateRect(cx,1,cx,1);
        }

        if (hWnd && ::IsWindow(hWnd)){
            _hDWP = ::DeferWindowPos(
                _hDWP,
                hWnd,
                NULL,
                rcPane.left,
                rcPane.top,
                rcPane.Width(),
                rcPane.Height(),
                SWP_NOZORDER|SWP_NOOWNERZORDER|SWP_SHOWWINDOW
                );

            ::RedrawWindow(
                hWnd,
                NULL,
                NULL,
                RDW_INVALIDATE|RDW_UPDATENOW
                |RDW_ERASE|RDW_ERASENOW
                );

        } // if (hWnd && ::IsWindow(hWnd)){
    }

    VERIFY( ::EndDeferWindowPos( _hDWP ) );

    FUNCTION_EXIT;
}


;

//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::AddPane(
     UINT nID,    // ID of the  pane
     int nIndex    // index of the pane
     )
{
    FUNCTION_ENTRY( "AddPane" );

    if (nIndex < 0 || nIndex > m_nCount){
        ASSERT(FALSE);

        FUNCTION_EXIT;
        return FALSE;
    }

    if (CommandToIndex(nID) != -1){
        ASSERT(FALSE);

        FUNCTION_EXIT;
        return FALSE;
    }

    CArray<_STATUSBAR_PANE_*,_STATUSBAR_PANE_*> arrPanesTmp;
    int iIndex = 0;
    for (iIndex = 0; iIndex < m_nCount+1; iIndex++)
    {
        _STATUSBAR_PANE_* pNewPane = new _STATUSBAR_PANE_;

        if (iIndex == nIndex){
            pNewPane->nID    = nID;
            pNewPane->nStyle = SBPS_NORMAL;
        }else{
            int idx = iIndex;
            if (iIndex > nIndex)
            {
                idx--;
            }


            _STATUSBAR_PANE_* pOldPane  = GetPanePtr(idx);
            pNewPane->cxText  = pOldPane->cxText;
            pNewPane->nFlags  = pOldPane->nFlags;
            pNewPane->nID     = pOldPane->nID;
            pNewPane->nStyle  = pOldPane->nStyle;
            pNewPane->strText = pOldPane->strText;
        }
        arrPanesTmp.Add(pNewPane);
    }

    int nPanesCount = arrPanesTmp.GetSize();
    UINT* lpIDArray = new UINT[ nPanesCount ];
    for (iIndex = 0; iIndex < nPanesCount; iIndex++) {
        lpIDArray[iIndex] = arrPanesTmp[iIndex]->nID;
    }

    // set the indicators
    SetIndicators(lpIDArray, nPanesCount);
    // free memory
    for (iIndex = 0; iIndex < nPanesCount; iIndex++){
        _STATUSBAR_PANE_* pPane = arrPanesTmp[iIndex];
        if (iIndex != nIndex)
        {
            PaneInfoSet(iIndex, pPane);
        }

        if(pPane)
        {
            delete pPane;
        }

    }

    arrPanesTmp.RemoveAll();
    if(lpIDArray)
    {
        delete []lpIDArray;
    }


    RepositionControls();

    FUNCTION_EXIT;
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::RemovePane(
    UINT nID    // ID of the pane
    )
{
    FUNCTION_ENTRY( "RemovePane" );

    if ( CommandToIndex(nID) == -1 || m_nCount == 1 ){
        ASSERT(FALSE);

        FUNCTION_EXIT;
        return FALSE;
    }

    CArray<_STATUSBAR_PANE_*,_STATUSBAR_PANE_*> arrPanesTmp;
    int nIndex;
    for (nIndex = 0; nIndex < m_nCount; nIndex++)
    {
        _STATUSBAR_PANE_* pOldPane = GetPanePtr(nIndex);

        if (pOldPane->nID == nID)
        {
            continue;
        }


        _STATUSBAR_PANE_* pNewPane = new _STATUSBAR_PANE_;

        pNewPane->cxText  = pOldPane->cxText;
        pNewPane->nFlags  = pOldPane->nFlags;
        pNewPane->nID     = pOldPane->nID;
        pNewPane->nStyle  = pOldPane->nStyle;
        pNewPane->strText = pOldPane->strText;
        arrPanesTmp.Add(pNewPane);
    }

    UINT* lpIDArray = new UINT[arrPanesTmp.GetSize()];
    for (nIndex = 0; nIndex < arrPanesTmp.GetSize(); nIndex++) {
        lpIDArray[nIndex] = arrPanesTmp[nIndex]->nID;
    }

    // set the indicators
    SetIndicators(lpIDArray, arrPanesTmp.GetSize());
    // free memory
    for (nIndex = 0; nIndex < arrPanesTmp.GetSize(); nIndex++){
        _STATUSBAR_PANE_* pPane = arrPanesTmp[nIndex];
        PaneInfoSet(nIndex, pPane);
        if(pPane)
        {
            delete pPane;
        }

    }

    for ( int i = 0; i < m_arrPaneControls.GetSize(); i++ ){
        if (m_arrPaneControls[i]->nID == nID){
            if( m_arrPaneControls[i]->hWnd && ::IsWindow(m_arrPaneControls[i]->hWnd) ) {
                ::ShowWindow(m_arrPaneControls[i]->hWnd, SW_HIDE);
                if( m_arrPaneControls[i]->bAutoDestroy ) {
                    ::DestroyWindow(m_arrPaneControls[i]->hWnd);
                }
            }
            _STATUSBAR_PANE_CTRL_ *pPaneCtrl = m_arrPaneControls[i];
            if( pPaneCtrl )
            {
                delete pPaneCtrl;
            }

            m_arrPaneControls.RemoveAt(i);
            break;
        }
    }

    arrPanesTmp.RemoveAll();
    if(lpIDArray)
    {
        delete []lpIDArray;
    }


    RepositionControls();

    FUNCTION_EXIT;
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::AddPaneControl(HWND hWnd, UINT nID, BOOL bAutoDestroy)
{
    FUNCTION_ENTRY( "AddPaneControl" );

    if (CommandToIndex (nID) == -1) {
        FUNCTION_EXIT;
        return FALSE;
    }

    _STATUSBAR_PANE_CTRL_* pPaneCtrl = new _STATUSBAR_PANE_CTRL_;
    pPaneCtrl->nID         = nID;
    pPaneCtrl->hWnd        = hWnd;
    pPaneCtrl->bAutoDestroy = bAutoDestroy;

    m_arrPaneControls.Add(pPaneCtrl);

    RepositionControls();

    FUNCTION_EXIT;
    return TRUE;
}


//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::PaneInfoGet(int nIndex, _STATUSBAR_PANE_* pPane)
{
    FUNCTION_ENTRY( "PaneInfoGet" );

    if( nIndex < m_nCount  && nIndex >= 0 )
    {
        GetPaneInfo( nIndex,  pPane->nID, pPane->nStyle, pPane->cxText );
        CString strPaneText;
        GetPaneText( nIndex , strPaneText );
        pPane->strText = LPCTSTR(strPaneText);

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


//////////////////////////////////////////////////////////////////////////

BOOL CExtStatusControlBar::PaneInfoSet(int nIndex, _STATUSBAR_PANE_* pPane)
{
    FUNCTION_ENTRY( "PaneInfoSet" );

    if( nIndex < m_nCount  && nIndex >= 0 ){
        SetPaneInfo( nIndex, pPane->nID, pPane->nStyle, pPane->cxText );
        SetPaneText( nIndex, LPCTSTR( pPane->strText) );

        FUNCTION_EXIT;
        return TRUE;
    }

    FUNCTION_EXIT;
    return FALSE;
}


