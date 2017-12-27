// NewProperty.cpp : implementation file
//

#include "stdafx.h"
#include "MenuProperty.h"

// CMenuPropertyPage dialog

IMPLEMENT_DYNAMIC(CMenuPropertyPage,  CPropertyPage)
CMenuPropertyPage::CMenuPropertyPage()
{
}

CMenuPropertyPage::~CMenuPropertyPage()
{
}

CMenuPropertyPage::CMenuPropertyPage(UINT nIDTemplate, UINT nIDCaption, DWORD dwSize)
{
  free(m_pPSP);
  m_pPSP=NULL;

	ASSERT(nIDTemplate != 0);
	AllocPSP(dwSize);
	CommonConstruct(MAKEINTRESOURCE(nIDTemplate), nIDCaption);
}

CMenuPropertyPage::CMenuPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption , DWORD dwSize)
{
  free(m_pPSP);
  m_pPSP=NULL;

	ASSERT(AfxIsValidString(lpszTemplateName));
	AllocPSP(dwSize);
	CommonConstruct(lpszTemplateName, nIDCaption);
}

// extended construction
CMenuPropertyPage::CMenuPropertyPage(UINT nIDTemplate, UINT nIDCaption,UINT nIDHeaderTitle, UINT nIDHeaderSubTitle, DWORD dwSize)
{
  free(m_pPSP);
  m_pPSP=NULL;

	ASSERT(nIDTemplate != 0);
	AllocPSP(dwSize);
	CommonConstruct(MAKEINTRESOURCE(nIDTemplate), nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle);
}

CMenuPropertyPage::CMenuPropertyPage(LPCTSTR lpszTemplateName, UINT nIDCaption, 	UINT nIDHeaderTitle, UINT nIDHeaderSubTitle, DWORD dwSize)
{
  free(m_pPSP);
  m_pPSP=NULL;

	ASSERT(AfxIsValidString(lpszTemplateName));
	AllocPSP(dwSize);
	CommonConstruct(lpszTemplateName, nIDCaption, nIDHeaderTitle, nIDHeaderSubTitle);
}


BEGIN_MESSAGE_MAP(CMenuPropertyPage,  CMenuFrame<CPropertyPage>)
END_MESSAGE_MAP()

// CMenuPropertyPage dialog

IMPLEMENT_DYNAMIC(CMenuPropertySheet,  CPropertySheet)
CMenuPropertySheet::CMenuPropertySheet()
{
}

CMenuPropertySheet::~CMenuPropertySheet()
{
}

CMenuPropertySheet::CMenuPropertySheet(UINT nIDCaption, CWnd* pParentWnd,	UINT iSelectPage)
{
	ASSERT(nIDCaption != 0);

	VERIFY(m_strCaption.LoadString(nIDCaption));
	CommonConstruct(pParentWnd, iSelectPage);
}

CMenuPropertySheet::CMenuPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd, UINT iSelectPage)
{
	ASSERT(pszCaption != NULL);

	m_strCaption = pszCaption;
	CommonConstruct(pParentWnd, iSelectPage);
}

// extended construction
CMenuPropertySheet::CMenuPropertySheet(UINT nIDCaption, CWnd* pParentWnd,	UINT iSelectPage, HBITMAP hbmWatermark,	HPALETTE hpalWatermark, HBITMAP hbmHeader)
{
	ASSERT(nIDCaption != 0);

	VERIFY(m_strCaption.LoadString(nIDCaption));
	CommonConstruct(pParentWnd, iSelectPage);
}

CMenuPropertySheet::CMenuPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd,	UINT iSelectPage, HBITMAP hbmWatermark,	HPALETTE hpalWatermark , HBITMAP hbmHeader)
{
	ASSERT(pszCaption != NULL);

	m_strCaption = pszCaption;
	CommonConstruct(pParentWnd, iSelectPage);
}

BEGIN_MESSAGE_MAP(CMenuPropertySheet,  CMenuFrame<CPropertySheet>)
END_MESSAGE_MAP()



// CMenuPropertyPage message handlers
