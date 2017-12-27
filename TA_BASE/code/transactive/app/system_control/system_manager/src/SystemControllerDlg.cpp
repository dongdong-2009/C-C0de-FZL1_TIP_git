/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/system_manager/src/SystemControllerDlg.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This is the dialog used to allow the user to enter the System Controller host name and
  * port number.
  */

#include "StdAfx.h"
#include "SystemControllerDlg.h"
#include "DataCache.h"
#include "SessionHandler.h"
#include "AdminManager.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/data_access_interface/system_controller/src/ISystemControllerData.h"
#include "core/data_access_interface/system_controller/src/SystemControllerAccessFactory.h"
#include "core/data_access_interface/entity_access/src/EntityAccessFactory.h"
#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/exceptions/src/CannotUseSysControllerException.h"
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"
#include "core/utilities/src/TA_MinMax.h"
#include "core/corba/src/CorbaUtil.h"
#include <boost/foreach.hpp>

using TA_Base_Core::RunParams;
using TA_Base_Core::DebugUtil;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

    SystemControllerDlg::SystemControllerDlg(CWnd* pParent /*=NULL*/)
	    : CDialog(SystemControllerDlg::IDD, pParent)
    {
    	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
    }


    void SystemControllerDlg::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(SystemControllerDlg)
		DDX_Control(pDX, IDC_HOST_NAME_EDIT, m_hostnameEdit);
	    DDX_Control(pDX, IDC_PORT_NUMBER, m_portNumber);
	    DDX_Control(pDX, IDC_HOST_NAME, m_hostnameCombo);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(SystemControllerDlg, CDialog)
	//{{AFX_MSG_MAP(SystemControllerDlg)
	ON_BN_CLICKED(IDOK, onOK)
	ON_EN_CHANGE(IDC_PORT_NUMBER, onChangePortNumber)
	ON_CBN_SELCHANGE(IDC_HOST_NAME, OnSelectHostName)
	ON_EN_CHANGE(IDC_HOST_NAME_EDIT, OnChangeHostNameEdit)
	ON_CBN_DROPDOWN(IDC_HOST_NAME, OnDropdownHostName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL SystemControllerDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    // Set the icon for this dialog.  The framework does this automatically
	    //  when the application's main window is not a dialog
	    SetIcon(m_hIcon, TRUE);			// Set big icon
	    SetIcon(m_hIcon, FALSE);		// Set small icon

		m_hostnameEdit.SetLimitText(60); // Hostnames have a maximum length of 60 characters
        m_portNumber.SetLimitText(4); // Port numbers can only be a maximum of 4 characters

		// TD7027 - Try to populate the combobox, if we cannot the
		// use the default edit field instead		
		try
		{
			populateHostnamesCombo();
			
			// Since we have the database connection string 
			// hide the edit field and show the combobox
			m_hostnameEdit.ShowWindow(SW_HIDE);
			m_hostnameCombo.ShowWindow(SW_SHOW);
			m_hostnameEdit.EnableWindow( false);
			m_hostnameCombo.EnableWindow( true);

			// If using the combobox clear the edit field
			m_hostnameEdit.SetWindowText("");
		}
		catch (...)
		{
			// An error occured, default to the edit field
			m_hostnameEdit.ShowWindow(SW_SHOW);
			m_hostnameCombo.ShowWindow(SW_HIDE);
			m_hostnameEdit.EnableWindow( true);
			m_hostnameCombo.EnableWindow( false);

            CString hostname = TA_Base_Core::RunParams::getInstance().get(RPARAM_HOSTNAME.c_str()).c_str();
            if (!hostname.IsEmpty())
            {
                m_hostnameEdit.SetWindowText(hostname);
            }

            LOG_GENERIC(SourceInfo, DebugUtil::DebugDebug, "Could not populate hostname field automatically");
		}

        // We want to get the port number to set in the field if possible
        CString portNumber = TA_Base_Core::RunParams::getInstance().get(RPARAM_PORTNUMBER.c_str()).c_str();
        if (portNumber.IsEmpty())
        {
            // If no port number is currently set then we want to use the default port number
            portNumber.Format("%lu",TA_Base_Core::DEFAULT_MGR_PORT);
        }

	    m_portNumber.SetWindowText(portNumber);
        
        refresh();

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SystemControllerDlg::onOK() 
    {
        FUNCTION_ENTRY("onOK");

        CString hostString("");
        if (m_hostnameEdit.IsWindowVisible())
        {
            m_hostnameEdit.GetWindowText(hostString);
        }
        else if (m_hostnameCombo.IsWindowVisible())
        {
            int index = m_hostnameCombo.GetCurSel();
            if( index != LB_ERR )
            {
                m_hostnameCombo.GetLBText( index, hostString);
            }
        }

        CString portNumberString;
        m_portNumber.GetWindowText( portNumberString );

        // Trim whitespace
        hostString.TrimLeft();
        hostString.TrimRight();
        portNumberString.TrimLeft();
        portNumberString.TrimRight();

        std::string portToText(portNumberString);
        if (-1 != portToText.find_first_not_of("0123456789") )
        {
            CString itemName;
            itemName.LoadString(IDS_PORT_NUMBER);
            showMsgBox(IDS_UE_020053, itemName);
            return;
        }

        TA_Base_Core::RunParams::getInstance().set(RPARAM_HOSTNAME.c_str(), hostString); 
	    TA_Base_Core::RunParams::getInstance().set(RPARAM_PORTNUMBER.c_str(), portNumberString); 

        FUNCTION_EXIT;
        CDialog::OnOK();
    }


    void SystemControllerDlg::onChangePortNumber() 
    {
        refresh();
    }


	void SystemControllerDlg::OnChangeHostNameEdit() 
	{
        refresh();
	}


	void SystemControllerDlg::OnSelectHostName() 
	{
		refresh();
	}


	void SystemControllerDlg::OnDropdownHostName() 
	{
		// Get the number of items in the combo box
		int itemCount = m_hostnameCombo.GetCount();

		// Get the larger value between the number of items or the minimum dropdown
		itemCount = ta_max(itemCount, 2);
		
		// Get the height of the first item (all items will be the same size)
		int height = m_hostnameCombo.GetItemHeight(0);

		// Get the size of the combobox
		CRect rect;
		m_hostnameCombo.GetWindowRect(&rect);
		
		// If maxlen = -1, dropdown list is limited in size to that of the screen.
		// Otherwise there is no limit on the size of the dropdown.
		const int maxlen = -1;

		// If size of drop down is not limited to the screen limit then
		// limit it to the maximum number of items 
		if(maxlen > 0)
		{
			itemCount = ta_max(maxlen, 2);
		}
		
		// Get width and calculate required height of combobox
		CSize size;
		size.cx = rect.Width();
		size.cy = height * (itemCount + 2);
		
		// If dropdown size is limited to screen size then calculate
		// the dropdown size
		if(maxlen < 0)
		{ 
			// screen limit

			if(rect.top - size.cy < 0 || rect.bottom + size.cy > ::GetSystemMetrics(SM_CYSCREEN))
			{
				// invoke limit

				// Compute the largest distance the dropdown can appear, 
				// relative to the screen (not the window!)		
				int factor = ta_max( (rect.top / height), 
					(::GetSystemMetrics(SM_CYSCREEN) - rect.bottom) / height);
				
				// Compute new space. Note that we don't really fill the screen.
				// We only have to use this size if it is smaller than the max size
				// actually required
				int height2 = height * factor;
                size.cy = std::min<int>(height2, size.cy);
			} 
		}
		
		// Set the size of the dropdown
		m_hostnameCombo.SetWindowPos(NULL, 0, 0, size.cx, size.cy, SWP_NOMOVE | SWP_NOZORDER);	
	}


    void SystemControllerDlg::refresh()
    {
        // Check whether all the fields are completed so we can determine whether
        // the ok button should be enabled or disabled.

        // Only one of the edit or combo is used so check the one that is visible.
        if (m_hostnameEdit.IsWindowVisible() || m_hostnameEdit.IsWindowEnabled() )
        {
            CString hostString;
            m_hostnameEdit.GetWindowText(hostString);

            if (hostString.IsEmpty())
            {
                GetDlgItem(IDOK)->EnableWindow(FALSE);
                return;
            }
        }
        else if (m_hostnameCombo.IsWindowVisible() || m_hostnameCombo.IsWindowEnabled() )
        {
            if (m_hostnameCombo.GetCurSel() == LB_ERR)
            {
                GetDlgItem(IDOK)->EnableWindow(FALSE);
                return;
            }
        }
        else
        {
            // Unable to determine hostname
            GetDlgItem(IDOK)->EnableWindow(FALSE);
            return;
        }

        CString portNumberString;
        m_portNumber.GetWindowText( portNumberString );

        if (portNumberString.IsEmpty())
        {
            GetDlgItem(IDOK)->EnableWindow(FALSE);
            return;
        }


        // If we get to here then the Ok button should be enabled as all fields have
        // been completed.
        GetDlgItem(IDOK)->EnableWindow(TRUE);
    }


	void SystemControllerDlg::populateHostnamesCombo()
	{
		FUNCTION_ENTRY("populateHostnamesCombo");

		try
		{
            // Loop through and get the names of system controllers
            // Populate the hostname combobox

            if (m_hostnames.empty())
            {
                m_hostnames = DataCache::getInstance().getAllHostnames();
            }

            BOOST_FOREACH(std::string& hostname, m_hostnames)
            {
                m_hostnameCombo.AddString(hostname.c_str());
            }

            // Select the hostname that was entered on the command line if it is available
            CString hostname = TA_Base_Core::RunParams::getInstance().get(RPARAM_HOSTNAME.c_str()).c_str();
            m_hostnameCombo.SelectString(-1, hostname);
		}
		catch ( const TA_Base_Core::DataException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DataException", "Could not retrieve system controller data.  Leaving hostname field blank");
			TA_THROW( TA_Base_Core::CannotUseSysControllerException("Could not retrieve system controller data") );
 		}
		catch ( const TA_Base_Core::DatabaseException& )
		{
			LOG( SourceInfo, DebugUtil::ExceptionCatch, "DatabaseException", "Could not retrieve system controller data.  Leaving hostname field blank");
			TA_THROW( TA_Base_Core::DatabaseException("Could not retrieve system controller data") );
		}

		FUNCTION_EXIT;
	}

} // namespace TA_Base_App
