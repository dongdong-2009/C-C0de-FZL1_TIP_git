/**
  * The source code in this file is the property of 
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/configuration/config_plugins/scada/src/AddAssociationDialog.cpp $
  * @author:  Karen Graham
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  * 
  * This class controls a dialog that is used to allow the user to add input and output association
  * data points.
  */

#include "app/configuration/config_plugins/scada/src/StdAfx.h"
#include "app/configuration/config_plugins/scada/src/AddAssociationDialog.h"

#include "bus/scada/scada_tree/src/EntityTreeItem.h"

#include "core/data_access_interface/entity_access/src/IEntityData.h"
#include "core/data_access_interface/entity_access/src/DataPointEntityData.h"

#include "core/utilities/src/DebugUtil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace TA_Base_App
{

    AddAssociationDialog::AddAssociationDialog(int numberOfStates, 
                                               TA_Base_Bus::IEntityFactory& entityFactoryForScadaTree,
                                               const CString& dialogTitle /*= "Add Association"*/,
                                               const CString& initialSelection /*= ""*/,
                                               CWnd* pParent /*=NULL*/,
											   bool allSelectionEnabled /*=false*/)
	    : m_dataPointTree(&entityFactoryForScadaTree),
          m_name(""),
          m_numberOfStates(numberOfStates),
          m_selectedState(0),
          m_dialogTitle(dialogTitle),
          m_initialSelection(initialSelection),
		  m_allSelectionEnabled ( allSelectionEnabled ),
          CDialog(AddAssociationDialog::IDD, pParent)
    {
	    //{{AFX_DATA_INIT(AddAssociationDialog)
		    // NOTE: the ClassWizard will add member initialization here
	    //}}AFX_DATA_INIT
    }


    void AddAssociationDialog::DoDataExchange(CDataExchange* pDX)
    {
	    CDialog::DoDataExchange(pDX);
	    //{{AFX_DATA_MAP(AddAssociationDialog)
	    DDX_Control(pDX, IDC_STATE_COMBO, m_stateCombo);
	    DDX_Control(pDX, IDC_DATAPOINT_TREE, m_dataPointTree);
	    //}}AFX_DATA_MAP
    }


BEGIN_MESSAGE_MAP(AddAssociationDialog, CDialog)
	//{{AFX_MSG_MAP(AddAssociationDialog)
	ON_NOTIFY(NM_DBLCLK, IDC_DATAPOINT_TREE, onDblclkDatapointTree)
	ON_BN_CLICKED(IDOK, onOK)
	ON_NOTIFY(TVN_SELCHANGED, IDC_DATAPOINT_TREE, onSelchangedDatapointTree)
	ON_CBN_SELCHANGE(IDC_STATE_COMBO, onSelchangeStateCombo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


    BOOL AddAssociationDialog::OnInitDialog() 
    {
	    CDialog::OnInitDialog();

        SetWindowText(m_dialogTitle);

        for (int i = 0; i < m_numberOfStates; ++i)
        {
            CString state;
            state.Format("%d",i);
            int position = m_stateCombo.AddString(state);
            m_stateCombo.SetItemData(position, i);
        }

        if (m_numberOfStates == 0)
        {
            GetDlgItem(IDC_STATE_COMBO)->EnableWindow(FALSE);
            GetDlgItem(IDC_STATE_STATIC)->EnableWindow(FALSE);
        }

        if(!m_initialSelection.IsEmpty())
        {
            m_dataPointTree.setInitialDataPoint( std::string(m_initialSelection) );
        }

        refresh();	    

	    return TRUE;  // return TRUE unless you set the focus to a control
	                  // EXCEPTION: OCX Property Pages should return FALSE
    }


    void AddAssociationDialog::onSelchangedDatapointTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;
    
        refresh();

	    *pResult = 0;
    }


    void AddAssociationDialog::onSelchangeStateCombo() 
    {
        refresh();
    }


    void AddAssociationDialog::onDblclkDatapointTree(NMHDR* pNMHDR, LRESULT* pResult) 
    {
	    onOK();
	    
	    *pResult = 0;
    }


    void AddAssociationDialog::onOK() 
    {
        FUNCTION_ENTRY("onOK");

        CObjectTreeItem* item = m_dataPointTree.getSelectedItem();
        if (item == NULL)
        {
            // No item is selected so we're not ready to perform an 'Ok'
            FUNCTION_EXIT;
            return;
        }

        TA_Base_Bus::CEntityTreeItem* entityItem = dynamic_cast<TA_Base_Bus::CEntityTreeItem*>(item);
        if (entityItem == NULL)
        {
            // No item is selected so we're not ready to perform an 'Ok'
            FUNCTION_EXIT;
            return;
        }

        if (m_numberOfStates != 0)
        {
            int selected = m_stateCombo.GetCurSel();
            if (selected == LB_ERR)
            {
                // No state has been selected so we're not ready to perform an 'Ok'
                FUNCTION_EXIT;
                return;
            }

            m_selectedState = m_stateCombo.GetItemData(selected);
        }

		if ( false == m_allSelectionEnabled )
		{
			if (entityItem->getEntityData()->getType() != TA_Base_Core::DataPointEntityData::getStaticType())
			{
				// Item selected is not a datapoint so we're not ready to perform an 'Ok'
				FUNCTION_EXIT;
				return;
			}
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
        FUNCTION_EXIT;
    }


    void AddAssociationDialog::refresh()
    {
        FUNCTION_ENTRY("refresh");

        bool shouldEnable = false;
        // If a tree item is selected and state does not need to be selected then everything has been selected
        // OR If a tree item is selected and a state is selected then we may be done
        if( ( (m_dataPointTree.getSelectedItem() != NULL) && ( m_numberOfStates == 0 ) ) ||
            ( (m_dataPointTree.getSelectedItem() != NULL) && ( m_stateCombo.GetCurSel() != LB_ERR) ) )
        {
			if ( true == m_allSelectionEnabled )
			{
				shouldEnable = true;
			}
			else
			{
				// Now check if the selected item is a datapoint.
				using TA_Base_Bus::CEntityTreeItem;

				CEntityTreeItem* item = dynamic_cast<CEntityTreeItem*>(m_dataPointTree.getSelectedItem());
				if (item->getEntityData()->getType() == TA_Base_Core::DataPointEntityData::getStaticType())
				{
					shouldEnable = true;
				}
			}
        }

        GetDlgItem(IDOK)->EnableWindow( shouldEnable );
		GetDlgItem(IDC_STATE_COMBO)->EnableWindow(shouldEnable);//TD18859
        GetDlgItem(IDC_STATE_STATIC)->EnableWindow(shouldEnable);//TD18859
        FUNCTION_EXIT;
    }

}
