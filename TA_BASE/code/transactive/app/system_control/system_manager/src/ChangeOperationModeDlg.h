/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/ChangeOperationModeDlg.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * ChangeOperationModeDlg is the class associated with the
  * Change Operation Mode dialog.
  *
  */

#if !defined(AFX_CHANGEOPERATIONMODEDLG_H__6CBF071A_BD8C_4AA9_8FB0_FEB16ADD9D11__INCLUDED_)
#define AFX_CHANGEOPERATIONMODEDLG_H__6CBF071A_BD8C_4AA9_8FB0_FEB16ADD9D11__INCLUDED_

#include "Resource.h"
#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include <string>
#include <vector>

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

namespace TA_Base_App
{

    class ChangeOperationModeDlg : public CDialog
    {

    public:
    
        /** 
          * Constructor
          *
          * @param processes  The entity names of the processes that we will be
          *                   changing the operation mode of.
          * @param parent     The parent window.
          *
          */
        ChangeOperationModeDlg(const std::vector<std::string>& processes, 
                                CWnd* parent = NULL);

        // Dialog Data
	    //{{AFX_DATA(ChangeOperationModeDlg)
	enum { IDD = IDD_CHANGEOPERATIONMODE_DIALOG };
	CListBox	m_processList;
	//}}AFX_DATA


        // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(ChangeOperationModeDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL
    
    private:
    
        /**
          * The entity names of the processes that we'll be changing the
          * the operating mode of.
          */
        std::vector<std::string> m_processes;

        /**
          * The operation mode we're changing to.
          */
        TA_Base_Core::EOperationMode m_operationMode;

	    // Generated message map functions
	    //{{AFX_MSG(ChangeOperationModeDlg)
	    virtual void OnOK();
	    virtual BOOL OnInitDialog();
	    afx_msg void OnRadioControl();
	    afx_msg void OnRadioMonitor();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

#endif // !defined(AFX_CHANGEOPERATIONMODEDLG_H__6CBF071A_BD8C_4AA9_8FB0_FEB16ADD9D11__INCLUDED_)
