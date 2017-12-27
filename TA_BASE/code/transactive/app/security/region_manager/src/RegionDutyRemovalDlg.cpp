/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionDutyRemovalDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog enables the operator to select from a list of removable regions.
  * A warning is displayed if the operator decides to effect a duty removal that 
  * leads to uncontrolled subsystems.
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
    const CString REGION_COLUMN_HEADING("Region");
    const int REGION_COLUMN_WIDTH(420);
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
using TA_Base_Bus::SubsystemTreeMap;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// RegionDutyRemovalDlg dialog


RegionDutyRemovalDlg::RegionDutyRemovalDlg(const SessionId& sourceSessionId, CWnd* pParent /*=NULL*/) :
	CDialog(RegionDutyRemovalDlg::IDD, pParent),
    m_sourceSessionId(sourceSessionId),
    m_dataCache(DutyDataStore::getDataCache()),
    m_sessionCache(DutyDataStore::getSessionCache())
{
	//{{AFX_DATA_INIT(RegionDutyRemovalDlg)
	//}}AFX_DATA_INIT
}


RegionDutyRemovalDlg::~RegionDutyRemovalDlg()
{
    m_removableRegions.clear();
}


void RegionDutyRemovalDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RegionDutyRemovalDlg)
	DDX_Control(pDX, IDC_UNASSIGN_BTN, m_unassignBtn);
	DDX_Control(pDX, IDC_REMOVERGN_LIST, m_removableRegionList);
	DDX_Control(pDX, IDC_SOURCEOP_TEXT, m_sourceOperator);
	//}}AFX_DATA_MAP
}


/////////////////////////////////////////////////////////////////////////////
// RegionDutyRemovalDlg message handlers

BEGIN_MESSAGE_MAP(RegionDutyRemovalDlg, CDialog)
	//{{AFX_MSG_MAP(RegionDutyRemovalDlg)
	ON_NOTIFY(LVN_ITEMCHANGED, IDC_REMOVERGN_LIST, OnRegionListItemChanged)
	ON_BN_CLICKED(IDC_UNASSIGN_BTN, OnUnassignBtn)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL RegionDutyRemovalDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
    try
    {
	    m_sourceOperator.SetWindowText(GridAssistant::generateOperatorNameAndLoginString(m_sourceSessionId).c_str());
        m_removableRegions = m_dutyAgent.getRemovableRegions(m_sourceSessionId);

        m_removableRegionList.SetExtendedStyle(m_removableRegionList.GetExtendedStyle() | LVS_EX_FULLROWSELECT | LVS_EX_INFOTIP);
        m_removableRegionList.InsertColumn(REGION_COLUMN, _T(REGION_COLUMN_HEADING), LVCFMT_LEFT, REGION_COLUMN_WIDTH);
    
        // Insert into the list control the removable regions retrieved from the Duty Agent.
        for (SubsystemTreeMap::const_iterator regionIt = m_removableRegions.begin(); regionIt != m_removableRegions.end(); regionIt++)
        {
            try
            {
                std::string regionName(m_dataCache.getRegionName(regionIt->first));

                int item(m_removableRegionList.InsertItem(m_removableRegionList.GetItemCount(), _T(regionName.c_str())));
                m_removableRegionList.SetItemData(item, regionIt->first);
            }
            catch (...)
            { 
                LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                            "Ignoring invalid removable region [r: %lu]", regionIt->first);
            }
        }

        // Disable the list if there are no regions that can be transfered to the target operator.
        if (m_removableRegionList.GetItemCount() == 0)
        {
            m_removableRegionList.EnableWindow(FALSE);
            m_removableRegionList.InsertItem(m_removableRegionList.GetItemCount(), _T("No removable regions."));
        }

        // Disable the Assign button until at least one region has been selected.
        m_unassignBtn.EnableWindow(FALSE);
    }
    catch (...)
    {
        EndDialog(IDABORT);
        throw;
    }

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RegionDutyRemovalDlg::OnRegionListItemChanged(NMHDR* pNMHDR, LRESULT* pResult) 
{
	NM_LISTVIEW* pNMListView = reinterpret_cast<NM_LISTVIEW*>(pNMHDR);

	if ((pNMListView->uNewState & LVIS_SELECTED) == LVIS_SELECTED ||
        (pNMListView->uOldState & LVIS_SELECTED) == LVIS_SELECTED)
    {
        m_unassignBtn.EnableWindow(m_removableRegionList.GetSelectedCount() > 0);
    }
	
	*pResult = 0;
}


void RegionDutyRemovalDlg::OnUnassignBtn() 
{
	TA_ASSERT(m_removableRegionList.GetSelectedCount() > 0, "No regions selected for transfer");

    RegionKeyList selectedRegionKeys;
    SubsystemTreeMap uncontrolledSubsystems;

    // Get the region keys corresponding to the selected list items.
    // There may be uncontrolled subsystems associated with these keys.
    int item(LV_ERR);
    while ((item = m_removableRegionList.GetNextItem(item, LVNI_SELECTED)) != LV_ERR)
    {
        RegionKey regionKey(m_removableRegionList.GetItemData(item));

        selectedRegionKeys.push_back(regionKey);
       
        SubsystemTreeMap::const_iterator rgnTreeIt(m_removableRegions.find(regionKey));
        TA_ASSERT(rgnTreeIt != m_removableRegions.end(), "Removable region not in map");

        if (!rgnTreeIt->second.empty())
        {
            uncontrolledSubsystems.insert(std::make_pair(rgnTreeIt->first, rgnTreeIt->second));
        }
    }

    // Let the operator know if unassigning the selected regions will lead to uncontrolled subsystems. 
    // If the operator decides not to proceed, then do nothing.
    if (!uncontrolledSubsystems.empty())
    {
        RegionDutyChangeConfirmDlg confirmDlg(uncontrolledSubsystems, RegionDutyChangeConfirmDlg::IDD_REMOVE, this);
       
        if (confirmDlg.DoModal() == IDNO)
        {
            return;
        }
    }

    try
    {
        CWaitCursor waitCursor;

        // Effect region duty removal. If this succeeds close the dialog and return to the main
        // Region Manager window.
        m_dutyAgent.removeRegionDuty(m_sessionCache.getCurrentSessionId(), m_sourceSessionId, selectedRegionKeys);

	    EndDialog(IDOK);
    }
    catch (const TA_Base_Bus::DutyMonitorFrameworkException& ex)
    {
        MessageBox::error("Error while removing region duty from source operator.", ex);
    }
}
