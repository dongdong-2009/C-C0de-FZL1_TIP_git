/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/PrintUtility.h $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class which prints the equipment status report to a selected printer. All printers supported
 * and all paper sizes/orientations supported
 *
 *  TD 14173 & 14535:   This PrintUtility will no longer be used.
 *                      Use mfc_extensions/ListCtrlPrint common class instead.
 *
 */

#if !defined(AFX_PRINTUTILITY_H__6ED9D4EF_5901_4644_AEDD_7ABF3199BC4C__INCLUDED_)
#define AFX_PRINTUTILITY_H__6ED9D4EF_5901_4644_AEDD_7ABF3199BC4C__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#pragma message( "This file not in use (remove from project): " __FILE__) 

#include "CustomDefines.h"

namespace TA_Base_App
{
	class CPrintUtility  
	{

	public:

		/**
		 *	Print()
		 *
		 *	Prints the equipment status report to a printer
		 *
		 *	@param	aParentWnd		Pointer to the parent window
		 *	@param	aLocationName	Name of the location to include in the header
		 *	@param	aReportName		Name of the report to include in the header
		 *	@param	aComment		Comment to include in the report header
		 *  @return					bool (true) for success and false for failure
		 *	
		 */
		bool Print(CWnd *aParentWnd, CString &aLocationName, CString &aReportName, CString &aComment);

		/**
		 *	SetListCtrl()
		 *
		 *	Sets the pointer to the list control from which information to print will be fetched
		 *
		 *	@param	pList	Pointer to the list ctrl
		 *
		 */
		void SetListCtrl(CListCtrl *pList);

		/**
		 *	CPrintUtility()
		 *
		 *	Default constructor
		 *
		 */
		CPrintUtility();

		/**
		 *	~CPrintUtility()
		 *
		 *	Default destructor
		 *
		 */
		virtual ~CPrintUtility();

	private:

        enum FIXED_STRING_LENGTHS { HEADER_STRING_LENGTH = 22 };


        /**
         * releases any open handles to printer status
         */
        void releaseHandles();

		/**
		 *	GetMaxStringLen()
		 *
		 *	Gets the maximum number of characters any one item has in a particular column
		 *
		 *	@param	aCol	Column from which items will be fetched and max returned
		 *  @return			Maximum number of characters
		 *
		 */
		int GetMaxStringLen(int aCol);

		/**
		 *	GetPrinterHeightFromScreenHeight()
		 *
		 *	Converts the Screen height to Printer height of the font
		 *
		 *	@param	aFontScreenHeight	Height of the font in screen dimensions
		 *  @return						Height of the font in printer dimensions
		 *
		 */
		int GetPrinterHeightFromScreenHeight(int aFontScreenHeight);

		/**
		 *	PrintHeader()
		 *
		 *	Prints the header of the document
		 *
		 *	@param	aOffsetY		How far from top of page (Y dimension) to start from
		 *  @param	aHeaderFont		Header font to use
		 *	@param	aLocationName	Name of the location to print
		 *	@param	aReportName		Name of the report to print
		 *	@param	aComment		Comment to print
		 *	@param	aPageCounter	Page counter of the current page
		 *	@return					Y offset adjusted after printing the header
		 *
		 */
		int PrintHeader(int aOffsetY, HFONT aHeaderFont, 
									CString &aLocationName, 
									CString &aReportName, 
									CString &aComment, int aPageCounter);

		/**
		 *	PrintListCtrlHeader()
		 *
		 *	Prints the list ctrl header (columns) 
		 *
		 *	@param	iOffsetY			How far from top of page (Y dimension) to start from
		 *  @param	hListFont			List font to use
		 *	@param	piColWidthTable		Widths of columns
		 *	@param	piColFormatTable	Alignment of columns
		 *	@param	pszColTitleTable	Titles of columns
		 *	@return						Y offset adjusted after printing the header
		 *
		 */
		int PrintListCtrlHeader(int iOffsetY, 
									HFONT hListFont, 
									int *piColWidthTable, 
									int *piColFormatTable, 
									char pszColTitleTable[ESL_COLUMN_MAX][HEADER_STRING_LENGTH]);


		/**
		 *	PrintListCtrlLine()
		 *
		 *	Prints the list ctrl items (as individual lines)
		 *
		 *	@param	iOffsetY			How far from top of page (Y dimension) to start from
		 *  @param	hListFont			List font to use
		 *	@param	iLine				Current line being printed
		 *	@param	piColWidthTable		Widths of columns
		 *	@param	piColFormatTable	Alignment of columns
		 *	@return	pbNewPage			Returns true if we need to print on a new page or false if not
		 *	@return						Y offset adjusted after printing the header
		 *
		 */
		int PrintListCtrlLine(int iOffsetY, 
									HFONT hListFont, 
									int iLine, 
									int *piColWidthTable, 
									int *piColFormatTable, 
									BOOL *pbNewPage);
		CListCtrl	*m_pListCtrl;
		HDC			m_hPrinterDC;
        HANDLE      m_hDevMode;
        HANDLE      m_hDevNames;
		int			m_iPageWidth;
		int			m_iPageHeight;
	};

} // TA_Base_App

#endif // !defined(AFX_PRINTUTILITY_H__6ED9D4EF_5901_4644_AEDD_7ABF3199BC4C__INCLUDED_)
