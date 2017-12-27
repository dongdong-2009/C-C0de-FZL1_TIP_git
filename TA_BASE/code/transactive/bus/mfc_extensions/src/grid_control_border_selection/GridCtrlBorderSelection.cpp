/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/grid_control_border_selection/GridCtrlBorderSelection.cpp $
  * @author:  Andy Siow
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $

  */
#include "bus/mfc_extensions/src/StdAfx.h"
#include "bus/mfc_extensions/src/grid_control_border_selection/GridCtrlBorderSelection.h"

#include "core/utilities/src/TAAssert.h"

#include <algorithm>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

    //
    // Constructor
    //
    GridCtrlBorderSelection::GridCtrlBorderSelection()
    : CGridCtrl() 
    {

		//Ripple: Instantiate the selectedRow auto pointer.
		m_selectedRow = std::auto_ptr<SelectedCell>(new SelectedCell());
		m_selectedRow->isRowSelected = false;	
    }


    //
    // Destructor
    //
    GridCtrlBorderSelection::~GridCtrlBorderSelection()
    {

    }



    BEGIN_MESSAGE_MAP(GridCtrlBorderSelection, CGridCtrl)
        //{{AFX_MSG_MAP(GridCtrlBorderSelection)

	ON_WM_LBUTTONDOWN()
	//}}AFX_MSG_MAP
    END_MESSAGE_MAP()


    //
    // PreSubclassWindow
    //
    void GridCtrlBorderSelection::PreSubclassWindow() 
    {

        SetEditable( FALSE );
        EnableDragAndDrop( FALSE );
        EnableSelection( FALSE );
        SetRowResize( FALSE );
        SetColumnResize( FALSE );

        // Set the background colour of the control so we cannot see where the background if the columns and rows are
        // smaller than the control.
        SetBkColor( GetSysColor(COLOR_3DFACE) );

        CGridCtrl::PreSubclassWindow();

    }

	void GridCtrlBorderSelection::OnDraw(CDC* pDC)
	{
		CRect rect;
		int row,col;

		CRect clipRect;
		if (pDC->GetClipBox(&clipRect) == ERROR) 
			return;

		EraseBkgnd(pDC);            // OnEraseBkgnd does nothing, so erase bkgnd here.
									// This necessary since we may be using a Memory DC.

		int nFixedRowHeight = GetFixedRowHeight();
		int nFixedColWidth  = GetFixedColumnWidth();

		CCellID idTopLeft = GetTopleftNonFixedCell();
		int minVisibleRow = idTopLeft.row,
			minVisibleCol = idTopLeft.col;

		CRect VisRect;
		CCellRange VisCellRange = GetVisibleNonFixedCellRange(VisRect);
		int maxVisibleRow = VisCellRange.GetMaxRow(),
			maxVisibleCol = VisCellRange.GetMaxCol();


		// draw top-left cells 0..m_nFixedRows-1, 0..m_nFixedCols-1
		rect.bottom = -1;
		for (row = 0; row < m_nFixedRows; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;
			rect.right = -1;
			for (col = 0; col < m_nFixedCols; col++)
			{
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;  
            
				DrawFixedCell(pDC, row, col, rect);
			}
		}
     
		// draw fixed column cells:  m_nFixedRows..n, 0..m_nFixedCols-1
		rect.bottom = nFixedRowHeight-1;
		for (row = minVisibleRow; row <= maxVisibleRow; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;

			// rect.bottom = bottom pixel of previous row
			if (rect.top > clipRect.bottom) 
				break;                // Gone past cliprect
			if (rect.bottom < clipRect.top)
				continue;             // Reached cliprect yet?

			rect.right = -1;
			for (col = 0; col < m_nFixedCols; col++)
			{
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;

				if (rect.left > clipRect.right) 
					break;            // gone past cliprect
				if (rect.right < clipRect.left) 
					continue;         // Reached cliprect yet?

				DrawFixedCell(pDC, row, col, rect);
			}
		}
    
		// draw fixed row cells  0..m_nFixedRows, m_nFixedCols..n
		rect.bottom = -1;
		for (row = 0; row < m_nFixedRows; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;

			// rect.bottom = bottom pixel of previous row
			if (rect.top > clipRect.bottom) 
				break;                // Gone past cliprect
			if (rect.bottom < clipRect.top) 
				continue;             // Reached cliprect yet?

			rect.right = nFixedColWidth-1;
			for (col = minVisibleCol; col <= maxVisibleCol; col++)
			{                                       
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;

				if (rect.left > clipRect.right)
					break;        // gone past cliprect
				if (rect.right < clipRect.left) 
					continue;     // Reached cliprect yet?

				DrawFixedCell(pDC, row, col, rect);
			}
		}
    
		// draw rest of non-fixed cells
		rect.bottom = nFixedRowHeight-1;
		for (row = minVisibleRow; row <= maxVisibleRow; row++)
		{
			rect.top = rect.bottom+1;
			rect.bottom = rect.top + GetRowHeight(row)-1;

			// rect.bottom = bottom pixel of previous row
			if (rect.top > clipRect.bottom) 
				break;                // Gone past cliprect
			if (rect.bottom < clipRect.top) 
				continue;             // Reached cliprect yet?

			rect.right = nFixedColWidth-1;
			for (col = minVisibleCol; col <= maxVisibleCol; col++)
			{
				rect.left = rect.right+1;
				rect.right = rect.left + GetColumnWidth(col)-1;

				if (rect.left > clipRect.right)
					break;        // gone past cliprect
				if (rect.right < clipRect.left) 
					continue;     // Reached cliprect yet?

				DrawCell(pDC, row, col, rect);
			}
		}


		CPen pen;
		TRY {
			pen.CreatePen(PS_SOLID, 0, m_crGridColour);
		}
		CATCH (CResourceException, e)
		{
			e->Delete();
			return;
		}
		END_CATCH
		pDC->SelectObject(&pen);

		// draw vertical lines (drawn at ends of cells)
		if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_VERT) 
		{
			int x = nFixedColWidth;
			for (col = minVisibleCol; col <= maxVisibleCol; col++) {
				x += GetColumnWidth(col);
				pDC->MoveTo(x-1, nFixedRowHeight);
				pDC->LineTo(x-1, VisRect.bottom);   
			}
		}
    
		// draw horizontal lines (drawn at bottom of each cell)
		if (m_nGridLines == GVL_BOTH || m_nGridLines == GVL_HORZ) 
		{
			int y = nFixedRowHeight;
			for (row = minVisibleRow; row <= maxVisibleRow; row++) {
				y += GetRowHeight(row);
				pDC->MoveTo(nFixedColWidth, y-1);    
				pDC->LineTo(VisRect.right,  y-1);
			}
		}

		if(m_selectedRow->isRowSelected)
		{
			CPen pen;
			TRY {
				pen.CreatePen(PS_SOLID, 3, RGB(256, 256, 256));
			}
			CATCH (CResourceException, e)
			{
				e->Delete();
				return;
			}
			END_CATCH
			pDC->SelectObject(&pen);

			CRect selectedRect;
			GetCellRangeRect(m_selectedRow->selectedRange, selectedRect);
			pDC->SelectStockObject(NULL_BRUSH);
			pDC->Rectangle(selectedRect);
		}


	#ifdef USE_MEMDC                        // Use a memDC for flicker free update
	}
	#else                                   // Use normal DC - this helps in debugging
	}
	#endif


	void GridCtrlBorderSelection::SelectRows(CCellID currentCell)
	{  
		if (!m_bEnableSelection)
			return;

		if(currentCell.row < m_nRows && currentCell.row > 0 )
		{

			// Overwrite this method if you don't want the selection to span the whole row lenght(m_nCols)
			CCellRange currentSelection(currentCell.row, 1, currentCell.row, m_nCols-1);

			if(m_selectedRow->isRowSelected)
			{
				// Clear the previous selection cell..
				for(unsigned int i=0; i<static_cast<unsigned int>(m_nCols)-1; ++i)
				{
					RedrawCell((m_selectedRow->selectedRange).GetMinRow(), i+1);		
				}
			}
				
			Invalidate(); // Invalidate just to make sure we clean up everything.
			m_selectedRow->selectedRange = currentSelection;
			m_selectedRow->isRowSelected = true;
			
		}

		//if (currentCell.row; == m_idCurrentCell.row) return;
		if (currentCell.row < m_nFixedRows)
			return;
		if (!IsValid(currentCell))
			return;

		if (GetListMode() && GetSingleRowSelection())
			SetSelectedRange(currentCell.row, GetFixedColumnCount(),
							 currentCell.row, GetColumnCount()-1);
		else 
			SetSelectedRange(min(m_SelectionStartCell.row, currentCell.row),
							 GetFixedColumnCount(), 
							 max(m_SelectionStartCell.row, currentCell.row), 
							 GetColumnCount()-1); 
	}


	void GridCtrlBorderSelection::OnLButtonDown(UINT nFlags, CPoint point)
	{
		HWND hOldFocusWnd = ::GetFocus();

		m_LeftClickDownPoint = point;
		m_LeftClickDownCell = GetCellFromPt(point);
    
		if (!IsValid(m_LeftClickDownCell)) return;


		m_SelectionStartCell = (nFlags & MK_SHIFT)? m_idCurrentCell : m_LeftClickDownCell;

		SetFocus();        // Auto-destroy any InPlaceEdit's

		// If the user clicks on the current cell, then prepare to edit it.
		// (If the user moves the mouse, then dragging occurs)
		if (m_LeftClickDownCell == m_idCurrentCell)
		{
			m_MouseMode = MOUSE_PREPARE_EDIT;
			return;
		}   
		else if (m_MouseMode != MOUSE_OVER_COL_DIVIDE &&
				 m_MouseMode != MOUSE_OVER_ROW_DIVIDE)        
		{

			if(m_LeftClickDownCell.col != 0)
			{
				if(m_selectedRow->isRowSelected)
				{
					// Clear the previous selection cell..
					for(unsigned int i=0; i<static_cast<unsigned int>(m_nCols)-1; ++i)
					{
						RedrawCell((m_selectedRow->selectedRange).GetMinRow(), i+1);		
					}
				}

				m_selectedRow->isRowSelected = false;

				Invalidate();

				SetFocusCell(-1,-1);
				SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
						 max(m_LeftClickDownCell.col, m_nFixedCols));
				
			} // If the whole row was selected and the selection is current, then we 
			  // want the whole row to be selected. 
			else if(m_LeftClickDownCell.col == 0 && m_selectedRow->isRowSelected)
			{
				SetFocusCell(-1,-1);
				SetFocusCell(max(m_LeftClickDownCell.row, m_nFixedRows),
						 max(m_LeftClickDownCell.col, m_nFixedCols));
			}

		}

	#ifndef GRIDCONTROL_NO_DRAGDROP
		// If the user clicks on a selected cell, then prepare to drag it.
		// (If the user moves the mouse, then dragging occurs)
		if (m_bAllowDragAndDrop && hOldFocusWnd == GetSafeHwnd() && 
			GetItemState(m_LeftClickDownCell.row, m_LeftClickDownCell.col) & GVNI_SELECTED)
		{
			m_MouseMode = MOUSE_PREPARE_DRAG;
			return;
		}
	#endif

		SetCapture();

		if (m_MouseMode == MOUSE_NOTHING)
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
			//else if (m_MouseMode != MOUSE_NOTHING)
			//{
			//    SetCursor(AfxGetApp()->LoadStandardCursor(IDC_ARROW));
			//    m_MouseMode = MOUSE_NOTHING;
			//}
		}

		if (m_MouseMode == MOUSE_OVER_COL_DIVIDE) // sizing column
		{
			m_MouseMode = MOUSE_SIZING_COL;
			CPoint start;
			if (!GetCellOrigin(0, m_LeftClickDownCell.col, &start)) return;

			CRect rect;
			GetClientRect(rect);
			CRect invertedRect(point.x, rect.top, point.x + 2, rect.bottom);

			CDC* pDC = GetDC();
			if (pDC) {
				pDC->InvertRect(&invertedRect);
				ReleaseDC(pDC);
			}

			if (point.x - start.x <= m_nResizeCaptureRange)        // clicked right of border
				if (!GetCellOrigin(0, --m_LeftClickDownCell.col, &start)) return;

			rect.left = start.x;
			ClientToScreen(rect);
	#ifndef _WIN32_WCE_NO_CURSOR
			ClipCursor(rect);
	#endif
		}
		else if (m_MouseMode == MOUSE_OVER_ROW_DIVIDE) // sizing row
		{
			m_MouseMode = MOUSE_SIZING_ROW;
			CPoint start;
			if (!GetCellOrigin(m_LeftClickDownCell, &start)) return;

			CRect rect;
			GetClientRect(rect);
			CRect invertedRect(rect.left, point.y, rect.right, point.y + 2);

			CDC* pDC = GetDC();
			if (pDC) {
				pDC->InvertRect(&invertedRect);
				ReleaseDC(pDC);
			}

			if (point.y - start.y <= m_nResizeCaptureRange)            // clicked below border
				if (!GetCellOrigin(--m_LeftClickDownCell.row, 0, &start)) return;

			rect.top = start.y;
			ClientToScreen(rect);
	#ifndef _WIN32_WCE_NO_CURSOR
			ClipCursor(rect);
	#endif
		}
		else // not sizing or editing -- selecting
		{    
			// If Ctrl pressed, save the current cell selection. This will get added
			// to the new cell selection at the end of the cell selection process
			m_PrevSelectedCellMap.RemoveAll();
			if (nFlags & MK_CONTROL) {
				for (POSITION pos = m_SelectedCellMap.GetStartPosition(); pos != NULL; )
				{
					DWORD key;
					CCellID cell;
					m_SelectedCellMap.GetNextAssoc(pos, key, (CCellID&)cell);
					m_PrevSelectedCellMap.SetAt(key, cell);
				}
			}
        
			if (m_LeftClickDownCell.row < GetFixedRowCount())
				OnFixedRowClick(m_LeftClickDownCell);
			else if (m_LeftClickDownCell.col < GetFixedColumnCount())
				OnFixedColumnClick(m_LeftClickDownCell);
			else
			{
				m_MouseMode = m_bListMode? MOUSE_SELECT_ROW : MOUSE_SELECT_CELLS;
				OnSelecting(m_LeftClickDownCell);
			}

			m_nTimerID = SetTimer(WM_LBUTTONDOWN, m_nTimerInterval, 0);

		}   
		m_LastMousePoint = point;

	}



