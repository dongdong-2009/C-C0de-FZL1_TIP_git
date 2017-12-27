/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ProgressBar.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#include "stdafx.h"
#include "ProgressBar.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

	IMPLEMENT_DYNCREATE(CProgressBar, CProgressCtrl)

	BEGIN_MESSAGE_MAP(CProgressBar, CProgressCtrl)
		//{{AFX_MSG_MAP(CProgressBar)
		ON_WM_ERASEBKGND()
		//}}AFX_MSG_MAP
	END_MESSAGE_MAP()


	CProgressBar::CProgressBar()
        : m_parentWnd(NULL)
	{
		m_Rect.SetRect(0,0,0,0);
	}

	
	CProgressBar::CProgressBar(CString aMessage, int aPercentSize, int aMaxValue, bool aSmooth, int aPane)
        : m_parentWnd(NULL)
	{
		Create(aMessage, aPercentSize, aMaxValue, aSmooth, aPane);
	}

	
	CProgressBar::~CProgressBar()
	{
		Clear();
	}

	
	CStatusBar* CProgressBar::GetStatusBar()
	{
		CWnd *pMainWnd = m_parentWnd;
		
		if (pMainWnd == NULL)
        {
            pMainWnd = AfxGetMainWnd();
        }
		
		if (pMainWnd == NULL)
			return NULL;
				
		return DYNAMIC_DOWNCAST(CStatusBar, pMainWnd->GetDescendantWindow(AFX_IDW_STATUS_BAR));
	}

	
	bool CProgressBar::Create(CString aMessage, int aPercentSize, int aMaxValue, bool aSmooth, int aPane, CWnd* pParentWnd)
	{
		bool bSuccess = false;

        if (NULL != pParentWnd)
        {
            m_parentWnd = pParentWnd;
        }
		
		CStatusBar *pStatusBar = GetStatusBar();
		
		if (pStatusBar == NULL)
			return false;

		DWORD dwStyle = WS_CHILD | WS_VISIBLE;
		
		if (aSmooth)
			dwStyle |= PBS_SMOOTH;
		
		CRect PaneRect;
		pStatusBar->GetItemRect(aPane, &PaneRect);
		
		BOOL result = CProgressCtrl::Create(dwStyle, PaneRect, pStatusBar, 1);		
		
		if (result == FALSE)
			return false;
		
		SetRange(0, aMaxValue);
		SetStep(1);

		m_Message  = aMessage;
		m_PercentSize = aPercentSize;
		m_Pane 	  = aPane;
		
		Resize();

		return true;
	}

	
	void CProgressBar::Clear()
	{
		if (!IsWindow(GetSafeHwnd()))
			return;

        ModifyStyle(WS_VISIBLE, 0);

		CString str;

		if (m_Pane == 0)
			str.LoadString(AFX_IDS_IDLEMESSAGE);   // Get the IDLE_MESSAGE

		CStatusBar *pStatusBar = GetStatusBar();
		
		if (pStatusBar)
		{
			pStatusBar->SetPaneText(m_Pane, str);
			pStatusBar->UpdateWindow();
		}
	}


	bool CProgressBar::SetMessage(CString aMessage)
	{ 
		m_Message = aMessage; 
		return Resize(); 
	}


	bool CProgressBar::SetSize(int aPercentSize)
	{
		m_PercentSize = aPercentSize; 
		return Resize();
	}


	void CProgressBar::SetBarColour(COLORREF aColorBar)
	{
		SendMessage(PBM_SETBARCOLOR, 0, (LPARAM) aColorBar);
	}


	void CProgressBar::SetBackgroundColour(COLORREF aColorBackground)
	{
		SendMessage(PBM_SETBKCOLOR, 0, (LPARAM) aColorBackground);
	}


	bool CProgressBar::SetRange(int aLower, int aUpper, int aStep)	
	{	 
		if (!IsWindow(GetSafeHwnd()))
			return false;

		CProgressCtrl::SetRange32(aLower, aUpper);		
		CProgressCtrl::SetStep(aStep);

		return true;
	}


	int CProgressBar::SetPos(int aPos)	   
	{
		if (!IsWindow(GetSafeHwnd()))
			return 0;

		ModifyStyle(0, WS_VISIBLE);

		return CProgressCtrl::SetPos(aPos);
	}

	int CProgressBar::OffsetPos(int aPos) 
	{ 
		if (!IsWindow(GetSafeHwnd()))
			return 0;

		ModifyStyle(0, WS_VISIBLE);
	
		return CProgressCtrl::OffsetPos(aPos);
	}

	int CProgressBar::SetStep(int aStep)
	{ 
		if (!IsWindow(GetSafeHwnd()))
			return 0;

		ModifyStyle(0, WS_VISIBLE);

		return CProgressCtrl::SetStep(aStep);	 
	}

	int CProgressBar::StepIt() 			
	{ 
		if (!IsWindow(GetSafeHwnd()))
			return 0;

		ModifyStyle(0, WS_VISIBLE);

		return CProgressCtrl::StepIt();    
	}

	bool CProgressBar::Resize() 
	{
		if (!IsWindow(GetSafeHwnd()))
			return false;

		CStatusBar *pStatusBar = GetStatusBar();
		
		if (pStatusBar == NULL)
			return false;
		
		if (IsWindowVisible())
		{
			pStatusBar->SetPaneText(m_Pane, m_Message);
			pStatusBar->UpdateWindow();
		}

		// find out how much space our message takes
		CClientDC dc(pStatusBar);
		CFont *pOldFont = dc.SelectObject(pStatusBar->GetFont());

		CSize size = dc.GetTextExtent(m_Message);		
		int margin = dc.GetTextExtent(_T(" ")).cx * 2;		

		dc.SelectObject(pOldFont);

		// Now calculate the rectangle in which we will draw the progress bar
		CRect rc;
		pStatusBar->GetItemRect(m_Pane, rc);

		// Position left of progress bar after text and right of progress bar
		// to requested percentage of status bar pane
		if (!m_Message.IsEmpty())
			rc.left += (size.cx + 2 * margin);
		rc.right -= (rc.right - rc.left) * (100 - m_PercentSize) / 100;

		if (rc.right < rc.left) rc.right = rc.left;
		
		// Leave a litle vertical margin (10%) between the top and bottom of the bar
		int Height = rc.bottom - rc.top;
		rc.bottom -= Height/10;
		rc.top	  += Height/10;

		// If the window size has changed, resize the window
		if (rc != m_Rect)
		{
			MoveWindow(&rc);
			m_Rect = rc;
		}

		return true;
	}

	BOOL CProgressBar::OnEraseBkgnd(CDC* pDC) 
	{
		Resize();
		return CProgressCtrl::OnEraseBkgnd(pDC);
	}

} // TA_Base_App
