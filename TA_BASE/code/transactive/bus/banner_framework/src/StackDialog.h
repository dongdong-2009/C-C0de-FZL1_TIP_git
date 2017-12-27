/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/StackDialog.h $
  * @author:  Zoran M.Todorovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The class TStackedDialog provides the "parent" dialog in the stacked dialog framework.
  * The three classes TStackedDialog, TStackedPage, and TStackedPageHelper 
  * used togeather, provide a framework to create a dialog similar in behaviour to a CPropertySheet.
  * Basically, the TStackedDialog acts as a frame to display "child" dialogs (TStackedPage).
  * At any given time all,but one TStackedPage objects (child dialogs) are hidden.
  * There is also a TStackedPageHelper class which provides help in manipulating and adding dialogs.
  *
  * TStackedDialog is an abstract class.
  *
  * Note that the original code was third party, as the original header of STACKDIALOG.H (below) indicates. 
  * The definitions and declarations all originally resided STACKDIALOG.H and STACKDIALOG.CPP
  */
/*#############################################################################
# STACKDIALOG.H
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

#if !defined __STACKED_DIALOG_H__
#define __STACKED_DIALOG_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "bus/banner_framework/src/StackedPageHelper.h"
#include "bus/generic_gui/src/TransActiveDialog.h"

namespace TA_Base_Bus
{
    class TStackedPage;

    class TStackedDialog : public TA_Base_Bus::TransActiveDialog
    {
    protected:
      UINT m_nPlaceholderID;
      TStackedPageHelperList m_PageList;

    private:
	    void init(UINT placeholder);
	    void flush(void);
	    void destroyPageDialog(TStackedPageHelper *pPage);
	    TStackedPageHelper* getFirst(void);
	    TStackedPageHelper* getCurrent(void);
	    TStackedPageHelper* getNext(void);

    protected:
	    void activatePage(TStackedPageHelper *pPage);
	    void deactivatePage(TStackedPageHelper *pPage);

    public:
	    TStackedDialog(TA_Base_Bus::IGUIAccess& gui, UINT nId, LPCTSTR lpszTemplate, CWnd* pParent = NULL);
	    TStackedDialog(TA_Base_Bus::IGUIAccess& gui, UINT nId, UINT nIDTemplate, CWnd* pParent = NULL);
	    virtual ~TStackedDialog();

	    TStackedPageHelper* AddPage(UINT nId, DWORD dwHelpId = 0L,
							      DWORD dwData1 = 0, DWORD dwData2 = 0);
	    void DelPage(TStackedPageHelper* pPage);

	    void SetPage(void);
	    void SetPage(UINT nId);
	    void SetFirstPage(void);
	    void SetNextPage(void);

	    TStackedPageHelper *GetPage(void);
	    TStackedPageHelper *GetPage(UINT nId);

	    virtual TStackedPage* CreatePage(UINT nId) = 0;
	    virtual void OnPageChanged(UINT nId, BOOL bActivated) = 0;

	    //{{AFX_VIRTUAL(TStackedDialog)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    protected:
	    //{{AFX_MSG(TStackedDialog)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnDestroy();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };
}
#endif //!defined __STACKED_DIALOG_H__
