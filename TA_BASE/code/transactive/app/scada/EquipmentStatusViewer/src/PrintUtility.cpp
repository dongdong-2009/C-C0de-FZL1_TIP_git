/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/PrintUtility.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which prints the equipment status report to a selected printer. All printers supported
 * and all paper sizes/orientations supported
 *
 */

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "PrintUtility.h"
#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

#define SPACE_HEIGHT	1

namespace TA_Base_App
{
    // TES #768 - define pixel offset - 15 pixels in either side of column
    const static DWORD TEXT_OFFSET_FROM_COLUMN = 12;

	CPrintUtility::CPrintUtility()
	{
		m_pListCtrl = NULL;		// list control
		m_hPrinterDC = NULL;	// printer device context
        m_hDevMode      = NULL; 
        m_hDevNames     = NULL; 

		m_iPageWidth = 0;		// printer selected page width
		m_iPageHeight = 0;		// printer selected page height		
	}

	CPrintUtility::~CPrintUtility()
	{
        releaseHandles();
	}

    void CPrintUtility::releaseHandles()
    {
	    if (m_hPrinterDC)
        {
			DeleteDC(m_hPrinterDC);
            m_hPrinterDC = 0;
        }
        
        if (m_hDevMode)
        {
            m_hDevMode = ::GlobalFree(m_hDevMode);
            m_hDevMode = 0;
        }

        if (m_hDevNames)
        {
            m_hDevNames = ::GlobalFree(m_hDevNames);
            m_hDevNames = 0;
        }
    }

	
	void CPrintUtility::SetListCtrl(CListCtrl *pList)
	{
		m_pListCtrl = pList;
	}

	bool CPrintUtility::Print(CWnd *aParentWnd, CString &aLocationName, CString &aReportName, CString &aComment)
	{				
		// delete various handles if already exists
        releaseHandles();

    	// display the print dialog
		CPrintDialog dlg(FALSE, PD_ALLPAGES | PD_DISABLEPRINTTOFILE | PD_NOSELECTION | PD_NOPAGENUMS | PD_RETURNDC, aParentWnd); 
		
        DWORD prevFlags = dlg.m_pd.Flags;

        // TES #767: Default to landscape orientation
        dlg.GetDefaults();
        // Restore flags to what we had set before (important to retain this data)
        dlg.m_pd.Flags = prevFlags;
        
        m_hDevMode = dlg.m_pd.hDevMode;
        m_hDevNames = dlg.m_pd.hDevNames;

        LPDEVMODE lp = (LPDEVMODE) ::GlobalLock(m_hDevMode);
        TA_ASSERT(lp, "Handle to device mode is NULL");
        lp->dmOrientation = DMORIENT_LANDSCAPE;
        ::GlobalUnlock(m_hDevMode);
        // End TES #767

		if (dlg.DoModal() == IDOK)
		{				
			// get Printer DC from dialog
			m_hPrinterDC = dlg.GetPrinterDC();
		
			if (m_hPrinterDC == NULL)
				return false;

		} else
			return false;

		// check if list is provided
		if (m_pListCtrl == NULL)
			return false;		

		// convert screen font sizes to printer font sizes
		int iPrinterHeaderFont = GetPrinterHeightFromScreenHeight(10 /* font size */);		
		int iPrinterListFont = GetPrinterHeightFromScreenHeight(10 /* font size */);

		// create Header and List fonts
		HFONT hHeaderFont = CreateFont(iPrinterHeaderFont, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
		HFONT hListFont = CreateFont(iPrinterListFont, 0, 0, 0, FW_NORMAL, FALSE, FALSE, FALSE, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH, "Arial");
  
		if ((hHeaderFont == NULL) || (hListFont == NULL))
		{
			if (hHeaderFont) 
				DeleteObject(hHeaderFont);
			if (hListFont) 
				DeleteObject(hListFont);			
	
			return false;
		}
  
		// get logical size of an individual character in our list's font in printer device context
		HFONT hOldFont = (HFONT)SelectObject(m_hPrinterDC, hListFont);
		char  szCharX[] = "X";
		char  szCharx[] = "x";		
		SIZE  stSizeX, stSizex;
  
		GetTextExtentPoint32(m_hPrinterDC, szCharX, strlen(szCharX), &stSizeX);		
		GetTextExtentPoint32(m_hPrinterDC, szCharx, strlen(szCharx), &stSizex);		
		SelectObject(m_hPrinterDC, hOldFont);
		
		// Get page dimension
		m_iPageWidth  = GetDeviceCaps(m_hPrinterDC, HORZRES);
		m_iPageHeight = GetDeviceCaps(m_hPrinterDC, VERTRES);
	
		// column alignments
		int   piColFormatTable[ESL_COLUMN_MAX] = 
                                    { HDF_LEFT, HDF_LEFT, HDF_LEFT, HDF_LEFT, 
									  HDF_LEFT, HDF_LEFT, HDF_LEFT, HDF_LEFT, HDF_LEFT };
		// column titles
		char pszColTitleTable[ESL_COLUMN_MAX][HEADER_STRING_LENGTH] =
                                    { "Asset", "Equipment Description", "Point Description",
									  "Type", "Value", "Quality",
                                      "Status", "Operator", "Timestamp" };
		
		
		// column width
		int *piColWidthTable  = new int[ESL_COLUMN_MAX];	
		int runningTotalWidth = 0;
		int freeSpace = 0;
        int shrinkableTotalWidth = 0;   //  total width of shrinkable columns Desc, DpName, DpType
        int unshrinkableTotalWidth = 0; //  total width of all the other columns

		// get max text in each column in logical units
		for (int col = ASSET_COLUMN; col < ESL_COLUMN_MAX; col++)
		{
			// get max no of characters in rows of this column
			int iMax = GetMaxStringLen(col);
			if (iMax == 0)
				iMax = strlen(pszColTitleTable[col]);
			else
			{
				if (iMax < (int)strlen(pszColTitleTable[col]))
					iMax = strlen(pszColTitleTable[col]);
			}
						
            // Asset column is all uppercase so give it a little more width
			if (col == ASSET_COLUMN)
				piColWidthTable[col] = stSizeX.cx * iMax;	// <- max width required		
			else 
				piColWidthTable[col] = stSizex.cx * iMax;	// <- max width required		

            // TES#768 - adding some padding to the width (so text not so close to column line)
            piColWidthTable[col] += (2 * TEXT_OFFSET_FROM_COLUMN);

			runningTotalWidth += piColWidthTable[col];

            if ( (col >= DESCRIPTION_COLUMN) && (col <= TYPE_COLUMN) )
            {
                //  Minimum width for these columns are their title width
                shrinkableTotalWidth+= (stSizex.cx * strlen(pszColTitleTable[col])) + (2 * TEXT_OFFSET_FROM_COLUMN);
            }
            else
            {
                // These columns can't shrink so their minimum width is their contents or title
                unshrinkableTotalWidth+= piColWidthTable[col];
            }
		}

		freeSpace = m_iPageWidth - runningTotalWidth;

        // Divide the free space evenly across all the columns
		if (freeSpace > 0)
		{
    		int freeSpacePerColumn = (int)( freeSpace / ((float)ESL_COLUMN_MAX));

			for (col = ESL_COLUMN_MIN; col < ESL_COLUMN_MAX; col++)
			{
				piColWidthTable[col] += freeSpacePerColumn;
			}
		}
        else
		{
            // Some columns will have to be truncated: description, datapoint name and/or datapoint type

            // Recalculate freespace as the width leftover after removing the fixed
            // columns' widths and the minimum column widths of the shrinkable columns.
    		freeSpace = m_iPageWidth - unshrinkableTotalWidth - shrinkableTotalWidth;
    		int availableWidthPerColumn = (int)( freeSpace / 3.0f);

            // See if there is a shrinkable column that doesn't need 1/3 of the remaining space
            freeSpace = m_iPageWidth - unshrinkableTotalWidth;
            int numColumnsTooWide = 3;
            for (col = DESCRIPTION_COLUMN; col <= TYPE_COLUMN; col++)
            {
                if ( piColWidthTable[col] < availableWidthPerColumn)
                {
                    freeSpace -= piColWidthTable[col];
                    numColumnsTooWide--;
                }
                else
                    freeSpace -= availableWidthPerColumn;
            }

            // If there is any free space left then one or more of the shrinkable columns didn't need
            // all the space ... so give it to the remaining column(s)

            if (freeSpace > 2.0)    //  allow for rounding error
            {
                for (col = DESCRIPTION_COLUMN; col <= TYPE_COLUMN; col++)
                {
                    if ( piColWidthTable[col] > availableWidthPerColumn)
                        piColWidthTable[col] = availableWidthPerColumn + (int)(freeSpace / numColumnsTooWide);
                }
            }
        }

		// Start printing
		DOCINFO stDI;

		stDI.cbSize       = sizeof(stDI);
		stDI.fwType       = 0;
		stDI.lpszDatatype = 0;
		stDI.lpszOutput   = 0;
		stDI.lpszDocName  = "Equipment Status Report"; 
				
		StartDoc(m_hPrinterDC, &stDI);		

		BOOL bNewPage     = TRUE;
		int  iPageCounter = 0;
		int  iOffsetY     = 0;

		int tempC = m_pListCtrl->GetItemCount();
        
		for (int line =0; line < m_pListCtrl->GetItemCount(); line++)
		{
			if (bNewPage)
			{
				StartPage(m_hPrinterDC);				

				bNewPage = FALSE;
				iOffsetY = 0;
				iPageCounter++;				
	  	  
				iOffsetY = PrintHeader(iOffsetY, hHeaderFont, aLocationName, aReportName, aComment, iPageCounter);
				iOffsetY = PrintListCtrlHeader(iOffsetY, hListFont, piColWidthTable, piColFormatTable, pszColTitleTable);
			}

			iOffsetY = PrintListCtrlLine(iOffsetY, hListFont, line, piColWidthTable, piColFormatTable, &bNewPage);

			if (iOffsetY == -1 && bNewPage)
			{
				EndPage(m_hPrinterDC);
				line--;
			}
		}

		if (bNewPage == FALSE) 
			EndPage(m_hPrinterDC);

		EndDoc(m_hPrinterDC);

		if (hHeaderFont) 
			DeleteObject(hHeaderFont);

		if (hListFont) 
			DeleteObject(hListFont);
  
		if (piColWidthTable) 
			delete [] piColWidthTable;
		
		return true;
	}


	/*	Converts the screen font size to printer font size
	*/
	int CPrintUtility::GetPrinterHeightFromScreenHeight(int aFontScreenHeight)
	{		 
		if (m_hPrinterDC == NULL) 
			return -1;

		HDC hScreenDC = GetDC(NULL);

		if (hScreenDC == NULL) 
			return -1;

		int iPrinterHeight = (aFontScreenHeight * GetDeviceCaps(m_hPrinterDC,LOGPIXELSY)) 
													/ GetDeviceCaps(hScreenDC,LOGPIXELSY);

		ReleaseDC(NULL, hScreenDC);

		return iPrinterHeight;
	}

	int CPrintUtility::GetMaxStringLen(int aCol)
	{
		int iMax = 0;

		int nItems = m_pListCtrl->GetItemCount();

		for (int item = 0; item < nItems; item++)
		{
			CString itemText = m_pListCtrl->GetItemText(item, aCol);
			int itemTextLength = itemText.GetLength();

			if (itemTextLength > iMax)
				iMax = itemTextLength;
		}

		return iMax;
	}

	int CPrintUtility::PrintHeader(int aOffsetY, HFONT aHeaderFont, 
									CString &aLocationName, 
									CString &aReportName, 
									CString &aComment, int aPageCounter)
	{
		LOGFONT stNormalLF, stDoubleLF;		// attibutes of a font
		HFONT   hDoubleFont;				

		// get information about our Header font
		GetObject(aHeaderFont, sizeof(stNormalLF), &stNormalLF);
		// copy information to stDoubleLF
		memcpy(&stDoubleLF, &stNormalLF, sizeof(LOGFONT));

		// double height
		stDoubleLF.lfHeight = stDoubleLF.lfHeight * 2;	
		// create the doubled font
		hDoubleFont = CreateFontIndirect(&stDoubleLF);

		RECT  stRect;
		char  szBuf[255];		

		// remember the original mode
		int iOldMode = SetBkMode(m_hPrinterDC, TRANSPARENT);


		// calculate the Header Rectangle box
		RECT stFrameRect;
		stFrameRect.left   = 0;
		stFrameRect.top    = aOffsetY;
		stFrameRect.right  = m_iPageWidth - 1;
		stFrameRect.bottom = aOffsetY + (2 * stDoubleLF.lfHeight) + (3 * stNormalLF.lfHeight) + (8 * SPACE_HEIGHT);

		// light gray out the Header Rectangle Box
		HBRUSH hBrush    = CreateSolidBrush(RGB(255,255,255));
		HBRUSH hOldBrush = (HBRUSH) SelectObject(m_hPrinterDC, hBrush);
		FillRect(m_hPrinterDC, &stFrameRect, hBrush);
		SelectObject(m_hPrinterDC, hOldBrush);
		
		// print the bounding frame box
		FrameRect(m_hPrinterDC, &stFrameRect, (HBRUSH)GetStockObject(BLACK_BRUSH));
		

		// calculate the APPLICATION TITLE rectangle box
		stRect.left   = 0 ;
		stRect.top    = aOffsetY;
		stRect.right  = m_iPageWidth;
		stRect.bottom = aOffsetY + SPACE_HEIGHT + stDoubleLF.lfHeight;

		// print the APPLICATION TITLE text
		SelectObject(m_hPrinterDC, hDoubleFont);
		sprintf(szBuf, "EQUIPMENT STATUS REPORT");
		DrawText(m_hPrinterDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_CENTER | DT_TOP | DT_NOPREFIX);
		aOffsetY += (stDoubleLF.lfHeight + SPACE_HEIGHT);


		// get current time
		time_t current_time;
		time(&current_time);

		CTime now(current_time);
		CString s = now.Format("%d %b %Y - %I:%M%p");				

		// calculate the DATE TIME rectangle box
		stRect.left   = 0;
		stRect.top    = aOffsetY;
		stRect.right  = m_iPageWidth;
		stRect.bottom = aOffsetY + SPACE_HEIGHT + stNormalLF.lfHeight;

		// print the DATE TIME text
		SelectObject(m_hPrinterDC, aHeaderFont);
		DrawText(m_hPrinterDC, s, s.GetLength(), &stRect, DT_SINGLELINE | DT_CENTER | DT_TOP | DT_NOPREFIX);
		aOffsetY += (stNormalLF.lfHeight + SPACE_HEIGHT);

		
		// calculate the LOCATION and REPORT NAME rectangle box
		stRect.left   = 5;
		stRect.top    = aOffsetY;
		stRect.right  = m_iPageWidth;
		stRect.bottom = aOffsetY + SPACE_HEIGHT + stDoubleLF.lfHeight;

		// print the LOCATION and REPORT NAME text
		SelectObject(m_hPrinterDC, hDoubleFont);
		sprintf(szBuf, "%s at %s", aReportName, aLocationName);
		DrawText(m_hPrinterDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);
		aOffsetY += (stDoubleLF.lfHeight + SPACE_HEIGHT);
		

		// calculate the Page Counter rectangle box
		stRect.left   = 5;
		stRect.top    = aOffsetY;
		stRect.right  = m_iPageWidth - 5;
		stRect.bottom = aOffsetY + stNormalLF.lfHeight;

		// print the Page Counter		
		SelectObject(m_hPrinterDC, aHeaderFont);  
		sprintf(szBuf, "Page %d", aPageCounter);    
		DrawText(m_hPrinterDC, szBuf, strlen(szBuf), &stRect, DT_SINGLELINE | DT_CENTER | DT_VCENTER | DT_NOPREFIX);		
		aOffsetY += stNormalLF.lfHeight;


		// draw a horizontal line in light gray
		/*HPEN hPen    = CreatePen(PS_SOLID, 0, RGB(127, 127, 127));
		HPEN hOldPen = (HPEN)SelectObject(m_hPrinterDC, hPen);
		
		MoveToEx(m_hPrinterDC, 0, aOffsetY, NULL);
		LineTo(m_hPrinterDC, m_iPageWidth, aOffsetY);
		aOffsetY += SPACE_HEIGHT;
		SelectObject(m_hPrinterDC, hOldPen);
*/
		// calculate the COMMENT rectangle box
		stRect.left   = 5;
		stRect.top    = aOffsetY;
		stRect.right  = m_iPageWidth;
		stRect.bottom = aOffsetY + SPACE_HEIGHT + stNormalLF.lfHeight;

		// print the COMMENT text
		SelectObject(m_hPrinterDC, aHeaderFont);
		CString comment = "COMMENT : ";
		comment += aComment;
		DrawText(m_hPrinterDC, comment, comment.GetLength(), &stRect, DT_SINGLELINE | DT_LEFT | DT_VCENTER | DT_NOPREFIX);
		aOffsetY += (stNormalLF.lfHeight + SPACE_HEIGHT);

		DeleteObject(hDoubleFont);
		
		// restore default values
		SetBkMode(m_hPrinterDC, iOldMode);
		SelectObject(m_hPrinterDC, aHeaderFont);

		return (aOffsetY + 80 /* extra space between header and listctrl header */);
	}


	int CPrintUtility::PrintListCtrlHeader(int iOffsetY, 
											HFONT hListFont, 											
											int *piColWidthTable, 
											int *piColFormatTable, 
											char pszColTitleTable[ESL_COLUMN_MAX][HEADER_STRING_LENGTH])
	{
		LOGFONT     stLF;
		RECT        stRect;
		int         iOffsetX    = 0;
		HFONT       hOldFont    = (HFONT)SelectObject(m_hPrinterDC, hListFont);
		
		
		// get information about the list font
		GetObject(hListFont, sizeof(stLF), &stLF);		
		
		// print all column titles
		for (int col = ESL_COLUMN_MIN; col < ESL_COLUMN_MAX; col++)
		{
			if ((iOffsetX + piColWidthTable[col]) > m_iPageWidth) 
				break;
			
			if (iOffsetX > m_iPageWidth) 
				break;
			
			if (piColWidthTable[col] == 0) 
				continue;			
			
			stRect.left   = iOffsetX;
			stRect.top    = iOffsetY;
			stRect.right  = iOffsetX + piColWidthTable[col] - 1;
			stRect.bottom = iOffsetY + stLF.lfHeight;
			
            // Offset text slightly in from column
            RECT textRect = stRect;
            // The width of column already includes length of 2 * TEXT_OFFSET_FROM_COLUMN
            // - inset set rect to lie within columns, by at least TEXT_OFFSET_FROM_COLUMN pixels
            textRect.left += TEXT_OFFSET_FROM_COLUMN;
            textRect.right -= TEXT_OFFSET_FROM_COLUMN;

			DrawText(m_hPrinterDC, pszColTitleTable[col], strlen(pszColTitleTable[col]), &textRect, DT_SINGLELINE | DT_VCENTER | piColFormatTable[col] | DT_NOPREFIX);
			
			iOffsetX += piColWidthTable[col];
			
			// print column separator 
			if (col > 0)
			{
				MoveToEx(m_hPrinterDC, stRect.left, stRect.top, NULL);
				LineTo(m_hPrinterDC, stRect.left, stRect.bottom + 20 /* height seperator */);
				//MoveToEx(m_hPrinterDC, stRect.left-2, stRect.top, NULL);
				//LineTo(m_hPrinterDC, stRect.left-2, stRect.bottom + 20 /* height seperator */);
			}
		}
		
		iOffsetY += stLF.lfHeight;
		
		// Print separator line
		HPEN hPen    = CreatePen(PS_SOLID, 3, RGB(0,0,0));
		HPEN hOldPen = (HPEN)SelectObject(m_hPrinterDC, hPen);
		
		MoveToEx(m_hPrinterDC, 0, iOffsetY, NULL);
		LineTo(m_hPrinterDC, m_iPageWidth, iOffsetY);
		
		// Restore default values
		SelectObject(m_hPrinterDC, hOldPen);
		
		SelectObject(m_hPrinterDC, hOldFont);
		
		DeleteObject(hPen);
  
		return (iOffsetY + 20 /* height seperator */);
	}

	
	int CPrintUtility::PrintListCtrlLine(int iOffsetY, HFONT hListFont, int iLine, int *piColWidthTable, int *piColFormatTable, BOOL *pbNewPage)
	{
		*pbNewPage = FALSE;
  
		if (m_pListCtrl == NULL) 
			return -1;		

		int iLineCount  = m_pListCtrl->GetItemCount();

		LOGFONT stLF;
  
		GetObject(hListFont, sizeof(stLF), &stLF);

		if ((iOffsetY + stLF.lfHeight) > m_iPageHeight)
		{
			*pbNewPage = TRUE;

			return -1;
		}

		int   iOffsetX = 0;
		HFONT hOldFont = (HFONT)SelectObject(m_hPrinterDC, hListFont);
  
		for (int col = ESL_COLUMN_MIN; col < ESL_COLUMN_MAX; col++)
		{
			if ((iOffsetX + piColWidthTable[col]) > m_iPageWidth) 
				break;
    
			if (iOffsetX > m_iPageWidth) 
				break;

			if (piColWidthTable[col] == 0) 
				continue;

			LVITEM      stLVI;			
			RECT        stRect;		
			char        szBuf[255];

			memset(szBuf, 0, sizeof(szBuf));

			stLVI.iItem      = iLine;
			stLVI.iSubItem   = col;
			stLVI.mask       = LVIF_STATE | LVIF_TEXT;
			stLVI.pszText    = szBuf;
			stLVI.cchTextMax = sizeof(szBuf);
      
			m_pListCtrl->GetItem(&stLVI);      

            stRect.left   = iOffsetX;
			stRect.top    = iOffsetY;
            stRect.right  = iOffsetX + piColWidthTable[col];
			stRect.bottom = iOffsetY + stLF.lfHeight;

            // Offset text slightly in from column
            RECT textRect = stRect;
            // The width of column already includes length of 2 * TEXT_OFFSET_FROM_COLUMN
            // - inset set rect to lie within columns, by at least TEXT_OFFSET_FROM_COLUMN pixels
            textRect.left += TEXT_OFFSET_FROM_COLUMN;
            textRect.right -= TEXT_OFFSET_FROM_COLUMN;
			
			DrawText(m_hPrinterDC, szBuf, strlen(szBuf), &textRect, DT_SINGLELINE | DT_VCENTER | piColFormatTable[col] | DT_NOPREFIX);

			iOffsetX += piColWidthTable[col];

			// print column separator 
			if (col > 0)
			{
	            MoveToEx(m_hPrinterDC, stRect.left, stRect.top, NULL);
				LineTo(m_hPrinterDC, stRect.left, stRect.bottom + 10 /* separator */);		
			//	MoveToEx(m_hPrinterDC, stRect.left-2, stRect.top, NULL);
			//	LineTo(m_hPrinterDC, stRect.left-2, stRect.bottom + 10 /* separator */);
			}
		}

		iOffsetY += stLF.lfHeight;

		// print line separator 
		HPEN hPen, hOldPen;		
		
		hPen    = CreatePen(PS_SOLID, 0, RGB(127, 127, 127));
		hOldPen = (HPEN) SelectObject(m_hPrinterDC, hPen);		
		
		MoveToEx(m_hPrinterDC, 0, iOffsetY + 5, NULL);
		LineTo(m_hPrinterDC, m_iPageWidth, iOffsetY + 5);	   

		SelectObject(m_hPrinterDC, hOldPen);
		SelectObject(m_hPrinterDC, hOldFont);

		return (iOffsetY + 10 /* extra space between rows */);
	}


} // TA_Base_App
