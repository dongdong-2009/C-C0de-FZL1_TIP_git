/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ShutdownSystemControllerDlg.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * ShutdownSystemControllerDlg is associated with the dialog box 
  * allowing operators to shutdown the System Controller currently
  * logged into.
  *
  */

#if !defined(AFX_SHUTDOWNSYSTEMCONTROLLERDLG_H__CA6BD944_2D8D_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_SHUTDOWNSYSTEMCONTROLLERDLG_H__CA6BD944_2D8D_11D5_B140_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{

    class ShutdownSystemControllerDlg : public CDialog
    {

    public:
        ShutdownSystemControllerDlg(std::string hostname, CWnd* pParent = NULL);
 
        // Dialog Data
	    //{{AFX_DATA(ShutdownSystemControllerDlg)
	    enum { IDD = IDD_SHUTDOWN_MONITOR_DIALOG };
	    CButton	m_shutdownButton;
	    CStatic	m_hostnameLabel;
	    //}}AFX_DATA

        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ShutdownSystemControllerDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL
  
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(ShutdownSystemControllerDlg)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:

        /**
          * The hostname of the System Controller we're shutting down.
          */
        std::string m_hostname;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_SHUTDOWNSYSTEMCONTROLLERDLG_H__CA6BD944_2D8D_11D5_B140_0050BAB0C7E8__INCLUDED_)
