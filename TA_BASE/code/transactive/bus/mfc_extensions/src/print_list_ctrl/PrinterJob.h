/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/TA_Common_V1_TIP/transactive/bus/mfc_extensions/src/print_list_ctrl/PrinterJob.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2015/01/19 17:43:23 $
  * Last modified by:  $Author: CM $
  *
  */
// PrinterJob.h: interface for the CPrinterJob class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRINTERJOB_H__7EADD893_6F4C_11D1_A6CD_0000B43382FE__INCLUDED_)
#define AFX_PRINTERJOB_H__7EADD893_6F4C_11D1_A6CD_0000B43382FE__INCLUDED_

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

#ifdef IMPORT_EXTENSIONS
class __declspec(dllimport) CPrinterJob  : public CWinThread
#else
class AFX_EXT_CLASS CPrinterJob  : public CWinThread
#endif
{
	DECLARE_DYNCREATE(CPrinterJob)

public:
    static const UINT WM_START_PRINT;

	CWnd * pParent;
	CString strTitle;
	bool bAlreadyAskedPageOpt;

    /**
     * Constructor
     *
     * Initialises font and window title.
     */
	CPrinterJob();

    /**
     * Destructor
     */
	virtual ~CPrinterJob();

    /**
     * DoPreparePrinting
     *
     * Populates the CPrintInfo object with valid information relating
     * to page and printer choices.
     *
     * @param pInfo The stored print information.
     *
     * @return bool
     */
	bool DoPreparePrinting(CPrintInfo* pInfo);

    /**
     * OnPrepareDC
     *
     * @param pDC The device context that will be drawn to.
     * @param pInfo The stored print information.
     */
	void OnPrepareDC(CDC * pDC, CPrintInfo * pInfo);

    /**
     * OnFilePrint
     *
     * This method is called to start the printing process.
     */
	virtual void OnFilePrint();

    /**
     * OnPrint
     *
     * This method is responsible for rendering the current page onto the provided
     * device context. This default implementation does nothing.
     *
     * @param pDC The device context that will be drawn to.
     * @param pInfo The stored print information.
     */
	virtual void OnPrint(CDC* pDC, CPrintInfo* pInfo);
	
	//TD14936
	/*
	 *  To print the selection
	 *
	 *  @param pDC The device context that will be drawn to.
	 *  @param pInfo The stored print information.
	 */
	virtual void PrintSelection(CDC* pDC, CPrintInfo* pInfo);

    /**
     * OnEndPrinting
     *
     * This method is called when the printing process has completed and should
     * perform any required clean up. This default implementation does nothing.
     *
     * @param pDC The device context that will be drawn to.
     * @param pInfo The stored print information.
     */
	virtual void OnEndPrinting(CDC* pDC, CPrintInfo* pInfo);

    /**
     * OnBeginPrinting
     *
     * This method is called after the printing process has been prepared and should
     * perform any required initialisation, such as header and footer text. This
     * default implementation does nothing.
     *
     * @param pDC The device context that will be drawn to.
     * @param pInfo The stored print information.
     */
	virtual void OnBeginPrinting(CDC* pDC, CPrintInfo* pInfo);

    /**
     * OnPreparePrinting
     *
     * This method is the first method called when printing is started. This implementation
     * simply returns true if previewing, otherwise it will invoke DoPreparePrinting.
     *
     * @param pInfo The stored print information.
     * @param bPrintPreview Specifies whether the print should be previewed.
     */
	virtual bool OnPreparePrinting(CPrintInfo* pInfo, bool bPrintPreview = false);

public:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CListCtrlPrint)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL
	//TD18772 
	CListCtrl	* pList;	// pointer of the list ctrl

// Implementation
protected:
	CFont m_font;
	//Begin---- TD15057 && TD18775
	//Ensure to print correctly when there have many pages

	//the start row of alarms to print in a page
	long	m_lStartRowInPage;
	//the rows of the alarm
	long	m_lTotalRows;
	//When one alarm is printed in diferent pages,the start wrap row in the second page
	long	m_lStartWrapRow;
	//End---- TD15057 && TD18775
	//Begin---- TD14936
	POSITION	m_pos;
	bool		m_PrintSelection;//Indicate if continue printing selected alarms
	//End---- TD14936

    /**
     * threadedPrint
     *
     * This message handler is invoked when the printing is being performed from a thread. This
     * default implementation just terminates the thread.
     */
	virtual afx_msg void threadedPrint(WPARAM wParam, LPARAM lParam);

    /**
     * endThread
     *
     * This message handler doesn't seem to be used, however if invoked it will simply terminate
     * the current thread.
     */
	virtual afx_msg LRESULT endThread(WPARAM wParam, LPARAM lParam);

	// Generated message map functions
	//{{AFX_MSG(CPrinterJob)
		// NOTE - the ClassWizard will add and remove member functions here.
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.


#endif // !defined(AFX_PRINTERJOB_H__7EADD893_6F4C_11D1_A6CD_0000B43382FE__INCLUDED_)

