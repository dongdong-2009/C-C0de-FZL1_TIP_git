/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/readonly_enabled_listbox/ReadOnlyListBox.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_LISTBOXEX_H__936A758C_0514_4EE7_B5A8_E753B8D3C47D__INCLUDED_)
#define AFX_LISTBOXEX_H__936A758C_0514_4EE7_B5A8_E753B8D3C47D__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// ListBoxEx.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CReadOnlyListBox window


namespace TA_Base_Bus
{ 
	#ifdef IMPORT_EXTENSIONS
    class __declspec(dllimport) CReadOnlyListBox : public CListBox
    #else
    class AFX_EXT_CLASS CReadOnlyListBox : public CListBox
    #endif
    {
    		// Construction
	public:
		CReadOnlyListBox();
		
		// Attributes
	public:
		
		// Operations
	public:
		
		// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CReadOnlyListBox)
	public:
	protected:
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL
		
		// Implementation
	public:
		virtual ~CReadOnlyListBox();
		BOOL EnableWindow(BOOL bEnable = TRUE);
		void SetBkColor(COLORREF crBkColor,COLORREF crSelectedColor = GetSysColor(COLOR_WINDOW));
		// Generated message map functions
		//{{AFX_MSG(CReadOnlyListBox)
		afx_msg void OnEnable(BOOL bEnable);
		afx_msg HBRUSH CtlColor(CDC* pDC, UINT nCtlColor);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()
			
	protected:
		CBrush m_bkBrush; 
		BOOL   m_bReadOnly;
		
		static const COLORREF COLOR_ENABLED;
		static const COLORREF COLOR_DISABLED;
		
	};
	
}
/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_LISTBOXEX_H__936A758C_0514_4EE7_B5A8_E753B8D3C47D__INCLUDED_)
