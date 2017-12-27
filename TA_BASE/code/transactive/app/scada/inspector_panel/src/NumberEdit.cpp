// NumberEdit.cpp : implementation file
//

#include "stdafx.h"
#include "NumberEdit.h"

// CNumberEdit
IMPLEMENT_DYNAMIC(CNumberEdit, CEdit)

CNumberEdit::CNumberEdit()
: m_acceptableNumChars("\b01234567890-."),
  m_numeric(false)
{

}

CNumberEdit::~CNumberEdit()
{
}

BEGIN_MESSAGE_MAP(CNumberEdit, CEdit)
	//{{AFX_MSG_MAP(CListCellEdit)
	ON_WM_CHAR()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL CNumberEdit::PreTranslateMessage(MSG* pMsg) 
{
	if( pMsg->message == WM_KEYDOWN )
	{
		if(pMsg->wParam == VK_RETURN || pMsg->wParam == VK_ESCAPE)
		{
			::TranslateMessage(pMsg);
			::DispatchMessage(pMsg);
			return TRUE;				// DO NOT process further
		}
	}	
	return CEdit::PreTranslateMessage(pMsg);
}

void CNumberEdit::OnChar(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	if( nChar == VK_ESCAPE || nChar == VK_RETURN)
	{
		GetParent()->SetFocus();
		return;
	}
	else if(m_numeric && (m_acceptableNumChars.Find(nChar) == -1))
	{
		return;
	}

	CEdit::OnChar(nChar, nRepCnt, nFlags);
}

void CNumberEdit::setNumeric(bool numeric)
{
	m_numeric = numeric;
}



