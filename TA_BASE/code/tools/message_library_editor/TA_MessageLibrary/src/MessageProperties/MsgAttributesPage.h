#pragma once

#include "MsgPropertyPage.h"
#include "..\Message.h"

// MsgAttributesPage dialog

class MsgAttributesPage : public MsgPropertyPage
{
	DECLARE_DYNAMIC(MsgAttributesPage)

public:
	MsgAttributesPage();
	virtual ~MsgAttributesPage();

	void DisableControls();

// Dialog Data
	enum { IDD = IDD_MSGATTRIBUTESPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	afx_msg void OnChangeHelpID();

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
};
