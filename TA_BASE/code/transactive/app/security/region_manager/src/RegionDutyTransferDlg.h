#if !defined(AFX_REGIONDUTYTRANSFERDLG_H__93524408_3C51_4D69_B07F_64632FE6BD4B__INCLUDED_)
#define AFX_REGIONDUTYTRANSFERDLG_H__93524408_3C51_4D69_B07F_64632FE6BD4B__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/**
  * The source code in this file is the property of
  * Ripple Systems and is not for redistribution
  * in any form.
  *
  * Source:   $File: //depot/GZL6_TIP/TA_BASE/transactive/app/security/region_manager/src/RegionDutyTransferDlg.h $
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

namespace TA_Base_App
{
    class RegionDutyTransferDlg : public CDialog
    {
    // Operations
    public:
	    RegionDutyTransferDlg(const TA_Base_Bus::SessionId& targetSessionId, CWnd* pParent = NULL);
        virtual ~RegionDutyTransferDlg();


    // Overrides
	    // ClassWizard generated virtual function overrides
	    //{{AFX_VIRTUAL(RegionDutyTransferDlg)
	protected:
	    virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	    //}}AFX_VIRTUAL


    // Implementation
    protected:
	    // Generated message map functions
	    //{{AFX_MSG(RegionDutyTransferDlg)
	    virtual BOOL OnInitDialog();
	    afx_msg void OnAssignBtn();
	    afx_msg void OnRegionListItemChanged(NMHDR* pNMHDR, LRESULT* pResult);
	    //}}AFX_MSG
	    DECLARE_MESSAGE_MAP()


    // Dialog Data
    private:
	    //{{AFX_DATA(RegionDutyTransferDlg)
	    enum { IDD = IDD_REGION_DUTY_XFER_DLG };
	    CButton	m_assignBtn;
	    CListCtrl	m_transferableRegionList;
	    CStatic	m_targetOperator;
	    //}}AFX_DATA

        TA_Base_Bus::SessionId m_targetSessionId;
        TA_Base_Bus::TransferableRegionList m_transferableRegions;
        TA_Base_Bus::SubsystemTreeMap m_uncontrolledSubsystems;

        TA_Base_Bus::DutyAgentAccess m_dutyAgent;
        TA_Base_Bus::DataCache& m_dataCache;
        TA_Base_Bus::SessionCache& m_sessionCache;
    };

} // namespace TA_Base_App

#endif // !defined(AFX_REGIONDUTYTRANSFERDLG_H__93524408_3C51_4D69_B07F_64632FE6BD4B__INCLUDED_)
