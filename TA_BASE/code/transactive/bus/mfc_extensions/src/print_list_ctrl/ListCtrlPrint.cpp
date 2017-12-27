/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/print_list_ctrl/ListCtrlPrint.cpp $
  * @author:  Ripple
  * @version: $Revision: #3 $
  *
  * Last modification: $DateTime: 2016/01/18 15:18:43 $
  * Last modified by:  $Author: Ouyang $
  *
  */
// ListCtrlPrint.cpp: implementation of the CListCtrlPrint class.
//
//////////////////////////////////////////////////////////////////////

#pragma warning (disable:4786)

#include "bus\mfc_extensions\src\stdafx.h"
#include "bus\mfc_extensions\src\print_list_ctrl\ListCtrlPrint.h"
#include "bus\mfc_extensions\src\resource.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////

IMPLEMENT_DYNCREATE(CListCtrlPrint, CPrinterJob)

CListCtrlPrint::CListCtrlPrint()
{
	rcMarginMM = CRect(10,10,10,10);
	szFtPrint = szFtHead = szFtFoot = CSize(0,0);
	pDx = NULL;
	iItemXPage = 1;
	iNumCol = 0;
	pList = NULL;
	pColWidthArray = NULL;
	szFoot = CSize(0,0);
	pFormat = NULL;
	pColTitle = NULL;
	iTotItem = 0;

	csFtHead = "Arial";
	csFtFoot = "Times New Roman";
	csFtPrint = "Arial";
	iFtHead = 10;
	iFtFoot = 6;
	iFtPrint = 8;
	csPage = "Page %d of %d";
	pImageStringVector = NULL;
}

CListCtrlPrint::~CListCtrlPrint()
{
	if (iNumCol) 
	{
		if (pDx) delete [] pDx;
		if (pFormat) delete [] pFormat;
		if (pColTitle)
		{
			for (int t = 0; t < iNumCol; t++)
				if (pColTitle[t]) delete [] pColTitle[t];
			delete [] pColTitle;
		}
	}
}

bool CListCtrlPrint::OnPreparePrinting(CPrintInfo* pInfo, bool bPrintPreview)
{
	pInfo->SetMinPage(1);
	if (!pList) return false;
	iTotItem = pList->GetItemCount();
	
	if (iTotItem <= 0)
		return false;

	if (pDx)
	{
		delete pDx;
		pDx = NULL;
	}

	if (pFormat)
	{
		delete pFormat;
		pFormat = NULL;
	}

	if (pColTitle)
	{
		delete pColTitle;
		pColTitle = NULL;
	}

	m_colToHide.clear();

	CHeaderCtrl* pHeader = (CHeaderCtrl*)(pList->GetDlgItem(0));
	ASSERT(pHeader);
	iNumCol = 0;
	// Count the number of non-zero columns
	for (int colNum=0; colNum<pHeader->GetItemCount(); ++colNum)
	{
		HDITEM hiWid;
		hiWid.mask = HDI_WIDTH;
		if ( pHeader->GetItem(colNum, &hiWid) )
		{
            //TD14766 print column even it's hidden.
			// if (hiWid.cxy > 0)
			{
				++iNumCol;
				m_colToHide.push_back(false);
			}
			// else
			// {
			//  	m_colToHide.push_back(true);
			// }
		}
		else // can't determine the width, so hide the column
		{
			m_colToHide.push_back(true);
		}
	}
	if (iNumCol <= 0) return false;
	
	pDx = new int[iNumCol+1];
	pFormat = new int[iNumCol];
	pColTitle = new char *[iNumCol];
	ZeroMemory(pColTitle, sizeof(char *) * iNumCol);
	ZeroMemory(pDx, sizeof(int) * (iNumCol + 1));
	ZeroMemory(pFormat, sizeof(int) * iNumCol);

	char txt[128];
	int i = 0; //count for iNumCol
	for (unsigned int t = 0; t <m_colToHide.size(); t++)
	{
		if (m_colToHide[t] == true) continue;
		
		if (i < iNumCol )
		{
			HDITEM hi;
			hi.cchTextMax = 128;
			hi.pszText    = txt;

			hi.mask = HDI_TEXT|HDI_WIDTH|HDI_FORMAT;
			if (pHeader->GetItem(t, &hi))
			{
				// TD 12763 enable changing of printout col width
				if (pColWidthArray==NULL)
				{
					pDx[i+1] = hi.cxy;
				}
				else
				{
					pDx[i+1] = pColWidthArray[t+1];
				}

				pFormat[i] = hi.fmt;
				if (hi.cchTextMax > 0 && hi.pszText)
				{
					unsigned int uTextSize = lstrlen(hi.pszText) + 1;
					if (uTextSize > hi.cchTextMax)
					{
						pColTitle[i] = new char[uTextSize];
						ASSERT(pColTitle[i]);
						memset(pColTitle[i], 0, uTextSize);
					}
					else
					{
					pColTitle[i] = new char[hi.cchTextMax];
					ASSERT(pColTitle[i]);
						memset(pColTitle[i], 0, hi.cchTextMax);
					}			
					lstrcpy(pColTitle[i], hi.pszText);
				}
			}
			++i;
		}
	}
	//TD15057 && TD18775
	m_lTotalRows = iTotItem;
	return CPrinterJob::OnPreparePrinting(pInfo, bPrintPreview);
}

void CListCtrlPrint::OnBeginPrinting(CDC * pDC, CPrintInfo * pInfo)
{
	ftPrint.CreatePointFont(iFtPrint*10, csFtPrint, pDC);
	ftHead.CreatePointFont(iFtHead*10, csFtHead, pDC);
	ftFoot.CreatePointFont(iFtFoot*10, csFtFoot, pDC);

	CString cs = "a";
	CFont * of = pDC->SelectObject(&ftPrint);
	szFtPrint = pDC->GetTextExtent(cs);
	pDC->SelectObject(&ftHead);
	szFtHead  = pDC->GetTextExtent(cs);
	pDC->SelectObject(&ftFoot);
	szFtFoot  = pDC->GetTextExtent(cs);
	pDC->SelectObject(of);

	szFtPrint.cy += 4;
	szFtHead.cy += 4;

	int t = 0;
	for (t = 1; t <= iNumCol; t++)
		pDx[t] += pDx[t-1];

	CRect rcPrint(pInfo->m_rectDraw);
	SubtractMargin(pDC, rcPrint);
	double db = rcPrint.Width();
	db /= (double) pDx[iNumCol];
	for (t = 0; t <= iNumCol; t++) pDx[t] = (int)double(pDx[t] * db);
	ASSERT(pList);

	PrintListHead(pDC, rcPrint, true);
	PrintPageHead(pDC, rcPrint, 0, 0, true);

	CSize sz1 = PrintListFoot(pDC, rcPrint, true);
	CSize sz2 = PrintPageFoot(pDC, rcPrint, true);
	szFoot.cy = sz1.cy+sz2.cy + 2;
	iItemXPage = rcPrint.Height() / (szFtPrint.cy + 1) -1;
	int nPages = iTotItem / iItemXPage + 1;
	
	nPages = GetTotalPages(pDC,szFtPrint.cy,pInfo);
	pInfo->SetMaxPage(nPages);

}

void CListCtrlPrint::OnEndPrinting(CDC* pDC, CPrintInfo* pInfo)
{
	ftPrint.DeleteObject();
	ftHead.DeleteObject();
	ftFoot.DeleteObject();
}

void CListCtrlPrint::SubtractMargin(CDC * pDC, CRect &rc)
{
	CSize upsz(rcMarginMM.left*100, rcMarginMM.top*100);
	CSize dwsz(rcMarginMM.right*100, rcMarginMM.bottom*100);
	pDC->HIMETRICtoDP(&upsz);
	pDC->HIMETRICtoDP(&dwsz);
	rc.top    += upsz.cy;
	rc.left   += upsz.cx;
	rc.right  -= dwsz.cx;
	rc.bottom -= dwsz.cy;
}

void CListCtrlPrint::OnPrint(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rcPrint(pInfo->m_rectDraw);

	SubtractMargin(pDC, rcPrint);
	int obkm = pDC->SetBkMode(TRANSPARENT);

	PrintPageHead(pDC, rcPrint, pInfo->m_nCurPage, (int) pInfo->GetMaxPage(), false);
	PrintListHead(pDC, rcPrint, false);

//	int iFirst = (pInfo->m_nCurPage - 1) * iItemXPage;
//	int iLast = (pInfo->m_nCurPage) * iItemXPage;
	int y = rcPrint.top;
	int x = rcPrint.left;

    CFont * oft = pDC->SelectObject(&ftPrint);
	CRect rc1;
	CString cs;
	int iMax = iTotItem;

	// Determine whether the device is a BW or Colour printer
	// TD 14559 remove BW printing
	bool isColourPrinter = true;
	//int colours = pDC->GetDeviceCaps(PLANES);
	//if (colours > 1) isColourPrinter = true; // there is more than 1 colour plane ie not black only. So, it is a colour Printer

    // When the list control is printed out, we want word wrapping done on the columns where 
    // the text is too long for the column width. If wrapping is done, then the height of the
    // column is extended. So the first thing we do is go through each column (for each row) 
    // and determine the maximum height that is needed so it can be applied for the whole row.
    // Then we draw the row.

	//Begin---- TD15057 && TD18775
	//Ensure to be printed correctly

	int iTotalRowsPerPage = iItemXPage; //Total rows can be printed in one page
	int iPrintedRows(0);				//The count of rows have been printed in a page 
	int iPreparePrintRows(0);			//The count of rows that a alarm will be printed
	int iPrintRowsOfAlarm(0);			//The count of rows that a alarm will be printed in current page
	CRect rc2;							//TD14173
	CRect rc3;							//TD14173
   // for (int i = iFirst; i < iLast; i++)
	while(m_lStartRowInPage < m_lTotalRows && iPrintedRows < iTotalRowsPerPage)
	{
	//	bool bIs = i < iMax;
        
        int nMaxHeight = szFtPrint.cy;
        int box = 0;

        // Determine the maximum height of the row
		int textCol = 0; //count for iNumCol
		for (unsigned int t = 0; t <m_colToHide.size() ; t++)
		{
			if (m_colToHide[t] == true)
			{
				continue;
			}
			
			if ( textCol<iNumCol ) // make sure the col does not exceed the array
			{
				rc1.SetRect(rcPrint.left+pDx[textCol] + 10, y, rcPrint.left+pDx[textCol+1], y+szFtPrint.cy); //TD14173

		//		if (bIs)
		//		{
				UINT ftm = DT_LEFT;                         
				cs = GetTextSizedToColumn(pDC, m_lStartRowInPage, t, rc1);
				//if (pFormat[t]&HDF_CENTER) ftm = DT_CENTER;
				//else if (pFormat[t]&HDF_RIGHT) ftm = DT_RIGHT;
				cs.TrimLeft();
				if (pColourMap!=NULL)
				{
					// Apply text colours.
					COLORREF cellColour = pColourMap->getDefaultColour();
					if (isColourPrinter) cellColour = pColourMap->getCellColour( t, m_lStartRowInPage );
					pDC->SetTextColor( cellColour );
				}
				
				CRect tmpRect = rc1;
				pDC->DrawText(cs, &tmpRect, ftm|DT_VCENTER|DT_WORDBREAK|DT_CALCRECT); 
			//	pDC->DrawText(cs, &tmpRect, ftm|DT_WORDBREAK|DT_CALCRECT); 
				nMaxHeight = max(nMaxHeight, tmpRect.Height());
					
		//		}

				++textCol; // next print col
			}
		}
		
		//when the alarm will be print in different pages ,determine the rows will be printed in this page and the 
		//start row printed in next page
		int iTemp;
		iTemp = abs((nMaxHeight-szFtPrint.cy)/34);
		iPreparePrintRows = iTemp + 1;
		
		if (iPrintedRows + iPreparePrintRows > iTotalRowsPerPage)
		{
			iPrintRowsOfAlarm = iTotalRowsPerPage - iPrintedRows;
			m_lStartWrapRow = iPrintRowsOfAlarm;
		}
		iPrintedRows += iPreparePrintRows;
		//if there has a alarm is printed in different pages,determine the height

        // Draw the row
     //   CRect rc2; //TD14173
		int printCol = 0; //count for iNumCol

		CEdit* pEdit = NULL;
		//TD14173----
		if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
		{
			pEdit = new CEdit;
			pEdit->Create(ES_MULTILINE|WS_CHILD|ES_LEFT|ES_AUTOVSCROLL|ES_WANTRETURN,rc2,pList,1);
			CFont* cfTemp;
			cfTemp = pDC->GetCurrentFont();
			pEdit->SetFont(cfTemp);
		}
		//----TD14173
		int nPrintHeight;
		if (iPrintRowsOfAlarm > 0)
		{
			nPrintHeight = szFtPrint.cy *iPrintRowsOfAlarm;
		}
		if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow > 0)
		{
			nPrintHeight = (iPreparePrintRows - m_lStartWrapRow)*szFtPrint.cy;	
		}
		//Begin---- TD14936
		if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow == 0)
		{
			nMaxHeight = iPreparePrintRows * szFtPrint.cy;
		}
		//End---- TD14936
		printCol = 0;
        for (unsigned int u = 0; u < m_colToHide.size(); u++)
		{
			if (m_colToHide[u] == true)
			{
				continue;
			}

			if ( printCol<iNumCol ) // make sure the col does not exceed the array
            {
				rc2.SetRect(rcPrint.left+pDx[printCol], y, rcPrint.left+pDx[printCol+1], y+nMaxHeight);
				rc3.SetRect(rcPrint.left+pDx[printCol] + 10, y, rcPrint.left+pDx[printCol+1], y+nMaxHeight); //TD14173
		//		if ( bIs )
		//		{
				
				
				cs = GetTextSizedToColumn(pDC, m_lStartRowInPage, u, rc3); //" " + pList->GetItemText(i, u);
				//TD14173----
				//TD14929
				cs.TrimLeft();
				cs.TrimRight();
				//if there has a alarm is printed in different pages,determine the context
				if (iPrintRowsOfAlarm > 0)
				{
					CString strTemp;						
					pEdit->SetRect(&rc3);
					pEdit->SetWindowText(cs);
					cs = "";
					for (int i=0;i<iPrintRowsOfAlarm;i++ )
					{
						char cTemp[501];
						memset(cTemp,0,sizeof(cTemp));
						int iret = pEdit->GetLine(i,cTemp,500);
						memset(cTemp+iret,0,500-iret);
						strTemp = cTemp;
						strTemp.TrimLeft();
						strTemp.TrimRight();
						//TD14929						
						cs += strTemp;
					}
				}
				if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow > 0)
				{				
					CString strTemp;
					pEdit->SetRect(&rc3);
					pEdit->SetWindowText(cs);
					cs = "";
					for (int i=m_lStartWrapRow;i<iPreparePrintRows;i++ )
					{
						char cTemp[501];
						memset(cTemp,0,sizeof(cTemp));
						int iret = pEdit->GetLine(i,cTemp,500);
						memset(cTemp+iret,0,500-iret);
						strTemp = cTemp;
						strTemp.TrimLeft();
						strTemp.TrimRight();
						//TD14929						
						cs += strTemp;
					}
				}				
				if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
				{				
					rc3.SetRect(rcPrint.left+pDx[printCol] + 10, y, rcPrint.left+pDx[printCol+1], y+nPrintHeight + 1);
				}
				else
				{
					rc3.SetRect(rcPrint.left+pDx[printCol] + 10, y, rcPrint.left+pDx[printCol+1], y+szFtPrint.cy*iPreparePrintRows + 1);
				}
				//----TD14173
				// If no colour map is specified the list is printed like it always has been.
				if ( NULL != pColourMap )
				{	// Determine the brush to use when filling the cell and the text colour
					// and fill the cell with the required colour.
					CBrush* cellBkBrush = pColourMap->getDefaultBkBrush();
					if (isColourPrinter) cellBkBrush = pColourMap->getCellBkBrush( u, m_lStartRowInPage );
					pDC->FillRect( rc2, cellBkBrush );
				}

				UINT ftm = DT_LEFT;
				//if (pFormat[t]&HDF_CENTER) ftm = DT_CENTER;
				//else if (pFormat[t]&HDF_RIGHT) ftm = DT_RIGHT;
				pDC->DrawText(cs, rc3, ftm|DT_VCENTER|DT_WORDBREAK);	//TD14173
		//		}
				if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
				{					
					pDC->MoveTo(rc2.left,y+nPrintHeight);
				}
				else
				{
					pDC->MoveTo(rc2.left,y+nMaxHeight);
				}
				
				pDC->LineTo(rc2.left,rc2.top);
				pDC->LineTo(rc2.right,rc2.top);
			
				++printCol; // next print col
			}
        }
		if (pEdit != NULL)
		{
			delete pEdit;
			pEdit = NULL;
		}
		//Begin---- TD14936
		if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
		{					
			pDC->LineTo(rc2.right,y+nPrintHeight);
			y += nPrintHeight;	
		}
		else
		{
			pDC->LineTo(rc2.right,y+nMaxHeight);
			y += nMaxHeight;
		}
		//End---- TD14936
		if (iPrintRowsOfAlarm == 0 )
		{
			m_lStartRowInPage ++;
		}
		if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow > 0)
		{					
			m_lStartWrapRow = 0;
		}
	}

	//End---- TD15057 && TD18775

	pDC->MoveTo(rcPrint.left,y);
	pDC->LineTo(rcPrint.right,y);
	pDC->SelectObject(oft);

	rcPrint = pInfo->m_rectDraw;
	SubtractMargin(pDC, rcPrint);
	rcPrint.top = rcPrint.bottom - szFoot.cy;

	PrintListFoot(pDC, rcPrint, false);
	PrintPageFoot(pDC, rcPrint, false);

	pDC->SetBkMode(obkm);
}


CString CListCtrlPrint::GetTextSizedToColumn(CDC * pDC, int nItem, int nSubitem, CRect colRect)
{
	//Determine the width of the text and insert space if it is longer than col width
	bool addedSpace = false;
	CString text;
	int colWidth = colRect.Width();

	// determine the font's ave width
	TEXTMETRIC	aTM;
	pDC->GetTextMetrics( &aTM );
	int aveCharWidth = aTM.tmAveCharWidth;

	//loop and find out the length of string before each blank space
	// take into acct the hidden columns
	CString remainText = pList->GetItemText(nItem, nSubitem);

	// check whether the column contains image if text is empty
	if (remainText.IsEmpty()) 
	{
		bool known=false;

		LVITEM item;
		item.mask = LVIF_IMAGE;
		item.iItem = nItem;
		item.iSubItem = nSubitem;
		if (pList->GetItem(&item))
		{
			// TO DO: Direct printing of the image instead of text
			if ( item.iImage>=0 && pImageStringVector!=NULL && pImageStringVector->size()>0 )
			{
				text=(*pImageStringVector)[item.iImage];
				known = true;
			}
		}
		
		if (!known) // this is unknown item
		{
			text = "-";
		}
		
		return text;
	}
	

    // TD14173
    // trim extra spaces at both end of text to avoid wrong wrapping over long text
    remainText.TrimLeft();
    remainText.TrimRight();

	bool anySpace = true;
	int numberOfTry = 0;
	int index = 0;
	
	while (anySpace)
	{
		index = remainText.Find(_T(' '), index+1); // find the first space
		if (index >= 0) // there is space
		{
			text = text + remainText.Left(index+1);
			remainText = remainText.Right(remainText.GetLength()-index-1);
		}
		else // there is no more space
		{
			// check whether the col width is less than text width
			int charsWide = 0;
            SIZE sizeString;
			GetTextExtentExPoint(pDC->GetSafeHdc(), 
                    (LPCTSTR) remainText, remainText.GetLength(),
                    colWidth, &charsWide, NULL, &sizeString );

			// TD 13994 if the charsWide for any reason cannot be fitted on first try
			// ie may be a hidden col
			// So, return the whole string without any change
			if (charsWide==0 && remainText.GetLength()>charsWide && numberOfTry==0)
			{
				if (addedSpace==false) addedSpace = true;
				break;
			}
			else if ( remainText.GetLength() > charsWide )
			{
				// the col width is shorter than the remaining text
				// add a break to the text
				remainText.Insert( charsWide, " " );
				if (addedSpace==false) addedSpace = true;
			}
			else
			{
				text = text + remainText;
				anySpace = false;
			}
		}
		++numberOfTry;
	}

	// add a space in front if it is not a long text for aesthetic reason
	if (addedSpace == false) text = " "+text;
	
	return text;
}

CSize CListCtrlPrint::PrintPageHead(CDC * pDC, CRect & rcPage, const int nPage, const int iMaxPage, bool bCalc)
{
	CFont * oft = pDC->SelectObject(&ftHead);
	int y = rcPage.top;
	int x = rcPage.left;
	CSize szPg(0,0);
	if (csPage != "")
	{
		CString cs;
		cs.Format(csPage, nPage, iMaxPage);
		CRect rc1(x,y,rcPage.right,y);
		pDC->DrawText(cs, rc1, DT_WORDBREAK|DT_CALCRECT|DT_RIGHT);
		rc1.left = rcPage.right - rc1.Width();
		rc1.right = rcPage.right;
		if (!bCalc) pDC->DrawText(cs, rc1, DT_WORDBREAK|DT_RIGHT);
		szPg = rc1.Size();
	}

	if (csPageHead != "")
	{
		CRect rc1(x,y,rcPage.right - szPg.cy,y);
		pDC->DrawText(csPageHead, rc1, DT_WORDBREAK|DT_CALCRECT|DT_LEFT);
		if (!bCalc) pDC->DrawText(csPageHead, rc1, DT_WORDBREAK|DT_LEFT);
		if (rc1.Height() > szPg.cy) szPg.cy = rc1.Height();
	}

	rcPage.top += szPg.cy;
	pDC->SelectObject(oft);
	return szPg;
}

CSize CListCtrlPrint::PrintPageFoot(CDC * pDC, CRect & rcPage, bool bCalc)
{
	CFont * oft = pDC->SelectObject(&ftFoot);
	int y = rcPage.top;
	int x = rcPage.left;
	CSize szPg(0,0);

	if (csPageFooter != "")
	{
		CRect rc1(x,y,rcPage.right - szPg.cy,y);
		pDC->DrawText(csPageFooter, rc1, DT_WORDBREAK|DT_CALCRECT|DT_LEFT);
		if (!bCalc) pDC->DrawText(csPageFooter, rc1, DT_WORDBREAK|DT_LEFT);
		szPg.cy = rc1.Height();
	}

	rcPage.bottom -= szPg.cy;
	pDC->SelectObject(oft);
	return szPg;
}

CSize CListCtrlPrint::PrintListHead(CDC * pDC, CRect & rcPage, bool bCalc)
{
	if (!bCalc)
	{
		CFont * oft = pDC->SelectObject(&ftPrint);
		CRect rcc(rcPage.left, rcPage.top, rcPage.right, rcPage.top + szFtPrint.cy);

		//pDC->MoveTo(rcc.left, rcc.top);
		//pDC->LineTo(rcc.right, rcc.top);
		//pDC->LineTo(rcc.right, rcc.bottom);
		//pDC->LineTo(rcc.left, rcc.bottom);
		//pDC->LineTo(rcc.left, rcc.top);

		COLORREF oldTextColour = pDC->SetTextColor(RGB(255,255,255));

		//pDC->FillSolidRect(rcc, RGB(0,0,0));

		int box = 0;
		CRect rc1;
		int y = rcPage.top;
		for (int t = 0; t < iNumCol; t++)
		{
			rc1.SetRect(rcPage.left+pDx[t], y, rcPage.left+pDx[t+1], y+szFtPrint.cy);
			CString cs;
			cs.Format(" %s",pColTitle[t] ? pColTitle[t] : "");
			//pDC->Rectangle(rc1);

			UINT ftm = DT_LEFT;
			if (pFormat[t]&HDF_CENTER)
			{
				ftm = DT_CENTER;
			}
			else if (pFormat[t]&HDF_RIGHT)
			{
				ftm = DT_RIGHT;
			}
			pDC->FillSolidRect(rc1, RGB(0,0,0));
			pDC->DrawText(cs, rc1, ftm|DT_VCENTER);

			//pDC->MoveTo(rc1.left,rc1.bottom);
			//pDC->LineTo(rc1.left,rc1.top);
			//pDC->LineTo(rc1.right,rc1.top);
		}
		pDC->SelectObject(oft);
		pDC->SetTextColor(oldTextColour);
	}
	rcPage.top += szFtPrint.cy;
	return CSize(0,szFtPrint.cy);
}

CSize CListCtrlPrint::PrintListFoot(CDC * pDC, CRect & rcPage, bool bCalc)
{
	return CSize(0,0);
}

void CListCtrlPrint::threadedPrint(WPARAM wParam, LPARAM lParam)
{
	t_ParamsToReceive* params = (t_ParamsToReceive*)(lParam);

	if (params == NULL)
	{
		return;
	}

    // Copy the parameters.
    csPageHead = params->csPageHead;
    strTitle = params->title;
	csPage = params->csPage;
	pColWidthArray = params->pColWidthArray;
    pList = params->list;
    pColourMap = params->colourMap;
	pImageStringVector = params->imageStringVector;

    // Perform the print.
    this->OnFilePrint();

    // Cleanup objects owned by the thread.
    delete params->colourMap;
    pColourMap = NULL;

	delete params;
    params = NULL;

    // Finally end the thread.
    AfxEndThread( 0, TRUE );
}
//Begin----   TD15057 && TD18775
long CListCtrlPrint::GetTotalPages(CDC * pDC, int vnHightPerRow,CPrintInfo* pInfo)
{
	ASSERT(iItemXPage > 0);
	CRect rcPrint(pInfo->m_rectDraw);	
	SubtractMargin(pDC, rcPrint);
	
	//libo
	CFont * oft = pDC->SelectObject(&ftPrint);
	//libo 

	int y = rcPrint.top;
	int x = rcPrint.left;
	CString cs;
	CRect rc1;
	//Begin---- TD14936
	int iTotalRowsPerPage = iItemXPage;
	if (pInfo->m_pPD->m_pd.nMinPage > pInfo->m_pPD->m_pd.nMaxPage )
		pInfo->SetMaxPage(pInfo->m_pPD->m_pd.nMinPage);
	if (pInfo->m_pPD->m_pd.nToPage < pInfo->m_pPD->m_pd.nFromPage )
		pInfo->m_pPD->m_pd.nToPage = pInfo->m_pPD->m_pd.nFromPage;
	int iTotalPages = 0;
	long lSelect = pInfo->m_pPD->PrintSelection();
	int nFromPage = pInfo->GetFromPage();
	m_lStartRowInPage = 0;
	m_lStartWrapRow = 0;
	long iPreparePrintRows = 0;
	if (lSelect > 0)
	{
		int iRow = 0;
		int nMaxHeight=0;
		POSITION pos = pList->GetFirstSelectedItemPosition();
		while(pos)
		{
			iRow = pList->GetNextSelectedItem(pos);
			int textCol = 0;
			for (unsigned int t = 0; t <m_colToHide.size() ; t++)
			{
				if (m_colToHide[t] == true)
				{
					continue;
				}
				
				if ( textCol<iNumCol ) // make sure the col does not exceed the array
				{
					rc1.SetRect(rcPrint.left+pDx[textCol] + 10, y, rcPrint.left+pDx[textCol+1], y+szFtPrint.cy); //TD14173
					UINT ftm = DT_LEFT;                         
					cs = GetTextSizedToColumn(pDC, iRow, t, rc1);			
					CRect tmpRect = rc1;
					pDC->DrawText(cs, tmpRect, ftm|DT_VCENTER|DT_WORDBREAK|DT_CALCRECT); 
					nMaxHeight = max(nMaxHeight, tmpRect.Height());
					++textCol; // next print col
				}
			}
			
			//when the alarm will be print in different pages ,determine the rows will be printed in this page and the 
			//start row printed in next page
			int iTemp;
			iTemp = abs((nMaxHeight-vnHightPerRow)/34);
			if (iTemp > abs((nMaxHeight-vnHightPerRow)/34))
			{
				iTemp +=1;
			}
			iPreparePrintRows += iTemp ;
		}
	}
	else
	{
		int iRow = 0;
		int iTempPages = 0;
			int nMaxHeight = 0;
		while(iRow < m_lTotalRows)
		{
			int textCol = 0;
			int nMaxHeight = szFtPrint.cy;
			for (unsigned int t = 0; t <m_colToHide.size() ; t++)
			{
				if (m_colToHide[t] == true)
				{
					continue;
				}
				
				if ( textCol<iNumCol ) // make sure the col does not exceed the array
				{
					int a1 = rcPrint.left+pDx[textCol];
					int a2 = rcPrint.left+pDx[textCol+1];
					int b1 = y;
					int b2 = y+szFtPrint.cy;

					rc1.SetRect(rcPrint.left+pDx[textCol], y, rcPrint.left+pDx[textCol+1], y+szFtPrint.cy);
					UINT ftm = DT_LEFT;     
					cs = GetTextSizedToColumn(pDC, iRow, t, rc1);	
					cs.TrimLeft();
					CRect tmpRect = rc1;
					int c = tmpRect.Height();
					int temp = pDC->DrawText(cs, tmpRect, ftm|DT_VCENTER|DT_WORDBREAK|DT_CALCRECT); 
					int a = tmpRect.Height();
					int b = rc1.Height();
					nMaxHeight = max(nMaxHeight, tmpRect.Height());
					++textCol; // next print col
				}
			}
			//when the alarm will be print in different pages ,determine the rows will be printed in this page and the 
			//start row printed in next page
			int iTemp;
			iTemp = abs((nMaxHeight-vnHightPerRow)/34);
			if (iTemp > abs((nMaxHeight-vnHightPerRow)/34))
			{
				iTemp +=1;
			}
			iPreparePrintRows += iTemp + 1;
			//iPreparePrintRows += iTemp ;
			iTempPages = abs(iPreparePrintRows/iItemXPage);
			//Begin---- TD14929
			if (nFromPage - 1 == 0)
			{
				m_lStartRowInPage = 0;
				m_lStartWrapRow = 0;
			}
			else
			{
				if (iTempPages == nFromPage - 1)
				{
					int iMod = iPreparePrintRows%iItemXPage;				
					pInfo->m_nCurPage = nFromPage;
					if (iMod == 0)
					{
						m_lStartWrapRow = 0;
						m_lStartRowInPage = iRow + 1;
					}
					else
					{
						m_lStartWrapRow = iTemp + 1 - iMod;
						m_lStartRowInPage = iRow;
					}				
				}
			}
			//End---- TD14929
			iRow++;
		}
	}
	//libo++
	int  nNumber = int(iPreparePrintRows/iItemXPage);
	if(nNumber ==0)
	{
		iTotalPages = 1;
	}
	else
	{
		iTotalPages = iPreparePrintRows%iItemXPage==0 ? nNumber : nNumber+1;
	}
	//++libo
	/*iTotalPages = abs(iPreparePrintRows/iItemXPage);
	if(iTotalPages < iPreparePrintRows/iItemXPage)
	{
		iTotalPages += 1;
	}*/
	if (lSelect > 0)
	{
		pInfo->m_pPD->m_pd.nFromPage = 1;
		pInfo->m_pPD->m_pd.nToPage = iTotalPages;
	}
	//End---- TD14936
	return iTotalPages;
}
//End---- TD15057 && TD18775

//Begin---- TD14936
void CListCtrlPrint::PrintSelection(CDC* pDC, CPrintInfo* pInfo)
{
	CRect rcPrint(pInfo->m_rectDraw);

	SubtractMargin(pDC, rcPrint);
	int obkm = pDC->SetBkMode(TRANSPARENT);

	PrintPageHead(pDC, rcPrint, pInfo->m_nCurPage, (int) pInfo->GetMaxPage(), false);
	PrintListHead(pDC, rcPrint, false);

//	int iFirst = (pInfo->m_nCurPage - 1) * iItemXPage;
//	int iLast = (pInfo->m_nCurPage) * iItemXPage;
	int y = rcPrint.top;
	int x = rcPrint.left;

    CFont * oft = pDC->SelectObject(&ftPrint);
	CRect rc1;
	CString cs;
	int iMax = iTotItem;

	// Determine whether the device is a BW or Colour printer
	// TD 14559 remove BW printing
	bool isColourPrinter = true;
	//int colours = pDC->GetDeviceCaps(PLANES);
	//if (colours > 1) isColourPrinter = true; // there is more than 1 colour plane ie not black only. So, it is a colour Printer

    // When the list control is printed out, we want word wrapping done on the columns where 
    // the text is too long for the column width. If wrapping is done, then the height of the
    // column is extended. So the first thing we do is go through each column (for each row) 
    // and determine the maximum height that is needed so it can be applied for the whole row.
    // Then we draw the row.

	//Begin---- TD15057 && TD18775
	//Ensure to be printed correctly

	int iTotalRowsPerPage = iItemXPage; //Total rows can be printed in one page
	int iPrintedRows(0);				//The count of rows have been printed in a page 
	int iPreparePrintRows(0);			//The count of rows that a alarm will be printed
	int iPrintRowsOfAlarm(0);			//The count of rows that a alarm will be printed in current page
   // for (int i = iFirst; i < iLast; i++)
	while(iPrintedRows < iTotalRowsPerPage)
	{
	//	bool bIs = i < iMax;
        
        int nMaxHeight = szFtPrint.cy;
        int box = 0;

        // Determine the maximum height of the row
		int textCol = 0; //count for iNumCol
		for (unsigned int t = 0; t <m_colToHide.size() ; t++)
		{
			if (m_colToHide[t] == true)
			{
				continue;
			}
			
			if ( textCol<iNumCol ) // make sure the col does not exceed the array
			{
				rc1.SetRect(rcPrint.left+pDx[textCol], y, rcPrint.left+pDx[textCol+1], y+szFtPrint.cy);

		//		if (bIs)
		//		{
				UINT ftm = DT_LEFT;                         
				cs = GetTextSizedToColumn(pDC, m_lStartRowInPage, t, rc1);
				//if (pFormat[t]&HDF_CENTER) ftm = DT_CENTER;
				//else if (pFormat[t]&HDF_RIGHT) ftm = DT_RIGHT;
				
				if (pColourMap!=NULL)
				{
					// Apply text colours.
					COLORREF cellColour = pColourMap->getDefaultColour();
					if (isColourPrinter) cellColour = pColourMap->getCellColour( t, m_lStartRowInPage );
					pDC->SetTextColor( cellColour );
				}
				
				CRect tmpRect = rc1;
				pDC->DrawText(cs, &tmpRect, ftm|DT_VCENTER|DT_WORDBREAK|DT_CALCRECT); 
			//	pDC->DrawText(cs, &tmpRect, ftm|DT_WORDBREAK|DT_CALCRECT); 
				nMaxHeight = max(nMaxHeight, tmpRect.Height());
					
		//		}

				++textCol; // next print col
			}
		}
		
		//when the alarm will be print in different pages ,determine the rows will be printed in this page and the 
		//start row printed in next page
		int iTemp;
		iTemp = abs((nMaxHeight-szFtPrint.cy)/34);
		iPreparePrintRows = iTemp + 1;
		
		if (iPrintedRows + iPreparePrintRows > iTotalRowsPerPage)
		{
			iPrintRowsOfAlarm = iTotalRowsPerPage - iPrintedRows;
			m_lStartWrapRow = iPrintRowsOfAlarm;
		}
		iPrintedRows += iPreparePrintRows;
		//if there has a alarm is printed in different pages,determine the height

        // Draw the row
        CRect rc2;
		int printCol = 0; //count for iNumCol

		CEdit* pEdit = NULL;
		if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
		{
			pEdit = new CEdit;
			pEdit->Create(ES_MULTILINE|WS_CHILD|ES_LEFT|ES_AUTOVSCROLL|ES_WANTRETURN,rc2,pList,1);
			CFont* cfTemp;
	//		LOGFONT lf;
	//		oft->GetLogFont(&lf);
	//		cfTemp.CreateFontIndirect(&lf);
			cfTemp = pDC->GetCurrentFont();
			pEdit->SetFont(cfTemp);
		}
		int nPrintHeight;
		if (iPrintRowsOfAlarm > 0)
		{
			nPrintHeight = szFtPrint.cy *iPrintRowsOfAlarm;
		}
		if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow > 0)
		{
			nPrintHeight = (iPreparePrintRows - m_lStartWrapRow)*szFtPrint.cy;	
		}
		if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow == 0)
		{
			nMaxHeight = iPreparePrintRows * szFtPrint.cy;
		}

		printCol = 0;
        for (unsigned int u = 0; u < m_colToHide.size(); u++)
		{
			if (m_colToHide[u] == true)
			{
				continue;
			}

			if ( printCol<iNumCol ) // make sure the col does not exceed the array
            {
				rc2.SetRect(rcPrint.left+pDx[printCol], y, rcPrint.left+pDx[printCol+1], y+nMaxHeight);
		//		if ( bIs )
		//		{
				
				
				cs = GetTextSizedToColumn(pDC, m_lStartRowInPage, u, rc2); //" " + pList->GetItemText(i, u);
				//TD14929
				cs.TrimLeft();
				//if there has a alarm is printed in different pages,determine the context
				if (iPrintRowsOfAlarm > 0)
				{
					cs.TrimLeft();
					cs.TrimRight();
					pEdit->SetRect(&rc2);
					pEdit->SetWindowText(cs);
					CString strTemp;
					for (int i=0;i<iPrintRowsOfAlarm;i++ )
					{
						char cTemp[501];
						memset(cTemp,0,sizeof(cTemp));
						int iret = pEdit->GetLine(i,cTemp,500);
						memset(cTemp+iret,0,500-iret);
						strTemp = cTemp;
						strTemp.TrimLeft();
						strTemp.TrimRight();						
						cs = "";
						//TD14929
						cs  = cs + strTemp;
					}
				}
				if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow > 0)
				{				
					cs.TrimLeft();
					cs.TrimRight();
					pEdit->SetRect(&rc2);
					pEdit->SetWindowText(cs);
					CString strTemp;
					for (int i=m_lStartWrapRow;i<iPreparePrintRows;i++ )
					{
						char cTemp[501];
						memset(cTemp,0,sizeof(cTemp));
						int iret = pEdit->GetLine(i,cTemp,500);
						memset(cTemp+iret,0,500-iret);
						strTemp = cTemp;
						strTemp.TrimLeft();
						strTemp.TrimRight();
						cs = "";
						//TD14929
						cs  = cs + strTemp;
					}
				}
				if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
				{				
					rc2.SetRect(rcPrint.left+pDx[printCol], y, rcPrint.left+pDx[printCol+1], y+nPrintHeight + 1);
				}
				else
				{
					rc2.SetRect(rcPrint.left+pDx[printCol], y, rcPrint.left+pDx[printCol+1], y+szFtPrint.cy*iPreparePrintRows + 1);
				}
				
				// If no colour map is specified the list is printed like it always has been.
				if ( NULL != pColourMap )
				{	// Determine the brush to use when filling the cell and the text colour
					// and fill the cell with the required colour.
					CBrush* cellBkBrush = pColourMap->getDefaultBkBrush();
					if (isColourPrinter) cellBkBrush = pColourMap->getCellBkBrush( u, m_lStartRowInPage );
					pDC->FillRect( rc2, cellBkBrush );
				}

				UINT ftm = DT_LEFT;
				//if (pFormat[t]&HDF_CENTER) ftm = DT_CENTER;
				//else if (pFormat[t]&HDF_RIGHT) ftm = DT_RIGHT;
				pDC->DrawText(cs, rc2, ftm|DT_VCENTER|DT_WORDBREAK);
		//		}

				if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
				{					
					pDC->MoveTo(rc2.left,y+nPrintHeight);
				}
				else
				{
					pDC->MoveTo(rc2.left,y+nMaxHeight);
				}
				pDC->LineTo(rc2.left,rc2.top);
				pDC->LineTo(rc2.right,rc2.top);

				++printCol; // next print col
			}
        }
		if (pEdit != NULL)
		{
			delete pEdit;
			pEdit = NULL;
		}
		if (iPrintRowsOfAlarm > 0 || m_lStartWrapRow > 0)
		{					
			pDC->LineTo(rc2.right,y+nPrintHeight);
			y += nPrintHeight;	
		}
		else
		{
			pDC->LineTo(rc2.right,y+nMaxHeight);
			y += nMaxHeight;
		}
		if (iPrintRowsOfAlarm == 0 )
		{
			if (m_pos)
			{
				m_lStartRowInPage  = pList->GetNextSelectedItem(m_pos);
			}
			else
			{
				m_PrintSelection = false;
				break;
			}
		}
		if (iPrintRowsOfAlarm == 0 && m_lStartWrapRow > 0)
		{					
			m_lStartWrapRow = 0;
		}
	}
	pDC->MoveTo(rcPrint.left,y);
	pDC->LineTo(rcPrint.right,y);
	pDC->SelectObject(oft);
	
	rcPrint = pInfo->m_rectDraw;
	SubtractMargin(pDC, rcPrint);
	rcPrint.top = rcPrint.bottom - szFoot.cy;
	
	PrintListFoot(pDC, rcPrint, false);
	PrintPageFoot(pDC, rcPrint, false);
	
	pDC->SetBkMode(obkm);
}
//End---- TD14936
