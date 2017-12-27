/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/StartProcessDlg.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * StartProcessDlg is associated with the dialog box allowing 
  * operators to launch a process.
  *
  */

#if !defined(AFX_STARTPROCESSDLG_H__679184B8_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_STARTPROCESSDLG_H__679184B8_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"

#include <string>
#include <vector>

namespace TA_Base_App
{

    class StartProcessDlg : public CDialog
    {

    public:

        /**
          * Constructor
          *
          * @param processes  The entity names of the processes to start.
          * @param parent     The parent window.
          */
	    StartProcessDlg(const std::vector<std::string>& processes, CWnd* parent = NULL);

        // Dialog Data
	    //{{AFX_DATA(StartProcessDlg)
	enum { IDD = IDD_STARTPROCESS_DIALOG };
	CListBox	m_processList;
	    CButton	m_okButton;
	//}}AFX_DATA

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(StartProcessDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

private:

        /**
          * The entity names of the processes to start.
          */
        std::vector<std::string> m_processes;

	    // Generated message map functions
	    //{{AFX_MSG(StartProcessDlg)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_STARTPROCESSDLG_H__679184B8_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
