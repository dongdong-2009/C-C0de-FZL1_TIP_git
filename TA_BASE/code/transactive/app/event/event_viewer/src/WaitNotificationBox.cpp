/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/event/event_viewer/src/WaitNotificationBox.cpp $
 * @author:  Nick Jardine
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * WaitNotificationBox is an implementation of CDialog. It is displayed while the application closes so the user knows
 * the application is still active.
 */

#include "app/event/event_viewer/src/stdafx.h"
#include "app/event/event_viewer/src/EventViewer.h"
#include "app/event/event_viewer/src/WaitNotificationBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// WaitNotificationBox dialog


WaitNotificationBox::WaitNotificationBox(CWnd* pParent /*=NULL*/)
	: CDialog(WaitNotificationBox::IDD, pParent)
{
	//{{AFX_DATA_INIT(WaitNotificationBox)
	//}}AFX_DATA_INIT
}


void WaitNotificationBox::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(WaitNotificationBox)
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(WaitNotificationBox, CDialog)
	//{{AFX_MSG_MAP(WaitNotificationBox)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// WaitNotificationBox message handlers

void WaitNotificationBox::OnOK() 
{
	// TODO: Add extra validation here
	
    // Comment out the default "OnOK" behaviour so "Enter" does nothing
	//CDialog::OnOK();
}

void WaitNotificationBox::OnCancel() 
{
	// TODO: Add extra cleanup here
	
    // Comment out the default "OnCancel" behaviour, so "Esc" does nothing
	//CDialog::OnCancel();
}
