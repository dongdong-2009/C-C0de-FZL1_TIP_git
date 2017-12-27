/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/NewItem.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// NewItem.cpp : implementation file
//
#pragma warning(disable:4786)

#include <vector>

#include "app/configuration/config_plugins/online_printer/src/StdAfx.h"
#include "app/configuration/config_plugins/online_printer/src/NewItem.h"

#include "core/data_access_interface/src/OnlinePrinterAccessFactory.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewItem dialog

typedef TA_Base_Core::OnlinePrinterAccessFactory::SubsystemRecord SubsystemRecord;
//using TA_Base_Core::OnlinePrinterAccessFactory::SubsystemRecord;

namespace TA_Base_App
{
    static const int PRINTER_FIELD_LENGTH = 50;

    CNewItem::CNewItem(unsigned long pLocation, CWnd* pParent /*=NULL*/)
	    : CDialog(CNewItem::IDD, pParent), m_newItem(NULL), m_location(pLocation)
    {
	    //{{AFX_DATA_INIT(CNewItem)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void CNewItem::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CNewItem)
	    DDX_Control(pDX, IDC_COMBO_SUBSYSTEM, m_subsystemCombo);
	    DDX_Control(pDX, IDC_COMBO_TYPE, m_typeCombo);
		//TD17780 marc_bugfix
		DDX_Control(pDX, IDC_COMBO_PRINTER, m_PrinterCombo);

	    //DDX_Control(pDX, IDC_EDIT_PRINTER, m_printerEdit);

        //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CNewItem, CDialog)
	    //{{AFX_MSG_MAP(CNewItem)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CNewItem message handlers

    BOOL CNewItem::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        //m_printerEdit.SetLimitText(PRINTER_FIELD_LENGTH);

        m_typeCombo.AddString("ALARM");
        m_typeCombo.AddString("EVENT");
        m_typeCombo.SetCurSel(0);

        std::vector<SubsystemRecord> subsystems = TA_Base_Core::OnlinePrinterAccessFactory::getInstance().getSubsystems();
        for (int i = 0; i < subsystems.size(); ++i)
        {
            m_subsystemCombo.AddString(subsystems[i].name.c_str());
        }

        m_subsystemCombo.SetCurSel(0);
		//TD17780 marc_bugfix
		m_PrinterCombo.SetCurSel(0);

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CNewItem::OnOK() 
    {
        std::vector<SubsystemRecord> subsystems = TA_Base_Core::OnlinePrinterAccessFactory::getInstance().getSubsystems();
        unsigned long subsystem = subsystems[m_subsystemCombo.GetCurSel()].key;
        bool isAlarm = (m_typeCombo.GetCurSel() == 0);
        CString printerString;
		//TD17780 marc_bugfix
		m_PrinterCombo.GetLBText(m_PrinterCombo.GetCurSel(), printerString);
        //m_printerEdit.GetWindowText(printerString);
		std::string printer (printerString);
        
        TA_Base_Core::IConfigOnlinePrinter* newItem = TA_Base_Core::OnlinePrinterAccessFactory::getInstance().createOnlinePrinter();
        if (newItem == NULL)
        {
            MessageBox("Unable to create item!");
            return;
        }

        newItem->setSubsystem(subsystem);
        newItem->setIsAlarm(isAlarm);
        newItem->setPrinter(printer);
        newItem->setLocation(m_location);

        try
        {
            newItem->applyChanges();
        }
        catch (TA_Base_Core::TransactiveException& e)
        {
            MessageBox(e.what());

            delete newItem;
            return;
        }

        // assign member variable, to be used by the caller
        // caller is responsible for deleting this
        m_newItem = newItem;

	    CDialog::OnOK();
    }

    void CNewItem::OnCancel() 
    {
	    // TODO: Add extra cleanup here
	    
	    CDialog::OnCancel();
    }
}
