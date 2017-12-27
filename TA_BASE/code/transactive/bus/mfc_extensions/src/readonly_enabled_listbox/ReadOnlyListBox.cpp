/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/readonly_enabled_listbox/ReadOnlyListBox.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ListBoxEx.cpp : implementation file
//

#include "bus/mfc_extensions/src/StdAfx.h"
#include "bus/mfc_extensions/src/readonly_enabled_listbox/ReadOnlyListBox.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyListBox

namespace TA_Base_Bus
{
	
	const COLORREF CReadOnlyListBox::COLOR_ENABLED = GetSysColor(COLOR_WINDOW);
	const COLORREF CReadOnlyListBox::COLOR_DISABLED = GetSysColor(COLOR_BTNFACE); 
	
	CReadOnlyListBox::CReadOnlyListBox()
	{	 
		m_bReadOnly = FALSE;
		m_bkBrush.CreateSolidBrush(GetSysColor(COLOR_WINDOW)); 
	}
	
	CReadOnlyListBox::~CReadOnlyListBox()
	{
		m_bkBrush.DeleteObject(); //Deletes the brush
	}
	
	
	BEGIN_MESSAGE_MAP(CReadOnlyListBox, CListBox)
		//{{AFX_MSG_MAP(CReadOnlyListBox)
		ON_WM_ENABLE()
		ON_WM_CTLCOLOR_REFLECT()
		//}}AFX_MSG_MAP
		END_MESSAGE_MAP()
		
		/////////////////////////////////////////////////////////////////////////////
		// CReadOnlyListBox message handlers
		
		void CReadOnlyListBox::PreSubclassWindow() 
	{
		// TODO: Add your specialized code here and/or call the base class	
		CListBox::PreSubclassWindow();
	}
	
	void CReadOnlyListBox::SetBkColor(COLORREF crBkColor,COLORREF crSelectedColor)
	{
		//Deletes previous brush. Must do in order to create a new one	
		m_bkBrush.DeleteObject(); 	
		m_bkBrush.CreateSolidBrush(crBkColor); 
		Invalidate(); //Forces Redraw
	}
	
	HBRUSH CReadOnlyListBox::CtlColor(CDC* pDC, UINT nCtlColor) 
	{
		CRect rect;
		GetClientRect(&rect);
		
		pDC->SetBkMode(TRANSPARENT); //Sets text background transparent
		pDC->FillRect(rect,&m_bkBrush); 
		
		return m_bkBrush;  
	}
	
	void CReadOnlyListBox::OnEnable(BOOL bEnable) 
	{ 
		COLORREF crBkColor = m_bReadOnly ? COLOR_DISABLED :
	COLOR_ENABLED;
	
	SetBkColor(crBkColor); 	 
	}
	
	BOOL CReadOnlyListBox::EnableWindow(BOOL bEnable) 
	{
		m_bReadOnly = !bEnable;
		
		SendMessage(WM_ENABLE);
		return m_bReadOnly; 
	}
}


 
