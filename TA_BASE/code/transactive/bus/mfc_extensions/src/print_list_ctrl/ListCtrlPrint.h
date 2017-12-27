/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/print_list_ctrl/ListCtrlPrint.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// ListCtrlPrint.h: interface for the CListCtrlPrint class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_LISTCTRLPRINT_H__E1B3DDE2_9D53_11D1_8BAC_0000B43382FE__INCLUDED_)
#define AFX_LISTCTRLPRINT_H__E1B3DDE2_9D53_11D1_8BAC_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#include "bus\mfc_extensions\src\print_list_ctrl\CellColourMap.h"
#include "bus\mfc_extensions\src\print_list_ctrl\PrinterJob.h"
#include <vector>

#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CListCtrlPrint : public CPrinterJob
#else
class AFX_EXT_CLASS CListCtrlPrint : public CPrinterJob
#endif
{
	DECLARE_DYNCREATE(CListCtrlPrint)

public:
	CListCtrlPrint();
	virtual ~CListCtrlPrint();

	// Since this is a thread these are parameters to receive from the calling class.
	struct t_ParamsToReceive
	{
        CString title;
		CString csPageHead;
		CString csPage;
		int * pColWidthArray;
		CListCtrl* list;
        TA_Base_Bus::CellColourMap* colourMap;
		
		// TO DO: Direct printing of the image instead of text
		std::vector<CString>* imageStringVector;

        t_ParamsToReceive() : title(""), csPageHead(""), csPage(""), pColWidthArray(NULL), list(NULL), colourMap(NULL) {}
	};
	
	CRect rcMarginMM;	// contain the margins in millimeters
	CFont	ftPrint, ftHead, ftFoot;	// fonts used for printing
	CSize	szFtPrint, szFtHead, szFtFoot;	// sizes of fonts (in device unit)
	int	*	pDx, * pFormat;	// datas about column size and format (center, right ..)
	char ** pColTitle;	// columns titles
	int		iItemXPage, iNumCol, iTotItem;	// number of item in page, of columns and of tot number of items in list control
	CListCtrl * pList;	// pointer of the list ctrl
	std::vector<CString>* pImageStringVector;

	// TD 12763 pointer to the array of col width if the default width from pList is not used
	// Take note that the array is starting with index 1 not 0
	int * pColWidthArray;

    TA_Base_Bus::CellColourMap* pColourMap;  // The class that maps cell coordinates to colour information.
	CString	csPageHead;	// text of header
	CString csPageFooter;	// text of footer
	CString csPage;	// format of page number / tot pages (ex. page %d of %d)

	CString	csFtHead, csFtFoot, csFtPrint;	// fonts type names
	int     iFtHead, iFtFoot, iFtPrint;	// fonts sizes

	CSize szFoot;	// internal use

	// this subtracts the margins from the page size
	void SubtractMargin(CDC * pDC, CRect &rc);

	// virtual overridden from base class; same meaning of CView's one
	void OnBeginPrinting(CDC * pDC, CPrintInfo * pInfo);
	void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);
	bool OnPreparePrinting(CPrintInfo* pInfo, bool bPrintPreview = false);
	void OnPrint(CDC* pDC, CPrintInfo* pInfo);

	// function self explicative; the items will be printed in the OnPrint function
	// if bCalc is true, the function is called to compute the space for items and so
	// no printing is needed, but only to return the size used and to subtract from the
	// rcpage the space used
	CSize PrintListFoot(CDC * pDC, CRect & rcPage, bool bCalc);
	CSize PrintListHead(CDC * pDC, CRect & rcPage, bool bCalc);
	CSize PrintPageFoot(CDC * pDC, CRect & rcPage, bool bCalc);
	CSize PrintPageHead(CDC * pDC, CRect & rcPage, const int nPage, const int iMaxPage, bool bCalc);
	//TD14936
	//Print the selected alarms
	void PrintSelection(CDC* pDC, CPrintInfo* pInfo);

private:
	//TD15057 && TD18775
	//Get the total pages will be printed
	long GetTotalPages(CDC * pDC,int vnHightPerRow,CPrintInfo* pInfo);

	afx_msg void threadedPrint(WPARAM wParam, LPARAM lParam);

	// This function will insert space to a long string or text
	// if the string or text is wider than its column
	CString GetTextSizedToColumn(CDC * pDC, int nItem, int nSubitem, CRect colRect);



	std::vector<bool> m_colToHide;
};

#endif // !defined(AFX_LISTCTRLPRINT_H__E1B3DDE2_9D53_11D1_8BAC_0000B43382FE__INCLUDED_)

