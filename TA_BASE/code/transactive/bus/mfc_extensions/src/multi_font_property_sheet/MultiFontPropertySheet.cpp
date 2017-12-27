// CMultiFontPropertySheet.cpp : implementation file
//

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.cpp $
  * @author:  Rob Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/09/27 10:01:34 $
  * Last modified by:  $Author: hoa.le $
  * 
  * This derivation of CPropertySheet provides a workaround for two MFC CPropertySheet Bugs:
  *
  * 1) Resetting/resizing of the CPropertyPages when not of system font. 
  * REFER to MSDN Knowledge Base Article 142170 - "PRPFONT - How to Set CPropertySheet Fonts"
  *
  * 2) Child CPropertySheet Hangs If Focus Is Switched. 
  * Refer to MSDN Knowledge Base Article 149501 - "PRB: Child CPropertySheet Hangs If Focus Is Switched"
  *
  * To use, simply substitute CMultiFontPropertySheet wherever you would use CPropertySheet.
  *
  */

#include "bus/mfc_extensions/src/stdafx.h"
#include "bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h"
#include <afxpriv.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define WM_RESIZEPAGE WM_APP+1

// For some reason this is the messageID CPropertySheet sends
// when a new page is Added. This is our only way to detect in the subclass
// when "CPropertySheet::AddPage" is called (as its not virtual). We need to 
// handle this message but still pass down to DefWindowProc so the 
// underlying control can do whatever it needs to when a page is added.
#define WM_PAGEADDED  WM_USER+103
// For some reason this is the messageID CPropertySheet sends
// when a page is Removed. This is our only way to detect in the subclass
// when "CPropertySheet::AddPage" is called (as its not virtual). We need to 
// handle this message but still pass down to DefWindowProc so the 
// underlying control can do whatever it needs to when a page is removed.
#define WM_PAGEREMOVED  WM_USER+102  

enum { CDF_CENTER, CDF_TOPLEFT, CDF_NONE };

// helper function which sets the font for a window and all its children
// and also resizes everything according to the new font
void ChangeDialogFont(CWnd* pWnd, CFont* pFont, int nFlag)
{
	CRect windowRect;

	// grab old and new text metrics
	TEXTMETRIC tmOld, tmNew;
	CDC * pDC = pWnd->GetDC();
	CFont * pSavedFont = pDC->SelectObject(pWnd->GetFont());
	pDC->GetTextMetrics(&tmOld);
	pDC->SelectObject(pFont);
	pDC->GetTextMetrics(&tmNew);
	pDC->SelectObject(pSavedFont);
	pWnd->ReleaseDC(pDC);

	long oldHeight = tmOld.tmHeight+tmOld.tmExternalLeading;
	long newHeight = tmNew.tmHeight+tmNew.tmExternalLeading;

	if (nFlag != CDF_NONE)
	{
		// calculate new dialog window rectangle
		CRect clientRect, newClientRect, newWindowRect;

		pWnd->GetWindowRect(windowRect);
		pWnd->GetClientRect(clientRect);
		long xDiff = windowRect.Width() - clientRect.Width();
		long yDiff = windowRect.Height() - clientRect.Height();
	
		newClientRect.left = newClientRect.top = 0;
		newClientRect.right = clientRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		newClientRect.bottom = clientRect.bottom * newHeight / oldHeight;

		if (nFlag == CDF_TOPLEFT) // resize with origin at top/left of window
		{
			newWindowRect.left = windowRect.left;
			newWindowRect.top = windowRect.top;
			newWindowRect.right = windowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = windowRect.top + newClientRect.bottom + yDiff;
		}
		else if (nFlag == CDF_CENTER) // resize with origin at center of window
		{
			newWindowRect.left = windowRect.left - 
							(newClientRect.right - clientRect.right)/2;
			newWindowRect.top = windowRect.top -
							(newClientRect.bottom - clientRect.bottom)/2;
			newWindowRect.right = newWindowRect.left + newClientRect.right + xDiff;
			newWindowRect.bottom = newWindowRect.top + newClientRect.bottom + yDiff;
		}
		pWnd->MoveWindow(newWindowRect);
	}

	pWnd->SetFont(pFont);

	// iterate through and move all child windows and change their font.
	CWnd* pChildWnd = pWnd->GetWindow(GW_CHILD);

	while (pChildWnd)
	{
		pChildWnd->SetFont(pFont);
		pChildWnd->GetWindowRect(windowRect);

		CString strClass;
		::GetClassName(pChildWnd->m_hWnd, strClass.GetBufferSetLength(32), 31);
		strClass.MakeUpper();
		if(strClass==_T("COMBOBOX"))
		{
			CRect rect;
			pChildWnd->SendMessage(CB_GETDROPPEDCONTROLRECT,0,(LPARAM) &rect);
			windowRect.right = rect.right;
			windowRect.bottom = rect.bottom;
		}

		pWnd->ScreenToClient(windowRect);
		windowRect.left = windowRect.left * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.right = windowRect.right * tmNew.tmAveCharWidth / tmOld.tmAveCharWidth;
		windowRect.top = windowRect.top * newHeight / oldHeight;
		windowRect.bottom = windowRect.bottom * newHeight / oldHeight;
		pChildWnd->MoveWindow(windowRect);
		
		pChildWnd = pChildWnd->GetWindow(GW_HWNDNEXT);
	}
}
/////////////////////////////////////////////////////////////////////////////
// CMultiFontPropertySheet

IMPLEMENT_DYNAMIC(CMultiFontPropertySheet, CPropertySheet)

CMultiFontPropertySheet::CMultiFontPropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, BOOL fPageFont)
	:CPropertySheet(nIDCaption, pParentWnd, iSelectPage), m_isModal(false), m_fPageFont(fPageFont)
{
}

CMultiFontPropertySheet::CMultiFontPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, BOOL fPageFont)
	:CPropertySheet(pszCaption, pParentWnd, iSelectPage), m_isModal(false), m_fPageFont(fPageFont)
{
}

CMultiFontPropertySheet::~CMultiFontPropertySheet()
{
	if (m_fntPage.m_hObject)
		VERIFY (m_fntPage.DeleteObject ());
}

BEGIN_MESSAGE_MAP(CMultiFontPropertySheet, CPropertySheet)
	//{{AFX_MSG_MAP(CMultiFontPropertySheet)
	//}}AFX_MSG_MAP
	ON_MESSAGE (WM_RESIZEPAGE, OnResizePage)
	ON_MESSAGE (WM_PAGEADDED, OnPageAdded)
	ON_MESSAGE (WM_PAGEREMOVED, OnPageRemoved)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CMultiFontPropertySheet message handlers

void CMultiFontPropertySheet::BuildPropPageArray()
{
	CPropertySheet::BuildPropPageArray();

	// get first page
	CPropertyPage* pPage = GetPage (0);
	ASSERT (pPage);
	
	// dialog template class in afxpriv.h
	CDialogTemplate dlgtemp;
	// load the dialog template
	VERIFY (dlgtemp.Load (pPage->m_psp.pszTemplate));
	// get the font information
	CString strFace;
	WORD	wSize;
	VERIFY (dlgtemp.GetFont (strFace, wSize));
	// Fix issue with default system font set in Property Page
	if (m_fPageFont)
	{
		if((wSize > 0) && !strFace.IsEmpty())
		{
			LPCPROPSHEETPAGE ppsp = m_psh.ppsp;
			const int nSize = static_cast<int>(m_pages.GetSize());

			for(int nPage = 0; nPage < nSize; nPage++)
			{
				const DLGTEMPLATE* pResource = ppsp->pResource;
				CDialogTemplate dlgTemplate(pResource);
				dlgTemplate.SetFont(strFace, wSize);
				memmove((void*)pResource, dlgTemplate.m_hTemplate, dlgTemplate.m_dwTemplateSize);

				(BYTE*&)ppsp += ppsp->dwSize;
			}
		}
	}
	else
	{
		if (m_fntPage.m_hObject)
			VERIFY (m_fntPage.DeleteObject ());
		// create a font using the info from first page
		VERIFY (m_fntPage.CreatePointFont (wSize*10, strFace));
	}
}

BOOL CMultiFontPropertySheet::OnInitDialog() 
{
	// Prevent Child CPropertySheet Hanging If Focus Is Switched 
	// ( MSDN Knowledge Base Article 149501 )
	ModifyStyleEx (0, WS_EX_CONTROLPARENT);

	if (m_fPageFont)
	{
		return CPropertySheet::OnInitDialog();
	}

	// The following workaround prevents the Property Sheet Tab Control being the wrong
	// size if the Property Sheet is sized more narrow than the minimum required 
	// by the modal instantiations's button array.
	if (false == m_isModal)
	{
		HWND hWnd = (HWND)::GetDlgItem(m_hWnd, AFX_IDC_TAB_CONTROL);
		CRect rectOld;
		::GetWindowRect(hWnd, &rectOld);
		ScreenToClient(rectOld);
		int nCx = rectOld.Width();
		int nCy = rectOld.Height();
		// move tab control
		::SetWindowPos(hWnd, NULL, 0, 0, 0, 0,
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
		GetWindowRect(&rectOld);
		rectOld.right -= nCx;
		rectOld.bottom -= nCy;
		SetWindowPos(NULL, 0, 0, rectOld.Width(), rectOld.Height(),
			SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE);
	}

	CPropertySheet::OnInitDialog();

	// The following resizes all pages of the Property Sheet to
	// correct font.

	// get the font for the first active page
	CPropertyPage* pPage = GetActivePage ();
	ASSERT (pPage);

	// change the font for the sheet
	ChangeDialogFont (this, &m_fntPage, CDF_CENTER);
	// change the font for each page
	for (int iCntr = 0; iCntr < GetPageCount (); iCntr++)
	{
		VERIFY (SetActivePage (iCntr));
		CPropertyPage* pPage = GetActivePage ();
		ASSERT (pPage);
		ChangeDialogFont (pPage, &m_fntPage, CDF_CENTER);
	}

	VERIFY (SetActivePage (pPage));

	// set and save the size of the page
	CTabCtrl* pTab = GetTabControl ();
	ASSERT (pTab);

	if (m_psh.dwFlags & PSH_WIZARD)
	{
		pTab->ShowWindow (SW_HIDE);
		GetClientRect (&m_rctPage);

		CWnd* pButton = GetDlgItem (ID_WIZBACK);
		ASSERT (pButton);
		CRect rc;
		pButton->GetWindowRect (&rc);
		ScreenToClient (&rc);
		m_rctPage.bottom = rc.top-2;
	}
	else
	{
		pTab->GetWindowRect (&m_rctPage);
		ScreenToClient (&m_rctPage);
		pTab->AdjustRect (FALSE, &m_rctPage);
	}

	// resize the page	
	pPage->MoveWindow (&m_rctPage);

	return TRUE;
}

BOOL CMultiFontPropertySheet::OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult) 
{
	if (!m_fPageFont)
	{
		NMHDR* pnmh = (LPNMHDR) lParam;

		// the sheet resizes the page whenever it is activated so we need to size it correctly
		if (TCN_SELCHANGE == pnmh->code)
		{
			int actIndex = GetActiveIndex();
			CPropertyPage* pPage = GetPage( actIndex );
			ASSERT (pPage);
			if ( pPage->GetSafeHwnd() == NULL)
			{
				PostMessage (WM_RESIZEPAGE, static_cast<WPARAM>(TRUE), 0); // WPARAM = TRUE to indicate change font is needed as page newly added
			}
			else
			{
				PostMessage (WM_RESIZEPAGE, static_cast<WPARAM>(FALSE), 0); // WPARAM = FALSE to indicate no change font needed as page not newly added
			}
		}
	}
	return CPropertySheet::OnNotify(wParam, lParam, pResult);
}

LONG CMultiFontPropertySheet::OnResizePage (WPARAM wParam, LPARAM lParam)
{
	BOOL changeFontRequired = static_cast<BOOL>(wParam);
	// resize the page
	CPropertyPage* pPage = GetActivePage ();
	ASSERT (pPage);
	if ( changeFontRequired )
	{
		ChangeDialogFont (pPage, &m_fntPage, CDF_CENTER);
	}
	pPage->MoveWindow (&m_rctPage);

	return 0;
}

BOOL CMultiFontPropertySheet::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	// the sheet resizes the page whenever the Apply button is clicked so we need to size it correctly
	if (ID_APPLY_NOW == wParam ||
		ID_WIZNEXT == wParam ||
		ID_WIZBACK == wParam)
		PostMessage (WM_RESIZEPAGE);
	
	return CPropertySheet::OnCommand(wParam, lParam);
}

// there might be a more elegant way to determine this but I don't know it.
int CMultiFontPropertySheet::DoModal() 
{
	m_isModal = true;
	
	return CPropertySheet::DoModal();
}

// This is a workaround so we know when a page is added after Create has already been
// called.
// The WM_PAGEADDED msg must be propagated down to DefWindowProc so as not to prevent
// the underlying control being notified of a page being added also (else AddPage won't work after
// this sheet has been created).
LRESULT CMultiFontPropertySheet::OnPageAdded (WPARAM wParam, LPARAM lParam)
{
	LRESULT retVal = DefWindowProc(WM_PAGEADDED, wParam, lParam);

	// recheck size for page to adjust if Tab Control is now stacked
	// (ie. with extra stack)
	CTabCtrl* pTab = GetTabControl ();
	ASSERT (pTab);

	if (m_psh.dwFlags & PSH_WIZARD)
	{
		pTab->ShowWindow (SW_HIDE);
		GetClientRect (&m_rctPage);

		CWnd* pButton = GetDlgItem (ID_WIZBACK);
		ASSERT (pButton);
		CRect rc;
		pButton->GetWindowRect (&rc);
		ScreenToClient (&rc);
		m_rctPage.bottom = rc.top-2;
	}
	else
	{
		pTab->GetWindowRect (&m_rctPage);
		ScreenToClient (&m_rctPage);
		pTab->AdjustRect (FALSE, &m_rctPage);
	}

	return retVal;
}


// This is a workaround so we know when a page is removed after Create has already been
// called.
// The WM_PAGEADDED msg must be propagated down to DefWindowProc so as not to prevent
// the underlying control being notified of a page being added also (else AddPage won't work after
// this sheet has been created).
LRESULT CMultiFontPropertySheet::OnPageRemoved(WPARAM wParam, LPARAM lParam)
{
	LRESULT retVal = DefWindowProc(WM_PAGEREMOVED, wParam, lParam);

	// recheck size for page to adjust if Tab Control is now stacked
	// (ie. with extra stack)
	CTabCtrl* pTab = GetTabControl ();
	ASSERT (pTab);

	if (m_psh.dwFlags & PSH_WIZARD)
	{
		pTab->ShowWindow (SW_HIDE);
		GetClientRect (&m_rctPage);

		CWnd* pButton = GetDlgItem (ID_WIZBACK);
		ASSERT (pButton);
		CRect rc;
		pButton->GetWindowRect (&rc);
		ScreenToClient (&rc);
		m_rctPage.bottom = rc.top-2;
	}
	else
	{
		pTab->GetWindowRect (&m_rctPage);
		ScreenToClient (&m_rctPage);
		pTab->AdjustRect (FALSE, &m_rctPage);
	}

	return retVal;
}