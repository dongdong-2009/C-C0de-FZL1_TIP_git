#pragma once
#include "..\gridctrl\gridctrl.h"
#include "..\XMLObjects\XMLDefines.h"
#include "..\SizingDialog\cdxCDynamicDialog.h"

// ParameterInputDlg dialog

class ParameterInputDlg : public CMenuFrame<cdxCDynamicDialog>
{
	DECLARE_DYNAMIC(ParameterInputDlg)

public:
	ParameterInputDlg(CWnd* pParent = NULL);   // standard constructor
	virtual ~ParameterInputDlg();

	bool InitialiseGridCtrl();
	bool PopulateGridCtrl(bool bRefresh = true);
	bool InsertNodeIntoGrid(MSXML2::IXMLDOMNodePtr& spParameter);
	int InsertAddParamRow();
	bool ModifyParameter(int iRow, bool bNewParam);
	bool ValidateParameter(int iRow, int iCol);
	bool DeleteParameter(int iRow);
	void EditCell(int iRow, int iCol);
	void SetRowData(int iRow, DWORD dwData);

	CStringArray m_rgsParamTypes;
	CGridCtrl m_gridCtrl;
	HICON m_hIcon;
	CNewMenu m_menuPopups;
	bool m_bShowObsolete;

// Dialog Data
	enum { IDD = IDD_PARAMETERINPUTDLG };

protected:

	static int CALLBACK pfnCellCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	void OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	void OnGridRightClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridShowObsolete();
	afx_msg void OnAddParam();
	afx_msg void OnMakeObsolete();
	afx_msg void OnDeleteParam();
	afx_msg void OnInitMenuPopup(CMenu* pMenu, UINT nIndex, BOOL bSysMenu);
	afx_msg LRESULT OnEditCell(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
public:
	
	virtual BOOL OnInitDialog();
	virtual void OnOK();
};
