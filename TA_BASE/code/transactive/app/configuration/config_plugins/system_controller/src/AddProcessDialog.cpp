/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/system_controller/src/AddProcessDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This dialog is used to add one or more processes to a System Controller.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/system_controller/src/stdafx.h"
#include "app/configuration/config_plugins/system_controller/src/AddProcessDialog.h"
#include "app/configuration/config_plugins/system_controller/src/SystemControllerDatabaseAccessor.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
    AddProcessDialog::AddProcessDialog(TA_Base_Core::IConfigSystemController& currentSystemController)
	    : m_currentSystemController(currentSystemController),
          CDialog(AddProcessDialog::IDD, NULL)
    {
	    //{{AFX_DATA_INIT(AddProcessDialog)
	//}}AFX_DATA_INIT
    }


    void AddProcessDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AddProcessDialog)
	DDX_Control(pDX, IDC_PROCESS_LIST, m_processList);
	DDX_Control(pDX, IDC_RADIO_CONTROL, m_control);
	DDX_Control(pDX, IDC_RADIO_MONITOR, m_monitor);
	//}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AddProcessDialog, CDialog)
	//{{AFX_MSG_MAP(AddProcessDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_NOTIFY(NM_DBLCLK, IDC_PROCESS_LIST, onDblclkProcessList)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_PROCESS_LIST, onItemchangedProcessList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // AddProcessDialog message handlers

    BOOL AddProcessDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        CRect listRect;
        m_processList.GetWindowRect( listRect );
        m_processList.InsertColumn(0, "",LVCFMT_LEFT, listRect.Width() - 10);

        m_processList.SetExtendedStyle(LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);

        // Now get all managed processes and add them to the list
        SystemControllerDatabaseAccessor dbAccessor;
        std::map<unsigned long, std::string> managedProcesses = dbAccessor.getAllManagedProcesses();

        TA_Base_Core::IConfigSystemController::ProcessMap alreadyUsedProcesses = m_currentSystemController.getProcesses();

        for (std::map<unsigned long, std::string>::iterator iter = managedProcesses.begin();
             iter != managedProcesses.end(); ++iter)
        {
            TA_Base_Core::IConfigSystemController::ProcessMap::iterator matching = alreadyUsedProcesses.find( iter->first );
            if (matching == alreadyUsedProcesses.end() )
            {
                // We can add this to the list as it is not already used
                int position = m_processList.InsertItem( 0, iter->second.c_str() );
                m_processList.SetItemData( position, iter->first );
            }
        }

        // Set Control mode to be the default
        m_control.SetCheck(1);
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void AddProcessDialog::onItemchangedProcessList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

        POSITION pos = m_processList.GetFirstSelectedItemPosition();
        if (pos == NULL)
        {
            GetDlgItem(IDOK)->EnableWindow(FALSE);
        }
        else
        {
            GetDlgItem(IDOK)->EnableWindow(TRUE);
        }
	    
	    *pResult = 0;
    }

    
    void AddProcessDialog::onDblclkProcessList(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    onOK();
	    
	    *pResult = 0;
    }


    void AddProcessDialog::onOK() 
    {
        using TA_Base_Core::IConfigSystemController;

        Process process;
        if (m_control.GetCheck() == 1)
        {
            process.operatingMode = IConfigSystemController::CONTROL;
        }
        else
        {
            process.operatingMode = IConfigSystemController::MONITOR;
        }

        int numSelected = m_processList.GetSelectedCount();
        if (numSelected <= 0)
        {
            // Nothing to modify so let's get out of here
            CDialog::OnOK();
        }

        int item = -1; // This will hold the last selected item

        // Get all of the selected items.
        for (int i=0; i < numSelected; i++)
        {
              item = m_processList.GetNextItem(item, LVNI_SELECTED);

              process.key = m_processList.GetItemData(item);
              process.name = m_processList.GetItemText(item, 0);

              m_processes.push_back(process);
        }

        CDialog::OnOK();	    
    }
}
