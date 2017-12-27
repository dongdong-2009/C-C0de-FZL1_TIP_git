#pragma once

#include "..\SizingDialog\cdxCDynamicBar.h"
#include "..\DHTMLView\SimpleBrowser.h"
#include "..\resource.h"

class HelpControlBar : public cdxCDynamicBarDlg
{
private:
	class HelpTopic
	{
	public:

		TCHAR m_szURL[128 + 1];
		UINT m_uiTitleStringID;
		bool m_bInternalTopic;

		inline HelpTopic(LPCTSTR pszURL, UINT uiTitleStringID, bool bInternalTopic)
		{
			_tcsncpy(m_szURL, pszURL, 128);
			m_uiTitleStringID = uiTitleStringID;
			m_bInternalTopic = bInternalTopic;
		}
	};


public:
	HelpControlBar(CWnd* pParent = NULL);
	~HelpControlBar(void);

	enum { IDD = IDD_HELP_DLGBAR };

	HWND    m_hWndToolbar; 
	CComboBox m_comboTopics;

	CMap<UINT, UINT, HelpTopic*, HelpTopic*> m_mapHelpTopics;

	void CreateToolBar();
	int FindTopicIndex(UINT uiTopic);
	void PopulateTopicCombo();

protected:

	SimpleBrowser m_browserCtrl;

	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(HelpControlBar)
	public:
		virtual BOOL OnInitDialog();
		virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

    //{{AFX_MSG(HelpControlBar)
		afx_msg LRESULT OnHelpTopic(WPARAM wParam, LPARAM lParam);
		afx_msg void OnNavigateBack();
		afx_msg void OnNavigateForward();
		afx_msg void OnNavigateStop();
		afx_msg void OnUpdateNavigateStop(CCmdUI* pCmdUI);
		afx_msg void OnRefresh();
		afx_msg void OnHome();
		afx_msg void OnPrint();
	afx_msg void OnChangeHelpTopic();
	//}}AFX_MSG
    DECLARE_MESSAGE_MAP()

};
