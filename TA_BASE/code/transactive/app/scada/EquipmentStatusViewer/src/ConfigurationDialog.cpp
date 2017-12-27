/**
 * The source code in this file is the property of 
 * Ripple Systems and is not for redistribution
 * in any form.
 *
 * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/scada/EquipmentStatusViewer/src/ConfigurationDialog.cpp $
 * @author:  Rod Rolirad
 * @version: $Revision: #1 $
 *
 * Last modification: $DateTime: 2012/06/12 13:35:44 $
 * Last modified by:  $Author: builder $
 * 
 * Class is a Configuration Dialog box allowing operators to configure current 
 * equipment reports with equipment/datapoint information from a particular location
 *
 */

#ifdef __WIN32__
#pragma warning(disable:4786)
#pragma warning(disable:4503)  // warning C4503: '  ' : decorated name length exceeded, name was truncated
#endif

#include "stdafx.h"
#include "equipmentstatusviewer.h"
#include "EquipmentStatusListCtrl.h"
#include "ConfigurationDialog.h"
#include "EquipmentStatusViewerDlg.h"
#include "EquipmentStatusListItem.h"
#include "DataNodeListItem.h"
#include "DataPointListItem.h"
#include "AuditMessageUtility.h"

#include "bus/generic_gui/src/TransActiveMessage.h"

// NOTE : Scada Common Factory
#include "bus/scada/proxy_library/src/ScadaProxyFactory.h"

// NOTE : Location related
#include "core/data_access_interface/src/Location.h"
#include "core/data_access_interface/src/LocationAccessFactory.h"

// NOTE : EquipReport and EquipReportData related
#include "core/data_access_interface/src/EquipReport.h"
#include "core/data_access_interface/src/EquipReportData.h"
#include "core/data_access_interface/src/EquipReportAccessFactory.h"
#include "core/data_access_interface/src/EquipReportDataAccessFactory.h"
#include "core/data_access_interface/src/ConfigEquipReport.h"
#include "core/data_access_interface/src/ConfigEquipReportData.h"

// NOTE : Entity related
#include "core\data_access_interface\entity_access\src\EntityAccessFactory.h"
#include "core\data_access_interface\entity_access\src\IEntityData.h"
#include "core\data_access_interface\entity_access\src\DataNodeEntityData.h"
#include "core\data_access_interface\entity_access\src\DataPointEntityData.h"

// NOTE : Exception related
#include "core/exceptions/src/DataException.h"
#include "core/exceptions/src/DatabaseException.h"
#include "core/exceptions/src/ScadaProxyException.h"

// NOTE : Debug related
#include "core/utilities/src/DebugUtil.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif


namespace TA_Base_App
{
	CConfigurationDialog::CConfigurationDialog(
        const unsigned long locationId, std::string& reportId,
        const std::string& locationName, const std::string& reportName,
        CProgressBar& progressBar, CWnd* pParentWnd /*=NULL*/)
		:
        CDialog(CConfigurationDialog::IDD, pParentWnd),
        m_entityFactory( locationId),
        m_equipmentTree( &m_entityFactory),
		m_ReportId( reportId),
		m_LocationId( locationId),
		m_LocationName( locationName),
		m_ReportName( reportName),
        m_progressBar( progressBar),
        m_populatorTimer( 0),
        m_equipmentListCtrl( m_reportItems)
	{
		m_ToRemoveCollection.clear();
		m_addedRemovedItems.clear();

		//{{AFX_DATA_INIT(CConfigurationDialog)
		//}}AFX_DATA_INIT
	}

	CConfigurationDialog::CConfigurationDialog(
        const unsigned long locationId,const std::string& locationName, 
        std::string& reportId, const std::string& reportName,
        CProgressBar& progressBar, CWnd* pParentWnd /*=NULL*/, TA_Base_Bus::IEntityFactory* entityRetrievalFactory)
		:
        CDialog(CConfigurationDialog::IDD, pParentWnd),
		m_LocationId( locationId),
        m_entityFactory( locationId),
        m_equipmentTree( entityRetrievalFactory),
		m_ReportId( reportId),
		m_LocationName( locationName),
		m_ReportName( reportName),
        m_progressBar( progressBar),
        m_populatorTimer( 0),
        m_equipmentListCtrl( m_reportItems)
	{
		m_ToRemoveCollection.clear();
		m_addedRemovedItems.clear();
		//{{AFX_DATA_INIT(CConfigurationDialog)
		//}}AFX_DATA_INIT
	}

	void CConfigurationDialog::DoDataExchange(CDataExchange* pDX)
	{
		CDialog::DoDataExchange(pDX);
		//{{AFX_DATA_MAP(CConfigurationDialog)
		DDX_Control(pDX, IDC_ADD_DATAPOINTS, m_AddDataPtsBtn);
		DDX_Control(pDX, IDC_ADD_DATANODES, m_AddDataNodeBtn);
		DDX_Control(pDX, IDC_REMOVE, m_RemoveBtn);
		DDX_Control(pDX, IDC_EQUIPMENT_DESCRIPTION, m_equipmentDescription);
		DDX_Control(pDX, IDC_EQUIPMENT_LIST, m_equipmentListCtrl);
		DDX_Control(pDX, IDC_EQUIPMENT_TREE, m_equipmentTree);
		//}}AFX_DATA_MAP
	}


	BEGIN_MESSAGE_MAP(CConfigurationDialog, CDialog)
		//{{AFX_MSG_MAP(CConfigurationDialog)
	    ON_WM_CLOSE()
		ON_BN_CLICKED(IDC_REMOVE, OnRemoveBtnClick)
		ON_NOTIFY(NM_CLICK, IDC_EQUIPMENT_LIST, OnClickEquipmentList)
		ON_NOTIFY(NM_SETFOCUS, IDC_EQUIPMENT_LIST, OnSetfocusEquipmentList)
	    ON_NOTIFY(LVN_COLUMNCLICK, IDC_EQUIPMENT_LIST, OnColumnclickEquipmentList)
		ON_BN_CLICKED(IDC_ADD_DATANODES, OnAddDatanodesClick)
		ON_BN_CLICKED(IDC_ADD_DATAPOINTS, OnAddDatapointsClick)
		ON_NOTIFY(NM_SETFOCUS, IDC_EQUIPMENT_TREE, OnSetfocusEquipmentTree)
		ON_NOTIFY(NM_CLICK, IDC_EQUIPMENT_TREE, OnClickEquipmentTreeClick)
	    ON_NOTIFY(TVN_ITEMEXPANDING, IDC_EQUIPMENT_TREE, OnItemExpandingEquipmentTree)
	    ON_NOTIFY(TVN_SELCHANGED, IDC_EQUIPMENT_TREE, OnSelchangedEquipmentTree)
	    ON_WM_SHOWWINDOW()
	    ON_WM_TIMER()
		ON_MESSAGE(WM_LIST_ITEM_UPDATE, OnListItemUpdate)
	    //}}AFX_MSG_MAP
	END_MESSAGE_MAP()

	/////////////////////////////////////////////////////////////////////////////
	// CConfigurationDialog message handlers

	void CConfigurationDialog::ExamineMessageQueue()
	{
        TA_ASSERT(false, "NOT IN USE");
		MSG msg;

		while (PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if (!(AfxGetApp()->PumpMessage()))
			{
				::PostQuitMessage(0);
				break;
			}
		}
	}

	BOOL CConfigurationDialog::OnInitDialog()
	{
		CDialog::OnInitDialog();

		// initialise the group box control
		CWnd *ConfigGroupBox = GetDlgItem(IDC_REPORT_CONFIGURATION_GROUP_BOX);

		// check if the report and location names are empty
		if (m_ReportName.empty() || m_LocationName.empty())
			ConfigGroupBox->SetWindowText("");
		else
		{
			// display the names in the control
			CString ConfigText;

			ConfigText.Format("Report:  %s at %s", m_ReportName.c_str(), m_LocationName.c_str());
			ConfigGroupBox->SetWindowText(ConfigText.GetBuffer(ConfigText.GetLength()));
		}

		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/08/26 11:30:00

        //PopulateEquipmentStatusList(m_ReportId);

		//Launch Manager///////////////////////////////////////////////////////////

		SetStatusBarText("Ready");

		return TRUE;
	}


	void CConfigurationDialog::OnRemoveBtnClick()
	{
        CWaitCursor waitCursor;
		// get the highlighted position of the item
		POSITION pos = m_equipmentListCtrl.GetFirstSelectedItemPosition();
		std::vector<int> indices;

		if (pos == NULL)
			return;

		while (pos)
		{
			// get the selected index of the item to remove
			int selectedIndex = m_equipmentListCtrl.GetNextSelectedItem(pos);
			indices.push_back(selectedIndex);
            unsigned long itemEntityKey = m_equipmentListCtrl.GetItemData(selectedIndex);

			// find if the item is in our list
			EntityKeySet::iterator itr = m_addedRemovedItems.find ( itemEntityKey );

			// if the item is already in our list, this means the item has been added previously 
			// and the user now wants to remove the item from the configuration.
			if ( m_addedRemovedItems.end() != itr )
			{
				// remove the item from our list to indicate that nothing has changed since 
				// this dialog is open
				m_addedRemovedItems.erase ( itr );
                std::map<unsigned long, std::string>::iterator removeitr;
                removeitr = m_ToRemoveCollection.find(itemEntityKey);
                if (removeitr != m_ToRemoveCollection.end())
                {
                    m_ToRemoveCollection.erase( removeitr);
                }
			}
			// if the item is not already in our list, this means the user has just removed a new item
			// from the configuration.
			else
			{
				// add the item to the list to indicate that this item has been removed
				m_addedRemovedItems.insert( itemEntityKey);
                std::map<unsigned long, std::string>::iterator removeitr;
                removeitr = m_ToRemoveCollection.find(itemEntityKey);
                if (removeitr == m_ToRemoveCollection.end())
                {
                    EquipmentStatusListItemMap::const_iterator eslIter = m_reportItems.find( itemEntityKey);
                    if ( m_reportItems.end() != eslIter)
                    {
                        m_ToRemoveCollection.insert(std::pair<unsigned long, std::string>(itemEntityKey, (eslIter->second)->getReportItemKey()));
                    }
                }
			}

            //  Remove from the datanode/point list so the proxy will receive no more updates
            //  (otherwise the item may be re-inserted into the listctrl when it updates).
            m_reportItems.erase( itemEntityKey);
		}

		for (std::vector<int>::reverse_iterator index = indices.rbegin(); index != indices.rend(); index++)
		{
			// remove the item from the list
			m_equipmentListCtrl.DeleteItem(*index);
		}

		// disable the remove button
		m_RemoveBtn.EnableWindow(FALSE);
	}


	void CConfigurationDialog::OnClickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult)
	{
		POSITION pos = m_equipmentListCtrl.GetFirstSelectedItemPosition();

		if (pos == NULL)
			m_RemoveBtn.EnableWindow(FALSE);
		else
			m_RemoveBtn.EnableWindow(TRUE);

		*pResult = 0;
	}


	void CConfigurationDialog::OnAddDatanodesClick()
	{
        CWaitCursor waitCursor;
		//	disable the remove button
		m_RemoveBtn.EnableWindow(FALSE);

		// get info on this node we just clicked
        TA_Base_Bus::CEntityTreeItem* TreeInfo =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(m_equipmentTree.getSelectedItem());

		if (TreeInfo == NULL)
			return;

		// only proceed if the select item is a DataNode
        if ( TreeInfo->getEntityData()->getType() != DataNodeEntityData::getStaticType() )
		{
            return;
        }

		HTREEITEM hItem = m_equipmentTree.GetSelectedItem();

		// Expand all nodes under the selected node to be able to add equipment later
		// as nodes are expanded only dynamically as needed

        m_equipmentTree.ExpandNodeAllLevels( *TreeInfo, hItem);

		m_equipmentTree.RedrawWindow();
		m_equipmentTree.SelectItem(hItem);

		// show message in the status bar so the user knows whats happening
		SetStatusBarText("Adding Equipment to list...");
		m_equipmentListCtrl.SetRedraw(FALSE);

		InspectTreeNodeForDataNodes(hItem);

		m_equipmentListCtrl.SetRedraw();
		SetStatusBarText("Ready");

		if (m_equipmentListCtrl.GetItemCount() > 0)
			m_equipmentListCtrl.EnsureVisible((m_equipmentListCtrl.GetItemCount()-1), FALSE);
	}


	//	Function adds datapoint(s) to the equipment list of the selected node
	//	depending on whether add datapoint or add datanode buttons were clicked.
	//	It uses recursion 
	void CConfigurationDialog::OnAddDatapointsClick()
	{
        CWaitCursor waitCursor;
		//	disable remove button
		m_RemoveBtn.EnableWindow(FALSE);

		//	get the currently selected item's data
        TA_Base_Bus::CEntityTreeItem* TreeInfo =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(m_equipmentTree.getSelectedItem());

		if (TreeInfo == NULL)
			return;

		//	If it is datanode we selected and we clicked on datapoint button 
		//	then must add all datapoints underneath the node recursively
        if ( TreeInfo->getEntityData()->getType() == DataNodeEntityData::getStaticType() )
		{
			// show message in the status bar so the user knows whats happening
			SetStatusBarText("Adding Data Points to list ...");
			m_equipmentListCtrl.SetRedraw(FALSE);

			InspectTreeNodeForDataPoints(m_equipmentTree.GetSelectedItem());

			m_equipmentListCtrl.SetRedraw();
			SetStatusBarText("Ready");

		} else
        if ( TreeInfo->getEntityData()->getType() == DataPointEntityData::getStaticType() )
		{
			// no need for status as this is VERY quick
			InspectTreeNodeForDataPoints(m_equipmentTree.GetSelectedItem());
		}

		if (m_equipmentListCtrl.GetItemCount() > 0)
			m_equipmentListCtrl.EnsureVisible((m_equipmentListCtrl.GetItemCount()-1), FALSE);
	}


	void CConfigurationDialog::OnSetfocusEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult)
	{
		m_RemoveBtn.EnableWindow(FALSE);	

		AdjustGUI();

		*pResult = 0;
	}


	void CConfigurationDialog::OnClickEquipmentTreeClick(NMHDR* pNMHDR, LRESULT* pResult)
	{
		// disable the remove button as we have lost focus from the list
		m_RemoveBtn.EnableWindow(FALSE);

		*pResult = 0;
	}


	void CConfigurationDialog::OnSetfocusEquipmentList(NMHDR* pNMHDR, LRESULT* pResult)
	{
		m_AddDataNodeBtn.EnableWindow(FALSE);
		m_AddDataPtsBtn.EnableWindow(FALSE);

		POSITION pos = m_equipmentListCtrl.GetFirstSelectedItemPosition();

		if (pos == NULL)
			m_RemoveBtn.EnableWindow(FALSE);
		else
			m_RemoveBtn.EnableWindow(TRUE);

		*pResult = 0;
	}


    void CConfigurationDialog::AdjustGUI()
	{
		// if no location in equipment tree then disable Add buttons
		// this is a fail-safe approach in case TREE doesnt get populated properly
		if (m_equipmentTree.GetCount() == 0)
		{
			m_AddDataNodeBtn.EnableWindow(FALSE);
			m_AddDataPtsBtn.EnableWindow(FALSE);
			return;
		}

		// get tree item data
        TA_Base_Bus::CEntityTreeItem* treeItem =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(m_equipmentTree.GetItemData( m_equipmentTree.GetSelectedItem()));

		if (treeItem == NULL)
			return;

        if ( treeItem->getEntityData()->getType() == DataPointEntityData::getStaticType())
		{
			m_AddDataNodeBtn.EnableWindow(FALSE);
			m_AddDataPtsBtn.EnableWindow(TRUE);
			m_AddDataPtsBtn.SetWindowText("Add DataPoint >>");
		}
		else
        if ( treeItem->getEntityData()->getType() == DataNodeEntityData::getStaticType())
		{
			m_AddDataNodeBtn.EnableWindow(TRUE);

			// if the DataNode has at least one DataPoint
			if ( ItemHasAtLeastOneChildDataPoint ( m_equipmentTree.GetSelectedItem() ) )
			{
				m_AddDataPtsBtn.EnableWindow(TRUE);
				m_AddDataPtsBtn.SetWindowText("Add DataPoints >>");
			}
			else
			{
				m_AddDataPtsBtn.EnableWindow(FALSE);
			}
		}
	}


	bool CConfigurationDialog::ItemHasAtLeastOneChildDataPoint(HTREEITEM item)
	{
		bool result = false;

		if ( m_equipmentTree.ItemHasChildren ( item ) )
		{
			// get a child item
			HTREEITEM aNextSibling = m_equipmentTree.GetChildItem ( item );

			while ( aNextSibling )
			{
                TA_Base_Bus::CEntityTreeItem* treeItem =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(m_equipmentTree.GetItemData(aNextSibling));

		        if (NULL != treeItem )
				{
                    if ( treeItem->getEntityData()->getType() == DataPointEntityData::getStaticType())
					{
						result = true;
						break;
					}
					// else keep searching
				}
				// else keep searching

				// get the next child item
				aNextSibling = m_equipmentTree.GetNextItem ( aNextSibling, TVGN_NEXT );
			}
		}

		return result;
	}


	void CConfigurationDialog::OnColumnclickEquipmentList(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NM_LISTVIEW* pNMListView = (NM_LISTVIEW*)pNMHDR;

		int nColumnClickedOn = pNMListView->iSubItem;

		m_equipmentListCtrl.SetSortOrder(nColumnClickedOn, !m_equipmentListCtrl.GetSortOrder(nColumnClickedOn));
		m_equipmentListCtrl.SortColumn(nColumnClickedOn);

		*pResult = 0;
	}


	void CConfigurationDialog::InspectTreeNodeForDataPoints(HTREEITEM aNode)
    {
        if (m_equipmentTree.ItemHasChildren(aNode))
        {
            HTREEITEM aNextSibling = m_equipmentTree.GetChildItem(aNode);
            while (aNextSibling)
            {
                InspectTreeNodeForDataPoints(aNextSibling);
                aNextSibling = m_equipmentTree.GetNextItem(aNextSibling, TVGN_NEXT);
            }

            return;
        }
        TA_Base_Bus::CEntityTreeItem* treeItem =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(m_equipmentTree.GetItemData(aNode));

		if (treeItem == NULL)
			return;

        if ( treeItem->getEntityData()->getType() != DataPointEntityData::getStaticType())
        {
            return;
        }

        unsigned long dpEntityKey = treeItem->getEntityData()->getKey();

        // check and add ONLY if not already in the list
        if (IsItemInList(dpEntityKey, DATAPOINT))
            return;

        m_unprocessedEntities.push_back( EquipmentReportEntity( DATAPOINT, dpEntityKey ));

        //  Start the timer so the new item will be added using the same code as the initial populate
        StartStopPopulatorTimer( true, true);

        // find if the item in our list
        std::set < unsigned long >::iterator itr = m_addedRemovedItems.find ( dpEntityKey );

        // if the item is already in our list, this means the item has been removed previously 
        // and the user now wants to add the item back into the configuration.
        if ( m_addedRemovedItems.end() != itr )
        {
            // remove the item from our list to indicate that nothing has changed since 
            // this dialog is open
            m_addedRemovedItems.erase ( itr );
            std::map<unsigned long, std::string>::iterator removeitr;
            removeitr = m_ToRemoveCollection.find(*itr);
            if (removeitr != m_ToRemoveCollection.end())
            {
                m_ToRemoveCollection.erase( removeitr);
            }
        }
        // if the item is not already in our list, this means the user has added a new item
        // into the configuration.
        else
        {
            // add the item to the list to indicate that a new item has been added
            m_addedRemovedItems.insert( dpEntityKey );
        }
    }


	void CConfigurationDialog::InspectTreeNodeForDataNodes(HTREEITEM aNode)
    {
        TA_Base_Bus::CEntityTreeItem* treeItem =  reinterpret_cast<TA_Base_Bus::CEntityTreeItem*>(m_equipmentTree.GetItemData(aNode));

		if (treeItem == NULL)
			return;

		// only proceed if the selected item is a DataNode or has children
        if ( !( (treeItem->getEntityData()->getType() == DataNodeEntityData::getStaticType())
            || (treeItem->hasChildren() )) )
        {
            return;
        }

        HTREEITEM aNextSibling = m_equipmentTree.GetChildItem(aNode);
        while (aNextSibling)
        {
            InspectTreeNodeForDataNodes(aNextSibling);
            aNextSibling = m_equipmentTree.GetNextItem(aNextSibling, TVGN_NEXT);
        }

        if ( treeItem->getEntityData()->getType()!=TA_Base_Core::DataNodeEntityData::getStaticType())
        {
            return;
        }

        unsigned long dnEntityKey = treeItem->getEntityData()->getKey();

        // check and add ONLY if not already in the list
        if (IsItemInList( dnEntityKey, DATANODE))
            return;

        //TD16889 Mintao 
        //need to check whether it is an equipment node
        if(EntityAccessFactory::getInstance().isEquipmentDataNode(dnEntityKey))
        {
            m_unprocessedEntities.push_back( EquipmentReportEntity( DATANODE, dnEntityKey ));
        }

        //  Start the timer so the new item will be added using the same code as the initial populate
        StartStopPopulatorTimer( true, true);

        // find if the item in our list
        std::set < unsigned long >::iterator itr = m_addedRemovedItems.find ( dnEntityKey );

        // if the item is already in our list, this means the item has been removed previously 
        // and the user now wants to add the item back into the configuration.
        if ( m_addedRemovedItems.end() != itr )
        {
            // remove the item from our list to indicate that nothing has changed since 
            // this dialog is open
            m_addedRemovedItems.erase ( itr );
            std::map<unsigned long, std::string>::iterator removeitr;
            removeitr = m_ToRemoveCollection.find(*itr);
            if (removeitr != m_ToRemoveCollection.end())
            {
                m_ToRemoveCollection.erase( removeitr);
            }
        }
        // if the item is not already in our list, this means the user has added a new item
        // into the configuration.
        else
        {
            // add the item to the list to indicate that a new item has been added
            m_addedRemovedItems.insert( dnEntityKey );
        }
    }


	void CConfigurationDialog::SetStatusBarText(const CString &message)
	{
		// show message in the status bar so the user knows whats happening
		// this function is a friend of the parent
        CEquipmentStatusViewerDlg * pDlg = reinterpret_cast<CEquipmentStatusViewerDlg *>(GetParent());
        if (NULL != pDlg)
        {
            pDlg->SetStatusBarText(0, message);
        }
	}


    void CConfigurationDialog::OnOK()
    {
        CWaitCursor waitCursor;
        // if datanode(s)/point(s) are added or removed
        if ( m_addedRemovedItems.empty() )
        {
            CDialog::OnOK();
            return;
        }

        CString msg;
        msg.Format("Saving %s ", m_ReportName.c_str());
        CProgressBar progressBar(msg, 30);
        progressBar.SetBarColour(PROGRESS_BAR_COLOR);

        using TA_Base_Core::IConfigEquipReportData;
        using TA_Base_Core::IEquipReportData;

        // REMOVE RECORDS WHICH WERE REMOVED FROM THE LIST
        if (m_ToRemoveCollection.empty() == FALSE)
        {
            CString msgComposed;
            msgComposed.Format("%s (removing old)", msg);
            progressBar.SetMessage(msgComposed);

            progressBar.SetRange(0, m_ToRemoveCollection.size(), 1);

            // only do this if there is something in the remove collection
            for ( std::map<unsigned long, std::string>::iterator counter = m_ToRemoveCollection.begin();
                    counter != m_ToRemoveCollection.end();
                    counter++)
            {
                IConfigEquipReportData *reportData = NULL;

                try
                {
                    reportData = dynamic_cast<IConfigEquipReportData *>(TA_Base_Core::EquipReportDataAccessFactory::getInstance().getEquipReportDataByKey( counter->second, true));

                    if (reportData)
                    {
                        reportData->deleteThisEquipReportData();

                        delete reportData;
                        reportData = NULL;
                    }

                    //  Remove from set - anything left in set is an "add"
                    m_addedRemovedItems.erase( counter->first);
                }
                catch (...)
                {
                    // if we are here, something went wrong with the deletion in the database
                    // it is safe to proceed though.i.e. to try again later if say connection comes back online
                }
                progressBar.StepIt();
            }
        }

        if ( m_addedRemovedItems.empty() )
        {
            CDialog::OnOK();
            return;
        }

        // ADD RECORDS WHICH HAVE BEEN ADDED IN THIS SESSION
        int ItemTotal = m_equipmentListCtrl.GetItemCount();

        if (ItemTotal > 0)
        {
            CString msgComposed;
            msgComposed.Format("%s (saving new)", msg);
            progressBar.SetMessage(msgComposed);
            progressBar.SetPos(0);
            progressBar.SetRange(0, ItemTotal, 1);
        }

        bool errorOccurred = false;

        EntityKeySet::iterator itr = m_addedRemovedItems.begin();
        for ( ; itr != m_addedRemovedItems.end(); itr++)
        {
            EquipmentStatusListItemMap::const_iterator eslIter = m_reportItems.find( *itr);
            if (m_reportItems.end() == eslIter)
            {
                //  The proxy has already been removed from the local map (probably has 
                //  been updated and found it has child datanodes: its a system or subsystem)
                continue;
            }
            IConfigEquipReportData *reportData = NULL;

            try
            {
                reportData = dynamic_cast<IConfigEquipReportData*>(TA_Base_Core::EquipReportDataAccessFactory::getInstance().createEquipReportData());

                if (reportData)
                {
                    reportData->setReportKey(m_ReportId);
                    if ( eslIter->second->getItemType() == DATANODE)
                        reportData->setDataNodeKey( *itr);
                    else
                        reportData->setDataPointKey(*itr);

                    reportData->applyChanges();

                    delete reportData;
                    reportData = NULL;
                }
            }
            catch (...)
            {
                // something went wrong
                errorOccurred = true;
                continue;
            }
        }

        if (errorOccurred)
        {
            TA_Base_Bus::TransActiveMessage userMsg;
            userMsg << m_ReportName.c_str();
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_083002);
        }

        // write audit log message
        CAuditMessageUtility auditMsgWriter;
        auditMsgWriter.LogAuditMessage(m_ReportName, m_LocationName, ESVAudit::ModifyReportLog);

        SetStatusBarText("Ready");

        CDialog::OnOK();
    }

	void CConfigurationDialog::OnCancel()
	{
		// if datanode(s)/point(s) are added or removed
		if ( false == m_addedRemovedItems.empty() )
		{
			// warn the user
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UW_083006);

			if (selectedButton == IDNO)
				return;
		}

		CDialog::OnCancel();
	}


	bool CConfigurationDialog::IsItemInList(unsigned long entityKey, Node_Item entityType)
	{
        return (0 <= EquipmentStatusListItem::FindInList( m_equipmentListCtrl, entityKey) );
	}


	void CConfigurationDialog::OnClose()
	{
		OnCancel();
	}


	void CConfigurationDialog::OnItemExpandingEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult)
	{
        *pResult = 0;
        return;

		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

		if (pNMTreeView->itemNew.hItem && (pNMTreeView->action == TVE_EXPAND) &&
			((pNMTreeView->itemNew.state & TVIS_EXPANDEDONCE) != TVIS_EXPANDEDONCE))
		{
			// if action is to expand and it has NOT been expanded before
			Expand(pNMHDR);

			// select that item programatically
			m_equipmentTree.SelectItem(pNMTreeView->itemNew.hItem);
		}

		*pResult = 0;
	}


	void CConfigurationDialog::Expand(NMHDR *pNMHDR)
	{
		// set the description of the currently selected tree item
		m_equipmentDescription.SetWindowText(m_equipmentTree.GetItemDescription().c_str());

		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

		if (pNMTreeView->itemNew.hItem )
		{
            m_equipmentTree.Expand( pNMTreeView->itemNew.hItem, TVE_EXPAND);
		}

		//adjust GUI controls
		AdjustGUI();
	}


	void CConfigurationDialog::OnSelchangedEquipmentTree(NMHDR* pNMHDR, LRESULT* pResult)
	{
		NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*)pNMHDR;

		m_equipmentDescription.SetWindowText(m_equipmentTree.GetItemDescription().c_str());

		// expand this node
		Expand(pNMHDR);

		AdjustGUI();
		*pResult = 0;
	}


    void CConfigurationDialog::OnShowWindow(BOOL bShow, UINT nStatus)
	{
		CDialog::OnShowWindow(bShow, nStatus);

		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/08/26 11:30:00

        PopulateEquipmentStatusList(m_ReportId);

		//Launch Manager///////////////////////////////////////////////////////////

        try
        {
		    m_equipmentTree.Expand(m_equipmentTree.GetRootItem(), TVE_EXPAND);
        }
        catch(...)
        {
            TA_ASSERT( false, "Config dialog failed");
        }

	}


    bool CConfigurationDialog::StartStopPopulatorTimer( bool start, bool postInitialTimerMsg)
    {
        bool result = false;
        bool timerRunning = (m_populatorTimer > 0);
        if ( timerRunning == start)
        {
            //  Don't start if already running, don't stop if not running.
            return false;
        }

        if (start)
        {  
			/////////////////////////////////////////////////////////////////////
			//Launch Manager modified by: zhangjunsheng 2008/08/25 13:00:00

//          const unsigned int REFRESH_DELAY_MSEC = 1000; // 1 second between timer ticks
            const unsigned int REFRESH_DELAY_MSEC = 1;    

			//Launch Manager///////////////////////////////////////////////////////////

            m_populatorTimer = SetTimer( 1, REFRESH_DELAY_MSEC, NULL);
            result = (m_populatorTimer > 0);
            if ( (false != result) && postInitialTimerMsg)
            {
                PostMessage( WM_TIMER, static_cast<WPARAM>(m_populatorTimer), 0);
            }
        }
        else
        {
            result = ( KillTimer( m_populatorTimer) != 0);
            if ( false != result)
            {
                m_populatorTimer = 0;
            }
        }
        return result;
    }


    void CConfigurationDialog::PopulateEquipmentStatusList(std::string& reportKey)
    {
        CWaitCursor waitCursor;

        if ((ACTIVE_PROFILE_ALL_REPORT == reportKey) || ( ACTIVE_PROFILE_MMS_REPORT == reportKey ))
        {
            //  These "ALL" reports are not configurable
            TA_ASSERT( false, "Attempt to modify the non-configurable reports.");
            return;
        }

        // Empty the vector of entity keys; these keys will be used to fill the reportItems map
        m_unprocessedEntities.clear();

        // Empty the map of report items (these encapsulate the datanode/point proxies)
        m_reportItems.clear();

        m_progressBar.SetMessage( "Reading database...");
        m_progressBar.SetRange( 0, 20, 1);
        m_progressBar.SetPos( 0);
        CString statusMessage;

        statusMessage = "Loading report...";
        try
        {
            statusMessage.Format( "Loading report %s", TA_Base_Core::EquipReportAccessFactory::getInstance().getEquipReportByKey(reportKey)->getName().c_str());
            m_progressBar.SetMessage( statusMessage );
        }
        catch (...)
        {
            //  Not worth stopping just because of the status bar text (it will probably get caught below anyway)
        }

        //  Get the Datanode/Datapoint entity keys to be watched through this Equipment Report
        std::vector<IEquipReportData *> equipReportRows;

        try
        {
            equipReportRows = TA_Base_Core::EquipReportDataAccessFactory::getInstance().getAllEquipReportDatumByReportKey(reportKey);
        }
        catch (...)
        {
            m_progressBar.Clear();
            TA_Base_Bus::TransActiveMessage userMsg;
            UINT selectedButton = userMsg.showMsgBox(IDS_UE_083004);
            return;
        }

        m_progressBar.StepIt();

        //  Now fill the entity key vector 

        for (std::vector<IEquipReportData *>::const_iterator it = equipReportRows.begin(); it != equipReportRows.end(); it++)
        {
            if (0 < (*it)->getDataNodeKey())
            {
                m_unprocessedEntities.push_back( EquipmentReportEntity( DATANODE, (*it)->getDataNodeKey(), (*it)->getId() ));
            }
            else
            {
                m_unprocessedEntities.push_back( EquipmentReportEntity( DATAPOINT, (*it)->getDataPointKey(), (*it)->getId()));
            }
        }

        m_progressBar.StepIt();
        m_progressBar.SetRange( 0, m_unprocessedEntities.size());

        //  Start the timer that will fill the list ctrl one page at a time (see OnTimer)
        //  and send an initial timer message to fill the first page (rather than wait 1 second)
        StartStopPopulatorTimer( true, true);
    }


    void CConfigurationDialog::OnTimer(UINT nIDEvent)
    {
        if ( nIDEvent != m_populatorTimer)
        {
    	    CDialog::OnTimer(nIDEvent);
            return;
        }

        m_equipmentListCtrl.SetRedraw( false);

        //  Populate the list control several lines at a time, using the vector of "unprocessed" entity keys
   
		/////////////////////////////////////////////////////////////////////
		//Launch Manager modified by: zhangjunsheng 2008/08/25 13:00:00

//      int linesRemaining = 3;
        int linesRemaining = 5;

		//Launch Manager///////////////////////////////////////////////////////////

        std::vector< EquipmentReportEntity>::iterator it = m_unprocessedEntities.begin();

        for ( ; linesRemaining > 0; --linesRemaining)
        {
            if (m_unprocessedEntities.end() == it)
            {
                StartStopPopulatorTimer( false);
                m_progressBar.Clear();
                break;
            }
            try
            {
                EquipmentStatusListItemSharedPtr pItem;

                if ( DATANODE == it->itemType)
                {
                    pItem.reset( new DataNodeListItem( it->entityKey, GetSafeHwnd() ));
                }
                else if (DATAPOINT == it->itemType)
                {
                    pItem.reset( new DataPointListItem( it->entityKey, GetSafeHwnd() ));
                }
                else
                {
                    TA_ASSERT( false, "Invalid item node type (not Datanode nor Datapoint).");
                }

                pItem->setReportItemKey( it->reportItemKey);
                m_reportItems[ it->entityKey] = pItem;

                //  Build the initial (mostly default) values for this item
                pItem->processEntityUpdateEvent( pItem->getEntityKey(), TA_Base_Bus::ConfigStateUpdate);

                //  Poke the partially complete item into the list control - it will be refreshed as Entity Updates come in.
                pItem->UpdateInList( m_equipmentListCtrl, -1, ASSET_COLUMN);
                pItem->UpdateInList( m_equipmentListCtrl, -1, DESCRIPTION_COLUMN);
                pItem->UpdateInList( m_equipmentListCtrl, -1, DATAPOINT_COLUMN);

                //  The entity has been "processed" into the reportItems vector, so remove it
                it = m_unprocessedEntities.erase(it);
                //  The iterator now points to the next entitykey in the sequence, or .end()

                m_progressBar.SetPos( m_reportItems.size());
            }
            catch( ScadaProxyException& )
            {
                //  Probably something isn't ready yet
                //  Remove it from the list here and put it at the end to have another go later.
                //  Abandon this timer tick to keep the UI running.
                m_unprocessedEntities.push_back( *it);
                m_unprocessedEntities.erase(it);	// legal erase as break here
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugDebug,
                            "catch at the CConfigurationDialog::OnTimer. unprocessEntities[%d]",m_unprocessedEntities.size() );                                
                break;
            }
        }

        m_equipmentListCtrl.SetRedraw( true);
        m_equipmentListCtrl.Invalidate( false);

	    CDialog::OnTimer(nIDEvent);
    }


    LRESULT CConfigurationDialog::OnListItemUpdate(WPARAM wParam, LPARAM lParam)
	{
        //  Handle the EntityUpdateEvent here in the UI thread
        unsigned long entityKey( wParam);
        EquipmentStatusListItemMap::iterator it = m_reportItems.find( entityKey);
        LRESULT result(0);
        if ( m_reportItems.end() != it )
        {

            result = it->second->UpdateInList( m_equipmentListCtrl, -1, ASSET_COLUMN);

            if ( it->second->isToBeDeleted() )
            {
                //  Probably a DataNode with child datanodes (ie. Location, system or subsystem)
                //  Take a copy of the shared pointer to keep its object alive until the end of this block.
                EquipmentStatusListItemSharedPtr eslPtr = it->second;
                TA_Base_Core::ThreadGuard guard( eslPtr->m_lock );
                m_reportItems.erase( it);
            }
            else
            {
                //  Fill the other columns if this item is still being displayed
                it->second->UpdateInList( m_equipmentListCtrl, -1, DESCRIPTION_COLUMN);
                it->second->UpdateInList( m_equipmentListCtrl, -1, DATAPOINT_COLUMN);
            }
        }
        return result;
	}


} // TA_Base_App
