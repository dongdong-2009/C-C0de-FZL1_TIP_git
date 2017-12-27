#if !defined(AFX_MULTIFONTPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_)
#define AFX_MULTIFONTPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_

/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/multi_font_property_sheet/MultiFontPropertySheet.h $
  * @author:  Rob Young
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2016/09/27 10:01:34 $
  * Last modified by:  $Author: hoa.le $
  * 
  * This derivation of CPropertySheet provides a workaround for two MFC CPropertySheet Bugs:
  *
  * 1) Resetting/resizing of the CPropertyPages when not of system font. 
  * REFER to MSDN Knowledge Base Article 142170 - "PRPFONT - How to Set CPropertySheet Fonts"
  *
  * 2) Child CPropertySheet Hangs If Focus Is Switched. 
  * Refer to MSDN Knowledge Base Article 149501 - "PRB: Child CPropertySheet Hangs If Focus Is Switched"
  *
  * To use, simply substitute CMultiFontPropertySheet wherever you would use CPropertySheet.
  *
  */

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxtempl.h>
#include <vector>

// MultiFontPropertySheet.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CMultiFontPropertySheet

// This guard is so that this MFC extension can be used in another
// MFC extension DLL
//
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CMultiFontPropertySheet : public CPropertySheet
#else
class AFX_EXT_CLASS CMultiFontPropertySheet : public CPropertySheet
#endif
{
protected:
	RECT m_rctPage;
	CFont m_fntPage;

	DECLARE_DYNAMIC(CMultiFontPropertySheet)

// Construction
public:
	CMultiFontPropertySheet(UINT nIDCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, BOOL fPageFont = false);
	CMultiFontPropertySheet(LPCTSTR pszCaption, CWnd* pParentWnd = NULL, UINT iSelectPage = 0, BOOL fPageFont = false);
	 
// Attributes
public:
private:
	/**
	* Flag for page font setting
	*/
	BOOL m_fPageFont;

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CMultiFontPropertySheet)
	public:
	virtual BOOL OnInitDialog();
	virtual int DoModal();
	protected:
	virtual BOOL OnNotify(WPARAM wParam, LPARAM lParam, LRESULT* pResult);
	virtual BOOL OnCommand(WPARAM wParam, LPARAM lParam);

	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CMultiFontPropertySheet();

	// Generated message map functions
protected:
	virtual void BuildPropPageArray ();

	//{{AFX_MSG(CMultiFontPropertySheet)
	//}}AFX_MSG
	afx_msg LONG OnResizePage (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPageAdded (WPARAM wParam, LPARAM lParam);
	afx_msg LRESULT OnPageRemoved(WPARAM wParam, LPARAM lParam);
	DECLARE_MESSAGE_MAP()



	bool m_isModal;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_MULTIFONTPROPERTYSHEET_H__F59F5AFF_C911_4D4E_B0CF_58AEAFE5B8EC__INCLUDED_)
