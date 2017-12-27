/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tab_dialog_control/TabDialogCtrl.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2015/01/23 16:08:36 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// TabDialogCtrl.cpp : implementation file
//

#include <afxwin.h>         // MFC core and standard components
#include <afxcmn.h>			// MFC support for Windows Common Controls
//#include "stdafx.h"
#include "TabDialogCtrl.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTabDialogCtrl

CTabDialogCtrl::CTabDialogCtrl()
{
	// Initialise the list of dialog pages
	m_activeDialog = NULL;
	m_selectedTab  = 0;
}

CTabDialogCtrl::~CTabDialogCtrl()
{
}


void CTabDialogCtrl::AddPage(CDialog&  dlg,UINT templateID,CString& label)
{
	ASSERT (dlg.m_hWnd==NULL);    // The dialog window should not be created yet
    ASSERT (this->m_hWnd==NULL);  // These must be added before the control is drawn

	// Add to the list of dialogs
	DialogInfo   dlgInfo;
	dlgInfo.bValid = true;
	dlgInfo.pDlg = &dlg;
	dlgInfo.nTemplateID = templateID;
	dlgInfo.label = label;
	m_dlgInfo.push_back(dlgInfo);
}

void CTabDialogCtrl::SetCurTab( UINT templateID )
{
	std::vector<DialogInfo>::iterator it;

	for (it = m_dlgInfo.begin(); it < m_dlgInfo.end(); ++it)
	{
		if ((*it).nTemplateID == templateID)
		{
			if ((*it).bValid)
			{
				bool haveSelectedTab = false;
				int j = GetItemCount();
				for (int i = 0; i < j; i++)
				{
					TCITEM tcItem;
					char buffer[256] = {0};

					tcItem.pszText = buffer;
					tcItem.cchTextMax = 256;
					tcItem.mask = TCIF_TEXT;
					GetItem( i, &tcItem );
					if (tcItem.pszText == (*it).label)
					{
						haveSelectedTab = true;
						m_selectedTab = i;
						CTabCtrl::SetCurSel(i);

 						// Hide the last dialog (if any)
 						if (m_activeDialog!=NULL)
 						{
 							m_activeDialog->ShowWindow(SW_HIDE);
 						}
						
 						(*it).pDlg->ShowWindow(SW_SHOW);
 						m_activeDialog = (*it).pDlg;

						break;
					}
				}

				if (!haveSelectedTab)
				{
					m_selectedTab = 0;
					SetCurSel(0);
				}
			}
		}
	}
}

void CTabDialogCtrl::DelPage(UINT templateID)
{
	
	std::vector<DialogInfo>::iterator   it;
	
	//ASSERT (tabPage<m_dlgInfo.size());
	
	// Hide the last dialog (if any)
	if (m_activeDialog!=NULL) m_activeDialog->ShowWindow(SW_HIDE);


	int nIndex = 0;
	for (it = m_dlgInfo.begin(); it < m_dlgInfo.end(); ++it)
	{
		if ((*it).bValid)
		{
			if ((*it).nTemplateID == templateID )
			{
				(*it).pDlg->ShowWindow(SW_HIDE);
				DeleteItem(nIndex);
				(*it).bValid= false;

				if (m_selectedTab >= nIndex)
					m_selectedTab -= 1;

				if (m_selectedTab < 0)
					m_selectedTab = 0;
				break;
			}
			++nIndex;
		}
	}

	SetCurSel(m_selectedTab);
}


int CTabDialogCtrl::SetCurSel(int nItem)
{
	int		iRet;

	// Do normal stuff
	iRet = CTabCtrl::SetCurSel(nItem);

	// Display associated dialog
	DisplayDialog(nItem);

	return iRet;
}

BEGIN_MESSAGE_MAP(CTabDialogCtrl, CTabCtrl)
	//{{AFX_MSG_MAP(CTabDialogCtrl)
	ON_NOTIFY_REFLECT_EX(TCN_SELCHANGE, OnSelchange)
	ON_WM_CREATE()
	ON_WM_DESTROY()
	ON_WM_PAINT()
	ON_WM_MOVE()
  	ON_WM_SHOWWINDOW()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTabDialogCtrl message handlers

void CTabDialogCtrl::OnPaint() 
{
	//	Update the active dialog
	CTabCtrl::OnPaint();
	if (m_activeDialog!=NULL) m_activeDialog->RedrawWindow();
}

BOOL CTabDialogCtrl::OnSelchange(NMHDR* pNMHDR, LRESULT* pResult) 
{
	// Display the matching dialog
	UINT newPage = this->GetCurSel();
	DisplayDialog(newPage);
	*pResult = 0;

	return TRUE;
}



int CTabDialogCtrl::OnCreate(LPCREATESTRUCT lpCreateStruct) 
{
	if (CTabCtrl::OnCreate(lpCreateStruct) == -1)
		return -1;

	// Create the associated modeless dialog windows 
	CreateDialogs();
	
	return 0;
}

void CTabDialogCtrl::OnDestroy() 
{
	// Dispose of the modeless dialogs
    //m_activeDialog = NULL;
	std::vector<DialogInfo>::iterator   it;
 	for (it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++)
 	{
 		(*it).pDlg->DestroyWindow();
 	}
	m_dlgInfo.clear();
	CTabCtrl::OnDestroy();
}

void CTabDialogCtrl::resetAllPages()
{
	for (int index = 0; index < m_dlgInfo.size(); ++index)
	{
		if (!m_dlgInfo[index].bValid)
		{
			
			InsertItem(index, m_dlgInfo[index].label);
			m_dlgInfo[index].bValid = true;
		}
	}
}
void CTabDialogCtrl::PreSubclassWindow() 
{
	// Perform normal stuff
	CTabCtrl::PreSubclassWindow();

	// The parent will be the dialog which hosts the tab control
	CreateDialogs();
}

void CTabDialogCtrl::CreateDialogs()
{
	int	iTab = 0;

	CWnd* pParent = this->GetParent();
    ASSERT (pParent!=NULL);

	// Create the associated modeless dialog windows 
	CString							title;
	std::vector<DialogInfo>::iterator   it;
	for (it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++)
	{
		// Setup the dialog
		(*it).pDlg->Create((*it).nTemplateID,pParent);
 		if (it==m_dlgInfo.begin())
		{
			(*it).pDlg->ShowWindow(SW_NORMAL);
		}
		else
		{
			(*it).pDlg->ShowWindow(SW_HIDE);
		}

		// Add a tab to the control for the dialog
		if ((*it).bValid)
		{
			InsertItem(iTab,(*it).label);
			iTab++;
		}

		ShowWindow(SW_NORMAL);

		// Save first page for current
		if (it==m_dlgInfo.begin())
		{
			m_activeDialog = (*it).pDlg;
		}
	}

	// Reset to first tab
	//SetCurSel(0);

	ResizeDialogs();
}

void CTabDialogCtrl::ResizeDialogs()
{
	std::vector<DialogInfo>::iterator   it;
	CRect		rect;
	int			offset = 40;

	// Determine the size the dialogs should be
	GetWindowRect(&rect);
	AdjustRect(false,&rect);

	// Removed Dodgy Hack (RobA 07/07/2003
	/*if ((this->GetParent()) != (AfxGetApp()->m_pMainWnd) )
	{
		ScreenToClient(&rect);
		CRect parentRect;
		CRect thisRect;
		::GetWindowRect(GetParent()->m_hWnd, &parentRect);
		::GetWindowRect(this->m_hWnd, &thisRect);
		unsigned int diffLeft = thisRect.left - parentRect.left;
		unsigned int diffTop = thisRect.top - parentRect.top;
		rect.left += diffLeft;
		rect.right += diffLeft;
		rect.top += diffTop;
		rect.bottom += diffTop;
	} */

	// If the dialog has a menu then positioning is wrong!
	// For demo we will make a small adjustment as this class is to be replaced
	CWnd*  pDlg = GetTopLevelParent();   //this->GetParent()
	if (pDlg!=NULL)
	{
		if (pDlg->GetMenu()==NULL) 
		{
			offset=20;
		}
	}

	// Resize all dialogs
	for (it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++)
	{
		(*it).pDlg->SetWindowPos(NULL,rect.left-4,rect.top-offset-9,rect.Width(),rect.Height(),SWP_NOZORDER);
	}
}


void CTabDialogCtrl::OnMove(int x, int y) 
{
	CTabCtrl::OnMove(x, y);

	// Refresh the active dialogs as it may be exposed and not yet redrawn
	if (m_activeDialog!=NULL) m_activeDialog->RedrawWindow();

	// TODO: Solve this problem for pop-up dialog pages whilst not breaking
	// child dialogs which are moved programmatically (ie via CWnd::MoveWindow)
 //	ResizeDialogs();
}

/*
 * DisplayDialog
 * 
 * This function will display the dialog associated with the specified tab, 
 * hiding any currently displayed one.
 *
 */
void CTabDialogCtrl::DisplayDialog(UINT tabPage)
{
	std::vector<DialogInfo>::iterator   it;

	ASSERT (tabPage<m_dlgInfo.size());
	
	// Hide the last dialog (if any)
	if (m_activeDialog!=NULL) m_activeDialog->ShowWindow(SW_HIDE);

	// Activate the new dialog
	//it = m_dlgInfo.begin();
	//it = it + tabPage;
	int nIndex = 0;
	for (it = m_dlgInfo.begin(); it < m_dlgInfo.end(); ++it  )
	{
		if ((*it).bValid)
		{
			if (nIndex == tabPage)
			{
				(*it).pDlg->ShowWindow(SW_NORMAL);

				m_activeDialog = (*it).pDlg;
				break;
			}
			nIndex++;
		}
	}
}

void CTabDialogCtrl::OnShowWindow(BOOL bShow, UINT nStatus) 
{
    CTabCtrl::OnShowWindow(bShow, nStatus);    

    if (!bShow)
    {
        // If this is a hide command, then hide all child dialogs

        std::vector<DialogInfo>::iterator   it;
	    for (it=m_dlgInfo.begin();it!=m_dlgInfo.end();it++)
	    {
            (*it).pDlg->ShowWindow(SW_HIDE);
	    }
    }
    else
    {
        // Only show the active dialog 
        if (m_activeDialog!=NULL) m_activeDialog->ShowWindow(SW_NORMAL);
    }	
	
}