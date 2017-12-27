/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/StackedPage.cpp $
  * @author:  Zoran M.Todorovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * TStackedPage class defintion
  */

#include "stdafx.h"

#include "bus/banner_framework/src/StackedPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    BEGIN_MESSAGE_MAP(TStackedPage, CDialog)
	    //{{AFX_MSG_MAP(TStackedPage)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    TStackedPage::TStackedPage(LPCTSTR lpszTemplateName, CWnd* pParent)
                       :CDialog(lpszTemplateName,pParent)
    {
    }

    TStackedPage::TStackedPage(UINT nIDTemplate, CWnd* pParent)
                       :CDialog(nIDTemplate,pParent)
    {
    }

    TStackedPage::~TStackedPage()
    {
    }

    void TStackedPage::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(TStackedPage)
	    //}}AFX_DATA_MAP
    }

    void TStackedPage::OnOK()
    {
    }

    void TStackedPage::OnCancel()
    {
    }

    BOOL TStackedPage::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

      DWORD style = GetStyle();
      ASSERT((style & WS_CHILD) != 0);
      ASSERT((style & WS_BORDER) == 0);
      ASSERT((style & WS_DISABLED) != 0);

      return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    // Called when a page is displayed
    void TStackedPage::OnSetActive(void)
    {
    }

    // Called before a page becomes invisible
    void TStackedPage::OnKillActive(void)
    {
    }

    // Called after the page is created
    void TStackedPage::OnCreatePage(void)
    {
    }

    // Called before the page is destroyed
    void TStackedPage::OnDestroyPage(void)
    {
    }
}
