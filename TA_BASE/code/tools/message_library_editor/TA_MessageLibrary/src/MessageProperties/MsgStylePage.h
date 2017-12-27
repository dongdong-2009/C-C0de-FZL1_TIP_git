#pragma once

#include "MsgPropertyPage.h"
// MsgStylePage dialog

class MsgStylePage : public MsgPropertyPage
{
	DECLARE_DYNAMIC(MsgStylePage)

public:
	MsgStylePage();
	virtual ~MsgStylePage();

// Dialog Data
	enum { IDD = IDD_MSGSTYLEPAGE };

	void ValidateDefaultButton(int iBtnCount);
	void SetButtons();
	void DisableControls();

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	inline afx_msg void OnChgMsgBtnOK() { ValidateDefaultButton(1); }
	inline afx_msg void OnChgMsgBtnOKCANCEL() { ValidateDefaultButton(2); }
	inline afx_msg void OnChgMsgBtnYESNO() { ValidateDefaultButton(2); }
	inline afx_msg void OnChgMsgBtnYESNOCANCEL() { ValidateDefaultButton(3); }
	inline afx_msg void OnChgMsgBtnRETRYCANCEL() { ValidateDefaultButton(2); }
	inline afx_msg void OnChgMsgBtnABORTRETRYCANCEL() { ValidateDefaultButton(3); }
	inline afx_msg void OnChgMsgBtnABORTTRYCANCEL() { ValidateDefaultButton(3); }
	
	inline afx_msg void OnChgHelpBtn() { SetButtons(); }

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
	
};
