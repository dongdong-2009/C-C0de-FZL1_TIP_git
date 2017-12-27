/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.cpp $
  * @author:  Alan Brims
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This class controls a dialog that is used to allow the user to select an entity
  * from a SCADA tree.
  * 
  */

#pragma warning(disable:4786)

#include "app/configuration/config_plugins/config_plugin_helper/src/StdAfx.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/helperresource.h"
#include "app/configuration/config_plugins/config_plugin_helper/src/ChangeEntityDialog.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "bus/scada/scada_tree/src/EntityTreeItem.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{
    ChangeEntityDialog::ChangeEntityDialog(
        const CString& currentSelection,
        CWnd* pParent,
        TA_Base_Bus::IEntityFactory* entityRetrievalFactory,
        bool shouldDeleteFactory)

        : m_entityTree( entityRetrievalFactory, shouldDeleteFactory),
          m_name(currentSelection),
          CDialog(ChangeEntityDialog::IDD, pParent)
    {
        //{{AFX_DATA_INIT(ChangeEntityDialog)
        // NOTE: the ClassWizard will add member initialization here
        //}}AFX_DATA_INIT
    }
    
    
    void ChangeEntityDialog::DoDataExchange(CDataExchange* pDX)
    {
        CDialog::DoDataExchange(pDX);
        //{{AFX_DATA_MAP(ChangeEntityDialog)
        DDX_Control(pDX, IDC_ENTITY_TREE, m_entityTree);
        //}}AFX_DATA_MAP
    }
    
    
    BEGIN_MESSAGE_MAP(ChangeEntityDialog, CDialog)
        //{{AFX_MSG_MAP(ChangeEntityDialog)
        ON_NOTIFY(TVN_SELCHANGED, IDC_ENTITY_TREE, onSelchangedTree)
        ON_NOTIFY(NM_DBLCLK, IDC_ENTITY_TREE, onDblclkTree)
        ON_BN_CLICKED(IDOK, onOK)
        //}}AFX_MSG_MAP
    END_MESSAGE_MAP()
        
    /////////////////////////////////////////////////////////////////////////////
    // ChangeEntityDialog message handlers
    
    BOOL ChangeEntityDialog::OnInitDialog() 
    {
        CDialog::OnInitDialog();
        
        if (!m_name.IsEmpty())
        {
            m_entityTree.setInitialDataPoint( std::string(m_name) );
        }
        
        GetDlgItem(IDOK)->EnableWindow( m_entityTree.getSelectedItem() != NULL );
        
        return TRUE;  // return TRUE unless you set the focus to a control
        // EXCEPTION: OCX Property Pages should return FALSE
    }
    
    void ChangeEntityDialog::onSelchangedTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
        
        GetDlgItem(IDOK)->EnableWindow( m_entityTree.getSelectedItem() != NULL );
        
        *pResult = 0;
    }
    
    void ChangeEntityDialog::onDblclkTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
        onOK();
        
        *pResult = 0;
    }
    
    void ChangeEntityDialog::onOK() 
    {
        CObjectTreeItem* item = m_entityTree.getSelectedItem();
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
