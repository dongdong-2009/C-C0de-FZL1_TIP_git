// MessageGridView.h : interface of the MessageGridView class
//
#pragma once

#include "..\SizingDialog\cdxCDynamicFormView.h"
#include "..\GridCtrl\GridCtrl.h"
#include "..\Message.h"
#include "..\XMLObjects\XMLDefines.h"
#include "..\resource.h"

#include "GridDefines.h"
#include "MessageGridDoc.h"

class ColumnHeader
{
public:
	CString m_sTitle; // Column header
	short	m_siSort, // Sort direction (0 - not sorted, 1 - ascending, -1 decending
			m_siPropertiesPage; // Property page to activate upon a double click on the grid
	bool	m_bVisible, // The column is visible
			m_bEditable,// Cells in the column may be edited
			m_bAvailable; // The column is not available for the user to hide/show
	DWORD m_dwColumnID; // may be a fixed identifier or the locale


	ColumnHeader(DWORD dwColumnID, LPCTSTR pszTitle, short siPropertiesPage, short siSort, bool bVisible, bool bEditable, bool bAvailable)
		: m_dwColumnID(dwColumnID), m_sTitle(pszTitle), m_siPropertiesPage(siPropertiesPage), m_siSort(siSort), m_bVisible(bVisible), 
			m_bEditable(bEditable), m_bAvailable(bAvailable) {};

	ColumnHeader(DWORD dwColumnID, UINT uiTitle, short siPropertiesPage, short siSort, bool bVisible, bool bEditable, bool bAvailable)
		: m_dwColumnID(dwColumnID), m_siPropertiesPage(siPropertiesPage), m_siSort(siSort), m_bVisible(bVisible), 
			m_bEditable(bEditable), m_bAvailable(bAvailable)
	{
		m_sTitle.LoadString(uiTitle);
	};
};

class MessageGridView : public cdxCDynamicFormView
{
	DECLARE_DYNCREATE(MessageGridView)

protected: // create from serialization only
	MessageGridView();
	
	enum { IDD = IDD_GRIDFORMVIEW };

// Attributes
public:
	MessageGridDoc* GetDocument() const;

	bool m_bShowObsolete; // Show obsolete messages in the grid
	CNewMenu m_menuPopups;
	CGridCtrl m_Grid;
	CArray<ColumnHeader*, ColumnHeader*> m_rgColumnHeaders;

	ColumnHeader* GetColumnHeader(DWORD dwColumnID);
	ColumnHeader* GetColumnHeader(LPCTSTR pszHeader);
	int GetColumnIndex(DWORD dwColumnID);
	void MakeColumnVisible(DWORD dwColumnID, bool bVisible);
	void MoveColumn(int iPrevIndex, int iNewIndex);
	int GetMessageRow(ULONG ulMessageID);
	bool ConstructTestMessage(CString& sMessage, CStringArray& rgsParams, CString& sError);
	bool CreateDefaultMessage(int iRow);

	// Validation
	bool ValidateHelpID(int iRow);
	bool ValidateStyle(int iRow);
	bool ValidateObsolete(int iRow);
	bool ValidateAuditEvent(int iRow);
	bool ValidateMessageType(int iRow);
	Message::enumMsgErrors ValidateMessage(int iRow, int iCol);
	bool GotoNextAnomoly(const COLORREF clr);

	bool InitialiseGridCtrl();
	bool PopulateGrid();
	bool InsertNodeIntoGrid(MSXML2::IXMLDOMNodePtr& spMessage, int iRow = -1);
	bool SaveCellChange(int iRow, int iCol);
	void EditCell(int iRow, int iCol);
	bool InitialiseColumns();
    int InsertAddParamRow();

// Overrides
public:
	virtual void OnDraw(CDC* pDC);  // overridden to draw this view
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual void OnInitialUpdate();
	virtual BOOL OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo);

protected:
	virtual BOOL OnPreparePrinting(CPrintInfo* pInfo);
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	

	static int CALLBACK pfnCellCompare(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);

// Implementation
public:
	virtual ~MessageGridView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif


// Generated message map functions
protected:

	afx_msg void OnSize(UINT nType, int cx, int cy);

	afx_msg void OnGridSelectionChange(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridBeginEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridEndEdit(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridDblClick(NMHDR *pNotifyStruct, LRESULT* pResult);
	afx_msg void OnGridRightClick(NMHDR *pNotifyStruct, LRESULT* pResult);

	afx_msg void OnGridProperties();
	afx_msg void OnGridColumnselector();
	afx_msg void OnGridDeletemessage();
	afx_msg void OnGridInsertmessage();
	afx_msg void OnGridMakeobsolete();
	afx_msg void OnGridShowobsolete();
	afx_msg void OnTestMessageBox();
	afx_msg void OnSendMessageToClipboard();
	afx_msg void OnTimer(UINT nIDEvent);

	inline afx_msg void OnGridGotoNextError() { GotoNextAnomoly(CLR_ERROR_LEVEL_3); }
	inline afx_msg void OnGridGotoNextWarning() { GotoNextAnomoly(CLR_ERROR_LEVEL_1); }
	inline afx_msg void OnGridGotoNextSpellingMistake() { GotoNextAnomoly(CLR_ERROR_LEVEL_2); }

	afx_msg LRESULT OnRefreshGridMessage(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshParameters(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnNewMessageColumn(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshGrid(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT	OnEmptyGrid(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnValidateMsgGrid(WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnRefreshErrorDescription(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP();
	DECLARE_DYNAMIC_MAP();
	
};

#ifndef _DEBUG  // debug version in MessageGridView.cpp
inline MessageGridDoc* MessageGridView::GetDocument() const
   { return reinterpret_cast<MessageGridDoc*>(m_pDocument); }
#endif

