/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/online_printer/src/NewLocation.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// newlocation.cpp : implementation file
//


#include "app/configuration/config_plugins/online_printer/src/StdAfx.h"
#include "app/configuration/config_plugins/online_printer/src/NewLocation.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CNewLocation dialog

//using TA_Base_Core::OnlinePrintCfgAccessFactory::LocationRecord;
typedef TA_Base_Core::OnlinePrintCfgAccessFactory::LocationRecord LocationRecord;

namespace TA_Base_App
{
    CNewLocation::CNewLocation(CWnd* pParent /*=NULL*/)
	    : CDialog(CNewLocation::IDD, pParent), m_newItem(NULL)
    {
	    //{{AFX_DATA_INIT(CNewLocation)
	    //}}AFX_DATA_INIT
    }


    void CNewLocation::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(CNewLocation)
	    DDX_Control(pDX, IDC_LOCATION, m_locationCombo);
	    //}}AFX_DATA_MAP
    }


    BEGIN_MESSAGE_MAP(CNewLocation, CDialog)
	    //{{AFX_MSG_MAP(CNewLocation)
	    //}}AFX_MSG_MAP
    END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // CNewLocation message handlers

    BOOL CNewLocation::canCreateNew()
    {
        m_locations = TA_Base_Core::OnlinePrintCfgAccessFactory::getInstance().getUnusedLocations();
        return !m_locations.empty();
    }

    BOOL CNewLocation::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    
        for (unsigned int i = 0; i < m_locations.size(); ++i)
        {
            m_locationCombo.AddString(m_locations[i].name.c_str());
        }

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }

    void CNewLocation::OnOK() 
    {
        TA_Base_Core::IConfigOnlinePrintCfg* newItem = TA_Base_Core::OnlinePrintCfgAccessFactory::getInstance().createOnlinePrintCfg();
        if (newItem == NULL)
        {
            MessageBox("Unable to create item!");
            return;
        }

        int index = m_locationCombo.GetCurSel();

        if (index < 0)
        {
            MessageBox("No location is selected!");
            return;
        }

        newItem->setLocation(m_locations[index].key);
        newItem->setLocationName(m_locations[index].name);

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

        m_newItem = newItem;
	    
	    CDialog::OnOK();
    }
}

