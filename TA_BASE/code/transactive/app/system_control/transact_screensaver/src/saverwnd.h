/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/transact_screensaver/src/saverwnd.h $
  * @author:  Jiang Haipeng
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * <description>
  *
  */

#if !defined(SAVERWND_H)
#define SAVERWND_H

#if _MSC_VER >= 1000
#pragma once
#endif // _MSC_VER >= 1000

/////////////////////////////////////////////////////////////////////////////
// CSaverWnd window
#include "app/system_control/transact_screensaver/src/Animation.h"
#include "app/system_control/transact_screensaver/src/ScreenSaverLoginDlg.h"

namespace TA_Base_App
{

    class CSaverWnd  : public CWnd //: public CDrawWnd
    {
    // Construction
    public:
	    CSaverWnd();

    // Operations
    public:
 	    BOOL Create( HWND hwndParent = 0);

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CSaverWnd)
	    protected:
	    virtual void PostNcDestroy();
	    virtual LRESULT WindowProc(UINT message, WPARAM wParam, LPARAM lParam);
	    //}}AFX_VIRTUAL

    // Implementation
    public:
	    void CloseSaverWindow();
	    void LayoutText( CDC& dc, CRect rect );
	    bool isPasswordValid(const CString &strPWD);
	    virtual ~CSaverWnd();

	    // Generated message map functions
    protected:
	    //{{AFX_MSG(CSaverWnd)
	    afx_msg void OnTimer(UINT nIDEvent);
	    afx_msg void OnPaint();
	    afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	    afx_msg void OnDestroy();
	    afx_msg void OnClose();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

        /** 
          * isControlStationActivated
          *
          * description, check whether the ControlSation is running, if run, assigne the current username to the m_strOperator
          *
          *
          * @return BOOL, true: the ControlStation is running, otherwise, not running or not login
          */
	    bool isControlStationActivated() ;

        /** 
          * VerifyPassword
          *
          * description, to verify the password that user inputs, if true, the system continue to run the screen saver wnd, otherwise return
          *
          * @param hWndParent
          *
          * @return TRUE: the user's input is correct, othewise false
          */
	    bool VerifyPassword(HWND hWndParent) ;
    
    private:
	    CString m_strOperator;
        
    // Attributes
    public:
	    HBRUSH m_hBrushBackground;
	    CFont m_font;
	    CUIntArray m_anRequestQueue;
 
	    CAnimationEngine m_animationEngine;
	    CRect m_clientRect;	
	    BOOL m_bIsDialogActive;    // if Verify Password dialog box is active
	    BOOL m_bReallyClose;    // for NT, if a WM_CLOSE came from us or it.
  
	    DWORD m_dwPasswordDelay;    // Password Delay in seconds
	    CScreenSaverLoginDlg m_loginDlg;
 
        static const CString NAME_SEPARATOR;

    };
}
/////////////////////////////////////////////////////////////////////////////

#endif // !defined(SAVERWND_H)
