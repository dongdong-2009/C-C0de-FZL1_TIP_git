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

#include "StdAfx.h"

#include "core/utilities/src/DebugUtil.h"
#include "BlankView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// CBlankView

IMPLEMENT_DYNCREATE(CBlankView, CView)

CBlankView::CBlankView()
{
    FUNCTION_ENTRY( "CBlankView" );

    // {{AFX_DATA_INIT(CBlankView)
        // NOTE: the ClassWizard will add member initialization here
    // }}AFX_DATA_INIT

    FUNCTION_EXIT;
}


CBlankView::~CBlankView()
{
    FUNCTION_ENTRY( "~CBlankView" );
    FUNCTION_EXIT;
}


BEGIN_MESSAGE_MAP(CBlankView, CView)
    // {{AFX_MSG_MAP(CBlankView)
    ON_WM_ERASEBKGND()
    // }}AFX_MSG_MAP
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CBlankView diagnostics

#ifdef _DEBUG
void CBlankView::AssertValid() const
{
    FUNCTION_ENTRY( "AssertValid" );

    CView::AssertValid();

    FUNCTION_EXIT;
}


void CBlankView::Dump(CDumpContext& dc) const
{
    FUNCTION_ENTRY( "Dump" );

    CView::Dump(dc);

    FUNCTION_EXIT;
}


#endif //_DEBUG


/////////////////////////////////////////////////////////////////////////////
// CBlankView message handlers

void CBlankView::OnDraw(CDC* pDC)
{
    FUNCTION_ENTRY( "OnDraw" );
    FUNCTION_EXIT;
}


BOOL CBlankView::OnEraseBkgnd(CDC* pDC)
{
    FUNCTION_ENTRY( "OnEraseBkgnd" );

    // TODO: Add your message handler code here and/or call default

    CBrush backBrush;
    backBrush.CreateSysColorBrush(COLOR_BTNFACE);

    CBrush* oldBrush = pDC->SelectObject(&backBrush);

    CRect clipBox;
    pDC->GetClipBox(clipBox);
    pDC->PatBlt(clipBox.left, clipBox.top, clipBox.Width(), clipBox.Height(), PATCOPY);
    pDC->SelectObject(oldBrush);

    FUNCTION_EXIT;
    return TRUE;
}


