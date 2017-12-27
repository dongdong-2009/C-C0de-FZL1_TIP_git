// GridCtrlS.h: interface for the CGridCtrlS class.
//
//////////////////////////////////////////////////////////////////////

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/grid_control/GridCtrlS.h $
  * @author:  Azenith Arcilla
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  * This class extends CGridCtrl,
  * to complete the requirements in TD15533:
  * display 2 images and 2 text in one cell.
  *
  */

#if !defined(AFX_GRIDCTRLS_H__4D978C2F_3EEB_45FA_80F6_0C179AD6BBDF__INCLUDED_)
#define AFX_GRIDCTRLS_H__4D978C2F_3EEB_45FA_80F6_0C179AD6BBDF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "GridCtrl.h"

#define SEPARATOR " / " //TD15533
#define EMPTY_SPACE "     " //TD15533

#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CGridCtrlS : public CGridCtrl
#else
class AFX_EXT_CLASS CGridCtrlS : public CGridCtrl 
#endif 
{
public:
	CGridCtrlS();
	virtual ~CGridCtrlS();

	BOOL   SetItemS(const GV_ITEM* pItem); //TD15533
	BOOL   GetItemS(GV_ITEM* pItem); //TD15533
	BOOL   SetItemTextS(int nRow, int nCol, LPCTSTR str); //TD15533
	virtual CString GetItemTextS(int nRow, int nCol); //TD15533
	BOOL   SetItemImageS(int nRow, int nCol, int iImage); //TD15533
	int    GetItemImageS(int nRow, int nCol) const; //TD15533
	BOOL   SetItemOverrided(int nRow, int nCol, BOOL bOverrided); //TD15533
	BOOL   GetItemOverrided(int nRow, int nCol) const; //TD15533

protected:
	virtual void EmptyCell(CGridCell* pCell, int nRow, int nCol);
	afx_msg void OnMouseMove(UINT nFlags, CPoint point);
	virtual BOOL  DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk=FALSE);
};;

#endif // !defined(AFX_GRIDCTRLS_H__4D978C2F_3EEB_45FA_80F6_0C179AD6BBDF__INCLUDED_)
