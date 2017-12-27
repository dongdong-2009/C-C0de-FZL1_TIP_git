#pragma once

#include "MsgPropertyPage.h"

// MsgStringPage dialog

class MsgStringPage : public MsgPropertyPage
{
	DECLARE_DYNAMIC(MsgStringPage)

public:
	MsgStringPage(LCID lcidLanguage = 3081);
	virtual ~MsgStringPage();

	LCID m_lcidLanguage;
	CString m_sTitle;

// Dialog Data
	enum { IDD = IDD_MSGSTRINGPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
};
