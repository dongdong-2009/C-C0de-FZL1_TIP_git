#pragma once

#include "..\SizingDialog\cdxCDynamicPropSheet.h"

#include "MsgAttributesPage.h"
#include "MsgStringPage.h"
#include "MsgStylePage.h"
#include "MsgParamsPage.h"

#include "..\MessageGrid\MessageGridDoc.h"
#include "..\Message.h"
// MessagePropertySheet


class MessagePropertySheet : public cdxCDynamicPropSheet
{
	DECLARE_DYNAMIC(MessagePropertySheet)

public:
	MessagePropertySheet(UINT nIDCaption, CWnd* pParentWnd, UINT iSelectPage, Message* message);
	MessagePropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage, Message* message);
	virtual ~MessagePropertySheet();

protected:
	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();

	bool SaveChanges();

	MsgAttributesPage m_pageAttributes;
	MsgStylePage m_pageStyle;
	MsgParamsPage m_pageParams;
	CArray<MsgStringPage*, MsgStringPage*> m_rgStringPages;
	
	bool m_bNewMessage;

	MessageGridDoc* m_pGridDoc;

	Message* m_pMessage;
};


