/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/3002_TIP/TA_BASE/transactive/app/soe/event_viewer/src/StatusProgressBar.h $
 * @author:  Nick Jardine
 * @version: $Revision: #3 $
 *
 * Last modification: $DateTime: 2009/12/03 15:46:08 $
 * Last modified by:  $Author: huirong.luo $
 * 
 * StatusProgressBar is an extesion of CStatsBar to include a CProgressCtrl in the status bar. It behaves like a normal
 * status bar, but provides methods to initialise, increment, and remove an embedded progress bar.
 */

#if !defined(AFX_STATUSPROGRESSBAR_H__1D505CB5_785F_4C29_A32E_7975B23609DE__INCLUDED_)
#define AFX_STATUSPROGRESSBAR_H__1D505CB5_785F_4C29_A32E_7975B23609DE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// StatusProgressBar.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// StatusProgressBar window

class StatusProgressBar : public CStatusBar
{
// Construction
public:
    /**
     * constructor
     */
	StatusProgressBar();

// Attributes
public:

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(StatusProgressBar)
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~StatusProgressBar();

	// Generated message map functions
protected:
	//{{AFX_MSG(StatusProgressBar)
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()

public:

    /**
     * startProgressBar
     *
     * Initialises the progress bar over the specifed status bar pane.
     * 
     * @param pane The numerical identifier of the status bar pane that is to contain the progress bar.
     */
    void startProgressBar(UINT pane);

    /**
     * setProgressBarPos
     *
     * Sets the position of the progress bar.
     *
     * @param pos The new position.
     */
    void setProgressBarPos(UINT pos);

    /**
     * incrementProgressBar
     *
     * Increments the progress bar by the specified amount, up to the maximum value.
     *
     * @param count The number of units to increment the progress bar by.
     */
    void incrementProgressBar(UINT count);

    /**
     * removeProgressBar
     *
     * Removes the progress bar from the status bar, and restores the usual status bar display.
     */
    void removeProgressBar();

    /**
     * resizeProgressBar
     *
     * Resizes the progress bar to match the progress pane.
     */
    void resizeProgressBar();

    /**
     * setProgressRange
     *
     * Sets the value range for the progress bar. The increment method should be called (upper-lower)/count times (approx).
     *
     * @param lower The initial value of the progress range.
     * @param upper The highest value the progress bar can take on.
     */
    void setProgressRange(UINT lower,UINT upper);

private:

    // The progress bar
    CProgressCtrl m_progressBar;

    // The numerical identifier indicating which pane currently contains the progress bar.
    UINT m_progressPane;
};

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_STATUSPROGRESSBAR_H__1D505CB5_785F_4C29_A32E_7975B23609DE__INCLUDED_)
