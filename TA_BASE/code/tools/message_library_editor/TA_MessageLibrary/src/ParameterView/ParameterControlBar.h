#pragma once

#include "..\SizingDialog\cdxCDynamicBar.h"
#include "..\DHTMLView\SimpleBrowser.h"
#include "..\resource.h"

class ParameterControlBar : public cdxCDynamicBarDlg
{
	//DECLARE_DYNAMIC(ParameterControlBar);
public:
	ParameterControlBar(CWnd* pParent = NULL);
	~ParameterControlBar(void);

	enum { IDD = IDD_PARAM_DLGBAR };

	SimpleBrowser m_browserCtrl;
	

/*	  virtual BOOL Create(
		CWnd* pParentWnd,			// mandatory
        CRuntimeClass *pViewClass,	// mandatory
		CCreateContext *pContext = NULL,
		LPCTSTR lpszWindowName = NULL,
		DWORD dwStyle = WS_CHILD | WS_VISIBLE | CBRS_TOP,
		UINT nID = AFX_IDW_PANE_FIRST);*/

protected:
//	CFrameWnd *m_pFrameWnd;
//	CCreateContext m_Context;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(ParameterControlBar)
	public:
		virtual BOOL OnInitDialog();
	//}}AFX_VIRTUAL

    //{{AFX_MSG(ParameterControlBar)
		afx_msg LRESULT OnRefreshParameters(WPARAM wParam, LPARAM lParam);
		afx_msg void OnNavigateComplete2(NMHDR *pNMHDR,LRESULT *pResult);
		afx_msg void OnDocumentComplete(NMHDR *pNMHDR,LRESULT *pResult);
 //   afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
//	afx_msg void OnSize(UINT nType, int cx, int cy);
    //}}AFX_MSG
    DECLARE_MESSAGE_MAP()
	
};
