#pragma once

#include "MsgPropertyPage.h"
// MsgParamsPage dialog

class MsgParamsPage : public MsgPropertyPage
{
	DECLARE_DYNAMIC(MsgParamsPage)

public:
	MsgParamsPage();
	virtual ~MsgParamsPage();

	CListCtrl m_listParams;

	bool InitialiseParameterList();
	bool PopulateParameterList();
	void UpdateButtonState();
	void DisableControls();

// Dialog Data
	enum { IDD = IDD_MSGPARAMSPAGE };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()

	afx_msg void OnAddParam();
	afx_msg void OnRemoveParam();
	afx_msg void OnMoveUp();
	afx_msg void OnMoveDown();
	afx_msg void OnParamListItemChange(NMHDR* pNMHDR, LRESULT* pResult);

public:
	virtual BOOL OnInitDialog();
};
