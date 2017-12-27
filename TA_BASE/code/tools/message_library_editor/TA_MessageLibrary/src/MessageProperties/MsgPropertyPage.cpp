// MsgPropertyPage.cpp : implementation file
//

#include "..\stdafx.h"

#include "..\TA_MessageLibrary.h"

#include "MsgPropertyPage.h"
#include "MessagePropertySheet.h"


// CMsgAttributesPage dialog

IMPLEMENT_DYNAMIC(MsgPropertyPage, cdxCDynamicPropPage)

MsgPropertyPage::MsgPropertyPage(UINT uiIDD)
	: cdxCDynamicPropPage(uiIDD)
{
}

MsgPropertyPage::~MsgPropertyPage()
{
}

void MsgPropertyPage::DoDataExchange(CDataExchange* pDX)
{
	cdxCDynamicPropPage::DoDataExchange(pDX);
}


BEGIN_MESSAGE_MAP(MsgPropertyPage, cdxCDynamicPropPage)
END_MESSAGE_MAP()


// MsgPropertyPage message handlers

BOOL MsgPropertyPage::OnApply()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();
	if(pSheet->SaveChanges())
	{
		SetModified(FALSE);
		return cdxCDynamicPropPage::OnApply();
	}
	else
		return FALSE;
}

void MsgPropertyPage::OnOK()
{
	MessagePropertySheet* pSheet = (MessagePropertySheet*)GetParent();

	if(pSheet->SaveChanges())
	{
		SetModified(FALSE);
		cdxCDynamicPropPage::OnOK();
	}
}
