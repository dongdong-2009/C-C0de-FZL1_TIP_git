/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/StackDialog.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
/*#############################################################################
# STACKDIALOG.CPP
#
# SCA Software International S.A.
# http://www.scasoftware.com
# scaadmin@scasoftware.com
#
# Copyright (c) 1999 SCA Software International S.A.
#
# Date: 17.12.1999.
# Author: Zoran M.Todorovic
#
# This software is provided "AS IS", without a warranty of any kind.
# You are free to use/modify this code but leave this header intact.
# 
#############################################################################*/

#include "stdafx.h"

#include "bus/banner_framework/src/StackDialog.h"
#include "bus/banner_framework/src/StackedPage.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_Bus
{
    BEGIN_MESSAGE_MAP(TStackedDialog, TransActiveDialog)
	    //{{AFX_MSG_MAP(TStackedDialog)
	    ON_WM_DESTROY()
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    TStackedDialog::TStackedDialog(TA_Base_Bus::IGUIAccess& gui,
                                   UINT nPlaceholder, UINT nIDTemplate, 
                                   CWnd* pParent)
    :
    TransActiveDialog(gui,nIDTemplate,pParent)
    {
        init(nPlaceholder);
    }


    TStackedDialog::~TStackedDialog()
    {
    }


    void TStackedDialog::init(UINT placeholder)
    {
      m_nPlaceholderID = placeholder;
    }


    void TStackedDialog::flush(void)
    {
        TStackedPageHelper *pPage;
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            pPage = &(*it);
            destroyPageDialog(pPage);
        }
        m_PageList.clear();
    }


    void TStackedDialog::destroyPageDialog(TStackedPageHelper *pPage)
    {
        if ( pPage->m_bCreated )
        {
            pPage->m_bCreated = FALSE;
            ASSERT(pPage->m_pDialog != NULL);
            pPage->m_pDialog->OnKillActive();
            pPage->m_pDialog->OnDestroyPage();
            pPage->m_pDialog->DestroyWindow();
            delete pPage->m_pDialog;
            pPage->m_pDialog = NULL;
        }
    }


    TStackedPageHelper *TStackedDialog::getFirst(void)
    {
        TStackedPageHelperList::iterator it = m_PageList.begin();
        if (it != m_PageList.end())
        {
            return &(*it);
        }
        
        return NULL;
    }


    TStackedPageHelper *TStackedDialog::getCurrent(void)
    {
        TStackedPageHelper *pPage = NULL;
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if (it->m_bActive)
            {
                pPage = &(*it);
                break;
            }
        }
        
        return pPage;
    }


    TStackedPageHelper *TStackedDialog::getNext(void)
    {
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if (it->m_bActive)
            {
                ++it;
                if (it != m_PageList.end())
                {
                    return &(*it);
                }
                
                return getFirst();
            }
        }

        return getFirst();
    }


    void TStackedDialog::activatePage(TStackedPageHelper *pPage)
    {
        //TD14522 - whenever one of these conditions fail, the application
        // should not raise an ASSERT. However, if ever a Page is activated
        // without having fully initialized, the dialog should not even
        // attempt to show it  until all pages are initialized. 
        //ASSERT(pPage != NULL);
        //ASSERT(pPage->m_pDialog != NULL);
        //ASSERT(pPage->m_bCreated != FALSE);
        if (pPage != NULL && ((pPage->m_pDialog != NULL) && (pPage->m_bCreated == TRUE)))
        {
            TStackedPage *pDialog = pPage->m_pDialog;
            // Show and update window
            pDialog->ShowWindow(SW_SHOW);
            pDialog->InvalidateRect(NULL);
            pDialog->UpdateWindow();
            pDialog->OnSetActive();
            pPage->m_bActive = TRUE;
            OnPageChanged(pPage->m_nDialogID, TRUE);
        }
    }


    void TStackedDialog::deactivatePage(TStackedPageHelper *pPage)
    {
        ASSERT(pPage != NULL);
        ASSERT(pPage->m_pDialog != NULL);
        ASSERT(pPage->m_bCreated != FALSE);
        TStackedPage *pDialog = pPage->m_pDialog;
        pDialog->OnKillActive();
        pDialog->ShowWindow(SW_HIDE);
        pPage->m_bActive = FALSE;
        OnPageChanged(pPage->m_nDialogID, FALSE);
    }


    BOOL TStackedDialog::OnInitDialog() 
    {
        TransActiveDialog::OnInitDialog();
        ModifyStyleEx (0, WS_EX_CONTROLPARENT);	
        return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE

    }

    void TStackedDialog::DoDataExchange(CDataExchange* pDX)
    {
	    TransActiveDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(TStackedDialog)
		    // NOTE: the ClassWizard will add DDX and DDV calls here
	    //}}AFX_DATA_MAP
    }


    void TStackedDialog::OnDestroy() 
    {
        flush();
        TransActiveDialog::OnDestroy();
    }


    // Add a new page (associated child dialog with resource nId) to the 
    // stacked dialog. Returns a pointer to the internal helper object that 
    // maintains the state of the page
    TStackedPageHelper *TStackedDialog::AddPage(UINT nId, DWORD dwHelpId, DWORD dwData1, DWORD dwData2) 
    {
        TStackedPageHelper page;
        page.m_nDialogID = nId;
        page.m_dwHelpID = dwHelpId;
        page.m_dwData1 = dwData1;
        page.m_dwData2 = dwData2;

        TStackedPageHelperList::iterator it;
        it = m_PageList.insert(m_PageList.end(), page);
        TStackedPageHelper *pPage = &(*it);

        pPage->m_pParent = this;

        pPage->m_pDialog = CreatePage(pPage->m_nDialogID);
        ASSERT(pPage->m_pDialog != NULL);
        pPage->m_pDialog->Create(pPage->m_nDialogID,this);
        ASSERT(IsWindow(pPage->m_pDialog->m_hWnd) != FALSE);

        CRect rect;
        CWnd *pWnd = GetDlgItem(m_nPlaceholderID);
        ASSERT(pWnd != NULL);
        ASSERT(IsWindow(pWnd->m_hWnd) != FALSE);
        pWnd->GetWindowRect(&rect);
        ScreenToClient(&rect);
        pPage->m_pDialog->SetWindowPos(NULL, rect.left, rect.top, 0, 0, SWP_NOZORDER | SWP_NOSIZE | SWP_NOACTIVATE );
        pPage->m_pDialog->EnableWindow(TRUE);
        pPage->m_pDialog->OnCreatePage();
        pPage->m_bCreated = TRUE;

        return pPage;
    }


    // Delete page identified with a pointer to its helper object
    void TStackedDialog::DelPage(TStackedPageHelper *pPage)
    {
        ASSERT(pPage != NULL);
        destroyPageDialog(pPage);
        
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if (pPage == &(*it))
            {
                m_PageList.erase(it);
                break;
            }
        }
    }


    // Activate no page (actually deactivates the active page)
    void TStackedDialog::SetPage(void)
    {
        TStackedPageHelper *pOldPage = NULL;
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if ((pOldPage == NULL) && it->m_bActive)
            {
                pOldPage = &(*it);
            }
        }

        if ( NULL != pOldPage )
        {
            deactivatePage(pOldPage);
        }
    }


    // Activate the page with the specified child dialog resource
    void TStackedDialog::SetPage(UINT nId)
    {
        TStackedPageHelper *pOldPage = NULL, *pNewPage = NULL;

        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if ((pNewPage == NULL) && (it->m_nDialogID == nId))
            {
                pNewPage = &(*it);
            }
            
            if ((pOldPage == NULL) && it->m_bActive)
            {
                pOldPage = &(*it);
            }
        }
        
        if (pOldPage)
        {
            deactivatePage(pOldPage);
        }
        
        if (pNewPage)
        {
            activatePage(pNewPage);
        }
    }


    // Set first page as active page
    void TStackedDialog::SetFirstPage(void)
    {
        TStackedPageHelper *pPage = getFirst();
        if (pPage)
        {
            SetPage(pPage->m_nDialogID);
        }
    }


    // Set next page as active page
    void TStackedDialog::SetNextPage(void)
    {
        TStackedPageHelper *pPage = getNext();
        if (pPage)
        {
            SetPage(pPage->m_nDialogID);
        }
    }


    // Get helper object associated with the currently active page
    TStackedPageHelper *TStackedDialog::GetPage(void)
    {
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if (it->m_bActive)
            {
                return &(*it);
            }
        }
        return NULL;
    }


    // Get helper object associated with the specified child resource id
    TStackedPageHelper *TStackedDialog::GetPage(UINT nId)
    {
        TStackedPageHelperList::iterator it;
        for ( it = m_PageList.begin(); it != m_PageList.end(); ++it )
        {
            if (it->m_nDialogID == nId)
            {
                return &(*it);
            }
        }
        return NULL;
    }
}
