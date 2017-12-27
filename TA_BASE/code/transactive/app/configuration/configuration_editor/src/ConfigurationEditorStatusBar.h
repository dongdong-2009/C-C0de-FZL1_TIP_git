/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/configuration_editor/src/ConfigurationEditorStatusBar.h $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class manipulates the status bar for the CMainFrame class. All status bar manipulations have
  * been put in this class.
  */


#if !defined(AFX_ConfigurationEditorStatusBar_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
#define AFX_ConfigurationEditorStatusBar_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_

#include <string>

#include "app/configuration/configuration_editor/src/IProgressStatusBar.h"

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{
    class ConfigurationEditorStatusBar  : public CStatusBar, public IProgressStatusBar
    {

    public:
        /**
          * Constructor
          *
          * @param CFrameWnd* - This is a pointer to the main frame which will ultimately be our parent
          *
          * @exception TransactiveException - Thrown if the status bar cannot be created
          */
	    ConfigurationEditorStatusBar(CFrameWnd* statusBarParent);


        /**
          * Destructor
          */
        virtual ~ConfigurationEditorStatusBar();

        // Overrides
        //{{AFX_VIRTUAL(CTextStatusBar)
        //}}AFX_VIRTUAL

        /**
          * displayProgressBar
          *
          * This displays a progress bar over the top of the status bar and returns the control
          * so that the progress can be incremented and controlled
          *
          * @return CProgressCtrl& - The progress control showing on the status bar
          */
        virtual CProgressCtrl& displayProgressBar();
        

        /**
          * hideProgressBar
          *
          * This hides the progress bar so that it is no longer showing over the status bar
          */
        virtual void hideProgressBar();


        /**
          * setStatusBarText
          *
          * This sets the current text displayed in the status bar.
          *
          * @param const CString& - The text to display
          */
        void setStatusBarText(const CString& textToDisplay);


    protected:
        //{{AFX_MSG(ConfigurationEditorStatusBar)
        //}}AFX_MSG
        afx_msg LRESULT OnSetText(WPARAM, LPARAM); 
        DECLARE_MESSAGE_MAP()

    private:
        /**
          * Copy Constructor
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorStatusBar( const ConfigurationEditorStatusBar&){ };


        /**
          * Operator =
          *
          * Hidden as a private method so it is not used.
          */
        ConfigurationEditorStatusBar& operator=(const ConfigurationEditorStatusBar&);


        std::string m_text;             // This is the text to display in our status bar.

        CFrameWnd* m_statusBarParent;   // This is the parent of the status bar. The main frame window

        CProgressCtrl m_progressControl;  // This is the progress control that will be shown and hidden
                                          // over the top of the status bar.
    };
}

/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_ConfigurationEditorStatusBar_H__E81E79BF_8A84_4604_9377_F3A8C1935E5A__INCLUDED_)
