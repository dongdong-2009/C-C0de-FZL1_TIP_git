// MessagePropertySheet.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\MessageGrid\MessageGridView.h"
#include "..\TA_MessageLibrary.h"
#include "..\MainFrm.h"
#include "..\UserMessages.h"
#include "..\GlobalFunctions.h"

#include "MessagePropertySheet.h"

static const int LAST_FIXED_PAGE = 3;
// MessagePropertySheet

IMPLEMENT_DYNAMIC(MessagePropertySheet, cdxCDynamicPropSheet)

MessagePropertySheet::MessagePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, 
											 Message* message)
	:cdxCDynamicPropSheet(nIDCaption, pParentWnd, iSelectPage), m_pMessage(message), m_bNewMessage(false)
{
	LCID locale = 0;
	CString sMessage;
	MessageGridView* pView = (MessageGridView*)pParentWnd;
	m_pGridDoc = pView->GetDocument();
	int iPageIndex = 0;

	AddPage(&m_pageAttributes);
	AddPage(&m_pageStyle);
	AddPage(&m_pageParams);
	// Adjust the value of LAST_FIXED_PAGE if add more fixed pages

	POSITION pos = m_pMessage->m_mapMessages.GetStartPosition();
	while(pos)
	{
		m_pMessage->m_mapMessages.GetNextAssoc(pos, locale, sMessage);

		if(GetApp()->m_bInternalMode || locale == GetAppLCID())
		{
			iPageIndex = m_rgStringPages.Add(new MsgStringPage(locale));
			AddPage(m_rgStringPages[iPageIndex]);
		}
	}
}

MessagePropertySheet::MessagePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage,
											 Message* message)
	:cdxCDynamicPropSheet(pszCaption, pParentWnd, iSelectPage), m_pMessage(message), m_bNewMessage(false)
{
	LCID locale = 0;
	CString sMessage;
	MessageGridView* pView = (MessageGridView*)pParentWnd;
	m_pGridDoc = pView->GetDocument();
	int iPageIndex = 0;

	AddPage(&m_pageAttributes);
	AddPage(&m_pageStyle);
	AddPage(&m_pageParams);
	// Adjust the value of LAST_FIXED_PAGE if add more fixed pages

	POSITION pos = m_pMessage->m_mapMessages.GetStartPosition();
	while(pos)
	{
		m_pMessage->m_mapMessages.GetNextAssoc(pos, locale, sMessage);
		
		if(GetApp()->m_bInternalMode || locale == GetAppLCID())
		{
			iPageIndex = m_rgStringPages.Add(new MsgStringPage(locale));
			AddPage(m_rgStringPages[iPageIndex]);
		}
	}
}

MessagePropertySheet::~MessagePropertySheet()
{
	for(int i = 0; i < m_rgStringPages.GetSize(); i++)
		delete m_rgStringPages[i];

	m_rgStringPages.RemoveAll();
}


BEGIN_MESSAGE_MAP(MessagePropertySheet, cdxCDynamicPropSheet)
END_MESSAGE_MAP()


// MessagePropertySheet message handlers

BOOL MessagePropertySheet::OnInitDialog()
{
	BOOL bResult = cdxCDynamicPropSheet::OnInitDialog();

	if(m_bNewMessage)
	{
		CWnd* pApplyButton = GetDlgItem(ID_APPLY_NOW);
		if(pApplyButton)
			pApplyButton->ShowWindow(SW_HIDE);
	}
	
	TCITEM tcItem;
	ZeroMemory(&tcItem, sizeof(tcItem));
	tcItem.mask = TCIF_TEXT;
	
	CTabCtrl* pTabCtrl = GetTabControl();
	for(int i = 0; i < m_rgStringPages.GetSize(); i++)
	{
		tcItem.pszText = m_rgStringPages[i]->m_sTitle.GetBuffer(0);
		tcItem.cchTextMax = m_rgStringPages[i]->m_sTitle.GetLength();
		
		pTabCtrl->SetItem(LAST_FIXED_PAGE + i, &tcItem);

		m_rgStringPages[i]->m_sTitle.ReleaseBuffer();
	}

	return bResult;
}

bool MessagePropertySheet::SaveChanges()
{
	MessageGridDoc* pDoc = reinterpret_cast<MessageGridDoc*>(GetActiveDoc());

	bool bReturn = (m_bNewMessage) ? pDoc->AddNewMessage(*m_pMessage) : pDoc->ModifyMessage(*m_pMessage);
	if(bReturn)
		PostViewMessage(UM_REFRESH_GRID_MSG, NULL, m_pMessage->m_ulID);

	return bReturn;	
}