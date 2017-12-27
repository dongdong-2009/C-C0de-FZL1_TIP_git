/*
 * The soiurce code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form
 *
 * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tab_dialog_control/tabdialogctrl.h $
 * @author:  Andy Parker
 * @version: $Revision: #2 $
 *
 * Last mofification: $DateTime: 2015/01/23 16:31:53 $
 * Last modified by:  $Author: limin.zhu $
 *
 * TabDialogCtrl.h : header file
 *
 * This class enhances the tab dialog to enable it to create and manage the display of a series
 * of modeless dialogs.  It will size them to stay within the tab control and enable the one 
 * associated with the tab selected.
 *
 * This has been initially developed for C830 radio and telephone HMI's but is not project specific
 *
 * To use, simply derive your class from it and add your dialog objects in the constructor.using the 
 * AddPage method.  Place a tabctrl on your form with the resource editor and subclass it with an 
 * instance of your derived class.  This can be done by assigning a tabctrl control variable in class wizard
 * and then replacing it with your derived class type.  You should be able to create a resource class
 * dynamically as well but I haven't tested that yet!
 */
#if !defined(MFC_EXTENSION_TAB_DIALOG_CTRL_H)
#define MFC_EXTENSION_TAB_DIALOG_CTRL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


#include <vector>

class AFX_EXT_CLASS CTabDialogCtrl : public CTabCtrl
{
	// Construction
	public:
		CTabDialogCtrl();

	// Attributes
	public:
		CDialog*  getActiveDialog() {return m_activeDialog;}

	// Operations
	public:

		int SetCurSel(int nItem);

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CTabDialogCtrl)
		protected:
		virtual void PreSubclassWindow();
		//}}AFX_VIRTUAL

	// Implementation
	public:
		virtual ~CTabDialogCtrl(); 

		// Generated message map functions
	protected:

		void AddPage(CDialog&  dlg,UINT TemplateId, CString& label);
		void DelPage(UINT templateID);
		void EnablePage(UINT templateID);
		void DisablePage(UINT templateID);
		void SetCurTab( UINT templateID );
		void ResizeDialogs();
		void resetAllPages();

		//{{AFX_MSG(CTabDialogCtrl)
		afx_msg BOOL OnSelchange(NMHDR* pNMHDR, LRESULT* pResult);
		afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
		afx_msg void OnDestroy();
		afx_msg void OnMove(int x, int y);
		afx_msg void OnPaint();
        afx_msg void OnShowWindow(BOOL bShow, UINT nStatus);
		//}}AFX_MSG

	private:

		void CreateDialogs();
		void DisplayDialog(UINT tabPage);

		struct DialogInfo
		{
			bool       bValid;
			CDialog*   pDlg;
			UINT       nTemplateID;
			CString    label;
		};

		std::vector<DialogInfo>	m_dlgInfo;
		CDialog*				m_activeDialog;
		int                     m_selectedTab;

		DECLARE_MESSAGE_MAP()


};


///////////////////////////////////////////////////////////////////////////
//{{AFX_INSERT_LOCATION
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.
//}}AFX_INSERT_LOCATION

//#undef  AFX_DATA
//#define AFX_DATA

#endif // !defined(MFC_EXTENSION_TAB_DIALOG_CTRL_H)
