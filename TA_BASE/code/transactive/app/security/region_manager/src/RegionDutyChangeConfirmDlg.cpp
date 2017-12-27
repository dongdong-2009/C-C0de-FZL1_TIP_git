/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionDutyChangeConfirmDlg.cpp $
  * @author:  Bart Golab
  * @version: $Revision: #1 $
  *
  * Last modification: $DateTime: 2012/06/12 13:35:44 $
  * Last modified by:  $Author: builder $
  *
  * This dialog is shown to the operator before effecting region duty transfer/removal
  * that leads to uncontrolled subsystems. The uncontrolled subsystems are displayed so
  * that the operator can decide whether or not to proceed with the duty change.
  *
  */

#include "StdAfx.h"
#include "app/security/region_manager/src/RegionManager.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

using TA_Base_Bus::DataCache;
using TA_Base_Bus::RegionKey;
using TA_Base_Bus::SubsystemKeyList;
using TA_Base_Bus::SubsystemTreeMap;

using namespace TA_Base_App;
using namespace TA_Base_Bus;

/////////////////////////////////////////////////////////////////////////////
// RegionDutyChangeConfirmDlg dialog

RegionDutyChangeConfirmDlg::RegionDutyChangeConfirmDlg(const TA_Base_Bus::SubsystemTreeMap& uncontrolledSubsystems, UINT nIDTemplate, CWnd* pParent) :
	CDialog(nIDTemplate, pParent),
    m_uncontrolledSubsystems(uncontrolledSubsystems)
{
	//{{AFX_DATA_INIT(RegionDutyChangeConfirmDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


RegionDutyChangeConfirmDlg::~RegionDutyChangeConfirmDlg()
{
    m_uncontrolledSubsystems.clear();
}


void RegionDutyChangeConfirmDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(RegionDutyChangeConfirmDlg)
	DDX_Control(pDX, IDC_UNCONTROLLED_SUBSYSTEM_TREE, m_subsystemTree);
	//}}AFX_DATA_MAP
}




/////////////////////////////////////////////////////////////////////////////
// RegionDutyChangeConfirmDlg message handlers

BEGIN_MESSAGE_MAP(RegionDutyChangeConfirmDlg, CDialog)
	//{{AFX_MSG_MAP(RegionDutyChangeConfirmDlg)
	ON_BN_CLICKED(IDYES, OnYes)
	ON_BN_CLICKED(IDNO, OnNo)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


BOOL RegionDutyChangeConfirmDlg::OnInitDialog() 
{
    TA_ASSERT(!m_uncontrolledSubsystems.empty(), "Empty uncontrolled subsystem tree map.");

	CDialog::OnInitDialog();

    DataCache& dataCache = DutyDataStore::getDataCache();
    HTREEITEM rootItem(m_subsystemTree.InsertItem(_T("Uncontrolled subsystems")));

    // Populate the uncontrolled subsystems tree control with regions and their subsystems.
    for (SubsystemTreeMap::const_iterator subTreeIt = m_uncontrolledSubsystems.begin(); subTreeIt != m_uncontrolledSubsystems.end(); subTreeIt++)
    {
        const RegionKey& regionKey = subTreeIt->first;
        const SubsystemKeyList& subsystemKeys = subTreeIt->second;

        TA_ASSERT(!subsystemKeys.empty(), "Empty uncontrolled subsystem list for region.");

        try
        {
            std::string regionName(dataCache.getRegionName(regionKey));
            HTREEITEM regionItem(m_subsystemTree.InsertItem(_T(regionName.c_str()), rootItem));

            for (SubsystemKeyList::const_iterator subKeyIt = subsystemKeys.begin(); subKeyIt != subsystemKeys.end(); subKeyIt++)
            {
                try
                {
                    std::string subsystemName(dataCache.getSubsystemName(*subKeyIt));
                    m_subsystemTree.InsertItem(_T(subsystemName.c_str()), regionItem);            
                }
                catch (...)
                {
                    LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, 
                                "Ignoring invalid uncontrolled subsystem [r: %lu, s: %lu]", regionKey, *subKeyIt);
                }
            }

            m_subsystemTree.Expand(regionItem, TVE_EXPAND);    
        }
        catch (...)
        {
            LOG_GENERIC(SourceInfo, TA_Base_Core::DebugUtil::DebugWarn, "Ignoring invalid region [r: %lu]", regionKey);
        }
    }

    m_subsystemTree.Expand(rootItem, TVE_EXPAND);

	return TRUE;  // return TRUE unless you set the focus to a control
	              // EXCEPTION: OCX Property Pages should return FALSE
}


void RegionDutyChangeConfirmDlg::OnYes() 
{
	EndDialog(IDYES);
}


void RegionDutyChangeConfirmDlg::OnNo() 
{
	EndDialog(IDNO);
}
