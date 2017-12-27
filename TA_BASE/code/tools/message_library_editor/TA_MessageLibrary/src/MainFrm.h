// MainFrm.h : interface of the MainFrame class
//
#pragma once

#include "HelpView\HelpControlBar.h"
#include "ParameterView\ParameterControlBar.h"
#include "ColourStatusBar\ColoredStatusBarCtrl.h"
#include "Menu\MenuToolBar.h"

class MainFrame : public CMenuFrameWnd
{
	
protected: // create from serialization only
	MainFrame();
	DECLARE_DYNCREATE(MainFrame)

// Attributes
public:

// Operations
public:
	void DockControlBarNextTo(CControlBar* pBar, CControlBar* pTargetBar);
	void SetMessageText(LPCTSTR lpszText, COLORREF crColour = GetSysColor(COLOR_MENUTEXT));

// Overrides
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// Implementation
public:
	virtual ~MainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members
	ColoredStatusBarCtrl  m_wndStatusBar;
	CMenuToolBar    m_wndToolBar;

public:
	cdxCDynamicBarT<HelpControlBar>	m_wndHelpCtrlBar;
	cdxCDynamicBarT<ParameterControlBar> m_wndParamCtrlBar;

// Generated message map functions
protected:
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnFileExtractfromexcel();
	afx_msg void OnUpdateFileExtractfromexcel(CCmdUI *pCmdUI);
	afx_msg void OnViewHelpbar();
	afx_msg void OnUpdateViewHelpbar(CCmdUI *pCmdUI);
	afx_msg void OnFileExporttorcfile();
	afx_msg void OnUpdateFileExporttorcfile(CCmdUI *pCmdUI);
	afx_msg void OnParameterInput();
	afx_msg void OnUpdateParameterInput(CCmdUI *pCmdUI);
	afx_msg void OnEditNewLang();
	afx_msg void OnUpdateEditNewLang(CCmdUI *pCmdUI);
	afx_msg void OnViewParams();
	afx_msg void OnUpdateViewParams(CCmdUI* pCmdUI);

	afx_msg LRESULT OnHelpTopic(WPARAM wParam, LPARAM lParam);

	virtual BOOL DestroyWindow();
};


