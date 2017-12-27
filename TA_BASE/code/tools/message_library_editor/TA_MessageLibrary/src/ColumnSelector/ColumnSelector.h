#pragma once

// ColumnSelector dialog
#include "..\SizingDialog\cdxCDynamicDialog.h"

class ColumnSelector : public cdxCDynamicDialog
{
	DECLARE_DYNAMIC(ColumnSelector)

public:
	ColumnSelector(CWnd* pParent);   // standard constructor
	virtual ~ColumnSelector();

	bool InitialiseColumnList();
	bool PopulateColumnList();

	CListCtrl m_listColumns;
	CWnd* m_pParent;

	bool m_bDragging;
	int m_iDragIndex, m_iDropIndex;
	CImageList* m_pDragImage;
	HICON m_hIcon;

// Dialog Data
	enum { IDD = IDD_COLUMNSELECTOR };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	virtual BOOL OnInitDialog();
protected:
	virtual void OnOK();
public:
	afx_msg void OnLvnBegindragListColumnSelector(NMHDR *pNMHDR, LRESULT *pResult);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
};
