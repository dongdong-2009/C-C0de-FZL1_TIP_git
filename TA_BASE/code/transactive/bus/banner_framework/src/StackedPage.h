/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/banner_framework/src/StackedPage.h $
  * @author:  Zoran M.Todorovic
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * The class TStackedPage provides a "child" dialog in the stacked dialog framework.
  * The three classes TStackedDialog, TStackedPage, and TStackedPageHelper 
  * used togeather, provide a framework to create a dialog similar in behaviour to a CPropertySheet.
  * Basically, the TStackedDialog acts as a frame to display "child" dialogs (TStackedPage).
  * At any given time all,but one TStackedPage objects (child dialogs) are hidden.
  * There is also a TStackedPageHelper class which provides help in manipulating and adding dialogs.
  *
  * Note that the original code was third party, as the original header of STACKDIALOG.H indicates. 
  * The definitions and declarations all originally resided in two files
  */


#if !defined __STACKED_PAGE_H__
#define __STACKED_PAGE_H__

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_Bus
{
    class TStackedPage : public CDialog
    {
    public:
	    TStackedPage(LPCTSTR lpszTemplateName, CWnd* pParent = NULL);
	    TStackedPage(UINT nIDTemplate, CWnd* pParent = NULL);
	    virtual ~TStackedPage();

	    virtual void OnSetActive(void);
	    virtual void OnKillActive(void);
	    virtual void OnCreatePage(void);
	    virtual void OnDestroyPage(void);

	    //{{AFX_VIRTUAL(TStackedPage)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    protected:
	    //{{AFX_MSG(TStackedPage)
	    virtual void OnOK();
	    virtual void OnCancel();
	    virtual BOOL OnInitDialog();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };
}

#endif //#if !defined __STACKED_PAGE_H__
