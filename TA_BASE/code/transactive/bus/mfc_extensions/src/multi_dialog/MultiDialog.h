/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/multi_dialog/MultiDialog.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// MultiDialog.h: interface for the CMultiDialog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_MULTIDIALOG_H__6084EBF2_F15C_4DE4_87BA_76FA9300075F__INCLUDED_)
#define AFX_MULTIDIALOG_H__6084EBF2_F15C_4DE4_87BA_76FA9300075F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <list>

// Make this work in a DLL
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) class CMultiDialog : public CDialog
#else
class AFX_EXT_CLASS CMultiDialog : public CDialog
#endif
{
	typedef std::list<CWnd*> WndList;
	typedef std::list<CWnd*>::iterator WndListIter;

	typedef std::list<CFont*> FontList;
	typedef std::list<CFont*>::iterator FontListIter;
	
	WndList m_ChildList;		// List of created child windows
	FontList m_FontList;		// List of created fonts
	CFont *m_pCurrentFont;
	
public:
	CMultiDialog() : childExists( false ), currentDlgId( 0 ) {};
	//CMultiDialog( CWnd* pParent = NULL ) : CDialog( CTestProjectDlg::IDD, pParent) {};
	CMultiDialog( LPCTSTR lpszTemplateName, CWnd *pParent = NULL ) : CDialog( lpszTemplateName, pParent ), childExists( false ), currentDlgId( 0 ) {}
	CMultiDialog( UINT nIDTemplate,	CWnd *pParent = NULL ) : CDialog( nIDTemplate, pParent ), childExists( false ), currentDlgId( 0 ) {};

	virtual ~CMultiDialog() {};

	void AddDialog( UINT id, UINT idMarker );

	virtual void PostNcDestroy();
	
private:
	WORD * ReadTemplateArray( WORD *pArray, WORD &ID, CString &name );
	DLGITEMTEMPLATE * AddDialogItem( DLGITEMTEMPLATE *pItem, LPRECT pRect );

	UINT currentDlgId; // Store the current child dialog showing
	bool childExists;
};

#endif // !defined(AFX_MULTIDIALOG_H__6084EBF2_F15C_4DE4_87BA_76FA9300075F__INCLUDED_)

