/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/tab_control_with_disable/TabCtrlWithDisable.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
#if !defined(AFX_TABCTRLWITHDISABLE_H__0C234C92_EBB8_4ED3_8D96_E672291DB10B__INCLUDED_)
#define AFX_TABCTRLWITHDISABLE_H__0C234C92_EBB8_4ED3_8D96_E672291DB10B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


////////////////////////////////////////////////////////////////
// 1998 Microsoft Systems Journal. 
// If this code works, it was written by Paul DiLascia.
// If not, I don't know who wrote it.
// Compiles with Visual C++ 5.0 on Windows 95
// See DisabTab.cpp
// 
#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CTabCtrlWithDisable : public CTabCtrl
#else
class AFX_EXT_CLASS CTabCtrlWithDisable : public CTabCtrl
#endif
{
   DECLARE_DYNAMIC(CTabCtrlWithDisable)
public:
   CTabCtrlWithDisable();
   virtual ~CTabCtrlWithDisable();

   // functions you must implement/call
   virtual  BOOL IsTabEnabled(int iTab) = 0;          // you must override
   BOOL     TranslatePropSheetMsg(MSG* pMsg);         // call from prop sheet
   BOOL     SubclassDlgItem(UINT nID, CWnd* pParent); // non-virtual override

   // helpers
   int      NextEnabledTab(int iTab, BOOL bWrap);  // get next enabled tab
   int      PrevEnabledTab(int iTab, BOOL bWrap);  // get prev enabled tab
   BOOL     SetActiveTab(UINT iNewTab);            // set tab (fail if disabled)

protected:
   DECLARE_MESSAGE_MAP()
   afx_msg  BOOL OnSelChanging(NMHDR* pNmh, LRESULT* pRes);

   // MFC overrides
   virtual  BOOL PreTranslateMessage(MSG* pMsg);
   virtual  void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

   // override to draw text only; eg, colored text or different font
   virtual  void OnDrawText(CDC& dc, CRect rc, CString sText, BOOL bDisabled);
};

#endif // !defined(AFX_TABCTRLWITHDISABLE_H__0C234C92_EBB8_4ED3_8D96_E672291DB10B__INCLUDED_)
