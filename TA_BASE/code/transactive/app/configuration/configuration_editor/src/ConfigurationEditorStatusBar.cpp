/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorStatusBar.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manipulates the status bar for the CMainFrame class. All status bar manipulations have
  * been put in this class.
  */

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorStatusBar.h"

#include "app/configuration/configuration_editor/src/resource.h"

#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TAAssert.h"
#include "core/exceptions/src/TransactiveException.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Core::DebugUtil;

namespace TA_Base_App
{
    // These are the indicators to show in the bottom right of the status bar
    static UINT indicators[] =
    {
        ID_SEPARATOR,           // status line indicator
	    ID_INDICATOR_CAPS,
	    ID_INDICATOR_NUM,
	    ID_INDICATOR_SCRL,
    };


    ConfigurationEditorStatusBar::ConfigurationEditorStatusBar(CFrameWnd* statusBarParent)
        : m_statusBarParent(statusBarParent), m_text("")
    {
        FUNCTION_ENTRY("Constructor");

        TA_ASSERT(m_statusBarParent != NULL, "Parent for the status bar is NULL");
        
	    if (!Create(m_statusBarParent) ||
		    !SetIndicators(indicators, sizeof(indicators)/sizeof(UINT)))
	    {
            TA_THROW( TA_Base_Core::TransactiveException("Failed to create status bar") );
	    }

        char text[100];
        ::LoadString(AfxGetInstanceHandle(),AFX_IDS_IDLEMESSAGE,text,100);

        m_text = text;
        

        m_statusBarParent->RecalcLayout(FALSE);

        FUNCTION_EXIT;
    }


    ConfigurationEditorStatusBar::~ConfigurationEditorStatusBar()
    {
    }


BEGIN_MESSAGE_MAP(ConfigurationEditorStatusBar, CStatusBar)
//{{AFX_MSG_MAP(ConfigurationEditorStatusBar)
//}}AFX_MSG_MAP
ON_MESSAGE( WM_SETTEXT, OnSetText )
END_MESSAGE_MAP()


    LRESULT ConfigurationEditorStatusBar::OnSetText(WPARAM wParam, LPARAM lParam)
    {
        if (lParam && 0 == strcmp((LPCTSTR)lParam, "" ))
        {
            // We want to use our m_text member variable
            return CStatusBar::OnSetText( wParam, (LPARAM)(m_text.c_str()) );
        }

        // Reset this so next time we move over an unknown toolbar or menu item we show "Ready"
        char text[100];
        ::LoadString(AfxGetInstanceHandle(),AFX_IDS_IDLEMESSAGE,text,100);
        m_text = text;

        return CStatusBar::OnSetText( wParam, lParam );
    } 


    void ConfigurationEditorStatusBar::setStatusBarText(const CString& textToDisplay)
    {
        m_text = std::string(textToDisplay);
        SetPaneText(0,m_text.c_str());
    }


    CProgressCtrl& ConfigurationEditorStatusBar::displayProgressBar()
    {
        FUNCTION_ENTRY("displayProgressBar");

        // If someone has forgotten to hide the progress bar we will do it ourselves
        if (m_progressControl.m_hWnd != NULL)
        {
            hideProgressBar();
        }

        // Get the position of the status bar so we can put the progress bar over the top
        CRect statusPosition;
        GetWindowRect( &statusPosition );
        m_statusBarParent->ScreenToClient( &statusPosition );
        
        // Create the progress bar
        m_progressControl.Create(WS_CHILD|WS_VISIBLE,statusPosition,m_statusBarParent,IDC_PROGRESS);
        m_progressControl.ShowWindow(SW_SHOW);

        // Ensure the progress bar shows over the top of the status bar
        m_progressControl.SetWindowPos(&CWnd::wndTop,0,0,0,0,SWP_NOMOVE|SWP_NOSIZE);

        FUNCTION_EXIT;
        return m_progressControl;
    }


    void ConfigurationEditorStatusBar::hideProgressBar()
    {
        FUNCTION_ENTRY("hideProgressBar");

        m_progressControl.DestroyWindow();

        FUNCTION_EXIT;
    }
}
