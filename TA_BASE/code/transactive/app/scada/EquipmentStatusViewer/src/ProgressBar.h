/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ProgressBar.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#ifndef PROGRESS_BAR_H
#define PROGRESS_BAR_H

namespace TA_Base_App
{

	class CProgressBar: public CProgressCtrl
	{

	public:
		CProgressBar();
		CProgressBar(CString aMessage, int aPercentSize = 100, 
									   int aMaxValue = 100, 
									   bool aSmooth = false, 
									   int aPane = 0);		

		bool Create(CString aMessage, int aPercentSize = 100, 
									  int aMaxValue = 100, 
									  bool aSmooth = false, 
									  int aPane = 0,
                                      CWnd* pParentWnd = NULL );

		~CProgressBar();

		DECLARE_DYNCREATE(CProgressBar)


	public:

		/**
		 *	SetRange
		 * 
		 *	Sets the range of values in the progress bar
		 *
		 *	@param	aLower	starting position
		 *	@param	aUpper	ending position
		 *	@param	aStep	jump by
		 *	@return			success (true) or failure (false);
		 *	
		 */
		bool SetRange(int aLower, int aUpper, int aStep = 1);

		bool SetMessage(CString aMessage);

		bool SetSize(int aPercentSize);
		
		void SetBarColour(COLORREF aColorBar);
		
		void SetBackgroundColour(COLORREF aColorBackground);
		
		int  SetPos(int aPos);
		
		int  OffsetPos(int aPos);
		
		int  SetStep(int aStep);
		
		int  StepIt();
		
		void Clear();


		//{{AFX_VIRTUAL(CProgressBar)
		//}}AFX_VIRTUAL


	protected:
		int		m_PercentSize;		// size of the control as a % of the size of the pane its in
		int		m_Pane;				// index of the pane this control is in
		CString	m_Message;			// message to display		
		CRect	m_Rect;				// dimensions of the control

		CStatusBar *GetStatusBar();	// gets the pointer to the status bar of the window

		bool Resize();				// resizes the control as the window is resized

        CWnd*   m_parentWnd;        // the parent of the CProgressBar (whose status bar will be used)

	protected:

		//{{AFX_MSG(CProgressBar)
		afx_msg BOOL OnEraseBkgnd(CDC* pDC);
		//}}AFX_MSG
		DECLARE_MESSAGE_MAP()

	};

} // TA_Base_App

#endif
