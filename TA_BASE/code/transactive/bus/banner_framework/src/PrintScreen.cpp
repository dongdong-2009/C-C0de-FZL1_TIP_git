
#include "stdafx.h"
#include "winspool.h"
#include "commdlg.h"
#include "core/utilities/src/DebugUtil.h"
#include "bus\banner_framework\src\PrintScreen.h"

#include "bus/mfc_extensions/src/prnScreen/prnScreen.h"

UINT CALLBACK PrintHookProc(
							HWND hdlg,      // handle to the dialog box window
							UINT uiMsg,     // message identifier
							WPARAM wParam,  // message parameter
							LPARAM lParam   // message parameter
							)
{
	switch(uiMsg)
	{
	case WM_INITDIALOG:
		{
			// save the handle to print dialog window for future use.
			SetWindowPos(hdlg,
				CWnd::wndTopMost,         // handle to window
				0, 0, 0, 0,          // dont-cares since,
				// this window options is set to
				// SWP_NOSIZE
				SWP_NOSIZE | SWP_NOMOVE    // window-positioning options
				);
			break;
		}
	}
	return 0;
}

namespace TA_Base_Bus
{
	// On Construction capture the screen
	PrintScreen::PrintScreen()
	{
		// Determine Screen information
		m_numOfScreens = GetSystemMetrics(SM_CMONITORS);
		m_ScrWidth = GetSystemMetrics(SM_CXSCREEN);
		m_SrcHeight = GetSystemMetrics(SM_CYSCREEN);

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Screen info: width = %ld, height = %ld and Number of Screen = %d", m_ScrWidth, m_SrcHeight, m_numOfScreens);

		int screenPos = 1;
		//Determined what screen we are
		CPoint pt(GetMessagePos());
	
		if (m_numOfScreens > 1)
		{	// Multiple screen
			for (int i=0; i< m_numOfScreens; i++)
			{
				if (pt.x > ((i*m_ScrWidth) + 1) && pt.x < ((i+1)*m_ScrWidth))
				{
					break;
				}
				screenPos++;
			}
		}

		LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugInfo, "Capture print screen at monitor = %d", screenPos);

		//Capture the correct screen
		captureScreen(screenPos);
	}

	PrintScreen::~PrintScreen()
	{
		// Destroys the objects
		SelectObject(m_memDC, m_oldBitmap);
		DeleteObject(m_memBitmap);
		DeleteDC(m_memDC);
	}

	void PrintScreen::printObject()
	{
		HDC dc;
		PRINTDLG pd;
		BOOL bPrintingOK, bDialog = TRUE;

		memset( &pd, 0, sizeof(PRINTDLG) ) ;

		//Fill out PrintDialog structure
		pd.lStructSize = sizeof(PRINTDLG);
		pd.hDC = NULL;
		pd.hwndOwner = NULL;
		pd.hInstance = NULL;
		pd.nMaxPage = 1;
		pd.nMinPage = 1;
		pd.nFromPage = 1;
		pd.nToPage = 1;
		pd.nCopies = 1;
		pd.hDevMode = NULL;
		pd.hDevNames = NULL;
		pd.lpfnPrintHook = PrintHookProc;
		if (bDialog)
			pd.Flags = PD_RETURNDC | PD_USEDEVMODECOPIESANDCOLLATE | PD_ENABLEPRINTHOOK;
		else
			pd.Flags = PD_RETURNDC | PD_RETURNDEFAULT;


		if(PrintDlg(&pd) == FALSE)
		{
			// Exit on cancel
			return;
		}
		
		dc = pd.hDC;
		
		char * strTitle;
		strTitle = "PrintScreen";
		DOCINFO di;                             // Initialise print document details
		::ZeroMemory (&di, sizeof (DOCINFO));
		di.cbSize = sizeof (DOCINFO);
		di.lpszDocName = strTitle;

		//    BOOL bPrintingOK = dc.StartDoc(&di);    // Begin a new print job
		::StartDoc(dc, &di);
		::StartPage(dc);
		// Get the printing extents and store in the m_rectDraw field of a 

		// Create a compatible bitmap to the printer
		CBitmap* objBm = CBitmap::FromHandle(m_memBitmap);
		HANDLE hDIB = DDBToDIB(*objBm);
		
		SetStretchBltMode(dc, HALFTONE);
 		FloodFill(dc, 0,0,RGB(255,255,255));
		
		double xRatio = (GetDeviceCaps(dc, HORZRES)-50) / (double) m_ScrWidth;
		double yRatio = (GetDeviceCaps(dc, VERTRES)-50) / (double) m_SrcHeight;

		
		if (yRatio < xRatio) xRatio = yRatio;

		if(GDI_ERROR == StretchDIBits(dc, 25, 25, (int)(xRatio * m_ScrWidth), (int) (xRatio * m_SrcHeight), 0, 0, m_lpbi->biWidth, m_lpbi->biHeight, (LPBYTE)m_lpbi + m_lpbi->biSize, (LPBITMAPINFO)m_lpbi, DIB_RGB_COLORS, SRCCOPY))
		{
			DWORD err = GetLastError();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to write bitmap to document, StretchDIBits failed with error code = %ld", err);
		}

		bPrintingOK = (::EndPage(dc)>0);

		if (bPrintingOK)
			::EndDoc(dc);       // end a print job
		else
			::AbortDoc(dc);     // abort job.

		::DeleteDC(dc);			// delete the printer DC
		GlobalFree(hDIB);		// Release memory
	}

	void PrintScreen::captureScreen(const int screenNum)
	{
		//Start Capture Screen
		m_memDC = CreateCompatibleDC(NULL);
		HDC winDC = GetDC(NULL);

		m_memBitmap = CreateCompatibleBitmap(winDC, m_ScrWidth, m_SrcHeight);
		m_oldBitmap = (HBITMAP) SelectObject(m_memDC, m_memBitmap);

		int targetX = (screenNum==1)?0:(((screenNum-1)*m_ScrWidth) + 1);
		int targetY = 0;//m_SrcHeight;

		// Initialize memDC to white
		FloodFill(m_memDC, 0, 0,RGB(255,255,255));

		if(!BitBlt(m_memDC, 0, 0, m_ScrWidth, m_SrcHeight, winDC, targetX, targetY, SRCCOPY))
		{
			DWORD err = GetLastError();
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "Unable to write bitmap to document, BitBlt failed with error code = %ld", err);
		}

		ReleaseDC(NULL, winDC);
	}


	// DDBToDIB		- Creates a DIB from a DDB
	// bitmap		- Device dependent bitmap
	// dwCompression	- Type of compression - see BITMAPINFOHEADER
	// pPal			- Logical palette
	HANDLE PrintScreen::DDBToDIB(CBitmap& bitmap, DWORD dwCompression/* = BI_RGB */, CPalette* pPal /* = NULL*/) 
	{
		BITMAP			bm;
		BITMAPINFOHEADER	bi;
		
		DWORD			dwLen;
		HANDLE			hDIB;
		HANDLE			handle;
		HDC 			hDC;
		HPALETTE		hPal;
		WORD			cClrBits;

		ASSERT( bitmap.GetSafeHandle() );

		// The function has no arg for bitfields
		if( dwCompression == BI_BITFIELDS )
			return NULL;

		// If a palette has not been supplied use default palette
		hPal = (HPALETTE) pPal->GetSafeHandle();
		if (hPal==NULL)
			hPal = (HPALETTE) GetStockObject(DEFAULT_PALETTE);

		// Get bitmap information
		bitmap.GetObject(sizeof(bm),(LPSTR)&bm);


		// Convert the color format to a count of bits.
		cClrBits = (WORD)(bm.bmPlanes * bm.bmBitsPixel);
		if (cClrBits == 1)
			cClrBits = 1;
		else if (cClrBits <= 4)
			cClrBits = 4;
		else if (cClrBits <= 8)
			cClrBits = 8;
		else if (cClrBits <= 16)
			cClrBits = 16;
		else if (cClrBits <= 24)
			cClrBits = 24;
		else cClrBits = 32;

		// Initialize the bitmapinfoheader
		bi.biSize		= sizeof(BITMAPINFOHEADER);
		bi.biWidth		= bm.bmWidth;
		bi.biHeight 		= bm.bmHeight;
		bi.biPlanes 		= bm.bmPlanes;
		bi.biBitCount		= bm.bmBitsPixel;
		bi.biCompression	= dwCompression;
		bi.biSizeImage		= 0; //(bi.biWidth + 7) /8 * bi.biHeight * cClrBits;;
		bi.biXPelsPerMeter	= 0;
		bi.biYPelsPerMeter	= 0;
		bi.biClrUsed		= 0;
		if (cClrBits < 24)
			bi.biClrUsed = (1<<cClrBits);

		bi.biClrImportant	= 0;

		// Compute the size of the  infoheader and the color table
		int nColors = (1 << bi.biBitCount);
		
		if( bi.biBitCount > 8 )
			nColors = 0;

		dwLen  = bi.biSize + nColors * sizeof(RGBQUAD);

		// We need a device context to get the DIB from
		hDC = GetDC(NULL);
		hPal = SelectPalette(hDC,hPal,FALSE);
		RealizePalette(hDC);

		// Allocate enough memory to hold bitmapinfoheader and color table
		hDIB = GlobalAlloc(GMEM_FIXED,dwLen);

		if (!hDIB){
			SelectPalette(hDC,hPal,FALSE);
			ReleaseDC(NULL,hDC);
			return NULL;
		}

		m_lpbi = (LPBITMAPINFOHEADER)hDIB;

		*m_lpbi = bi;

		// Call GetDIBits with a NULL lpBits param, so the device driver 
		// will calculate the biSizeImage field 
		int nRetVal = GetDIBits(hDC, (HBITMAP)bitmap.GetSafeHandle(), 0L, (DWORD)bi.biHeight,
			(LPBYTE)NULL, (LPBITMAPINFO)m_lpbi, (DWORD)DIB_RGB_COLORS);
		
		if (nRetVal == 0)
		{
			LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugError, "GetDIBits Failed.");
			return NULL;
		}


		bi = *m_lpbi;

		// If the driver did not fill in the biSizeImage field, then compute it
		// Each scan line of the image is aligned on a DWORD (32bit) boundary
		if (bi.biSizeImage == 0){
			bi.biSizeImage = ((((bi.biWidth * bi.biBitCount) + 31) & ~31) / 8) 
				* bi.biHeight;

			// If a compression scheme is used the result may infact be larger
			// Increase the size to account for this.
			if (dwCompression != BI_RGB)
				bi.biSizeImage = (bi.biSizeImage * 3) / 2;
		}

		// Realloc the buffer so that it can hold all the bits
		dwLen += bi.biSizeImage;
		if (handle = GlobalReAlloc(hDIB, dwLen, GMEM_MOVEABLE))
			hDIB = handle;
		else{
			GlobalFree(hDIB);

			// Reselect the original palette
			SelectPalette(hDC,hPal,FALSE);
			ReleaseDC(NULL,hDC);
			return NULL;
		}

		// Get the bitmap bits
		m_lpbi = (LPBITMAPINFOHEADER)hDIB;

		// FINALLY get the DIB
		BOOL bGotBits = GetDIBits( hDC, (HBITMAP)bitmap.GetSafeHandle(),
			0L,				// Start scan line
			(DWORD)bi.biHeight,		// # of scan lines
			(LPBYTE)m_lpbi 			// address for bitmap bits
			+ (bi.biSize + nColors * sizeof(RGBQUAD)),
			(LPBITMAPINFO)m_lpbi,		// address of bitmapinfo
			(DWORD)DIB_RGB_COLORS);		// Use RGB for color table

		if( !bGotBits )
		{
			GlobalFree(hDIB);

			SelectPalette(hDC,hPal,FALSE);
			ReleaseDC(NULL,hDC);
			return NULL;
		}

		SelectPalette(hDC,hPal,FALSE);
		ReleaseDC(NULL,hDC);
		return hDIB;
	}

}