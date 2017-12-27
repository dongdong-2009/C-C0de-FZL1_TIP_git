/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/print_list_ctrl/PrinterJob.cpp $
  * @author:  Ripple
  * @version: $Revision: #2 $
  *
  * Last modification: $DateTime: 2017/06/07 20:50:52 $
  * Last modified by:  $Author: limin.zhu $
  *
  */
// PrinterJob.cpp: implementation of the CPrinterJob class.
//
//////////////////////////////////////////////////////////////////////

#include "bus\mfc_extensions\src\stdafx.h"
#include "bus\mfc_extensions\src\print_list_ctrl\PrinterJob.h"
#include "bus\mfc_extensions\src\standard_dialogs_arial_bold\FileDialogArial.h"
#include <stdlib.h>

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

IMPLEMENT_DYNCREATE(CPrinterJob, CWinThread)

const UINT CPrinterJob::WM_START_PRINT = WM_USER + 123;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

bool bGfxUserAbort;

BOOL CALLBACK EXPORT _GfxAbortProc( HDC, int )
{
	MSG msg;
	while (!bGfxUserAbort && ::PeekMessage(&msg, NULL, NULL, NULL, PM_NOREMOVE))
	{
		if (!AfxGetThread()->PumpMessage()) return FALSE;
	}

	return !bGfxUserAbort;
}
//TD15057 && TD18775
CPrinterJob::CPrinterJob():m_lStartRowInPage(0),m_lStartWrapRow(0),m_lTotalRows(0)
{
	m_font.CreatePointFont(90, "ËÎÌו", NULL); // 10pt Arial Bold
	pParent = NULL;
    strTitle = AfxGetAppName(); 

	bAlreadyAskedPageOpt = false;
}

CPrinterJob::~CPrinterJob()
{
}

BEGIN_MESSAGE_MAP(CPrinterJob, CWinThread)

	//{{AFX_MSG_MAP(CPrinterJob)
		// NOTE - the ClassWizard will add and remove mapping macros here.
	//}}AFX_MSG_MAP

	ON_THREAD_MESSAGE(WM_START_PRINT,threadedPrint)
END_MESSAGE_MAP()



BOOL CPrinterJob::InitInstance()
{	
	return TRUE;
}

int CPrinterJob::ExitInstance()
{
	return CWinThread::ExitInstance();
}

bool CPrinterJob::OnPreparePrinting(CPrintInfo * pInfo, bool bPrintPreview)
{
	if (bPrintPreview) return true;
	return DoPreparePrinting(pInfo);
}

void CPrinterJob::OnBeginPrinting(CDC * pDC, CPrintInfo * pInfo)
{
}

void CPrinterJob::OnEndPrinting(CDC * pDC, CPrintInfo * pInfo)
{
}

void CPrinterJob::OnPrint(CDC * pDC, CPrintInfo * pInfo)
{
}
void CPrinterJob::PrintSelection(CDC * pDC, CPrintInfo * pInfo)
{
}

bool CPrinterJob::DoPreparePrinting(CPrintInfo * pInfo)
{
	ASSERT(pInfo != NULL);
	ASSERT(pInfo->m_pPD != NULL);
	if (pInfo->m_pPD->m_pd.nMinPage > pInfo->m_pPD->m_pd.nMaxPage)
		pInfo->m_pPD->m_pd.nMaxPage = pInfo->m_pPD->m_pd.nMinPage;
	pInfo->m_pPD->m_pd.nFromPage = (WORD)pInfo->GetMinPage();

	//Begin----	  TD14936
	pInfo->m_pPD->m_pd.Flags &= ~PD_NOSELECTION;
	pInfo->m_pPD->m_pd.Flags &= ~PD_NOPAGENUMS;
	//End----	  TD14936

	pInfo->m_pPD->m_pd.nToPage = (WORD)pInfo->GetMaxPage();

	if (AfxGetApp()->DoPrintDialog(pInfo->m_pPD) != IDOK) return FALSE;
	ASSERT(pInfo->m_pPD != NULL);
	ASSERT(pInfo->m_pPD->m_pd.hDC != NULL);
	if (pInfo->m_pPD->m_pd.hDC == NULL) return FALSE;
	pInfo->m_nNumPreviewPages = AfxGetApp()->m_nNumPreviewPages;
	VERIFY(pInfo->m_strPageDesc.LoadString(AFX_IDS_PREVIEWPAGEDESC));
	return TRUE;
}

void CPrinterJob::OnFilePrint()
{
	CPrintInfo printInfo;

	ASSERT(printInfo.m_pPD != NULL);    // must be set

	if (OnPreparePrinting(&printInfo))
	{
		// hDC must be set (did you remember to call DoPreparePrinting?)
		ASSERT(printInfo.m_pPD->m_pd.hDC != NULL);
		
		// gather file to print to if print-to-file selected
		CString strOutput;
		if (printInfo.m_pPD->m_pd.Flags & PD_PRINTTOFILE)
		{
			// construct CFileDialog for browsing
			CString strDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULTEXT));
			CString strPrintDef(MAKEINTRESOURCE(AFX_IDS_PRINTDEFAULT));
			CString strFilter(MAKEINTRESOURCE(AFX_IDS_PRINTFILTER));
			CString strCaption(MAKEINTRESOURCE(AFX_IDS_PRINTCAPTION));
			CFileDialogArial dlg(FALSE, strDef, strPrintDef,	OFN_HIDEREADONLY|OFN_OVERWRITEPROMPT, strFilter);
			//dlg.SetFont(&m_font);
			dlg.m_ofn.lpstrTitle = strCaption;
			if (dlg.DoModal() != IDOK) return;
			// set output device to resulting path name
			strOutput = dlg.GetPathName();
		}
		DOCINFO docInfo;
		memset(&docInfo, 0, sizeof(DOCINFO));
		docInfo.cbSize = sizeof(DOCINFO);
		docInfo.lpszDocName = strTitle;
		CString strPortName;
		int nFormatID;
		if (strOutput.IsEmpty())
		{
			docInfo.lpszOutput = NULL;
			strPortName = printInfo.m_pPD->GetPortName();
			nFormatID = AFX_IDS_PRINTONPORT;
		}
		else
		{
			docInfo.lpszOutput = strOutput;
			nFormatID = AFX_IDS_PRINTTOFILE;
		}
		
		// setup the printing DC
		CDC dcPrint;

		dcPrint.Attach(printInfo.m_pPD->m_pd.hDC);  // attach printer dc

		// Print in a landscape mode.
		DEVMODE* pDevMode = printInfo.m_pPD->GetDevMode();
		
		if (pDevMode != NULL)
		{
			pDevMode->dmOrientation = DMORIENT_LANDSCAPE;
			dcPrint.ResetDC(pDevMode);
			::GlobalUnlock(pDevMode);
		}
		
		dcPrint.m_bPrinting = TRUE;
	
		
		printInfo.m_rectDraw.SetRect(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
		
		
		dcPrint.DPtoLP(&printInfo.m_rectDraw);
		
		int temp = (int) printInfo.GetMaxPage();
		
		OnBeginPrinting(&dcPrint, &printInfo);
		temp = (int) printInfo.GetMaxPage();
		dcPrint.SetAbortProc(_GfxAbortProc);
		
		// disable main window while printing & init printing status dialog
		CString csFormat, cs, strTemp;
		AfxGetMainWnd()->EnableWindow(FALSE);
		
		// start document printing process
		if (dcPrint.StartDoc(&docInfo) == SP_ERROR)
		{
			// enable main window before proceeding
			AfxGetMainWnd()->EnableWindow(TRUE);

			// cleanup and show error message
			OnEndPrinting(&dcPrint, &printInfo);
			dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
			return;
		}
		//Begin---- TD15057 && TD18775
		
		/*
		// Guarantee values are in the valid range
		UINT nEndPage = printInfo.GetToPage();
		UINT nStartPage = printInfo.GetFromPage();

		if (nEndPage < printInfo.GetMinPage()) nEndPage = printInfo.GetMinPage();
		if (nEndPage > printInfo.GetMaxPage()) nEndPage = printInfo.GetMaxPage();
		if (nStartPage < printInfo.GetMinPage()) nStartPage = printInfo.GetMinPage();
		if (nStartPage > printInfo.GetMaxPage()) nStartPage = printInfo.GetMaxPage();
		int nStep = (nEndPage >= nStartPage) ? 1 : -1;
		nEndPage = (nEndPage == 0xffff) ? 0xffff : nEndPage + nStep;
		int iPageTot = abs(abs(nEndPage) - abs(nStartPage)), iPrintedPages = 0;
		*/
		// begin page printing loop
	//	DWORD dwStart = GetTickCount(), dTickPerPage = 0;

		bool bRectSet = false;
		BOOL bError = FALSE;
		
		
//		for (printInfo.m_nCurPage = nStartPage; printInfo.m_nCurPage != nEndPage; printInfo.m_nCurPage += nStep)
		//Begin---- TD14936
		long lSelect = printInfo.m_pPD->PrintSelection();
		if(lSelect > 0)
		{
			
			m_pos = pList->GetFirstSelectedItemPosition();
			if (m_pos)
			{
				m_PrintSelection = true;
				m_lStartRowInPage = pList->GetNextSelectedItem(m_pos);
			}
			else
			{
				m_PrintSelection = false;
			}
			while(m_PrintSelection)
			{
//				iPrintedPages ++;
				OnPrepareDC(&dcPrint, &printInfo);

// 				// check for end of print	
				if (!printInfo.m_bContinuePrinting) break;
// 				// write current page

				printInfo.m_rectDraw.SetRect(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
				dcPrint.DPtoLP(&printInfo.m_rectDraw);

				// attempt to start the current page
 				if (dcPrint.StartPage() < 0)
 				{
 					bError = TRUE;
 					break;
 				}
				OnPrepareDC(&dcPrint, &printInfo);
				ASSERT(printInfo.m_bContinuePrinting);
				// page successfully started, so now render the page
				int is = dcPrint.SaveDC();
				PrintSelection(&dcPrint, &printInfo);
				if (dcPrint.EndPage() < 0 || !_GfxAbortProc(dcPrint.m_hDC, 0))
				{
					bError = TRUE;
					break;
				}
				printInfo.m_nCurPage += 1;
				dcPrint.RestoreDC(is);
			}
		}
		else
		{
			while(m_lStartRowInPage < m_lTotalRows)
			{
//				iPrintedPages ++;
				
				OnPrepareDC(&dcPrint, &printInfo);
				
// 				// check for end of print	
				if (!printInfo.m_bContinuePrinting) break;
				if (printInfo.GetToPage() < printInfo.m_nCurPage) break;
// 				// write current page
/*
				DWORD tck = dTickPerPage * (iPageTot - iPrintedPages + 1) / 1000;
				if (tck > 60)
				{
 					DWORD tckS = tck%60;
 					tck /= 60;
 					strTemp.Format("%d minuti, %d secondi", tck, tckS);
				}
 				else
 				{
 					if (iPageTot > 1)
 					{
 						if (tck > 0) strTemp.Format("%d secondi", tck);
 						else strTemp = "??";
					}
 				}
*/
				printInfo.m_rectDraw.SetRect(0, 0, dcPrint.GetDeviceCaps(HORZRES), dcPrint.GetDeviceCaps(VERTRES));
				dcPrint.DPtoLP(&printInfo.m_rectDraw);

				// attempt to start the current page
 				if (dcPrint.StartPage() < 0)
 				{
 					bError = TRUE;
 					break;
 				}
				OnPrepareDC(&dcPrint, &printInfo);
				ASSERT(printInfo.m_bContinuePrinting);
				// page successfully started, so now render the page
				int is = dcPrint.SaveDC();
				temp = (int) printInfo.GetMaxPage();
				OnPrint(&dcPrint, &printInfo);
				if (dcPrint.EndPage() < 0 || !_GfxAbortProc(dcPrint.m_hDC, 0))
				{
					bError = TRUE;
					break;
				}
				printInfo.m_nCurPage += 1;
				dcPrint.RestoreDC(is);

//				dTickPerPage = (GetTickCount() - dwStart) / iPrintedPages;
			}

			//End---- TD15057 && TD18775

		}
		//End---- TD14936

		// cleanup document printing process
		if (!bError) dcPrint.EndDoc();
		else dcPrint.AbortDoc();
		AfxGetMainWnd()->EnableWindow();    // enable main window
		OnEndPrinting(&dcPrint, &printInfo);    // clean up after printing
//		dlgPrintStatus.DestroyWindow();
		dcPrint.Detach();   // will be cleaned up by CPrintInfo destructor
	}
}

void CPrinterJob::OnPrepareDC(CDC * pDC, CPrintInfo * pInfo)
{
	ASSERT_VALID(pDC);
	UNUSED(pDC); // unused in release builds

	// Default to one page printing if doc length not known
	if (pInfo != NULL)
		pInfo->m_bContinuePrinting =
			(pInfo->GetMaxPage() != 0xffff || (pInfo->m_nCurPage == 1));
}

void CPrinterJob::threadedPrint(WPARAM wParam, LPARAM lParam)
{
    // Default implementation just ends the thread.
    AfxEndThread( 0, TRUE );
}

LRESULT CPrinterJob::endThread(WPARAM wParam, LPARAM lParam)
{
    // Default implementation just ends the thread.
    AfxEndThread( 0, TRUE );
    return 0L;
}
