/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/ChangeParentDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to change the parent of a datapoint or
  * data node.
  */

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/ChangeParentDialog.h"
#include "app/configuration/config_plugins/scada/src/ChangeParentEntityFactory.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    ChangeParentDialog::ChangeParentDialog(const CString& currentSelection, CWnd* pParent /*=NULL*/)
	    : m_scadaTree(new ChangeParentEntityFactory),
          m_name(currentSelection),
          CDialog(ChangeParentDialog::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(ChangeParentDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void ChangeParentDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(ChangeParentDialog)
	    DDX_Control(pDX, IDC_PARENT_TREE, m_scadaTree);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(ChangeParentDialog, CDialog)
	//{{AFX_MSG_MAP(ChangeParentDialog)
	ON_BN_CLICKED(IDOK, onOK)
	ON_NOTIFY(NM_DBLCLK, IDC_PARENT_TREE, onDblclkTree)
	ON_NOTIFY(TVN_SELCHANGED, IDC_PARENT_TREE, onSelchangedTree)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

    /////////////////////////////////////////////////////////////////////////////
    // ChangeParentDialog message handlers


    BOOL ChangeParentDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();
	    
	    if (!m_name.IsEmpty())
        {
            m_scadaTree.setInitialDataPoint( std::string(m_name) );
        }

        GetDlgItem(IDOK)->EnableWindow( m_scadaTree.getSelectedItem() != NULL );
	    
	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void ChangeParentDialog::onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

        GetDlgItem(IDOK)->EnableWindow( m_scadaTree.getSelectedItem() != NULL );
	    
	    *pResult = 0;
    }


    void ChangeParentDialog::onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    onOK();
	    
	    *pResult = 0;
    }


    void ChangeParentDialog::onOK() 
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

        try
        {
            m_key = entityItem->getEntityData()->getKey();
            m_name = entityItem->getEntityData()->getName().c_str();
        }
        catch ( ... )
        {
            m_name = "";
        }

        CDialog::OnOK();
    }
}
