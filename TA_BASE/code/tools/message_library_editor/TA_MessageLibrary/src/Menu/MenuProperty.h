#pragma once


// CMenuPropertyPage dialog

class CMenuPropertyPage : public CMenuFrame<CPropertyPage>
{
	DECLARE_DYNAMIC(CMenuPropertyPage)

public:
	CMenuPropertyPage();
	virtual ~CMenuPropertyPage();

  CMenuPropertyPage(UINT nIDTemplate, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	CMenuPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));

	// extended construction
	CMenuPropertyPage(UINT nIDTemplate, UINT nIDCaption,UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));
	CMenuPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption, 	UINT nIDHeaderTitle, UINT nIDHeaderSubTitle = 0, DWORD dwSize = sizeof(PROPSHEETPAGE));

protected:
	DECLARE_MESSAGE_MAP()
};

class CMenuPropertySheet : public CMenuFrame<CPropertySheet>
{
	DECLARE_DYNAMIC(CMenuPropertySheet)

public:
	CMenuPropertySheet();
	virtual ~CMenuPropertySheet();

	CMenuPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL,	UINT iSelectPage = 0);
	CMenuPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0);

	// extended construction
	CMenuPropertySheet(UINT nIDCaption, CWnd* pParentWnd,	UINT iSelectPage, HBITMAP hbmWatermark,	HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);
	CMenuPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd,	UINT iSelectPage, HBITMAP hbmWatermark,	HPALETTE hpalWatermark = NULL, HBITMAP hbmHeader = NULL);

protected:
	DECLARE_MESSAGE_MAP()
};


