/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/NewLocation.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_NEWLOCATION_H__8C5FD08D_F770_4BA7_B671_781E97D252E7__INCLUDED_)
#define AFX_NEWLOCATION_H__8C5FD08D_F770_4BA7_B671_781E97D252E7__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewLocation.h : header file
//

#include <vector>
#include "core/data_access_interface/src/OnlinePrintCfgAccessFactory.h"
#include "app/configuration/config_plugins/online_printer/src/resource.h"
#include "core/data_access_interface/src/IConfigOnlinePrintCfg.h"

/////////////////////////////////////////////////////////////////////////////
// CNewLocation dialog
namespace TA_Base_App
{
    class CNewLocation : public CDialog
    {
    // Construction
    public:
	    CNewLocation(CWnd* pParent = NULL);   // standard constructor

        // check if there are any locations unused
        BOOL canCreateNew();

        TA_Base_Core::IConfigOnlinePrintCfg* m_newItem;
        
    // Dialog Data
	    //{{AFX_DATA(CNewLocation)
	    enum { IDD = IDD_NEWLOCATION };
	    CComboBox			m_locationCombo;
	    //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CNewLocation)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CNewLocation)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        std::vector<TA_Base_Core::OnlinePrintCfgAccessFactory::LocationRecord> m_locations;
    };
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWLOCATION_H__8C5FD08D_F770_4BA7_B671_781E97D252E7__INCLUDED_)
