/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/NewItem.h $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
#if !defined(AFX_NEWITEM_H__9934DE7B_4E19_4BFD_A673_F1B29DFE995A__INCLUDED_)
#define AFX_NEWITEM_H__9934DE7B_4E19_4BFD_A673_F1B29DFE995A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// NewItem.h : header file
//
#include "app/configuration/config_plugins/online_printer/src/resource.h"
#include "bus/mfc_extensions/src/text_colour_edit/TextColourEdit.h"
#include "core/data_access_interface/src/IConfigOnlinePrinter.h"

/////////////////////////////////////////////////////////////////////////////
// CNewItem dialog

namespace TA_Base_App
{
    class CNewItem : public CDialog
    {
    // Construction
    public:
	    CNewItem(unsigned long pLocation, CWnd* pParent = NULL);   // standard constructor

        TA_Base_Core::IConfigOnlinePrinter* m_newItem;

    // Dialog Data
	    //{{AFX_DATA(CNewItem)
	    enum { IDD = IDD_NEWITEM };
	    //TA_Base_Bus::TextColourEdit		m_printerEdit;
		CComboBox			m_PrinterCombo;
	    CComboBox			m_subsystemCombo;
	    CComboBox			m_typeCombo;
        //}}AFX_DATA


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(CNewItem)
	    protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL

    // Implementation
    protected:

	    // Generated message map functions
	    //{{AFX_MSG(CNewItem)
	    virtual BOOL OnInitDialog();
	    virtual void OnOK();
	    virtual void OnCancel();
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()

    private:
        unsigned long m_location;
    };
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_NEWITEM_H__9934DE7B_4E19_4BFD_A673_F1B29DFE995A__INCLUDED_)
