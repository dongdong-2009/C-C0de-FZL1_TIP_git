// ColoredStatusBarCtrl.cpp : implementation file
//

#include "..\stdafx.h"
//#include "dlgviewtest.h"
#include "ColoredStatusBarCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// ColoredStatusBarCtrl

ColoredStatusBarCtrl::ColoredStatusBarCtrl()
{
	m_crText = GetSysColor(COLOR_MENUTEXT);
	m_sText.Empty();
}

ColoredStatusBarCtrl::~ColoredStatusBarCtrl()
{
}


BEGIN_MESSAGE_MAP(ColoredStatusBarCtrl, CStatusBar)
	//{{AFX_MSG_MAP(ColoredStatusBarCtrl)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// ColoredStatusBarCtrl message handlers

void ColoredStatusBarCtrl::DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct)
{
	// Attach to a CDC object
	CDC dc;
	dc.Attach(lpDrawItemStruct->hDC);

	dc.SetBkMode(TRANSPARENT);

	// Get the pane rectangle and calculate text coordinates
	CRect rect(&lpDrawItemStruct->rcItem);

	switch(lpDrawItemStruct->itemID) {

		case 0:
			dc.SetTextColor(m_crText);
			dc.TextOut(rect.left+2, rect.top, m_sText);
			break;
		default:
			dc.TextOut(rect.left+2, rect.top, GetPaneText(lpDrawItemStruct->itemID));
	}

	// Detach from the CDC object, otherwise the hDC will be
	// destroyed when the CDC object goes out of scope
	dc.Detach();
}

BOOL ColoredStatusBarCtrl::SetPaneText( int nIndex, LPCTSTR lpszNewText, BOOL bUpdate, COLORREF crText)
{
	m_crText = crText;

	if(nIndex)
		return CStatusBar::SetPaneText(nIndex, lpszNewText, bUpdate);
	else
	{
		m_sText = lpszNewText;
		return GetStatusBarCtrl().SetText(lpszNewText, 0, SBT_OWNERDRAW|SBT_NOBORDERS);
	}
}