/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/4677_ALL_TIP/TA_BASE/transactive/app/system_control/system_manager/src/OptionsDlg.h $
  * @author:  Dominique Beauregard
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2010/08/12 12:11:21 $
  * Last modified by:  $Author: builder $
  *
  * OptionsDlg is associated with the dialog that allows
  * operators to set the data refresh rate.
  *
  */

#if !defined(AFX_OPTIONSDLG_H__679184BF_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
#define AFX_OPTIONSDLG_H__679184BF_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Resource.h"

namespace TA_Base_App
{

    class OptionsDlg : public CDialog
    {

    public:

        /**
          * Constructor
          */
	    OptionsDlg(CWnd* pParent = NULL);

    // Dialog Data
	    //{{AFX_DATA(OptionsDlg)
	    enum { IDD = IDD_OPTIONS_DIALOG };
	    CComboBox	m_RefreshRate;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(OptionsDlg)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    protected:

	    // Generated message map functions
	    //{{AFX_MSG(OptionsDlg)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()
    };

} // namespace TA_Base_App

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONSDLG_H__679184BF_2D48_11D5_B140_0050BAB0C7E8__INCLUDED_)
