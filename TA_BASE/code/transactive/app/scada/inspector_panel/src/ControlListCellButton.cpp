/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/scada/inspector_panel/src/ControlListCellButton.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2009/09/24 11:10:12 $
  * Last modified by:  $Author: grace.koh $
  *
  */
// ListCellButton.cpp : implementation file
//

#include "stdafx.h"
//#include "resource.h"
#include "ControlListCellButton.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif
#define IDC_LIST_EDIT_CTRL 5000

namespace TA_Base_App
{
	/////////////////////////////////////////////////////////////////////////////
	// CListCellButton

	CControlListCellButton::CControlListCellButton(CWnd* pWnd, CRect cellRect, int item, CString strValue, IButtonProcessor* listCellButtonProcessor) 
		: m_parent(pWnd), 
		m_item(item),
		m_bExpandDialog(false),
		m_bInProccess(false), /*m_subItem(subItem), */
		m_listCellButtonProcessor(listCellButtonProcessor),
		m_bIsWaitingForConfirm(false),
		m_confirmDialogHandle(NULL)
	{

		int nHightAdjust = (int)cellRect.Height() * 0.1;
		int nWidthAdjust = (int)cellRect.Width() * 0.05;
		CRect buttonRect(cellRect.left + nWidthAdjust, cellRect.top + nHightAdjust, cellRect.right - nWidthAdjust, cellRect.bottom - nHightAdjust);

		m_stringValue = strValue;
		CString strCaption = strValue;
		Create(strCaption, WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON, buttonRect, m_parent, IDC_LIST_EDIT_CTRL);


		SetFocus();

		ShowWindow(SW_SHOW);
	}



	CControlListCellButton::~CControlListCellButton()
	{
	}


	BEGIN_MESSAGE_MAP(CControlListCellButton, CButton)
		//{{AFX_MSG_MAP(CListCellButton)
		ON_WM_LBUTTONDOWN()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CListCellButton message handlers
	void CControlListCellButton::expandDialog()
	{
		m_bExpandDialog = true;
	}
	void CControlListCellButton::unExpandDialog()
	{
		m_bExpandDialog = false;
	}

	int CControlListCellButton::getItem()
	{
		return m_item;
	}

	bool CControlListCellButton::getIfExpandDialog()
	{
		return m_bExpandDialog;
	}

	void CControlListCellButton::OnLButtonDown(UINT nFlags, CPoint point)
	{
		SetFocus();
		m_listCellButtonProcessor->processButtonClick(m_item, /*m_subItem,*/m_stringValue);

	}

	void CControlListCellButton::setValue(CString strValue)
	{
		m_stringValue = strValue;
	}

	CString CControlListCellButton::getValue()
	{
		return m_stringValue;
	}


	bool CControlListCellButton::isInProgress()
	{
		return m_bInProccess;
	}

	void CControlListCellButton::EnterProcess()
	{
		m_bInProccess = true;
	}

	void CControlListCellButton::ExitProcess()
	{
		m_bInProccess = false;
	}

	void CControlListCellButton::displayButton(bool isDisplay)
	{
		EnableWindow(isDisplay);
	}

	bool	CControlListCellButton::getIsWaitingForConfirm()
	{
		return m_bIsWaitingForConfirm;
	}
	
	void	CControlListCellButton::setIsWaitingForConfirm(bool isWaitConfirm)
	{
		m_bIsWaitingForConfirm = isWaitConfirm;
	}

	CWnd*	CControlListCellButton::getConfirmDialogHandle()
	{
		return m_confirmDialogHandle;
	}

	void	CControlListCellButton::setConfirmDialogHandle(CWnd* handle)
	{
		m_confirmDialogHandle = handle;
	}

}
