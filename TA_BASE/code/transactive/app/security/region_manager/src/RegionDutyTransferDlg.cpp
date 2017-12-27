/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionDutyTransferDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog enables the operator to select from a list of transferable regions.
  * A warning is displayed if the operator decides to effect a transfer that leads
  * to uncontrolled subsystems.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

namespace
{
    const int REGION_COLUMN(0);
    const int OPERATOR_COLUMN(1);
    const int PROFILE_COLUMN(2);

    const CString REGION_COLUMN_HEADING("Region");
    const CString OPERATOR_COLUMN_HEADING("Operator");
    const CString PROFILE_COLUMN_HEADING("Profile");

    const int REGION_COLUMN_WIDTH(120);
    const int OPERATOR_COLUMN_WIDTH(180);
    const int PROFILE_COLUMN_WIDTH(120);
}

using TA_Base_Bus::DutyAgentAccess;
using TA_Base_Bus::DataCache;
using TA_Base_Bus::SessionCache;
using TA_Base_Bus::SessionInfoEx;
using TA_Base_Bus::SessionId;
using TA_Base_Bus::ProfileKey;
using TA_Base_Bus::ProfileKeyList;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::RegionKeyList;
using TA_Base_Bus::TransferableRegionList;
using TA_Base_Bus::SubsystemTreeMap;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// RegionDutyTransferDlg dialog


RegionDutyTransferDlg::RegionDutyTransferDlg(const SessionId& targetSessionId, CWnd* pParent /*=NULL*/) :
	CDialog(RegionDutyTransferDlg::IDD, pParent),
    m_targetSessionId(targetSessionId),
    m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache())
{
	//{{AFX_DATA_INIT(RegionDutyTransferDlg)
	//}}AFX_DATA_INIT
}


RegionDutyTransferDlg::~RegionDutyTransferDlg()
{
    m_transferableRegions.clear();
    m_uncontrolledSubsystems.clear();
}


void RegionDutyTransferDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RegionDutyTransferDlg)
	DDX_Control(pDX, IDC_ASSIGN_BTN, m_assignBtn);
	DDX_Control(pDX, IDC_TRANSFERRGN_LIST, m_transferableRegionList);
	DDX_Control(pDX, IDC_TARGETOP_TEXT, m_targetOperator);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// RegionDutyTransferDlg message handlers

BEGIN_MESSAGE_MAP(RegionDutyTransferDlg, CDialog)
	//{{AFX_MSG_MAP(RegionDutyTransferDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_TRANSFERRGN_LIST, OnRegionListItemChanged)
	ON_BN_CLICKED(IDC_ASSIGN_BTN, OnAssignBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL RegionDutyTransferDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    try
    {
	    m_targetOperator.SetWindowText(GridAssistant::generateOperatorNameAndLoginString(m_targetSessionId).c_str());
        m_dutyAgent.getTransferableRegions(m_targetSessionId, m_transferableRegions, m_uncontrolledSubsystems);

        m_transferableRegionList.SetExtendedStyle(m_transferableRegionList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_transferableRegionList.InsertColumn(REGION_COLUMN, _T(REGION_COLUMN_HEADING), LVCFMT_LEFT, REGION_COLUMN_WIDTH);
        m_transferableRegionList.InsertColumn(OPERATOR_COLUMN, _T(OPERATOR_COLUMN_HEADING), LVCFMT_LEFT, OPERATOR_COLUMN_WIDTH);
        m_transferableRegionList.InsertColumn(PROFILE_COLUMN, _T(PROFILE_COLUMN_HEADING), LVCFMT_LEFT, PROFILE_COLUMN_WIDTH);
    
        // Insert into the list control the transferable regions retrieved from the Duty Agent.
        for (TransferableRegionList::const_iterator regionIt = m_transferableRegions.begin(); regionIt != m_transferableRegions.end(); regionIt++)
        {
            try
            {
                std::string regionName(m_dataCache.getRegionName(regionIt->regionKey));
                std::string operatorName(""), profileName("");

                // Unassigned regions have the operator key and profile key set to "invalid". 
                // In such case operator name and profile name should be left blank.
                if (regionIt->operatorKey != TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY)
                {
                    operatorName = m_dataCache.getOperatorDescription(regionIt->operatorKey);
                }

                if (regionIt->profileKey != TA_Base_Bus::DutyAgentTypeCorbaDef::INVALID_KEY)
                {
                    profileName = m_dataCache.getProfileName(regionIt->profileKey);
                }

                int item(m_transferableRegionList.InsertItem(m_transferableRegionList.GetItemCount(), _T("")));

                m_transferableRegionList.SetItemText(item, REGION_COLUMN, _T(regionName.c_str()));
                m_transferableRegionList.SetItemText(item, OPERATOR_COLUMN, _T(operatorName.c_str()));
                m_transferableRegionList.SetItemText(item, PROFILE_COLUMN, _T(profileName.c_str()));
                m_transferableRegionList.SetItemData(item, regionIt->regionKey);
            }
            catch (...)
            { 
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                            "Ignoring invalid transferable region [r: %lu, o: %lu, p: %lu]",
                            regionIt->regionKey, regionIt->operatorKey, regionIt->profileKey);
            }
        }

        // Disable the list if there are no regions that can be transfered to the target operator.
        if (m_transferableRegionList.GetItemCount() == 0)
        {
            m_transferableRegionList.EnableWindow(FALSE);
            
            int item(m_transferableRegionList.InsertItem(m_transferableRegionList.GetItemCount(), _T("")));
            m_transferableRegionList.SetItemText(item, OPERATOR_COLUMN, _T("No transferable regions."));
        }

        // Disable the Assign button until at least one region has been selected.
        m_assignBtn.EnableWindow(FALSE);
    }
    catch (...)
    {
        EndDialog(IDABORT);
        throw;
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RegionDutyTransferDlg::OnRegionListItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED ||
        (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        m_assignBtn.EnableWindow(m_transferableRegionList.GetSelectedCount() > 0);
    }
	
	*pResult = 0;
}


void RegionDutyTransferDlg::OnAssignBtn() 
{
	TA_ASSERT(m_transferableRegionList.GetSelectedCount() > 0, "No regions selected for transfer");

    RegionKeyList selectedRegionKeys;
    SubsystemTreeMap uncontrolledSubsystems;

    // Get the region keys corresponding to the selected list items.
    // There may be uncontrolled subsystems associated with these keys.
    int item(LV_ERR);
    while ((item = m_transferableRegionList.GetNextItem(item, LVNI_SELECTED)) != LV_ERR)
    {
        RegionKey regionKey(m_transferableRegionList.GetItemData(item));

        selectedRegionKeys.push_back(regionKey);
       
        SubsystemTreeMap::const_iterator subTreeIt(m_uncontrolledSubsystems.find(regionKey));

        if (subTreeIt != m_uncontrolledSubsystems.end() && !subTreeIt->second.empty())
        {
            uncontrolledSubsystems.insert(std::make_pair(subTreeIt->first, subTreeIt->second));
        }
    }

    // Let the operator know if transferring the selected regions will lead to uncontrolled subsystems. 
    // If the operator decides not to proceed, then do nothing.
    if (!uncontrolledSubsystems.empty())
    {
        RegionDutyChangeConfirmDlg confirmDlg(uncontrolledSubsystems, RegionDutyChangeConfirmDlg::IDD_XFER, this);
       
        if (confirmDlg.DoModal() == IDNO)
        {
            return;
        }
    }

    try
    {
        CWaitCursor waitCursor;

        // Effect region duty transfer. If this succeeds close the dialog and return to the main
        // Region Manager window.
        m_dutyAgent.transferRegionDuty(m_sessionCache.getCurrentSessionId(), m_targetSessionId, selectedRegionKeys);

	    EndDialog(IDOK);
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        MessageBox::error("Error while assigning region duty to target operator.", ex);
    }	
}
