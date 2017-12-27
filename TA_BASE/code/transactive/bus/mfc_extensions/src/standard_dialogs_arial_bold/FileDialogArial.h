/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/standard_dialogs_arial_bold/FileDialogArial.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  * 
  * This converts all the fields in the FileOpen dialogue into an Arial Bold 10pt
  * font. Instead of using CFileDialog just use CFileDialogArial and use as per
  * normal.
  *
  * NOTE: This still does not convert the list control to bold arial. This needs
  *       to be added.
  */

#if !defined(AFX_FONTFILEDIALOG_H__66EAB521_4E7D_4B73_BB16_A3077486A82A__INCLUDED_)
#define AFX_FONTFILEDIALOG_H__66EAB521_4E7D_4B73_BB16_A3077486A82A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// FontFileDialog.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CFileDialogArial dialog

class AFX_EXT_CLASS CFileDialogArial : public CFileDialog
{
	DECLARE_DYNAMIC(CFileDialogArial)

public:
	CFileDialogArial(BOOL bOpenFileDialog, // TRUE for FileOpen, FALSE for FileSaveAs
		LPCTSTR lpszDefExt = NULL,
		LPCTSTR lpszFileName = NULL,
		DWORD dwFlags = OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT,
		LPCTSTR lpszFilter = NULL,
		CWnd* pParentWnd = NULL);

protected:
	//{{AFX_MSG(CFileDialogArial)
	virtual BOOL OnInitDialog();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

    CFont m_font;    // This will be set up as the font to apply to each control
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_FONTFILEDIALOG_H__66EAB521_4E7D_4B73_BB16_A3077486A82A__INCLUDED_)
