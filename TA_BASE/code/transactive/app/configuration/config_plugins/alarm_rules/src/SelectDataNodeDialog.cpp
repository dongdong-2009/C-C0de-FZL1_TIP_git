/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/alarm_rules/src/SelectDataNodeDialog.cpp $
  * @author:  Dirk McCormick
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class controls a dialog that is used to allow the user to select
  * a datapoint or data node.
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/alarm_rules/src/StdAfx.h"
#include "app/configuration/config_plugins/alarm_rules/src/SelectDataNodeDialog.h"
#include "core/data_access_interface/alarm_rule/src/AlarmRuleAccessFactory.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

#include "core/utilities/src/TAAssert.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    SelectDataNodeDialog::SelectDataNodeDialog(
        AlarmRulesDatabaseAccessor* databaseAccessor,
        CWnd* pParent /*=NULL*/)
	    :
        m_databaseAccessor(databaseAccessor),
        m_name(""),
        CDialog(SelectDataNodeDialog::IDD, pParent)
    {
        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor must not be NULL");
	    //{{AFX_DATA_INIT(SelectDataNodeDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void SelectDataNodeDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(SelectDataNodeDialog)
	    DDX_Control(pDX, IDC_TREE, m_scadaTree);
        DDX_Control(pDX, IDC_COMBO_DN_ALARM_TYPE, m_alarmTypeCombo);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(SelectDataNodeDialog, CDialog)
	//{{AFX_MSG_MAP(SelectDataNodeDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_NOTIFY(NM_DBLCLK, IDC_TREE, onDblclkTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_TREE, onSelchangedTree)
    ON_CBN_SELCHANGE(IDC_COMBO_DN_ALARM_TYPE, onSelectionChangeComboAlarmType)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // SelectDataNodeDialog message handlers


    BOOL SelectDataNodeDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
        GetDlgItem(IDOK)->EnableWindow( m_scadaTree.getSelectedItem() != NULL );

        TA_ASSERT(m_databaseAccessor != NULL, "Database accessor must not be NULL");
        
        m_alarmTypeKeyDispNameMap = m_databaseAccessor->getAllAlarmTypeDisplays();

        for(std::map<unsigned long, std::string>::iterator it =
                m_alarmTypeKeyDispNameMap.begin();
            it != m_alarmTypeKeyDispNameMap.end();
            it ++)
        {
            m_alarmTypeCombo.AddString((*it).second.c_str());
        }

        // Initialise to the first item in the list
        m_alarmTypeCombo.SetFocus();
        m_alarmTypeCombo.SetCurSel(0);

        CString alarmTypeString;
        m_alarmTypeCombo.GetLBText(0, alarmTypeString);
        m_alarmType = alarmTypeString;

        return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void SelectDataNodeDialog::onSelectionChangeComboAlarmType()
    {
        CString alarmTypeString;
        m_alarmTypeCombo.GetLBText(m_alarmTypeCombo.GetCurSel(), alarmTypeString);
        m_alarmType = alarmTypeString;
    }

    void SelectDataNodeDialog::onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        GetDlgItem(IDOK)->EnableWindow( m_scadaTree.getSelectedItem() != NULL );
	    
	    *pResult = 0;
    }


    unsigned long SelectDataNodeDialog::getAlarmTypeKey()
    {
        for(std::map<unsigned long, std::string>::iterator it =
                m_alarmTypeKeyDispNameMap.begin();
            it != m_alarmTypeKeyDispNameMap.end();
            it ++)
        {
            if((*it).second == m_alarmType)
            {
                return (*it).first;
            }
        }

        TA_ASSERT(false, "There should be a key for every alarm type name");
        return 0;
    }

    void SelectDataNodeDialog::onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    onOK();
	    
	    *pResult = 0;
    }


    void SelectDataNodeDialog::onOK() 
    {
        CObjectTreeItem* item = m_scadaTree.getSelectedItem();
        if (item == NULL)
        {
            return;
        }

        TA_Base_Bus::CEntityTreeItem* entityItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(item);
        if (entityItem == NULL)
        {
            return;
        }

        m_key = entityItem->getEntityData()->getKey();
        m_name = entityItem->getEntityData()->getName().c_str();

        CDialog::OnOK();
    }
}
