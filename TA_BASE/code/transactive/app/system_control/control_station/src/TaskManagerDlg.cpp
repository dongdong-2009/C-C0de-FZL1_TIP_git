/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/system_control/control_station/src/TaskManagerDlg.cpp $
  * @author:  Ripple
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  */
// TaskManagerDlg.cpp : implementation file
//

#include "app\system_control\control_station\src\stdafx.h"
#include "app\system_control\control_station\src\IProcessManager.h"
#include "app\system_control\control_station\src\TaskManagerDlg.h"
#include "app\system_control\control_station\src\RunningApplication.h"
#include "app\system_control\control_station\src\SessionManager.h"
#include "core\utilities\src\RunParams.h"
#include "core\process_management\IDL\src\ProcessManagementDataDefinitionsCorbaDef.h"

using TA_Base_Core::RunParams;

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CTaskManagerDlg dialog


CTaskManagerDlg::CTaskManagerDlg(CWnd* pParent /*=NULL*/)
	:	m_apps(NULL),
        m_isEditable(true),
		CDialog(CTaskManagerDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CTaskManagerDlg)
	//}}AFX_DATA_INIT
    IProcessManager* pm = SessionManager::getInstance().getProcessManager();
    if (pm != NULL)
    {
        m_apps = static_cast<IProcessManager::RunningApplicationMap*>(&(pm->getRunningApplicationObjects()));
    }
}


void CTaskManagerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CTaskManagerDlg)
	DDX_Control(pDX, IDC_BUTTON_ADD_PARAM, m_addParamButton);
	DDX_Control(pDX, IDC_EDIT_ADD_PARAM, m_addParamEdit);
	DDX_Control(pDX, IDC_BUTTON_TERMINATE, m_terminateButton);
	DDX_Control(pDX, IDC_LIST_PARAMETERS, m_parametersList);
	DDX_Control(pDX, IDC_COMBO_PROCESS, m_processCombo);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CTaskManagerDlg, CDialog)
	//{{AFX_MSG_MAP(CTaskManagerDlg)
	ON_CBN_SELCHANGE(IDC_COMBO_PROCESS, OnSelchangeComboProcess)
	ON_NOTIFY(LVN_ENDLABELEDIT, IDC_LIST_PARAMETERS, OnEndlabeleditListParameters)
	ON_BN_CLICKED(IDC_BUTTON_TERMINATE, OnButtonTerminate)
	ON_WM_SHOWWINDOW()
	ON_BN_CLICKED(IDC_BUTTON_ADD_PARAM, OnButtonAddParam)
	ON_EN_UPDATE(IDC_EDIT_ADD_PARAM, OnUpdateEditAddParam)
	ON_WM_CLOSE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CTaskManagerDlg message handlers

void CTaskManagerDlg::OnSelchangeComboProcess() 
{
	m_parametersList.DeleteAllItems();

	int index = m_processCombo.GetCurSel();

	if(index == CB_ERR)
	{
		m_parametersList.EnableWindow(false);
		m_addParamButton.EnableWindow(false);
		m_addParamEdit.EnableWindow(false);
		m_addParamEdit.Clear();
		m_terminateButton.EnableWindow(false);
	}
	else
	{
		if (m_isEditable)
		{
			m_parametersList.EnableWindow();
			m_addParamEdit.EnableWindow();
			m_terminateButton.EnableWindow();
		}
		m_parametersList.SetRedraw(false);

        int selectedItemData = m_processCombo.GetItemData(index);
		if( selectedItemData == -1)
		{
			m_terminateButton.EnableWindow(false);
			RunParams::ParamVector tmpParamVector;

			RunParams::getInstance().getAll(tmpParamVector);

			for (unsigned long i = 0; i < tmpParamVector.size(); ++i)
			{
				int listIndex = m_parametersList.InsertItem(i, tmpParamVector[i].value.c_str());
				m_parametersList.SetItemText(listIndex, 0, tmpParamVector[i].value.c_str());
				m_parametersList.SetItemText(listIndex, 1, tmpParamVector[i].name.c_str());
			}
		}	
		else
		{
            if (m_apps != NULL)
            {
                ProcessManager::RunningApplicationMap::iterator iter = m_apps->find(selectedItemData);
                if (iter == m_apps->end())
                {
                    return;
                }

                
                TA_Base_Core::RunParamSeq* tmpParams = iter->second->getParameters();
                if ( NULL != tmpParams )
                {
				    for (unsigned long i = 0; i < tmpParams->length(); ++i)
				    {
					    int listIndex = m_parametersList.InsertItem(i, (*tmpParams)[i].value);
					    m_parametersList.SetItemText(listIndex, 0, (*tmpParams)[i].value);
					    m_parametersList.SetItemText(listIndex, 1, (*tmpParams)[i].name);
				    }
                }
            }
		}

		m_parametersList.SetRedraw(true);
		m_parametersList.Invalidate();
		m_parametersList.UpdateWindow();
	}	
}


void CTaskManagerDlg::OnEndlabeleditListParameters(NMHDR* pNMHDR, LRESULT* pResult) 
{
	if (!m_isEditable)
	{
		return;
	}

	LV_DISPINFO* pDispInfo = (LV_DISPINFO*)pNMHDR;
	if (pDispInfo->item.pszText != NULL)
	{
		CString paramName = m_parametersList.GetItemText(pDispInfo->item.iItem, 1);
		CString paramValue(pDispInfo->item.pszText);

		int index = m_processCombo.GetCurSel();
        int selectedItemData = m_processCombo.GetItemData(index);

		if(selectedItemData == -1)
		{
			RunParams::getInstance().set(paramName, paramValue);
		}
		else
		{
            TA_Base_Core::RunParamSeq* tmpParams = new TA_Base_Core::RunParamSeq;
            if (tmpParams == NULL)
            {
                *pResult = 0;
                return;
            }

            tmpParams->length(1);
			(*tmpParams)[0].name = CORBA::string_dup(paramName);
			(*tmpParams)[0].value = CORBA::string_dup(paramValue);

            if (m_apps != NULL)
            {
                ProcessManager::RunningApplicationMap::iterator iter = m_apps->find( selectedItemData );
                if (iter != m_apps->end() )
                {
                    iter->second->passNewParameters(tmpParams);
                }
            }
            //delete tmpParams;
		}
		*pResult = true;
	}
	else
	{
		*pResult = 0;
	}
}


void CTaskManagerDlg::OnButtonTerminate() 
{
	int index = m_processCombo.GetCurSel();
    int selectedItemData = m_processCombo.GetItemData(index);

    if (selectedItemData == -1)
    {
        return;
    }
    if (m_apps == NULL)
    {
        return;
    }

    ProcessManager::RunningApplicationMap::iterator iter = m_apps->find( selectedItemData );
    if (iter == m_apps->end())
    {
        return;
    }

    iter->second->terminate();

    Sleep(1000);

    loadProcessCombo();
    m_processCombo.SetCurSel(0);
    OnSelchangeComboProcess();
    m_addParamEdit.Clear();
}


BOOL CTaskManagerDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	m_parametersList.SetExtendedStyle(LVS_EX_FULLROWSELECT);
	m_parametersList.InsertColumn(0, "Value", LVCFMT_LEFT, 235, 0);
	m_parametersList.InsertColumn(1, "Parameter", LVCFMT_LEFT, 90, 1);
	LPINT	columnOrder;
	columnOrder = (LPINT) malloc(2 * sizeof(int));
	columnOrder[0] = 1;
	columnOrder[1] = 0;

	m_parametersList.SetColumnOrderArray(2,columnOrder);

	m_addParamEdit.Clear();

	// If it is not editable then disable the Add Parameter stuff
	if (!m_isEditable)
	{
		m_addParamEdit.EnableWindow(false);
		m_addParamButton.EnableWindow(false);
		m_terminateButton.EnableWindow(false);
	}
	else
	{
		m_addParamEdit.EnableWindow(true);
		m_terminateButton.EnableWindow(true);
	}


	return TRUE;  
}


void CTaskManagerDlg::OnShowWindow(BOOL bShow, UINT nStatus) 
{
	CDialog::OnShowWindow(bShow, nStatus);
	
	if(bShow)
	{
        loadProcessCombo();
		m_processCombo.SetCurSel(0);
		m_addParamEdit.Clear();
		if (!m_isEditable)
		{
			m_addParamEdit.EnableWindow(false);
			m_addParamButton.EnableWindow(false);
			m_terminateButton.EnableWindow(false);
		}
		else
		{
			m_addParamEdit.EnableWindow(true);
			m_terminateButton.EnableWindow(true);

		}

		OnSelchangeComboProcess();

        SetWindowPos(&wndTopMost, 0,0,0,0,SWP_NOMOVE | SWP_NOSIZE);
	}
}


void CTaskManagerDlg::OnButtonAddParam() 
{
	CString paramName;
	m_addParamEdit.GetWindowText(paramName);

	int listIndex;
	listIndex = m_parametersList.InsertItem(m_parametersList.GetItemCount(), "");
	m_parametersList.SetItemText(listIndex, 0, "");
	m_parametersList.SetItemText(listIndex, 1, paramName);

}


void CTaskManagerDlg::OnUpdateEditAddParam() 
{
	if (m_isEditable)
	{
		CString tmpBuff;
		m_addParamEdit.GetWindowText(tmpBuff);
		m_addParamButton.EnableWindow(!tmpBuff.IsEmpty());
	}
}


void CTaskManagerDlg::loadProcessCombo()
{
    m_processCombo.Clear();
    m_processCombo.ResetContent();

	int index = m_processCombo.AddString("<Default Parameters>");
	m_processCombo.SetItemData(index, -1);

    if (m_apps == NULL)
    {
        return;
    }

    ProcessManager::RunningApplicationMap::iterator iter = m_apps->begin();
    while (iter != m_apps->end())
    {   
        index = m_processCombo.AddString( iter->second->getApplicationName().c_str() );
        m_processCombo.SetItemData(index, iter->first);
        ++iter;
    }
}


void CTaskManagerDlg::OnClose() 
{
    ShowWindow(SW_HIDE);
}
