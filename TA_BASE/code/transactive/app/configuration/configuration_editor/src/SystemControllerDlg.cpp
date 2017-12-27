/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File$
  * @author:  Karen Graham
  * @version: $Revision$
  *
  * Last modification: $DateTime$
  * Last modified by:  $Author$
  * 
  * This is the dialog used to allow the user to enter the System Controller host name and
  * port number.
  */

#include "app/configuration/configuration_editor/src/stdafx.h"
#include "app/configuration/configuration_editor/src/SystemControllerDlg.h"
#include "app/configuration/configuration_editor/src/ConfigurationEditorConstants.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

#include "core/process_management/IDL/src/ProcessManagementDataDefinitionsCorbaDef.h"

#include "core/utilities/src/RunParams.h"
#include "core/utilities/src/DebugUtil.h"

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
	    DDX_Control(pDX, IDC_PORT_NUMBER, m_portNumber);
	    DDX_Control(pDX, IDC_HOST_NAME, m_hostName);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(SystemControllerDlg, CDialog)
	//{{AFX_MSG_MAP(SystemControllerDlg)
	ON_BN_CLICKED(IDOK, onOK)
	ON_EN_CHANGE(IDC_HOST_NAME, onChangeHostName)
	ON_EN_CHANGE(IDC_PORT_NUMBER, onChangePortNumber)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL SystemControllerDlg::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    // Set the icon for this dialog.  The framework does this automatically
	    //  when the application's main window is not a dialog
	    SetIcon(m_hIcon, TRUE);			// Set big icon
	    SetIcon(m_hIcon, FALSE);		// Set small icon

        using namespace TA_Configuration;

        // We want to get the port number to set in the field if possible
        CString portNumber = TA_Base_Core::RunParams::getInstance().get(RPARAM_PORTNUMBER.c_str()).c_str();
        if (portNumber.IsEmpty())
        {
            // If no port number is currently set then we want to use the default port number
            portNumber.Format("%lu",TA_Base_Core::DEFAULT_MGR_PORT);
        }

        m_portNumber.SetLimitText(5); // Port numbers can only be a maximum of 5 characters
	    m_portNumber.SetWindowText(portNumber);

        refresh();

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SystemControllerDlg::onOK() 
    {
        FUNCTION_ENTRY("onOK");

        using namespace TA_Base_App::TA_Configuration;

        CString hostName;
        m_hostName.GetWindowText(hostName);

        CString portNumber;
        m_portNumber.GetWindowText(portNumber);

        std::string portToText(portNumber);
		if (std::string::npos != std::string(portNumber).find_first_not_of("0123456789") )
        {
            CString portNumber;
            portNumber.LoadString(IDS_PORT_NUMBER);
            UINT selectedButton = showMsgBox(IDS_UE_030062, portNumber);
            return;
        }

        TA_Base_Core::RunParams::getInstance().set(RPARAM_HOSTNAME.c_str(), hostName); 
	    TA_Base_Core::RunParams::getInstance().set(RPARAM_PORTNUMBER.c_str(), portNumber); 

        FUNCTION_EXIT;
        CDialog::OnOK();
    }


    void SystemControllerDlg::onChangeHostName() 
    {
        refresh();
    }


    void SystemControllerDlg::onChangePortNumber() 
    {
        refresh();
    }


    void SystemControllerDlg::refresh()
    {
        CString hostString;
        m_hostName.GetWindowText( hostString );

        CString portNumberString;
        m_portNumber.GetWindowText( portNumberString );

        // If one of the fields is empty then the Ok button should be disabled
        if( hostString.IsEmpty() || portNumberString.IsEmpty() )
        {
            GetDlgItem(IDOK)->EnableWindow(FALSE);
        }
        else
        {
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        }
    }
}
