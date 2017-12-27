/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionManagerPropertySheet.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class works around a shortcoming in CMultiFontPropertySheet which causes 
  * pages to be resized to their original dimensions upon activation.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using namespace TA_Base_App;


/////////////////////////////////////////////////////////////////////////////
// RegionManagerPropertySheet

IMPLEMENT_DYNAMIC(RegionManagerPropertySheet, CMultiFontPropertySheet)

RegionManagerPropertySheet::RegionManagerPropertySheet() :
	CMultiFontPropertySheet("Region Manager PS", NULL, 0)
{
}


RegionManagerPropertySheet::~RegionManagerPropertySheet()
{
}




/////////////////////////////////////////////////////////////////////////////
// RegionManagerPropertySheet message handlers

BEGIN_MESSAGE_MAP(RegionManagerPropertySheet, CMultiFontPropertySheet)
	//{{AFX_MSG_MAP(RegionManagerPropertySheet)
	ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


void RegionManagerPropertySheet::OnSize(UINT nType, int cx, int cy) 
{
	CMultiFontPropertySheet::OnSize(nType, cx, cy);
	
	// TODO: Add your message handler code here
    if (GetActivePage() != NULL && ::IsWindow(GetActivePage()->GetSafeHwnd()))
    {
        CTabCtrl* tabCtrl = GetTabControl();
        CRect tabCtrlRect;

        tabCtrl->GetWindowRect(tabCtrlRect);
        ScreenToClient(tabCtrlRect);
        tabCtrlRect.right = cx;
        tabCtrlRect.bottom = cy;
        tabCtrl->MoveWindow(tabCtrlRect);
        tabCtrl->AdjustRect(FALSE, tabCtrlRect);

        // CMultiFontPropertySheet::m_rctPage must be reset, otherwise, as pages are activated,
        // their size will be changed back to whatever it was originally.
        m_rctPage = tabCtrlRect;  

        // Reposition the pages and tell them to resize. This will adjust the sizes and positions of individual controls.
        for (int i = 0; i < GetPageCount(); i++)
        {
            CPropertyPage* page = GetPage(i);

            if (page != NULL && ::IsWindow(page->GetSafeHwnd()))
            {
                page->SetWindowPos(NULL, tabCtrlRect.left, tabCtrlRect.top, tabCtrlRect.Width(), tabCtrlRect.Height(), SWP_NOZORDER);
            }
        }
    }
}
