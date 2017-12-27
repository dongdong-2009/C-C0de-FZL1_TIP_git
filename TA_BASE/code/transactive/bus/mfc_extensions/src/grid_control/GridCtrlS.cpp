// GridCtrlS.cpp: implementation of the CGridCtrlS class.
//
//////////////////////////////////////////////////////////////////////

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/grid_control/GridCtrlS.cpp $
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

#include "../stdafx.h"
//#include "mfc_extensions.h"
#include "GridCtrlS.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CGridCtrlS::CGridCtrlS()
{
	CGridCtrl::CGridCtrl();
}

CGridCtrlS::~CGridCtrlS()
{
	
}

//TD15533++
BOOL CGridCtrlS::SetItemS(const GV_ITEM* pItem)
{
    if (!pItem) 
        return FALSE;
    CGridCell* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell) 
        return FALSE;

    if (pItem->mask & GVIF_TEXT)   pCell->szTextS  = pItem->szTextS;
    if (pItem->mask & GVIF_IMAGE)  pCell->iImageS  = pItem->iImageS;
	if (pItem->mask & GVIF_IMAGE)  pCell->bOverrided = pItem->bOverrided; //reuses GVIF_IMAGE //TD15533

    return TRUE;
}
//++TD15533

//TD15533++
BOOL CGridCtrlS::GetItemS(GV_ITEM* pItem)
{
    if (!pItem)
        return FALSE;
    CGridCell* pCell = GetCell(pItem->row, pItem->col);
    if (!pCell) 
        return FALSE;

    if (pItem->mask & GVIF_TEXT)   pItem->szTextS  = GetItemTextS(pItem->row, pItem->col);
    if (pItem->mask & GVIF_IMAGE)  pItem->iImageS  = pCell->iImageS;
	if (pItem->mask & GVIF_IMAGE)  pItem->bOverrided = pCell->bOverrided; //reuses GVIF_IMAGE //TD15533

    return TRUE;
}
//++TD15533

//TD15533++
BOOL CGridCtrlS::SetItemTextS(int nRow, int nCol, LPCTSTR str)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    if (!pCell)
        return FALSE;

    pCell->szTextS = str;
    return TRUE;
}
//++TD15533

//TD15533++
CString CGridCtrlS::GetItemTextS(int nRow, int nCol)
{
    if (nRow < 0 || nRow >= m_nRows || nCol < 0 || nCol >= m_nCols) return "";

    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) return "";

    return pCell->szTextS;
}
//++TD15533

//TD15533++
BOOL CGridCtrlS::SetItemImageS(int nRow, int nCol, int iImage)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    if (!pCell) 
        return FALSE;

    pCell->iImageS = iImage;
    return TRUE;
}
//++TD15533

//TD15533++
int CGridCtrlS::GetItemImageS(int nRow, int nCol) const
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) 
        return -1;

    return pCell->iImageS;
}
//++TD15533

//TD15533++
BOOL CGridCtrlS::SetItemOverrided(int nRow, int nCol, BOOL bOverrided)
{
    CGridCell* pCell = GetCell(nRow, nCol);
    if (!pCell) 
        return FALSE;

    pCell->bOverrided = bOverrided;
    return TRUE;
}
//++TD15533

//TD15533++
BOOL CGridCtrlS::GetItemOverrided(int nRow, int nCol) const
{
    CGridCell* pCell = GetCell(nRow, nCol);
    ASSERT(pCell);
    if (!pCell) 
        return -1;

    return pCell->bOverrided;
}
//++TD15533

/* NOTE: The following overrided method reuses code in CGridCtrl 
 * Any changes made in the parent class should be reflected here.
 */

void CGridCtrlS::EmptyCell(CGridCell* pCell, int nRow, int nCol)
{
	CGridCtrl::EmptyCell(pCell, nRow, nCol);

	pCell->szTextS.Empty(); //TD15533
}

void CGridCtrlS::OnMouseMove(UINT nFlags, CPoint point)
{
    CRect rect;
    GetClientRect(rect);

#ifndef GRIDCONTROL_NO_DRAGDROP
    // If outside client area, return (unless we are drag n dropping)
    if (m_MouseMode != MOUSE_DRAGGING && !rect.PtInRect(point))
        return;
#endif

    // If the left mouse button is up, then test to see if row/column sizing is imminent
    if (!(nFlags & MK_LBUTTON) 
        || (m_MouseMode == MOUSE_NOTHING && (nFlags & MK_LBUTTON)))
    {
        if (m_bAllowColumnResize && MouseOverColumnResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_COL_DIVIDE)
            {
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZEWE));
                m_MouseMode = MOUSE_OVER_COL_DIVIDE;
            }
        }
        else if (m_bAllowRowResize && MouseOverRowResizeArea(point))
        {
            if (m_MouseMode != MOUSE_OVER_ROW_DIVIDE)
            {
                SetCursor(AfxGetApp()->LoadStandardCursor(IDC_SIZENS));
                m_MouseMode = MOUSE_OVER_ROW_DIVIDE;
            }
        }
        else if (m_MouseMode != MOUSE_NOTHING)
        {
            SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
            m_MouseMode = MOUSE_NOTHING;
        }

#ifndef GRIDCONTROL_NO_TITLETIPS
        if (m_MouseMode == MOUSE_NOTHING && m_bTitleTips)
        {
            CCellID idCurrentCell = GetCellFromPt(point);
            CRect TextRect, CellRect;
            if (GetTextRect(idCurrentCell.row, idCurrentCell.col, TextRect) &&
                GetCellRect(idCurrentCell.row, idCurrentCell.col, CellRect))
			{
				//TD15533++
				CString text = GetItemText(idCurrentCell.row, idCurrentCell.col);
				CString text2 = GetItemTextS(idCurrentCell.row, idCurrentCell.col);
				if (GetItemOverrided(idCurrentCell.row, idCurrentCell.col))
				{
					if (text.Compare("") != 0) // base actionGroup is not empty
					{
						// text has already been filled up
						text += SEPARATOR;
					}
					else // base actionGroup is empty
					{
						// compute the length of override actionGroup
						if (text2.Compare("") != 0)
						{
							text += EMPTY_SPACE;
							text += SEPARATOR;
						}
						// override actionGroup is empty
						else
						{
							// no separator, this will be a gray cell
						}
					}
				}
				text += text2;
				//++TD15533

                m_TitleTip.Show(TextRect, 
                                //GetItemText(idCurrentCell.row, idCurrentCell.col), //TD15533++
								text, //TD15533++
                                0,
                                CellRect,
                                GetItemFont(idCurrentCell.row, idCurrentCell.col));
			}
        }
#endif

        m_LastMousePoint = point;
        return;
    }

    if (!IsValid(m_LeftClickDownCell))
    {
        m_LastMousePoint = point;
        return;
    }

    // If the left mouse button is down, then process appropriately
    if (nFlags & MK_LBUTTON) 
    {
        switch(m_MouseMode)
        {
            case MOUSE_SELECT_ALL:        
                break;

            case MOUSE_SELECT_COL:
            case MOUSE_SELECT_ROW:    
            case MOUSE_SELECT_CELLS:    
                {
                    CCellID idCurrentCell = GetCellFromPt(point);
                    if (!IsValid(idCurrentCell)) 
                        return;
				
                    if (idCurrentCell != GetFocusCell())
                    {
                        OnSelecting(idCurrentCell);
                        //SetFocusCell(max(idCurrentCell.row, m_nFixedRows),
                        //             max(idCurrentCell.col, m_nFixedCols));
                        if (idCurrentCell.row >= m_nFixedRows &&
                            idCurrentCell.col >= m_nFixedCols)
                        {
                            SetFocusCell(idCurrentCell);
                        }
                    }
                    break;
                }

            case MOUSE_SIZING_COL:   
                {
                    CDC* pDC = GetDC();
                    if (!pDC) break;
                    
                    CRect oldInvertedRect(m_LastMousePoint.x, rect.top, 
                        m_LastMousePoint.x + 2, rect.bottom);
                    pDC->InvertRect(&oldInvertedRect);
                    CRect newInvertedRect(point.x, rect.top, 
                        point.x + 2, rect.bottom);
                    pDC->InvertRect(&newInvertedRect);
                    ReleaseDC(pDC);
                }
                break;
                
            case MOUSE_SIZING_ROW:        
                {
                    CDC* pDC = GetDC();
                    if (!pDC) break;
                    
                    CRect oldInvertedRect(rect.left, m_LastMousePoint.y, 
                        rect.right, m_LastMousePoint.y + 2);
                    pDC->InvertRect(&oldInvertedRect);
                    CRect newInvertedRect(rect.left, point.y, 
                        rect.right, point.y + 2);
                    pDC->InvertRect(&newInvertedRect);
                    ReleaseDC(pDC);
                }
                break;

#ifndef GRIDCONTROL_NO_DRAGDROP
            case MOUSE_PREPARE_DRAG:   
                OnBeginDrag();    
                break;
#endif
        }    
    }

    m_LastMousePoint = point;
}


BOOL CGridCtrlS::DrawCell(CDC* pDC, int nRow, int nCol, CRect rect, BOOL bEraseBk)
{
    if (!m_bAllowDraw) 
        return FALSE;

    GV_ITEM Item;
    Item.mask = GVIF_TEXT | GVIF_FORMAT | GVIF_STATE | GVIF_IMAGE | GVIF_BKCLR | GVIF_FGCLR;
    Item.row = nRow;
    Item.col = nCol;
    if (!GetItem(&Item))
        return FALSE;

	GetItemS(&Item); //TD15533

    COLORREF TextBkClr = (Item.crBkClr == CLR_DEFAULT)? GetTextBkColor() : Item.crBkClr;
    COLORREF TextClr   = (Item.crFgClr == CLR_DEFAULT)? GetTextColor()   : Item.crFgClr;

    // Force redraw of background if custom colour
    //if (Item.crBkClr != CLR_DEFAULT)
        bEraseBk = TRUE;

    int nSavedDC = pDC->SaveDC();

    pDC->SetBkMode(TRANSPARENT);

    if (Item.state & GVIS_FOCUSED 
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
        && !pDC->IsPrinting()
#endif
        ) 
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBk) 
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
        }
        rect.right--; rect.bottom--;    
        pDC->SelectStockObject(BLACK_PEN);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(rect);
        pDC->SetTextColor(TextClr);

        rect.DeflateRect(1,1);

    }
    else if (Item.state & GVIS_SELECTED 
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
        && !pDC->IsPrinting()
#endif
        ) 
    {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        pDC->FillSolidRect(rect, ::GetSysColor(COLOR_HIGHLIGHT));
        rect.right--; rect.bottom--;
        pDC->SetTextColor(::GetSysColor(COLOR_HIGHLIGHTTEXT));
    } else {
        rect.right++; rect.bottom++;    // FillRect doesn't draw RHS or bottom
        if (bEraseBk) 
        {
            CBrush brush(TextBkClr);
            pDC->FillRect(rect, &brush);
        }
        rect.right--; rect.bottom--;
        pDC->SetTextColor(TextClr);
    }

    if (Item.state & GVIS_DROPHILITED 
#if !defined(_WIN32_WCE_NO_PRINTING) && !defined(GRIDCONTROL_NO_PRINTING)
        && !pDC->IsPrinting()
#endif
        )
    {
        pDC->SelectStockObject(BLACK_PEN);
        pDC->SelectStockObject(NULL_BRUSH);
        pDC->Rectangle(rect);
    }

    // Create the appropriate font and select into DC
    LOGFONT *pLF = GetItemFont(nRow, nCol);

    //CCellID FocusCell = GetFocusCell();
    //if (IsCellFixed(nRow, nCol) && (FocusCell.row == nRow || FocusCell.col == nCol))
    //{
    //    static LOGFONT lf;
    //    memcpy(&lf, pLF, sizeof(LOGFONT));
    //    lf.lfWeight = SELECTED_CELL_FONT_WEIGHT;
    //    pLF = &lf;
    //}
   
    CFont Font;
    Font.CreateFontIndirect(pLF);
    pDC->SelectObject(&Font);

    rect.DeflateRect(m_nMargin, 0);

	//TD15533++
	if (Item.szText.Compare("") != 0)
	{
		if (m_pImageList && Item.iImage >= 0)
		{
			IMAGEINFO Info;
			if (m_pImageList->GetImageInfo(Item.iImage, &Info))
			{
				//CRgn rgn;
				//rgn.CreateRectRgnIndirect(rect);
				//pDC->SelectClipRgn(&rgn);
				//rgn.DeleteObject();

				int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
				m_pImageList->Draw(pDC, Item.iImage, rect.TopLeft(), ILD_NORMAL);
				rect.left += nImageWidth+m_nMargin;
			}
	
		}

		//DrawText(pDC->m_hDC, GetVisibleCellText(*pDC, rect, Item.szText), -1, rect, Item.nFormat);
		DrawText(pDC->m_hDC, Item.szText, -1, rect, Item.nFormat|DT_WORD_ELLIPSIS); //TD15533
	}
	//++TD15533

	//TD15533++
	//add separator for overrided profile
	if (Item.bOverrided)
	{
		if (Item.szText.Compare("") != 0) // base actionGroup is not empty
		{
			rect.left += pDC->GetTextExtent(Item.szText).cx;
			DrawText(pDC->m_hDC, SEPARATOR, -1, rect, Item.nFormat|DT_WORD_ELLIPSIS); 
			rect.left += pDC->GetTextExtent(SEPARATOR).cx;
		}
		else // base actionGroup is empty
		{
			// compute the length of override actionGroup
			if (Item.szTextS.Compare("") != 0)
			{
				rect.left += pDC->GetTextExtent(EMPTY_SPACE).cx;
				DrawText(pDC->m_hDC, SEPARATOR, -1, rect, Item.nFormat|DT_WORD_ELLIPSIS); 
				rect.left += pDC->GetTextExtent(SEPARATOR).cx;
			}
			// override actionGroup is empty
			else
			{
				// no separator, this will be a gray cell
			}
		}
	}
	//++TD15533

	//TD15533++
	if (Item.szTextS.Compare("") != 0)
	{
		CPoint point = CPoint(rect.left, rect.top);
		if (m_pImageList && Item.iImageS >= 0)
		{
			IMAGEINFO Info;
			if (m_pImageList->GetImageInfo(Item.iImageS, &Info))
			{
				//CRgn rgn;
				//rgn.CreateRectRgnIndirect(rect);
				//pDC->SelectClipRgn(&rgn);
				//rgn.DeleteObject();

				int nImageWidth = Info.rcImage.right-Info.rcImage.left+1;
				m_pImageList->Draw(pDC, Item.iImageS, point, ILD_NORMAL); //TD15533
				rect.left += nImageWidth+m_nMargin;
			}
		}

		//DrawText(pDC->m_hDC, GetVisibleCellText(*pDC, rect, Item.szTextS), -1, rect, Item.nFormat);
		DrawText(pDC->m_hDC, Item.szTextS, -1, rect, Item.nFormat|DT_WORD_ELLIPSIS); 
	}
	//++TD15533

    pDC->RestoreDC(nSavedDC);
    Font.DeleteObject();

    return TRUE;
}
