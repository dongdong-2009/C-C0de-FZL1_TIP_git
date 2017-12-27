/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ProcessParamsDlg.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ProcessParamsDlg is associated with the dialog box allowing operators 
  * to set the processes' runtime parameters. In the current version, it  
  * only allows the debug level of processes to be changed.
  *
  */

#if !defined(AFX_PROCESSPARAMSDLG_H__8A2F2951_3A0A_11D5_B147_0050BAB0C7E8__INCLUDED_)
#define AFX_PROCESSPARAMSDLG_H__8A2F2951_3A0A_11D5_B147_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{

    class ProcessParamsDlg : public CDialog
    {

    public:

        /**
          * ProcessParamsDlg
          * 
          * Constucts a ProcessParamDlg with preselected processes. If the 
          * preselected processes have the same value for a parameter, this value
          * will be the displayed value for the parameter. Otherwise the displayed 
          * value will be the default one for that parameter.
          */
        ProcessParamsDlg(const std::vector<std::string> processes, CWnd* pParent = NULL);

        // Dialog Data
	    //{{AFX_DATA(ProcessParamsDlg)
	    enum { IDD = IDD_PROCESS_PARAMS };
	    CComboBox	m_debugLevel;
	    //}}AFX_DATA

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ProcessParamsDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    private:

        /**
          * The processes that we will be setting the run param for.
          */
        std::vector<std::string> m_processes;

	    // Generated message map functions
	    //{{AFX_MSG(ProcessParamsDlg)
	    virtual void OnOK();
	    virtual BOOL OnInitDialog();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PROCESSPARAMSDLG_H__8A2F2951_3A0A_11D5_B147_0050BAB0C7E8__INCLUDED_)
