#pragma once

#include "..\SizingDialog\cdxCDynamicPropSheet.h"

// MsgPropertyPage dialog

class MsgPropertyPage : public cdxCDynamicPropPage
{
	DECLARE_DYNAMIC(MsgPropertyPage)

public:
	MsgPropertyPage(UINT uiIDD);
	virtual ~MsgPropertyPage();

// Dialog Data

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnApply();
	virtual void OnOK();
};
